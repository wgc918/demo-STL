#include <set>
#include <vector>
#include <algorithm>
#include <random>

#include "set/set.h"
#include "benchmark_config.h"
#include "csv_writer.h"

#include "benchmark_utils.h"

namespace
{
volatile long long sink = 0;

std::vector<int> generate_ints(std::size_t n)
{
    std::vector<int> data;
    data.reserve(n);
    std::mt19937 rng(42);
    std::uniform_int_distribution<int> dist(1, 10000000);
    for (std::size_t i = 0; i < n; ++i)
        data.push_back(dist(rng));
    return data;
}

template <typename S>
void run_construct(S&, csv_writer::CsvWriter& writer, const std::vector<int>& src,
                   std::size_t n)
{
    {
        auto op = [&]() {
            S s;
            sink = s.size();
        };
        double t = measure_median_us(op);
        writer.write_row({"set", "default_construct", n, "demo", t});
    }
    {
        auto op = [&]() {
            std::set<int> s;
            sink = s.size();
        };
        double t = measure_median_us(op);
        writer.write_row({"set", "default_construct", n, "std", t});
    }

    {
        auto op = [&]() {
            S s(src.begin(), src.begin() + static_cast<std::ptrdiff_t>(n));
            sink = s.size();
        };
        double t = measure_median_us(op);
        writer.write_row({"set", "range_construct", n, "demo", t});
    }
    {
        auto op = [&]() {
            std::set<int> s(src.begin(), src.begin() + static_cast<std::ptrdiff_t>(n));
            sink = s.size();
        };
        double t = measure_median_us(op);
        writer.write_row({"set", "range_construct", n, "std", t});
    }
}

template <typename S>
void run_insert(S&, csv_writer::CsvWriter& writer, const std::vector<int>& src, std::size_t n)
{
    {
        auto op = [&]() {
            S s;
            for (std::size_t i = 0; i < n; ++i)
                s.insert(src[i]);
            sink = s.size();
        };
        double t = measure_median_us(op);
        writer.write_row({"set", "insert", n, "demo", t});
    }
    {
        auto op = [&]() {
            std::set<int> s;
            for (std::size_t i = 0; i < n; ++i)
                s.insert(src[i]);
            sink = s.size();
        };
        double t = measure_median_us(op);
        writer.write_row({"set", "insert", n, "std", t});
    }
}

template <typename S>
void run_find(S& prebuilt, csv_writer::CsvWriter& writer, std::size_t n)
{
    {
        auto op = [&]() {
            long long sum = 0;
            for (std::size_t i = 0; i < n; ++i)
            {
                if (prebuilt.find(static_cast<int>(i)) != prebuilt.end())
                    sum += 1;
            }
            sink = sum;
        };
        double t = measure_median_us(op);
        writer.write_row({"set", "find", n, "demo", t});
    }
    {
        std::set<int> std_prebuilt(prebuilt.begin(), prebuilt.end());
        auto op = [&]() {
            long long sum = 0;
            for (std::size_t i = 0; i < n; ++i)
            {
                if (std_prebuilt.find(static_cast<int>(i)) != std_prebuilt.end())
                    sum += 1;
            }
            sink = sum;
        };
        double t = measure_median_us(op);
        writer.write_row({"set", "find", n, "std", t});
    }
}

template <typename S>
void run_erase(S& prebuilt, csv_writer::CsvWriter& writer, std::size_t n)
{
    std::size_t erase_count = n / 10 > 0 ? n / 10 : 1;
    {
        auto op = [&]() {
            S s(prebuilt);
            for (std::size_t i = 0; i < erase_count; ++i)
            {
                auto it = s.begin();
                if (it != s.end())
                    s.erase(it);
            }
            sink = s.size();
        };
        double t = measure_median_us(op);
        writer.write_row({"set", "erase", n, "demo", t});
    }
    {
        std::set<int> std_prebuilt(prebuilt.begin(), prebuilt.end());
        auto op = [&]() {
            std::set<int> s(std_prebuilt);
            for (std::size_t i = 0; i < erase_count; ++i)
            {
                auto it = s.begin();
                if (it != s.end())
                    s.erase(it);
            }
            sink = s.size();
        };
        double t = measure_median_us(op);
        writer.write_row({"set", "erase", n, "std", t});
    }
}

template <typename S>
void run_clear(S& prebuilt, csv_writer::CsvWriter& writer, std::size_t n)
{
    {
        auto op = [&]() {
            S s(prebuilt);
            s.clear();
            sink = s.size();
        };
        double t = measure_median_us(op);
        writer.write_row({"set", "clear", n, "demo", t});
    }
    {
        std::set<int> std_prebuilt(prebuilt.begin(), prebuilt.end());
        auto op = [&]() {
            std::set<int> s(std_prebuilt);
            s.clear();
            sink = s.size();
        };
        double t = measure_median_us(op);
        writer.write_row({"set", "clear", n, "std", t});
    }
}

template <typename S>
void run_iterate(S& prebuilt, csv_writer::CsvWriter& writer, std::size_t n)
{
    {
        auto op = [&]() {
            long long sum = 0;
            for (auto it = prebuilt.begin(); it != prebuilt.end(); ++it)
                sum += *it;
            sink = sum;
        };
        double t = measure_median_us(op);
        writer.write_row({"set", "forward_iterate", n, "demo", t});
    }
    {
        std::set<int> std_prebuilt(prebuilt.begin(), prebuilt.end());
        auto op = [&]() {
            long long sum = 0;
            for (auto it = std_prebuilt.begin(); it != std_prebuilt.end(); ++it)
                sum += *it;
            sink = sum;
        };
        double t = measure_median_us(op);
        writer.write_row({"set", "forward_iterate", n, "std", t});
    }
}

} // anonymous namespace

void benchmark_set(csv_writer::CsvWriter& writer)
{
    auto full_data = generate_ints(100000);

    for (std::size_t n : benchmark_config::DATA_SIZES)
    {
        if (n > 100000) continue;
        const auto& src = full_data;

        demo::set<int> demo_s;
        run_construct(demo_s, writer, src, n);

        demo::set<int> demo_prebuilt;
        for (std::size_t i = 0; i < n; ++i)
            demo_prebuilt.insert(src[i]);

        run_insert(demo_s, writer, src, n);
        run_find(demo_prebuilt, writer, n);
        run_erase(demo_prebuilt, writer, n);
        run_clear(demo_prebuilt, writer, n);
        run_iterate(demo_prebuilt, writer, n);
    }
}