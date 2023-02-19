#include <atomic>

#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>

#include "../third-party/httplib/httplib.h"
#include "../tools/threadpool.h"

std::atomic<int> count(0);

jimu::ThreadPool thread_pool(4);

void test() {
    httplib::Client cli("http://localhost:8080");

    while(true) {
        auto i = count.fetch_add(1);
        if (i < 100000) {
            auto res = cli.Get("/test");
            if (i%100 == 0) {
                std::cout << "request: " << i << std::endl;
            }
        }
        else {
            break;
        }
    }
}

int main() {
    struct timeval time;
    gettimeofday(&time, NULL);
    long long start = (time.tv_sec*1000 + time.tv_usec/1000);

    for (int i = 0 ; i < 4 ; i++) {
        thread_pool.enqueue(test);
    }

    while(count.load() < 100000) {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    gettimeofday(&time, NULL);
    long long end = (time.tv_sec*1000 + time.tv_usec/1000);

    std::cout << end-start << std::endl;
    std::cout << (end-start)/count.load() << std::endl;

    return 0;
}