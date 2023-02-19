#ifndef JIMU_TOOLS_THREADPOOLS_H
#define JIMU_TOOLS_THREADPOOLS_H

#include <queue>
#include <vector>
#include <thread>
#include <future>
#include <functional>
#include <condition_variable>
#include <iostream>

namespace jimu {

class ThreadPool {

    using Function=std::function<void()>;
    std::atomic<int> count;

    std::atomic<bool> ret;

    std::mutex taskMutex;
    std::queue<Function> tasks;
    std::vector<std::thread> workers;
    std::condition_variable condition;

public:

    template<class Func,class... FuncArgs>
    void enqueue(Func&& func,FuncArgs&&... funcArgs);

    explicit ThreadPool(size_t poolSize);
    ~ThreadPool();
};

template<class Func,class... FuncArgs>
void ThreadPool::enqueue(Func&& func,FuncArgs&&... funcArgs){

    auto task = std::make_shared<std::packaged_task<void()> >(
            std::bind(std::forward<Func>(func),
            std::forward<FuncArgs>(funcArgs)...)
    );

    std::cout << "enqueue" << std::endl;

    {
        std::unique_lock<std::mutex> lock(taskMutex);

        tasks.emplace([task](){
            (*task)();
        });

        if (count.load() < 0) {
            count.fetch_add(1);
            condition.notify_one();
        }
        else{
            count.fetch_add(1);
        }

        lock.unlock();
    }
}

}

#endif