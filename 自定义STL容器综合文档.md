# 自定义STL容器综合文档

# 文档概述

本文档是自定义实现STL（Standard Template Library）容器的**总览入口**，旨在汇总所有已实现的容器类型，简要说明各容器的核心定位、设计目标及适用场景，同时提供明确的跳转指引，帮助开发者快速定位到对应容器的详细开发文档，实现"总览-详情"的高效联动。

本文档适配所有自定义STL容器，后续新增容器将同步更新本总览；所有容器均遵循C++17及以上标准，接口设计对齐STL标准库，保证兼容性和易用性，底层均采用RAII设计原则，确保内存安全，支持自定义分配器（默认适配std::allocator）。

# 容器适配说明

- 编译环境：兼容GCC、Clang等主流编译器，需开启C++17及以上标准（编译参数添加-std=c++17）；
- 命名空间：所有自定义容器均位于`demo`命名空间下（如demo::vector），避免与标准库std命名空间冲突；
- 跳转规则：本文档中每个容器的"详细文档跳转"部分，均对应单独的容器详细开发文档（如vector对应vector开发文档），跳转方式可根据实际文件存储路径调整（本地文档直接跳转、工程内关联跳转等）；
- 测试支持：所有容器均配套完整测试用例（如vector的test_demo_vector_string测试函数），可直接运行验证功能完整性。

# 自定义STL容器总览

当前已实现的自定义STL容器涵盖"序列式容器"类别，后续将逐步扩展关联容器、适配器等，所有容器均实现核心功能，接口对齐STL标准，同时补充异常安全、性能优化等特性，以下是各容器详细简介及跳转指引。

## 一、序列式容器（Sequence Containers）

序列式容器按照**插入顺序**存储元素，支持随机访问或顺序访问，核心特点是元素有序且可重复，适配大多数基础场景（如元素存储、遍历、增删改查）。

### 1. demo::vector（动态数组容器）

**核心定位**：最常用的序列式容器，底层基于动态数组实现，支持随机访问、动态扩容，兼顾高效访问和灵活增删（尾部增删效率最优），是自定义STL容器的基础实现。

**设计目标**：对齐std::vector接口，解决标准库vector的底层实现透明化需求，优化内存分配策略，补充异常安全机制，适配复杂元素类型（如std::string、自定义类）。

**核心特性**：

- 支持随机访问（operator[]、at()接口），时间复杂度O(1)；
- 动态扩容：默认扩容因子为(capacity + 2) \* 2，空容器初始扩容至4，避免频繁扩容；
- 完整的元素生命周期管理：通过std::allocator_traits封装分配器，统一处理内存申请/释放、元素构造/析构；
- 支持拷贝构造、移动构造、赋值运算符重载，实现高效拷贝和移动语义；
- 补充异常安全：越界访问、非法迭代器操作均抛出std::out_of_range异常，便于调试。

**适用场景**：需要频繁随机访问元素、尾部增删元素的场景（如数据存储、遍历展示、批量处理），替代std::vector用于底层实现学习和定制化需求。

**详细文档跳转**：跳转至[demo::vector容器开发文档](./vector/vector.md)

### 2. demo::forward_list（单向链表容器）

**核心定位**：轻量级序列式容器，底层基于单向链表实现，仅支持前向顺序访问，核心优势是**极低的内存开销**和**高效的前端 / 任意位置插入删除操作**（时间复杂度 O (1)），相比双向链表省去反向指针，内存占用更少。

**设计目标**：对齐 std::forward_list 接口，解决双向链表（list）内存开销高的问题，实现单向链表的节点管理、前向迭代器设计，补充异常安全机制和高效的链表操作（排序、合并、去重）。

**核心特性**：

- 仅支持前向迭代器（std::forward_iterator_tag），无反向遍历、随机访问能力；
- 前端增删（push_front/pop_front）、指定位置后增删（insert_after/erase_after）时间复杂度 O (1)，仅调整指针无需移动元素；
- 前哨节点设计：避免空链表边界处理，简化插入 / 删除逻辑；
- 高效链表操作：内置归并排序（sort）、有序合并（merge）、去重（unique）、反转（reverse）等接口，时间复杂度符合链表最优特性；
- 移动语义优化：支持 emplace_front/emplace_after 原地构造元素，避免拷贝 / 移动开销；
- 异常安全：空链表调用 pop_front 抛 std::out_of_range 异常，内存分配失败抛 std::bad_alloc 异常。

**适用场景**：需要频繁插入 / 删除元素、无需反向遍历、追求低内存开销的轻量级场景（如轻量级数据链表、队列底层实现、高频插入的日志存储）。

**详细文档跳转**：跳转至[demo::forward_list 容器开发文档](./list/forward_list.md)

### 3. demo::list（双向链表容器）

**核心定位**：底层基于双向链表实现，支持双向顺序访问，核心优势是**任意位置增删效率最优**（无需移动元素），牺牲随机访问能力换取灵活增删，相比单向链表（forward_list）支持反向遍历和首尾两端高效操作。

**设计目标**：对齐std::list接口，解决vector中间位置增删效率低的问题，实现双向链表的节点管理、双向指针操作，补充异常安全机制和迭代器稳定性（增删元素不影响其他迭代器有效性）。

**核心特性**：

- 支持双向访问（begin()/end()、rbegin()/rend()），顺序访问时间复杂度O(1)，随机访问O(n)；
- 任意位置插入/删除元素时间复杂度O(1)（仅需调整节点指针，无需移动元素）；
- 首尾增删效率最优（push_front/pop_front、push_back/pop_back），时间复杂度均为O(1)；
- 迭代器稳定性：插入/删除元素时，仅影响当前节点的迭代器，其他迭代器保持有效；
- 支持双向迭代器（std::bidirectional_iterator_tag），支持前置/后置自增、自减操作；
- 内置高效链表操作：归并排序（sort）、有序合并（merge）、拼接（splice）、反转（reverse）、去重（unique）等接口；
- 支持拷贝、移动语义，适配复杂元素类型，内存分配采用节点级分配（每个节点单独分配内存）；
- 异常安全：空链表调用pop_front/pop_back抛std::out_of_range异常，内存分配失败抛std::bad_alloc异常。

**适用场景**：需要频繁在任意位置增删元素、需要双向遍历、追求迭代器稳定性的场景（如有序容器、频繁更新的数据集、队列/双端队列底层实现）。

**详细文档跳转**：跳转至[demo::list容器开发文档](./list/list.md)。

### 4.demo::queue（队列容器）【待实现】

### 5. demo::deque（双端队列容器）

**核心定位**：底层基于**分段数组+中控器（Map）**实现的双端队列容器，兼顾 `vector` 的随机访问效率和 `list` 的双端增删效率，是两者的中间适配容器。`deque` 支持在首尾两端高效插入和删除元素，同时提供 O(1) 时间复杂度的随机访问能力。

**设计目标**：对齐 `std::deque` 接口，解决 `vector` 头部增删效率低、`list` 随机访问效率低的问题，优化分段内存分配策略，支持高效双端操作，补充异常安全机制。

**核心特性**：

- **随机访问支持**：支持 `operator[]` 和 `at()` 接口，时间复杂度 O(1)；
- **双端增删效率最优**：`push_front()`、`pop_front()`、`push_back()`、`pop_back()` 时间复杂度均摊 O(1)；
- **分段内存分配**：采用 Map+Buffer 二级结构，避免 `vector` 扩容时的大量元素移动；
- **随机访问迭代器**：支持完整的迭代器算术运算（`+`、`-`、`+=`、`-=`），迭代器内部维护当前 buffer 指针、元素偏移量和 buffer 边界信息；
- **异常安全保障**：空队列调用 `pop_front`/`pop_back`/`front`/`back` 抛 `std::out_of_range` 异常，`at()` 越界访问抛 `std::out_of_range` 异常；
- **移动语义优化**：支持 `emplace` 系列接口原地构造元素，避免拷贝/移动开销；
- **内存管理策略**：map 容量不足时动态扩容（容量翻倍），支持 `shrink_to_fit()` 释放未使用内存。

**核心数据结构设计原理**：

`deque` 采用**二级指针结构（Map + Buffer）**实现，核心组件包括：

- **Map（中控器）**：指向 Buffer 指针数组的指针，用于管理所有数据块；
- **Buffer（数据块）**：固定大小的连续内存区域，每个 Buffer 包含约 512 字节的元素；
- **m_buffer_size**：每个 Buffer 的元素数量，计算公式为 `512 / sizeof(value_type)`；
- **m_begin/m_end**：指向首尾元素的迭代器，维护 `deque` 的实际数据范围。

这种设计使得 `deque` 在两端扩展时只需操作 Map，无需移动现有元素；而随机访问时通过计算元素所在的 Buffer 索引和偏移量实现 O(1) 定位。

**常用操作方法及时间复杂度**：

| 操作方法                   | 功能描述         | 时间复杂度 | 说明              |
| :------------------------- | :--------------- | :--------- | :---------------- |
| `push_front(const T&)`     | 头部插入元素     | O(1)       | 可能触发 map 扩容 |
| `push_back(const T&)`      | 尾部插入元素     | O(1)       | 可能触发 map 扩容 |
| `pop_front()`              | 删除头部元素     | O(1)       | 空队列抛异常      |
| `pop_back()`               | 删除尾部元素     | O(1)       | 空队列抛异常      |
| `emplace_front(Args&&...)` | 头部原地构造元素 | O(1)       | 避免拷贝开销      |
| `emplace_back(Args&&...)`  | 尾部原地构造元素 | O(1)       | 避免拷贝开销      |
| `emplace(pos, Args&&...)`  | 指定位置原地构造 | O(n)       | 需要移动元素      |
| `insert(pos, const T&)`    | 指定位置插入元素 | O(n)       | 需要移动元素      |
| `erase(pos)`               | 删除指定位置元素 | O(n)       | 需要移动元素      |
| `operator[](size_type)`    | 随机访问元素     | O(1)       | 无越界检查        |
| `at(size_type)`            | 随机访问元素     | O(1)       | 有越界检查        |
| `front()`                  | 获取头部元素引用 | O(1)       | 空队列抛异常      |
| `back()`                   | 获取尾部元素引用 | O(1)       | 空队列抛异常      |
| `size()`                   | 返回元素个数     | O(1)       | 直接返回成员变量  |
| `empty()`                  | 判断是否为空     | O(1)       | 比较 size 与 0    |
| `clear()`                  | 清空容器         | O(n)       | 析构所有元素      |
| `swap(deque&)`             | 交换两个容器     | O(1)       | 交换内部指针      |
| `shrink_to_fit()`          | 释放多余内存     | O(n)       | 重新分配并拷贝    |

**使用示例代码**：

```cpp
#include <iostream>
#include "deque/deque.h"

int main() {
    // 创建 deque 容器
    demo::deque<int> dq{1, 2, 3, 4, 5};

    // 尾部插入元素
    dq.push_back(6);
    dq.emplace_back(7);  // 原地构造，更高效

    // 头部插入元素
    dq.push_front(0);
    dq.emplace_front(-1);

    // 随机访问
    std::cout << "第3个元素: " << dq[2] << std::endl;  // 输出: 0
    std::cout << "第5个元素: " << dq.at(4) << std::endl;  // 输出: 2

    // 遍历容器
    std::cout << "遍历 deque: ";
    for (const auto& val : dq) {
        std::cout << val << " ";
    }
    std::cout << std::endl;  // 输出: -1 0 1 2 3 4 5 6 7

    // 删除元素
    dq.pop_front();  // 删除 -1
    dq.pop_back();   // 删除 7

    // 反向遍历
    std::cout << "反向遍历: ";
    for (auto it = dq.rbegin(); it != dq.rend(); ++it) {
        std::cout << *it << " ";
    }
    std::cout << std::endl;  // 输出: 6 5 4 3 2 1 0

    return 0;
}
```

**与 vector、list 容器的功能对比及适用场景差异**：

| 特性              | demo::deque               | demo::vector           | demo::list             |
| :---------------- | :------------------------ | :--------------------- | :--------------------- |
| **随机访问**      | O(1)                      | O(1)（最优）           | O(n)                   |
| **头部插入/删除** | **O(1)**                  | O(n)（需移动所有元素） | O(1)                   |
| **尾部插入/删除** | **O(1)**                  | 均摊 O(1)              | O(1)                   |
| **中间插入/删除** | O(n)                      | O(n)                   | **O(1)**               |
| **内存布局**      | 分段连续（Buffer 内连续） | 单一连续内存           | 非连续（节点式）       |
| **缓存局部性**    | 同一 Buffer 内较优        | **最优**               | 较差                   |
| **迭代器类型**    | 随机访问迭代器            | 随机访问迭代器         | 双向迭代器             |
| **迭代器失效**    | 插入可能失效所有迭代器    | 扩容时失效所有迭代器   | 仅失效被删除节点       |
| **内存开销**      | 中等（Map + Buffer 开销） | 低（仅数组）           | 高（每个节点两个指针） |
| **扩容代价**      | 低（仅扩展 Map）          | 高（可能需整体拷贝）   | 无（按需分配节点）     |

**适用场景建议**：

| 场景             | 推荐容器            | 原因                       |
| :--------------- | :------------------ | :------------------------- |
| 需要频繁随机访问 | `vector`            | 连续内存，缓存友好         |
| 需要频繁两端增删 | `deque`             | O(1) 双端操作 + 随机访问   |
| 需要频繁中间增删 | `list`              | O(1) 任意位置操作          |
| 作为栈（LIFO）   | `deque` 或 `vector` | `deque` 更优（无扩容压力） |
| 作为队列（FIFO） | `deque`             | O(1) 双端操作              |
| 滑动窗口算法     | `deque`             | 高效的两端操作 + 随机访问  |

**内存管理机制**：

- **扩容策略**：当 map 容量不足以容纳新 Buffer 时，执行 map 扩容（容量翻倍），将现有 Buffer 移动到新 map 的中间位置，保持双向扩展能力；
- **Buffer 大小固定**：每个 Buffer 大小固定（约 512 字节），与元素类型大小无关，保证内存管理的一致性；
- **内存释放**：`clear()` 仅析构元素但不释放 Buffer，`shrink_to_fit()` 会释放多余的 Buffer 内存；
- **析构行为**：容器析构时会依次释放所有 Buffer，然后释放 Map 数组。

**迭代器实现原理**：

`deque` 的迭代器采用四级结构设计：

```cpp
class iterator {
    T* m_cur;        // 当前元素指针
    T* m_first;      // 当前 Buffer 起始指针
    T* m_last;       // 当前 Buffer 结束指针（不包含）
    T** m_map_node;  // 当前 Buffer 在 Map 中的位置
};
```

迭代器支持完整的随机访问操作：

- **前置/后置 ++/--**：在当前 Buffer 范围内直接移动，跨 Buffer 时更新 Map 索引；
- **算术运算**：通过计算目标位置所在的 Buffer 索引和偏移量实现 O(1) 随机访问；
- **比较运算**：先比较 Map 索引，再比较 Buffer 内偏移量。

**异常安全保证**：

- **强异常安全**：`push_front`、`push_back`、`emplace_front`、`emplace_back` 在异常发生时保证容器状态不变；
- **基本异常安全**：`insert`、`emplace`、`erase` 在异常发生时保证容器处于有效状态，但元素顺序可能改变；
- **不抛异常操作**：`swap`、`size`、`empty`、`operator[]`（无越界检查）保证不抛异常。

**详细文档跳转**：跳转至[demo::deque容器开发文档](./deque/deque.md)。

## 二、关联式容器（Associative Containers）

关联式容器按照**键值（key）**存储元素，元素自动排序（默认升序），支持基于键值的快速查找，核心特点是元素有序且不重复（或可重复，如multiset/multimap），适配查找频繁的场景。

### 1. demo::map（有序键值对容器）

**核心定位**：基于红黑树实现的关联容器，存储键值对（key-value pairs），键唯一且自动按升序排序，支持 O(log n) 时间复杂度的插入、删除和查找操作，是实现字典、配置管理等场景的理想选择。

**设计目标**：对齐 `std::map` 接口，实现红黑树的自平衡机制，提供高效的键值查找和有序遍历能力，补充异常安全机制和迭代器稳定性。

**核心特性**：

- **红黑树实现**：底层基于红黑树数据结构，保证最坏情况下 O(log n) 的时间复杂度；
- **键值对存储**：存储 `std::pair<const K, T>` 类型，键不可修改，值可修改；
- **键唯一性**：自动保证键的唯一性，重复键插入会被忽略；
- **有序遍历**：迭代器按中序遍历顺序访问元素（按键升序）；
- **高效查找**：提供 `find()`、`lower_bound()`、`upper_bound()`、`equal_range()` 等查找接口；
- **双向迭代器**：支持双向迭代器（`std::bidirectional_iterator_tag`），支持前置/后置自增、自减操作；
- **高效插入**：支持 `insert_or_assign()`（插入或赋值）、`try_emplace()`（原地构造）等高效插入接口；
- **移动语义优化**：支持移动构造、移动赋值，避免不必要的拷贝开销；
- **异常安全**：`at()` 访问不存在的键抛 `std::out_of_range` 异常，内存分配失败抛 `std::bad_alloc` 异常；
- **扩展接口**：支持从已排序的 `vector` 快速构建平衡树（`sorted_tag`），优化初始化效率。

**核心数据结构设计原理**：

`map` 采用红黑树（Red-Black Tree）实现，红黑树是一种自平衡二叉搜索树，通过维护节点颜色（红色/黑色）和旋转操作保持树的平衡，确保：

1. 根节点为黑色；
2. 每个红色节点的子节点均为黑色（红节点不相邻）；
3. 从任意节点到其所有叶子节点的路径包含相同数量的黑色节点；
4. 叶子节点（哨兵节点）为黑色。

这些性质保证了树的高度为 O(log n)，从而实现高效的查找、插入和删除操作。

**常用操作方法及时间复杂度**：

| 操作方法                   | 功能描述         | 时间复杂度 | 说明              |
| :------------------------- | :--------------- | :--------- | :---------------- |
| `insert(const value_type&)` | 插入键值对       | O(log n)   | 键已存在则忽略    |
| `emplace(key, args...)`    | 原地构造键值对   | O(log n)   | 避免拷贝开销      |
| `try_emplace(key, args...)`| 键不存在时插入   | O(log n)   | 键存在则不操作    |
| `insert_or_assign(key, val)`| 插入或覆盖值     | O(log n)   | 键存在则更新值    |
| `erase(key)`               | 删除指定键的元素 | O(log n)   | 返回下一个迭代器  |
| `erase(iterator)`          | 删除迭代器指向元素 | O(log n)  | 返回下一个迭代器  |
| `find(key)`                | 查找指定键       | O(log n)   | 返回迭代器        |
| `count(key)`               | 统计键出现次数   | O(log n)   | map 中返回 0 或 1 |
| `lower_bound(key)`         | 第一个不小于key的元素 | O(log n) | 返回迭代器        |
| `upper_bound(key)`         | 第一个大于key的元素 | O(log n) | 返回迭代器        |
| `equal_range(key)`         | 等于key的元素范围 | O(log n) | 返回迭代器对      |
| `operator[](key)`          | 访问或插入元素   | O(log n)   | 键不存在则插入    |
| `at(key)`                  | 访问元素（带检查） | O(log n) | 键不存在抛异常    |
| `size()`                   | 返回元素个数     | O(1)       | 直接返回成员变量  |
| `empty()`                  | 判断是否为空     | O(1)       | 比较根节点        |
| `clear()`                  | 清空容器         | O(n)       | 销毁所有节点      |
| `swap(map&)`               | 交换两个容器     | O(1)       | 交换内部指针      |

**使用示例代码**：

```cpp
#include <iostream>
#include "map/map.h"
#include <string>

int main() {
    // 创建 map 容器
    demo::map<std::string, int> m{{"apple", 1}, {"banana", 2}, {"cherry", 3}};

    // 插入元素
    m.insert({"date", 4});
    m.emplace("elderberry", 5);
    m["fig"] = 6;

    // 访问元素
    std::cout << "apple: " << m.at("apple") << std::endl;
    std::cout << "banana: " << m["banana"] << std::endl;

    // 查找元素
    auto it = m.find("cherry");
    if (it != m.end()) {
        std::cout << "Found: " << it->first << " -> " << it->second << std::endl;
    }

    // 有序遍历（按键升序）
    std::cout << "Forward traversal: ";
    for (const auto& [key, value] : m) {
        std::cout << key << ":" << value << " ";
    }
    std::cout << std::endl;

    // 反向遍历
    std::cout << "Reverse traversal: ";
    for (auto it = m.rbegin(); it != m.rend(); ++it) {
        std::cout << it->first << ":" << it->second << " ";
    }
    std::cout << std::endl;

    // 使用 lower_bound 和 upper_bound
    auto range = m.equal_range("cherry");
    std::cout << "Range size: " << std::distance(range.first, range.second) << std::endl;

    // 使用 insert_or_assign
    m.insert_or_assign("apple", 10);
    std::cout << "apple after insert_or_assign: " << m["apple"] << std::endl;

    // 使用 try_emplace
    m.try_emplace("grape", 7);
    m.try_emplace("apple", 100); // 键已存在，不覆盖
    std::cout << "apple after try_emplace: " << m["apple"] << std::endl;

    // 删除元素
    m.erase("banana");
    std::cout << "Size after erase: " << m.size() << std::endl;

    return 0;
}
```

**输出结果**：

```
apple: 1
banana: 2
Found: cherry -> 3
Forward traversal: apple:1 banana:2 cherry:3 date:4 elderberry:5 fig:6 
Reverse traversal: fig:6 elderberry:5 date:4 cherry:3 banana:2 apple:1 
Range size: 1
apple after insert_or_assign: 10
apple after try_emplace: 10
Size after erase: 5
```

**适用场景**：

| 场景             | 推荐使用 map 的原因                     |
| :--------------- | :------------------------------------- |
| 字典/配置管理    | 键值对存储，快速查找                   |
| 缓存系统         | 按键快速查找、插入、删除               |
| 有序数据存储     | 自动排序，支持有序遍历                 |
| 去重存储         | 键唯一性保证，自动去重                 |
| 范围查询         | `lower_bound()`/`upper_bound()` 高效范围查找 |

**详细文档跳转**：跳转至[demo::map容器开发文档](./map/map.md)

### 2. demo::unordered_map（无序键值对容器）

**核心定位**：基于哈希表实现的关联容器，存储键值对（key-value pairs），键唯一但**不保证任何特定顺序**，支持平均 O(1) 时间复杂度的插入、删除和查找操作，是实现高速查找、缓存系统等场景的理想选择。

**设计目标**：对齐 `std::unordered_map` 接口，实现哈希表的高效查找机制，提供平均常数时间复杂度的操作，补充异常安全机制和哈希策略管理。

**核心特性**：

- **哈希表实现**：底层基于链地址法（Separate Chaining）的哈希表，保证平均 O(1) 的时间复杂度；
- **键值对存储**：存储 `std::pair<const K, T>` 类型，键不可修改，值可修改；
- **键唯一性**：自动保证键的唯一性，重复键插入会被忽略；
- **无序遍历**：迭代器按桶顺序访问元素，不保证特定顺序，重哈希后顺序可能改变；
- **高效查找**：提供 `find()`、`count()`、`equal_range()` 等查找接口；
- **前向迭代器**：支持前向迭代器（`std::forward_iterator_tag`），支持前置/后置自增操作；
- **高效插入**：支持 `insert_or_assign()`（插入或赋值）、`try_emplace()`（原地构造）等高效插入接口；
- **哈希策略管理**：支持调整负载因子、重哈希、预留空间等操作；
- **桶接口**：支持 `bucket_count()`、`bucket_size()`、`bucket()` 等桶相关接口；
- **移动语义优化**：支持移动构造、移动赋值，避免不必要的拷贝开销；
- **异常安全**：`at()` 访问不存在的键抛 `std::out_of_range` 异常，内存分配失败抛 `std::bad_alloc` 异常。

**核心数据结构设计原理**：

`unordered_map` 采用**链地址法哈希表**实现，核心组件包括：

- **桶数组（m_table）**：存储节点指针的数组，每个桶指向一个链表；
- **节点（Node）**：存储键值对、哈希值缓存和指向下一个节点的指针；
- **负载因子（load_factor）**：元素数量与桶数量的比值，超过阈值时触发重哈希；
- **哈希函数（Hash）**：将键转换为哈希值，默认使用 `std::hash<Key>`；
- **键比较函数（KeyEqual）**：判断两个键是否相等，默认使用 `std::equal_to<Key>`。

**常用操作方法及时间复杂度**：

| 操作方法                   | 功能描述         | 时间复杂度 | 说明              |
| :------------------------- | :--------------- | :--------- | :---------------- |
| `insert(const value_type&)` | 插入键值对       | O(1) 平均  | 键已存在则忽略    |
| `emplace(key, args...)`    | 原地构造键值对   | O(1) 平均  | 避免拷贝开销      |
| `try_emplace(key, args...)`| 键不存在时插入   | O(1) 平均  | 键存在则不操作    |
| `insert_or_assign(key, val)`| 插入或覆盖值     | O(1) 平均  | 键存在则更新值    |
| `erase(key)`               | 删除指定键的元素 | O(1) 平均  | 返回删除数量(0或1)|
| `erase(iterator)`          | 删除迭代器指向元素 | O(1) 平均 | 返回下一个迭代器  |
| `find(key)`                | 查找指定键       | O(1) 平均  | 返回迭代器        |
| `count(key)`               | 统计键出现次数   | O(1) 平均  | 返回 0 或 1       |
| `equal_range(key)`         | 等于key的元素范围 | O(1) 平均 | 返回迭代器对      |
| `operator[](key)`          | 访问或插入元素   | O(1) 平均  | 键不存在则插入    |
| `at(key)`                  | 访问元素（带检查） | O(1) 平均 | 键不存在抛异常    |
| `size()`                   | 返回元素个数     | O(1)       | 直接返回成员变量  |
| `empty()`                  | 判断是否为空     | O(1)       | 比较 size 与 0    |
| `clear()`                  | 清空容器         | O(n)       | 销毁所有节点      |
| `swap(map&)`               | 交换两个容器     | O(1)       | 交换内部指针      |
| `rehash(n)`                | 重新哈希         | O(n)       | 重新分配桶数组    |
| `reserve(n)`               | 预留空间         | O(n)       | 可能触发 rehash   |

**使用示例代码**：

```cpp
#include <iostream>
#include "unordered_map/unordered_map.h"
#include <string>

int main() {
    // 创建 unordered_map 容器
    demo::unordered_map<std::string, int> m{{"apple", 1}, {"banana", 2}, {"cherry", 3}};

    // 插入元素
    m.insert({"date", 4});
    m.emplace("elderberry", 5);
    m["fig"] = 6;

    // 访问元素
    std::cout << "apple: " << m.at("apple") << std::endl;
    std::cout << "banana: " << m["banana"] << std::endl;

    // 查找元素
    auto it = m.find("cherry");
    if (it != m.end()) {
        std::cout << "Found: " << it->first << " -> " << it->second << std::endl;
    }

    // 遍历（无序）
    std::cout << "Traversal: ";
    for (const auto& [key, value] : m) {
        std::cout << key << ":" << value << " ";
    }
    std::cout << std::endl;

    // 使用 insert_or_assign
    m.insert_or_assign("apple", 10);
    std::cout << "apple after insert_or_assign: " << m["apple"] << std::endl;

    // 使用 try_emplace
    m.try_emplace("grape", 7);
    m.try_emplace("apple", 100); // 键已存在，不覆盖
    std::cout << "apple after try_emplace: " << m["apple"] << std::endl;

    // 删除元素
    m.erase("banana");
    std::cout << "Size after erase: " << m.size() << std::endl;

    // 查看哈希策略信息
    std::cout << "Bucket count: " << m.bucket_count() << std::endl;
    std::cout << "Load factor: " << m.load_factor() << std::endl;

    return 0;
}
```

**输出结果**：

```
apple: 1
banana: 2
Found: cherry -> 3
Traversal: apple:1 banana:2 cherry:3 date:4 elderberry:5 fig:6 
apple after insert_or_assign: 10
apple after try_emplace: 10
Size after erase: 5
Bucket count: 16
Load factor: 0.3125
```

**与 demo::map 的区别**：

| 特性              | demo::unordered_map       | demo::map               |
| :---------------- | :------------------------ | :---------------------- |
| **底层实现**      | 哈希表（链地址法）        | 红黑树                  |
| **元素顺序**      | 无序                      | 按键升序排序            |
| **查找复杂度**    | O(1) 平均，O(n) 最坏      | O(log n) 最坏           |
| **插入复杂度**    | O(1) 平均，O(n) 最坏      | O(log n) 最坏           |
| **删除复杂度**    | O(1) 平均，O(n) 最坏      | O(log n) 最坏           |
| **内存布局**      | 非连续（节点式+桶数组）   | 非连续（节点式）        |
| **迭代器类型**    | 前向迭代器                | 双向迭代器              |
| **内存开销**      | 较高（哈希表+链表）       | 较高（红黑树结构）      |
| **适用场景**      | 快速查找、不关心顺序      | 有序遍历、范围查询      |

**适用场景**：

| 场景             | 推荐使用 unordered_map 的原因           |
| :--------------- | :------------------------------------- |
| 缓存系统         | 高速查找、插入、删除                   |
| 字典/配置管理    | 键值对存储，快速查找                   |
| 快速索引         | O(1) 平均查找复杂度                   |
| 不关心顺序的场景 | 无序存储，无需排序开销                 |

**详细文档跳转**：跳转至[demo::unordered_map容器开发文档](./unordered_map/unordered_map.md)

### 3. demo::set（有序不重复集合容器）

**核心定位**：基于红黑树实现的关联容器，仅存储唯一的键（key），键即元素值，自动按升序排序，支持 O(log n) 时间复杂度的插入、删除和查找操作，是实现集合运算、去重处理等场景的理想选择。

**设计目标**：对齐 `std::set` 接口，实现红黑树的自平衡机制，提供高效的键查找和有序遍历能力，补充异常安全机制和迭代器稳定性。

**核心特性**：

- **红黑树实现**：底层基于红黑树数据结构，保证最坏情况下 O(log n) 的时间复杂度；
- **键唯一存储**：仅存储键，元素类型与键类型相同（`value_type = key_type`），自动保证键的唯一性；
- **有序遍历**：迭代器按中序遍历顺序访问元素（按键升序）；
- **高效查找**：提供 `find()`、`count()`、`lower_bound()`、`upper_bound()`、`equal_range()` 等查找接口；
- **双向迭代器**：支持双向迭代器（`std::bidirectional_iterator_tag`），支持前置/后置自增、自减操作；
- **高效插入**：支持 `emplace()`（原地构造）、`emplace_hint()`（提示位置插入）等高效插入接口；
- **移动语义优化**：支持移动构造、移动赋值，避免不必要的拷贝开销；
- **异常安全**：内存分配失败抛 `std::bad_alloc` 异常；
- **扩展接口**：支持从已排序的 `vector` 快速构建平衡树（`sorted_tag`），优化初始化效率。

**核心数据结构设计原理**：

`set` 采用红黑树（Red-Black Tree）实现，红黑树是一种自平衡二叉搜索树，通过维护节点颜色（红色/黑色）和旋转操作保持树的平衡，确保：

1. 根节点为黑色；
2. 每个红色节点的子节点均为黑色（红节点不相邻）；
3. 从任意节点到其所有叶子节点的路径包含相同数量的黑色节点；
4. 叶子节点（哨兵节点）为黑色。

这些性质保证了树的高度为 O(log n)，从而实现高效的查找、插入和删除操作。

**常用操作方法及时间复杂度**：

| 操作方法                   | 功能描述         | 时间复杂度 | 说明              |
| :------------------------- | :--------------- | :--------- | :---------------- |
| `insert(const value_type&)` | 插入键           | O(log n)   | 键已存在则忽略    |
| `insert(value_type&&)`      | 插入键（移动版本） | O(log n)  | 键已存在则忽略    |
| `emplace(args...)`         | 原地构造键并插入 | O(log n)   | 避免拷贝开销      |
| `emplace_hint(hint, args...)` | 在提示位置插入 | O(log n) 或 O(1) | 有效hint可优化为O(1) |
| `erase(key)`               | 删除指定键的元素 | O(log n)   | 返回删除数量(0或1)|
| `erase(iterator)`          | 删除迭代器指向元素 | O(log n)  | 返回下一个迭代器  |
| `erase(first, last)`       | 删除范围元素     | O(m log n) | m为删除元素个数   |
| `find(key)`                | 查找指定键       | O(log n)   | 返回迭代器        |
| `count(key)`               | 统计键出现次数   | O(log n)   | set中返回0或1     |
| `lower_bound(key)`         | 第一个不小于key的元素 | O(log n) | 返回迭代器        |
| `upper_bound(key)`         | 第一个大于key的元素 | O(log n) | 返回迭代器        |
| `equal_range(key)`         | 等于key的元素范围 | O(log n) | 返回迭代器对      |
| `size()`                   | 返回元素个数     | O(1)       | 直接返回成员变量  |
| `empty()`                  | 判断是否为空     | O(1)       | 比较根节点        |
| `clear()`                  | 清空容器         | O(n)       | 销毁所有节点      |
| `swap(set&)`               | 交换两个容器     | O(1)       | 交换内部指针      |

**使用示例代码**：

```cpp
#include <iostream>
#include "set/set.h"
#include <string>

int main() {
    // 创建 set 容器
    demo::set<std::string> s{"apple", "banana", "cherry"};

    // 插入元素
    s.insert("date");
    s.emplace("elderberry");
    
    // 尝试插入重复元素（会被忽略）
    auto [it, inserted] = s.insert("apple");
    std::cout << "apple inserted: " << std::boolalpha << inserted << std::endl; // false

    // 查找元素
    auto find_it = s.find("cherry");
    if (find_it != s.end()) {
        std::cout << "Found: " << *find_it << std::endl;
    }

    // 统计元素数量
    std::cout << "cherry count: " << s.count("cherry") << std::endl; // 1
    std::cout << "grape count: " << s.count("grape") << std::endl;   // 0

    // 有序遍历（按键升序）
    std::cout << "Forward traversal: ";
    for (const auto& key : s) {
        std::cout << key << " ";
    }
    std::cout << std::endl;

    // 反向遍历
    std::cout << "Reverse traversal: ";
    for (auto it = s.rbegin(); it != s.rend(); ++it) {
        std::cout << *it << " ";
    }
    std::cout << std::endl;

    // 使用 lower_bound 和 upper_bound
    auto lower = s.lower_bound("banana");
    auto upper = s.upper_bound("cherry");
    std::cout << "Range [banana, cherry): ";
    for (auto it = lower; it != upper; ++it) {
        std::cout << *it << " ";
    }
    std::cout << std::endl;

    // 删除元素
    s.erase("banana");
    std::cout << "Size after erase: " << s.size() << std::endl;

    // 清空 set
    s.clear();
    std::cout << "Is empty after clear: " << std::boolalpha << s.empty() << std::endl;

    return 0;
}
```

**输出结果**：

```
apple inserted: false
Found: cherry
cherry count: 1
grape count: 0
Forward traversal: apple banana cherry date elderberry 
Reverse traversal: elderberry date cherry banana apple 
Range [banana, cherry): banana cherry 
Size after erase: 4
Is empty after clear: true
```

**适用场景**：

| 场景             | 推荐使用 set 的原因                     |
| :--------------- | :------------------------------------- |
| 集合运算         | 支持交、并、差等集合操作                 |
| 去重处理         | 自动去重，键唯一性保证                   |
| 唯一性检查       | `count()` 快速判断键是否存在             |
| 有序数据存储     | 自动排序，支持有序遍历                   |
| 范围查询         | `lower_bound()`/`upper_bound()` 高效范围查找 |

**与 demo::map 的区别**：

| 特性              | demo::set               | demo::map               |
| :---------------- | :---------------------- | :---------------------- |
| **存储内容**      | 仅存储键（key）         | 存储键值对（key-value） |
| **value_type**    | 等于 key_type           | `std::pair<const K, T>` |
| **元素访问**      | 仅支持迭代器访问        | 支持 `operator[]`/`at()`|
| **应用场景**      | 集合运算、去重          | 字典、配置管理          |

**详细文档跳转**：跳转至[demo::set容器开发文档](./set/set.md)

### 4. demo::unordered_set（无序不重复集合容器）

**核心定位**：基于哈希表实现的关联容器，仅存储唯一的键（key），键即元素值，**不保证任何特定顺序**，支持平均 O(1) 时间复杂度的插入、删除和查找操作，是实现高速去重、快速集合成员判断等场景的理想选择。

**设计目标**：对齐 `std::unordered_set` 接口，实现哈希表的高效查找机制，提供平均常数时间复杂度的操作，补充异常安全机制和哈希策略管理。

**核心特性**：

- **哈希表实现**：底层基于链地址法（Separate Chaining）的哈希表，保证平均 O(1) 的时间复杂度；
- **键唯一存储**：仅存储键，元素类型与键类型相同（`value_type = key_type`），自动保证键的唯一性；
- **无序遍历**：迭代器按桶顺序访问元素，不保证特定顺序，重哈希后顺序可能改变；
- **高效查找**：提供 `find()`、`count()`、`equal_range()` 等查找接口；
- **前向迭代器**：支持前向迭代器（`std::forward_iterator_tag`），支持前置/后置自增操作；
- **桶内迭代器**：支持 `local_iterator`/`const_local_iterator` 遍历单个桶的元素；
- **高效插入**：支持 `emplace()`（原地构造）、`insert()`（拷贝/移动版本）等高效插入接口；
- **哈希策略管理**：支持调整负载因子、重哈希、预留空间等操作；
- **桶接口**：支持 `bucket_count()`、`bucket_size()`、`bucket()` 等桶相关接口；
- **移动语义优化**：支持移动构造、移动赋值，避免不必要的拷贝开销；
- **合并操作**：支持 `merge()` 合并异类型 unordered_set 的元素；
- **异常安全**：内存分配失败抛 `std::bad_alloc` 异常。

**核心数据结构设计原理**：

`unordered_set` 采用**链地址法哈希表**实现，核心组件包括：

- **桶数组（m_table）**：存储节点指针的数组，每个桶指向一个链表；
- **节点（Node）**：存储键值、哈希值缓存和指向下一个节点的指针；
- **负载因子（load_factor）**：元素数量与桶数量的比值，超过阈值时触发重哈希；
- **哈希函数（Hash）**：将键转换为哈希值，默认使用 `std::hash<Key>`；
- **键比较函数（KeyEqual）**：判断两个键是否相等，默认使用 `std::equal_to<Key>`；
- **桶数量优化**：桶数量始终为 2 的幂，通过位与运算 `hash & m_mask` 快速定位桶索引。

**常用操作方法及时间复杂度**：

| 操作方法                   | 功能描述         | 时间复杂度 | 说明              |
| :------------------------- | :--------------- | :--------- | :---------------- |
| `insert(const value_type&)` | 插入键（拷贝版本） | O(1) 平均  | 键已存在则忽略    |
| `insert(value_type&&)`      | 插入键（移动版本） | O(1) 平均  | 键已存在则忽略    |
| `emplace(args...)`         | 原地构造键并插入 | O(1) 平均  | 避免拷贝开销      |
| `emplace_hint(hint, args...)` | 在提示位置插入 | O(1) 平均  | hint仅用于兼容性  |
| `erase(key)`               | 删除指定键的元素 | O(1) 平均  | 返回删除数量(0或1)|
| `erase(iterator)`          | 删除迭代器指向元素 | O(1) 平均 | 返回下一个迭代器  |
| `erase(first, last)`       | 删除范围元素     | O(m) 平均  | m为删除元素个数   |
| `find(key)`                | 查找指定键       | O(1) 平均  | 返回迭代器        |
| `count(key)`               | 统计键出现次数   | O(1) 平均  | 返回 0 或 1       |
| `equal_range(key)`         | 等于key的元素范围 | O(1) 平均 | 返回迭代器对      |
| `size()`                   | 返回元素个数     | O(1)       | 直接返回成员变量  |
| `empty()`                  | 判断是否为空     | O(1)       | 比较 size 与 0    |
| `clear()`                  | 清空容器         | O(n)       | 销毁所有节点      |
| `swap(set&)`               | 交换两个容器     | O(1)       | 交换内部指针      |
| `merge(source)`            | 合并另一个容器   | O(m) 平均  | m为源容器元素个数 |
| `rehash(n)`                | 重新哈希         | O(n)       | 重新分配桶数组    |
| `reserve(n)`               | 预留空间         | O(n)       | 可能触发 rehash   |
| `bucket_count()`            | 返回桶数量       | O(1)       | -                 |
| `load_factor()`             | 返回负载因子     | O(1)       | size/bucket_count |
| `bucket(key)`              | 返回键所在桶索引 | O(1) 平均  | -                 |

**使用示例代码**：

```cpp
#include <iostream>
#include "unordered_set/unordered_set.h"
#include <string>

int main() {
    // 创建 unordered_set 容器
    demo::unordered_set<std::string> s{"apple", "banana", "cherry"};

    // 插入元素
    s.insert("date");
    s.emplace("elderberry");
    
    // 尝试插入重复元素（会被忽略）
    auto [it, inserted] = s.insert("apple");
    std::cout << "apple inserted: " << std::boolalpha << inserted << std::endl; // false

    // 查找元素
    auto find_it = s.find("cherry");
    if (find_it != s.end()) {
        std::cout << "Found: " << *find_it << std::endl;
    }

    // 遍历（无序）
    std::cout << "Traversal: ";
    for (const auto& key : s) {
        std::cout << key << " ";
    }
    std::cout << std::endl;

    // 统计元素数量
    std::cout << "cherry count: " << s.count("cherry") << std::endl; // 1
    std::cout << "grape count: " << s.count("grape") << std::endl;   // 0

    // 查看哈希策略信息
    std::cout << "Bucket count: " << s.bucket_count() << std::endl;
    std::cout << "Load factor: " << s.load_factor() << std::endl;

    // 删除元素
    s.erase("banana");
    std::cout << "Size after erase: " << s.size() << std::endl;

    return 0;
}
```

**输出结果**：

```
apple inserted: false
Found: cherry
Traversal: apple banana cherry date elderberry 
cherry count: 1
grape count: 0
Bucket count: 16
Load factor: 0.3125
Size after erase: 4
```

**与 demo::set 的区别**：

| 特性              | demo::unordered_set       | demo::set               |
| :---------------- | :------------------------ | :---------------------- |
| **底层实现**      | 哈希表（链地址法）        | 红黑树                  |
| **元素顺序**      | 无序                      | 按键升序排序            |
| **查找复杂度**    | O(1) 平均，O(n) 最坏      | O(log n) 最坏           |
| **插入复杂度**    | O(1) 平均，O(n) 最坏      | O(log n) 最坏           |
| **删除复杂度**    | O(1) 平均，O(n) 最坏      | O(log n) 最坏           |
| **内存布局**      | 非连续（节点式+桶数组）   | 非连续（节点式）        |
| **迭代器类型**    | 前向迭代器                | 双向迭代器              |
| **反向遍历**      | 不支持                    | 支持（rbegin/rend）     |
| **范围查询**      | 不支持（无序）            | 支持（lower/upper_bound）|
| **内存开销**      | 较高（哈希表+链表）       | 较高（红黑树结构）      |
| **适用场景**      | 快速查找、去重、不关心顺序 | 有序遍历、范围查询      |

**适用场景**：

| 场景             | 推荐使用 unordered_set 的原因           |
| :--------------- | :------------------------------------- |
| 去重处理         | 键唯一性保证，自动去重                   |
| 集合成员判断     | O(1) 平均查找，快速判断元素是否在集合中   |
| 缓存/去重系统    | 高速查找、插入、删除                   |
| 不关心顺序的场景 | 无序存储，无需排序开销                 |
| 唯一性验证       | 快速判断数据是否重复                   |

**与 demo::unordered_map 的区别**：

| 特性              | demo::unordered_set        | demo::unordered_map         |
| :---------------- | :------------------------- | :-------------------------- |
| **存储内容**      | 仅存储键（key）            | 存储键值对（key-value）     |
| **value_type**    | 等于 key_type              | `std::pair<const K, T>`     |
| **元素访问**      | 仅支持迭代器访问           | 支持 `operator[]`/`at()`   |
| **应用场景**      | 集合运算、去重、成员判断   | 字典、配置管理、快速索引    |

**详细文档跳转**：跳转至[demo::unordered_set容器开发文档](./unordered_set/unordered_set.md)

### 待实现关联式容器

后续将逐步实现以下关联式容器，实现后将同步更新本总览及对应详细文档：

- demo::multiset（有序可重复集合）：set的扩展，允许键值重复；
- demo::multimap（有序可重复键值对容器）：map的扩展，允许键值重复。

## 三、容器适配器（Container Adapters）【待扩展】

容器适配器不直接存储元素，而是基于序列式容器（如vector、deque、list）进行封装，提供特定的接口（如栈的push/pop、队列的push/pop），屏蔽底层容器的实现细节，适配特定场景。

后续将逐步实现以下容器适配器，实现后将同步更新本总览及对应详细文档：

- demo::stack（栈适配器）：基于deque（默认）/vector/list实现，遵循"后进先出"（LIFO）原则；
- demo::queue（队列适配器）：基于deque（默认）/list实现，遵循"先进先出"（FIFO）原则；
- demo::priority_queue（优先队列适配器）：基于vector实现，内部通过堆排序保证队首元素为优先级最高元素。

# 文档使用指引

## 1. 快速定位容器

可通过文档目录快速跳转至目标容器：

- 序列式容器：直接定位到对应容器（如vector、list）的简介和跳转指引；
- 关联式/适配器容器：待实现后，可通过目录快速定位，查看核心特性和适用场景。

## 2. 跳转至详细文档

每个已实现容器的"详细文档跳转"部分，均关联对应容器的完整开发文档，跳转方式建议：

- 本地文档：将本综合文档与各容器详细文档放在同一目录，通过相对路径关联（如"[demo::vector详细文档](vector开发文档.md)"）；
- 工程内文档：在IDE中（如VS Code、Clion），将详细文档与对应头文件（vector.h）关联，点击跳转即可查看；
- 在线文档：可将详细文档上传至在线文档平台，在本总览中添加对应文档链接，实现点击跳转。

## 3. 新增容器更新

当新增自定义STL容器（如list、set）时，需同步更新本综合文档：

- 在对应容器类别（序列式/关联式/适配器）下，添加容器简介、核心特性、适用场景；
- 补充详细文档跳转指引，确保与新增容器的详细文档关联；
- 更新版本历史，记录新增容器的版本和变更内容。

# 版本历史

| 版本   | 变更内容                                                                                                                                                                                                                               | 日期       |
| :----- | :------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- | :--------- |
| v1.1   | 初始版本，包含 demo::vector、demo::forward_list 容器文档                                                                                                                                                                               | 2026-04-10 |
| v1.2   | 1. 更新 demo::list 容器为已实现状态；2. 补充 list 核心特性、适用场景和详细文档跳转；3. 更新版本历史记录                                                                                                                                | 2026-04-14 |
| v1.3   | 1. 新增 demo::deque 容器简介及跳转指引；2. 补充 deque 核心特性、适用场景和详细文档跳转；3. 更新版本历史记录                                                                                                                            | 2026-04-27 |
| v1.4   | 1. 更新 demo::deque 为已实现状态；2. 补充 deque 核心数据结构设计原理、关键成员函数实现逻辑与时间复杂度分析、与 vector 对比、内存管理机制、迭代器实现原理及失效情况、异常安全保证级别、完整使用示例；3. 更新版本历史记录                | 2026-04-27 |
| v1.5   | 1. 完善 demo::deque 文档，添加常用操作方法及时间复杂度表格；2. 补充 deque 与 vector、list 的三容器对比分析；3. 添加适用场景建议表格；4. 详细说明迭代器实现原理和异常安全保证；5. 更新 README.md 中 deque 相关内容；6. 更新版本历史记录 | 2026-05-05 |
| v1.6   | 1. 更新 demo::map 为已实现状态；2. 补充 map 核心特性、核心数据结构设计原理、常用操作方法及时间复杂度、完整使用示例和输出结果；3. 添加适用场景建议表格；4. 更新 README.md 中 map 相关内容；5. 更新版本历史记录 | 2026-05-17 |
| v1.7   | 1. 更新 demo::set 为已实现状态；2. 补充 set 核心特性、核心数据结构设计原理、常用操作方法及时间复杂度、完整使用示例和输出结果；3. 添加适用场景建议表格；4. 添加与 demo::map 的对比表格；5. 更新 README.md 中 set 相关内容；6. 更新版本历史记录 | 2026-05-20 |
| v1.8   | 1. 更新 demo::unordered_map 为已实现状态；2. 补充 unordered_map 核心特性、核心数据结构设计原理、常用操作方法及时间复杂度、完整使用示例和输出结果；3. 添加与 demo::map 的对比表格；4. 添加适用场景建议表格；5. 更新 README.md 中 unordered_map 相关内容；6. 更新版本历史记录 | 2026-05-26 |
| v1.9   | 1. 更新 demo::unordered_set 为已实现状态；2. 补充 unordered_set 核心特性、核心数据结构设计原理、常用操作方法及时间复杂度、完整使用示例和输出结果；3. 添加与 demo::set 的对比表格；4. 添加与 demo::unordered_map 的对比表格；5. 添加适用场景建议表格；6. 更新 README.md 中 unordered_set 相关内容；7. 更新版本历史记录 | 2026-05-27 |
| 待更新 | 1. 扩展关联式容器（multiset、multimap）、容器适配器的框架内容                                                                                                                                                                          | -          |

# 注意事项

- 所有自定义容器均位于demo命名空间下，使用时需添加`using namespace demo;`或显式调用（如demo::vector），避免与std标准库容器冲突；
- 详细文档与本综合文档同步更新，确保容器特性、接口说明的一致性，避免出现文档与代码不匹配的情况；
- 待实现容器的框架仅作为预留，实现后需及时补充完整内容，确保文档的实用性和完整性；
- 若修改容器的核心特性（如vector的扩容因子、接口名称），需同时更新本综合文档和对应容器的详细文档，避免误导开发者。
