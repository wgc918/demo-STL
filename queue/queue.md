# demo::queue 适配器开发文档

## 1. 文档概述

### 1.1 文档目的

本文档详细描述自定义实现的 `demo::queue` 容器适配器的设计思路、核心功能、接口规范、使用方法及注意事项，为开发者提供完整的开发、测试和维护参考，同时对齐 C++ 标准库 `std::queue` 的接口设计，保证兼容性和易用性。

### 1.2 适配器简介

`demo::queue` 是一个遵循 C++17 标准的**容器适配器**，提供**先进先出（FIFO, First In First Out）**的数据结构。`queue` 封装了一个底层序列容器，仅允许在容器的一端（队尾）进行插入操作，在另一端（队首）进行删除操作，默认使用 `demo::deque` 作为底层容器。

### 1.3 适用范围

- 编译环境：兼容 GCC、Clang、MSVC 等主流编译器，需开启 C++17 及以上标准（`-std=c++17`）；
- 元素类型：支持任意可拷贝/可移动/可析构的类型（内置类型、`std::string`、自定义类等）；
- 底层容器：要求支持 `front()`、`back()`、`push_back()`、`pop_front()` 操作的序列容器（默认 `deque`，也支持 `list` 等）；
- 应用场景：适合需要先进先出操作的场景（如任务队列、消息队列、广度优先搜索、缓冲区管理等）。

## 2. 设计思路

### 2.1 核心架构

#### 2.1.1 容器适配器模式

`queue` 是一个典型的容器适配器，本身不直接存储元素，而是通过封装一个底层容器来实现队列的功能：

```c++
template <typename T, class Container = deque<T>>
class queue {
protected:
    Container m_container;  // 底层容器
    // ...
};
```

#### 2.1.2 容器核心成员

| 成员变量 | 类型 | 作用 |
| :------: | :--: | :--: |
| `m_container` | `Container` | 底层序列容器，实际存储队列中的所有元素 |

#### 2.1.3 架构优势

- **简单高效**：通过适配器模式复用已有的序列容器，无需重新实现底层数据结构；
- **灵活性强**：可自定义底层容器（如对频繁首尾操作性能有要求可用 `deque`，对迭代器稳定性有要求可用 `list`）；
- **接口简洁**：提供最小化的队列操作接口，符合 FIFO 语义；
- **异常安全**：依赖底层容器的异常保证，简化异常处理；
- **标准兼容**：完全对齐 `std::queue` 的接口设计，易于迁移和使用。

### 2.2 底层容器选择

`queue` 默认使用 `demo::deque` 作为底层容器，用户也可选择其他符合要求的容器：

| 底层容器 | 特点 | 适用场景 |
| :------: | :--: | :------: |
| `deque`（默认） | 双端队列，首尾操作高效，内存分块 | 通用场景，平衡性能和内存 |
| `list` | 双向链表，任意位置插入删除高效，迭代器稳定 | 需要迭代器稳定性的场景 |

> **注意**：`vector` 不适合作为 `queue` 的底层容器，因为 `vector` 的 `pop_front()` 操作时间复杂度为 O(n)，会严重影响队列性能。

### 2.3 内存管理策略

1. **完全依赖底层容器**：所有内存管理操作（分配、释放、构造、析构）均由底层容器负责；
2. **异常安全**：继承底层容器的异常保证，底层容器提供的保证即为 queue 的保证；
3. **移动语义**：移动构造/赋值时直接移动底层容器，时间复杂度 O(1)；
4. **拷贝语义**：拷贝构造/赋值时拷贝底层容器，时间复杂度 O(n)；
5. **swap 操作**：直接交换底层容器，时间复杂度 O(1)。

## 3. 接口说明

### 3.1 类型别名（Type Aliases）

| 类型别名 | 含义 |
| :------: | :--: |
| `container_type` | 底层容器类型（`Container`） |
| `value_type` | 元素类型（`Container::value_type`） |
| `reference` | 元素引用类型（`Container::reference`） |
| `const_reference` | 常量元素引用类型（`Container::const_reference`） |
| `size_type` | 无符号整数类型（`Container::size_type`） |

### 3.2 构造与析构

| 接口 | 功能说明 | 示例 |
| :--: | :------: | :--: |
| `queue()` | 默认构造，创建空队列 | `demo::queue<int> q;` |
| `explicit queue(const Container& cont)` | 从容器拷贝构造 | `demo::deque<int> d{1,2,3}; demo::queue<int> q(d);` |
| `explicit queue(Container&& cont)` | 从容器移动构造 | `demo::queue<int> q(std::move(d));` |
| `queue(const queue& other)` | 拷贝构造 | `demo::queue<int> q2(q1);` |
| `queue(queue&& other) noexcept` | 移动构造 | `demo::queue<int> q2(std::move(q1));` |
| `~queue()` | 析构函数 | - |

### 3.3 赋值操作

| 接口 | 功能说明 | 示例 |
| :--: | :------: | :--: |
| `queue& operator=(const queue& other)` | 拷贝赋值 | `q2 = q1;` |
| `queue& operator=(queue&& other) noexcept` | 移动赋值 | `q2 = std::move(q1);` |

### 3.4 元素访问

| 接口 | 功能说明 | 注意事项 |
| :--: | :------: | :------: |
| `reference front()` | 返回队首元素的引用 | 在空队列上调用会导致未定义行为 |
| `const_reference front() const` | 返回队首元素的常量引用 | 在空队列上调用会导致未定义行为 |
| `reference back()` | 返回队尾元素的引用 | 在空队列上调用会导致未定义行为 |
| `const_reference back() const` | 返回队尾元素的常量引用 | 在空队列上调用会导致未定义行为 |

### 3.5 容量操作

| 接口 | 功能说明 | 示例 |
| :--: | :------: | :--: |
| `bool empty() const` | 判断队列是否为空 | `if (q.empty()) { ... }` |
| `size_type size() const` | 返回队列中元素的个数 | `auto sz = q.size();` |

### 3.6 元素修改操作

| 接口 | 功能说明 | 时间复杂度 |
| :--: | :------: | :--------: |
| `void push(const value_type& value)` | 在队尾插入元素（拷贝版本） | 依赖底层容器，通常 O(1) |
| `void push(value_type&& value)` | 在队尾插入元素（移动版本） | 依赖底层容器，通常 O(1) |
| `template <class... Args> decltype(auto) emplace(Args&&... args)` | 在队尾原地构造元素 | 依赖底层容器，通常 O(1) |
| `void pop()` | 移除队首元素 | 依赖底层容器，通常 O(1) |
| `void swap(queue& other)` | 交换两个队列的内容 | 依赖底层容器，通常 O(1) |

### 3.7 比较运算符

| 运算符 | 功能说明 | 示例 |
| :----: | :------: | :--: |
| `bool operator==(const queue& lhs, const queue& rhs)` | 比较两个队列是否相等 | `if (q1 == q2) { ... }` |
| `bool operator!=(const queue& lhs, const queue& rhs)` | 比较两个队列是否不相等 | `if (q1 != q2) { ... }` |

## 4. 使用示例

### 4.1 基础使用

```c++
#include "queue.h"
#include "../deque/deque.h"
#include "../list/list.h"
#include <iostream>
#include <string>

int main() {
    // 1. 构造 queue（默认使用 deque 作为底层容器）
    demo::queue<std::string> q;
    
    // 2. 入队元素
    q.push("hello");
    q.push("world");
    q.emplace("!");
    
    // 3. 访问队首和队尾元素
    std::cout << "队首元素：" << q.front() << std::endl;
    std::cout << "队尾元素：" << q.back() << std::endl;
    
    // 4. 出队元素
    q.pop();
    std::cout << "出队后队首元素：" << q.front() << std::endl;
    
    // 5. 查看容量
    std::cout << "队列大小：" << q.size() << std::endl;
    std::cout << "队列是否为空：" << (q.empty() ? "是" : "否") << std::endl;
    
    // 6. 使用 list 作为底层容器
    demo::queue<int, demo::list<int>> list_queue;
    list_queue.push(1);
    list_queue.push(2);
    list_queue.push(3);
    std::cout << "list 队列大小：" << list_queue.size() << std::endl;
    
    // 7. 拷贝和移动
    demo::queue<std::string> q2(q);  // 拷贝
    demo::queue<std::string> q3(std::move(q2));  // 移动
    
    // 8. 比较
    if (q == q3) {
        std::cout << "q 和 q3 相等" << std::endl;
    }
    
    return 0;
}
```

### 4.2 输出结果

```
队首元素：hello
队尾元素：!
出队后队首元素：world
队列大小：2
队列是否为空：否
list 队列大小：3
q 和 q3 相等
```

## 5. 异常处理

| 异常场景 | 抛出类型 | 预防措施 |
| :------: | :------: | :------: |
| `front()`、`back()` 或 `pop()` 在空队列上调用 | 未定义行为 | 调用前使用 `empty()` 检查 |
| 构造/插入元素时内存分配失败 | 依赖底层容器 | 捕获异常或提前检查可用内存 |
| 元素构造/析构失败 | 依赖元素类型 | 确保元素类型满足异常安全要求 |

## 6. 性能说明

### 6.1 时间复杂度

所有 `queue` 操作的时间复杂度完全依赖于底层容器的实现：

| 操作 | 时间复杂度（默认 deque） | 备注 |
| :--: | :----------------------: | :--: |
| `front()`/`back()` | O(1) | 直接访问容器首尾 |
| `push()`/`emplace()` | O(1) 均摊 | 可能触发底层容器扩容 |
| `pop()` | O(1) | 直接删除容器首元素 |
| `size()`/`empty()` | O(1) | 直接返回容器大小信息 |
| `swap()` | O(1) | 交换底层容器 |
| 拷贝构造/赋值 | O(n) | 拷贝所有元素 |
| 移动构造/赋值 | O(1) | 移动底层容器 |

### 6.2 性能优化建议

1. **选择合适的底层容器**：
   - 通用场景使用默认的 `deque`
   - 需要迭代器稳定性使用 `list`

2. **使用 `emplace` 替代 `push`**：直接在队尾原地构造元素，避免不必要的拷贝/移动开销

3. **使用移动语义**：对于支持移动语义的元素类型，使用移动版本的 `push` 或直接 `emplace`

## 7. 与 std::queue 的差异

| 特性 | demo::queue | std::queue |
| :--: | :---------: | :--------: |
| 底层容器 | 默认使用 demo::deque | 默认使用 std::deque |
| 接口设计 | 完全对齐标准库 | - |
| 异常保证 | 继承自底层容器 | 继承自底层容器 |
| 可移植性 | 自定义实现，需包含 queue.h | 标准库容器，直接包含 &lt;queue&gt; |

## 8. 已知限制

1. **不支持遍历**：`queue` 不提供迭代器，无法直接遍历所有元素（如需遍历可继承 queue 访问底层容器或改用其他容器）；
2. **不支持比较运算符（&lt;/&gt;/&lt;=/&gt;=）**：仅支持 `==`/`!=`；
3. **不支持直接访问底层容器**：底层容器为 `protected` 成员，仅派生类可访问；
4. **不支持范围构造**：不支持从迭代器范围构造（如需此功能可先构造底层容器再传递给 queue）；
5. **不推荐使用 vector 作为底层容器**：`vector` 的 `pop_front()` 操作效率较低（O(n)）。

## 9. 测试建议

1. 基础功能测试：构造、push、pop、front、back、size、empty 等核心接口；
2. 边界测试：空队列操作、单元素队列、大量元素入队/出队；
3. 类型兼容性测试：测试内置类型、复杂类型（std::string）、自定义类的兼容性；
4. 自定义底层容器测试：测试使用 deque、list 作为底层容器；
5. 拷贝/移动语义测试：测试拷贝构造、拷贝赋值、移动构造、移动赋值、swap 操作的正确性；
6. 异常测试：测试在空队列上调用 front()/back()/pop() 的行为，以及元素构造/析构失败的处理；
7. 性能测试：对比 std::queue 的性能，测试不同底层容器的性能差异。

---

**文档版本**：v1.0
**创建日期**：2026-05-30
**适用容器**：`demo::queue<T, Container>`