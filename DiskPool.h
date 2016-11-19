//
// Created by Agustin Gianni on 11/18/16.
//

#ifndef THREADPROFILER_DISKPOOL_H
#define THREADPROFILER_DISKPOOL_H

#include <atomic>
#include <string>
#include <cstddef>
#include <cassert>

#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <cerrno>
#include <stdatomic.h>

class DiskPool {
    uint8_t *m_address;
    size_t m_size;
    std::atomic<size_t> m_top;

public:
    DiskPool(const std::string &filename, size_t size) : m_size{size}, m_top{0} {
        auto fd = open(filename.c_str(), O_RDWR | O_CREAT | O_TRUNC, 0644);
        if (fd == -1) {
            throw std::runtime_error("Failed to open file: " + std::string(strerror(errno)));
        }

        if (ftruncate(fd, m_size) == -1) {
            close(fd);
            throw std::runtime_error("Failed to ftruncate file: " + std::string(strerror(errno)));
        }

        m_address = reinterpret_cast<uint8_t *>(mmap(nullptr, m_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0));
        if (m_address == MAP_FAILED) {
            close(fd);
            throw std::runtime_error("Failed to map file: " + std::string(strerror(errno)));
        }

        if (close(fd) != 0) {
            throw std::runtime_error("Failed to close file: " + std::string(strerror(errno)));
        }
    }

    ~DiskPool() {
        if (munmap(m_address, m_size) != 0) {
            throw std::runtime_error("Failed to unmap file: " + std::string(strerror(errno)));
        };
    }

    uint8_t *alloc(size_t size) {
        return m_address + m_top.fetch_add(size, std::memory_order_relaxed);
    }

    // Flush to disk so we release physical pages.
    void flush() {
        assert(madvise(m_address, m_size, MADV_DONTNEED) == 0 && "Failed to de-commit memory.");
    }
};


#endif //THREADPROFILER_DISKPOOL_H
