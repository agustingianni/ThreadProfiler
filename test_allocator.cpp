//
// Created by Agustin Gianni (agustin.gianni@gmail.com) on 11/30/16.
//

#include <benchmark/benchmark.h>

#include "Allocator.h"
#include "DiskPool.h"

template <typename DISK_POOL_TYPE, template <typename> class THREADING_POLICY> void BM_ALLOC_IMPLICIT_POOL(benchmark::State &state) {
    using CustomAllocator = Allocator<DISK_POOL_TYPE, THREADING_POLICY>;
    while (state.KeepRunning()) {
        auto mem = CustomAllocator::alloc(sizeof(size_t));
        benchmark::DoNotOptimize(mem);
    }
}

BENCHMARK_TEMPLATE(BM_ALLOC_IMPLICIT_POOL, DiskPoolAtomic, PerProcessPolicy)->ThreadRange(1, 16);
BENCHMARK_TEMPLATE(BM_ALLOC_IMPLICIT_POOL, DiskPoolLock, PerProcessPolicy)->ThreadRange(1, 16);
BENCHMARK_TEMPLATE(BM_ALLOC_IMPLICIT_POOL, DiskPoolSpinLock, PerProcessPolicy)->ThreadRange(1, 16);
BENCHMARK_TEMPLATE(BM_ALLOC_IMPLICIT_POOL, DiskPoolAtomic, PerThreadPolicy)->ThreadRange(1, 16);
BENCHMARK_TEMPLATE(BM_ALLOC_IMPLICIT_POOL, DiskPoolLock, PerThreadPolicy)->ThreadRange(1, 16);
BENCHMARK_TEMPLATE(BM_ALLOC_IMPLICIT_POOL, DiskPoolSpinLock, PerThreadPolicy)->ThreadRange(1, 16);

template <typename DISK_POOL_TYPE, template <typename> class THREADING_POLICY> void BM_ALLOC_EXPLICIT_POOL(benchmark::State &state) {
    using CustomAllocator = Allocator<DISK_POOL_TYPE, THREADING_POLICY>;
    auto &pool = CustomAllocator::instance();
    while (state.KeepRunning()) {
        auto mem = CustomAllocator::alloc(pool, sizeof(size_t));
        benchmark::DoNotOptimize(mem);
    }
}

BENCHMARK_TEMPLATE(BM_ALLOC_EXPLICIT_POOL, DiskPoolAtomic, PerProcessPolicy)->ThreadRange(1, 16);
BENCHMARK_TEMPLATE(BM_ALLOC_EXPLICIT_POOL, DiskPoolLock, PerProcessPolicy)->ThreadRange(1, 16);
BENCHMARK_TEMPLATE(BM_ALLOC_EXPLICIT_POOL, DiskPoolSpinLock, PerProcessPolicy)->ThreadRange(1, 16);
BENCHMARK_TEMPLATE(BM_ALLOC_EXPLICIT_POOL, DiskPoolAtomic, PerThreadPolicy)->ThreadRange(1, 16);
BENCHMARK_TEMPLATE(BM_ALLOC_EXPLICIT_POOL, DiskPoolLock, PerThreadPolicy)->ThreadRange(1, 16);
BENCHMARK_TEMPLATE(BM_ALLOC_EXPLICIT_POOL, DiskPoolSpinLock, PerThreadPolicy)->ThreadRange(1, 16);

BENCHMARK_MAIN()