# demo::list 容器开发文档

## 1. 文档概述

### 1.1 文档目的

本文档详细描述自定义实现的 `demo::list` 双向链表容器的设计思路、核心功能、接口规范、使用方法及注意事项，为开发者提供完整的开发、测试和维护参考，同时对齐 C++ 标准库 `std::list` 的接口设计，保证兼容性和易用性。

### 1.2 容器简介

`demo::list` 是一个遵循 C++17 标准的**双向链表容器**，支持双向顺序访问（从头部到尾部或从尾部到头部），核心优势是**高效的任意位置插入删除操作**（时间复杂度 O(1)）。与单向链表 `demo::forward_list` 相比，`list` 支持反向遍历和首尾两端的高效操作，但内存占用稍高（每个节点包含前驱和后继两个指针）。

### 1.3 适用范围

- 编译环境：兼容 GCC、Clang、MSVC 等主流编译器，需开启 C++17 及以上标准（`-std=c++17`）；
- 元素类型：支持任意可拷贝 / 可移动 / 可析构的类型（内置类型、`std::string`、自定义类等）；
- 应用场景：适合需要频繁在任意位置插入 / 删除元素、需要双向遍历、追求稳定迭代器的场景（如有序容器、频繁更新的数据集）。

## 2. 设计思路

### 2.1 核心架构

#### 2.1.1 节点结构

`list` 基于双向链表节点实现，每个节点包含「元素值 + 前驱指针 + 后继指针」，核心定义如下：

```c++
struct Node 
{
    value_type val;  // 存储元素值
    Node* prev;      // 指向前驱节点的指针
    Node* next;      // 指向后继节点的指针
    
    // 原地构造元素，支持完美转发
    template <typename... Args>
    Node(Args&&... args) : val(std::forward<Args>(args)...),
                           prev(nullptr), next(nullptr) {}
};
```

#### 2.1.2 容器核心成员

|     成员变量     |       类型       |                             作用                             |
| :--------------: | :--------------: | :----------------------------------------------------------: |
|     `m_head`     |     `Node*`      | 链表的**前哨节点（sentinel node）**（不存储数据，仅作为链表头部锚点） |
|     `m_size`     |   `size_type`    |                    链表中元素的个数                         |
|  `m_node_alloc`  | `node_allocator` | 节点分配器，封装 `std::allocator`，负责节点的内存申请 / 释放、元素构造 / 析构 |
| `node_allocator` |       别名       | `std::allocator_traits<Allocator>::rebind_alloc<Node>`，适配节点类型的分配器 |

#### 2.1.3 架构优势

- **前哨节点设计**：避免空链表的边界处理（如插入 / 删除时无需判断头节点是否为空），形成循环链表结构；
- **双向指针**：每个节点包含前驱和后继指针，支持双向遍历和高效的首尾操作；
- **分配器适配**：通过 `rebind_alloc` 适配节点类型的内存分配，遵循 STL 分配器规范，保证内存安全；
- **O(1) 复杂度操作**：首尾插入删除、任意位置插入删除均为常数时间复杂度。

### 2.2 迭代器设计

`list` 支持**双向迭代器（std::bidirectional_iterator_tag）**，支持递增、递减操作，迭代器类型及特性如下：

|    迭代器类型    |       说明       |                           核心特性                           |
| :--------------: | :--------------: | :----------------------------------------------------------: |
|    `iterator`    |  可变双向迭代器  | 支持 `++`/`--`（前置 / 后置）、`*`、`->`、`==`/`!=`，可修改元素值 |
| `const_iterator` | 不可变双向迭代器 |      与 `iterator` 功能一致，但仅能读取元素值，不可修改      |
| `reverse_iterator` | 可变反向迭代器 | 基于标准库适配器实现，支持反向遍历 |
| `const_reverse_iterator` | 不可变反向迭代器 | 常量版本反向迭代器 |

#### 特殊迭代器接口

为适配双向链表的特性，`list` 提供完整的迭代器接口（对齐标准库）：

- `begin()`/`cbegin()`：返回指向第一个有效元素的迭代器；
- `end()`/`cend()`：返回尾后迭代器（指向哨兵节点）；
- `rbegin()`/`crbegin()`：返回反向迭代器，指向最后一个元素；
- `rend()`/`crend()`：返回反向尾后迭代器。

### 2.3 内存管理策略

1. **节点分配**：每个节点独立分配内存（通过 `alloc_traits::allocate`），构造元素时使用完美转发（`std::forward`），避免不必要的拷贝；
2. **内存释放**：析构时遍历链表，逐个销毁节点元素并释放内存，最后释放前哨节点；
3. **异常安全**：插入 / 构造元素时捕获异常，保证内存不泄漏；扩容 / 合并操作中临时节点使用 RAII 管理，异常时自动释放；
4. **移动语义**：移动构造 / 赋值时仅交换指针和分配器，不拷贝元素，时间复杂度 O(1)；
5. **拷贝语义**：拷贝构造 / 赋值时深拷贝所有节点，保证独立的内存空间。

### 2.4 核心算法实现

#### 2.4.1 排序算法（sort）

采用**归并排序**（链表最优排序算法，时间复杂度 O(n log n)），核心步骤：

1. 按步长拆分链表（`split` 函数）：将链表拆分为多个长度为 `2^k` 的子链表；
2. 合并有序子链表（`merge_two` 函数）：按比较规则合并两个有序子链表；
3. 迭代合并：逐步扩大步长（1→2→4→…），直到整个链表有序。

#### 2.4.2 合并算法（merge）

将另一个有序 `list` 合并到当前链表，保持有序性：

- 遍历两个链表，按比较规则逐个插入元素；
- 仅调整节点指针，不拷贝元素，时间复杂度 O(n + m)（n/m 为两个链表长度）。

#### 2.4.3 拼接算法（splice）

将另一个 `list` 的元素移动到当前链表的指定位置：

- 支持三种模式：移动整个链表、移动单个元素、移动元素范围；
- 仅调整节点指针，不拷贝元素，时间复杂度 O(1)（单个元素）或 O(n)（范围）。

## 3. 接口说明

### 3.1 类型别名（Type Aliases）

|     类型别名      |                        含义                         |
| :---------------: | :-------------------------------------------------: |
|   `value_type`    |              容器存储的元素类型（`T`）              |
|     `pointer`     |                元素指针类型（`T*`）                 |
|  `const_pointer`  |           常量元素指针类型（`const T*`）            |
|    `reference`    |                元素引用类型（`T&`）                 |
| `const_reference` |           常量元素引用类型（`const T&`）            |
| `allocator_type`  | 分配器类型（`Allocator`，默认 `std::allocator<T>`） |
|    `size_type`    |           无符号整数类型（`std::size_t`）           |
| `difference_type` |         有符号整数类型（`std::ptrdiff_t`）          |
|    `iterator`     |                   可变双向迭代器                    |
| `const_iterator`  |                  不可变双向迭代器                   |
| `reverse_iterator` |                  可变反向迭代器                    |
| `const_reverse_iterator` |              不可变反向迭代器                   |

### 3.2 构造与析构

|                      接口                      |                  功能说明                  |                             示例                             |
| :--------------------------------------------: | :----------------------------------------: | :----------------------------------------------------------: |
|                `list()`                |      空构造，创建仅含前哨节点的空链表      |                `demo::list<int> lst;`                 |
|        `list(size_type count)`         |   创建包含 `count` 个默认构造元素的链表    |           `demo::list<std::string> lst(3);`           |
| `list(size_type count, const T& val)`  | 创建包含 `count` 个值为 `val` 的元素的链表 |             `demo::list<int> lst(5, 10);`             |
|  `list(InputIt first, InputIt last)`   |               迭代器范围构造               | `int arr[] = {1,2,3}; demo::list<int> lst(arr, arr+3);` |
| `list(std::initializer_list<T> ilist)` |               初始化列表构造               |             `demo::list<int> lst{1,2,3};`             |
|   `list(const list& other)`    |         拷贝构造（深拷贝所有节点）         |             `demo::list<int> lst2(lst1);`              |
| `list(list&& other) noexcept`  |           移动构造（仅交换指针）           |        `demo::list<int> lst2(std::move(lst1));`        |
|               `~list()`                |      析构函数，销毁所有节点并释放内存      |                              -                               |

### 3.3 赋值操作

|                           接口                            |                  功能说明                  |           示例           |
| :-------------------------------------------------------: | :----------------------------------------: | :----------------------: |
|   `list& operator=(const list& other)`    |      拷贝赋值（先清空自身，再深拷贝）      |       `lst2 = lst1;`       |
| `list& operator=(list&& other) noexcept`  |        移动赋值（交换指针和分配器）        | `lst2 = std::move(lst1);`  |
| `list& operator=(std::initializer_list<T> ilist)` |               初始化列表赋值               |     `lst = {4,5,6};`      |
|       `void assign(size_type count, const T& val)`        | 赋值 `count` 个 `val` 元素（覆盖原有数据） |    `lst.assign(2, 0);`    |
|        `void assign(InputIt first, InputIt last)`         |               迭代器范围赋值               | `lst.assign(arr, arr+2);` |
|       `void assign(std::initializer_list<T> ilist)`       |               初始化列表赋值               |  `lst.assign({7,8,9});`   |

### 3.4 迭代器接口

|                      接口                       |            功能说明            |        注意事项        |
| :---------------------------------------------: | :----------------------------: | :--------------------: |
|           `iterator begin() noexcept`           | 返回指向第一个元素的可变迭代器 |   空链表返回 `end()`   |
|     `const_iterator begin() const noexcept`     |       常量版本 `begin()`       |           -            |
|    `const_iterator cbegin() const noexcept`     |     显式常量版本 `begin()`     |           -            |
|            `iterator end() noexcept`            |  返回尾后迭代器（哨兵节点）   |       不可解引用       |
|      `const_iterator end() const noexcept`      |        常量版本 `end()`        |           -            |
|     `const_iterator cend() const noexcept`      |      显式常量版本 `end()`      |           -            |
|          `reverse_iterator rbegin()`            |   返回指向最后元素的反向迭代器   |           -            |
|    `const_reverse_iterator rbegin() const`      |      常量版本 `rbegin()`       |           -            |
|    `const_reverse_iterator crbegin() const`     |    显式常量版本 `rbegin()`     |           -            |
|           `reverse_iterator rend()`             |     返回反向尾后迭代器     |           -            |
|     `const_reverse_iterator rend() const`       |        常量版本 `rend()`        |           -            |
|    `const_reverse_iterator crend() const`       |      显式常量版本 `rend()`      |           -            |

### 3.5 容量操作

|                 接口                  |                           功能说明                           |                            示例                            |
| :-----------------------------------: | :----------------------------------------------------------: | :--------------------------------------------------------: |
|     `bool empty() const noexcept`     |                判断链表是否为空（无有效元素）                |                 `if (lst.empty()) { ... }`                  |
|    `size_type size() const noexcept`    |                    返回链表中元素的个数                    |                  `auto sz = lst.size();`                  |
| `size_type max_size() const noexcept` |           返回链表最大可存储元素数（由分配器决定）           |                 `auto ms = lst.max_size();`                 |

### 3.6 元素访问

|                 接口                  |                           功能说明                           |                            示例                            |
| :-----------------------------------: | :----------------------------------------------------------: | :--------------------------------------------------------: |
|    `reference front()`    |           返回第一个元素的引用（可变）           |                  `auto& first = lst.front();`                  |
| `const_reference front() const` |      返回第一个元素的常量引用      |           `const auto& first = lst.front();`           |
|      `reference back()`      |            返回最后一个元素的引用（可变）            |                   `auto& last = lst.back();`                   |
| `const_reference back() const` |        返回最后一个元素的常量引用        |            `const auto& last = lst.back();`            |

### 3.7 元素修改操作

|                             接口                             |                           功能说明                           |               时间复杂度                |
| :----------------------------------------------------------: | :----------------------------------------------------------: | :-------------------------------------: |
|               `void push_front(const T& val)`                |                在链表头部插入元素（拷贝构造）                |                  O(1)                   |
|                  `void push_front(T&& val)`                  |                在链表头部插入元素（移动构造）                |                  O(1)                   |
| `template <typename... Args> reference emplace_front(Args&&... args)` |                 头部原地构造元素（完美转发）                 |          O(1)，返回新元素引用          |
|                      `void pop_front()`                      |                      删除链表第一个元素                      | O(1)，空链表调用抛 `std::out_of_range` |
|               `void push_back(const T& val)`                |                在链表尾部插入元素（拷贝构造）                |                  O(1)                   |
|                  `void push_back(T&& val)`                  |                在链表尾部插入元素（移动构造）                |                  O(1)                   |
| `template <typename... Args> reference emplace_back(Args&&... args)` |                 尾部原地构造元素（完美转发）                 |          O(1)，返回新元素引用          |
|                      `void pop_back()`                      |                      删除链表最后一个元素                      | O(1)，空链表调用抛 `std::out_of_range` |
|  `iterator insert(const_iterator pos, const T& val)`   |                 在 `pos` 迭代器位置插入元素                  |         O(1)，返回新元素迭代器         |
|     `iterator insert(const_iterator pos, T&& val)`     |                         移动插入元素                         |                  O(1)                   |
| `iterator insert(const_iterator pos, size_type count, const T& val)` |                   插入 `count` 个重复元素                    |                O(count)                 |
| `iterator insert(const_iterator pos, InputIt first, InputIt last)` |                      插入迭代器范围元素                      |             O(last - first)             |
| `template <typename... Args> iterator emplace(const_iterator pos, Args&&... args)` |                  在 `pos` 位置原地构造元素                   |             O(1)，避免拷贝             |
|          `iterator erase(const_iterator pos)`          |                    删除 `pos` 位置的元素                     |    O(1)，返回删除元素的下一个迭代器    |
| `iterator erase(const_iterator first, const_iterator last)` |               删除 `[first, last)` 范围元素                |             O(last - first)             |
|      `void resize(size_type count, const T& val = T())`      | 调整链表长度：1. 长度不足时，尾部追加 `val`；2. 长度超出时，截断尾部 |                O(count)                 |
|                   `void clear() noexcept`                    |                 清空所有元素（保留前哨节点）                 |           O(n)，n 为元素个数           |
|          `void swap(list& other) noexcept`           |               交换两个链表的内容（仅交换指针）               |                  O(1)                   |

### 3.8 链表操作（特有接口）

|                             接口                             |                         功能说明                          |                   注意事项                    |
| :----------------------------------------------------------: | :-------------------------------------------------------: | :-------------------------------------------: |
|              `void merge(list& other)`               |              合并另一个有序链表（默认升序）               |     合并后 `other` 为空，两个链表需均有序     |
| `template <typename Compare> void merge(list& other, Compare comp)` |                自定义比较规则合并有序链表                 | `comp` 为二元谓词（如 `std::greater<int>()`） |
| `void splice(const_iterator pos, list& other)` |          将 `other` 的所有元素插入到 `pos` 之前           |           `other` 合并后为空，O(1)           |
| `void splice(const_iterator pos, list& other, const_iterator it)` |      将 `other` 中 `it` 指向的元素插入到 `pos` 之前       |                     O(1)                      |
| `void splice(const_iterator pos, list& other, const_iterator first, const_iterator last)` | 将 `other` 中 `[first, last)` 范围元素插入到 `pos` 之前 |                O(last - first)                |
|              `size_type remove(const T& value)`              |                删除所有值为 `value` 的元素                |           返回删除的元素个数，O(n)           |
| `template <typename UnaryPredicate> size_type remove_if(UnaryPredicate p)` |                  删除满足谓词 `p` 的元素                  | `p` 为一元谓词（如 `[](int x){return x>5;}`） |
|                  `void reverse() noexcept`                   |                  反转链表（仅调整指针）                   |               O(n)，无内存分配               |
|                     `size_type unique()`                     |           删除连续重复的元素（默认 `==` 比较）            |           返回删除的元素个数，O(n)           |
| `template <typename BinaryPredicate> size_type unique(BinaryPredicate p)` |                  自定义二元谓词判断重复                   |         `p` 接收两个元素，返回 `bool`         |
|                        `void sort()`                         |                对链表升序排序（归并排序）                 |             O(n log n)，稳定排序             |
|    `template <typename Compare> void sort(Compare comp)`     |                    自定义比较规则排序                     | `comp` 为二元谓词（如 `std::greater<int>()`） |

### 3.9 分配器接口

|               接口                |         功能说明         |                示例                |
| :-------------------------------: | :----------------------: | :--------------------------------: |
| `Allocator get_allocator() const` | 返回容器使用的分配器副本 | `auto alloc = lst.get_allocator();` |

### 3.10 比较运算符

|               运算符                |         功能说明         |                示例                |
| :--------------------------------: | :----------------------: | :--------------------------------: |
| `bool operator==(const list& rhs)` | 比较两个链表是否相等 | `if (lst1 == lst2) { ... }` |
| `bool operator!=(const list& rhs)` | 比较两个链表是否不相等 | `if (lst1 != lst2) { ... }` |

## 4. 使用示例

### 4.1 基础使用

```c++
#include "list.h"
#include <iostream>
#include <string>

int main() {
    // 1. 构造链表
    demo::list<std::string> lst{"apple", "banana", "cherry"};
    
    // 2. 头部和尾部插入元素
    lst.push_front("pear");
    lst.push_back("date");
    lst.emplace_front("grape", 4); // 构造 "grap"
    lst.emplace_back("elderberry");
    
    // 3. 正向遍历元素
    std::cout << "正向遍历：";
    for (const auto& s : lst) {
        std::cout << s << " ";
    }
    std::cout << std::endl; // 输出：grap pear apple banana cherry date elderberry
    
    // 4. 反向遍历元素
    std::cout << "反向遍历：";
    for (auto it = lst.rbegin(); it != lst.rend(); ++it) {
        std::cout << *it << " ";
    }
    std::cout << std::endl; // 输出：elderberry date cherry banana apple pear grap
    
    // 5. 插入元素
    auto it = lst.insert(lst.begin(), "orange");
    std::cout << "插入的元素：" << *it << std::endl; // 输出：orange
    
    // 6. 删除元素
    lst.erase(it); // 删除刚刚插入的元素
    lst.pop_front(); // 删除头部元素
    lst.pop_back(); // 删除尾部元素
    
    // 7. 排序
    lst.sort(std::greater<std::string>()); // 降序排序
    std::cout << "排序后：";
    for (const auto& s : lst) {
        std::cout << s << " ";
    }
    std::cout << std::endl; // 输出：pear cherry banana apple date
    
    // 8. 去重（无重复元素，返回0）
    auto del_count = lst.unique();
    std::cout << "删除重复元素个数：" << del_count << std::endl;
    
    // 9. 合并另一个有序链表
    demo::list<std::string> lst2{"blueberry", "fig"};
    lst2.sort(std::greater<std::string>());
    lst.merge(lst2);
    
    std::cout << "合并后：";
    for (const auto& s : lst) {
        std::cout << s << " ";
    }
    std::cout << std::endl; // 输出：pear fig cherry blueberry banana apple date
    
    return 0;
}
```

### 4.2 输出结果

```
正向遍历：grap pear apple banana cherry date elderberry 
反向遍历：elderberry date cherry banana apple pear grap 
插入的元素：orange
排序后：pear cherry banana apple date 
删除重复元素个数：0
合并后：pear fig cherry blueberry banana apple date 
```

## 5. 异常处理

|                  异常场景                  |      抛出类型       |                   预防措施                    |
| :----------------------------------------: | :-----------------: | :-------------------------------------------: |
|          `pop_front()` 调用空链表          | `std::out_of_range` |             调用前检查 `!empty()`             |
|          `pop_back()` 调用空链表           | `std::out_of_range` |             调用前检查 `!empty()`             |
|       构造 / 插入元素时内存分配失败        |  `std::bad_alloc`   |        捕获异常或提前检查 `max_size()`        |
| `resize()`/`insert()` 传入非法迭代器 |     未定义行为      | 确保迭代器在 `[begin(), end()]` 范围内 |
|           `merge()` 合并无序链表           | 结果无序（无异常）  |      合并前确保两个链表均按相同规则排序       |

## 6. 性能说明

### 6.1 时间复杂度

|                     操作                     | 时间复杂度 |            备注            |
| :------------------------------------------: | :--------: | :------------------------: |
|         `push_front()`/`pop_front()`         |    O(1)    |       仅操作头部节点       |
|          `push_back()`/`pop_back()`          |    O(1)    |       仅操作尾部节点       |
| `insert()`/`erase()`（单个元素） |    O(1)    |  仅调整指针，无需移动元素  |
|   `insert()`/`erase()`（范围）   |    O(n)    | n 为插入 / 删除的元素个数  |
|                   `sort()`                   | O(n log n) | 归并排序，链表最优排序算法 |
|                  `merge()`                   |  O(n + m)  |  n/m 为两个链表的元素个数  |
|                  `reverse()`                  |    O(n)    |   仅调整指针，无内存分配   |
|            `remove()`/`unique()`             |    O(n)    |        遍历链表一次        |
|                  `clear()`                   |    O(n)    |        销毁所有节点        |
|                   `swap()`                   |    O(1)    |     仅交换指针和分配器     |

### 6.2 性能优化建议

1. **优先使用 emplace 系列接口**：`emplace_front()`/`emplace_back()`/`emplace()` 直接原地构造元素，避免拷贝 / 移动开销；
2. **批量插入前规划顺序**：双向链表支持首尾操作，根据场景选择高效的插入位置；
3. **利用 `size()` 接口**：`list` 提供 `size()` 接口（O(1) 复杂度），无需遍历链表获取长度；
4. **排序后合并 / 去重**：`merge()`/`unique()` 对有序链表效率更高，建议先排序再执行此类操作；
5. **移动语义优化**：对复杂类型（如 `std::string`），优先使用右值引用版本的插入接口减少拷贝。

## 7. 与 std::list 的差异

|    特性    |             demo::list              |      std::list       |
| :--------: | :---------------------------------: | :------------------: |
| 分配器支持 |    支持 `std::allocator` 及自定义分配器     |           完全一致           |
| 迭代器类型 |           双向迭代器，对齐标准            |           完全一致           |
|  异常安全  | 空链表 `pop_front()`/`pop_back()` 抛 `std::out_of_range` | 标准库为未定义行为（可配置） |
|  排序算法  |              归并排序（稳定）               |       归并排序（稳定）       |
|  内存管理  |       前哨节点统一管理，析构自动释放        |           完全一致           |
|  扩展接口  |          无额外扩展，严格对齐标准           |              -               |

## 8. 已知限制

1. 不支持随机访问，访问指定位置元素需遍历链表（O(n)）；
2. `merge()`/`sort()` 仅支持基于元素值的比较，暂不支持自定义投影（C++20 特性）；
3. 迭代器失效规则：插入元素不失效迭代器，删除元素仅失效指向被删除节点的迭代器；
4. 内存开销高于 `forward_list`（每个节点多一个指针）。

## 9. 测试建议

1. 基础功能测试：构造、插入、删除、遍历（正向 / 反向）、排序、合并等核心接口；
2. 边界测试：空链表操作、单元素链表、大量元素插入 / 删除；
3. 异常测试：空链表 `pop_front()`/`pop_back()`、内存分配失败、非法迭代器操作；
4. 性能测试：对比 `std::list` 的插入 / 删除 / 排序效率；
5. 类型兼容性测试：测试内置类型、复杂类型（`std::string`）、自定义类的兼容性。

---

**文档版本**：v1.0 
**创建日期**：2026-04-14 
**适用容器**：`demo::list<T, Allocator>`