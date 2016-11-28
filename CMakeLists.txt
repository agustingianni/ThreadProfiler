cmake_minimum_required(VERSION 3.5)
project(ThreadProfiler)

set(CMAKE_CXX_STANDARD 11)

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2 -fno-exceptions -Wno-format")

set(SOURCE_FILES main.cpp DiskPool.h measure.h ThreadProfiler.h Allocator.h)
add_executable(ThreadProfiler ${SOURCE_FILES})

target_link_libraries(ThreadProfiler benchmark pthread)