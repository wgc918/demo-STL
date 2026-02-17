# demo::vector 容器开发文档

## 1. 文档概述

### 1.1 文档目的

本文档详细描述自定义实现的 `demo::vector` 容器的设计思路、核心功能、使用方法及注意事项，旨在为开发者提供完整的开发、测试和维护参考。

### 1.2 容器简介

`demo::vector` 是一个兼容 C++ 标准库接口的动态数组容器，支持随机访问、动态扩容、元素增删改查等核心功能，底层基于分配器（Allocator）管理内存，遵循 RAII 设计原则，保证内存安全。

### 1.3 适用范围

- 兼容 C++17 及以上标准的编译环境；
- 支持任意可拷贝 / 可移动的元素类型（包括内置类型、`std::string` 等复杂类型）；
- 可替换标准库 `std::vector` 进行基础场景使用（核心接口对齐）。

## 2. 设计思路

### 2.1 核心架构

`demo::vector` 采用「三指针 / 三成员」经典设计模式，核心成员变量如下：

|   成员变量    |    类型     |                         作用                         |
| :-----------: | :---------: | :--------------------------------------------------: |
|   `m_data`    |    `T*`     |              指向动态分配的内存起始地址              |
|   `m_size`    | `size_type` |                容器中已存储的元素数量                |
| `m_capacity`  | `size_type` | 容器已分配的内存容量（可存储的最大元素数，无需扩容） |
| `m_allocator` | `Allocator` | 内存分配器，负责内存的申请 / 释放、元素的构造 / 析构 |

### 2.2 内存管理策略

1. **扩容规则**：当新增元素导致 `m_size >= m_capacity` 时，扩容至 `(m_capacity + 2) * 2`（空容器初始扩容至 4），避免频繁扩容；
2. **内存分配**：通过 `std::allocator_traits` 封装分配器接口，统一调用 `allocate`/`deallocate` 管理内存，`construct`/`destroy` 管理元素生命周期；
3. **异常安全**：扩容、插入元素时通过 try-catch 保证异常回滚，避免内存泄漏；
4. **缩容机制**：提供 `shrink_to_fit()` 接口，将容量收缩至实际元素数量，释放冗余内存。

### 2.3 迭代器设计

实现符合 C++ 标准的随机访问迭代器（`random_access_iterator_tag`），包含：

- `iterator`：可变迭代器，支持元素修改；
- `const_iterator`：不可变迭代器，仅支持元素访问；
- `reverse_iterator`/`const_reverse_iterator`：基于标准库适配器实现反向迭代器。

迭代器核心特性：支持算术运算（`+`/`-`/`+=`/`-=`）、比较运算（`==`/`!=`/`<`/`>` 等）、随机访问（`[]`）。

## 3. 接口说明

### 3.1 类型别名（Type Aliases）

|         类型别名         |                含义                |
| :----------------------: | :--------------------------------: |
|       `value_type`       |     容器存储的元素类型（`T`）      |
|        `pointer`         |        元素指针类型（`T*`）        |
|     `const_pointer`      |   常量元素指针类型（`const T*`）   |
|       `reference`        |        元素引用类型（`T&`）        |
|    `const_reference`     |   常量元素引用类型（`const T&`）   |
|     `allocator_type`     |     分配器类型（`Allocator`）      |
|       `size_type`        |  无符号整数类型（`std::size_t`）   |
|    `difference_type`     | 有符号整数类型（`std::ptrdiff_t`） |
|        `iterator`        |         可变随机访问迭代器         |
|     `const_iterator`     |        不可变随机访问迭代器        |
|    `reverse_iterator`    |           反向可变迭代器           |
| `const_reverse_iterator` |          反向不可变迭代器          |

### 3.2 构造与析构

|                   接口                   |                 功能说明                  |                             示例                             |
| :--------------------------------------: | :---------------------------------------: | :----------------------------------------------------------: |
|                `vector()`                |            空构造，创建空容器             |                `demo::vector<std::string> v;`                |
|         `vector(size_type size)`         |   创建包含 `size` 个默认构造元素的容器    |                  `demo::vector<int> v(5);`                   |
|  `vector(size_type size, const T& val)`  | 创建包含 `size` 个值为 `val` 的元素的容器 |          `demo::vector<std::string> v(3, "hello");`          |
|  `vector(InputIt first, InputIt last)`   |              迭代器范围构造               | `std::string arr[] = {"a","b"}; demo::vector<std::string> v(arr, arr+2);` |
| `vector(std::initializer_list<T> ilist)` |              初始化列表构造               |         `demo::vector<std::string> v{"a","b","c"};`          |
|      `vector(const vector& other)`       |                 拷贝构造                  |             `demo::vector<std::string> v2(v1);`              |
|    `vector(vector&& other) noexcept`     |                 移动构造                  |        `demo::vector<std::string> v2(std::move(v1));`        |
|               `~vector()`                |     析构函数，释放内存并销毁所有元素      |                              -                               |

### 3.3 赋值操作

|                        接口                         |          功能说明          |          示例           |
| :-------------------------------------------------: | :------------------------: | :---------------------: |
|      `vector& operator=(const vector& other)`       |          拷贝赋值          |       `v2 = v1;`        |
|    `vector& operator=(vector&& other) noexcept`     |          移动赋值          |  `v2 = std::move(v1);`  |
| `vector& operator=(std::initializer_list<T> ilist)` |       初始化列表赋值       |  `v = {"x","y","z"};`   |
|    `void assign(size_type count, const T& val)`     | 赋值 `count` 个 `val` 元素 | `v.assign(2, "test");`  |
|     `void assign(InputIt first, InputIt last)`      |       迭代器范围赋值       | `v.assign(arr, arr+2);` |
|    `void assign(std::initializer_list<T> ilist)`    |       初始化列表赋值       | `v.assign({"a","b"});`  |

### 3.4 元素访问

|                        接口                         |              功能说明               |             注意事项             |
| :-------------------------------------------------: | :---------------------------------: | :------------------------------: |
|           `reference at(size_type index)`           | 访问索引 `index` 的元素，越界抛异常 | 安全访问，抛 `std::out_of_range` |
|     `const_reference at(size_type index) const`     |           常量版本 `at()`           |                -                 |
|       `reference operator[](size_type index)`       |      随机访问元素，无越界检查       |   高效但不安全，越界未定义行为   |
| `const_reference operator[](size_type index) const` |            常量版本 `[]`            |                -                 |
|                 `reference front()`                 |           访问第一个元素            | 空容器调用抛 `std::out_of_range` |
|           `const_reference front() const`           |         常量版本 `front()`          |                -                 |
|                 `reference back()`                  |          访问最后一个元素           | 空容器调用抛 `std::out_of_range` |
|           `const_reference back() const`            |          常量版本 `back()`          |                -                 |
|                  `pointer data()`                   |       返回指向底层数组的指针        |      直接操作指针需注意越界      |
|            `const_pointer data() const`             |          常量版本 `data()`          |                -                 |

### 3.5 迭代器

|                   接口                   |                  功能说明                  |
| :--------------------------------------: | :----------------------------------------: |
|            `iterator begin()`            |       返回指向第一个元素的可变迭代器       |
|      `const_iterator begin() const`      |      返回指向第一个元素的不可变迭代器      |
|     `const_iterator cbegin() const`      |  返回指向第一个元素的不可变迭代器（显式）  |
|             `iterator end()`             |    返回指向末尾的可变迭代器（尾后位置）    |
|       `const_iterator end() const`       |         返回指向末尾的不可变迭代器         |
|      `const_iterator cend() const`       |     返回指向末尾的不可变迭代器（显式）     |
|       `reverse_iterator rbegin()`        |     返回反向迭代器（指向最后一个元素）     |
| `const_reverse_iterator rbegin() const`  |            常量版本 `rbegin()`             |
| `const_reverse_iterator crbegin() const` |          显式常量版本 `rbegin()`           |
|        `reverse_iterator rend()`         | 返回反向迭代器（指向第一个元素的前一位置） |
|  `const_reverse_iterator rend() const`   |             常量版本 `rend()`              |
|  `const_reverse_iterator crend() const`  |           显式常量版本 `rend()`            |

### 3.6 容量操作

|               接口                |                 功能说明                 |            示例            |
| :-------------------------------: | :--------------------------------------: | :------------------------: |
|       `bool empty() const`        |             判断容器是否为空             |  `if (v.empty()) { ... }`  |
|     `size_type size() const`      |               返回元素数量               |    `auto s = v.size();`    |
|   `size_type max_size() const`    | 返回容器最大可存储元素数（由分配器决定） | `auto ms = v.max_size();`  |
| `void reserve(size_type new_cap)` |     预分配至少 `new_cap` 容量的内存      |     `v.reserve(100);`      |
|   `size_type capacity() const`    |               返回当前容量               | `auto cap = v.capacity();` |
|      `void shrink_to_fit()`       |          收缩容量至实际元素数量          |    `v.shrink_to_fit();`    |
|   `void resize(size_type size)`   |      调整容器大小，新增元素默认构造      |       `v.resize(5);`       |

### 3.7 修改操作

|                             接口                             |            功能说明             |                 注意事项                 |
| :----------------------------------------------------------: | :-----------------------------: | :--------------------------------------: |
|                        `void clear()`                        |    清空所有元素（保留容量）     |          仅析构元素，不释放内存          |
|     `iterator insert(const_iterator pos, const T& val)`      |   在 `pos` 位置插入元素 `val`   |          返回指向新元素的迭代器          |
|        `iterator insert(const_iterator pos, T&& val)`        |          移动插入元素           |                    -                     |
| `iterator insert(const_iterator pos, size_type count, const T& val)` |   插入 `count` 个 `val` 元素    |                    -                     |
| `iterator insert(const_iterator pos, InputIt first, InputIt last)` |      插入迭代器范围的元素       |                    -                     |
| `iterator insert(const_iterator pos, std::initializer_list<T> ilist)` |       插入初始化列表元素        |                    -                     |
| `template <typename... Args> void emplace(const_iterator pos, Args&&... args)` |   原地构造元素插入 `pos` 位置   |            避免拷贝，效率更高            |
|             `iterator erase(const_iterator pos)`             |      删除 `pos` 位置的元素      |     返回指向删除元素下一位置的迭代器     |
| `iterator erase(const_iterator first, const_iterator last)`  | 删除 `[first, last)` 范围的元素 |                    -                     |
|                `void push_back(const T& val)`                |          尾部添加元素           |            容量不足时自动扩容            |
|                  `void push_back(T&& val)`                   |        尾部移动添加元素         |                    -                     |
| `template <typename... Args> reference emplace_back(Args&&... args)` |        尾部原地构造元素         |             返回新元素的引用             |
|                      `void pop_back()`                       |        删除最后一个元素         |     空容器调用抛 `std::out_of_range`     |
|                  `void swap(vector& other)`                  |       交换两个容器的内容        | 常数时间复杂度，仅交换指针 / 大小 / 容量 |

### 3.8 运算符重载

|                    接口                    |                  功能说明                   |
| :----------------------------------------: | :-----------------------------------------: |
| `bool operator==(const vector& rhs) const` | 判断两个容器是否相等（元素数量 + 值均相同） |
| `bool operator!=(const vector& rhs) const` |           判断两个容器是否不相等            |

## 4. 使用示例

### 4.1 基础使用

```c++
#include "vector.h"
#include <iostream>
#include <string>

int main() 
{
    // 1. 构造容器
    demo::vector<std::string> v{"apple", "banana", "cherry"};
    
    // 2. 元素访问
    std::cout << "第一个元素：" << v.front() << std::endl;
    std::cout << "索引1的元素：" << v[1] << std::endl;
    
    // 3. 新增元素
    v.push_back("date");
    v.emplace_back("elderberry", 5); // 构造 "elderb"
    
    // 4. 插入元素
    auto it = v.insert(v.begin() + 2, "grape");
    std::cout << "插入的元素：" << *it << std::endl;
    
    // 5. 删除元素
    v.erase(v.end() - 1);
    
    // 6. 遍历元素
    for (const auto& s : v) 
    {
        std::cout << s << " ";
    }
    std::cout << std::endl;
    
    // 7. 容量操作
    std::cout << "size: " << v.size() << ", capacity: " << v.capacity() << std::endl;
    v.shrink_to_fit();
    std::cout << "shrink后 capacity: " << v.capacity() << std::endl;
    
    return 0;
}
```

### 4.2 输出结果

```bash
第一个元素：apple
索引1的元素：banana
插入的元素：grape
apple banana grape cherry date 
size: 5, capacity: 8
shrink后 capacity: 5
```

## 5. 异常处理

|              异常场景               |      抛出类型       |                预防措施                |
| :---------------------------------: | :-----------------: | :------------------------------------: |
| `at()`/`front()`/`back()` 越界访问  | `std::out_of_range` |   访问前通过 `empty()`/`size()` 检查   |
| `erase()`/`insert()` 传入非法迭代器 | `std::out_of_range` | 确保迭代器在 `[begin(), end()]` 范围内 |
|       `pop_back()` 调用空容器       | `std::out_of_range` |         调用前检查 `!empty()`          |
|            内存分配失败             |  `std::bad_alloc`   |  扩容前检查 `max_size()`，或捕获异常   |

## 6. 性能说明

### 6.1 时间复杂度

|               操作               | 时间复杂度 |                备注                |
| :------------------------------: | :--------: | :--------------------------------: |
|     随机访问（`[]`/`at()`）      |    O(1)    |            直接指针偏移            |
|  `push_back()`/`emplace_back()`  | 均摊 O (1) | 扩容时为 O (n)，但均摊后仍为 O (1) |
|           `pop_back()`           |    O(1)    |         仅析构最后一个元素         |
| `insert()`/`erase()`（中间位置） |    O(n)    |          需要移动后续元素          |
|            `resize()`            |    O(n)    |    新增 / 删除元素需构造 / 析构    |
|             `swap()`             |    O(1)    |      仅交换指针 / 大小 / 容量      |
|            `clear()`             |    O(n)    |            析构所有元素            |

### 6.2 优化建议

1. 批量插入元素前调用 `reserve()` 预分配容量，避免多次扩容；
2. 优先使用 `emplace_back()`/`emplace()` 替代 `push_back()`/`insert()`，减少拷贝；
3. 大量删除元素后调用 `shrink_to_fit()` 释放冗余内存；
4. 移动语义（`std::move`）可显著提升复杂类型（如 `std::string`）的操作效率。

## 7. 已知限制

1. 不支持 `std::vector` 的部分高级接口（如 `get_allocator()` 仅返回分配器副本，未支持分配器扩展）；
2. 迭代器失效规则与标准库一致：扩容、`insert()`/`erase()` 会导致迭代器失效，使用时需重新获取；
3. 暂未实现比较运算符（`<`/`>`/`<=`/`>=`），仅支持 `==`/`!=`；
4. 不支持自定义扩容因子，固定为 `(capacity + 2) * 2`。

## 8. 测试建议

1. 覆盖基础功能测试（构造、访问、增删改查）；
2. 重点测试异常场景（越界访问、空容器操作、内存分配失败）；
3. 测试复杂类型（如 `std::string`、自定义类）的兼容性；
4. 性能测试：对比标准库 `std::vector` 的扩容、插入、遍历效率。