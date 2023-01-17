#ifndef JIMU_LOGGER_LOGGER_BUFFER_H
#define JIMU_LOGGER_LOGGER_BUFFER_H

#include <string>
#include <fstream>
#include <iostream>
#include <mutex>
#include <semaphore>
#include <memory>

#include "tools/threadpool.h"

namespace jimu  {
namespace logger {

#define LOGGER_BUFFER_SIZE 4096

using WriteFilePoint = std::shared_ptr<std::ofstream>;
using LockGuard = std::lock_guard<std::mutex>;

class LoggerBufferFlushPool {
    ThreadPool* thread_pool = nullptr;

public:
    LoggerBufferFlushPool();
    ~LoggerBufferFlushPool();

    static LoggerBufferFlushPool* instance();
    uint32_t init(uint32_t thread_number);
    uint32_t write(WriteFilePoint file_p, const char * write_buffer,
        const int &write_size, std::counting_semaphore<1> & buffer_sm);
};

class LoggerBuffer {
    WriteFilePoint file_p;
    
    char* buffer[2];
    uint32_t buffer_length[2];

    bool buffer_now_use;

    std::counting_semaphore<1> back_flush_sm;
    std::mutex front_flush_mutex;

    uint32_t flush_buffer();

    void init();

public:

    LoggerBuffer();
    ~LoggerBuffer();

    uint32_t reset_file_p(WriteFilePoint file_p);
    uint32_t write(const char * write_buffer, const int &write_size);

};

}
}

#endif