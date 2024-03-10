#include "thread_pool.h"
#include <iostream>
#include <sstream>

int fn(int i) {
    std::stringstream ss;
    ss << "Task " << i << " processed by thread " << std::this_thread::get_id() << std::endl;
    std::cout << ss.str();
    return i * i;
}

int main() {
    ThreadPool* pool = new ThreadPool(8);

    std::vector<std::future<int> > results;

    for (int i = 0; i < 20; ++i) {
        results.emplace_back(pool->enqueue(fn, i));
    }

    delete pool;

    for (auto& result : results) {
        std::cout << "Result: " << result.get() << std::endl;
    }

    return 0;
}