#include "logger_buffer.h"

namespace jimu {
namespace logger {
    
LoggerBufferFlushPool::LoggerBufferFlushPool() {}

LoggerBufferFlushPool::~LoggerBufferFlushPool() {
    delete thread_pool;
}

uint32_t LoggerBufferFlushPool::init(uint32_t thread_number) {
    thread_pool = new ThreadPool(thread_number);
    
    if (thread_pool == nullptr) {
        return 1;
    }

    return 0;
}

LoggerBufferFlushPool* LoggerBufferFlushPool::instance() {
    static LoggerBufferFlushPool pool;
    return & pool;
}

static void write_g(WriteFilePoint file_p,
        const char * write_buffer, const int &write_size,
        std::counting_semaphore<1> & buffer_sm) {
    file_p->write(write_buffer, write_size);
    buffer_sm.release();
}

uint32_t LoggerBufferFlushPool::write(WriteFilePoint file_p,
        const char * write_buffer, const int &write_size,
        std::counting_semaphore<1> & buffer_sm) {
    if (file_p == nullptr || write_buffer == nullptr) {
        return 1;
    }

    if (write_size == 0) {
        buffer_sm.release();
        return 0;
    }

    thread_pool->enqueue(write_g, file_p, write_buffer, write_size, buffer_sm);

    return 0;
}

void LoggerBuffer::init() {
    file_p = nullptr;
    
    buffer_now_use = 0;

    for (int i = 0; i < 2; i++) {
        buffer[i] = new char[LOGGER_BUFFER_SIZE];
        buffer_length[i] = 0;
    }
}

LoggerBuffer::LoggerBuffer() :back_flush_sm(1) {
    init();
}

LoggerBuffer::~LoggerBuffer() {
    for (int i = 0; i < 2; i++) {
        if (buffer[i] != nullptr) {
            delete buffer[i];
        }
    }
}

uint32_t LoggerBuffer::reset_file_p(WriteFilePoint file_p) {
    if (file_p == nullptr) {
        return 1;
    }

    {
        LockGuard guard(front_flush_mutex);
        this->file_p = file_p;
    }

    return 0;
}

uint32_t LoggerBuffer::flush_buffer() {
    return LoggerBufferFlushPool::instance()->write(file_p,
        buffer[buffer_now_use], buffer_length[buffer_now_use],
        back_flush_sm);
}

uint32_t LoggerBuffer::write(const char * write_buffer, const int &write_size) {
    if (file_p == nullptr) {
        return 1;
    }

    if (write_size <= 0) {
        return 2;
    }

    {
        LockGuard front_guard(front_flush_mutex);
        uint32_t wp = 0;

        while (wp < write_size) {
            uint32_t source_rest = write_size - wp;
            uint32_t target_reset = LOGGER_BUFFER_SIZE - buffer_length[buffer_now_use];
            
            if (source_rest > target_reset) {
                source_rest = target_reset;
            }

            char *tp = buffer[buffer_now_use] + buffer_length[buffer_now_use];
            for (const char *p = write_buffer + wp;
                p < write_buffer + wp + source_rest; p++) {
                *tp = *p;
            }

            wp += source_rest;
            buffer_length[buffer_now_use] += source_rest;

            if (buffer_length[buffer_now_use] == LOGGER_BUFFER_SIZE) {
                back_flush_sm.acquire();
                if (flush_buffer() < 0) {
                    return 3;
                }

                buffer_now_use ^= 1;
                buffer_length[buffer_now_use] = 0;
            }

        }
    }

    return 0;
}

}
}