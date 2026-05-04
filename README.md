# demo-STL - 自定义STL容器实现

[![License](https://img.shields.io/badge/License-MIT-blue.svg)](LICENSE)
[![C++ Standard](https://img.shields.io/badge/C%2B%2B-17-blue.svg)](https://en.cppreference.com/w/cpp/17)

## 项目概述

`demo-STL` 是一个基于 C++17 标准实现的自定义 STL（Standard Template Library）容器库，旨在深入理解 STL 容器的底层实现原理，同时提供与标准库兼容的接口设计。本项目不仅是学习 STL 内部机制的绝佳资源，也是一个可用于实际项目的轻量级容器库。

### 项目目标

- **教育价值**：提供清晰、可读的 STL 容器实现代码，帮助开发者深入理解容器的内部工作机制
- **兼容性**：接口设计严格对齐 `std::` 标准库，保证代码的可移植性和易用性
- **扩展性**：模块化设计，便于添加新的容器类型和功能扩展
- **性能优化**：在保证正确性的前提下，优化关键路径的性能表现
- **异常安全**：提供完善的异常处理机制，确保内存安全和程序稳定性

### 设计理念

1. **RAII 原则**：所有资源管理遵循 RAII（Resource Acquisition Is Initialization）原则，确保异常安全
2. **分配器抽象**：使用 `std::allocator` 作为默认分配器，支持自定义分配器扩展
3. **迭代器设计**：实现符合标准的迭代器类型，支持 STL 算法库
4. **异常处理**：提供明确的异常抛出机制，便于调试和错误处理
5. **代码规范**：遵循现代 C++ 编码规范，代码清晰、注释完善

## 功能特性

### 已实现容器

| 容器名称             | 类型     | 状态      | 核心特性                                |
| :------------------- | :------- | :-------- | :-------------------------------------- |
| `demo::vector`       | 动态数组 | ✅ 已实现 | 随机访问、动态扩容、异常安全            |
| `demo::forward_list` | 单向链表 | ✅ 已实现 | 前向迭代、O(1) 前端操作、低内存开销     |
| `demo::list`         | 双向链表 | ✅ 已实现 | 双向迭代、O(1) 任意位置操作、迭代器稳定 |
| `demo::deque`        | 双端队列 | ✅ 已实现 | 双端操作、分段存储、随机访问迭代器      |

### 核心功能

- **动态数组容器 (`demo::vector`)**
  - 支持随机访问（`operator[]`、`at()`）
  - 智能动态扩容策略
  - 完整的元素生命周期管理
  - 支持移动语义和完美转发

- **单向链表容器 (`demo::forward_list`)**
  - 仅支持前向迭代器
  - O(1) 时间复杂度的前端插入/删除
  - 内置归并排序算法
  - 低内存开销（每个节点仅一个指针）

- **双向链表容器 (`demo::list`)**
  - 支持双向迭代器和反向迭代器
  - O(1) 时间复杂度的首尾操作和任意位置插入/删除
  - 迭代器稳定性（增删不影响其他迭代器）
  - 内置排序、合并、拼接、反转、去重等操作

- **双端队列容器 (`demo::deque`)**
  - 支持随机访问迭代器（`operator[]`、`at()`）
  - O(1) 时间复杂度的双端增删操作（`push_front`/`pop_front`、`push_back`/`pop_back`）
  - 分段存储结构（Map + Buffer），避免扩容时的大量元素移动
  - 支持移动语义和完美转发，高效的 emplace 系列接口

## 快速开始

### 环境要求

- **编译器**：GCC 8.0+、Clang 6.0+、MSVC 2019+
- **标准**：C++17 及以上
- **构建工具**：CMake 3.16+（推荐）

### 编译项目

```bash
# 创建构建目录
mkdir build && cd build

# 生成构建文件
cmake ..

# 编译项目
cmake --build .

# 运行测试
ctest
```

### 使用示例

```c++
#include <iostream>
#include "list/list.h"

int main() {
    // 创建双向链表
    demo::list<int> lst{1, 2, 3, 4, 5};

    // 尾部插入
    lst.push_back(6);

    // 头部插入
    lst.push_front(0);

    // 遍历链表
    for (const auto& val : lst) {
        std::cout << val << " ";
    }
    std::cout << std::endl;

    // 排序
    lst.sort();

    // 反向遍历
    for (auto it = lst.rbegin(); it != lst.rend(); ++it) {
        std::cout << *it << " ";
    }
    std::cout << std::endl;

    return 0;
}
```

## 项目结构

```
demo-STL/
├── README.md                    # 项目说明文档（本文件）
├── 自定义STL容器综合文档.md       # 容器综合文档入口
├── CMakeLists.txt               # 项目构建配置
├── list/                        # 链表容器模块
│   ├── list.h                   # 双向链表实现
│   ├── forward_list.h           # 单向链表实现
│   ├── list.md                  # list详细文档
│   ├── forward_list.md          # forward_list详细文档
│   └── utests/                  # 测试用例目录
│       ├── CMakeLists.txt       # 测试构建配置
│       ├── doctest.h            # 测试框架
│       ├── test_list.cpp        # list测试用例
│       └── main.cpp             # 测试入口
├── vector/                      # 动态数组容器模块
│   ├── vector.h                 # 动态数组实现
│   ├── vector.md                # vector详细文档
│   └── utests/                  # 测试用例目录
│       ├── CMakeLists.txt       # 测试构建配置
│       ├── doctest.h            # 测试框架
│       ├── test_vector.cpp      # vector测试用例
│       └── main.cpp             # 测试入口
└── deque/                       # 双端队列容器模块
    ├── deque.h                  # 双端队列实现
    ├── deque.md                 # deque详细文档
    └── utest/                   # 测试用例目录
        ├── CMakeLists.txt       # 测试构建配置
        ├── doctest.h            # 测试框架
        ├── test_deque.cpp       # deque测试用例
        └── main.cpp             # 测试入口
```

## API 参考

### 容器总览

完整的 API 文档请参考：[自定义STL容器综合文档](./自定义STL容器综合文档.md)

#### 通用接口

所有容器均支持以下通用操作：

| 接口              | 功能说明                          |
| :---------------- | :-------------------------------- |
| `empty()`         | 判断容器是否为空                  |
| `size()`          | 返回元素个数（forward_list 除外） |
| `max_size()`      | 返回最大可存储元素数              |
| `clear()`         | 清空容器                          |
| `swap()`          | 交换两个容器内容                  |
| `get_allocator()` | 返回分配器副本                    |

#### 迭代器接口

| 迭代器类型         | 说明           | 支持操作                               | 支持容器      |
| :----------------- | :------------- | :------------------------------------- | :------------ |
| `iterator`         | 可变迭代器     | `++`, `--`, `*`, `->`                  | 所有容器      |
| `const_iterator`   | 常量迭代器     | 只读访问                               | 所有容器      |
| `reverse_iterator` | 反向迭代器     | 反向遍历                               | list, deque   |
| `random_access`    | 随机访问迭代器 | `++`, `--`, `[]`, `+`, `-`, 比较运算符 | vector, deque |

## 测试框架

本项目使用 [doctest](https://github.com/onqtam/doctest) 作为测试框架，测试用例位于 `*/utests/` 目录下：

- **测试覆盖**：每个容器均包含完整的单元测试
- **测试类型**：涵盖正常流程、边界条件、异常场景
- **运行方式**：通过 CTest 运行所有测试用例

## 贡献指南

欢迎提交 Issue 和 Pull Request！

### 提交规范

1. **代码风格**：遵循 Google C++ 编码规范
2. **提交信息**：使用清晰的提交信息，格式如下：

   ```
   [类型] 描述

   详细说明（可选）
   ```

   类型：`feat`（新功能）、`fix`（修复）、`docs`（文档）、`refactor`（重构）、`test`（测试）

3. **测试要求**：新增功能必须配套单元测试

### 开发流程

```
1. Fork 仓库
2. 创建功能分支
3. 实现功能并编写测试
4. 提交代码并创建 Pull Request
5. 代码审查通过后合并
```

## 许可证

本项目采用 MIT 许可证，详见 [LICENSE](LICENSE) 文件。

## 致谢

- 感谢 C++ 标准库委员会提供的优秀接口设计
- 感谢 doctest 提供的轻量级测试框架
- 感谢所有为项目做出贡献的开发者

---

**文档版本**：v1.5
**最后更新**：2026-05-05
**项目地址**：[demo-STL](https://github.com/wgc918/demo-STL)

> **提示**：如需了解各容器的详细实现，请查阅 [自定义STL容器综合文档](./自定义STL容器综合文档.md)
