#include <vector>
#include <algorithm>
#include <numeric>
#include <random>

#include "vector/vector.h"
#include "benchmark_config.h"
#include "csv_writer.h"

#include "benchmark_utils.h"

namespace
{
// 生成随机整数数组
std::vector<int> generate_ints(std::size_t n)
{
    std::vector<int> data(n);
    std::mt19937 rng(42);
    std::uniform_int_distribution<int> dist(1, 1000000);
    for (auto& x : data)
        x = dist(rng);
    return data;
}

// 防止编译器优化掉结果
volatile long long sink = 0;

template <typename Vec>
void run_construct(Vec&, csv_writer::CsvWriter& writer, const std::vector<int>& src,
                   std::size_t n)
{
    // 默认构造
    {
        auto op = [&]() {
            Vec v;
            sink = v.size();
        };
        double t = measure_median_us(op);
        writer.write_row({"vector", "default_construct", n, "demo", t});
    }
    {
        auto op = [&]() {
            std::vector<int> v;
            sink = v.size();
        };
        double t = measure_median_us(op);
        writer.write_row({"vector", "default_construct", n, "std", t});
    }

    // N个元素构造
    {
        auto op = [&]() {
            Vec v(n);
            sink = v.size();
        };
        double t = measure_median_us(op);
        writer.write_row({"vector", "fill_construct", n, "demo", t});
    }
    {
        auto op = [&]() {
            std::vector<int> v(n);
            sink = v.size();
        };
        double t = measure_median_us(op);
        writer.write_row({"vector", "fill_construct", n, "std", t});
    }

    // 范围构造
    {
        auto op = [&]() {
            Vec v(src.begin(), src.begin() + static_cast<std::ptrdiff_t>(n));
            sink = v.size();
        };
        double t = measure_median_us(op);
        writer.write_row({"vector", "range_construct", n, "demo", t});
    }
    {
        auto op = [&]() {
            std::vector<int> v(src.begin(), src.begin() + static_cast<std::ptrdiff_t>(n));
            sink = v.size();
        };
        double t = measure_median_us(op);
        writer.write_row({"vector", "range_construct", n, "std", t});
    }
}

template <typename Vec>
void run_copy_construct(Vec& prebuilt, csv_writer::CsvWriter& writer, std::size_t n)
{
    {
        auto op = [&]() {
            Vec v(prebuilt);
            sink = v.size();
        };
        double t = measure_median_us(op);
        writer.write_row({"vector", "copy_construct", n, "demo", t});
    }
    {
        std::vector<int> std_prebuilt(prebuilt.begin(), prebuilt.end());
        auto op = [&]() {
            std::vector<int> v(std_prebuilt);
            sink = v.size();
        };
        double t = measure_median_us(op);
        writer.write_row({"vector", "copy_construct", n, "std", t});
    }
}

template <typename Vec>
void run_push_back(Vec&, csv_writer::CsvWriter& writer, const std::vector<int>& src,
                   std::size_t n)
{
    {
        auto op = [&]() {
            Vec v;
            for (std::size_t i = 0; i < n; ++i)
                v.push_back(src[i]);
            sink = v.size();
        };
        double t = measure_median_us(op);
        writer.write_row({"vector", "push_back", n, "demo", t});
    }
    {
        auto op = [&]() {
            std::vector<int> v;
            for (std::size_t i = 0; i < n; ++i)
                v.push_back(src[i]);
            sink = v.size();
        };
        double t = measure_median_us(op);
        writer.write_row({"vector", "push_back", n, "std", t});
    }
}

template <typename Vec>
void run_insert(Vec&, csv_writer::CsvWriter& writer, const std::vector<int>& src,
                std::size_t n)
{
    // 在中间位置插入（使用较小的 n/10 避免 O(n²) 过大）
    std::size_t insert_count = n / 10 > 0 ? n / 10 : 1;
    {
        auto op = [&]() {
            Vec v;
            for (std::size_t i = 0; i < insert_count; ++i)
                v.insert(v.begin() + static_cast<std::ptrdiff_t>(v.size() / 2), src[i]);
            sink = v.size();
        };
        double t = measure_median_us(op);
        writer.write_row({"vector", "insert_middle", n, "demo", t});
    }
    {
        auto op = [&]() {
            std::vector<int> v;
            for (std::size_t i = 0; i < insert_count; ++i)
                v.insert(v.begin() + static_cast<std::ptrdiff_t>(v.size() / 2), src[i]);
            sink = v.size();
        };
        double t = measure_median_us(op);
        writer.write_row({"vector", "insert_middle", n, "std", t});
    }
}

template <typename Vec>
void run_pop_back(Vec& prebuilt, csv_writer::CsvWriter& writer, std::size_t n)
{
    {
        auto op = [&]() {
            Vec v(prebuilt);
            for (std::size_t i = 0; i < n && !v.empty(); ++i)
                v.pop_back();
            sink = v.size();
        };
        double t = measure_median_us(op);
        writer.write_row({"vector", "pop_back", n, "demo", t});
    }
    {
        std::vector<int> std_prebuilt(prebuilt.begin(), prebuilt.end());
        auto op = [&]() {
            std::vector<int> v(std_prebuilt);
            for (std::size_t i = 0; i < n && !v.empty(); ++i)
                v.pop_back();
            sink = v.size();
        };
        double t = measure_median_us(op);
        writer.write_row({"vector", "pop_back", n, "std", t});
    }
}

template <typename Vec>
void run_erase(Vec& prebuilt, csv_writer::CsvWriter& writer, std::size_t n)
{
    // 从中间擦除（使用较小的擦除数量）
    std::size_t erase_count = n / 10 > 0 ? n / 10 : 1;
    {
        auto op = [&]() {
            Vec v(prebuilt);
            for (std::size_t i = 0; i < erase_count && !v.empty(); ++i)
                v.erase(v.begin() + static_cast<std::ptrdiff_t>(v.size() / 2));
            sink = v.size();
        };
        double t = measure_median_us(op);
        writer.write_row({"vector", "erase_middle", n, "demo", t});
    }
    {
        std::vector<int> std_prebuilt(prebuilt.begin(), prebuilt.end());
        auto op = [&]() {
            std::vector<int> v(std_prebuilt);
            for (std::size_t i = 0; i < erase_count && !v.empty(); ++i)
                v.erase(v.begin() + static_cast<std::ptrdiff_t>(v.size() / 2));
            sink = v.size();
        };
        double t = measure_median_us(op);
        writer.write_row({"vector", "erase_middle", n, "std", t});
    }
}

template <typename Vec>
void run_clear(Vec& prebuilt, csv_writer::CsvWriter& writer, std::size_t n)
{
    {
        auto op = [&]() {
            Vec v(prebuilt);
            v.clear();
            sink = v.size();
        };
        double t = measure_median_us(op);
        writer.write_row({"vector", "clear", n, "demo", t});
    }
    {
        std::vector<int> std_prebuilt(prebuilt.begin(), prebuilt.end());
        auto op = [&]() {
            std::vector<int> v(std_prebuilt);
            v.clear();
            sink = v.size();
        };
        double t = measure_median_us(op);
        writer.write_row({"vector", "clear", n, "std", t});
    }
}

template <typename Vec>
void run_operator_at(Vec& prebuilt, csv_writer::CsvWriter& writer, std::size_t n)
{
    // 各位置随机访问
    {
        auto op = [&]() {
            long long sum = 0;
            for (std::size_t i = 0; i < n; ++i)
                sum += prebuilt[i];
            sink = sum;
        };
        double t = measure_median_us(op);
        writer.write_row({"vector", "operator_at", n, "demo", t});
    }
    {
        std::vector<int> std_prebuilt(prebuilt.begin(), prebuilt.end());
        auto op = [&]() {
            long long sum = 0;
            for (std::size_t i = 0; i < n; ++i)
                sum += std_prebuilt[i];
            sink = sum;
        };
        double t = measure_median_us(op);
        writer.write_row({"vector", "operator_at", n, "std", t});
    }
}

template <typename Vec>
void run_iterate(Vec& prebuilt, csv_writer::CsvWriter& writer, std::size_t n)
{
    // 正向遍历
    {
        auto op = [&]() {
            long long sum = 0;
            for (auto it = prebuilt.begin(); it != prebuilt.end(); ++it)
                sum += *it;
            sink = sum;
        };
        double t = measure_median_us(op);
        writer.write_row({"vector", "forward_iterate", n, "demo", t});
    }
    {
        std::vector<int> std_prebuilt(prebuilt.begin(), prebuilt.end());
        auto op = [&]() {
            long long sum = 0;
            for (auto it = std_prebuilt.begin(); it != std_prebuilt.end(); ++it)
                sum += *it;
            sink = sum;
        };
        double t = measure_median_us(op);
        writer.write_row({"vector", "forward_iterate", n, "std", t});
    }

    // 反向遍历
    {
        auto op = [&]() {
            long long sum = 0;
            for (auto it = prebuilt.rbegin(); it != prebuilt.rend(); ++it)
                sum += *it;
            sink = sum;
        };
        double t = measure_median_us(op);
        writer.write_row({"vector", "reverse_iterate", n, "demo", t});
    }
    {
        std::vector<int> std_prebuilt(prebuilt.begin(), prebuilt.end());
        auto op = [&]() {
            long long sum = 0;
            for (auto it = std_prebuilt.rbegin(); it != std_prebuilt.rend(); ++it)
                sum += *it;
            sink = sum;
        };
        double t = measure_median_us(op);
        writer.write_row({"vector", "reverse_iterate", n, "std", t});
    }
}

template <typename Vec>
void run_sort(Vec& prebuilt, csv_writer::CsvWriter& writer, std::size_t n)
{
    {
        auto op = [&]() {
            Vec v(prebuilt);
            std::sort(v.begin(), v.end());
            sink = v.size();
        };
        double t = measure_median_us(op);
        writer.write_row({"vector", "sort", n, "demo", t});
    }
    {
        std::vector<int> std_prebuilt(prebuilt.begin(), prebuilt.end());
        auto op = [&]() {
            std::vector<int> v(std_prebuilt);
            std::sort(v.begin(), v.end());
            sink = v.size();
        };
        double t = measure_median_us(op);
        writer.write_row({"vector", "sort", n, "std", t});
    }
}

} // anonymous namespace

void benchmark_vector(csv_writer::CsvWriter& writer)
{
    auto full_data = generate_ints(1000000);

    for (std::size_t n : benchmark_config::DATA_SIZES)
    {
        const auto& src = full_data; // 使用完整数据源

        // 构造
        demo::vector<int> demo_v;
        run_construct(demo_v, writer, src, n);

        // 预构建一个满的容器用于后续测试
        demo::vector<int> demo_prebuilt;
        for (std::size_t i = 0; i < n; ++i)
            demo_prebuilt.push_back(src[i]);

        // 拷贝构造
        run_copy_construct(demo_prebuilt, writer, n);

        // push_back
        run_push_back(demo_v, writer, src, n);

        // insert
        run_insert(demo_v, writer, src, n);

        // pop_back
        run_pop_back(demo_prebuilt, writer, n);

        // erase
        run_erase(demo_prebuilt, writer, n);

        // clear
        run_clear(demo_prebuilt, writer, n);

        // operator[]
        run_operator_at(demo_prebuilt, writer, n);

        // 遍历
        run_iterate(demo_prebuilt, writer, n);

        // sort (skip for 1M to avoid excessive time)
        if (n <= 100000)
        {
            run_sort(demo_prebuilt, writer, n);
        }
    }
}