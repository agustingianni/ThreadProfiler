//
// Created by Agustin Gianni (agustin.gianni@gmail.com) on 11/21/16.
//

#ifndef THREADPROFILER_ALLOCATOR_H
#define THREADPROFILER_ALLOCATOR_H

#include <cstdint>
#include <cstddef>

#include "DiskPool.h"

// Create an instance of 'T' in a per thread fashion.
template <typename T>
struct PerThreadPolicy {
    template <typename ... Ts>
    static T &instance(Ts ... args) {
        thread_local T g_instance(args ...);
        return g_instance;
    }
};

// Create an instance of 'T' in a per process fashion.
template <typename T>
struct PerProcessPolicy {
    template <typename ... Ts>
    static T &instance(Ts ... args) {
        static T g_instance(args ...);
        return g_instance;
    }
};

// Create a memory allocator using a given 'MemoryPool'.
template <typename MemoryPool, template <typename> class ThreadingPolicy>
class Allocator {
public:
    static uint8_t *alloc(size_t size) {
        return instance().alloc(size);
    }

    template <typename T>
    static uint8_t *alloc() {
        return instance().alloc(sizeof(T));
    }

private:
    // Return an instance to the memory pool according to 'ThreadingPolicy'.
    static MemoryPool &instance() {
        return ThreadingPolicy<MemoryPool>::instance("/tmp/memory.log", GB(8));
    }

    // Avoid explicit construction and destruction.
    Allocator() = delete;
    ~Allocator() = delete;

    // Avoid copies.
    Allocator(const Allocator &) = delete;
    Allocator &operator=(const Allocator &) = delete;

    // Avoid moves.
    Allocator(Allocator &&) = delete;
    Allocator &operator=(Allocator &&) = delete;
};

#endif //THREADPROFILER_ALLOCATOR_H
