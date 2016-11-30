cmake_minimum_required(VERSION 3.5)
project(ThreadProfiler)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2 -fno-exceptions -Wno-format")

# Create a test binary for the disk pool.
set(TEST_DISK_POOL_SOURCES test_disk_pool.cpp DiskPool.h measure.h ThreadProfiler.h Allocator.h)
add_executable(test_disk_pool ${TEST_DISK_POOL_SOURCES})
target_link_libraries(test_disk_pool benchmark pthread)

# Create a test binary for the allocator.
set(TEST_ALLOCATOR_SOURCES test_allocator.cpp DiskPool.h Allocator.h)
add_executable(test_allocator ${TEST_ALLOCATOR_SOURCES})
target_link_libraries(test_allocator benchmark pthread)