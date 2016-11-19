#include <atomic>
#include <thread>
#include <chrono>
#include <iostream>

#include "DiskPool.h"
#include "measure.h"

constexpr size_t KB(size_t size) { return size * 1024; }
constexpr size_t MB(size_t size) { return KB(size) * 1024; }
constexpr size_t GB(size_t size) { return MB(size) * 1024; }

std::atomic<int> stop{0};
DiskPool memory_pool("/tmp/memory.log", GB(8));

// Write all the data to disk.
void io_thread() {
    using clock = std::chrono::steady_clock;
    using precision = std::chrono::milliseconds;

    printf("Starting IO thread.\n");

    while (!stop.load(std::memory_order_acquire)) {
        sleep(1);

        auto t0 = clock::now();
        memory_pool.flush();
        auto delta = std::chrono::duration_cast<precision>(clock::now() - t0).count();

        printf("Took %llu ms.\n", delta);
    }

    printf("Stopping IO thread.\n");
}

// Generate all the data.
void work_thread() {
    using clock = std::chrono::steady_clock;
    using precision = std::chrono::nanoseconds;

    printf("Starting Work thread.\n");

    constexpr double alpha = 1.0 / 1000;
    constexpr double one_minus_alpha = 1.0 - alpha;
    double accumulator = 0.0;

    for (int i = 0; i < 10000000; i++) {
        auto t0 = clock::now();
        auto mem = reinterpret_cast<int *>(memory_pool.alloc(sizeof(i)));
        auto count = std::chrono::duration_cast<precision>(clock::now() - t0).count();

        // Compute the "average".
        accumulator = (alpha * count) + one_minus_alpha * accumulator;

        *mem = i;
    }

    printf("Average time for allocation: %f ns\n", accumulator);

    // Stop the IO thread.
    stop.store(1, std::memory_order_release);

    printf("Stopping Work thread.\n");
}

void bench_malloc() {
    size_t malloc_size = 32;
    for (auto i = 0; i < 1000; i++) {
        auto ret = measure<std::chrono::nanoseconds>::execution(malloc, malloc_size);
        free(ret.first);
        //printf("Time for malloc(%zu): %lld ns\n", malloc_size, ret.second);
    }
}

int main() {
    std::thread t1(io_thread);
    std::thread t2(work_thread);
    std::thread t3(work_thread);
    // std::thread t4(bench_malloc);

    t1.join();
    t2.join();
    t3.join();
    //t4.join();

    return 0;
}