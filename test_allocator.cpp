//
// Created by Agustin Gianni (agustin.gianni@gmail.com) on 11/30/16.
//

#include <benchmark/benchmark.h>

#include "Allocator.h"
#include "DiskPool.h"

void BM_ALLOC_TEMPLATED_SIZE_PER_PROCESS(benchmark::State &state) {
    while (state.KeepRunning()) {
        auto mem = Allocator<DiskPoolAtomic, PerProcessPolicy>::alloc<size_t>();
        benchmark::DoNotOptimize(mem);
    }
}

BENCHMARK(BM_ALLOC_TEMPLATED_SIZE_PER_PROCESS)->ThreadRange(1, 32);

void BM_ALLOC_EXPLICIT_SIZE_PER_PROCESS(benchmark::State &state) {
    while (state.KeepRunning()) {
        auto mem = Allocator<DiskPoolAtomic, PerProcessPolicy>::alloc(sizeof(size_t));
        benchmark::DoNotOptimize(mem);
    }
}

BENCHMARK(BM_ALLOC_EXPLICIT_SIZE_PER_PROCESS)->ThreadRange(1, 32);

void BM_ALLOC_TEMPLATED_SIZE_PER_THREAD(benchmark::State &state) {
    while (state.KeepRunning()) {
        auto mem = Allocator<DiskPoolAtomic, PerThreadPolicy>::alloc<size_t>();
        benchmark::DoNotOptimize(mem);
    }
}

BENCHMARK(BM_ALLOC_TEMPLATED_SIZE_PER_THREAD)->ThreadRange(1, 32);

void BM_ALLOC_EXPLICIT_SIZE_PER_THREAD(benchmark::State &state) {
    while (state.KeepRunning()) {
        auto mem = Allocator<DiskPoolAtomic, PerThreadPolicy>::alloc(sizeof(size_t));
        benchmark::DoNotOptimize(mem);
    }
}

BENCHMARK(BM_ALLOC_EXPLICIT_SIZE_PER_THREAD)->ThreadRange(1, 32);


BENCHMARK_MAIN()