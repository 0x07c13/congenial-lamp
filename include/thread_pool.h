#pragma once

#include <queue>
#include <vector>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <future>

class ThreadPool {
private: 
    // maintain a storage struct here 
    struct Pool {
        std::mutex mtx_;
        std::condition_variable cond_work_; // condition that I got work to do
        bool stop = false;
        std::vector<std::thread> workers_; // worker threads
        std::queue<std::function<void ()> > tasks_; // tasks queue
    };

    std::shared_ptr<Pool> pool_ptr_; // outlet to manipulate the storage

public: 
    explicit ThreadPool(size_t numThreads = 8); 

    ThreadPool(ThreadPool&&) = default; 

     ~ThreadPool();

    // enqueue tasks to the thread pool
    // enqueue will return a future type so I can get the return value of the function in the future
    template<class F, class... Args>
    auto enqueue(F&& f, Args&&... args) -> std::future<typename std::result_of<F(Args...)>::type>;
};

#ifdef TEMPLATE_HEADERS_INCLUDE_SOURCES
#include "../src/pool/thread_pool.cpp"
#endif