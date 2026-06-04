# demo::multiset 容器开发文档

## 1. 文档概述

### 1.1 文档目的

本文档详细描述自定义实现的 `demo::multiset` 容器的设计思路、核心功能、接口规范、使用方法及注意事项，为开发者提供完整的开发、测试和维护参考，同时对齐 C++ 标准库 `std::multiset` 的接口设计，保证兼容性和易用性。

### 1.2 容器简介

`demo::multiset` 是一个遵循 C++17 标准的**关联容器**，存储键（key），允许键重复，按键的顺序进行自动排序。`multiset` 基于红黑树（Red-Black Tree）实现，保证 O(log n) 的插入、删除和查找操作，适用于需要可重复键存储、快速查找、有序遍历和元素计数统计的场景。

### 1.3 适用范围

- 编译环境：兼容 GCC、Clang、MSVC 等主流编译器，需开启 C++17 及以上标准（`-std=c++17`）；
- 键类型：支持任意可比较的类型（通过 `Compare` 函数对象自定义比较规则，默认 `std::less<Key>`）；
- 值类型：multiset 只存储键，元素类型与键类型相同；
- 应用场景：适合需要存储可重复值、统计元素出现次数、有序遍历的场景（如多重集合运算、词频统计、事件计数）。

## 2. 设计思路

### 2.1 核心架构

#### 2.1.1 红黑树节点结构

`multiset` 基于红黑树实现，每个节点存储键、前驱/后继指针、父指针及颜色标记：

```c++
struct Node
{
    value_type key;     // 键值（与 value_type 相同）
    Node*      left;    // 左子节点指针
    Node*      right;   // 右子节点指针
    Node*      parent;  // 父节点指针
    Color      color;   // 节点颜色（RED/BLACK）
};
```

#### 2.1.2 哨兵节点设计

`multiset` 使用哨兵节点（`m_nil`）表示空引用，简化边界处理：

- 所有空指针指向 `m_nil`（而非 `nullptr`）；
- 哨兵节点颜色固定为黑色；
- 迭代器的 `end()` 返回指向哨兵节点的迭代器。

#### 2.1.3 容器核心成员

|     成员变量     |         类型          |                              作用                               |
| :--------------: | :-------------------: | :-------------------------------------------------------------: |
|     `m_root`     |       `Node*`         |                      红黑树的根节点指针                         |
|     `m_nil`      |       `Node*`         |              哨兵节点，表示空引用（类似 `nullptr`）              |
|     `m_size`     |     `size_type`       |                      容器中元素的个数（含重复键）                |
|     `m_comp`     |       `Compare`       |              键比较函数对象，默认 `std::less<Key>`               |
| `m_node_alloc`   |  `node_alloc_type`   | 节点分配器，封装 `std::allocator`，负责节点的内存申请 / 释放、元素构造 / 析构 |

#### 2.1.4 架构优势

- **有序存储**：键自动排序，支持有序遍历（按中序遍历顺序）；
- **高效查找**：红黑树保证 O(log n) 时间复杂度的查找、插入、删除操作；
- **自平衡**：通过节点颜色和旋转操作维护树平衡，避免最坏情况退化；
- **键可重复**：允许同一个键出现多次，插入总是成功，不会因键重复而拒绝；
- **精确计数**：`count(key)` 返回指定键的实际出现次数，而非仅 0 或 1；
- **分配器适配**：通过 `rebind_alloc` 适配节点类型的内存分配，遵循 STL 分配器规范。

### 2.2 迭代器设计

`multiset` 支持**双向迭代器（std::bidirectional_iterator_tag）**，按中序遍历顺序访问元素：

|    迭代器类型    |         说明         |                           核心特性                            |
| :--------------: | :------------------: | :-----------------------------------------------------------: |
|    `iterator`    |   可变双向迭代器    | 支持 `++`/`--`（前置 / 后置）、`*`、`->`、`==`/`!=`，可读取元素 |
| `const_iterator` | 不可变双向迭代器    |        与 `iterator` 功能一致，但仅能读取元素值，不可修改        |
| `reverse_iterator` |   可变反向迭代器    |              基于标准库适配器实现，支持反向遍历                  |
| `const_reverse_iterator` | 不可变反向迭代器 |                    常量版本反向迭代器                         |

#### 迭代器遍历顺序

`multiset` 的迭代器按中序遍历（In-order Traversal）顺序访问元素：

1. 最小键元素 → 中间元素 → 最大键元素；
2. 遍历顺序与键的比较顺序一致（默认升序）；
3. 重复键按插入顺序依次排列（等价键在右子树中）。

### 2.3 内存管理策略

1. **节点分配**：每个节点独立分配内存（通过 `alloc_traits::allocate`），构造元素时使用完美转发（`std::forward`），避免不必要的拷贝；
2. **内存释放**：析构时递归销毁所有节点，释放哨兵节点；
3. **异常安全**：插入 / 构造元素时捕获异常，保证内存不泄漏；
4. **移动语义**：移动构造 / 赋值时仅交换指针和分配器，不拷贝元素，时间复杂度 O(1)；
5. **拷贝语义**：拷贝构造 / 赋值时递归拷贝所有节点，保证独立的内存空间。

### 2.4 红黑树核心算法

#### 2.4.1 插入平衡算法（insert_balance）

新节点默认插入为红色，通过以下规则维护红黑树性质：

1. 若父节点为黑色，插入完成；
2. 若父节点为红色（违反红黑树性质），分情况处理：
   - **Case 1**：叔节点也为红色 → 父、叔变黑，祖父变红，递归向上处理；
   - **Case 2**：叔节点为黑色，且新节点为内孙子 → 先旋转变为 Case 3；
   - **Case 3**：叔节点为黑色，且新节点为外孙子 → 旋转祖父节点，交换父、祖颜色。

#### 2.4.2 插入路由策略（multiset 特有）

与 `set` 不同，`multiset` 在插入时允许重复键。当新键与当前节点键等价（即 `!comp(key, cur->key) && !comp(cur->key, key)`）时，`multiset` 将新节点插入到右子树，而非拒绝插入。这保证了等价键按插入顺序存储在树中，且 `equal_range` 能正确返回所有等价键的范围。

#### 2.4.3 删除平衡算法（erase_balance）

删除节点后，若删除的是黑色节点（违反黑高度性质），通过以下规则恢复平衡：

1. 若当前节点为红色 → 直接染黑，终止；
2. 若当前节点是根节点 → 直接终止；
3. 根据兄弟节点颜色和子节点颜色，分多种情况处理，通过旋转和染色恢复黑高度。

#### 2.4.4 有序构建算法（build_tree）

支持从已排序的 vector 快速构建平衡二叉搜索树：

- 采用二分法选取中间元素作为根节点；
- 递归构建左右子树，保证树高平衡；
- 根节点为黑色，深度奇偶性决定节点颜色。

## 3. 接口说明

### 3.1 类型别名（Type Aliases）

|         类型别名         |                        含义                         |
| :----------------------: | :-------------------------------------------------: |
|       `key_type`         |              键类型（`Key`）                          |
|     `value_type`         |              元素类型（与键类型相同，`Key`）          |
|    `key_compare`         |              键比较函数类型（`Compare`）           |
|        `pointer`         |                元素指针类型（`Key*`）                 |
|     `const_pointer`      |           常量元素指针类型（`const Key*`）            |
|       `reference`        |                元素引用类型（`Key&`）                 |
|    `const_reference`     |           常量元素引用类型（`const Key&`）            |
|     `allocator_type`     | 分配器类型（`Allocator`，默认 `std::allocator<Key>`） |
|       `size_type`        |           无符号整数类型（`std::size_t`）           |
|    `difference_type`     |         有符号整数类型（`std::ptrdiff_t`）          |
|        `iterator`        |                   可变双向迭代器                    |
|     `const_iterator`     |                  不可变双向迭代器                   |
|    `reverse_iterator`    |                   可变反向迭代器                    |
| `const_reverse_iterator` |                  不可变反向迭代器                   |

### 3.2 构造与析构

|                      接口                      |                  功能说明                  |                             示例                             |
| :--------------------------------------------: | :----------------------------------------: | :----------------------------------------------------------: |
|                `multiset()`                |     空构造，创建不含任何元素的空 multiset       |                `demo::multiset<int> ms;`                 |
|        `explicit multiset(const Compare& comp)`         |   使用指定比较函数创建空 multiset   |            `demo::multiset<int, std::greater<int>> ms;`            |
|        `multiset(InputIt first, InputIt last, const Compare& comp = Compare())`         |   迭代器范围构造   |            `std::vector<int> v{1, 2, 1, 3}; demo::multiset<int> ms(v.begin(), v.end());`            |
|        `multiset(std::initializer_list<value_type> ilist, const Compare& comp = Compare())`         |   初始化列表构造   |            `demo::multiset<int> ms{1, 2, 1, 3};`            |
| `explicit multiset(std::vector<value_type>& vec, sorted_tag)` | 从已排序的 vector 构造 multiset（非标准，优化版本） | `std::vector<int> v{1, 1, 2, 3}; demo::multiset<int> ms(v, demo::multiset<int>::sorted_tag{});` |
| `explicit multiset(std::vector<value_type>& vec, unsorted_tag)` | 从未排序的 vector 构造 multiset（非标准） | `std::vector<int> v{3, 1, 2, 1}; demo::multiset<int> ms(v, demo::multiset<int>::unsorted_tag{});` |
|        `multiset(const multiset& other)`         |            拷贝构造（深拷贝所有节点）             |            `demo::multiset<int> ms2(ms1);`            |
|          `multiset(multiset&& other) noexcept`          |            移动构造（仅交换指针）            |        `demo::multiset<int> ms2(std::move(ms1));`        |
|               `~multiset()`                |            析构函数，销毁所有节点并释放内存            |                              -                               |

### 3.3 赋值操作

|                           接口                            |                  功能说明                  |           示例           |
| :-------------------------------------------------------: | :----------------------------------------: | :----------------------: |
|   `multiset& operator=(const multiset& other)`    |            拷贝赋值（先清空自身，再深拷贝）            |       `ms2 = ms1;`       |
| `multiset& operator=(multiset&& other) noexcept`  |            移动赋值（交换指针和分配器）            | `ms2 = std::move(ms1);`  |
| `multiset& operator=(std::initializer_list<value_type> ilist)` |            初始化列表赋值            |     `ms = {1, 2, 1, 3};`      |

### 3.4 迭代器接口

|                      接口                       |            功能说明            |        注意事项        |
| :---------------------------------------------: | :----------------------------: | :--------------------: |
|           `iterator begin() noexcept`           | 返回指向最小键元素的可变迭代器 |   空容器返回 `end()`   |
|     `const_iterator begin() const noexcept`     |       常量版本 `begin()`       |           -            |
|    `const_iterator cbegin() const noexcept`     |     显式常量版本 `begin()`     |           -            |
|            `iterator end() noexcept`            |      返回尾后迭代器（哨兵节点）      |       不可解引用       |
|      `const_iterator end() const noexcept`       |        常量版本 `end()`        |           -            |
|     `const_iterator cend() const noexcept`       |      显式常量版本 `end()`      |           -            |
|          `reverse_iterator rbegin()`             |   返回指向最大键元素的反向迭代器   |           -            |
|    `const_reverse_iterator rbegin() const`      |      常量版本 `rbegin()`       |           -            |
|    `const_reverse_iterator crbegin() const`    |    显式常量版本 `rbegin()`     |           -            |
|           `reverse_iterator rend()`              |   返回指向最小键元素之前位置的反向迭代器   |           -            |
|     `const_reverse_iterator rend() const`       |        常量版本 `rend()`       |           -            |
|    `const_reverse_iterator crend() const`        |      显式常量版本 `rend()`     |           -            |

### 3.5 容量操作

|                 接口                  |                           功能说明                           |                            示例                            |
| :-----------------------------------: | :----------------------------------------------------------: | :--------------------------------------------------------: |
|     `bool empty() const noexcept`     |                判断 multiset 是否为空（无有效元素）                |                 `if (ms.empty()) { ... }`                  |
|    `size_type size() const noexcept`    |                    返回 multiset 中元素的个数（含重复键）                    |                  `auto sz = ms.size();`                  |
| `size_type max_size() const noexcept` |           返回 multiset 最大可存储元素数（由分配器决定）           |                 `auto ms = ms.max_size();`                  |

### 3.6 元素修改操作

|                             接口                             |                           功能说明                           |               时间复杂度                |
| :----------------------------------------------------------: | :----------------------------------------------------------: | :-------------------------------------: |
|               `iterator insert(const value_type& value)`                |           插入键（拷贝版本），允许重复键           |         O(log n)，返回指向插入元素的迭代器         |
|              `iterator insert(value_type&& value)`               |           插入键（移动版本），允许重复键           |         O(log n)，返回指向插入元素的迭代器         |
|              `iterator insert(const_iterator pos, const value_type& value)`              |     在 `pos` 位置附近插入键（拷贝版本，hint 优化）     |         O(log n)（可能优化为 O(1)）         |
|              `iterator insert(const_iterator pos, value_type&& value)`              |     在 `pos` 位置附近插入键（移动版本，hint 优化）     |         O(log n)（可能优化为 O(1)）         |
| `void insert(InputIt first, InputIt last)` |                      插入迭代器范围的键                      |             O((last - first) log n)             |
| `void insert(std::initializer_list<value_type> ilist)` |                      插入初始化列表的键                      |             O(ilist.size() log n)             |
| `template <typename... Args> iterator emplace(Args&&... args)` | 原地构造键并插入，允许重复键 | O(log n)，返回指向插入元素的迭代器 |
| `template <typename... Args> iterator emplace_hint(const_iterator hint, Args&&... args)` | 在指定位置附近原地构造键并插入 | O(log n)（可能优化为 O(1)） |
|          `iterator erase(const_iterator pos)`          |                    删除 `pos` 位置的键                     |    O(log n)，返回删除元素的下一个迭代器    |
|          `size_type erase(const key_type& key)`          |                    删除指定键的所有元素                     |    O(k log n)，k 为匹配键的元素数量，返回删除元素数量    |
| `iterator erase(const_iterator first, const_iterator last)` |               删除 `[first, last)` 范围键                |             O((last - first) log n)             |
|            `void clear() noexcept`                    |                 清空所有键（销毁所有节点）                 |           O(n)             |
|          `void swap(multiset& other) noexcept`           |               交换两个 multiset 的内容（仅交换指针）               |                  O(1)                  |
| `template <typename Compare2> void merge(multiset<Key, Compare2, Allocator>& other)` | 合并另一个 multiset 的元素到当前 multiset | O(n log n) |

### 3.7 查找操作

|                             接口                             |                           功能说明                           |               时间复杂度                |
| :----------------------------------------------------------: | :----------------------------------------------------------: | :-------------------------------------: |
|          `size_type count(const key_type& key) const`          |  统计指定键的元素数量（multiset 中键可重复，可返回大于 1 的值）  |         O(log n)         |
|            `iterator find(const key_type& key)`            |           查找指定键的第一个元素           |         O(log n)         |
|     `const_iterator find(const key_type& key) const`      |           查找指定键的第一个元素（常量版本）           |         O(log n)         |
| `std::pair<iterator, iterator> equal_range(const key_type& key)` | 返回等于指定键的所有元素范围（multiset 中范围可能包含多个元素） | O(log n) |
| `std::pair<const_iterator, const_iterator> equal_range(const key_type& key) const` | 返回等于指定键的所有元素范围（常量版本） | O(log n) |
| `iterator lower_bound(const key_type& key)` | 返回第一个不小于指定键的元素的迭代器 | O(log n) |
| `const_iterator lower_bound(const key_type& key) const` | 返回第一个不小于指定键的元素的迭代器（常量版本） | O(log n) |
| `iterator upper_bound(const key_type& key)` | 返回第一个大于指定键的元素的迭代器 | O(log n) |
| `const_iterator upper_bound(const key_type& key) const` | 返回第一个大于指定键的元素的迭代器（常量版本） | O(log n) |

### 3.8 比较函数接口

|               接口                |         功能说明         |                示例                |
| :-------------------------------: | :----------------------: | :--------------------------------: |
| `key_compare key_comp() const` | 返回键比较函数对象的副本 | `auto comp = ms.key_comp();` |

### 3.9 分配器接口

|               接口                |         功能说明         |                示例                |
| :-------------------------------: | :----------------------: | :--------------------------------: |
| `allocator_type get_allocator() const` | 返回容器使用的分配器副本 | `auto alloc = ms.get_allocator();` |

### 3.10 比较运算符

|               运算符                |         功能说明         |                示例                |
| :--------------------------------: | :----------------------: | :--------------------------------: |
| `bool operator==(const multiset& rhs)` | 比较两个 multiset 是否相等（包含重复键的精确比较） | `if (ms1 == ms2) { ... }` |
| `bool operator!=(const multiset& rhs)` | 比较两个 multiset 是否不相等 | `if (ms1 != ms2) { ... }` |

## 4. 使用示例

### 4.1 基础使用

```c++
#include "multiset.h"
#include <iostream>
#include <string>

int main() {
    // 1. 构造 multiset
    demo::multiset<std::string> ms{"apple", "banana", "apple", "cherry"};

    // 2. 插入元素（允许重复键）
    ms.insert("date");
    ms.insert("apple");  // 重复键，插入成功
    ms.emplace("banana"); // 重复键，插入成功

    // 3. 统计元素出现次数
    std::cout << "apple 出现次数: " << ms.count("apple") << std::endl;   // 3
    std::cout << "banana 出现次数: " << ms.count("banana") << std::endl; // 2
    std::cout << "grape 出现次数: " << ms.count("grape") << std::endl;   // 0

    // 4. 查找元素
    auto find_it = ms.find("cherry");
    if (find_it != ms.end()) {
        std::cout << "找到元素: " << *find_it << std::endl;
    }

    // 5. 正向遍历（按键升序，重复键依次排列）
    std::cout << "正向遍历：";
    for (const auto& key : ms) {
        std::cout << key << " ";
    }
    std::cout << std::endl;

    // 6. 反向遍历（按键降序）
    std::cout << "反向遍历：";
    for (auto it = ms.rbegin(); it != ms.rend(); ++it) {
        std::cout << *it << " ";
    }
    std::cout << std::endl;

    // 7. 使用 equal_range 遍历所有重复键
    std::cout << "所有 apple 元素：";
    auto range = ms.equal_range("apple");
    for (auto it = range.first; it != range.second; ++it) {
        std::cout << *it << " ";
    }
    std::cout << std::endl;

    // 8. 使用 lower_bound 和 upper_bound
    auto lower = ms.lower_bound("apple");
    auto upper = ms.upper_bound("apple");
    std::cout << "apple 范围元素数量: " << std::distance(lower, upper) << std::endl;

    // 9. 删除指定键的所有元素
    auto erased_count = ms.erase("apple");
    std::cout << "删除 apple 的数量: " << erased_count << std::endl;
    std::cout << "删除后 multiset 大小: " << ms.size() << std::endl;

    // 10. 清空 multiset
    ms.clear();
    std::cout << "clear 后是否为空: " << std::boolalpha << ms.empty() << std::endl;

    return 0;
}
```

### 4.2 输出结果

```
apple 出现次数: 3
banana 出现次数: 2
grape 出现次数: 0
找到元素: cherry
正向遍历：apple apple apple banana banana cherry date
反向遍历：date cherry banana banana apple apple apple
所有 apple 元素：apple apple apple
apple 范围元素数量: 3
删除 apple 的数量: 3
删除后 multiset 大小: 4
clear 后是否为空: true
```

### 4.3 使用自定义比较函数

```c++
#include "multiset.h"
#include <iostream>

int main() {
    // 使用 std::greater 实现降序排列
    demo::multiset<int, std::greater<int>> ms{1, 2, 2, 3, 4, 5};

    std::cout << "降序遍历：";
    for (const auto& key : ms) {
        std::cout << key << " ";
    }
    std::cout << std::endl; // 输出：5 4 3 2 2 1

    return 0;
}
```

### 4.4 词频统计示例

```c++
#include "multiset.h"
#include <iostream>
#include <string>
#include <vector>

int main() {
    std::vector<std::string> words = {"hello", "world", "hello", "cpp", "world", "hello"};

    // 使用 multiset 进行词频统计
    demo::multiset<std::string> ms(words.begin(), words.end());

    std::cout << "hello 出现次数: " << ms.count("hello") << std::endl; // 3
    std::cout << "world 出现次数: " << ms.count("world") << std::endl; // 2
    std::cout << "cpp 出现次数: " << ms.count("cpp") << std::endl;     // 1

    std::cout << "总词数: " << ms.size() << std::endl; // 6

    return 0;
}
```

## 5. 异常处理

|                  异常场景                  |      抛出类型       |                   预防措施                    |
| :----------------------------------------: | :-----------------: | :-------------------------------------------: |
|       构造 / 插入元素时内存分配失败        |  `std::bad_alloc`   |        捕获异常或提前检查 `max_size()`        |
| `erase()`/`insert()` 传入非法迭代器 |     未定义行为      | 确保迭代器在 `[begin(), end()]` 范围内 |
|  键类型不支持比较函数（未定义 `Compare` 行为） |     未定义行为      |      确保键类型满足比较要求（默认 `std::less`）      |

## 6. 性能说明

### 6.1 时间复杂度

|                     操作                     | 时间复杂度 |            备注            |
| :------------------------------------------: | :--------: | :------------------------: |
|                   查找（`find`/`count`）                   |  O(log n)  |      红黑树二分查找      |
|          插入单个元素（`insert`/`emplace`）           |  O(log n)  |      插入后需平衡调整，multiset 中插入总是成功      |
|             删除单个元素（`erase` 迭代器版）             |  O(log n)  |      删除后需平衡调整      |
|             删除指定键（`erase` 键版本）             |  O(k log n)  |      k 为匹配键的元素数量，删除所有匹配元素      |
|        范围插入 / 删除（`insert`/`erase` 范围）        |  O(m log n) | m 为插入 / 删除的元素个数 |
|                 有序遍历（`begin` → `end`）                 |    O(n)    |      中序遍历所有节点      |
|                 `lower_bound`/`upper_bound`                 |  O(log n)  |      二分查找定位边界      |
|                 `equal_range`                 |  O(log n)  |      调用 lower_bound + upper_bound，返回范围可包含多个元素     |
|        `insert` with hint（提示位置有效）        |  O(log n) 或 O(1) |  有效 hint 可跳过查找直接插入 |
|                   `clear`                   |    O(n)    |        销毁所有节点        |
|                   `swap`                   |    O(1)    |      仅交换指针和元数据      |

### 6.2 性能优化建议

1. **使用 `emplace` 替代 `insert`**：直接原地构造键，避免不必要的拷贝 / 移动开销；
2. **使用 hint 版本的 `insert`**：当已知插入位置附近时，使用 `insert(pos, value)` 可能跳过查找优化为 O(1)；
3. **批量插入时优先使用有序数据**：可通过 `sorted_tag` 从已排序 vector 快速构建平衡树；
4. **利用 `count()` 统计元素出现次数**：对 multiset 而言，`count()` 返回实际出现次数，可用于词频统计等场景；
5. **使用 `equal_range()` 遍历重复键**：当需要遍历所有重复键时，`equal_range()` 提供高效的范围查询；
6. **批量删除重复键时使用 `erase(key)`**：直接删除指定键的所有元素，比逐个遍历删除更高效；
7. **利用有序遍历特性**：multiset 按键排序存储，适合需要有序遍历和范围查询的场景。

## 7. 与 std::multiset 的差异

|    特性    |             demo::multiset              |      std::multiset       |
| :--------: | :---------------------------------: | :------------------: |
|  底层结构  |           红黑树（手写实现）           |        红黑树（编译器实现）        |
| 分配器支持 |    支持 `std::allocator` 及自定义分配器     |           完全一致           |
| 迭代器类型 |           双向迭代器，对齐标准            |           完全一致           |
|  异常安全  |  内存分配失败时抛 `std::bad_alloc`，其他情况行为与标准一致 |           完全一致           |
|   扩展接口  |     支持 `sorted_tag`/`unsorted_tag` 从 vector 构造     |              不支持              |
|  内存管理  |       哨兵节点统一管理，析构自动释放        |           完全一致           |
|  重复键处理  |       允许重复键，插入总是成功，等价键插入右子树        |           完全一致           |

## 8. 与 demo::set 的区别与联系

|    特性    |             demo::multiset              |      demo::set       |
| :--------: | :---------------------------------: | :------------------: |
|  键唯一性  |              允许重复键，同一个键可以出现多次              |        键唯一，不允许重复        |
|  `insert` 返回值  |        返回 `iterator`（指向插入元素），插入总是成功        |      返回 `std::pair<iterator, bool>`（second 指示是否插入成功）      |
|  `erase(key)` 返回值  |              返回 `size_type`（实际删除的元素数量，可大于 1）              |   返回 `size_type`（0 或 1）   |
|  `count(key)` 返回值  |           返回 `size_type`（可大于 1）           |           返回 `size_type`（0 或 1）           |
|  `equal_range`  |           返回范围可包含多个元素           |           返回范围最多包含一个元素           |
|  插入路由策略  |     等价键插入右子树，允许重复     |     等价键直接跳过，不插入     |
|  底层实现  |           红黑树（相同实现）           |           红黑树（相同实现）           |
|  时间复杂度  |           插入/删除/查找 O(log n)           |           插入/删除/查找 O(log n)           |
|  应用场景  |     词频统计、事件计数、多重集合运算     |     去重、集合运算、唯一性检查     |

## 9. 已知限制

1. **不支持比较运算符（`<`/`>`/`<=`/`>=`）**：仅支持 `==`/`!=`；
2. **不支持 `erase_if`（C++20 特性）**：需手动遍历删除；
3. **不支持 `extract`（C++17 特性）**：无法无拷贝提取节点；
4. **不支持 `value_comp`**：仅支持 `key_comp`；
5. **不支持 `bucket` 接口**：非哈希表实现，无桶概念；
6. **迭代器失效规则**：删除元素时，仅失效指向被删除元素的迭代器，其他迭代器保持有效；
7. **不支持 `count` 的 O(1) 优化**：`count` 通过 `equal_range` 遍历实现，大规模重复键时效率可能不理想。

## 10. 测试建议

1. 基础功能测试：构造、插入、删除、查找、遍历（正向 / 反向）等核心接口；
2. 重复键测试：插入重复键、`count` 计数、`erase` 批量删除、`equal_range` 范围遍历；
3. 边界测试：空 multiset 操作、单元素 multiset、大量重复键、大量元素插入 / 删除；
4. 异常测试：内存分配失败、非法迭代器操作；
5. 性能测试：对比 `std::multiset` 的插入 / 删除 / 查找效率，验证重复键插入和批量删除性能；
6. 类型兼容性测试：测试内置类型、复杂类型（`std::string`）、自定义类的兼容性；
7. 自定义比较函数测试：测试 `std::greater`、`std::less`、自定义比较器；
8. 红黑树性质验证：使用 `validate_tree()`（DEBUG 模式）验证树结构正确性；
9. 多重集合运算测试：验证 `merge`、`swap`、复制 / 移动语义的正确性；
10. 迭代器正确性测试：验证迭代器在重复键场景下的遍历顺序、`++`/`--`、`lower_bound`/`upper_bound`/`equal_range` 的正确性。

---

**文档版本**：v1.0
**创建日期**：2026-06-04
**适用容器**：`demo::multiset<Key, Compare, Allocator>`