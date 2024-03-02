#include "threadPool.h"
#include <iostream>

int main() {
    ThreadPool* pool = new ThreadPool();

    std::vector<std::future<int>> results;

    for (int i = 0; i < 10; ++i) {
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