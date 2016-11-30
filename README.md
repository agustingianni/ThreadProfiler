# ThreadProfiler
Collection of utilities used while developing multi-threaded code.

# DiskPool.h
Memory allocator used quickly get memory from the system. It is lock free and backed by a file so anything you write to the memory will be saved.
The main purpose of this allocator is to provide memory to a profiler that needs to save data.

It does not support freeing memory as it does not make any sense in the context where it is used.

**Clone the repository**
```
➜  /tmp git clone git@github.com:agustingianni/ThreadProfiler.git
Cloning into 'ThreadProfiler'...
remote: Counting objects: 19, done.
remote: Compressing objects: 100% (17/17), done.
remote: Total 19 (delta 4), reused 11 (delta 2), pack-reused 0
Receiving objects: 100% (19/19), 5.93 KiB | 0 bytes/s, done.
Resolving deltas: 100% (4/4), done.
```

**Install dependencies**
```
➜  /tmp brew install google-benchmark
```

**Compile**
```
➜  /tmp cd ThreadProfiler
➜  ThreadProfiler git:(master) mkdir build
➜  ThreadProfiler git:(master) cd build
➜  build git:(master) cmake ..
-- The C compiler identification is AppleClang 8.0.0.8000042
-- The CXX compiler identification is AppleClang 8.0.0.8000042
-- Check for working C compiler: /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/cc
-- Check for working C compiler: /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/cc -- works
-- Detecting C compiler ABI info
-- Detecting C compiler ABI info - done
-- Detecting C compile features
-- Detecting C compile features - done
-- Check for working CXX compiler: /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++
-- Check for working CXX compiler: /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ -- works
-- Detecting CXX compiler ABI info
-- Detecting CXX compiler ABI info - done
-- Detecting CXX compile features
-- Detecting CXX compile features - done
-- Configuring done
-- Generating done
-- Build files have been written to: /tmp/ThreadProfiler/build

➜  build git:(master) make
Scanning dependencies of target ThreadProfiler
[ 33%] Building CXX object CMakeFiles/ThreadProfiler.dir/main.cpp.o
[ 66%] Building CXX object CMakeFiles/ThreadProfiler.dir/DiskPool.cpp.o
[100%] Linking CXX executable ThreadProfiler
[100%] Built target ThreadProfiler
```

**Run the benchmarks**
```
➜  build git:(master) ./ThreadProfiler
/Users/anon/workspace/ThreadProfiler/cmake-build-debug/ThreadProfiler
Benchmark                                                         Time           CPU Iterations
Run on (8 X 2500 MHz CPU s)
-----------------------------------------------------------------------------------------------
2016-11-30 03:19:36
BM_MemoryAllocation_DiskPoolRaw/SingleThreaded                    2 ns          2 ns  358994610
BM_MemoryAllocation_DiskPoolLock/Threaded/threads:1              23 ns         23 ns   31998391
BM_MemoryAllocation_DiskPoolLock/Threaded/threads:2            2840 ns       3044 ns     224036
BM_MemoryAllocation_DiskPoolLock/Threaded/threads:4            3263 ns       3420 ns     205736
BM_MemoryAllocation_DiskPoolLock/Threaded/threads:8            3281 ns       3443 ns     206008
BM_MemoryAllocation_DiskPoolLock/Threaded/threads:16           3275 ns       3434 ns     197824
BM_MemoryAllocation_DiskPoolLock/Threaded/threads:32           3266 ns       3422 ns     198528
BM_MemoryAllocation_DiskPoolSpinLock/Threaded/threads:1           9 ns          9 ns   69723199
BM_MemoryAllocation_DiskPoolSpinLock/Threaded/threads:2          70 ns        140 ns    6619448
BM_MemoryAllocation_DiskPoolSpinLock/Threaded/threads:4         125 ns        500 ns    1858724
BM_MemoryAllocation_DiskPoolSpinLock/Threaded/threads:8         677 ns       5304 ns     173120
BM_MemoryAllocation_DiskPoolSpinLock/Threaded/threads:16        778 ns       7230 ns     142448
BM_MemoryAllocation_DiskPoolSpinLock/Threaded/threads:32        879 ns       9094 ns     141504
BM_MemoryAllocation_DiskPoolAtomic/Threaded/threads:1             7 ns          7 ns   91669831
BM_MemoryAllocation_DiskPoolAtomic/Threaded/threads:2            26 ns         52 ns   13613778
BM_MemoryAllocation_DiskPoolAtomic/Threaded/threads:4            26 ns        105 ns    7132884
BM_MemoryAllocation_DiskPoolAtomic/Threaded/threads:8            27 ns        215 ns    3571176
BM_MemoryAllocation_DiskPoolAtomic/Threaded/threads:16           25 ns        214 ns    3480400
BM_MemoryAllocation_DiskPoolAtomic/Threaded/threads:32            7 ns        193 ns    3390752
BM_MemoryAllocation_malloc/threads:1                             34 ns         34 ns   20852883
BM_MemoryAllocation_malloc/threads:2                             17 ns         34 ns   20739818
BM_MemoryAllocation_malloc/threads:4                              9 ns         35 ns   20206540
BM_MemoryAllocation_malloc/threads:8                              7 ns         57 ns   12690872
BM_MemoryAllocation_malloc/threads:16                             6 ns         57 ns   12490528
BM_MemoryAllocation_malloc/threads:32                             6 ns         57 ns   12205760
```

# measure.h

Small wrapper to measure execution time of arbitrary functions.

**Usage**

* Just include `measure.h` in your source file and use it to wrap what you want to measure.
* You can specify the precision and the clock used, although I would not change the clock.
* If the function you want to measure returns void, you need to wrap it on a lambda and make
the lambda return a value.
* If the function to measure is a method, use a lambda and capture the object.

```
void io_thread(DiskPool_mt *memory_pool) {
    auto ret = measure<std::chrono::milliseconds>::execution(
        [memory_pool] {
            memory_pool->flush();
            return 0;
        }
    );

    printf("Took %ld ms.\n", ret.second);
}
```