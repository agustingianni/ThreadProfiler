//
// Created by Agustin Gianni on 11/18/16.
//

#ifndef THREADPROFILER_DISKPOOL_H
#define THREADPROFILER_DISKPOOL_H

#include <atomic>
#include <string>
#include <cstddef>
#include <cassert>
#include <cstdio>
#include <cstring>
#include <cerrno>
#include <iostream>

#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>

constexpr size_t KB(size_t size) { return size * 1024; }
constexpr size_t MB(size_t size) { return KB(size) * 1024; }
constexpr size_t GB(size_t size) { return MB(size) * 1024; }

// Implement a memory map policy using 'mmap'.
struct mmap_policy {
    static void *map(const std::string &filename, size_t size) {
        auto fd = open(filename.c_str(), O_RDWR | O_CREAT | O_TRUNC, 0644);
        if (fd == -1) {
            std::cerr << "Failed to open file: " << strerror(errno) << std::endl;
            abort();
        }

        if (ftruncate(fd, size) == -1) {
            close(fd);
            std::cerr << "Failed to ftruncate file: " << strerror(errno) << std::endl;
            abort();
        }

        auto address = reinterpret_cast<uint8_t *>(mmap(nullptr, size, PROT_READ | PROT_WRITE, MAP_FILE | MAP_SHARED, fd, 0));
        if (address == MAP_FAILED) {
            close(fd);
            std::cerr << "Failed to map file: " << strerror(errno) << std::endl;
            abort();
        }

        if (close(fd) != 0) {
            unmap(address, size);
            std::cerr << "Failed to close file: " << strerror(errno) << std::endl;
            abort();
        }

        return address;
    }

    static void unmap(void *address, size_t size) {
        if (munmap(address, size) != 0) {
            std::cerr << "Failed to unmap file: " << strerror(errno) << std::endl;
            abort();
        }
    }

    static void flush(void *address, size_t size) {
        if (madvise(address, size, MADV_DONTNEED)) {
            std::cerr << "Failed to flush memory: " << strerror(errno) << std::endl;
            abort();
        }
    }
};

// Simple implementation of a spin lock.
class SpinLock {
public:
    void lock() {
        while(m_lock.test_and_set(std::memory_order_acquire)) {
        }
    }

    void unlock() {
        m_lock.clear(std::memory_order_release);
    }

private:
    std::atomic_flag m_lock = ATOMIC_FLAG_INIT;
};

// Implement a policy that increments the value non atomically.
template <typename T = size_t> class RawIncrement {
private:
    T m_top{0};

public:
    T increment(T size) {
        auto tmp = m_top;
        m_top += size;
        return m_top;
    }
};

// Implement a policy that uses std::atomic to increment the value.
template <typename T = size_t> class AtomicIncrement {
private:
    std::atomic<T> m_top{0};

public:
    T increment(T size) {
        return m_top.fetch_add(size, std::memory_order_relaxed);
    }
};

// Implement a policy that uses std::mutex to increment the value.
template <typename T = size_t> class LockedIncrement {
private:
    std::mutex m_mutex;
    T m_top{0};

public:
    T increment(T size) {
        std::lock_guard<std::mutex> lock(m_mutex);
        auto tmp = m_top;
        m_top += size;
        return tmp;
    }
};

// Implement a policy that uses SpinLock to increment the value.
template <typename T = size_t> class SpinLockIncrement {
private:
    SpinLock m_spinlock;
    T m_top{0};

public:
    T increment(T size) {
        m_spinlock.lock();
        auto tmp = m_top;
        m_top += size;
        m_spinlock.unlock();
        return tmp;
    }
};

// A 'DiskPool' is a named file backed memory allocator.
template<typename IncrementPolicy = AtomicIncrement<size_t>, typename FileMapPolicy = mmap_policy>
class DiskPool: public IncrementPolicy {
protected:
    const std::string m_filename;
    uint8_t *m_address;
    size_t m_size;

public:
    DiskPool(const std::string &filename, size_t size) : m_filename{filename}, m_size{size} {
        m_address = static_cast<uint8_t *>(FileMapPolicy::map(m_filename, m_size));
    }

    ~DiskPool() {
        FileMapPolicy::unmap(m_address, m_size);
    }

    uint8_t *alloc(size_t size) {
        return m_address + IncrementPolicy::increment(size);
    }

    void flush() const {
        FileMapPolicy::flush(m_address, m_size);
    }
};

// Define basic DiskPool implementations to be used by the client.
using DiskPoolRaw = DiskPool<RawIncrement<size_t>>;
using DiskPoolAtomic = DiskPool<AtomicIncrement<size_t>>;
using DiskPoolLock = DiskPool<LockedIncrement<size_t>>;
using DiskPoolSpinLock = DiskPool<SpinLockIncrement<size_t>>;

#endif //THREADPROFILER_DISKPOOL_H
