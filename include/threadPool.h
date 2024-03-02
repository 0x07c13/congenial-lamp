#pragma once

#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <future>

class ThreadPool {
private: 
    // maintain a storage struct here 
    struct Pool {
        std::mutex _mtx;
        std::condition_variable _cond_work; // condition that I got work to do
        bool stop = false;
        std::vector<std::thread> _workers; // worker threads
        std::queue<std::function<void ()> > _tasks; // tasks queue
    };

    std::shared_ptr<Pool> _pool_ptr; // outlet to manipulate the storage

public: 
    explicit ThreadPool(size_t numThreads = 8); 

    ThreadPool(ThreadPool&&) = default; 

     ~ThreadPool();

    // enqueue tasks to the thread pool
    template<class F, class... Args>
    auto enqueue(F&& f, Args&&... args) -> std::future<typename std::result_of<F(Args...)>::type>;
};

#ifdef TEMPLATE_HEADERS_INCLUDE_SOURCES
#include "../src/pool/threadPool.cpp"
#endif