#pragma once

#include <cstddef>
#include <vector>

namespace benchmark_config
{

// 数据规模（元素数量）
const std::vector<std::size_t> DATA_SIZES = {1000, 10000};

// 每个操作重复执行的次数（取中位数）
constexpr int REPEAT_COUNT = 5;

// 预热次数
constexpr int WARMUP_COUNT = 1;

// 结果输出目录
inline constexpr const char* RESULTS_DIR = "results";

} // namespace benchmark_config