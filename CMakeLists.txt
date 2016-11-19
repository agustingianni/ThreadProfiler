cmake_minimum_required(VERSION 3.6)
project(ThreadProfiler)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES main.cpp DiskPool.cpp DiskPool.h measure.h)
add_executable(ThreadProfiler ${SOURCE_FILES})