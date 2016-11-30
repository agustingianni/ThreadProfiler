//
// Created by anon on 11/21/16.
//

#ifndef THREADPROFILER_ALLOCATOR_H
#define THREADPROFILER_ALLOCATOR_H

#include <cstdint>
#include <cstddef>

#include "DiskPool.h"

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
    // Return an instance to the allocator. Thread safe in C++11.
    static DiskPoolAtomic &instance() {
        static DiskPoolAtomic g_allocator("/tmp/memory.log", GB(8));
        return g_allocator;
    }

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
