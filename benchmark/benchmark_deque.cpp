#include <deque>
#include <vector>
#include <algorithm>
#include <random>

#include "deque/deque.h"
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

template <typename DQ>
void run_construct(DQ&, csv_writer::CsvWriter& writer, const std::vector<int>& src,
                   std::size_t n)
{
    // 默认构造
    {
        auto op = [&]() {
            DQ dq;
            sink = dq.size();
        };
        double t = measure_median_us(op);
        writer.write_row({"deque", "default_construct", n, "demo", t});
    }
    {
        auto op = [&]() {
            std::deque<int> dq;
            sink = dq.size();
        };
        double t = measure_median_us(op);
        writer.write_row({"deque", "default_construct", n, "std", t});
    }

    // 范围构造
    {
        auto op = [&]() {
            DQ dq(src.begin(), src.begin() + static_cast<std::ptrdiff_t>(n));
            sink = dq.size();
        };
        double t = measure_median_us(op);
        writer.write_row({"deque", "range_construct", n, "demo", t});
    }
    {
        auto op = [&]() {
            std::deque<int> dq(src.begin(), src.begin() + static_cast<std::ptrdiff_t>(n));
            sink = dq.size();
        };
        double t = measure_median_us(op);
        writer.write_row({"deque", "range_construct", n, "std", t});
    }
}

template <typename DQ>
void run_push_back(DQ&, csv_writer::CsvWriter& writer, const std::vector<int>& src,
                   std::size_t n)
{
    {
        auto op = [&]() {
            DQ dq;
            for (std::size_t i = 0; i < n; ++i)
                dq.push_back(src[i]);
            sink = dq.size();
        };
        double t = measure_median_us(op);
        writer.write_row({"deque", "push_back", n, "demo", t});
    }
    {
        auto op = [&]() {
            std::deque<int> dq;
            for (std::size_t i = 0; i < n; ++i)
                dq.push_back(src[i]);
            sink = dq.size();
        };
        double t = measure_median_us(op);
        writer.write_row({"deque", "push_back", n, "std", t});
    }
}

template <typename DQ>
void run_push_front(DQ&, csv_writer::CsvWriter& writer, const std::vector<int>& src,
                    std::size_t n)
{
    {
        auto op = [&]() {
            DQ dq;
            for (std::size_t i = 0; i < n; ++i)
                dq.push_front(src[i]);
            sink = dq.size();
        };
        double t = measure_median_us(op);
        writer.write_row({"deque", "push_front", n, "demo", t});
    }
    {
        auto op = [&]() {
            std::deque<int> dq;
            for (std::size_t i = 0; i < n; ++i)
                dq.push_front(src[i]);
            sink = dq.size();
        };
        double t = measure_median_us(op);
        writer.write_row({"deque", "push_front", n, "std", t});
    }
}

template <typename DQ>
void run_insert(DQ&, csv_writer::CsvWriter& writer, const std::vector<int>& src,
                std::size_t n)
{
    std::size_t insert_count = n / 10 > 0 ? n / 10 : 1;
    {
        auto op = [&]() {
            DQ dq;
            for (std::size_t i = 0; i < insert_count; ++i)
                dq.insert(dq.begin() + static_cast<std::ptrdiff_t>(dq.size() / 2), src[i]);
            sink = dq.size();
        };
        double t = measure_median_us(op);
        writer.write_row({"deque", "insert_middle", n, "demo", t});
    }
    {
        auto op = [&]() {
            std::deque<int> dq;
            for (std::size_t i = 0; i < insert_count; ++i)
                dq.insert(dq.begin() + static_cast<std::ptrdiff_t>(dq.size() / 2), src[i]);
            sink = dq.size();
        };
        double t = measure_median_us(op);
        writer.write_row({"deque", "insert_middle", n, "std", t});
    }
}

template <typename DQ>
void run_pop_back(DQ& prebuilt, csv_writer::CsvWriter& writer, std::size_t n)
{
    {
        auto op = [&]() {
            DQ dq(prebuilt);
            for (std::size_t i = 0; i < n && !dq.empty(); ++i)
                dq.pop_back();
            sink = dq.size();
        };
        double t = measure_median_us(op);
        writer.write_row({"deque", "pop_back", n, "demo", t});
    }
    {
        std::deque<int> std_prebuilt(prebuilt.begin(), prebuilt.end());
        auto op = [&]() {
            std::deque<int> dq(std_prebuilt);
            for (std::size_t i = 0; i < n && !dq.empty(); ++i)
                dq.pop_back();
            sink = dq.size();
        };
        double t = measure_median_us(op);
        writer.write_row({"deque", "pop_back", n, "std", t});
    }
}

template <typename DQ>
void run_pop_front(DQ& prebuilt, csv_writer::CsvWriter& writer, std::size_t n)
{
    {
        auto op = [&]() {
            DQ dq(prebuilt);
            for (std::size_t i = 0; i < n && !dq.empty(); ++i)
                dq.pop_front();
            sink = dq.size();
        };
        double t = measure_median_us(op);
        writer.write_row({"deque", "pop_front", n, "demo", t});
    }
    {
        std::deque<int> std_prebuilt(prebuilt.begin(), prebuilt.end());
        auto op = [&]() {
            std::deque<int> dq(std_prebuilt);
            for (std::size_t i = 0; i < n && !dq.empty(); ++i)
                dq.pop_front();
            sink = dq.size();
        };
        double t = measure_median_us(op);
        writer.write_row({"deque", "pop_front", n, "std", t});
    }
}

template <typename DQ>
void run_clear(DQ& prebuilt, csv_writer::CsvWriter& writer, std::size_t n)
{
    {
        auto op = [&]() {
            DQ dq(prebuilt);
            dq.clear();
            sink = dq.size();
        };
        double t = measure_median_us(op);
        writer.write_row({"deque", "clear", n, "demo", t});
    }
    {
        std::deque<int> std_prebuilt(prebuilt.begin(), prebuilt.end());
        auto op = [&]() {
            std::deque<int> dq(std_prebuilt);
            dq.clear();
            sink = dq.size();
        };
        double t = measure_median_us(op);
        writer.write_row({"deque", "clear", n, "std", t});
    }
}

template <typename DQ>
void run_operator_at(DQ& prebuilt, csv_writer::CsvWriter& writer, std::size_t n)
{
    {
        auto op = [&]() {
            long long sum = 0;
            for (std::size_t i = 0; i < n; ++i)
                sum += prebuilt[i];
            sink = sum;
        };
        double t = measure_median_us(op);
        writer.write_row({"deque", "operator_at", n, "demo", t});
    }
    {
        std::deque<int> std_prebuilt(prebuilt.begin(), prebuilt.end());
        auto op = [&]() {
            long long sum = 0;
            for (std::size_t i = 0; i < n; ++i)
                sum += std_prebuilt[i];
            sink = sum;
        };
        double t = measure_median_us(op);
        writer.write_row({"deque", "operator_at", n, "std", t});
    }
}

template <typename DQ>
void run_iterate(DQ& prebuilt, csv_writer::CsvWriter& writer, std::size_t n)
{
    {
        auto op = [&]() {
            long long sum = 0;
            for (auto it = prebuilt.begin(); it != prebuilt.end(); ++it)
                sum += *it;
            sink = sum;
        };
        double t = measure_median_us(op);
        writer.write_row({"deque", "forward_iterate", n, "demo", t});
    }
    {
        std::deque<int> std_prebuilt(prebuilt.begin(), prebuilt.end());
        auto op = [&]() {
            long long sum = 0;
            for (auto it = std_prebuilt.begin(); it != std_prebuilt.end(); ++it)
                sum += *it;
            sink = sum;
        };
        double t = measure_median_us(op);
        writer.write_row({"deque", "forward_iterate", n, "std", t});
    }

    {
        auto op = [&]() {
            long long sum = 0;
            for (auto it = prebuilt.rbegin(); it != prebuilt.rend(); ++it)
                sum += *it;
            sink = sum;
        };
        double t = measure_median_us(op);
        writer.write_row({"deque", "reverse_iterate", n, "demo", t});
    }
    {
        std::deque<int> std_prebuilt(prebuilt.begin(), prebuilt.end());
        auto op = [&]() {
            long long sum = 0;
            for (auto it = std_prebuilt.rbegin(); it != std_prebuilt.rend(); ++it)
                sum += *it;
            sink = sum;
        };
        double t = measure_median_us(op);
        writer.write_row({"deque", "reverse_iterate", n, "std", t});
    }
}

} // anonymous namespace

void benchmark_deque(csv_writer::CsvWriter& writer)
{
    auto full_data = generate_ints(1000000);

    for (std::size_t n : benchmark_config::DATA_SIZES)
    {
        const auto& src = full_data;

        demo::deque<int> demo_dq;
        run_construct(demo_dq, writer, src, n);

        // 预构建
        demo::deque<int> demo_prebuilt;
        for (std::size_t i = 0; i < n; ++i)
            demo_prebuilt.push_back(src[i]);

        run_push_back(demo_dq, writer, src, n);
        run_push_front(demo_dq, writer, src, n);
        run_insert(demo_dq, writer, src, n);
        run_pop_back(demo_prebuilt, writer, n);
        run_pop_front(demo_prebuilt, writer, n);
        run_clear(demo_prebuilt, writer, n);
        run_operator_at(demo_prebuilt, writer, n);
        run_iterate(demo_prebuilt, writer, n);
    }
}