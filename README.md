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
Run on (8 X 2500 MHz CPU s)
2016-11-20 18:18:55
Benchmark                                                    Time           CPU Iterations
------------------------------------------------------------------------------------------
BM_MemoryAllocation_DiskPool_st/SingleThreaded               2 ns          2 ns  392702466
BM_MemoryAllocation_DiskPool_mt/Threaded/threads:1           7 ns          7 ns   89359801
BM_MemoryAllocation_DiskPool_mt/Threaded/threads:2          30 ns         60 ns   12258658
BM_MemoryAllocation_DiskPool_mt/Threaded/threads:4          36 ns        145 ns    5047772
BM_MemoryAllocation_DiskPool_mt/Threaded/threads:8          28 ns        218 ns    3231128
BM_MemoryAllocation_DiskPool_mt/Threaded/threads:16         25 ns        224 ns    3212512
BM_MemoryAllocation_DiskPool_mt/Threaded/threads:32          8 ns        215 ns    3280896
BM_MemoryAllocation_malloc/threads:1                        33 ns         33 ns   20863635
BM_MemoryAllocation_malloc/threads:2                        17 ns         34 ns   20570388
BM_MemoryAllocation_malloc/threads:4                         9 ns         35 ns   19849288
BM_MemoryAllocation_malloc/threads:8                         7 ns         55 ns   12442760
BM_MemoryAllocation_malloc/threads:16                        6 ns         56 ns   12364480
BM_MemoryAllocation_malloc/threads:32                        5 ns         57 ns   12604448
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