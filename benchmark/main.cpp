#include <filesystem>
#include <iostream>
#include <string>
#include <vector>

#include "benchmark_config.h"
#include "benchmark_utils.h"
#include "csv_writer.h"

// 声明所有 benchmark 函数
void benchmark_vector(csv_writer::CsvWriter& writer);
void benchmark_forward_list(csv_writer::CsvWriter& writer);
void benchmark_list(csv_writer::CsvWriter& writer);
void benchmark_deque(csv_writer::CsvWriter& writer);
void benchmark_map(csv_writer::CsvWriter& writer);
void benchmark_multimap(csv_writer::CsvWriter& writer);
void benchmark_set(csv_writer::CsvWriter& writer);
void benchmark_multiset(csv_writer::CsvWriter& writer);
void benchmark_unordered_map(csv_writer::CsvWriter& writer);
void benchmark_unordered_multimap(csv_writer::CsvWriter& writer);
void benchmark_unordered_set(csv_writer::CsvWriter& writer);
void benchmark_unordered_multiset(csv_writer::CsvWriter& writer);
void benchmark_stack(csv_writer::CsvWriter& writer);
void benchmark_queue(csv_writer::CsvWriter& writer);
void benchmark_priority_queue(csv_writer::CsvWriter& writer);

int main()
{
    std::cout << "Starting benchmark..." << std::endl;

    // 确定 results 目录路径（相对于可执行文件位置或源码目录）
    std::filesystem::path results_dir = benchmark_config::RESULTS_DIR;
    if (!std::filesystem::exists(results_dir))
    {
        std::filesystem::create_directories(results_dir);
    }
    std::cout << "Results directory: " << std::filesystem::absolute(results_dir) << std::endl;

    // 创建 results 目录下的所有 CSV 文件
    auto make_path = [&](const char* filename) -> std::string
    { return (results_dir / filename).string(); };

    csv_writer::CsvWriter writer_vec(make_path("benchmark_vector.csv"));
    std::cout << "vector CSV opened: " << writer_vec.is_open() << std::endl;
    csv_writer::CsvWriter writer_fl(make_path("benchmark_forward_list.csv"));
    csv_writer::CsvWriter writer_list(make_path("benchmark_list.csv"));
    csv_writer::CsvWriter writer_deque(make_path("benchmark_deque.csv"));
    csv_writer::CsvWriter writer_map(make_path("benchmark_map.csv"));
    csv_writer::CsvWriter writer_mmap(make_path("benchmark_multimap.csv"));
    csv_writer::CsvWriter writer_set(make_path("benchmark_set.csv"));
    csv_writer::CsvWriter writer_mset(make_path("benchmark_multiset.csv"));
    csv_writer::CsvWriter writer_umap(make_path("benchmark_unordered_map.csv"));
    csv_writer::CsvWriter writer_ummap(make_path("benchmark_unordered_multimap.csv"));
    csv_writer::CsvWriter writer_uset(make_path("benchmark_unordered_set.csv"));
    csv_writer::CsvWriter writer_umset(make_path("benchmark_unordered_multiset.csv"));
    csv_writer::CsvWriter writer_stack(make_path("benchmark_stack.csv"));
    csv_writer::CsvWriter writer_queue(make_path("benchmark_queue.csv"));
    csv_writer::CsvWriter writer_pq(make_path("benchmark_priority_queue.csv"));

    std::cout << "Running vector benchmark..." << std::endl;
    // 顺序容器
    benchmark_vector(writer_vec);
    std::cout << "Vector done. Running forward_list..." << std::endl;
    benchmark_forward_list(writer_fl);
    std::cout << "Forward_list done. Running list..." << std::endl;
    benchmark_list(writer_list);
    std::cout << "List done. Running deque..." << std::endl;
    benchmark_deque(writer_deque);

    std::cout << "Running map benchmarks..." << std::endl;
    // 有序关联容器
    benchmark_map(writer_map);
    benchmark_multimap(writer_mmap);
    benchmark_set(writer_set);
    benchmark_multiset(writer_mset);

    std::cout << "Running unordered benchmarks..." << std::endl;
    // 无序关联容器
    benchmark_unordered_map(writer_umap);
    benchmark_unordered_multimap(writer_ummap);
    benchmark_unordered_set(writer_uset);
    benchmark_unordered_multiset(writer_umset);

    std::cout << "Running adapter benchmarks..." << std::endl;
    // 容器适配器
    benchmark_stack(writer_stack);
    benchmark_queue(writer_queue);
    benchmark_priority_queue(writer_pq);

    std::cout << "All benchmarks completed." << std::endl;
    return 0;
}