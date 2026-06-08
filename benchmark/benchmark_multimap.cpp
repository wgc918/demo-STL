#include <map>
#include <vector>
#include <string>
#include <random>

#include "multimap/multimap.h"
#include "benchmark_config.h"
#include "csv_writer.h"

#include "benchmark_utils.h"

namespace
{
volatile long long sink = 0;

std::vector<std::pair<int, std::string>> generate_pairs(std::size_t n)
{
    std::vector<std::pair<int, std::string>> data;
    data.reserve(n);
    std::mt19937 rng(42);
    std::uniform_int_distribution<int> dist(1, n / 5 + 1); // 允许重复键
    for (std::size_t i = 0; i < n; ++i)
        data.emplace_back(dist(rng), "value_" + std::to_string(i));
    return data;
}

template <typename M>
void run_construct(M&, csv_writer::CsvWriter& writer,
                   const std::vector<std::pair<int, std::string>>& src, std::size_t n)
{
    {
        auto op = [&]() {
            M m;
            sink = m.size();
        };
        double t = measure_median_us(op);
        writer.write_row({"multimap", "default_construct", n, "demo", t});
    }
    {
        auto op = [&]() {
            std::multimap<int, std::string> m;
            sink = m.size();
        };
        double t = measure_median_us(op);
        writer.write_row({"multimap", "default_construct", n, "std", t});
    }

    {
        auto op = [&]() {
            M m(src.begin(), src.begin() + static_cast<std::ptrdiff_t>(n));
            sink = m.size();
        };
        double t = measure_median_us(op);
        writer.write_row({"multimap", "range_construct", n, "demo", t});
    }
    {
        auto op = [&]() {
            std::multimap<int, std::string> m(src.begin(),
                                               src.begin() + static_cast<std::ptrdiff_t>(n));
            sink = m.size();
        };
        double t = measure_median_us(op);
        writer.write_row({"multimap", "range_construct", n, "std", t});
    }
}

template <typename M>
void run_insert(M&, csv_writer::CsvWriter& writer,
                const std::vector<std::pair<int, std::string>>& src, std::size_t n)
{
    {
        auto op = [&]() {
            M m;
            for (std::size_t i = 0; i < n; ++i)
                m.insert(src[i]);
            sink = m.size();
        };
        double t = measure_median_us(op);
        writer.write_row({"multimap", "insert", n, "demo", t});
    }
    {
        auto op = [&]() {
            std::multimap<int, std::string> m;
            for (std::size_t i = 0; i < n; ++i)
                m.insert(src[i]);
            sink = m.size();
        };
        double t = measure_median_us(op);
        writer.write_row({"multimap", "insert", n, "std", t});
    }
}

template <typename M>
void run_find(M& prebuilt, csv_writer::CsvWriter& writer, std::size_t n)
{
    {
        auto op = [&]() {
            long long sum = 0;
            for (std::size_t i = 0; i < n; ++i)
            {
                auto it = prebuilt.find(static_cast<int>(i));
                if (it != prebuilt.end())
                    sum += 1;
            }
            sink = sum;
        };
        double t = measure_median_us(op);
        writer.write_row({"multimap", "find", n, "demo", t});
    }
    {
        std::multimap<int, std::string> std_prebuilt(prebuilt.begin(), prebuilt.end());
        auto op = [&]() {
            long long sum = 0;
            for (std::size_t i = 0; i < n; ++i)
            {
                auto it = std_prebuilt.find(static_cast<int>(i));
                if (it != std_prebuilt.end())
                    sum += 1;
            }
            sink = sum;
        };
        double t = measure_median_us(op);
        writer.write_row({"multimap", "find", n, "std", t});
    }
}

template <typename M>
void run_erase(M& prebuilt, csv_writer::CsvWriter& writer, std::size_t n)
{
    std::size_t erase_count = n / 10 > 0 ? n / 10 : 1;
    {
        auto op = [&]() {
            M m(prebuilt);
            for (std::size_t i = 0; i < erase_count; ++i)
            {
                auto it = m.begin();
                if (it != m.end())
                    m.erase(it);
            }
            sink = m.size();
        };
        double t = measure_median_us(op);
        writer.write_row({"multimap", "erase", n, "demo", t});
    }
    {
        std::multimap<int, std::string> std_prebuilt(prebuilt.begin(), prebuilt.end());
        auto op = [&]() {
            std::multimap<int, std::string> m(std_prebuilt);
            for (std::size_t i = 0; i < erase_count; ++i)
            {
                auto it = m.begin();
                if (it != m.end())
                    m.erase(it);
            }
            sink = m.size();
        };
        double t = measure_median_us(op);
        writer.write_row({"multimap", "erase", n, "std", t});
    }
}

template <typename M>
void run_clear(M& prebuilt, csv_writer::CsvWriter& writer, std::size_t n)
{
    {
        auto op = [&]() {
            M m(prebuilt);
            m.clear();
            sink = m.size();
        };
        double t = measure_median_us(op);
        writer.write_row({"multimap", "clear", n, "demo", t});
    }
    {
        std::multimap<int, std::string> std_prebuilt(prebuilt.begin(), prebuilt.end());
        auto op = [&]() {
            std::multimap<int, std::string> m(std_prebuilt);
            m.clear();
            sink = m.size();
        };
        double t = measure_median_us(op);
        writer.write_row({"multimap", "clear", n, "std", t});
    }
}

template <typename M>
void run_iterate(M& prebuilt, csv_writer::CsvWriter& writer, std::size_t n)
{
    {
        auto op = [&]() {
            long long sum = 0;
            for (auto it = prebuilt.begin(); it != prebuilt.end(); ++it)
                sum += it->first;
            sink = sum;
        };
        double t = measure_median_us(op);
        writer.write_row({"multimap", "forward_iterate", n, "demo", t});
    }
    {
        std::multimap<int, std::string> std_prebuilt(prebuilt.begin(), prebuilt.end());
        auto op = [&]() {
            long long sum = 0;
            for (auto it = std_prebuilt.begin(); it != std_prebuilt.end(); ++it)
                sum += it->first;
            sink = sum;
        };
        double t = measure_median_us(op);
        writer.write_row({"multimap", "forward_iterate", n, "std", t});
    }
}

} // anonymous namespace

void benchmark_multimap(csv_writer::CsvWriter& writer)
{
    auto full_data = generate_pairs(100000);

    for (std::size_t n : benchmark_config::DATA_SIZES)
    {
        if (n > 100000) continue;
        const auto& src = full_data;

        demo::multimap<int, std::string> demo_m;
        run_construct(demo_m, writer, src, n);

        demo::multimap<int, std::string> demo_prebuilt;
        for (std::size_t i = 0; i < n; ++i)
            demo_prebuilt.insert(src[i]);

        run_insert(demo_m, writer, src, n);
        run_find(demo_prebuilt, writer, n);
        run_erase(demo_prebuilt, writer, n);
        run_clear(demo_prebuilt, writer, n);
        run_iterate(demo_prebuilt, writer, n);
    }
}