# ThreadProfiler

**Description**

Collection of utilities used while developing multi-threaded code ranging from time measuring routines to fast
allocators. We try to keep things header only so in order to use any of the provided solutions you usually
have to include the right header file in your code.

We have provided a `vagrant` file so testing can be performed easily.

# Allocator.h
Example client for DiskPool.h that performs object allocations (but not deallocations). This is intended
to be used as memory provider for sampling objects collected by the profiler.

# DiskPool.h
Memory allocator used quickly get memory from the system. It is lock free and backed by a file so anything you write to the memory will be saved.
The main purpose of this allocator is to provide memory to a profiler that needs to save data.

It does not support freeing memory as it does not make any sense in the context where it is used.

**Running with `vagrant`***

```
localhost:~$ vagrant up
localhost:~$ vagrant ssh
ubuntu@ubuntu-xenial:~$ cd ThreadProfiler/build/
ubuntu@ubuntu-xenial:~/ThreadProfiler/build$ ls -l test*
-rwxr-xr-x 1 ubuntu ubuntu 970856 Nov 30 17:01 test_allocator
-rwxr-xr-x 1 ubuntu ubuntu 976896 Nov 30 17:01 test_disk_pool
```

**Dependencies**

The headers do not have any dependencies but you do need to have `google-benchmark` in order to build and run the tests.
 
If you are using `vagrant` everything should be resolved for you automatically.  

**Compile**
```
$ cd ThreadProfiler
$ mkdir build
$ cd build
$ cmake ..
$ make
```

**Run the benchmarks**
```
$ ./test_allocator
  Run on (8 X 2500 MHz CPU s)
  2016-11-30 18:16:04
  Benchmark                    Time           CPU Iterations
  ----------------------------------------------------------
  BM_ALLOC/threads:1           7 ns          7 ns   93923171
  BM_ALLOC/threads:2          34 ns         68 ns   10084348
  BM_ALLOC/threads:4          37 ns        147 ns    4461300
  BM_ALLOC/threads:8          28 ns        215 ns    3131464
  BM_ALLOC/threads:16         23 ns        220 ns    3607440
  BM_ALLOC/threads:32         19 ns        220 ns    3405920
  

$ ./test_disk_pool
  Run on (8 X 2500 MHz CPU s)
  2016-11-30 18:16:35
  Benchmark                                                         Time           CPU Iterations
  -----------------------------------------------------------------------------------------------
  BM_MemoryAllocation_DiskPoolRaw/SingleThreaded                    2 ns          2 ns  405083216
  BM_MemoryAllocation_DiskPoolLock/Threaded/threads:1              20 ns         20 ns   34006996
  BM_MemoryAllocation_DiskPoolLock/Threaded/threads:2            2841 ns       3026 ns     252904
  BM_MemoryAllocation_DiskPoolLock/Threaded/threads:4            3276 ns       3434 ns     199416
  BM_MemoryAllocation_DiskPoolLock/Threaded/threads:8            3127 ns       3295 ns     209504
  BM_MemoryAllocation_DiskPoolLock/Threaded/threads:16           3157 ns       3326 ns     205152
  BM_MemoryAllocation_DiskPoolLock/Threaded/threads:32           3247 ns       3415 ns     203488
  BM_MemoryAllocation_DiskPoolSpinLock/Threaded/threads:1           9 ns          9 ns   71797081
  BM_MemoryAllocation_DiskPoolSpinLock/Threaded/threads:2          89 ns        178 ns    5713936
  BM_MemoryAllocation_DiskPoolSpinLock/Threaded/threads:4         158 ns        633 ns    1206844
  BM_MemoryAllocation_DiskPoolSpinLock/Threaded/threads:8         590 ns       4406 ns     187976
  BM_MemoryAllocation_DiskPoolSpinLock/Threaded/threads:16        445 ns       4865 ns     183712
  BM_MemoryAllocation_DiskPoolSpinLock/Threaded/threads:32       1504 ns      14955 ns     143168
  BM_MemoryAllocation_DiskPoolAtomic/Threaded/threads:1             7 ns          7 ns   89753946
  BM_MemoryAllocation_DiskPoolAtomic/Threaded/threads:2            20 ns         41 ns   17576898
  BM_MemoryAllocation_DiskPoolAtomic/Threaded/threads:4            24 ns         94 ns    7323056
  BM_MemoryAllocation_DiskPoolAtomic/Threaded/threads:8            24 ns        182 ns    3359448
  BM_MemoryAllocation_DiskPoolAtomic/Threaded/threads:16           21 ns        210 ns    4339472
  BM_MemoryAllocation_DiskPoolAtomic/Threaded/threads:32           16 ns        197 ns    3200000
  BM_MemoryAllocation_malloc/threads:1                             34 ns         34 ns   19975858
  BM_MemoryAllocation_malloc/threads:2                             17 ns         34 ns   20609754
  BM_MemoryAllocation_malloc/threads:4                             10 ns         38 ns   20154616
  BM_MemoryAllocation_malloc/threads:8                              7 ns         55 ns   12378616
  BM_MemoryAllocation_malloc/threads:16                             6 ns         57 ns   12634672
  BM_MemoryAllocation_malloc/threads:32                             3 ns         56 ns   12425888
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