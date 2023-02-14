#include "jimu_executer.h"

namespace jimu {
namespace graph {

JimuExecuter::JimuExecuter() : thread_pool(nullptr) {}

JimuExecuter::~JimuExecuter() {
    if (thread_pool != nullptr) {
        delete thread_pool;
    }
}

uint32_t JimuExecuter::init(uint32_t thread_number) {
    thread_pool = new ThreadPool(thread_number);
    
    if (thread_pool == nullptr) {
        return 1;
    }

    return 0;
}

}
}