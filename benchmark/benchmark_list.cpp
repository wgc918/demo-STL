#include <list>
#include <vector>
#include <algorithm>
#include <random>

#include "list/list.h"
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

template <typename L>
void run_construct(L&, csv_writer::CsvWriter& writer, const std::vector<int>& src,
                   std::size_t n)
{
    // 默认构造
    {
        auto op = [&]() {
            L l;
            sink = l.size();
        };
        double t = measure_median_us(op);
        writer.write_row({"list", "default_construct", n, "demo", t});
    }
    {
        auto op = [&]() {
            std::list<int> l;
            sink = l.size();
        };
        double t = measure_median_us(op);
        writer.write_row({"list", "default_construct", n, "std", t});
    }

    // 范围构造
    {
        auto op = [&]() {
            L l(src.begin(), src.begin() + static_cast<std::ptrdiff_t>(n));
            sink = l.size();
        };
        double t = measure_median_us(op);
        writer.write_row({"list", "range_construct", n, "demo", t});
    }
    {
        auto op = [&]() {
            std::list<int> l(src.begin(), src.begin() + static_cast<std::ptrdiff_t>(n));
            sink = l.size();
        };
        double t = measure_median_us(op);
        writer.write_row({"list", "range_construct", n, "std", t});
    }
}

template <typename L>
void run_push_back(L&, csv_writer::CsvWriter& writer, const std::vector<int>& src,
                   std::size_t n)
{
    {
        auto op = [&]() {
            L l;
            for (std::size_t i = 0; i < n; ++i)
                l.push_back(src[i]);
            sink = l.size();
        };
        double t = measure_median_us(op);
        writer.write_row({"list", "push_back", n, "demo", t});
    }
    {
        auto op = [&]() {
            std::list<int> l;
            for (std::size_t i = 0; i < n; ++i)
                l.push_back(src[i]);
            sink = l.size();
        };
        double t = measure_median_us(op);
        writer.write_row({"list", "push_back", n, "std", t});
    }
}

template <typename L>
void run_push_front(L&, csv_writer::CsvWriter& writer, const std::vector<int>& src,
                    std::size_t n)
{
    {
        auto op = [&]() {
            L l;
            for (std::size_t i = 0; i < n; ++i)
                l.push_front(src[i]);
            sink = l.size();
        };
        double t = measure_median_us(op);
        writer.write_row({"list", "push_front", n, "demo", t});
    }
    {
        auto op = [&]() {
            std::list<int> l;
            for (std::size_t i = 0; i < n; ++i)
                l.push_front(src[i]);
            sink = l.size();
        };
        double t = measure_median_us(op);
        writer.write_row({"list", "push_front", n, "std", t});
    }
}

template <typename L>
void run_insert(L&, csv_writer::CsvWriter& writer, const std::vector<int>& src,
                std::size_t n)
{
    std::size_t insert_count = n / 10 > 0 ? n / 10 : 1;
    {
        auto op = [&]() {
            L l;
            l.push_back(0);
            for (std::size_t i = 0; i < insert_count; ++i)
                l.insert(l.begin(), src[i]);
            sink = l.size();
        };
        double t = measure_median_us(op);
        writer.write_row({"list", "insert", n, "demo", t});
    }
    {
        auto op = [&]() {
            std::list<int> l;
            l.push_back(0);
            for (std::size_t i = 0; i < insert_count; ++i)
                l.insert(l.begin(), src[i]);
            sink = l.size();
        };
        double t = measure_median_us(op);
        writer.write_row({"list", "insert", n, "std", t});
    }
}

template <typename L>
void run_pop_back(L& prebuilt, csv_writer::CsvWriter& writer, std::size_t n)
{
    {
        auto op = [&]() {
            L l(prebuilt);
            for (std::size_t i = 0; i < n && !l.empty(); ++i)
                l.pop_back();
            sink = l.size();
        };
        double t = measure_median_us(op);
        writer.write_row({"list", "pop_back", n, "demo", t});
    }
    {
        std::list<int> std_prebuilt(prebuilt.begin(), prebuilt.end());
        auto op = [&]() {
            std::list<int> l(std_prebuilt);
            for (std::size_t i = 0; i < n && !l.empty(); ++i)
                l.pop_back();
            sink = l.size();
        };
        double t = measure_median_us(op);
        writer.write_row({"list", "pop_back", n, "std", t});
    }
}

template <typename L>
void run_pop_front(L& prebuilt, csv_writer::CsvWriter& writer, std::size_t n)
{
    {
        auto op = [&]() {
            L l(prebuilt);
            for (std::size_t i = 0; i < n && !l.empty(); ++i)
                l.pop_front();
            sink = l.size();
        };
        double t = measure_median_us(op);
        writer.write_row({"list", "pop_front", n, "demo", t});
    }
    {
        std::list<int> std_prebuilt(prebuilt.begin(), prebuilt.end());
        auto op = [&]() {
            std::list<int> l(std_prebuilt);
            for (std::size_t i = 0; i < n && !l.empty(); ++i)
                l.pop_front();
            sink = l.size();
        };
        double t = measure_median_us(op);
        writer.write_row({"list", "pop_front", n, "std", t});
    }
}

template <typename L>
void run_clear(L& prebuilt, csv_writer::CsvWriter& writer, std::size_t n)
{
    {
        auto op = [&]() {
            L l(prebuilt);
            l.clear();
            sink = l.size();
        };
        double t = measure_median_us(op);
        writer.write_row({"list", "clear", n, "demo", t});
    }
    {
        std::list<int> std_prebuilt(prebuilt.begin(), prebuilt.end());
        auto op = [&]() {
            std::list<int> l(std_prebuilt);
            l.clear();
            sink = l.size();
        };
        double t = measure_median_us(op);
        writer.write_row({"list", "clear", n, "std", t});
    }
}

template <typename L>
void run_iterate(L& prebuilt, csv_writer::CsvWriter& writer, std::size_t n)
{
    {
        auto op = [&]() {
            long long sum = 0;
            for (auto it = prebuilt.begin(); it != prebuilt.end(); ++it)
                sum += *it;
            sink = sum;
        };
        double t = measure_median_us(op);
        writer.write_row({"list", "forward_iterate", n, "demo", t});
    }
    {
        std::list<int> std_prebuilt(prebuilt.begin(), prebuilt.end());
        auto op = [&]() {
            long long sum = 0;
            for (auto it = std_prebuilt.begin(); it != std_prebuilt.end(); ++it)
                sum += *it;
            sink = sum;
        };
        double t = measure_median_us(op);
        writer.write_row({"list", "forward_iterate", n, "std", t});
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
        writer.write_row({"list", "reverse_iterate", n, "demo", t});
    }
    {
        std::list<int> std_prebuilt(prebuilt.begin(), prebuilt.end());
        auto op = [&]() {
            long long sum = 0;
            for (auto it = std_prebuilt.rbegin(); it != std_prebuilt.rend(); ++it)
                sum += *it;
            sink = sum;
        };
        double t = measure_median_us(op);
        writer.write_row({"list", "reverse_iterate", n, "std", t});
    }
}

template <typename L>
void run_sort(L& prebuilt, csv_writer::CsvWriter& writer, std::size_t n)
{
    {
        auto op = [&]() {
            L l(prebuilt);
            l.sort();
            sink = l.size();
        };
        double t = measure_median_us(op);
        writer.write_row({"list", "sort", n, "demo", t});
    }
    {
        std::list<int> std_prebuilt(prebuilt.begin(), prebuilt.end());
        auto op = [&]() {
            std::list<int> l(std_prebuilt);
            l.sort();
            sink = l.size();
        };
        double t = measure_median_us(op);
        writer.write_row({"list", "sort", n, "std", t});
    }
}

} // anonymous namespace

void benchmark_list(csv_writer::CsvWriter& writer)
{
    auto full_data = generate_ints(1000000);

    for (std::size_t n : benchmark_config::DATA_SIZES)
    {
        const auto& src = full_data;

        demo::list<int> demo_l;
        run_construct(demo_l, writer, src, n);

        // 预构建
        demo::list<int> demo_prebuilt;
        for (std::size_t i = 0; i < n; ++i)
            demo_prebuilt.push_back(src[i]);

        run_push_back(demo_l, writer, src, n);
        run_push_front(demo_l, writer, src, n);
        run_insert(demo_l, writer, src, n);
        run_pop_back(demo_prebuilt, writer, n);
        run_pop_front(demo_prebuilt, writer, n);
        run_clear(demo_prebuilt, writer, n);
        run_iterate(demo_prebuilt, writer, n);

        if (n <= 100000)
        {
            run_sort(demo_prebuilt, writer, n);
        }
    }
}