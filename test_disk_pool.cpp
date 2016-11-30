#include <thread>
#include <iostream>

#include <benchmark/benchmark.h>

#include "DiskPool.h"
#include "measure.h"
#include "ThreadProfiler.h"

std::atomic<int> stop{0};

// Write all the data to disk.
void io_thread(DiskPoolAtomic *memory_pool) {
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
DiskPoolRaw memory_pool_raw("/tmp/memory.log", GB(8));
void BM_MemoryAllocation_DiskPoolRaw(benchmark::State &state, DiskPoolRaw *pool) {
    while (state.KeepRunning()) {
        auto mem = pool->alloc(sizeof(int));
        benchmark::DoNotOptimize(mem);
    }
}

BENCHMARK_CAPTURE(BM_MemoryAllocation_DiskPoolRaw, SingleThreaded, &memory_pool_raw);

// Check the multi-threaded locked performance.
static DiskPoolLock memory_pool_lock("/tmp/memory.log", GB(8));
void BM_MemoryAllocation_DiskPoolLock(benchmark::State &state, DiskPoolLock *pool) {
    while (state.KeepRunning()) {
        auto mem = pool->alloc(sizeof(int));
        benchmark::DoNotOptimize(mem);
    }
}

BENCHMARK_CAPTURE(BM_MemoryAllocation_DiskPoolLock, Threaded, &memory_pool_lock)->ThreadRange(1, 32);

// Check the multi-threaded locked performance.
static DiskPoolSpinLock memory_pool_spinlock("/tmp/memory.log", GB(8));
void BM_MemoryAllocation_DiskPoolSpinLock(benchmark::State &state, DiskPoolSpinLock *pool) {
    while (state.KeepRunning()) {
        auto mem = pool->alloc(sizeof(int));
        benchmark::DoNotOptimize(mem);
    }
}

BENCHMARK_CAPTURE(BM_MemoryAllocation_DiskPoolSpinLock, Threaded, &memory_pool_spinlock)->ThreadRange(1, 32);

// Check the multi-threaded atomic performance.
static DiskPoolAtomic memory_pool_atomic("/tmp/memory.log", GB(8));
void BM_MemoryAllocation_DiskPoolAtomic(benchmark::State &state, DiskPoolAtomic *pool) {
    while (state.KeepRunning()) {
        auto mem = pool->alloc(sizeof(int));
        benchmark::DoNotOptimize(mem);
    }
}

BENCHMARK_CAPTURE(BM_MemoryAllocation_DiskPoolAtomic, Threaded, &memory_pool_atomic)->ThreadRange(1, 32);

void BM_MemoryAllocation_malloc(benchmark::State &state) {
    while (state.KeepRunning()) {
        auto mem = malloc(sizeof(int));
        benchmark::DoNotOptimize(mem);
    }
}

BENCHMARK(BM_MemoryAllocation_malloc)->ThreadRange(1, 32);

BENCHMARK_MAIN()