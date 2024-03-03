#include "thread_pool.h"
#include <iostream>

int fn(int i) {
    return i * i;
}

int main() {
    ThreadPool* pool = new ThreadPool(10);

    std::vector<std::future<int> > results;

    for (int i = 0; i < 10; ++i) {
        results.emplace_back(pool->enqueue(fn, i));
    }

    delete pool;

    for (auto& result : results) {
        std::cout << "Result: " << result.get() << std::endl;
    }

    return 0;
}