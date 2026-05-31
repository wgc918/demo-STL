# demo::priority_queue 适配器开发文档

## 1. 文档概述

### 1.1 文档目的

本文档详细描述自定义实现的 `demo::priority_queue` 容器适配器的设计思路、核心功能、接口规范、使用方法及注意事项，为开发者提供完整的开发、测试和维护参考，同时对齐 C++ 标准库 `std::priority_queue` 的接口设计，保证兼容性和易用性。

### 1.2 适配器简介

`demo::priority_queue` 是一个遵循 C++17 标准的**容器适配器**，提供**优先队列**功能的数据结构。`priority_queue` 封装了一个底层序列容器，使用**堆（Heap）数据结构**来实现元素的优先级管理，顶部元素（top）始终是优先级最高的元素。默认使用 `demo::vector` 作为底层容器。

### 1.3 适用范围

- 编译环境：兼容 GCC、Clang、MSVC 等主流编译器，需开启 C++17 及以上标准（`-std=c++17`）；
- 元素类型：支持任意可拷贝/可移动/可析构的类型（内置类型、`std::string`、自定义类等）；
- 底层容器：要求支持随机访问迭代器和 `back()`、`push_back()`、`pop_back()` 操作的序列容器（默认 `vector`，也支持 `deque` 等）；
- 应用场景：适合需要优先级管理的场景（如任务调度、事件处理、最短路径算法、合并有序文件等）。

## 2. 设计思路

### 2.1 核心架构

#### 2.1.1 容器适配器模式

`priority_queue` 是一个典型的容器适配器，本身不直接存储元素，而是通过封装一个底层容器并使用堆数据结构来实现优先队列的功能：

```c++
template <typename T, typename Container = vector<T>,
          typename Compare = std::less<typename Container::value_type>>
class priority_queue {
protected:
    Container m_container;  // 底层容器
    Compare   m_comp;        // 比较函数对象
    // ...
};
```

#### 2.1.2 堆数据结构原理

堆是一种完全二叉树，满足**堆性质**：对于最大堆，父节点的值总是大于或等于其子节点的值。`priority_queue` 默认使用最大堆实现（通过 `std::less` 比较函数），队顶元素始终是最大元素。

堆的物理存储使用数组，通过下标计算即可得到父节点和子节点的关系：

- 父节点索引：`parent(i) = (i - 1) / 2`
- 左子节点索引：`left_child(i) = 2 * i + 1`
- 右子节点索引：`right_child(i) = 2 * i + 2`

#### 2.1.3 容器核心成员

|   成员变量    |    类型     |                          作用                          |
| :-----------: | :---------: | :----------------------------------------------------: |
| `m_container` | `Container` |       底层序列容器，实际存储优先队列中的所有元素       |
|   `m_comp`    |  `Compare`  | 比较函数对象，用于确定元素优先级，默认使用 `std::less` |

#### 2.1.4 架构优势

- **高效优先级操作**：堆数据结构使得插入和删除操作的时间复杂度为 O(log n)，获取顶部元素为 O(1)；
- **灵活性强**：可自定义底层容器（如对内存效率有要求可用 `vector`，对首尾操作有要求可用 `deque`）和比较函数；
- **接口简洁**：提供最小化的优先队列操作接口，符合优先级语义；
- **标准兼容**：完全对齐 `std::priority_queue` 的接口设计，易于迁移和使用。

### 2.2 堆操作实现

#### 2.2.1 向上调整（up）

当向堆中插入新元素时，将新元素放在堆的末尾（数组最后），然后执行向上调整操作：比较新元素与其父节点，如果不满足堆性质则交换，重复此过程直到根节点或满足堆性质。时间复杂度为 O(log n)。

#### 2.2.2 向下调整（down）

当从堆中移除顶部元素时，将堆的最后一个元素移到根部，然后执行向下调整操作：从当前节点与其子节点中选择优先级最高（根据比较函数）的节点，如果不满足堆性质则交换，重复此过程直到叶子节点或满足堆性质。时间复杂度为 O(log n)。

#### 2.2.3 Floyd 建堆算法（build_heap_floyd）

从最后一个非叶子节点(最后一个节点的父节点)开始，向根节点方向，依次对每个节点执行向下调整操作。时间复杂度为 O(n)，相比逐个插入元素的 O(n log n) 更为高效。

### 2.3 底层容器选择

`priority_queue` 默认使用 `demo::vector` 作为底层容器，用户也可选择其他符合要求的容器：

|     底层容器     |               特点               |              适用场景              |
| :--------------: | :------------------------------: | :--------------------------------: |
| `vector`（默认） | 动态数组，随机访问高效，内存连续 | 通用场景，追求内存效率和缓存友好性 |
|     `deque`      | 双端队列，首尾操作高效，内存分块 |       需要高效首尾操作的场景       |

### 2.4 比较函数

`priority_queue` 使用模板参数 `Compare` 来定义元素的优先级比较规则，默认使用 `std::less`，即数值越大优先级越高（最大堆）。

可以通过自定义比较函数实现最小堆或其他自定义优先级规则：

```c++
demo::priority_queue<int, std::vector<int>, std::greater<int>> min_heap;  // 最小堆
```

### 2.5 内存管理策略

1. **完全依赖底层容器**：所有内存管理操作（分配、释放、构造、析构）均由底层容器负责；
2. **异常安全**：继承底层容器的异常保证，底层容器提供的保证即为 priority_queue 的保证；
3. **移动语义**：移动构造/赋值时直接移动底层容器，时间复杂度 O(1)；
4. **拷贝语义**：拷贝构造/赋值时拷贝底层容器，时间复杂度 O(n)；
5. **swap 操作**：直接交换底层容器，时间复杂度 O(1)。

## 3. 接口说明

### 3.1 类型别名（Type Aliases）

|     类型别名      |                       含义                       |
| :---------------: | :----------------------------------------------: |
| `container_type`  |           底层容器类型（`Container`）            |
|   `value_type`    |       元素类型（`Container::value_type`）        |
|    `reference`    |      元素引用类型（`Container::reference`）      |
| `const_reference` | 常量元素引用类型（`Container::const_reference`） |
|    `size_type`    |     无符号整数类型（`Container::size_type`）     |

### 3.2 构造与析构

|                                              接口                                              |             功能说明             |                                 示例                                  |
| :--------------------------------------------------------------------------------------------: | :------------------------------: | :-------------------------------------------------------------------: |
|                                       `priority_queue()`                                       |     默认构造，创建空优先队列     |                    `demo::priority_queue<int> pq;`                    |
|                     `explicit priority_queue(const Container& container)`                      |       从容器拷贝构造并建堆       |    `demo::vector<int> v{3,1,2}; demo::priority_queue<int> pq(v);`     |
|                         `explicit priority_queue(const Compare& comp)`                         |          从比较函数构造          | `demo::priority_queue<int, vector<int>, std::greater<int>> pq(comp);` |
|               `priority_queue(const Container& container, const Compare& comp)`                |    从容器和比较函数构造并建堆    |               `demo::priority_queue<int> pq(v, comp);`                |
|                            `priority_queue(Container&& container)`                             |       从容器移动构造并建堆       |             `demo::priority_queue<int> pq(std::move(v));`             |
|                  `priority_queue(Container&& container, const Compare& comp)`                  |  从容器移动构造和比较函数并建堆  |          `demo::priority_queue<int> pq(std::move(v), comp);`          |
|           `template <typename InputIt> priority_queue(InputIt first, InputIt last)`            |      从迭代器范围构造并建堆      |             `demo::priority_queue<int> pq(first, last);`              |
| `template <typename InputIt> priority_queue(InputIt first, InputIt last, const Compare& comp)` | 从迭代器范围和比较函数构造并建堆 |          `demo::priority_queue<int> pq(first, last, comp);`           |
|                         `priority_queue(const priority_queue& other)`                          |             拷贝构造             |                 `demo::priority_queue<int> pq2(pq1);`                 |
|                       `priority_queue(priority_queue&& other) noexcept`                        |             移动构造             |           `demo::priority_queue<int> pq2(std::move(pq1));`            |
|                                      `~priority_queue()`                                       |             析构函数             |                                   -                                   |

### 3.3 赋值操作

|                             接口                             | 功能说明 |          示例           |
| :----------------------------------------------------------: | :------: | :---------------------: |
|   `priority_queue& operator=(const priority_queue& other)`   | 拷贝赋值 |      `pq2 = pq1;`       |
| `priority_queue& operator=(priority_queue&& other) noexcept` | 移动赋值 | `pq2 = std::move(pq1);` |

### 3.4 元素访问

|             接口              |                  功能说明                  |              注意事项              |
| :---------------------------: | :----------------------------------------: | :--------------------------------: |
| `const_reference top() const` | 返回顶部元素的常量引用（优先级最高的元素） | 在空优先队列上调用会导致未定义行为 |

### 3.5 容量操作

|           接口           |         功能说明         |           示例            |
| :----------------------: | :----------------------: | :-----------------------: |
|   `bool empty() const`   |   判断优先队列是否为空   | `if (pq.empty()) { ... }` |
| `size_type size() const` | 返回优先队列中元素的个数 |  `auto sz = pq.size();`   |

### 3.6 元素修改操作

|                            接口                            |        功能说明        | 时间复杂度 |
| :--------------------------------------------------------: | :--------------------: | :--------: |
|            `void push(const value_type& value)`            |  插入元素（拷贝版本）  |  O(log n)  |
|              `void push(value_type&& value)`               |  插入元素（移动版本）  |  O(log n)  |
| `template <typename... Args> void emplace(Args&&... args)` |      原地构造元素      |  O(log n)  |
|                        `void pop()`                        |      移除顶部元素      |  O(log n)  |
|             `void swap(priority_queue& other)`             | 交换两个优先队列的内容 |    O(1)    |

### 3.7 保护成员函数（内部实现）

|                 接口                 |                 功能说明                  |
| :----------------------------------: | :---------------------------------------: |
|          `void up(int idx)`          | 向上调整堆，使索引 idx 处的元素满足堆性质 |
|         `void down(int idx)`         | 向下调整堆，使索引 idx 处的元素满足堆性质 |
|   `int get_parent(int idx) const`    |              获取父节点索引               |
| `int get_left_child(int idx) const`  |             获取左子节点索引              |
| `int get_right_child(int idx) const` |             获取右子节点索引              |
|      `void build_heap_floyd()`       |      使用 Floyd 算法从已有元素构建堆      |

## 4. 使用示例

### 4.1 基础使用

```c++
#include "priority_queue.h"
#include "../vector/vector.h"
#include "../deque/deque.h"
#include <iostream>
#include <string>
#include <functional>

int main() {
    // 1. 构造 priority_queue（默认使用 vector 作为底层容器）
    demo::priority_queue<int> pq;

    // 2. 插入元素
    pq.push(30);
    pq.push(10);
    pq.push(50);
    pq.push(20);
    pq.emplace(40);

    // 3. 访问顶部元素（最大元素）
    std::cout << "顶部元素：" << pq.top() << std::endl;

    // 4. 查看容量
    std::cout << "优先队列大小：" << pq.size() << std::endl;
    std::cout << "优先队列是否为空：" << (pq.empty() ? "是" : "否") << std::endl;

    // 5. 弹出元素
    pq.pop();
    std::cout << "弹出后顶部元素：" << pq.top() << std::endl;

    // 6. 使用自定义底层容器
    demo::priority_queue<int, demo::deque<int>> deque_pq;
    deque_pq.push(100);
    deque_pq.push(50);
    std::cout << "deque 优先队列顶部元素：" << deque_pq.top() << std::endl;

    // 7. 使用自定义比较函数（最小堆）
    demo::priority_queue<int, demo::vector<int>, std::greater<int>> min_pq;
    min_pq.push(30);
    min_pq.push(10);
    min_pq.push(50);
    std::cout << "最小堆顶部元素：" << min_pq.top() << std::endl;

    // 8. 从容器构造
    demo::vector<int> v = {5, 2, 8, 1, 9};
    demo::priority_queue<int> pq_from_vec(v);
    std::cout << "从 vector 构造的优先队列顶部元素：" << pq_from_vec.top() << std::endl;

    // 9. 拷贝和移动
    demo::priority_queue<int> pq2(pq);  // 拷贝
    demo::priority_queue<int> pq3(std::move(pq2));  // 移动

    // 10. 交换
    demo::priority_queue<int> pq4;
    pq4.push(999);
    pq4.swap(pq3);
    std::cout << "交换后 pq4 顶部元素：" << pq4.top() << std::endl;

    return 0;
}
```

### 4.2 输出结果

```
顶部元素：50
优先队列大小：5
优先队列是否为空：否
弹出后顶部元素：40
deque 优先队列顶部元素：100
最小堆顶部元素：10
从 vector 构造的优先队列顶部元素：9
交换后 pq4 顶部元素：50
```

## 5. 异常处理

|               异常场景                |   抛出类型   |           预防措施           |
| :-----------------------------------: | :----------: | :--------------------------: |
| `top()` 或 `pop()` 在空优先队列上调用 |  未定义行为  |  调用前使用 `empty()` 检查   |
|      构造/插入元素时内存分配失败      | 依赖底层容器 |  捕获异常或提前检查可用内存  |
|           元素构造/析构失败           | 依赖元素类型 | 确保元素类型满足异常安全要求 |
|           比较函数抛出异常            |  未定义行为  |    确保比较函数不抛出异常    |

## 6. 性能说明

### 6.1 时间复杂度

|           操作            | 时间复杂度（默认 vector） |           备注           |
| :-----------------------: | :-----------------------: | :----------------------: |
|          `top()`          |           O(1)            |  直接访问容器第一个元素  |
|    `empty()`/`size()`     |           O(1)            |   直接返回容器大小信息   |
|   `push()`/`emplace()`    |         O(log n)          |   插入元素并向上调整堆   |
|          `pop()`          |         O(log n)          | 移除顶部元素并向下调整堆 |
| 构造（从容器/迭代器范围） |           O(n)            |   使用 Floyd 算法建堆    |
|       拷贝构造/赋值       |           O(n)            |       拷贝所有元素       |
|       移动构造/赋值       |           O(1)            |       移动底层容器       |
|         `swap()`          |           O(1)            |       交换底层容器       |

### 6.2 空间复杂度

|         操作         | 空间复杂度 |             备注             |
| :------------------: | :--------: | :--------------------------: |
|       基础存储       |    O(n)    |    底层容器存储 n 个元素     |
| `push()`/`emplace()` | O(1) 额外  |     可能触发底层容器扩容     |
|       `pop()`        | O(1) 额外  | 仅调整堆结构，不额外占用空间 |

### 6.3 性能优化建议

1. **选择合适的底层容器**：
   - 通用场景使用默认的 `vector`（缓存友好）
   - 需要高效首尾操作使用 `deque`

2. **使用 `emplace` 替代 `push`**：直接在优先队列中原地构造元素，避免不必要的拷贝/移动开销

3. **批量插入优化**：如果需要插入大量元素，可以先构造底层容器，然后通过迭代器范围构造函数一次性建堆，比逐个 `push` 效率更高

4. **使用移动语义**：对于支持移动语义的元素类型，使用移动版本的 `push` 或直接 `emplace`

5. **预先估计容量**：如果使用 `vector` 作为底层容器且大致知道元素数量，可以预先调用 `reserve()` 方法预留空间

## 7. 与 std::priority_queue 的差异

|   特性   |        demo::priority_queue         |     std::priority_queue      |
| :------: | :---------------------------------: | :--------------------------: |
| 底层容器 |        默认使用 demo::vector        |     默认使用 std::vector     |
| 比较函数 |    默认使用 std::less（最大堆）     | 默认使用 std::less（最大堆） |
| 接口设计 |           完全对齐标准库            |              -               |
| 异常保证 |           继承自底层容器            |        继承自底层容器        |
| 可移植性 | 自定义实现，需包含 priority_queue.h | 标准库容器，直接包含 <queue> |

## 8. 已知限制

1. **不支持遍历**：`priority_queue` 不提供迭代器，无法直接遍历所有元素（如需遍历可继承 priority_queue 访问底层容器或改用其他容器）；
2. **不支持比较运算符（`==`、`!=`、`<`、`>`、`<=`、`>=`）**：`priority_queue` 不支持直接比较操作；
3. **不支持直接访问底层容器**：底层容器和比较函数均为 `protected` 成员，仅派生类可访问；
4. **不支持随机访问元素**：只能访问顶部元素（`top()`），无法访问其他位置的元素；
5. **不支持修改顶部元素**：没有提供修改顶部元素的接口，需要先 `pop()` 再 `push()`。

## 9. 测试建议

1. **基础功能测试**：构造、push、pop、top、size、empty 等核心接口；
2. **边界测试**：空优先队列操作、单元素优先队列、大量元素插入/弹出；
3. **类型兼容性测试**：测试内置类型、复杂类型（std::string）、自定义类的兼容性；
4. **自定义底层容器测试**：测试使用 vector、deque 作为底层容器；
5. **自定义比较函数测试**：测试使用 std::greater（最小堆）、自定义比较函数；
6. **建堆算法测试**：测试从容器构造和从迭代器范围构造的 Floyd 建堆算法；
7. **拷贝/移动语义测试**：测试拷贝构造、拷贝赋值、移动构造、移动赋值、swap 操作的正确性；
8. **异常测试**：测试在空优先队列上调用 top()/pop() 的行为，以及元素构造/析构失败的处理；
9. **性能测试**：对比 std::priority_queue 的性能，测试不同底层容器和不同元素数量的性能差异。

---

**文档版本**：v1.0
**创建日期**：2026-05-31
**适用容器**：`demo::priority_queue<T, Container, Compare>`
