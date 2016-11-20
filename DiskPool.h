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

template<typename T>
class DiskPool {
protected:
    uint8_t *m_address;
    size_t m_size;

public:
    DiskPool(const std::string &filename, size_t size) : m_size{size} {
        auto fd = open(filename.c_str(), O_RDWR | O_CREAT | O_TRUNC, 0644);
        if (fd == -1) {
            std::cerr << "Failed to open file: " << strerror(errno) << std::endl;
            abort();
        }

        if (ftruncate(fd, m_size) == -1) {
            close(fd);
            std::cerr << "Failed to ftruncate file: " << strerror(errno) << std::endl;
            abort();
        }

        m_address = reinterpret_cast<uint8_t *>(mmap(nullptr, m_size, PROT_READ | PROT_WRITE, MAP_FILE | MAP_SHARED, fd, 0));
        if (m_address == MAP_FAILED) {
            close(fd);
            std::cerr << "Failed to map file: " << strerror(errno) << std::endl;
            abort();
        }

        if (close(fd) != 0) {
            std::cerr << "Failed to close file: " << strerror(errno) << std::endl;
            abort();
        }
    }

    ~DiskPool() {
        if (munmap(m_address, m_size) != 0) {
            std::cerr << "Failed to unmap file: " << strerror(errno) << std::endl;
            abort();
        };
    }

    uint8_t *alloc(size_t size) {
        return static_cast<T *>(this)->allocImpl(size);
    }

    // Flush to disk so we release physical pages.
    void flush() const {
        if (madvise(m_address, m_size, MADV_DONTNEED)) {
            std::cerr << "Failed to flush memory: " << strerror(errno) << std::endl;
            abort();
        }
    }
};

// Single thread disk pool. This does not implement any locking, use with care.
class DiskPool_st : public DiskPool<DiskPool_st> {
    // Let DiskPool access our private fields.
    friend class DiskPool;

    size_t m_top;

public:
    DiskPool_st(const std::string &filename, size_t size) : DiskPool<DiskPool_st>{filename, size}, m_top{0} {
    }

private:
    uint8_t *allocImpl(size_t size) {
        auto tmp = m_top;
        m_top += size;
        return m_address + tmp;
    }
};

// Thread safe disk pool. Uses atomic to handle accesses from multiple threads.
class DiskPool_mt : public DiskPool<DiskPool_mt> {
    // Let DiskPool access our private fields.
    friend class DiskPool;

    std::atomic<size_t> m_top;

public:
    DiskPool_mt(const std::string &filename, size_t size) : DiskPool<DiskPool_mt>{filename, size}, m_top{0} {
    }

private:
    uint8_t *allocImpl(size_t size) {
        return m_address + m_top.fetch_add(size, std::memory_order_relaxed);
    }
};


#endif //THREADPROFILER_DISKPOOL_H
