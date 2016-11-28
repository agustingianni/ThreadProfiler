#include <thread>
#include <iostream>

#include <benchmark/benchmark.h>

#include "DiskPool.h"
#include "measure.h"
#include "ThreadProfiler.h"

std::atomic<int> stop{0};

// Write all the data to disk.
void io_thread(DiskPool_mt *memory_pool) {
    using precision = std::chrono::milliseconds;

    printf("Starting IO thread.\n");

    while (!stop.load(std::memory_order_acquire)) {
        sleep(1);

        auto ret = measure<precision>::execution(
            [memory_pool] {
                memory_pool->flush();
                return 0;
            }
        );

        printf("Took %ld ms.\n", ret.second);
    }

    printf("Stopping IO thread.\n");
}

// Check the single thread performance.
DiskPool_st memory_pool_st("/tmp/memory.log", GB(8));
void BM_MemoryAllocation_DiskPool_st(benchmark::State &state, DiskPool_st *pool) {
    while (state.KeepRunning()) {
        auto mem = pool->alloc(sizeof(int));
        benchmark::DoNotOptimize(mem);
    }
}

BENCHMARK_CAPTURE(BM_MemoryAllocation_DiskPool_st, SingleThreaded, &memory_pool_st);

static DiskPool_mt memory_pool_mt("/tmp/memory.log", GB(8));
void BM_MemoryAllocation_DiskPool_mt(benchmark::State &state, DiskPool_mt *pool) {
    DiskPool_st memory_pool("/tmp/memory.log", GB(8));
    while (state.KeepRunning()) {
        auto mem = pool->alloc(sizeof(int));
        benchmark::DoNotOptimize(mem);
    }
}

BENCHMARK_CAPTURE(BM_MemoryAllocation_DiskPool_mt, Threaded, &memory_pool_mt)->ThreadRange(1, 32);

void BM_MemoryAllocation_malloc(benchmark::State &state) {
    while (state.KeepRunning()) {
        auto mem = malloc(sizeof(int));
        benchmark::DoNotOptimize(mem);
    }
}

BENCHMARK(BM_MemoryAllocation_malloc)->ThreadRange(1, 32);

BENCHMARK_MAIN()