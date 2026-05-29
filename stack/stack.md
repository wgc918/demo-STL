# demo::stack 适配器开发文档

## 1. 文档概述

### 1.1 文档目的

本文档详细描述自定义实现的 `demo::stack` 容器适配器的设计思路、核心功能、接口规范、使用方法及注意事项，为开发者提供完整的开发、测试和维护参考，同时对齐 C++ 标准库 `std::stack` 的接口设计，保证兼容性和易用性。

### 1.2 适配器简介

`demo::stack` 是一个遵循 C++17 标准的**容器适配器**，提供**后进先出（LIFO, Last In First Out）**的数据结构。`stack` 封装了一个底层序列容器，仅允许在容器的一端（栈顶）进行插入和删除操作，默认使用 `demo::deque` 作为底层容器。

### 1.3 适用范围

- 编译环境：兼容 GCC、Clang、MSVC 等主流编译器，需开启 C++17 及以上标准（`-std=c++17`）；
- 元素类型：支持任意可拷贝/可移动/可析构的类型（内置类型、`std::string`、自定义类等）；
- 底层容器：要求支持 `back()`、`push_back()`、`pop_back()` 操作的序列容器（默认 `deque`，也支持 `vector`、`list` 等）；
- 应用场景：适合需要后进先出操作的场景（如函数调用栈、表达式求值、撤销操作、回溯算法等）。

## 2. 设计思路

### 2.1 核心架构

#### 2.1.1 容器适配器模式

`stack` 是一个典型的容器适配器，本身不直接存储元素，而是通过封装一个底层容器来实现栈的功能：

```c++
template <typename T, class Container = deque<T>>
class stack {
protected:
    Container m_container;  // 底层容器
    // ...
};
```

#### 2.1.2 容器核心成员

| 成员变量 | 类型 | 作用 |
| :------: | :--: | :--: |
| `m_container` | `Container` | 底层序列容器，实际存储栈中的所有元素 |

#### 2.1.3 架构优势

- **简单高效**：通过适配器模式复用已有的序列容器，无需重新实现底层数据结构；
- **灵活性强**：可自定义底层容器（如对内存效率有要求可用 `vector`，对频繁插入删除性能有要求可用 `list`）；
- **接口简洁**：提供最小化的栈操作接口，符合 LIFO 语义；
- **异常安全**：依赖底层容器的异常保证，简化异常处理；
- **标准兼容**：完全对齐 `std::stack` 的接口设计，易于迁移和使用。

### 2.2 底层容器选择

`stack` 默认使用 `demo::deque` 作为底层容器，用户也可选择其他符合要求的容器：

| 底层容器 | 特点 | 适用场景 |
| :------: | :--: | :------: |
| `deque`（默认） | 双端队列，首尾操作高效，内存分块 | 通用场景，平衡性能和内存 |
| `vector` | 动态数组，随机访问高效，内存连续 | 内存敏感场景，栈大小相对稳定 |
| `list` | 双向链表，任意位置插入删除高效 | 栈大小频繁大幅变化，内存碎片容忍度高 |

### 2.3 内存管理策略

1. **完全依赖底层容器**：所有内存管理操作（分配、释放、构造、析构）均由底层容器负责；
2. **异常安全**：继承底层容器的异常保证，底层容器提供的保证即为 stack 的保证；
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
| `difference_type` | 有符号整数类型（`Container::difference_type`） |

### 3.2 构造与析构

| 接口 | 功能说明 | 示例 |
| :--: | :------: | :--: |
| `stack()` | 默认构造，创建空栈 | `demo::stack<int> stk;` |
| `explicit stack(const Container& cont)` | 从容器拷贝构造 | `demo::deque<int> d{1,2,3}; demo::stack<int> stk(d);` |
| `explicit stack(Container&& cont)` | 从容器移动构造 | `demo::stack<int> stk(std::move(d));` |
| `stack(const stack& other)` | 拷贝构造 | `demo::stack<int> stk2(stk1);` |
| `stack(stack&& other) noexcept` | 移动构造 | `demo::stack<int> stk2(std::move(stk1));` |
| `~stack()` | 析构函数 | - |

### 3.3 赋值操作

| 接口 | 功能说明 | 示例 |
| :--: | :------: | :--: |
| `stack& operator=(const stack& other)` | 拷贝赋值 | `stk2 = stk1;` |
| `stack& operator=(stack&& other) noexcept` | 移动赋值 | `stk2 = std::move(stk1);` |

### 3.4 元素访问

| 接口 | 功能说明 | 注意事项 |
| :--: | :------: | :------: |
| `reference top()` | 返回栈顶元素的引用 | 在空栈上调用会导致未定义行为 |
| `const_reference top() const` | 返回栈顶元素的常量引用 | 在空栈上调用会导致未定义行为 |

### 3.5 容量操作

| 接口 | 功能说明 | 示例 |
| :--: | :------: | :--: |
| `bool empty() const` | 判断栈是否为空 | `if (stk.empty()) { ... }` |
| `size_type size() const` | 返回栈中元素的个数 | `auto sz = stk.size();` |

### 3.6 元素修改操作

| 接口 | 功能说明 | 时间复杂度 |
| :--: | :------: | :--------: |
| `void push(const value_type& value)` | 在栈顶插入元素（拷贝版本） | 依赖底层容器，通常 O(1) |
| `void push(value_type&& value)` | 在栈顶插入元素（移动版本） | 依赖底层容器，通常 O(1) |
| `template <class... Args> decltype(auto) emplace(Args&&... args)` | 在栈顶原地构造元素 | 依赖底层容器，通常 O(1) |
| `void pop()` | 移除栈顶元素 | 依赖底层容器，通常 O(1) |
| `void swap(stack& other)` | 交换两个栈的内容 | 依赖底层容器，通常 O(1) |

### 3.7 比较运算符

| 运算符 | 功能说明 | 示例 |
| :----: | :------: | :--: |
| `bool operator==(const stack& lhs, const stack& rhs)` | 比较两个栈是否相等 | `if (stk1 == stk2) { ... }` |
| `bool operator!=(const stack& lhs, const stack& rhs)` | 比较两个栈是否不相等 | `if (stk1 != stk2) { ... }` |

## 4. 使用示例

### 4.1 基础使用

```c++
#include "stack.h"
#include "../deque/deque.h"
#include "../vector/vector.h"
#include <iostream>
#include <string>

int main() {
    // 1. 构造 stack（默认使用 deque 作为底层容器）
    demo::stack<std::string> stk;
    
    // 2. 压入元素
    stk.push("hello");
    stk.push("world");
    stk.emplace("!");
    
    // 3. 访问栈顶元素
    std::cout << "栈顶元素：" << stk.top() << std::endl;
    
    // 4. 弹出元素
    stk.pop();
    std::cout << "弹出后栈顶元素：" << stk.top() << std::endl;
    
    // 5. 查看容量
    std::cout << "栈大小：" << stk.size() << std::endl;
    std::cout << "栈是否为空：" << (stk.empty() ? "是" : "否") << std::endl;
    
    // 6. 使用自定义底层容器
    demo::stack<int, demo::vector<int>> vec_stack;
    vec_stack.push(1);
    vec_stack.push(2);
    vec_stack.push(3);
    std::cout << "vector 栈大小：" << vec_stack.size() << std::endl;
    
    // 7. 拷贝和移动
    demo::stack<std::string> stk2(stk);  // 拷贝
    demo::stack<std::string> stk3(std::move(stk2));  // 移动
    
    // 8. 比较
    if (stk == stk3) {
        std::cout << "stk 和 stk3 相等" << std::endl;
    }
    
    return 0;
}
```

### 4.2 输出结果

```
栈顶元素：!
弹出后栈顶元素：world
栈大小：2
栈是否为空：否
vector 栈大小：3
stk 和 stk3 相等
```

## 5. 异常处理

| 异常场景 | 抛出类型 | 预防措施 |
| :------: | :------: | :------: |
| `top()` 或 `pop()` 在空栈上调用 | 未定义行为 | 调用前使用 `empty()` 检查 |
| 构造/插入元素时内存分配失败 | 依赖底层容器 | 捕获异常或提前检查可用内存 |
| 元素构造/析构失败 | 依赖元素类型 | 确保元素类型满足异常安全要求 |

## 6. 性能说明

### 6.1 时间复杂度

所有 `stack` 操作的时间复杂度完全依赖于底层容器的实现：

| 操作 | 时间复杂度（默认 deque） | 备注 |
| :--: | :----------------------: | :--: |
| `top()` | O(1) | 直接访问容器末尾 |
| `push()`/`emplace()` | O(1) 均摊 | 可能触发底层容器扩容 |
| `pop()` | O(1) | 直接删除容器末尾元素 |
| `size()`/`empty()` | O(1) | 直接返回容器大小信息 |
| `swap()` | O(1) | 交换底层容器 |
| 拷贝构造/赋值 | O(n) | 拷贝所有元素 |
| 移动构造/赋值 | O(1) | 移动底层容器 |

### 6.2 性能优化建议

1. **选择合适的底层容器**：
   - 通用场景使用默认的 `deque`
   - 对内存效率有要求使用 `vector`
   - 对频繁插入删除性能有要求使用 `list`

2. **使用 `emplace` 替代 `push`**：直接在栈顶原地构造元素，避免不必要的拷贝/移动开销

3. **预先估计栈大小**：如果使用 `vector` 作为底层容器，可预先调用底层容器的 `reserve()` 方法预留空间（需直接访问底层容器或继承 stack）

4. **使用移动语义**：对于支持移动语义的元素类型，使用移动版本的 `push` 或直接 `emplace`

## 7. 与 std::stack 的差异

| 特性 | demo::stack | std::stack |
| :--: | :---------: | :--------: |
| 底层容器 | 默认使用 demo::deque | 默认使用 std::deque |
| 接口设计 | 完全对齐标准库 | - |
| 异常保证 | 继承自底层容器 | 继承自底层容器 |
| 可移植性 | 自定义实现，需包含 stack.h | 标准库容器，直接包含 &lt;stack&gt; |

## 8. 已知限制

1. **不支持遍历**：`stack` 不提供迭代器，无法直接遍历所有元素（如需遍历可继承 stack 访问底层容器或改用其他容器）；
2. **不支持比较运算符（&lt;/&gt;/&lt;=/&gt;=）**：仅支持 `==`/`!=`；
3. **不支持直接访问底层容器**：底层容器为 `protected` 成员，仅派生类可访问；
4. **不支持范围构造**：不支持从迭代器范围构造（如需此功能可先构造底层容器再传递给 stack）。

## 9. 测试建议

1. 基础功能测试：构造、push、pop、top、size、empty 等核心接口；
2. 边界测试：空栈操作、单元素栈、大量元素压入/弹出；
3. 类型兼容性测试：测试内置类型、复杂类型（std::string）、自定义类的兼容性；
4. 自定义底层容器测试：测试使用 deque、vector、list 作为底层容器；
5. 拷贝/移动语义测试：测试拷贝构造、拷贝赋值、移动构造、移动赋值、swap 操作的正确性；
6. 异常测试：测试在空栈上调用 top()/pop() 的行为，以及元素构造/析构失败的处理；
7. 性能测试：对比 std::stack 的性能，测试不同底层容器的性能差异。

---

**文档版本**：v1.0
**创建日期**：2026-05-30
**适用容器**：`demo::stack<T, Container>`
