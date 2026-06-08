## 任务：为 demo-STL 所有容器及适配器新增 benchmark 并与标准库做性能对比

### 背景

demo-STL 是一个自定义 STL 容器库，位于 `h:\demo-STL\`，实现了以下全部容器和适配器：

**顺序容器：** vector, forward_list, list, deque
**有序关联容器：** map, multimap, set, multiset
**无序关联容器：** unordered_map, unordered_multimap, unordered_set, unordered_multiset
**容器适配器：** stack, queue, priority_queue

项目使用 CMake + doctest，各容器头文件位于 `h:\demo-STL\<容器名>\<容器名>.h`（例如 `vector/vector.h`，`map/map.h`）。

### 目标

在 `h:\demo-STL\benchmark\` 目录下新建一套完整的 benchmark 体系，对每个容器/适配器执行以下典型操作的性能测试，并与 `std::` 标准库对应实现进行对比。

### 具体要求

#### 1. 目录结构

h:\demo-STL\benchmark
├── CMakeLists.txt # 统一构建所有 benchmark
├── benchmark_config.h # 公共配置（数据规模、重复次数等）
├── csv_writer.h # CSV 输出工具
├── benchmark_vector.cpp
├── benchmark_forward_list.cpp
├── benchmark_list.cpp
├── benchmark_deque.cpp
├── benchmark_map.cpp
├── benchmark_multimap.cpp
├── benchmark_set.cpp
├── benchmark_multiset.cpp
├── benchmark_unordered_map.cpp
├── benchmark_unordered_multimap.cpp
├── benchmark_unordered_set.cpp
├── benchmark_unordered_multiset.cpp
├── benchmark_stack.cpp
├── benchmark_queue.cpp
├── benchmark_priority_queue.cpp
├── main.cpp # 入口，依次调用所有 benchmark
├── results/ # CSV 输出目录（自动创建）
│ └── \*.csv
├── visualize/ # Python 可视化脚本
│ ├── requirements.txt
│ ├── plot_all.py # 主脚本：读取 CSV 生成所有图表
│ └── plot_per_container.py # 按容器独立绘图
└── README.md

#### 2. Benchmark 设计规范

##### 2.1 测试维度

每个容器至少覆盖以下操作（根据容器特性选择适用项）：

| 操作类别   | 具体操作                                                                         | 适用容器              |
| ---------- | -------------------------------------------------------------------------------- | --------------------- |
| 构造       | 默认构造、N个元素构造、范围构造、拷贝构造                                        | 全部                  |
| 插入       | push_back, push_front, insert(任意位置), emplace, emplace_back                   | 根据容器特性          |
| 删除       | pop_back, pop_front, erase(任意位置), clear                                      | 根据容器特性          |
| 查找       | find, operator[], at, count, contains                                            | 关联容器/vector/deque |
| 遍历       | 正向迭代器遍历、反向迭代器遍历                                                   | 全部                  |
| 排序       | sort（list/forward_list 成员函数）                                               | list/forward_list     |
| 适配器操作 | push/pop/top (stack), push/pop/front/back (queue), push/pop/top (priority_queue) | 适配器                |

##### 2.2 数据规模

使用 3-4 个量级的数据规模来观察复杂度趋势：

- **小规模：** 1,000
- **中规模：** 10,000
- **大规模：** 100,000
- **超大规模：** 1,000,000（仅对 O(n) 及更优操作，如 push_back、遍历等；O(n log n) 操作可选择性使用）

##### 2.3 计时方式

- 使用 `std::chrono::high_resolution_clock`
- 每个操作重复执行多次（如 10-50 次），取**中位数**或**平均值**，消除噪声
- 预热：正式计时前先执行一轮避免冷启动偏差

##### 2.4 对比方式

每次 benchmark 同时测试 `demo::xxx` 和 `std::xxx`，输出对比数据。

#### 3. CSV 输出格式

每个 benchmark 文件输出一个 CSV，格式如下：
Container,Operation,DataSize,Implementation,TimeUs
vector,push_back,1000,demo,12.5
vector,push_back,1000,std,10.2
vector,push_back,10000,demo,145.3
vector,push_back,10000,std,130.1
...

- `Container`: 容器名（vector, list, map, ...）
- `Operation`: 操作名（push_back, insert, find, ...）
- `DataSize`: 数据规模
- `Implementation`: demo 或 std
- `TimeUs`: 耗时（微秒）

所有 CSV 汇总输出到 `benchmark/results/` 目录。

#### 4. Python 可视化脚本

##### 4.1 `plot_all.py`

- 读取 `results/` 下所有 CSV
- 为每个容器的每个操作生成独立的柱状图（demo vs std × 各数据规模）
- 生成一张**总览图**：所有容器的核心操作相对性能（demo/std 比值），用热力图或分组柱状图展示
- 图表使用 matplotlib 或 plotly，中文标注无乱码
- 输出到 `benchmark/visualize/output/` 目录

##### 4.2 `plot_per_container.py`

- 对每个容器单独生成一页多子图（所有操作 × 所有数据规模）
- 方便单独查看某个容器的完整性能画像

##### 4.3 `demo_vs_std.py`

- 一张图可视化所有容器、适配器demo和std在整体上的对比

##### 4.4 图表要求

- 柱状图：demo vs std 并排对比，不同数据规模分面（facet）或用不同颜色
- X 轴：数据规模，Y 轴：耗时（微秒），对数坐标可选
- 每个图表标题清晰：`{容器名} - {操作名} 性能对比`
- 保存为 PNG，DPI ≥ 150

#### 5. 代码规范

- 使用 C++17 标准
- 编译优化级别：`-O2`（GCC/Clang）或 `/O2`（MSVC）
- 使用 `<chrono>` 进行高精度计时
- 避免编译器将 benchmark 代码优化掉（使用 `volatile` 或 `benchmark::DoNotOptimize` 等价手段）
- 使用 doctest 作为 runner（可选），或者直接用 `main()` 调度
- CSV 写入尽量简单，可以直接用 `std::ofstream`，避免引入第三方 CSV 库
- 所有 benchmark 文件只包含对应容器的头文件，不引入无关依赖
