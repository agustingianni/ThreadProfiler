# ThreadProfiler
Collection of utilities used while developing multi-threaded code.

# DiskPool.h
Memory allocator used quickly get memory from the system. It is lock free and backed by a file so anything you write to the memory will be saved.
The main purpose of this allocator is to provide memory to a profiler that needs to save data.

It does not support freeing memory as it does not make any sense in the context where it is used.
