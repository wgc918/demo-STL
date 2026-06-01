# demo::unordered_multimap 容器开发文档

## 1. 文档概述

### 1.1 文档目的

本文档详细描述自定义实现的 `demo::unordered_multimap` 容器的设计思路、核心功能、接口规范、使用方法及注意事项，为开发者提供完整的开发、测试和维护参考，同时对齐 C++ 标准库 `std::unordered_multimap` 的接口设计，保证兼容性和易用性。

### 1.2 容器简介

`demo::unordered_multimap` 是一个遵循 C++17 标准的**关联容器**，存储键值对（key-value pairs），**键允许重复**，但不保证任何特定顺序。`unordered_multimap` 基于哈希表实现，采用链地址法（Separate Chaining）处理哈希冲突，保证平均 O(1) 的插入、删除和查找操作，适用于需要快速查找且允许键重复的场景。

与 `demo::unordered_map` 的核心区别在于：`unordered_multimap` 不强制键的唯一性，同一个键可以对应多个不同的值。这一设计差异导致了一系列接口和行为上的变化，详见本文档各章节。

### 1.3 适用范围

- 编译环境：兼容 GCC、Clang、MSVC 等主流编译器，需开启 C++17 及以上标准（`-std=c++17`）；
- 键类型：支持任意可哈希的类型（通过 `Hash` 函数对象自定义哈希规则，默认 `std::hash<Key>`）；
- 值类型：支持任意可拷贝 / 可移动 / 可析构的类型（内置类型、`std::string`、自定义类等）；
- 应用场景：适合需要快速查找、插入、删除操作，且允许键重复的场景（如多值索引、事件日志、消息队列、标签系统）。

## 2. 设计思路

### 2.1 核心架构

#### 2.1.1 哈希桶节点结构

`unordered_multimap` 基于哈希表实现，每个桶存储一个链表，用于处理哈希冲突：

```c++
struct Node
{
    value_type value;      // 键值对 std::pair<const Key, T>
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
- **键可重复**：不强制键的唯一性，同一键可对应多个值，插入操作总是成功；
- **同键元素连续存储**：插入时自动将相同键的元素保持为桶内连续的一组，确保 `equal_range()` 可以高效地返回正确的迭代器范围。

### 2.2 迭代器设计

`unordered_multimap` 支持**前向迭代器（std::forward_iterator_tag）**，按桶顺序遍历所有元素：

|    迭代器类型    |         说明         |                           核心特性                            |
| :--------------: | :------------------: | :-----------------------------------------------------------: |
|    `iterator`    |   可变前向迭代器    | 支持 `++`（前置 / 后置）、`*`、`->`、`==`/`!=`，可修改元素值（但键不可修改） |
| `const_iterator` | 不可变前向迭代器    |        与 `iterator` 功能一致，但仅能读取元素值，不可修改        |
| `local_iterator` |   桶内可变前向迭代器    |              仅在单个桶内遍历，不跨桶移动                  |
| `const_local_iterator` | 桶内不可变前向迭代器 |                    常量版本桶内迭代器                         |

#### 迭代器遍历顺序

`unordered_multimap` 的迭代器按桶顺序遍历所有元素：

1. 遍历桶数组（从桶 0 到桶 `bucket_count()-1`）；
2. 每个桶内按链表顺序遍历；
3. 相同键的元素在桶内**连续排列**（由插入策略保证），但不同键之间的顺序取决于插入顺序和哈希值；
4. 遍历顺序**不保证与插入顺序或键的大小顺序一致**；
5. 重哈希后迭代顺序可能改变。

#### 迭代器桶边界感知

`iterator` 和 `const_iterator` 内部包含 `m_local` 标志位，用于 `equal_range()` 返回的迭代器。当 `m_local` 为 `true` 时，`operator++` 仅沿桶内链表移动，不会跨越到其他桶，确保 `equal_range()` 返回的迭代器范围精确包含所有匹配元素，不会包含不匹配的元素。

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

#### 2.4.2 插入算法（同键元素连续存储）

与 `unordered_map` 不同，`unordered_multimap` 的插入算法不检查键唯一性，但需要确保相同键的元素在桶内连续存储，以支持 `equal_range()` 的正确工作：

1. 计算键的哈希值和桶索引；
2. 在目标桶中遍历，**查找第一个相同键的节点**；
3. 如果找到，将新节点插入到该节点**之前**（即插入到相同键元素组的开头）；
4. 如果未找到，将新节点插入到桶的链表头部；
5. 更新 `m_size`，检查是否需要重哈希。

**设计意图**：通过此插入策略，相同键的元素在桶内形成连续的一组，`equal_range()` 只需找到该组的起始位置和结束位置，即可返回精确的迭代器范围。

#### 2.4.3 重哈希算法（do_rehash）

当负载因子超过阈值时触发重哈希：

1. **计算新桶数量**：取大于等于目标桶数量的最小 2 的幂；
2. **分配新桶数组**：使用 `bucket_allocator_type` 分配新的桶数组；
3. **重新分配节点**：遍历所有旧桶的节点，计算新桶索引，将节点插入新桶（保持链表顺序）；
4. **释放旧桶数组**：销毁旧桶数组，更新 `m_table`、`m_mask`、`m_bucket_count`。

#### 2.4.4 键批量删除算法（erase_key_from_bucket）

`unordered_multimap` 特有的批量删除辅助方法，遍历桶内链表，删除所有键匹配的节点：

1. 从桶头开始遍历链表；
2. 对每个节点，检查键是否匹配；
3. 匹配时：从链表中摘除节点，销毁并释放内存，计数器加 1；
4. 不匹配时：继续遍历下一个节点；
5. 最后从 `m_size` 中减去删除数量，返回删除数量。

#### 2.4.5 负载因子管理

负载因子计算公式：`load_factor() = size() / bucket_count()`

**触发条件**：当 `load_factor() > max_load_factor()` 时自动调用 `rehash()`

**默认参数**：
- 默认桶数量：16（`UNORDERED_MULTIMAP_DEFAULT_BUCKET_COUNT`）；
- 默认最大负载因子：1.0（`UNORDERED_MULTIMAP_DEFAULT_LOAD_FACTOR`）。

## 3. 接口说明

### 3.1 类型别名（Type Aliases）

|         类型别名         |                        含义                         |
| :----------------------: | :-------------------------------------------------: |
|       `key_type`         |              键类型（`Key`）                          |
|    `mapped_type`         |              值类型（`T`）                          |
|     `value_type`         |       元素类型（`std::pair<const Key, T>`）          |
|        `pointer`         |                元素指针类型（`T*`）                 |
|     `const_pointer`      |           常量元素指针类型（`const T*`）            |
|       `reference`        |                元素引用类型（`T&`）                 |
|    `const_reference`     |           常量元素引用类型（`const T&`）            |
|     `allocator_type`     | 分配器类型（`Allocator`，默认 `std::allocator<pair<const Key, T>>`） |
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
|                `unordered_multimap()`                |     空构造，创建不含任何元素的空 unordered_multimap，默认桶数量 16       |                `demo::unordered_multimap<std::string, int> m;`                 |
|        `explicit unordered_multimap(size_type bucket_count)`         |   使用指定初始桶数量创建空 unordered_multimap   |            `demo::unordered_multimap<int, std::string> m(64);`            |
|        `unordered_multimap(InputIt first, InputIt last, size_type bucket_count = 16)`         |   迭代器范围构造   |            `std::vector<std::pair<int, std::string>> v; demo::unordered_multimap<int, std::string> m(v.begin(), v.end());`            |
|        `unordered_multimap(std::initializer_list<value_type> ilist, size_type bucket_count = 16)`         |   初始化列表构造   |            `demo::unordered_multimap<int, std::string> m{{1, "a"}, {1, "b"}, {2, "c"}};`            |
|        `unordered_multimap(const unordered_multimap& other)`         |            拷贝构造（深拷贝所有节点）             |            `demo::unordered_multimap<int, std::string> m2(m1);`            |
|          `unordered_multimap(unordered_multimap&& other) noexcept`          |            移动构造（仅交换指针）            |        `demo::unordered_multimap<int, std::string> m2(std::move(m1));`        |
|               `~unordered_multimap()`                |            析构函数，销毁所有节点并释放内存            |                              -                               |

### 3.3 赋值操作

|                           接口                            |                  功能说明                  |           示例           |
| :-------------------------------------------------------: | :----------------------------------------: | :----------------------: |
|   `unordered_multimap& operator=(const unordered_multimap& other)`    |            拷贝赋值（先清空自身，再深拷贝）            |       `m2 = m1;`       |
| `unordered_multimap& operator=(unordered_multimap&& other) noexcept`  |            移动赋值（交换指针和分配器）            | `m2 = std::move(m1);`  |

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
|     `bool empty() const noexcept`     |                判断 unordered_multimap 是否为空（无有效元素）                |                 `if (m.empty()) { ... }`                  |
|    `size_type size() const noexcept`    |                    返回 unordered_multimap 中元素的个数                    |                  `auto sz = m.size();`                  |
| `size_type max_size() const noexcept` |           返回 unordered_multimap 最大可存储元素数（由分配器决定）           |                 `auto ms = m.max_size();`                  |

### 3.7 元素修改操作

|                             接口                             |                           功能说明                           |               时间复杂度                |
| :----------------------------------------------------------: | :----------------------------------------------------------: | :-------------------------------------: |
|               `iterator insert(const value_type& value)`                |           插入键值对（拷贝版本），**始终成功**，返回指向新元素的迭代器           |         O(1) 平均，O(n) 最坏         |
|              `iterator insert(value_type&& value)`               |           插入键值对（移动版本），**始终成功**，返回指向新元素的迭代器           |         O(1) 平均，O(n) 最坏         |
|              `iterator insert(const_iterator hint, const value_type& value)`              |     在 `hint` 位置附近插入键值对（拷贝版本，hint 仅用于兼容性），**始终成功**     |         O(1) 平均，O(n) 最坏         |
|              `iterator insert(const_iterator hint, value_type&& value)`              |     在 `hint` 位置附近插入键值对（移动版本，hint 仅用于兼容性），**始终成功**     |         O(1) 平均，O(n) 最坏         |
| `void insert(InputIt first, InputIt last)` |                      插入迭代器范围的键值对                      |             O(m) 平均，m 为插入元素个数             |
| `void insert(std::initializer_list<value_type> ilist)` |                      插入初始化列表的键值对                      |             O(m) 平均，m 为插入元素个数             |
| `template <typename... Args> iterator emplace(Args&&... args)` | 原位构造元素并插入，**始终成功**，返回指向新元素的迭代器 | O(1) 平均，O(n) 最坏 |
| `template <typename... Args> iterator emplace_hint(const_iterator hint, Args&&... args)` | 原位构造元素并插入（hint 仅用于兼容性），**始终成功** | O(1) 平均，O(n) 最坏 |
|          `iterator erase(iterator pos)`          |                    删除 `pos` 位置的键值对                     |    O(1) 平均，O(n) 最坏    |
|          `iterator erase(const_iterator pos)`          |                    删除 `pos` 位置的键值对（常量版本）                     |    O(1) 平均，O(n) 最坏    |
| `iterator erase(const_iterator first, const_iterator last)` |               删除 `[first, last)` 范围键值对                |             O(m) 平均，m 为删除元素个数             |
|          `size_type erase(const Key& key)`          |            **删除所有匹配键的键值对**，返回被删除的元素数量（0 或多个）            |    O(1) 平均，O(n) 最坏    |
|            `void clear() noexcept`                    |                 清空所有键值对（销毁所有节点）                 |           O(n)             |
|          `void swap(unordered_multimap& other)`           |               交换两个 unordered_multimap 的内容（仅交换指针）               |                  O(1)                  |
| `template <typename H2, typename P2> void merge(unordered_multimap<Key, T, H2, P2, Allocator>& source)` | 合并另一个 unordered_multimap 的元素到当前容器（**无条件合并所有元素**，不检查键冲突） | O(m) 平均，m 为源容器元素个数 |
| `template <typename H2, typename P2> void merge(unordered_multimap<Key, T, H2, P2, Allocator>&& source)` | 合并另一个 unordered_multimap 的元素（右值版本） | O(m) 平均，m 为源容器元素个数 |

> **与 unordered_map 的重要区别**：
> - `insert()` 返回 `iterator` 而非 `std::pair<iterator, bool>`，因为插入总是成功；
> - `emplace()` 返回 `iterator` 而非 `std::pair<iterator, bool>`，同上；
> - `erase(key)` 删除**所有**匹配键的元素，返回 `size_type`（删除数量），而非仅删除一个；
> - `merge()` 无条件合并所有元素，不跳过键冲突的元素；
> - **不提供** `operator[]`、`at()`、`insert_or_assign()`、`try_emplace()`，因为键不唯一时这些操作的目标不明确。

### 3.8 查找操作

|                             接口                             |                           功能说明                           |               时间复杂度                |
| :----------------------------------------------------------: | :----------------------------------------------------------: | :-------------------------------------: |
|          `size_type count(const Key& key) const`          |  统计指定键的元素数量（**可返回大于 1 的值**，遍历桶内所有匹配元素）  |         O(1) 平均，O(n) 最坏         |
|            `iterator find(const Key& key)`            |           查找指定键的**任意一个**元素（桶内第一个匹配的节点）           |         O(1) 平均，O(n) 最坏         |
|     `const_iterator find(const Key& key) const`      |           查找指定键的任意一个元素（常量版本）           |         O(1) 平均，O(n) 最坏         |
| `std::pair<iterator, iterator> equal_range(const Key& key)` | 返回等于指定键的元素范围（**可包含多个元素**，使用桶内迭代器，不跨桶） | O(1) 平均，O(n) 最坏 |
| `std::pair<const_iterator, const_iterator> equal_range(const Key& key) const` | 返回等于指定键的元素范围（常量版本） | O(1) 平均，O(n) 最坏 |

> **与 unordered_map 的重要区别**：
> - `count()` 可返回大于 1 的值，需要遍历整个桶统计匹配元素；
> - `find()` 返回任意一个匹配元素（桶内第一个），不保证返回特定元素；
> - `equal_range()` 返回的迭代器范围可包含多个元素，且迭代器设置了桶内遍历模式（`m_local = true`），不会跨越到其他桶。

### 3.9 桶接口

|                 接口                  |                           功能说明                           |                            示例                            |
| :-----------------------------------: | :----------------------------------------------------------: | :--------------------------------------------------------: |
|    `size_type bucket_count() const`    |                    返回当前桶数量                    |                  `auto bc = m.bucket_count();`                  |
| `size_type max_bucket_count() const` |           返回最大桶数量（受限于 size_type）           |                 `auto mbc = m.max_bucket_count();`                  |
|    `size_type bucket_size(size_type n) const`    |                    返回桶 n 中的元素数量                    |                  `auto bs = m.bucket_size(0);`                  |
|    `size_type bucket(const Key& key) const`    |                    返回键 key 所在的桶索引                    |                  `auto idx = m.bucket(1);`                  |

### 3.10 哈希策略接口

|                 接口                  |                           功能说明                           |                            示例                            |
| :-----------------------------------: | :----------------------------------------------------------: | :--------------------------------------------------------: |
|    `float load_factor() const`    |                    返回当前负载因子                    |                  `auto lf = m.load_factor();`                  |
|    `float max_load_factor() const`    |                    返回最大负载因子                    |                  `auto mlf = m.max_load_factor();`                  |
|    `void max_load_factor(float new_max_load_factor)`    |                    设置最大负载因子                    |                  `m.max_load_factor(0.75f);`                  |
|    `void rehash(size_type new_bucket_count)`    |                    重新哈希，改变桶数量                    |                  `m.rehash(1024);`                  |
|    `void reserve(size_type count)`    |                    预留空间以容纳至少 count 个元素                    |                  `m.reserve(1000);`                  |

### 3.11 观察器接口

|               接口                |         功能说明         |                示例                |
| :-------------------------------: | :----------------------: | :--------------------------------: |
| `hash_type hash_function() const` | 返回哈希函数对象的副本 | `auto hash_fn = m.hash_function();` |
| `key_equal key_eq() const` | 返回键比较函数对象的副本 | `auto eq_fn = m.key_eq();` |

### 3.12 分配器接口

|               接口                |         功能说明         |                示例                |
| :-------------------------------: | :----------------------: | :--------------------------------: |
| `allocator_type get_allocator() const` | 返回容器使用的分配器副本 | `auto alloc = m.get_allocator();` |

### 3.13 比较运算符

|               运算符                |         功能说明         |                示例                |
| :--------------------------------: | :----------------------: | :--------------------------------: |
| `bool operator==(const unordered_multimap& lhs, const unordered_multimap& rhs)` | 比较两个 unordered_multimap 是否相等（元素数量相同且每个键值对都能在对方中找到匹配） | `if (m1 == m2) { ... }` |
| `bool operator!=(const unordered_multimap& lhs, const unordered_multimap& rhs)` | 比较两个 unordered_multimap 是否不相等 | `if (m1 != m2) { ... }` |

> **与 unordered_map 的重要区别**：`operator==` 使用 `equal_range()` 逐元素匹配，因为同一个键可能对应多个值。两个 multimap 相等当且仅当它们具有相同数量的元素，且对于每个键值对 `(k, v)` 在 lhs 中，都存在一个键值对 `(k, v')` 在 rhs 中使得 `k == k'` 且 `v == v'`。

## 4. 使用示例

### 4.1 基础使用

```c++
#include "unordered_multimap.h"
#include <iostream>
#include <string>

int main() {
    // 1. 构造 unordered_multimap（允许重复键）
    demo::unordered_multimap<std::string, int> m{
        {"apple",  1},
        {"banana", 2},
        {"apple",  10},  // 重复键 "apple"，允许
        {"cherry", 3}
    };

    // 2. 插入元素（始终成功）
    m.insert({"banana", 20});  // 重复键 "banana"
    m.emplace("date", 4);

    std::cout << "插入后大小：" << m.size() << std::endl;

    // 3. 统计指定键的元素数量
    std::cout << "apple 的数量：" << m.count("apple") << std::endl;   // 输出: 2
    std::cout << "banana 的数量：" << m.count("banana") << std::endl; // 输出: 2

    // 4. 查找任意一个匹配元素
    auto it = m.find("apple");
    if (it != m.end()) {
        std::cout << "找到一个 apple：" << it->first << " -> " << it->second << std::endl;
    }

    // 5. 使用 equal_range 遍历所有匹配元素
    auto range = m.equal_range("apple");
    std::cout << "所有 apple 的值：";
    for (auto iter = range.first; iter != range.second; ++iter) {
        std::cout << iter->second << " ";
    }
    std::cout << std::endl;

    // 6. 遍历所有元素（无序）
    std::cout << "遍历所有元素：";
    for (const auto& [key, value] : m) {
        std::cout << key << ":" << value << " ";
    }
    std::cout << std::endl;

    // 7. 删除指定键的所有元素
    size_t erased = m.erase("banana");
    std::cout << "删除了 " << erased << " 个 banana 元素" << std::endl;
    std::cout << "删除后大小：" << m.size() << std::endl;

    // 8. 查看桶信息
    std::cout << "桶数量：" << m.bucket_count() << std::endl;
    std::cout << "负载因子：" << m.load_factor() << std::endl;

    return 0;
}
```

### 4.2 输出结果

```
插入后大小：6
apple 的数量：2
banana 的数量：2
找到一个 apple：apple -> 10
所有 apple 的值：10 1
遍历所有元素：cherry:3 date:4 apple:10 apple:1 banana:20 banana:2
删除了 2 个 banana 元素
删除后大小：4
桶数量：16
负载因子：0.25
```

### 4.3 使用自定义哈希函数

```c++
#include "unordered_multimap.h"
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
    demo::unordered_multimap<std::string, int, MyHash> m{
        {"one",   1},
        {"two",   2},
        {"one",   10},  // 重复键 "one"
        {"three", 3}
    };

    std::cout << "自定义哈希，one 的数量：" << m.count("one") << std::endl;

    auto range = m.equal_range("one");
    std::cout << "所有 one 的值：";
    for (auto it = range.first; it != range.second; ++it) {
        std::cout << it->second << " ";
    }
    std::cout << std::endl;

    return 0;
}
```

### 4.4 合并两个 multimap

```c++
#include "unordered_multimap.h"
#include <iostream>

int main() {
    demo::unordered_multimap<int, int> m1{{1, 10}, {1, 20}, {2, 30}};
    demo::unordered_multimap<int, int> m2{{1, 40}, {2, 50}, {3, 60}};

    std::cout << "合并前 m1 大小：" << m1.size() << std::endl;
    std::cout << "合并前 m2 大小：" << m2.size() << std::endl;

    m1.merge(m2);  // 无条件合并所有元素

    std::cout << "合并后 m1 大小：" << m1.size() << std::endl;  // 6
    std::cout << "合并后 m2 大小：" << m2.size() << std::endl;  // 0
    std::cout << "m1 中 key=1 的数量：" << m1.count(1) << std::endl;  // 3

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

> **与 unordered_map 的重要区别**：`unordered_multimap` **不提供** `at()` 方法，因此不存在 `at()` 访问不存在的键时抛出 `std::out_of_range` 异常的场景。

## 6. 性能说明

### 6.1 时间复杂度

|                     操作                     | 时间复杂度 |            备注            |
| :------------------------------------------: | :--------: | :------------------------: |
|                   查找（`find`）                   |  O(1) 平均 |      哈希表直接定位，返回第一个匹配元素      |
|                   统计（`count`）                   |  O(1) 平均，O(n) 最坏 |      遍历桶内所有匹配元素      |
|            `equal_range`                   |  O(1) 平均，O(n) 最坏 |      遍历桶内匹配组      |
|          插入单个元素（`insert`/`emplace`）           |  O(1) 平均 |      可能触发重哈希，需遍历桶定位同键组      |
|             删除单个元素（`erase` 按位置）             |  O(1) 平均 |      定位桶后遍历链表      |
|             删除所有匹配键元素（`erase` 按键）             |  O(1) 平均，O(n) 最坏 |      遍历桶内所有节点      |
|        范围插入 / 删除（`insert`/`erase` 范围）        |  O(m) 平均 | m 为插入 / 删除的元素个数 |
|                 遍历（`begin` → `end`）                 |    O(n)    |      遍历所有桶和节点      |
|                   `rehash`                   |    O(n)    |      重新分配所有节点      |
|                   `reserve`                   |    O(n)    |      可能触发 rehash      |
|                   `clear`                   |    O(n)    |        销毁所有节点        |
|                   `swap`                   |    O(1)    |      仅交换指针和元数据      |
|                   `merge`                   |    O(m)    |      m 为源容器元素个数，逐元素插入并删除      |

### 6.2 性能优化建议

1. **合理设置初始桶数量**：预估元素数量，通过 `reserve()` 或构造函数设置合适的初始桶数量，减少重哈希次数；
2. **调整负载因子**：对于读多写少的场景，可适当提高 `max_load_factor()` 以节省内存；对于写频繁的场景，可降低负载因子以保持性能；
3. **使用 `emplace` 替代 `insert`**：直接原地构造键值对，避免不必要的拷贝 / 移动开销；
4. **使用 `equal_range` 获取所有同键元素**：当需要遍历某个键的所有值时，使用 `equal_range()` 比多次调用 `find()` 更高效；
5. **使用 `count` 判断键的存在性**：当仅需判断键是否存在时，`count()` 比 `find()` 语义更清晰；
6. **优化哈希函数**：对于自定义类型，提供高效的哈希函数，减少哈希冲突；
7. **批量插入前调用 `reserve`**：避免插入过程中多次重哈希；
8. **使用 `erase(key)` 批量删除同键元素**：一次性删除所有匹配键的元素，比逐个 `find()` 再 `erase()` 更高效。

## 7. 与 std::unordered_multimap 的差异

|    特性    |             demo::unordered_multimap              |      std::unordered_multimap       |
| :--------: | :---------------------------------: | :------------------: |
|  底层结构  |           哈希表（手写实现，链地址法）           |        哈希表（编译器实现）        |
| 桶数量策略 |    桶数量始终为 2 的幂，使用位与运算定位桶    |   桶数量策略由实现决定   |
| 重哈希触发 | 负载因子超过阈值时自动触发 | 完全一致 |
| 迭代器类型 |           前向迭代器，对齐标准            |           完全一致           |
| 同键元素存储 | 插入时保持同键元素在桶内连续，支持 `equal_range` 正确性 | 实现定义 |
| 插入返回值 | `iterator`（始终成功） | 完全一致 |
| `erase(key)` 返回值 | `size_type`（删除数量） | 完全一致 |
| 不支持的方法 | 无 `operator[]`、`at()`、`insert_or_assign`、`try_emplace` | 完全一致 |
|  异常安全  |  内存分配失败时抛 `std::bad_alloc`，其他情况行为与标准一致 |           完全一致           |
|  桶接口  |     支持 `local_iterator`/`const_local_iterator` 遍历单个桶     |              完全一致              |
|  哈希策略  |       支持 `rehash()`/`reserve()`/`load_factor()` 等接口        |           完全一致           |

## 8. 与 demo::unordered_map 的核心差异

本节专门列出从 `demo::unordered_map` 到 `demo::unordered_multimap` 的关键行为差异，供已熟悉 `unordered_map` 的开发者参考。

|    特性    |             demo::unordered_map              |      demo::unordered_multimap       |
| :--------: | :---------------------------------: | :------------------: |
|  键唯一性  | 键必须唯一，重复插入会被忽略 | 键可重复，所有插入都成功 |
| `insert()` 返回值 | `std::pair<iterator, bool>` | `iterator` |
| `emplace()` 返回值 | `std::pair<iterator, bool>` | `iterator` |
| `erase(key)` 行为 | 删除至多一个元素，返回 0 或 1 | 删除所有匹配元素，返回 0 或多个 |
| `count()` 返回值 | 0 或 1 | 0 或多个 |
| `equal_range()` 范围 | 最多包含一个元素 | 可包含多个元素，使用桶内迭代器 |
| `find()` 行为 | 返回唯一匹配元素 | 返回任意一个匹配元素 |
| `merge()` 行为 | 跳过键冲突的元素 | 无条件合并所有元素 |
| `operator==` 算法 | 直接比较每个键对应的值 | 使用 `equal_range` 逐元素匹配 |
| `operator[]` | 支持 | **不支持**（键不唯一，参数不明确） |
| `at()` | 支持 | **不支持**（键不唯一，参数不明确） |
| `insert_or_assign()` | 支持 | **不支持**（键不唯一，参数不明确） |
| `try_emplace()` | 支持 | **不支持**（键不唯一，语义不明确） |
| 插入位置策略 | 插入到桶头部 | 插入到同键元素组之前，保持同键元素连续 |

## 9. 已知限制

1. **不支持比较运算符（`<`/`>`/`<=`/`>=`）**：仅支持 `==`/`!=`；
2. **不支持 `erase_if`（C++20 特性）**：需手动遍历删除；
3. **不支持 `extract`（C++17 特性）**：无法无拷贝提取节点；
4. **迭代器失效规则**：插入元素可能使迭代器失效（重哈希时），删除元素仅失效指向被删除元素的迭代器；
5. **桶数量始终为 2 的幂**：可能导致内存浪费（如需特定桶数量）；
6. **`equal_range` 返回的迭代器不可跨桶遍历**：`equal_range` 返回的迭代器设置了桶内遍历模式，`operator++` 不会跨越桶边界，这是确保返回范围精确性的设计选择；
7. **`find` 不保证返回特定元素**：当存在多个相同键的元素时，`find()` 返回桶内第一个匹配的元素，不保证返回最新或最旧的元素；
8. **桶内同键元素的顺序**：新插入的元素位于同键元素组的开头，因此同键元素的遍历顺序是 LIFO（后进先出）。

## 10. 测试建议

1. 基础功能测试：构造、插入、删除、查找、遍历、`equal_range` 等核心接口；
2. 重复键测试：插入多个相同键的元素，验证 `count()`、`equal_range()`、`erase(key)` 的行为正确性；
3. 边界测试：空 multimap 操作、单元素 multimap、大量元素插入 / 删除、大量重复键；
4. 异常测试：内存分配失败、非法迭代器操作、非法桶索引；
5. 性能测试：对比 `std::unordered_multimap` 的插入 / 删除 / 查找效率，测试重哈希性能；
6. 类型兼容性测试：测试内置类型、复杂类型（`std::string`）、自定义类的兼容性；
7. 自定义哈希函数测试：测试自定义哈希函数和键比较函数；
8. 重哈希测试：测试负载因子触发重哈希的正确性，验证重哈希后同键元素仍保持连续；
9. 移动语义测试：测试移动构造、移动赋值、swap 操作的正确性；
10. equal_range 跨桶测试：验证当不同键分布在不同桶时，`equal_range` 不会跨越桶边界；
11. merge 测试：验证合并操作无条件合并所有元素，不跳过键冲突。

---

**文档版本**：v1.0
**创建日期**：2026-06-01
**适用容器**：`demo::unordered_multimap<Key, T, Hash, KeyEqual, Allocator>`