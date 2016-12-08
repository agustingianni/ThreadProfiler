//
// Created by Agustin Gianni (agustin.gianni@gmail.com) on 11/19/16.
//

#ifndef THREADPROFILER_MEASURE_H
#define THREADPROFILER_MEASURE_H

#include <unistd.h>
#include <chrono>
#include <string>
#include <iostream>

// Measure the time it takes to execute a given function.
template<typename time_type=std::chrono::milliseconds, typename clock=std::chrono::steady_clock>
struct measure {
    template<typename F, typename ...Args>
    static std::pair<typename std::result_of<F(Args...)>::type, typename time_type::rep> execution(F func, Args&&... args) {
        auto t0 = clock::now();
        auto ret = func(std::forward<Args>(args)...);
        auto t1 = clock::now();
        auto delta = std::chrono::duration_cast<time_type>(t1 - t0);
        return std::make_pair(ret, delta.count());
    }
};

#endif //THREADPROFILER_MEASURE_H
