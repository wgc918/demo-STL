# demo::unordered_set 容器开发文档

## 1. 文档概述

### 1.1 文档目的

本文档详细描述自定义实现的 `demo::unordered_set` 容器的设计思路、核心功能、接口规范、使用方法及注意事项，为开发者提供完整的开发、测试和维护参考，同时对齐 C++ 标准库 `std::unordered_set` 的接口设计，保证兼容性和易用性。

### 1.2 容器简介

`demo::unordered_set` 是一个遵循 C++17 标准的**关联容器**，存储唯一的键（keys），键是唯一的，但**不保证任何特定顺序**。`unordered_set` 基于哈希表实现，采用链地址法（Separate Chaining）处理哈希冲突，保证平均 O(1) 的插入、删除和查找操作，适用于需要快速查找但不关心元素顺序且不允许重复元素的场景。

### 1.3 适用范围

- 编译环境：兼容 GCC、Clang、MSVC 等主流编译器，需开启 C++17 及以上标准（`-std=c++17`）；
- 键类型：支持任意可哈希的类型（通过 `Hash` 函数对象自定义哈希规则，默认 `std::hash<Key>`）；
- 值类型：支持任意可拷贝 / 可移动 / 可析构的类型（内置类型、`std::string`、自定义类等）；
- 应用场景：适合需要快速查找、插入、删除操作，且不要求有序遍历、不允许重复元素的场景（如去重系统、唯一性验证、快速查找集合）。

## 2. 设计思路

### 2.1 核心架构

#### 2.1.1 哈希桶节点结构

`unordered_set` 基于哈希表实现，每个桶存储一个链表，用于处理哈希冲突：

```c++
struct Node
{
    value_type value;      // 键
    Node*      next;       // 指向同桶下一个节点的指针
    uint64_t   hash_code;  // 键的哈希值缓存
};
```

#### 2.1.2 容器核心成员

|     成员变量     |         类型          |                              作用                               |
| :--------------: | :-------------------: | :-------------------------------------------------------------: |
|    `m_table`     |      `Node**`         |                      桶数组指针（指向指针数组）                  |
|    `m_mask`      |     `size_type`       |               桶数组掩码，用于快速计算桶索引（位运算）            |
| `m_bucket_count` |     `size_type`       |                      当前桶数量                                 |
|     `m_size`     |     `size_type`       |                      容器中元素的个数                           |
| `m_max_load_factor` |      `float`         |              最大负载因子阈值，超过时触发重哈希                  |
| `m_hash_function` |      `hash_type`      |              哈希函数对象，默认 `std::hash<Key>`               |
|   `m_key_eq`     |      `key_equal`      |              键比较函数对象，默认 `std::equal_to<Key>`          |
| `m_node_allocator` | `node_allocator_type` | 节点分配器，负责节点的内存申请 / 释放、元素构造 / 析构 |
| `m_bucket_allocator` | `bucket_allocator_type` | 桶数组分配器，负责桶数组的内存管理 |

#### 2.1.3 架构优势

- **平均 O(1) 复杂度**：哈希表保证平均情况下的常数时间查找、插入、删除操作；
- **链地址法冲突处理**：采用链表存储同桶元素，避免开放寻址法的聚集问题；
- **自动重哈希**：负载因子超过阈值时自动扩展桶数量，保持性能稳定；
- **哈希值缓存**：节点缓存键的哈希值，避免重复计算；
- **位运算优化**：桶数量始终为 2 的幂，通过位与运算快速定位桶索引；
- **键唯一性**：自动保证键的唯一性，重复键插入会被忽略。

### 2.2 迭代器设计

`unordered_set` 支持**前向迭代器（std::forward_iterator_tag）**，按桶顺序遍历所有元素：

|    迭代器类型    |         说明         |                           核心特性                            |
| :--------------: | :------------------: | :-----------------------------------------------------------: |
|    `iterator`    |   可变前向迭代器    | 支持 `++`（前置 / 后置）、`*`、`->`、`==`/`!=`，可遍历元素 |
| `const_iterator` | 不可变前向迭代器    |        与 `iterator` 功能一致，但仅能读取元素值，不可修改        |
| `local_iterator` |   桶内可变前向迭代器    |              仅在单个桶内遍历，不跨桶移动                  |
| `const_local_iterator` | 桶内不可变前向迭代器 |                    常量版本桶内迭代器                         |

#### 迭代器遍历顺序

`unordered_set` 的迭代器按桶顺序遍历所有元素：

1. 遍历桶数组（从桶 0 到桶 `bucket_count()-1`）；
2. 每个桶内按链表顺序遍历；
3. 遍历顺序**不保证与插入顺序或键的大小顺序一致**；
4. 重哈希后迭代顺序可能改变。

### 2.3 内存管理策略

1. **节点分配**：每个节点独立分配内存（通过 `node_allocator_type`），构造元素时使用完美转发（`std::forward`），避免不必要的拷贝；
2. **桶数组管理**：桶数组为指针数组，通过 `bucket_allocator_type` 管理，桶数量始终为 2 的幂；
3. **内存释放**：析构时销毁所有节点，释放桶数组；
4. **异常安全**：插入 / 构造元素时捕获异常，保证内存不泄漏；
5. **移动语义**：移动构造 / 赋值时仅交换指针和分配器，不拷贝元素，时间复杂度 O(1)；
6. **拷贝语义**：拷贝构造 / 赋值时重新分配桶数组并拷贝所有节点，保证独立的内存空间。

### 2.4 核心算法

#### 2.4.1 哈希函数与桶索引计算

哈希表的核心是将键映射到桶索引：

```c++
size_type bucket_index(const Key& key) const {
    uint64_t hash = m_hash_function(key);
    return hash & m_mask;  // 位与运算快速定位桶索引
}
```

**设计要点**：
- 桶数量始终为 2 的幂（通过 `next_power_of_two` 计算）；
- 使用位与运算 `hash & m_mask` 替代取模运算 `hash % bucket_count`，提升性能；
- 哈希值缓存到节点中，避免重复计算。

#### 2.4.2 重哈希算法（do_rehash）

当负载因子超过阈值时触发重哈希：

1. **计算新桶数量**：取大于等于当前桶数量 2 倍的最小 2 的幂；
2. **分配新桶数组**：使用 `bucket_allocator_type` 分配新的桶数组；
3. **重新分配节点**：遍历所有旧桶的节点，计算新桶索引，将节点插入新桶；
4. **释放旧桶数组**：销毁旧桶数组，更新 `m_table`、`m_mask`、`m_bucket_count`。

#### 2.4.3 负载因子管理

负载因子计算公式：`load_factor() = size() / bucket_count()`

**触发条件**：当 `load_factor() > max_load_factor()` 时自动调用 `rehash()`

**默认参数**：
- 默认桶数量：16（`UNORDERED_SET_DEFAULT_BUCKET_COUNT`）；
- 默认最大负载因子：1.0（`UNORDERED_SET_DEFAULT_LOAD_FACTOR`）。

## 3. 接口说明

### 3.1 类型别名（Type Aliases）

|         类型别名         |                        含义                         |
| :----------------------: | :-------------------------------------------------: |
|       `key_type`         |              键类型（`Key`）                          |
|     `value_type`         |              元素类型（与键类型相同，`Key`）          |
|        `pointer`         |                元素指针类型（`Key*`）                 |
|     `const_pointer`      |           常量元素指针类型（`const Key*`）            |
|       `reference`        |                元素引用类型（`Key&`）                 |
|    `const_reference`     |           常量元素引用类型（`const Key&`）            |
|     `allocator_type`     | 分配器类型（`Allocator`，默认 `std::allocator<Key>`） |
|       `size_type`        |           无符号整数类型（`std::size_t`）           |
|    `difference_type`     |         有符号整数类型（`std::ptrdiff_t`）          |
|    `hash_type`           |              哈希函数类型（`Hash`）           |
|    `key_equal`           |              键比较函数类型（`KeyEqual`）           |
|        `iterator`        |                   可变前向迭代器                    |
|     `const_iterator`     |                  不可变前向迭代器                   |
|    `local_iterator`      |                   桶内可变前向迭代器                    |
| `const_local_iterator`   |                  桶内不可变前向迭代器                   |

### 3.2 构造与析构

|                      接口                      |                  功能说明                  |                             示例                             |
| :--------------------------------------------: | :----------------------------------------: | :----------------------------------------------------------: |
|                `unordered_set()`                |     空构造，创建不含任何元素的空 unordered_set，默认桶数量 16       |                `demo::unordered_set<std::string> s;`                 |
|        `explicit unordered_set(size_type bucket_count)`         |   使用指定初始桶数量创建空 unordered_set   |            `demo::unordered_set<int> s(64);`            |
|        `unordered_set(InputIt first, InputIt last, size_type bucket_count = 16)`         |   迭代器范围构造   |            `std::vector<std::string> v{"a", "b", "c"}; demo::unordered_set<std::string> s(v.begin(), v.end());`            |
|        `unordered_set(std::initializer_list<value_type> ilist, size_type bucket_count = 16)`         |   初始化列表构造   |            `demo::unordered_set<int> s{1, 2, 3};`            |
|        `unordered_set(const unordered_set& other)`         |            拷贝构造（深拷贝所有节点）             |            `demo::unordered_set<int> s2(s1);`            |
|          `unordered_set(unordered_set&& other) noexcept`          |            移动构造（仅交换指针）            |        `demo::unordered_set<int> s2(std::move(s1));`        |
|               `~unordered_set()`                |            析构函数，销毁所有节点并释放内存            |                              -                               |

### 3.3 赋值操作

|                           接口                            |                  功能说明                  |           示例           |
| :-------------------------------------------------------: | :----------------------------------------: | :----------------------: |
|   `unordered_set& operator=(const unordered_set& other)`    |            拷贝赋值（先清空自身，再深拷贝）            |       `s2 = s1;`       |
| `unordered_set& operator=(unordered_set&& other) noexcept`  |            移动赋值（交换指针和分配器）            | `s2 = std::move(s1);`  |
| `unordered_set& operator=(std::initializer_list<value_type> ilist)` |            初始化列表赋值            |     `s = {1, 2, 3};`      |

### 3.4 迭代器接口

|                      接口                       |            功能说明            |        注意事项        |
| :---------------------------------------------: | :----------------------------: | :--------------------: |
|           `iterator begin() noexcept`           | 返回指向第一个元素的可变迭代器 |   空容器返回 `end()`   |
|     `const_iterator begin() const noexcept`     |       常量版本 `begin()`       |           -            |
|    `const_iterator cbegin() const noexcept`     |     显式常量版本 `begin()`     |           -            |
|            `iterator end() noexcept`            |      返回尾后迭代器（nullptr）      |       不可解引用       |
|      `const_iterator end() const noexcept`       |        常量版本 `end()`        |           -            |
|     `const_iterator cend() const noexcept`       |      显式常量版本 `end()`      |           -            |

### 3.5 桶迭代器接口

|                      接口                       |            功能说明            |        注意事项        |
| :---------------------------------------------: | :----------------------------: | :--------------------: |
|           `local_iterator begin(size_type n)`           | 返回桶 n 的起始迭代器 |   n 必须 < bucket_count()   |
|     `const_local_iterator begin(size_type n) const`     |       常量版本桶起始迭代器       |           -            |
|    `const_local_iterator cbegin(size_type n) const`     |     显式常量版本桶起始迭代器     |           -            |
|            `local_iterator end(size_type n)`            |      返回桶 n 的尾后迭代器      |       不可解引用       |
|      `const_local_iterator end(size_type n) const`       |        常量版本桶尾后迭代器        |           -            |
|     `const_local_iterator cend(size_type n) const`       |      显式常量版本桶尾后迭代器      |           -            |

### 3.6 容量操作

|                 接口                  |                           功能说明                           |                            示例                            |
| :-----------------------------------: | :----------------------------------------------------------: | :--------------------------------------------------------: |
|     `bool empty() const noexcept`     |                判断 unordered_set 是否为空（无有效元素）                |                 `if (s.empty()) { ... }`                  |
|    `size_type size() const noexcept`    |                    返回 unordered_set 中元素的个数                    |                  `auto sz = s.size();`                  |
| `size_type max_size() const noexcept` |           返回 unordered_set 最大可存储元素数（由分配器决定）           |                 `auto ms = s.max_size();`                  |

### 3.7 元素修改操作

|                             接口                             |                           功能说明                           |               时间复杂度                |
| :----------------------------------------------------------: | :----------------------------------------------------------: | :-------------------------------------: |
|               `std::pair<iterator, bool> insert(const value_type& value)`                |           插入键（拷贝版本）           |         O(1) 平均，O(n) 最坏         |
|              `std::pair<iterator, bool> insert(value_type&& value)`               |           插入键（移动版本）           |         O(1) 平均，O(n) 最坏         |
|              `iterator insert(const_iterator hint, const value_type& value)`              |     在 `hint` 位置附近插入键（拷贝版本，hint 仅用于兼容性）     |         O(1) 平均，O(n) 最坏         |
|              `iterator insert(const_iterator hint, value_type&& value)`              |     在 `hint` 位置附近插入键（移动版本，hint 仅用于兼容性）     |         O(1) 平均，O(n) 最坏         |
| `void insert(InputIt first, InputIt last)` |                      插入迭代器范围的键                      |             O(m) 平均，m 为插入元素个数             |
| `void insert(std::initializer_list<value_type> ilist)` |                      插入初始化列表的键                      |             O(m) 平均，m 为插入元素个数             |
| `template <typename... Args> std::pair<iterator, bool> emplace(Args&&... args)` | 原位构造元素并插入 | O(1) 平均，O(n) 最坏 |
| `template <typename... Args> iterator emplace_hint(const_iterator hint, Args&&... args)` | 原位构造元素并插入（带提示，hint 仅用于兼容性） | O(1) 平均，O(n) 最坏 |
|          `iterator erase(iterator pos)`          |                    删除 `pos` 位置的键                     |    O(1) 平均，O(n) 最坏    |
|          `iterator erase(const_iterator pos)`          |                    删除 `pos` 位置的键（常量版本）                     |    O(1) 平均，O(n) 最坏    |
| `iterator erase(const_iterator first, const_iterator last)` |               删除 `[first, last)` 范围的键                |             O(m) 平均，m 为删除元素个数             |
|          `size_type erase(const Key& key)`          |                    删除指定键的键                     |    O(1) 平均，O(n) 最坏    |
|            `void clear() noexcept`                    |                 清空所有键（销毁所有节点）                 |           O(n)             |
|          `void swap(unordered_set& other)`           |               交换两个 unordered_set 的内容（仅交换指针）               |                  O(1)                  |
| `template <typename H2, typename P2> void merge(unordered_set<Key, H2, P2, Allocator>& source)` | 合并另一个 unordered_set 的元素到当前容器（键冲突时跳过重复键） | O(m) 平均，m 为源容器元素个数 |

### 3.8 查找操作

|                             接口                             |                           功能说明                           |               时间复杂度                |
| :----------------------------------------------------------: | :----------------------------------------------------------: | :-------------------------------------: |
|          `size_type count(const Key& key) const`          |  统计指定键的元素数量（unordered_set 中键唯一，返回 0 或 1）  |         O(1) 平均，O(n) 最坏         |
|            `iterator find(const Key& key)`            |           查找指定键的元素           |         O(1) 平均，O(n) 最坏         |
|     `const_iterator find(const Key& key) const`      |           查找指定键的元素（常量版本）           |         O(1) 平均，O(n) 最坏         |
| `std::pair<iterator, iterator> equal_range(const Key& key)` | 返回等于指定键的元素范围（unordered_set 中范围最多包含一个元素） | O(1) 平均，O(n) 最坏 |
| `std::pair<const_iterator, const_iterator> equal_range(const Key& key) const` | 返回等于指定键的元素范围（常量版本） | O(1) 平均，O(n) 最坏 |

### 3.9 桶接口

|                 接口                  |                           功能说明                           |                            示例                            |
| :-----------------------------------: | :----------------------------------------------------------: | :--------------------------------------------------------: |
|    `size_type bucket_count() const`    |                    返回当前桶数量                    |                  `auto bc = s.bucket_count();`                  |
| `size_type max_bucket_count() const` |           返回最大桶数量（受限于 size_type）           |                 `auto mbc = s.max_bucket_count();`                  |
|    `size_type bucket_size(size_type n) const`    |                    返回桶 n 中的元素数量                    |                  `auto bs = s.bucket_size(0);`                  |
|    `size_type bucket(const Key& key) const`    |                    返回键 key 所在的桶索引                    |                  `auto idx = s.bucket("one");`                  |

### 3.10 哈希策略接口

|                 接口                  |                           功能说明                           |                            示例                            |
| :-----------------------------------: | :----------------------------------------------------------: | :--------------------------------------------------------: |
|    `float load_factor() const`    |                    返回当前负载因子                    |                  `auto lf = s.load_factor();`                  |
|    `float max_load_factor() const`    |                    返回最大负载因子                    |                  `auto mlf = s.max_load_factor();`                  |
|    `void max_load_factor(float new_max_load_factor)`    |                    设置最大负载因子                    |                  `s.max_load_factor(0.75f);`                  |
|    `void rehash(size_type new_bucket_count)`    |                    重新哈希，改变桶数量                    |                  `s.rehash(1024);`                  |
|    `void reserve(size_type count)`    |                    预留空间以容纳至少 count 个元素                    |                  `s.reserve(1000);`                  |

### 3.11 观察器接口

|               接口                |         功能说明         |                示例                |
| :-------------------------------: | :----------------------: | :--------------------------------: |
| `hash_type hash_function() const` | 返回哈希函数对象的副本 | `auto hash_fn = s.hash_function();` |
| `key_equal key_eq() const` | 返回键比较函数对象的副本 | `auto eq_fn = s.key_eq();` |

### 3.12 分配器接口

|               接口                |         功能说明         |                示例                |
| :-------------------------------: | :----------------------: | :--------------------------------: |
| `allocator_type get_allocator() const` | 返回容器使用的分配器副本 | `auto alloc = s.get_allocator();` |

### 3.13 比较运算符

|               运算符                |         功能说明         |                示例                |
| :--------------------------------: | :----------------------: | :--------------------------------: |
| `bool operator==(const unordered_set& rhs)` | 比较两个 unordered_set 是否相等 | `if (s1 == s2) { ... }` |
| `bool operator!=(const unordered_set& rhs)` | 比较两个 unordered_set 是否不相等 | `if (s1 != s2) { ... }` |

## 4. 使用示例

### 4.1 基础使用

```c++
#include "unordered_set.h"
#include <iostream>
#include <string>

int main() {
    // 1. 构造 unordered_set
    demo::unordered_set<std::string> s{"apple", "banana", "cherry"};

    // 2. 插入元素
    s.insert("date");
    s.emplace("elderberry");
    auto [it, inserted] = s.emplace("fig");
    std::cout << "fig 插入状态：" << (inserted ? "成功" : "已存在") << std::endl;

    // 3. 查找元素
    auto find_it = s.find("cherry");
    if (find_it != s.end()) {
        std::cout << "找到 cherry：" << *find_it << std::endl;
    }

    // 4. 遍历（无序）
    std::cout << "遍历元素：";
    for (const auto& key : s) {
        std::cout << key << " ";
    }
    std::cout << std::endl;

    // 5. 插入重复元素
    auto [it2, inserted2] = s.insert("apple");
    std::cout << "重复插入 apple 状态：" << (inserted2 ? "成功" : "已存在，忽略") << std::endl;
    std::cout << "集合大小：" << s.size() << std::endl;

    // 6. 删除元素
    s.erase("banana");
    std::cout << "删除 banana 后大小：" << s.size() << std::endl;

    // 7. 查看桶信息
    std::cout << "桶数量：" << s.bucket_count() << std::endl;
    std::cout << "负载因子：" << s.load_factor() << std::endl;

    // 8. 使用 count
    std::cout << "apple 存在：" << s.count("apple") << std::endl;

    // 9. 使用 equal_range
    auto range = s.equal_range("cherry");
    std::cout << "cherry 范围起始有效：" << (range.first != s.end() ? "是" : "否") << std::endl;

    return 0;
}
```

### 4.2 输出结果

```
fig 插入状态：成功
找到 cherry：cherry
遍历元素：apple banana cherry date elderberry fig 
重复插入 apple 状态：已存在，忽略
集合大小：6
删除 banana 后大小：5
桶数量：16
负载因子：0.3125
apple 存在：1
cherry 范围起始有效：是
```

### 4.3 使用自定义哈希函数

```c++
#include "unordered_set.h"
#include <iostream>
#include <string>

struct MyHash {
    size_t operator()(const std::string& s) const {
        size_t hash = 0;
        for (char c : s) {
            hash = hash * 31 + static_cast<size_t>(c);
        }
        return hash;
    }
};

int main() {
    demo::unordered_set<std::string, MyHash> s{"one", "two", "three"};

    std::cout << "自定义哈希遍历：";
    for (const auto& key : s) {
        std::cout << key << " ";
    }
    std::cout << std::endl;

    return 0;
}
```

### 4.4 使用自定义键比较函数

```c++
#include "unordered_set.h"
#include <iostream>
#include <string>
#include <cctype>

struct CaseInsensitiveEq {
    bool operator()(const std::string& a, const std::string& b) const {
        if (a.size() != b.size()) return false;
        for (size_t i = 0; i < a.size(); ++i) {
            if (std::tolower(a[i]) != std::tolower(b[i])) return false;
        }
        return true;
    }
};

struct CaseInsensitiveHash {
    size_t operator()(const std::string& s) const {
        size_t hash = 0;
        for (char c : s) {
            hash = hash * 31 + static_cast<size_t>(std::tolower(c));
        }
        return hash;
    }
};

int main() {
    demo::unordered_set<std::string, CaseInsensitiveHash, CaseInsensitiveEq> s;

    s.insert("Apple");
    s.insert("APPLE");
    s.insert("apple");

    std::cout << "大小（去重后）：" << s.size() << std::endl;  // 输出 1
    std::cout << "包含 Apple：" << (s.find("apple") != s.end() ? "是" : "否") << std::endl;

    return 0;
}
```

## 5. 异常处理

|                  异常场景                  |      抛出类型       |                   预防措施                    |
| :----------------------------------------: | :-----------------: | :-------------------------------------------: |
|       构造 / 插入元素时内存分配失败        |  `std::bad_alloc`   |        捕获异常或提前检查 `max_size()`        |
| `erase()`/`insert()` 传入非法迭代器 |     未定义行为      | 确保迭代器在 `[begin(), end()]` 范围内 |
|  键类型不支持哈希函数（未定义 `Hash` 行为） |     未定义行为      |      确保键类型满足哈希要求（默认 `std::hash`）      |
|  `bucket()`/`bucket_size()` 传入非法桶索引 |     未定义行为      |      确保索引 < `bucket_count()`      |

## 6. 性能说明

### 6.1 时间复杂度

|                     操作                     | 时间复杂度 |            备注            |
| :------------------------------------------: | :--------: | :------------------------: |
|                   查找（`find`/`count`）                   |  O(1) 平均 |      哈希表直接定位      |
|          插入单个元素（`insert`/`emplace`）           |  O(1) 平均 |      可能触发重哈希      |
|             删除单个元素（`erase`）             |  O(1) 平均 |      定位桶后遍历链表      |
|        范围插入 / 删除（`insert`/`erase` 范围）        |  O(m) 平均 | m 为插入 / 删除的元素个数 |
|                 遍历（`begin` → `end`）                 |    O(n)    |      遍历所有桶和节点      |
|                   `rehash`                   |    O(n)    |      重新分配所有节点      |
|                   `reserve`                   |    O(n)    |      可能触发 rehash      |
|                   `clear`                   |    O(n)    |        销毁所有节点        |
|                   `swap`                   |    O(1)    |      仅交换指针和元数据      |

### 6.2 性能优化建议

1. **合理设置初始桶数量**：预估元素数量，通过 `reserve()` 或构造函数设置合适的初始桶数量，减少重哈希次数；
2. **调整负载因子**：对于读多写少的场景，可适当提高 `max_load_factor()` 以节省内存；对于写频繁的场景，可降低负载因子以保持性能；
3. **使用 `emplace` 替代 `insert`**：直接原地构造键，避免不必要的拷贝 / 移动开销；
4. **优化哈希函数**：对于自定义类型，提供高效的哈希函数，减少哈希冲突；
5. **批量插入前调用 `reserve`**：避免插入过程中多次重哈希；
6. **使用 `merge` 合并容器**：批量移动元素，避免逐个插入的重复查找开销。

## 7. 与 std::unordered_set 的差异

|    特性    |             demo::unordered_set              |      std::unordered_set       |
| :--------: | :---------------------------------: | :------------------: |
|  底层结构  |           哈希表（手写实现，链地址法）           |        哈希表（编译器实现）        |
| 桶数量策略 |    桶数量始终为 2 的幂，使用位与运算定位桶    |   桶数量策略由实现决定   |
| 重哈希触发 | 负载因子超过阈值时自动触发 | 完全一致 |
| 迭代器类型 |           前向迭代器，对齐标准            |           完全一致           |
|  异常安全  |  内存分配失败时抛 `std::bad_alloc`，其他情况行为与标准一致 |           完全一致           |
|  桶接口  |     支持 `local_iterator`/`const_local_iterator` 遍历单个桶     |              完全一致              |
|  哈希策略  |       支持 `rehash()`/`reserve()`/`load_factor()` 等接口        |           完全一致           |
| `merge` 操作 | 支持异类型 unordered_set 合并 | 完全一致 |

## 8. 已知限制

1. **不支持比较运算符（`<`/`>`/`<=`/`>=`）**：仅支持 `==`/`!=`，unordered_set 不保证有序，无法提供有序比较；
2. **不支持 `erase_if`（C++20 特性）**：需手动遍历删除；
3. **不支持 `extract`（C++17 特性）**：无法无拷贝提取节点；
4. **不支持 `contains`（C++20 特性）**：需使用 `find()` 或 `count()` 检查元素存在性；
5. **迭代器失效规则**：插入元素可能使迭代器失效（重哈希时），删除元素仅失效指向被删除元素的迭代器；
6. **桶数量始终为 2 的幂**：可能导致内存浪费（如需特定桶数量）；
7. **插入 hint 仅用于兼容性**：当前实现中 `insert(hint, value)` 和 `emplace_hint` 不提供实际的位置优化。

## 9. 测试建议

1. 基础功能测试：构造、插入、删除、查找、遍历等核心接口；
2. 边界测试：空 unordered_set 操作、单元素 unordered_set、大量元素插入 / 删除；
3. 异常测试：内存分配失败、非法迭代器操作、非法桶索引；
4. 性能测试：对比 `std::unordered_set` 的插入 / 删除 / 查找效率，测试重哈希性能；
5. 类型兼容性测试：测试内置类型、复杂类型（`std::string`）、自定义类的兼容性；
6. 自定义哈希函数测试：测试自定义哈希函数和键比较函数；
7. 去重测试：验证重复键插入会被正确忽略；
8. 重哈希测试：测试负载因子触发重哈希的正确性；
9. 移动语义测试：测试移动构造、移动赋值、swap 操作的正确性；
10. merge 测试：测试合并两个 unordered_set 的正确性。

---

**文档版本**：v1.0
**创建日期**：2026-05-27
**适用容器**：`demo::unordered_set<Key, Hash, KeyEqual, Allocator>`
