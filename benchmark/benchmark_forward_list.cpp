#include <forward_list>
#include <vector>
#include <algorithm>
#include <random>

#include "list/forward_list.h"
#include "benchmark_config.h"
#include "csv_writer.h"

#include "benchmark_utils.h"

namespace
{
volatile long long sink = 0;

std::vector<int> generate_ints(std::size_t n)
{
    std::vector<int> data(n);
    std::mt19937 rng(42);
    std::uniform_int_distribution<int> dist(1, 1000000);
    for (auto& x : data)
        x = dist(rng);
    return data;
}

template <typename FL>
void run_construct(FL&, csv_writer::CsvWriter& writer, const std::vector<int>& src,
                   std::size_t n)
{
    // 默认构造
    {
        auto op = [&]() {
            FL fl;
            sink = 0;
        };
        double t = measure_median_us(op);
        writer.write_row({"forward_list", "default_construct", n, "demo", t});
    }
    {
        auto op = [&]() {
            std::forward_list<int> fl;
            sink = 0;
        };
        double t = measure_median_us(op);
        writer.write_row({"forward_list", "default_construct", n, "std", t});
    }

    // 范围构造
    {
        auto op = [&]() {
            FL fl(src.begin(), src.begin() + static_cast<std::ptrdiff_t>(n));
            sink = 0;
        };
        double t = measure_median_us(op);
        writer.write_row({"forward_list", "range_construct", n, "demo", t});
    }
    {
        auto op = [&]() {
            std::forward_list<int> fl(src.begin(), src.begin() + static_cast<std::ptrdiff_t>(n));
            sink = 0;
        };
        double t = measure_median_us(op);
        writer.write_row({"forward_list", "range_construct", n, "std", t});
    }
}

template <typename FL>
void run_push_front(FL&, csv_writer::CsvWriter& writer, const std::vector<int>& src,
                    std::size_t n)
{
    {
        auto op = [&]() {
            FL fl;
            for (std::size_t i = 0; i < n; ++i)
                fl.push_front(src[i]);
            sink = 0;
        };
        double t = measure_median_us(op);
        writer.write_row({"forward_list", "push_front", n, "demo", t});
    }
    {
        auto op = [&]() {
            std::forward_list<int> fl;
            for (std::size_t i = 0; i < n; ++i)
                fl.push_front(src[i]);
            sink = 0;
        };
        double t = measure_median_us(op);
        writer.write_row({"forward_list", "push_front", n, "std", t});
    }
}

template <typename FL>
void run_insert_after(FL&, csv_writer::CsvWriter& writer, const std::vector<int>& src,
                      std::size_t n)
{
    std::size_t insert_count = n / 10 > 0 ? n / 10 : 1;
    {
        auto op = [&]() {
            FL fl;
            fl.push_front(0);
            for (std::size_t i = 0; i < insert_count; ++i)
                fl.insert_after(fl.before_begin(), src[i]);
            sink = 0;
        };
        double t = measure_median_us(op);
        writer.write_row({"forward_list", "insert_after", n, "demo", t});
    }
    {
        auto op = [&]() {
            std::forward_list<int> fl;
            fl.push_front(0);
            for (std::size_t i = 0; i < insert_count; ++i)
                fl.insert_after(fl.before_begin(), src[i]);
            sink = 0;
        };
        double t = measure_median_us(op);
        writer.write_row({"forward_list", "insert_after", n, "std", t});
    }
}

template <typename FL>
void run_pop_front(FL& prebuilt, csv_writer::CsvWriter& writer, std::size_t n)
{
    {
        auto op = [&]() {
            FL fl(prebuilt);
            for (std::size_t i = 0; i < n && !fl.empty(); ++i)
                fl.pop_front();
            sink = 0;
        };
        double t = measure_median_us(op);
        writer.write_row({"forward_list", "pop_front", n, "demo", t});
    }
    {
        std::forward_list<int> std_prebuilt(prebuilt.begin(), prebuilt.end());
        auto op = [&]() {
            std::forward_list<int> fl(std_prebuilt);
            for (std::size_t i = 0; i < n && !fl.empty(); ++i)
                fl.pop_front();
            sink = 0;
        };
        double t = measure_median_us(op);
        writer.write_row({"forward_list", "pop_front", n, "std", t});
    }
}

template <typename FL>
void run_clear(FL& prebuilt, csv_writer::CsvWriter& writer, std::size_t n)
{
    {
        auto op = [&]() {
            FL fl(prebuilt);
            fl.clear();
            sink = 0;
        };
        double t = measure_median_us(op);
        writer.write_row({"forward_list", "clear", n, "demo", t});
    }
    {
        std::forward_list<int> std_prebuilt(prebuilt.begin(), prebuilt.end());
        auto op = [&]() {
            std::forward_list<int> fl(std_prebuilt);
            fl.clear();
            sink = 0;
        };
        double t = measure_median_us(op);
        writer.write_row({"forward_list", "clear", n, "std", t});
    }
}

template <typename FL>
void run_iterate(FL& prebuilt, csv_writer::CsvWriter& writer, std::size_t n)
{
    {
        auto op = [&]() {
            long long sum = 0;
            for (auto it = prebuilt.begin(); it != prebuilt.end(); ++it)
                sum += *it;
            sink = sum;
        };
        double t = measure_median_us(op);
        writer.write_row({"forward_list", "forward_iterate", n, "demo", t});
    }
    {
        std::forward_list<int> std_prebuilt(prebuilt.begin(), prebuilt.end());
        auto op = [&]() {
            long long sum = 0;
            for (auto it = std_prebuilt.begin(); it != std_prebuilt.end(); ++it)
                sum += *it;
            sink = sum;
        };
        double t = measure_median_us(op);
        writer.write_row({"forward_list", "forward_iterate", n, "std", t});
    }
}

template <typename FL>
void run_sort(FL& prebuilt, csv_writer::CsvWriter& writer, std::size_t n)
{
    {
        auto op = [&]() {
            FL fl(prebuilt);
            fl.sort();
            sink = 0;
        };
        double t = measure_median_us(op);
        writer.write_row({"forward_list", "sort", n, "demo", t});
    }
    {
        std::forward_list<int> std_prebuilt(prebuilt.begin(), prebuilt.end());
        auto op = [&]() {
            std::forward_list<int> fl(std_prebuilt);
            fl.sort();
            sink = 0;
        };
        double t = measure_median_us(op);
        writer.write_row({"forward_list", "sort", n, "std", t});
    }
}

} // anonymous namespace

void benchmark_forward_list(csv_writer::CsvWriter& writer)
{
    auto full_data = generate_ints(1000000);

    for (std::size_t n : benchmark_config::DATA_SIZES)
    {
        const auto& src = full_data;

        demo::forward_list<int> demo_fl;
        run_construct(demo_fl, writer, src, n);

        // 预构建
        demo::forward_list<int> demo_prebuilt;
        for (std::size_t i = n; i > 0; --i)
            demo_prebuilt.push_front(src[i - 1]);

        run_push_front(demo_fl, writer, src, n);
        run_insert_after(demo_fl, writer, src, n);
        run_pop_front(demo_prebuilt, writer, n);
        run_clear(demo_prebuilt, writer, n);
        run_iterate(demo_prebuilt, writer, n);

        if (n <= 100000)
        {
            run_sort(demo_prebuilt, writer, n);
        }
    }
}