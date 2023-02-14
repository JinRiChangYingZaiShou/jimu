#ifndef JIMU_GRAPH_JIMU_EXECUTER_H
#define JIMU_GRAPH_JIMU_EXECUTER_H

#include "../tools/threadpool.h"

namespace jimu {
namespace graph {

class JimuExecuter {
    ThreadPool * thread_pool = nullptr;

public:
    JimuExecuter();
    ~JimuExecuter();

    uint32_t init(uint32_t thread_number);

    template<typename ...Args>
    void run(Args&&... args) {
        thread_pool->enqueue(std::forward<Args>(args)...);
    }

};

}
}

#endif