//
// Created by anon on 11/30/16.
//

#include <benchmark/benchmark.h>

#include "Allocator.h"

void BM_ALLOC(benchmark::State &state) {
    while (state.KeepRunning()) {
        auto mem = Allocator::alloc<size_t>();
        benchmark::DoNotOptimize(mem);
    }
}

BENCHMARK(BM_ALLOC)->ThreadRange(1, 32);

BENCHMARK_MAIN()