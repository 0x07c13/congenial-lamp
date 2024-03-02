#include "threadPool.h"
#include <iostream>

int main() {
    size_t numThreads = 4;
    ThreadPool* pool = new ThreadPool(numThreads);

    std::vector<std::future<int>> results;

    for (int i = 0; i < 8; ++i) {
        results.emplace_back(pool->enqueue([i] {
            std::cout << "Task " << i << " executed by thread " << std::this_thread::get_id() << std::endl;
            return i * i;
        }));
    }

    delete pool;

    for (auto& result : results) {
        std::cout << "Result: " << result.get() << std::endl;
    }

    return 0;
}