#include <queue>
#include <vector>
#include <random>

#include "queue/queue.h"
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

template <typename Q>
void run_push(Q&, csv_writer::CsvWriter& writer, const std::vector<int>& src, std::size_t n)
{
    {
        auto op = [&]() {
            Q q;
            for (std::size_t i = 0; i < n; ++i)
                q.push(src[i]);
            sink = q.size();
        };
        double t = measure_median_us(op);
        writer.write_row({"queue", "push", n, "demo", t});
    }
    {
        auto op = [&]() {
            std::queue<int> q;
            for (std::size_t i = 0; i < n; ++i)
                q.push(src[i]);
            sink = q.size();
        };
        double t = measure_median_us(op);
        writer.write_row({"queue", "push", n, "std", t});
    }
}

template <typename Q>
void run_pop(Q& prebuilt, csv_writer::CsvWriter& writer, std::size_t n)
{
    {
        auto op = [&]() {
            Q q(prebuilt);
            for (std::size_t i = 0; i < n && !q.empty(); ++i)
                q.pop();
            sink = q.size();
        };
        double t = measure_median_us(op);
        writer.write_row({"queue", "pop", n, "demo", t});
    }
    {
        // build std queue from demo prebuilt
        std::queue<int> std_prebuilt;
        {
            Q tmp(prebuilt);
            while (!tmp.empty())
            {
                std_prebuilt.push(tmp.front());
                tmp.pop();
            }
        }
        auto op = [&]() {
            std::queue<int> q(std_prebuilt);
            for (std::size_t i = 0; i < n && !q.empty(); ++i)
                q.pop();
            sink = q.size();
        };
        double t = measure_median_us(op);
        writer.write_row({"queue", "pop", n, "std", t});
    }
}

template <typename Q>
void run_front(Q& prebuilt, csv_writer::CsvWriter& writer, std::size_t n)
{
    (void)n;
    {
        auto op = [&]() {
            sink = prebuilt.front();
        };
        double t = measure_median_us(op);
        writer.write_row({"queue", "front", n, "demo", t});
    }
    {
        std::queue<int> std_prebuilt;
        {
            Q tmp(prebuilt);
            while (!tmp.empty())
            {
                std_prebuilt.push(tmp.front());
                tmp.pop();
            }
        }
        auto op = [&]() {
            sink = std_prebuilt.front();
        };
        double t = measure_median_us(op);
        writer.write_row({"queue", "front", n, "std", t});
    }
}

template <typename Q>
void run_back(Q& prebuilt, csv_writer::CsvWriter& writer, std::size_t n)
{
    (void)n;
    {
        auto op = [&]() {
            sink = prebuilt.back();
        };
        double t = measure_median_us(op);
        writer.write_row({"queue", "back", n, "demo", t});
    }
    {
        std::queue<int> std_prebuilt;
        {
            Q tmp(prebuilt);
            while (!tmp.empty())
            {
                std_prebuilt.push(tmp.front());
                tmp.pop();
            }
        }
        auto op = [&]() {
            sink = std_prebuilt.back();
        };
        double t = measure_median_us(op);
        writer.write_row({"queue", "back", n, "std", t});
    }
}

} // anonymous namespace

void benchmark_queue(csv_writer::CsvWriter& writer)
{
    auto full_data = generate_ints(1000000);

    for (std::size_t n : benchmark_config::DATA_SIZES)
    {
        const auto& src = full_data;

        demo::queue<int> demo_prebuilt;
        for (std::size_t i = 0; i < n; ++i)
            demo_prebuilt.push(src[i]);

        demo::queue<int> demo_q;
        run_push(demo_q, writer, src, n);
        run_pop(demo_prebuilt, writer, n);
        run_front(demo_prebuilt, writer, n);
        run_back(demo_prebuilt, writer, n);
    }
}