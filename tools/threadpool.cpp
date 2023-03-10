#include "threadpool.h"

namespace jimu {

ThreadPool::ThreadPool(size_t poolSize):
        count(0),
        ret(false){
    for (size_t i = 0; i < poolSize; i++) {
        workers.emplace_back(
                [this]{
                    for( ; ; ){
                        Function task;
                        {
                            std::unique_lock<std::mutex> lock(this->taskMutex);

                            if (this->ret.load()) {
                                return;
                            }

                            if (count.load() <= 0) {
                                count.fetch_sub(1);
                                this->condition.wait(lock);
                            }
                            else {
                                count.fetch_sub(1);
                            }

                            task = std::move(this->tasks.front());
                            this->tasks.pop();

                            lock.unlock();
                        }
                        std::cout << "run_task" << std::endl;
                        task();
                    }
                }
        );
    }
}

ThreadPool::~ThreadPool() {
    {
        std::unique_lock<std::mutex> lock;
        ret.store(true);
        lock.unlock();

        condition.notify_all();

        for (auto &worker : workers) {
            worker.join();
        }
    }
}

}