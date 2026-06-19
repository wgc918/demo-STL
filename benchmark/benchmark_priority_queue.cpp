#include <queue>
#include <random>
#include <vector>

#include "benchmark_config.h"
#include "benchmark_utils.h"
#include "csv_writer.h"
#include "priority_queue/priority_queue.h"

namespace
{
volatile long long sink = 0;

std::vector<int> generate_ints(std::size_t n)
{
    std::vector<int>                   data(n);
    std::mt19937                       rng(42);
    std::uniform_int_distribution<int> dist(1, 1000000);
    for (auto& x : data)
        x = dist(rng);
    return data;
}

template <typename PQ>
void run_push(PQ&, csv_writer::CsvWriter& writer, const std::vector<int>& src, std::size_t n)
{
    {
        auto op = [&]()
        {
            PQ pq;
            for (std::size_t i = 0; i < n; ++i)
                pq.push(src[i]);
            sink = pq.size();
        };
        double t = measure_median_us(op);
        writer.write_row({"priority_queue", "push", n, "demo", t});
    }
    {
        auto op = [&]()
        {
            std::priority_queue<int> pq;
            for (std::size_t i = 0; i < n; ++i)
                pq.push(src[i]);
            sink = pq.size();
        };
        double t = measure_median_us(op);
        writer.write_row({"priority_queue", "push", n, "std", t});
    }
}

template <typename PQ>
void run_pop(PQ& prebuilt, csv_writer::CsvWriter& writer, std::size_t n)
{
    {
        auto op = [&]()
        {
            PQ pq(prebuilt);
            for (std::size_t i = 0; i < n && !pq.empty(); ++i)
                pq.pop();
            sink = pq.size();
        };
        double t = measure_median_us(op);
        writer.write_row({"priority_queue", "pop", n, "demo", t});
    }
    {
        // build std pq from demo prebuilt
        std::priority_queue<int> std_prebuilt;
        {
            PQ tmp(prebuilt);
            while (!tmp.empty())
            {
                std_prebuilt.push(tmp.top());
                tmp.pop();
            }
        }
        auto op = [&]()
        {
            std::priority_queue<int> pq(std_prebuilt);
            for (std::size_t i = 0; i < n && !pq.empty(); ++i)
                pq.pop();
            sink = pq.size();
        };
        double t = measure_median_us(op);
        writer.write_row({"priority_queue", "pop", n, "std", t});
    }
}

template <typename PQ>
void run_top(PQ& prebuilt, csv_writer::CsvWriter& writer, std::size_t n)
{
    (void)n;
    {
        auto   op = [&]() { sink = prebuilt.top(); };
        double t  = measure_median_us(op);
        writer.write_row({"priority_queue", "top", n, "demo", t});
    }
    {
        std::priority_queue<int> std_prebuilt;
        {
            PQ tmp(prebuilt);
            while (!tmp.empty())
            {
                std_prebuilt.push(tmp.top());
                tmp.pop();
            }
        }
        auto   op = [&]() { sink = std_prebuilt.top(); };
        double t  = measure_median_us(op);
        writer.write_row({"priority_queue", "top", n, "std", t});
    }
}

}  // anonymous namespace

void benchmark_priority_queue(csv_writer::CsvWriter& writer)
{
    auto full_data = generate_ints(1000000);

    for (std::size_t n : benchmark_config::DATA_SIZES)
    {
        if (n > 100000)
            continue;  // skip 1M for pq
        const auto& src = full_data;

        demo::priority_queue<int> demo_prebuilt;
        for (std::size_t i = 0; i < n; ++i)
            demo_prebuilt.push(src[i]);

        demo::priority_queue<int> demo_pq;
        run_push(demo_pq, writer, src, n);
        run_pop(demo_prebuilt, writer, n);
        run_top(demo_prebuilt, writer, n);
    }
}