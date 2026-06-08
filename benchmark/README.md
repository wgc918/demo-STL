# demo-STL Benchmark

demo-STL 容器与标准库性能对比 benchmark 套件。

## 目录结构

```
benchmark/
├── CMakeLists.txt              # 统一构建所有 benchmark
├── benchmark_config.h          # 公共配置（数据规模、重复次数等）
├── benchmark_utils.h           # 计时工具（中位数、预热）
├── csv_writer.h                # CSV 输出工具
├── benchmark_vector.cpp        # vector
├── benchmark_forward_list.cpp  # forward_list
├── benchmark_list.cpp          # list
├── benchmark_deque.cpp         # deque
├── benchmark_map.cpp           # map
├── benchmark_multimap.cpp      # multimap
├── benchmark_set.cpp           # set
├── benchmark_multiset.cpp      # multiset
├── benchmark_unordered_map.cpp         # unordered_map
├── benchmark_unordered_multimap.cpp    # unordered_multimap
├── benchmark_unordered_set.cpp         # unordered_set
├── benchmark_unordered_multiset.cpp    # unordered_multiset
├── benchmark_stack.cpp         # stack
├── benchmark_queue.cpp         # queue
├── benchmark_priority_queue.cpp # priority_queue
├── main.cpp                    # 入口，依次调用所有 benchmark
├── results/                    # CSV 输出目录（自动创建）
│   └── *.csv
├── visualize/                  # Python 可视化脚本
│   ├── requirements.txt
│   ├── plot_all.py             # 主脚本：读取 CSV 生成所有图表
│   ├── plot_per_container.py   # 按容器独立绘图
│   ├── demo_vs_std.py          # 整体对比图
│   └── output/                 # 图表输出目录（自动创建）
└── README.md
```

## 构建与运行

### 构建

```bash
cd benchmark/build
cmake ..
cmake --build . --config Release
```

### 运行

```bash
cd benchmark/build/Release
./benchmark.exe
```

CSV 结果将输出到 `benchmark/results/` 目录。

### 可视化

```bash
cd benchmark/visualize
pip install -r requirements.txt
python plot_all.py
python plot_per_container.py
python demo_vs_std.py
```

图表将输出到 `benchmark/visualize/output/` 目录。

## 测试维度

每个容器覆盖以下操作：

| 操作类别 | 具体操作 | 适用容器 |
| --- | --- | --- |
| 构造 | 默认构造、范围构造、拷贝构造 | 全部 |
| 插入 | push_back, push_front, insert | 根据容器特性 |
| 删除 | pop_back, pop_front, erase, clear | 根据容器特性 |
| 查找 | find, operator[] | 关联容器/vector/deque |
| 遍历 | 正向迭代器、反向迭代器 | 全部 |
| 排序 | sort (list/forward_list 成员函数) | list/forward_list |
| 适配器 | push/pop/top/front/back | 适配器 |

## 数据规模

- 小规模：1,000
- 中规模：10,000
- 大规模：100,000
- 超大规模：1,000,000（仅 O(n) 及更优操作）

## CSV 格式

| 字段 | 说明 |
| --- | --- |
| Container | 容器名 |
| Operation | 操作名 |
| DataSize | 数据规模 |
| Implementation | demo 或 std |
| TimeUs | 耗时（微秒） |
