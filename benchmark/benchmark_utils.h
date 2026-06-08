#pragma once

#include <chrono>
#include <algorithm>
#include <vector>

#include "benchmark_config.h"

// 计时辅助函数：运行 lambda 并返回中位数耗时（微秒）
template <typename F>
double measure_median_us(F&& func)
{
    namespace bc = benchmark_config;
    using clock = std::chrono::high_resolution_clock;

    // 预热
    for (int i = 0; i < bc::WARMUP_COUNT; ++i)
    {
        func();
    }

    // 正式计时
    std::vector<double> times;
    times.reserve(bc::REPEAT_COUNT);
    for (int i = 0; i < bc::REPEAT_COUNT; ++i)
    {
        auto start = clock::now();
        func();
        auto end = clock::now();
        double us = static_cast<double>(
            std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count()) / 1000.0;
        times.push_back(us);
    }

    // 取中位数
    std::sort(times.begin(), times.end());
    return times[times.size() / 2];
}