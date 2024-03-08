#include "thread_pool.h"

ThreadPool::ThreadPool(size_t numThreads) : pool_ptr_(std::make_shared<Pool>()) {
    // create numThreads threads
    for (size_t i = 0; i < numThreads; ++i) {
        pool_ptr_->workers_.emplace_back([pool_ptr = pool_ptr_] {
            // declare the locker. lock it manually later
            std::unique_lock<std::mutex> locker(pool_ptr->mtx_, std::defer_lock);
            
            while (true) {
                locker.lock();
                /* critical section */
                if (!pool_ptr->tasks_.empty()) { // if I got tasks
                    auto task = std::move(pool_ptr->tasks_.front());
                    pool_ptr->tasks_.pop();
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
                    pool_ptr->cond_work_.wait(locker);
                    // don't forget to unlock the locker after being awakened 
                    locker.unlock();
                }
            }
        });
    }
}

ThreadPool::~ThreadPool() {
    { // critical section
        std::unique_lock<std::mutex> locker(pool_ptr_->mtx_);
        pool_ptr_->stop = true;
    }
    
    // get the resources released
    for (std::thread& worker : pool_ptr_->workers_) {
        worker.join();
    }
}

// enqueue will return a future type so I can get the return value of the function in the future
template<class F, class... Args>
auto ThreadPool::enqueue(F&& f, Args&&... args) -> std::future<typename std::result_of<F(Args...)>::type> {
    using return_type = typename std::result_of<F(Args...)>::type;
    auto task = std::make_shared<std::packaged_task<return_type()> >(std::bind(std::forward<F>(f), std::forward<Args>(args)...));
    std::future<return_type> result = task->get_future();
    {
        std::unique_lock<std::mutex> lock(pool_ptr_->mtx_);
        if (pool_ptr_->stop) throw std::runtime_error("enqueue on stopped ThreadPool");
        // create a new void() lambda to emplace to the _tasks queue
        pool_ptr_->tasks_.emplace([task]() { (*task)(); });
    }
    pool_ptr_->cond_work_.notify_one();
    return result;
}