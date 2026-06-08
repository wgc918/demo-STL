#include <stack>
#include <vector>
#include <random>

#include "stack/stack.h"
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

template <typename S>
void run_push(S&, csv_writer::CsvWriter& writer, const std::vector<int>& src, std::size_t n)
{
    {
        auto op = [&]() {
            S s;
            for (std::size_t i = 0; i < n; ++i)
                s.push(src[i]);
            sink = s.size();
        };
        double t = measure_median_us(op);
        writer.write_row({"stack", "push", n, "demo", t});
    }
    {
        auto op = [&]() {
            std::stack<int> s;
            for (std::size_t i = 0; i < n; ++i)
                s.push(src[i]);
            sink = s.size();
        };
        double t = measure_median_us(op);
        writer.write_row({"stack", "push", n, "std", t});
    }
}

template <typename S>
void run_pop(S& prebuilt, csv_writer::CsvWriter& writer, std::size_t n)
{
    {
        auto op = [&]() {
            S s(prebuilt);
            for (std::size_t i = 0; i < n && !s.empty(); ++i)
                s.pop();
            sink = s.size();
        };
        double t = measure_median_us(op);
        writer.write_row({"stack", "pop", n, "demo", t});
    }
    {
        std::stack<int> std_prebuilt;
        {
            // copy elements from demo prebuilt
            S tmp(prebuilt);
            while (!tmp.empty())
            {
                std_prebuilt.push(tmp.top());
                tmp.pop();
            }
        }
        auto op = [&]() {
            std::stack<int> s(std_prebuilt);
            for (std::size_t i = 0; i < n && !s.empty(); ++i)
                s.pop();
            sink = s.size();
        };
        double t = measure_median_us(op);
        writer.write_row({"stack", "pop", n, "std", t});
    }
}

template <typename S>
void run_top(S& prebuilt, csv_writer::CsvWriter& writer, std::size_t n)
{
    (void)n;
    {
        auto op = [&]() {
            sink = prebuilt.top();
        };
        double t = measure_median_us(op);
        writer.write_row({"stack", "top", n, "demo", t});
    }
    {
        std::stack<int> std_prebuilt;
        {
            S tmp(prebuilt);
            while (!tmp.empty())
            {
                std_prebuilt.push(tmp.top());
                tmp.pop();
            }
        }
        auto op = [&]() {
            sink = std_prebuilt.top();
        };
        double t = measure_median_us(op);
        writer.write_row({"stack", "top", n, "std", t});
    }
}

} // anonymous namespace

void benchmark_stack(csv_writer::CsvWriter& writer)
{
    auto full_data = generate_ints(1000000);

    for (std::size_t n : benchmark_config::DATA_SIZES)
    {
        const auto& src = full_data;

        // 预构建
        demo::stack<int> demo_prebuilt;
        for (std::size_t i = 0; i < n; ++i)
            demo_prebuilt.push(src[i]);

        demo::stack<int> demo_s;
        run_push(demo_s, writer, src, n);
        run_pop(demo_prebuilt, writer, n);
        run_top(demo_prebuilt, writer, n);
    }
}