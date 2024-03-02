#include "threadPool.h"

ThreadPool::ThreadPool(size_t numThreads) : _pool_ptr(std::make_shared<Pool>()) {
    // create numThreads threads
    for (int i = 0; i < numThreads; ++i) {
        _pool_ptr->_workers.emplace_back([pool_ptr = _pool_ptr] {
            // declare the locker. lock it manually later
            std::unique_lock<std::mutex> locker(pool_ptr->_mtx, std::defer_lock);
            
            while (true) {
                locker.lock();
                /* critical section */
                if (!pool_ptr->_tasks.empty()) { // if I got tasks
                    auto task = std::move(pool_ptr->_tasks.front());
                    pool_ptr->_tasks.pop();
                    locker.unlock();
                    task();
                }
                // if the pool is stopping, unlock then end this thread
                else if (pool_ptr->stop) {
                    locker.unlock();
                    break;
                }
                // there are no jobs to do, wait for jobs, and release the lock, 
                // so that other workers can do their jobs
                else {
                    pool_ptr->_cond_work.wait(locker);
                    // don't forget to unlock the locker after being awakened 
                    locker.unlock();
                }
            }
        });
    }
}

ThreadPool::~ThreadPool() {
    { // critical section
        std::unique_lock<std::mutex> locker(_pool_ptr->_mtx);
        _pool_ptr->stop = true;
    }
    
    // get the resources released
    for (std::thread& worker : _pool_ptr->_workers) {
        worker.join();
    }
}

template<class F, class... Args>
auto ThreadPool::enqueue(F&& f, Args&&... args) -> std::future<typename std::result_of<F(Args...)>::type> {
    using return_type = typename std::result_of<F(Args...)>::type;
    auto task = std::make_shared<std::packaged_task<return_type()>>(std::bind(std::forward<F>(f), std::forward<Args>(args)...));
    std::future<return_type> result = task->get_future();
    {
        std::unique_lock<std::mutex> lock(_pool_ptr->_mtx);
        if (_pool_ptr->stop) throw std::runtime_error("enqueue on stopped ThreadPool");
        _pool_ptr->_tasks.emplace([task]() { (*task)(); });
    }
    _pool_ptr->_cond_work.notify_one();
    return result;
}