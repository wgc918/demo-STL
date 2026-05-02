//-----------------------------------------------------------------------------
// 版权所有 (C) 2026 demo-STL 项目
//
// 文件: deque.h
// 作者: wgc
// 创建日期: 2026年1月
// 最后修改: 2026年4月
//
// 描述:
//     本文件实现了一个 STL 风格的双端队列容器 (deque)。
//     deque 是一种双端队列，支持在头尾两端高效插入和删除元素，
//     并提供随机访问迭代器支持。
//
// 功能特性:
//     - 随机访问迭代器支持
//     - 高效的头尾插入/删除操作
//     - 动态扩展的分段存储结构
//     - 符合 STL 容器规范
//
// 实现说明:
//     deque 使用二级指针结构（map + buffer）实现，其中 map 是指向 buffer 的指针数组，
//     每个 buffer 包含固定数量（m_buffer_size）的元素。这种设计使得 deque 能够在
//     两端高效扩展，同时保持良好的内存局部性。
//
// 许可证:
//     MIT License
//
//     版权所有 (c) 2026 wgc
//
//     特此免费授予获得本软件副本和相关文档文件（以下简称"软件"）的任何人以处理软件的权利，
//     包括但不限于使用、复制、修改、合并、出版、分发、再许可和/或出售软件副本，
//     以及允许软件适用者这样做，须在下列条件下：
//
//     上述版权声明和本许可声明应包含在软件的所有副本或实质性部分中。
//
//     软件按"原样"提供，不提供任何形式的明示或暗示的保证，
//     包括但不限于对适销性、特定用途适用性和非侵权性的保证。
//     在任何情况下，作者或版权持有人均不对任何索赔、损害或其他责任负责，
//     无论是在合同诉讼、侵权诉讼或其他诉讼中，
//     由于软件或软件的使用或其他交易产生的。
//-----------------------------------------------------------------------------

#pragma once

#include <type_traits>
#include <iterator>
#include <memory>
#include <initializer_list>

namespace demo
{
/// @brief deque 容器默认初始化大小
/// @details deque 容器默认初始化时 map 的大小为 8
#define DEQUE_DEFAULT_INIT_SIZE 8

/// @brief deque 容器默认 buffer 大小
/// @details deque 容器默认 buffer 大小为 512 字节
#define DEQUE_DEFAULT_BUFFER_SIZE 512

    /// @brief deque 容器类
    /// @details deque（双端队列）是一种高效的序列容器，支持在头尾两端快速插入和删除元素，
    ///          并提供随机访问能力。deque 采用分段存储结构，通过 map 数组管理多个 buffer，
    ///          每个 buffer 存储固定数量的元素。
    template <typename T, typename Allocator = std::allocator<T>>
    class deque;

    /// @brief 比较两个双端队列是否相等
    /// @tparam T 元素类型
    /// @tparam Allocator 分配器类型
    /// @param lhs 左操作数
    /// @param rhs 右操作数
    /// @return 如果两个双端队列相等返回true，否则返回false
    template <typename T, typename Allocator>
    bool operator==(const deque<T, Allocator> &lhs,
                    const deque<T, Allocator> &rhs);

    /// @brief 比较两个双端队列是否不相等
    /// @tparam T 元素类型
    /// @tparam Allocator 分配器类型
    /// @param lhs 左操作数
    /// @param rhs 右操作数
    /// @return 如果两个双端队列不相等返回true，否则返回false
    template <typename T, typename Allocator>
    bool operator!=(const deque<T, Allocator> &lhs,
                    const deque<T, Allocator> &rhs);

    template <typename T, typename Allocator>
    class deque
    {
        friend bool operator== <>(const deque<T, Allocator> &lhs,
                                  const deque<T, Allocator> &rhs);
        friend bool operator!= <>(const deque<T, Allocator> &lhs,
                                  const deque<T, Allocator> &rhs);

    public:
        using value_type = T;                   ///< 元素类型
        using pointer = T *;                    ///< 元素指针类型
        using const_pointer = const T *;        ///< 常量元素指针类型
        using reference = T &;                  ///< 元素引用类型
        using const_reference = const T &;      ///< 常量元素引用类型
        using size_type = std::size_t;          ///< 大小类型
        using difference_type = std::ptrdiff_t; ///< 差值类型
        using allocator_type = Allocator;       ///< 分配器类型

        class const_iterator;

        /// @brief 随机访问迭代器类
        /// @details deque 的迭代器采用四级结构：当前元素指针、当前槽起始指针、
        ///          当前槽结束指针、以及指向当前槽在 map 中位置的指针。
        ///          这种设计支持高效的随机访问和跨槽遍历。
        class iterator
        {
            friend class deque;

        public:
            using iterator_category =
                std::random_access_iterator_tag;    ///< 迭代器类别（随机访问迭代器）
            using value_type = T;                   ///< 元素类型
            using pointer = T *;                    ///< 元素指针类型
            using reference = T &;                  ///< 元素引用类型
            using difference_type = std::ptrdiff_t; ///< 差值类型
            using size_type = std::size_t;          ///< 大小类型

            /// @brief 默认构造函数，创建空迭代器
            iterator();

            /// @brief 构造函数，从指针创建迭代器
            /// @param cur 当前元素指针
            /// @param first 当前槽的第一个元素指针
            /// @param last 当前槽的最后一个元素指针（不包含）
            /// @param map_node 当前槽在 map 中的指针
            iterator(pointer cur, pointer first, pointer last, value_type **map_node);

            /// @brief 解引用操作符，返回当前元素的引用
            /// @return 当前元素的引用
            reference operator*() const;

            /// @brief 箭头操作符，返回当前元素的指针
            /// @return 当前元素的指针
            pointer operator->() const;

            /// @brief 计算两个迭代器之间的距离
            /// @param other 另一个迭代器
            /// @return 两个迭代器之间的元素数量差
            difference_type operator-(const iterator &other) const;

            /// @brief 加法操作，返回向后移动 n 个位置的迭代器
            /// @param n 要移动的位置数
            /// @return 移动后的迭代器
            iterator operator+(difference_type n) const;

            /// @brief 减法操作，返回向前移动 n 个位置的迭代器
            /// @param n 要移动的位置数
            /// @return 移动后的迭代器
            iterator operator-(difference_type n) const;

            /// @brief 复合赋值加法，向后移动 n 个位置
            /// @param n 要移动的位置数
            /// @return 移动后的迭代器引用
            iterator &operator+=(difference_type n);

            /// @brief 复合赋值减法，向前移动 n 个位置
            /// @param n 要移动的位置数
            /// @return 移动后的迭代器引用
            iterator &operator-=(difference_type n);

            /// @brief 前置自增，移动到下一个元素
            /// @return 自增后的迭代器引用
            iterator &operator++();

            /// @brief 前置自减，移动到前一个元素
            /// @return 自减后的迭代器引用
            iterator &operator--();

            /// @brief 后置自增，移动到下一个元素
            /// @return 自增前的迭代器副本
            iterator operator++(int);

            /// @brief 后置自减，移动到前一个元素
            /// @return 自减前的迭代器副本
            iterator operator--(int);

            /// @brief 下标访问（const版本），返回偏移 n 个位置的元素迭代器
            /// @param n 偏移量
            /// @return 偏移后的迭代器
            iterator operator[](difference_type n) const;

            /// @brief 下标访问，返回偏移 n 个位置的元素迭代器
            /// @param n 偏移量
            /// @return 偏移后的迭代器
            iterator operator[](difference_type n);

            /// @brief 比较两个迭代器是否相等
            /// @param other 要比较的另一个迭代器
            /// @return 如果相等返回true，否则返回false
            bool operator==(const iterator &other) const;

            /// @brief 比较两个迭代器是否不相等
            /// @param other 要比较的另一个迭代器
            /// @return 如果不相等返回true，否则返回false
            bool operator!=(const iterator &other) const;

            /// @brief 比较迭代器是否小于另一个迭代器
            /// @param other 要比较的另一个迭代器
            /// @return 如果小于返回true，否则返回false
            bool operator<(const iterator &other) const;

            /// @brief 比较迭代器是否大于另一个迭代器
            /// @param other 要比较的另一个迭代器
            /// @return 如果大于返回true，否则返回false
            bool operator>(const iterator &other) const;

            /// @brief 比较迭代器是否小于等于另一个迭代器
            /// @param other 要比较的另一个迭代器
            /// @return 如果小于等于返回true，否则返回false
            bool operator<=(const iterator &other) const;

            /// @brief 比较迭代器是否大于等于另一个迭代器
            /// @param other 要比较的另一个迭代器
            /// @return 如果大于等于返回true，否则返回false
            bool operator>=(const iterator &other) const;

        private:
            pointer m_cur;           ///< 当前元素指针
            pointer m_first;         ///< 当前槽的第一个元素指针
            pointer m_last;          ///< 当前槽的最后一个元素指针（不包含）
            value_type **m_map_node; ///< 当前槽在 map 中的指针
        };

        /// @brief 常量随机访问迭代器类
        /// @details deque 的常量迭代器，不允许修改指向的元素。
        ///          采用与 iterator 相同的四级结构设计。
        class const_iterator
        {
            friend class deque;

        public:
            using iterator_category =
                std::random_access_iterator_tag;    ///< 迭代器类别（随机访问迭代器）
            using value_type = T;                   ///< 元素类型
            using pointer = const T *;              ///< 常量元素指针类型
            using reference = const T &;            ///< 常量元素引用类型
            using difference_type = std::ptrdiff_t; ///< 差值类型
            using size_type = std::size_t;          ///< 大小类型

            /// @brief 默认构造函数，创建空迭代器
            const_iterator();

            /// @brief 构造函数，从指针创建迭代器
            /// @param ptr 当前元素指针
            /// @param first 当前槽的第一个元素指针
            /// @param last 当前槽的最后一个元素指针（不包含）
            /// @param map_node 当前槽在 map 中的指针
            const_iterator(pointer ptr, pointer first, pointer last, value_type **map_node);

            /// @brief 从非const迭代器构造
            /// @param other 非const迭代器
            const_iterator(const iterator &other);

            /// @brief 解引用操作符，返回当前元素的常量引用
            /// @return 当前元素的常量引用
            reference operator*() const;

            /// @brief 箭头操作符，返回当前元素的常量指针
            /// @return 当前元素的常量指针
            pointer operator->() const;

            /// @brief 计算两个迭代器之间的距离
            /// @param other 另一个迭代器
            /// @return 两个迭代器之间的元素数量差
            difference_type operator-(const const_iterator &other) const;

            /// @brief 加法操作，返回向后移动 n 个位置的迭代器
            /// @param n 要移动的位置数
            /// @return 移动后的迭代器
            const_iterator operator+(difference_type n) const;

            /// @brief 减法操作，返回向前移动 n 个位置的迭代器
            /// @param n 要移动的位置数
            /// @return 移动后的迭代器
            const_iterator operator-(difference_type n) const;

            /// @brief 复合赋值加法，向后移动 n 个位置
            /// @param n 要移动的位置数
            /// @return 移动后的迭代器引用
            const_iterator &operator+=(difference_type n);

            /// @brief 复合赋值减法，向前移动 n 个位置
            /// @param n 要移动的位置数
            /// @return 移动后的迭代器引用
            const_iterator &operator-=(difference_type n);

            /// @brief 前置自增，移动到下一个元素
            /// @return 自增后的迭代器引用
            const_iterator &operator++();

            /// @brief 前置自减，移动到前一个元素
            /// @return 自减后的迭代器引用
            const_iterator &operator--();

            /// @brief 后置自增，移动到下一个元素
            /// @return 自增前的迭代器副本
            const_iterator operator++(int);

            /// @brief 后置自减，移动到前一个元素
            /// @return 自减前的迭代器副本
            const_iterator operator--(int);

            /// @brief 下标访问（const版本），返回偏移 n 个位置的元素迭代器
            /// @param n 偏移量
            /// @return 偏移后的迭代器
            const_iterator operator[](difference_type n) const;

            /// @brief 下标访问，返回偏移 n 个位置的元素迭代器
            /// @param n 偏移量
            /// @return 偏移后的迭代器
            const_iterator operator[](difference_type n);

            /// @brief 比较两个迭代器是否相等
            /// @param other 要比较的另一个迭代器
            /// @return 如果相等返回true，否则返回false
            bool operator==(const const_iterator &other) const;

            /// @brief 比较两个迭代器是否不相等
            /// @param other 要比较的另一个迭代器
            /// @return 如果不相等返回true，否则返回false
            bool operator!=(const const_iterator &other) const;

            /// @brief 比较迭代器是否小于另一个迭代器
            /// @param other 要比较的另一个迭代器
            /// @return 如果小于返回true，否则返回false
            bool operator<(const const_iterator &other) const;

            /// @brief 比较迭代器是否大于另一个迭代器
            /// @param other 要比较的另一个迭代器
            /// @return 如果大于返回true，否则返回false
            bool operator>(const const_iterator &other) const;

            /// @brief 比较迭代器是否小于等于另一个迭代器
            /// @param other 要比较的另一个迭代器
            /// @return 如果小于等于返回true，否则返回false
            bool operator<=(const const_iterator &other) const;

            /// @brief 比较迭代器是否大于等于另一个迭代器
            /// @param other 要比较的另一个迭代器
            /// @return 如果大于等于返回true，否则返回false
            bool operator>=(const const_iterator &other) const;

        private:
            pointer m_cur;           ///< 当前元素指针
            pointer m_first;         ///< 当前槽的第一个元素指针
            pointer m_last;          ///< 当前槽的最后一个元素指针（不包含）
            value_type **m_map_node; ///< 当前槽在 map 中的指针
        };

        /// @brief 反向迭代器类型，使用标准库适配器
        using reverse_iterator = std::reverse_iterator<iterator>;

        /// @brief 常量反向迭代器类型，使用标准库适配器
        using const_reverse_iterator =
            std::reverse_iterator<const_iterator>;

        // 构造函数和析构函数

        /// @brief 默认构造函数，创建空双端队列
        deque();

        /// @brief 填充构造函数，创建包含 count 个默认构造元素的双端队列
        /// @param count 元素数量
        explicit deque(size_type count);

        /// @brief 填充构造函数，创建包含 count 个 value 的双端队列
        /// @param count 元素数量
        /// @param value 元素值
        deque(size_type count, const value_type &value);

        /// @brief 范围构造函数，复制 [first, last) 范围内的元素
        /// @tparam InputIt 输入迭代器类型
        /// @param first 范围起始迭代器
        /// @param last 范围结束迭代器
        template <typename InputIt,
                  std::enable_if_t<
                      !std::is_integral<InputIt>::value, int> = 0>
        deque(InputIt first, InputIt last);

        /// @brief 初始化列表构造函数
        /// @param ilist 初始化列表
        deque(std::initializer_list<T> ilist);

        /// @brief 拷贝构造函数
        /// @param other 要拷贝的双端队列
        deque(const deque &other);

        /// @brief 拷贝赋值运算符
        /// @param other 要拷贝的双端队列
        /// @return 当前双端队列的引用
        deque &operator=(const deque &other);

        /// @brief 初始化列表赋值运算符
        /// @param ilist 初始化列表
        /// @return 当前双端队列的引用
        deque &operator=(std::initializer_list<T> ilist);

        /// @brief 移动构造函数
        /// @param other 要移动的双端队列
        deque(deque &&other) noexcept;

        /// @brief 移动赋值运算符
        /// @param other 要移动的双端队列
        /// @return 当前双端队列的引用
        deque &operator=(deque &&other) noexcept;

        /// @brief 析构函数，释放所有资源
        ~deque();

        // 修改器

        /// @brief 将双端队列内容替换为 count 个 value
        /// @param count 元素数量
        /// @param value 元素值
        void assign(size_type count, const value_type &value);

        /// @brief 将双端队列内容替换为 [first, last) 范围内的元素
        /// @tparam InputIt 输入迭代器类型
        /// @param first 范围起始迭代器
        /// @param last 范围结束迭代器
        template <typename InputIt,
                  std::enable_if_t<
                      !std::is_integral<InputIt>::value, int> = 0>
        void assign(InputIt first, InputIt last);

        /// @brief 将双端队列内容替换为初始化列表中的元素
        /// @param ilist 初始化列表
        void assign(std::initializer_list<T> ilist);

        /// @brief 获取分配器
        /// @return 分配器对象
        allocator_type get_allocator() const noexcept;

        // 元素访问

        /// @brief 获取指定位置的元素引用（带边界检查）
        /// @param pos 元素位置
        /// @return 指定位置元素的引用
        /// @throw std::out_of_range 如果 pos 超出范围
        reference at(size_type pos);

        /// @brief 获取指定位置的元素常量引用（带边界检查）
        /// @param pos 元素位置
        /// @return 指定位置元素的常量引用
        /// @throw std::out_of_range 如果 pos 超出范围
        const_reference at(size_type pos) const;

        /// @brief 获取指定位置的元素引用（无边界检查）
        /// @param pos 元素位置
        /// @return 指定位置元素的引用
        reference operator[](size_type pos);

        /// @brief 获取指定位置的元素常量引用（无边界检查）
        /// @param pos 元素位置
        /// @return 指定位置元素的常量引用
        const_reference operator[](size_type pos) const;

        /// @brief 获取第一个元素的引用
        /// @return 第一个元素的引用
        /// @throw std::out_of_range 如果双端队列为空
        reference front();

        /// @brief 获取第一个元素的常量引用
        /// @return 第一个元素的常量引用
        /// @throw std::out_of_range 如果双端队列为空
        const_reference front() const;

        /// @brief 获取最后一个元素的引用
        /// @return 最后一个元素的引用
        /// @throw std::out_of_range 如果双端队列为空
        reference back();

        /// @brief 获取最后一个元素的常量引用
        /// @return 最后一个元素的常量引用
        /// @throw std::out_of_range 如果双端队列为空
        const_reference back() const;

        // 迭代器

        /// @brief 返回指向第一个元素的迭代器
        /// @return 指向首元素的迭代器
        iterator begin();

        /// @brief 返回指向第一个元素的常量迭代器
        /// @return 指向首元素的常量迭代器
        const_iterator begin() const;

        /// @brief 返回指向第一个元素的常量迭代器（const版本）
        /// @return 指向首元素的常量迭代器
        const_iterator cbegin() const;

        /// @brief 返回指向末尾的迭代器
        /// @return 指向末尾的迭代器
        iterator end();

        /// @brief 返回指向末尾的常量迭代器
        /// @return 指向末尾的常量迭代器
        const_iterator end() const;

        /// @brief 返回指向末尾的常量迭代器（const版本）
        /// @return 指向末尾的常量迭代器
        const_iterator cend() const;

        /// @brief 返回指向最后一个元素的反向迭代器
        /// @return 指向末尾的反向迭代器
        reverse_iterator rbegin();

        /// @brief 返回指向最后一个元素的常量反向迭代器
        /// @return 指向末尾的常量反向迭代器
        const_reverse_iterator rbegin() const;

        /// @brief 返回指向最后一个元素的常量反向迭代器（const版本）
        /// @return 指向末尾的常量反向迭代器
        const_reverse_iterator crbegin() const;

        /// @brief 返回指向第一个元素之前位置的反向迭代器
        /// @return 指向首元素之前位置的反向迭代器
        reverse_iterator rend();

        /// @brief 返回指向第一个元素之前位置的常量反向迭代器
        /// @return 指向首元素之前位置的常量反向迭代器
        const_reverse_iterator rend() const;

        /// @brief 返回指向第一个元素之前位置的常量反向迭代器（const版本）
        /// @return 指向首元素之前位置的常量反向迭代器
        const_reverse_iterator crend() const;

        // 容量

        /// @brief 检查双端队列是否为空
        /// @return 如果为空返回true，否则返回false
        bool empty() const;

        /// @brief 返回双端队列中的元素数量
        /// @return 元素数量
        size_type size() const;

        /// @brief 返回双端队列能容纳的最大元素数量
        /// @return 最大元素数量
        size_type max_size() const;

        /// @brief 调整双端队列大小，新元素默认构造
        /// @param count 新的大小
        void resize(size_type count);

        /// @brief 调整双端队列大小，新元素用指定值填充
        /// @param count 新的大小
        /// @param value 用于填充新元素的值
        void resize(size_type count, const value_type &value);

        /// @brief 释放未使用的槽位，减少内存占用
        void shrink_to_fit();

        // 修改器（续）

        /// @brief 清空双端队列，删除所有元素
        void clear() noexcept;

        /// @brief 在指定位置插入元素（拷贝版本）
        /// @param pos 插入位置
        /// @param value 要插入的值
        /// @return 指向新插入元素的迭代器
        iterator insert(const_iterator pos,
                        const value_type &value);

        /// @brief 在指定位置插入元素（移动版本）
        /// @param pos 插入位置
        /// @param value 要插入的值（右值）
        /// @return 指向新插入元素的迭代器
        iterator insert(const_iterator pos,
                        const value_type &&value);

        /// @brief 在指定位置插入 count 个相同元素
        /// @param pos 插入位置
        /// @param count 插入数量
        /// @param value 要插入的值
        /// @return 指向第一个新插入元素的迭代器
        iterator insert(const_iterator pos,
                        size_type count, const value_type &value);

        /// @brief 在指定位置插入 [first, last) 范围内的元素
        /// @tparam InputIt 输入迭代器类型
        /// @param pos 插入位置
        /// @param first 范围起始迭代器
        /// @param last 范围结束迭代器
        /// @return 指向第一个新插入元素的迭代器
        template <typename InputIt,
                  std::enable_if_t<
                      !std::is_integral<InputIt>::value, int> = 0>
        iterator insert(const_iterator pos, InputIt first, InputIt last);

        /// @brief 在指定位置插入初始化列表中的元素
        /// @param pos 插入位置
        /// @param ilist 初始化列表
        /// @return 指向第一个新插入元素的迭代器
        iterator insert(const_iterator pos, std::initializer_list<T> ilist);

        /// @brief 在指定位置原地构造元素
        /// @tparam Args 构造参数类型
        /// @param pos 插入位置
        /// @param args 构造参数
        /// @return 指向新构造元素的迭代器
        template <typename... Args>
        iterator emplace(const_iterator pos, Args &&...args);

        /// @brief 删除指定位置的元素
        /// @param pos 要删除的元素位置
        /// @return 指向被删除元素之后的迭代器
        iterator erase(const_iterator pos);

        /// @brief 删除 [first, last) 范围内的元素
        /// @param first 范围起始迭代器
        /// @param last 范围结束迭代器
        /// @return 指向最后一个被删除元素之后的迭代器
        iterator erase(const_iterator first, const_iterator last);

        /// @brief 在开头添加元素（拷贝版本）
        /// @param value 要添加的值
        void push_front(const value_type &value);

        /// @brief 在开头添加元素（移动版本）
        /// @param value 要添加的值（右值）
        void push_front(const value_type &&value);

        /// @brief 在末尾添加元素（拷贝版本）
        /// @param value 要添加的值
        void push_back(const value_type &value);

        /// @brief 在末尾添加元素（移动版本）
        /// @param value 要添加的值（右值）
        void push_back(const value_type &&value);

        /// @brief 在末尾原地构造元素
        /// @tparam Args 构造参数类型
        /// @param args 构造参数
        /// @return 指向新构造元素的引用
        template <typename... Args>
        reference emplace_back(Args &&...args);

        /// @brief 在开头原地构造元素
        /// @tparam Args 构造参数类型
        /// @param args 构造参数
        /// @return 指向新构造元素的引用
        template <typename... Args>
        reference emplace_front(Args &&...args);

        /// @brief 删除开头元素
        /// @throw std::out_of_range 如果双端队列为空
        void pop_front();

        /// @brief 删除末尾元素
        /// @throw std::out_of_range 如果双端队列为空
        void pop_back();

        /// @brief 交换两个双端队列的内容
        /// @param other 要交换的双端队列
        void swap(deque &other) noexcept;

    private:
        /// @brief map 分配器类型，用于分配存储 buffer 指针的数组
        using map_allocator_type =
            typename std::allocator_traits<allocator_type>::
                template rebind_alloc<pointer>;

        /// @brief 元素分配器的 traits 类型
        using alloc_traits = std::allocator_traits<allocator_type>;

        /// @brief map 分配器的 traits 类型
        using map_alloc_traits =
            std::allocator_traits<map_allocator_type>;

    private:
        value_type **m_map;                 ///< 指向 buffer 指针数组的指针（map）
        size_type m_map_size;               ///< map 的大小（buffer 数量）
        allocator_type m_allocator;         ///< 元素分配器
        map_allocator_type m_map_allocator; ///< map 分配器
        iterator m_begin;                   ///< 指向第一个元素的迭代器
        iterator m_end;                     ///< 指向最后一个元素之后位置的迭代器

        /// @brief 每个 buffer 的大小（元素数量）
        /// @details 默认为 512 字节除以元素大小，保证每个 buffer 约 512 字节
        static const size_type m_buffer_size =
            DEQUE_DEFAULT_BUFFER_SIZE / sizeof(value_type);
    };

    // ----------------------- iterator 实现 ---------------------------
    template <typename T, typename Allocator>
    inline deque<T, Allocator>::iterator::iterator()
        : m_cur(nullptr), m_first(nullptr),
          m_last(nullptr), m_map_node(nullptr)
    {
    }

    template <typename T, typename Allocator>
    inline deque<T, Allocator>::iterator::
        iterator(pointer cur, pointer first, pointer last, value_type **map_node)
        : m_cur(cur), m_first(first), m_last(last), m_map_node(map_node)
    {
    }

    template <typename T, typename Allocator>
    inline typename deque<T, Allocator>::reference
    deque<T, Allocator>::iterator::operator*() const
    {
        return *m_cur;
    }
    template <typename T, typename Allocator>
    inline typename deque<T, Allocator>::pointer
    deque<T, Allocator>::iterator::operator->() const
    {
        return m_cur;
    }

    template <typename T, typename Allocator>
    inline typename deque<T, Allocator>::difference_type
    deque<T, Allocator>::iterator::operator-(const iterator &other) const
    {
        // 如果两个迭代器指向同一个槽，直接返回元素之间的距离
        if (m_map_node == other.m_map_node)
        {
            return m_cur - other.m_cur;
        }
        // 否则，返回跨槽的距离
        else
        {
            difference_type distance = 0;
            size_type slot_distance = m_map_node - other.m_map_node - 1;
            distance += slot_distance * m_buffer_size;
            distance += m_cur - m_first;
            distance += other.m_last - other.m_cur;

            return distance;
        }
    }

    template <typename T, typename Allocator>
    inline typename deque<T, Allocator>::iterator
    deque<T, Allocator>::iterator::operator+(difference_type n) const
    {
        if (m_cur + n < m_last)
        {
            return iterator(m_cur + n, m_first, m_last, m_map_node);
        }
        else
        {
            n -= m_last - m_cur - 1;
            // 计算新的槽指针和元素指针
            value_type **new_map_node = m_map_node + (n / m_buffer_size + 1);
            pointer new_cur = *new_map_node + n % m_buffer_size;
            return iterator(new_cur, *new_map_node, *new_map_node + m_buffer_size, new_map_node);
        }
    }

    template <typename T, typename Allocator>
    inline typename deque<T, Allocator>::iterator
    deque<T, Allocator>::iterator::operator-(difference_type n) const
    {
        if (m_cur - n >= m_first)
        {
            return iterator(m_cur - n, m_first, m_last, m_map_node);
        }
        else
        {
            n -= m_cur - m_first;
            // 计算新的槽指针和元素指针
            value_type **new_map_node = m_map_node - (n / m_buffer_size + 1);
            pointer new_cur = *new_map_node + m_buffer_size - n % m_buffer_size;
            return iterator(new_cur, *new_map_node, *new_map_node + m_buffer_size, new_map_node);
        }
    }

    template <typename T, typename Allocator>
    inline typename deque<T, Allocator>::iterator &
    deque<T, Allocator>::iterator::operator+=(difference_type n)
    {
        *this = *this + n;
        return *this;
    }

    template <typename T, typename Allocator>
    inline typename deque<T, Allocator>::iterator &
    deque<T, Allocator>::iterator::operator-=(difference_type n)
    {
        *this = *this - n;
        return *this;
    }

    template <typename T, typename Allocator>
    inline typename deque<T, Allocator>::iterator &
    deque<T, Allocator>::iterator::operator++()
    {
        if (m_cur + 1 < m_last)
            m_cur++;
        else
        {
            m_map_node++;
            m_cur = *m_map_node;
            m_first = *m_map_node;
            m_last = *m_map_node + m_buffer_size;
        }
        return *this;
    }

    template <typename T, typename Allocator>
    inline typename deque<T, Allocator>::iterator &
    deque<T, Allocator>::iterator::operator--()
    {
        if (m_cur - 1 >= m_first)
            m_cur--;
        else
        {
            m_map_node--;
            m_cur = *m_map_node + m_buffer_size - 1;
            m_first = *m_map_node;
            m_last = *m_map_node + m_buffer_size;
        }
        return *this;
    }

    template <typename T, typename Allocator>
    inline typename deque<T, Allocator>::iterator
    deque<T, Allocator>::iterator::operator++(int)
    {
        iterator temp(*this);
        if (m_cur + 1 < m_last)
            m_cur++;
        else
        {
            m_map_node++;
            m_cur = *m_map_node;
            m_first = *m_map_node;
            m_last = *m_map_node + m_buffer_size;
        }
        return temp;
    }

    template <typename T, typename Allocator>
    inline typename deque<T, Allocator>::iterator
    deque<T, Allocator>::iterator::operator--(int)
    {
        iterator temp(*this);
        if (m_cur - 1 >= m_first)
            m_cur--;
        else
        {
            m_map_node--;
            m_cur = *m_map_node + m_buffer_size - 1;
            m_first = *m_map_node;
            m_last = *m_map_node + m_buffer_size;
        }
        return temp;
    }

    template <typename T, typename Allocator>
    inline typename deque<T, Allocator>::iterator
    deque<T, Allocator>::iterator::operator[](difference_type n) const
    {
        return *this + n;
    }
    template <typename T, typename Allocator>
    inline typename deque<T, Allocator>::iterator
    deque<T, Allocator>::iterator::operator[](difference_type n)
    {
        return *this + n;
    }

    template <typename T, typename Allocator>
    inline bool deque<T, Allocator>::iterator::
    operator==(const iterator &other) const
    {
        return (m_cur == other.m_cur) && (m_first == other.m_first) &&
               (m_last == other.m_last) && (m_map_node == other.m_map_node);
    }

    template <typename T, typename Allocator>
    inline bool deque<T, Allocator>::iterator::
    operator!=(const iterator &other) const
    {
        return !(*this == other);
    }

    template <typename T, typename Allocator>
    inline bool deque<T, Allocator>::iterator::
    operator<(const iterator &other) const
    {
        if (m_map_node == other.m_map_node)
        {
            return m_cur < other.m_cur;
        }
        else
        {
            return m_map_node < other.m_map_node;
        }
    }

    template <typename T, typename Allocator>
    inline bool deque<T, Allocator>::iterator::
    operator>(const iterator &other) const
    {
        return !(*this < other) && *this != other;
    }

    template <typename T, typename Allocator>
    inline bool deque<T, Allocator>::iterator::
    operator<=(const iterator &other) const
    {
        return !(*this > other);
    }

    template <typename T, typename Allocator>
    inline bool deque<T, Allocator>::iterator::
    operator>=(const iterator &other) const
    {
        return !(*this < other);
    }

    // ---------------------- const_iterator 实现 -------------------
    template <typename T, typename Allocator>
    inline deque<T, Allocator>::const_iterator::const_iterator()
        : m_cur(nullptr), m_first(nullptr),
          m_last(nullptr), m_map_node(nullptr)
    {
    }

    template <typename T, typename Allocator>
    inline deque<T, Allocator>::const_iterator::
        const_iterator(pointer ptr, pointer first,
                       pointer last, value_type **map_node)
        : m_cur(ptr), m_first(first), m_last(last), m_map_node(map_node)
    {
    }

    template <typename T, typename Allocator>
    inline deque<T, Allocator>::const_iterator::
        const_iterator(const iterator &other)
        : m_cur(other.m_cur), m_first(other.m_first),
          m_last(other.m_last), m_map_node(other.m_map_node)
    {
    }

    template <typename T, typename Allocator>
    inline typename deque<T, Allocator>::const_iterator::reference
    deque<T, Allocator>::const_iterator::operator*() const
    {
        return *m_cur;
    }
    template <typename T, typename Allocator>
    inline typename deque<T, Allocator>::const_iterator::pointer
    deque<T, Allocator>::const_iterator::operator->() const
    {
        return m_cur;
    }

    template <typename T, typename Allocator>
    inline typename deque<T, Allocator>::difference_type
    deque<T, Allocator>::const_iterator::operator-(const const_iterator &other) const
    {
        // 如果两个迭代器指向同一个槽，直接返回元素之间的距离
        if (m_map_node == other.m_map_node)
        {
            return m_cur - other.m_cur;
        }
        // 否则，返回跨槽的距离
        else
        {
            difference_type distance = 0;
            size_type slot_distance = m_map_node - other.m_map_node - 1;
            distance += slot_distance * m_buffer_size;
            distance += m_cur - m_first;
            distance += other.m_last - other.m_cur;

            return distance;
        }
    }

    template <typename T, typename Allocator>
    inline typename deque<T, Allocator>::const_iterator
    deque<T, Allocator>::const_iterator::operator+(difference_type n) const
    {
        if (m_cur + n < m_last)
        {
            return const_iterator(m_cur + n, m_first, m_last, m_map_node);
        }
        else
        {
            n -= m_last - m_cur - 1;
            // 计算新的槽指针和元素指针
            value_type **new_map_node = m_map_node + (n / m_buffer_size + 1);
            pointer new_cur = *new_map_node + n % m_buffer_size;
            return const_iterator(new_cur, *new_map_node, *new_map_node + m_buffer_size, new_map_node);
        }
    }

    template <typename T, typename Allocator>
    inline typename deque<T, Allocator>::const_iterator
    deque<T, Allocator>::const_iterator::operator-(difference_type n) const
    {
        if (m_cur - n >= m_first)
        {
            return const_iterator(m_cur - n, m_first, m_last, m_map_node);
        }
        else
        {
            n -= m_cur - m_first;
            // 计算新的槽指针和元素指针
            value_type **new_map_node = m_map_node - (n / m_buffer_size + 1);
            pointer new_cur = *new_map_node + m_buffer_size - n % m_buffer_size;
            return const_iterator(new_cur, *new_map_node, *new_map_node + m_buffer_size, new_map_node);
        }
    }

    template <typename T, typename Allocator>
    inline typename deque<T, Allocator>::const_iterator &
    deque<T, Allocator>::const_iterator::operator+=(difference_type n)
    {
        *this = *this + n;
        return *this;
    }

    template <typename T, typename Allocator>
    inline typename deque<T, Allocator>::const_iterator &
    deque<T, Allocator>::const_iterator::operator-=(difference_type n)
    {
        *this = *this - n;
        return *this;
    }

    template <typename T, typename Allocator>
    inline typename deque<T, Allocator>::const_iterator &
    deque<T, Allocator>::const_iterator::operator++()
    {
        if (m_cur + 1 < m_last)
            m_cur++;
        else
        {
            m_map_node++;
            m_cur = *m_map_node;
            m_first = *m_map_node;
            m_last = *m_map_node + m_buffer_size;
        }
        return *this;
    }

    template <typename T, typename Allocator>
    inline typename deque<T, Allocator>::const_iterator &
    deque<T, Allocator>::const_iterator::operator--()
    {
        if (m_cur - 1 >= m_first)
            m_cur--;
        else
        {
            m_map_node--;
            m_cur = *m_map_node + m_buffer_size - 1;
            m_first = *m_map_node;
            m_last = *m_map_node + m_buffer_size;
        }
        return *this;
    }

    template <typename T, typename Allocator>
    inline typename deque<T, Allocator>::const_iterator
    deque<T, Allocator>::const_iterator::operator++(int)
    {
        const_iterator temp(*this);
        if (m_cur + 1 < m_last)
            m_cur++;
        else
        {
            m_map_node++;
            m_cur = *m_map_node;
            m_first = *m_map_node;
            m_last = *m_map_node + m_buffer_size;
        }
        return temp;
    }

    template <typename T, typename Allocator>
    inline typename deque<T, Allocator>::const_iterator
    deque<T, Allocator>::const_iterator::operator--(int)
    {
        const_iterator temp(*this);
        if (m_cur - 1 >= m_first)
            m_cur--;
        else
        {
            m_map_node--;
            m_cur = *m_map_node + m_buffer_size - 1;
            m_first = *m_map_node;
            m_last = *m_map_node + m_buffer_size;
        }
        return temp;
    }

    template <typename T, typename Allocator>
    inline typename deque<T, Allocator>::const_iterator
    deque<T, Allocator>::const_iterator::operator[](difference_type n) const
    {
        return *this + n;
    }

    template <typename T, typename Allocator>
    inline typename deque<T, Allocator>::const_iterator
    deque<T, Allocator>::const_iterator::operator[](difference_type n)
    {
        return *this + n;
    }

    template <typename T, typename Allocator>
    inline bool deque<T, Allocator>::const_iterator::
    operator==(const const_iterator &other) const
    {
        return (m_cur == other.m_cur) && (m_first == other.m_first) &&
               (m_last == other.m_last) && (m_map_node == other.m_map_node);
    }

    template <typename T, typename Allocator>
    inline bool deque<T, Allocator>::const_iterator::
    operator!=(const const_iterator &other) const
    {
        return !(*this == other);
    }

    template <typename T, typename Allocator>
    inline bool deque<T, Allocator>::const_iterator::
    operator<(const const_iterator &other) const
    {
        if (m_map_node == other.m_map_node)
        {
            return m_cur < other.m_cur;
        }
        else
        {
            return m_map_node < other.m_map_node;
        }
    }

    template <typename T, typename Allocator>
    inline bool deque<T, Allocator>::const_iterator::
    operator>(const const_iterator &other) const
    {
        return !(*this < other) && *this != other;
    }

    template <typename T, typename Allocator>
    inline bool deque<T, Allocator>::const_iterator::
    operator<=(const const_iterator &other) const
    {
        return !(*this > other);
    }

    template <typename T, typename Allocator>
    inline bool deque<T, Allocator>::const_iterator::
    operator>=(const const_iterator &other) const
    {
        return !(*this < other);
    }

    // ----------------------- deque 实现 ---------------------------
    template <typename T, typename Allocator>
    deque<T, Allocator>::deque()
        : m_map(nullptr), m_map_size(DEQUE_DEFAULT_INIT_SIZE),
          m_allocator(), m_map_allocator(), m_begin(), m_end()
    {
        m_map = map_alloc_traits::allocate(m_map_allocator, m_map_size);
        for (size_type i = 0; i < m_map_size; i++)
            m_map[i] = nullptr;

        m_begin.m_cur = nullptr;
        m_begin.m_first = nullptr;
        m_begin.m_last = nullptr;
        m_begin.m_map_node = m_map + m_map_size / 2;

        m_end = m_begin;
    }

    template <typename T, typename Allocator>
    deque<T, Allocator>::deque(size_type count)
        : m_map(nullptr), m_map_size(DEQUE_DEFAULT_INIT_SIZE),
          m_allocator(), m_map_allocator(), m_begin(), m_end()
    {
        size_type slots = (count + m_buffer_size - 1) / m_buffer_size;
        m_map_size = std::max(slots, m_map_size);

        m_map = map_alloc_traits::allocate(m_map_allocator, m_map_size);
        for (size_type i = 0; i < m_map_size; i++)
            m_map[i] = nullptr;

        size_type first_slot = (m_map_size - slots) / 2;
        size_type remaining = count;

        // 默认从 first_slot 开始填充， 每个槽位填充 m_buffer_size 个元素
        // 最后一个槽位可能填充不足 m_buffer_size 个元素
        for (size_type i = first_slot; i < first_slot + slots; i++)
        {
            m_map[i] = alloc_traits::allocate(m_allocator, m_buffer_size);

            for (size_type j = 0; j < std::min(m_buffer_size, remaining); j++)
                alloc_traits::construct(m_allocator, m_map[i] + j);

            remaining -= m_buffer_size;
        }

        // m_begin 指向第一个元素
        m_begin.m_cur = m_map[first_slot];
        m_begin.m_first = m_map[first_slot];
        m_begin.m_last = m_map[first_slot] + m_buffer_size;
        m_begin.m_map_node = m_map + first_slot;

        // m_end 指向最后一个元素的下一个位置
        const size_type last_slot = first_slot + slots - 1;
        m_end.m_cur = m_map[last_slot] + (count % m_buffer_size == 0
                                              ? m_buffer_size
                                              : count % m_buffer_size);
        m_end.m_first = m_map[last_slot];
        m_end.m_last = m_map[last_slot] + m_buffer_size;
        m_end.m_map_node = m_map + last_slot;
    }

    template <typename T, typename Allocator>
    deque<T, Allocator>::deque(size_type count, const value_type &value)
        : m_map(nullptr), m_map_size(DEQUE_DEFAULT_INIT_SIZE),
          m_allocator(), m_map_allocator(), m_begin(), m_end()
    {
        assign(count, value);
    }

    template <typename T, typename Allocator>
    template <typename InputIt,
              std::enable_if_t<
                  !std::is_integral<InputIt>::value, int>>
    deque<T, Allocator>::deque(InputIt first, InputIt last)
        : m_map(nullptr), m_map_size(8), m_allocator(),
          m_map_allocator(), m_begin(), m_end()
    {
        assign(first, last);
    }

    template <typename T, typename Allocator>
    deque<T, Allocator>::deque(std::initializer_list<T> ilist)
        : m_map(nullptr), m_map_size(DEQUE_DEFAULT_INIT_SIZE),
          m_allocator(), m_map_allocator(), m_begin(), m_end()
    {
        assign(ilist);
    }

    template <typename T, typename Allocator>
    deque<T, Allocator>::deque(const deque &other)
        : m_map(nullptr), m_map_size(other.m_map_size),
          m_allocator(), m_map_allocator(), m_begin(), m_end()
    {
        m_map = map_alloc_traits::allocate(m_map_allocator, m_map_size);
        size_type first_slot = other.m_begin.m_map_node - m_map;
        size_type slots = other.m_end.m_map_node - other.m_begin.m_map_node + 1;

        // 单独处理第一个槽
        m_map[first_slot] = alloc_traits::allocate(m_allocator, m_buffer_size);
        size_type first_slot_size = std::min(m_buffer_size,
                                             static_cast<size_type>(other.m_begin.m_last - other.m_begin.m_cur));
        for (size_type j = (other.m_begin.m_first - other.m_map[first_slot]);
             j < first_slot_size; j++)
            alloc_traits::construct(m_allocator, m_map[first_slot] + j,
                                    *(other.m_map[first_slot] + j));

        // 单独处理最后一个槽
        size_type last_slot = first_slot + slots - 1;
        m_map[last_slot] = alloc_traits::allocate(m_allocator, m_buffer_size);
        size_type last_slot_size = std::min(m_buffer_size,
                                             static_cast<size_type>(other.m_end.m_cur - other.m_end.m_first));
        for (size_type j = 0; j < last_slot_size; j++)
            alloc_traits::construct(m_allocator, m_map[last_slot] + j,
                                    *(other.m_map[last_slot] + j));

        // 中间槽位一定都是满的
        for (size_type i = first_slot + 1; i < first_slot + slots - 1; i++)
        {
            m_map[i] = alloc_traits::allocate(m_allocator, m_buffer_size);
            for (size_type j = 0; j < m_buffer_size; j++)
                alloc_traits::construct(m_allocator, m_map[i] + j,
                                        *(other.m_map[i] + j));
        }

        // m_begin 指向第一个元素
        m_begin.m_cur = m_map[first_slot] + (other.m_begin.m_first -
                                             other.m_map[first_slot]);
        m_begin.m_first = m_map[first_slot];
        m_begin.m_last = m_map[first_slot] + m_buffer_size;
        m_begin.m_map_node = m_map + first_slot;

        // m_end 指向最后一个元素的下一个位置
        m_end.m_cur = m_map[last_slot] + (other.m_end.m_cur -
                                          other.m_end.m_first);
        m_end.m_first = m_map[last_slot];
        m_end.m_last = m_map[last_slot] + m_buffer_size;
        m_end.m_map_node = m_map + last_slot;
    }

    template <typename T, typename Allocator>
    deque<T, Allocator> &deque<T, Allocator>::operator=(const deque &other)
    {
        if (this != &other)
        {
            clear();
            assign(other.begin(), other.end());
        }
        return *this;
    }

    template <typename T, typename Allocator>
    deque<T, Allocator> &deque<T, Allocator>::
    operator=(std::initializer_list<T> ilist)
    {
        clear();
        assign(ilist);
        return *this;
    }

    template <typename T, typename Allocator>
    deque<T, Allocator>::deque(deque &&other) noexcept
        : m_map(other.m_map), m_map_size(other.m_map_size),
          m_allocator(std::move(other.m_allocator)),
          m_map_allocator(std::move(other.m_map_allocator)),
          m_begin(other.m_begin), m_end(other.m_end)
    {
        // other 重置为有效空状态
        other.m_map_size = DEQUE_DEFAULT_INIT_SIZE;
        other.m_map = map_alloc_traits::allocate(other.m_map_allocator, other.m_map_size);
        other.m_begin = iterator();
        other.m_end = iterator();
    }

    template <typename T, typename Allocator>
    deque<T, Allocator> &deque<T, Allocator>::operator=(deque &&other) noexcept
    {
        if (this != &other)
        {
            clear();
            m_map = other.m_map;
            m_map_size = other.m_map_size;
            m_allocator = std::move(other.m_allocator);
            m_map_allocator = std::move(other.m_map_allocator);
            m_begin = other.m_begin;
            m_end = other.m_end;

            // other 重置为有效空状态
            other.m_map_size = DEQUE_DEFAULT_INIT_SIZE;
            other.m_map = map_alloc_traits::allocate(other.m_map_allocator, other.m_map_size);
            other.m_begin = iterator();
            other.m_end = iterator();
        }
        return *this;
    }

    template <typename T, typename Allocator>
    deque<T, Allocator>::~deque()
    {
        clear();
        map_alloc_traits::deallocate(m_map_allocator, m_map, m_map_size);
    }

    template <typename T, typename Allocator>
    void deque<T, Allocator>::assign(size_type count,
                                     const value_type &value)
    {
        clear();

        size_type slots = (count + m_buffer_size - 1) / m_buffer_size;
        m_map_size = std::max(slots, m_map_size);
        m_map = map_alloc_traits::allocate(m_map_allocator, m_map_size);
        for (size_type i = 0; i < m_map_size; i++)
            m_map[i] = nullptr;

        size_type first_slot = (m_map_size - slots) / 2;
        size_type remaining = count;
        for (size_type i = first_slot; i < first_slot + slots; i++)
        {
            m_map[i] = alloc_traits::allocate(m_allocator, m_buffer_size);
            for (size_type j = 0; j < std::min(m_buffer_size, remaining); j++)
                alloc_traits::construct(m_allocator, m_map[i] + j, value);
            remaining -= m_buffer_size;
        }

        m_begin.m_cur = m_map[first_slot];
        m_begin.m_first = m_map[first_slot];
        m_begin.m_last = m_map[first_slot] + m_buffer_size;
        m_begin.m_map_node = m_map + first_slot;

        size_type last_slot = first_slot + slots - 1;
        m_end.m_cur = m_map[last_slot] + (count % m_buffer_size == 0
                                              ? m_buffer_size
                                              : count % m_buffer_size);
        m_end.m_first = m_map[last_slot];
        m_end.m_last = m_map[last_slot] + m_buffer_size;
        m_end.m_map_node = m_map + last_slot;
    }

    template <typename T, typename Allocator>
    template <typename InputIt,
              std::enable_if_t<
                  !std::is_integral<InputIt>::value, int>>
    void deque<T, Allocator>::assign(InputIt first, InputIt last)
    {
        clear();

        if (first == last)
            return;

        size_type count = std::distance(first, last);
        size_type slots = (count + m_buffer_size - 1) / m_buffer_size;
        m_map_size = std::max(slots, m_map_size);
        m_map = map_alloc_traits::allocate(m_map_allocator, m_map_size);
        for (size_type i = 0; i < m_map_size; i++)
            m_map[i] = nullptr;

        size_type first_slot = (m_map_size - slots) / 2;
        size_type remaining = count;

        for (size_type i = first_slot; i < first_slot + slots; i++)
        {
            m_map[i] = alloc_traits::allocate(m_allocator, m_buffer_size);
            for (size_type j = 0;
                 j < std::min(m_buffer_size, remaining) && first != last; j++)
                alloc_traits::construct(m_allocator, m_map[i] + j, *first++);
            remaining -= m_buffer_size;
        }

        m_begin.m_cur = m_map[first_slot];
        m_begin.m_first = m_map[first_slot];
        m_begin.m_last = m_map[first_slot] + m_buffer_size;
        m_begin.m_map_node = m_map + first_slot;

        size_type last_slot = first_slot + slots - 1;
        m_end.m_cur = m_map[last_slot] + (count % m_buffer_size == 0
                                              ? m_buffer_size
                                              : count % m_buffer_size);
        m_end.m_first = m_map[last_slot];
        m_end.m_last = m_map[last_slot] + m_buffer_size;
        m_end.m_map_node = m_map + last_slot;
    }
    template <typename T, typename Allocator>
    void deque<T, Allocator>::assign(std::initializer_list<T> ilist)
    {
        assign(ilist.begin(), ilist.end());
    }

    template <typename T, typename Allocator>
    inline typename deque<T, Allocator>::allocator_type
    deque<T, Allocator>::get_allocator() const noexcept
    {
        return m_allocator;
    }

    template <typename T, typename Allocator>
    inline typename deque<T, Allocator>::reference
    deque<T, Allocator>::at(size_type pos)
    {
        size_type slot = pos / m_map_size;
        if (slot >= m_map_size)
            throw std::out_of_range("deque::at: pos out of range");

        size_type offset = pos % m_map_size;
        if (offset >= m_buffer_size)
            throw std::out_of_range("deque::at: pos out of range");

        return m_map[slot][offset];
    }

    template <typename T, typename Allocator>
    inline typename deque<T, Allocator>::const_reference
    deque<T, Allocator>::at(size_type pos) const
    {
        size_type slot = pos / m_map_size;
        if (slot >= m_map_size)
            throw std::out_of_range("deque::at: pos out of range");

        size_type offset = pos % m_map_size;
        if (offset >= m_buffer_size)
            throw std::out_of_range("deque::at: pos out of range");

        return m_map[slot][offset];
    }

    template <typename T, typename Allocator>
    inline typename deque<T, Allocator>::reference
    deque<T, Allocator>::operator[](size_type pos)
    {
        size_type slot = pos / m_map_size;
        size_type offset = pos % m_map_size;

        return m_map[slot][offset];
    }
    template <typename T, typename Allocator>
    inline typename deque<T, Allocator>::const_reference
    deque<T, Allocator>::operator[](size_type pos) const
    {
        size_type slot = pos / m_map_size;
        size_type offset = pos % m_map_size;

        return m_map[slot][offset];
    }
    template <typename T, typename Allocator>
    inline typename deque<T, Allocator>::reference
    deque<T, Allocator>::front()
    {
        if (empty())
            throw std::out_of_range("deque::front: deque is empty");
        return *m_begin.m_cur;
    }
    template <typename T, typename Allocator>
    inline typename deque<T, Allocator>::const_reference
    deque<T, Allocator>::front() const
    {
        if (empty())
            throw std::out_of_range("deque::front: deque is empty");
        return *m_begin.m_cur;
    }
    template <typename T, typename Allocator>
    inline typename deque<T, Allocator>::reference
    deque<T, Allocator>::back()
    {
        if (empty())
            throw std::out_of_range("deque::back: deque is empty");
        return *(m_end.m_cur - 1);
    }
    template <typename T, typename Allocator>
    inline typename deque<T, Allocator>::const_reference
    deque<T, Allocator>::back() const
    {
        if (empty())
            throw std::out_of_range("deque::back: deque is empty");
        return *(m_end.m_cur - 1);
    }

    template <typename T, typename Allocator>
    inline typename deque<T, Allocator>::iterator deque<T, Allocator>::begin()
    {
        return m_begin;
    }

    template <typename T, typename Allocator>
    inline typename deque<T, Allocator>::const_iterator deque<T, Allocator>::begin() const
    {
        return m_begin;
    }

    template <typename T, typename Allocator>
    inline typename deque<T, Allocator>::const_iterator deque<T, Allocator>::cbegin() const
    {
        return m_begin;
    }
    template <typename T, typename Allocator>
    inline typename deque<T, Allocator>::iterator deque<T, Allocator>::end()
    {
        return m_end;
    }

    template <typename T, typename Allocator>
    inline typename deque<T, Allocator>::const_iterator deque<T, Allocator>::end() const
    {
        return m_end;
    }

    template <typename T, typename Allocator>
    inline typename deque<T, Allocator>::const_iterator deque<T, Allocator>::cend() const
    {
        return m_end;
    }
    template <typename T, typename Allocator>
    inline typename deque<T, Allocator>::reverse_iterator deque<T, Allocator>::rbegin()
    {
        return reverse_iterator(m_end);
    }

    template <typename T, typename Allocator>
    inline typename deque<T, Allocator>::const_reverse_iterator deque<T, Allocator>::rbegin() const
    {
        return const_reverse_iterator(m_end);
    }

    template <typename T, typename Allocator>
    inline typename deque<T, Allocator>::const_reverse_iterator deque<T, Allocator>::crbegin() const
    {
        return const_reverse_iterator(m_end);
    }

    template <typename T, typename Allocator>
    inline typename deque<T, Allocator>::reverse_iterator deque<T, Allocator>::rend()
    {
        return reverse_iterator(m_begin);
    }

    template <typename T, typename Allocator>
    inline typename deque<T, Allocator>::const_reverse_iterator deque<T, Allocator>::rend() const
    {
        return const_reverse_iterator(m_begin);
    }

    template <typename T, typename Allocator>
    inline typename deque<T, Allocator>::const_reverse_iterator deque<T, Allocator>::crend() const
    {
        return const_reverse_iterator(m_begin);
    }

    template <typename T, typename Allocator>
    inline bool deque<T, Allocator>::empty() const
    {
        return m_begin.m_cur == nullptr;
    }
    template <typename T, typename Allocator>
    inline typename deque<T, Allocator>::size_type deque<T, Allocator>::size() const
    {
        return static_cast<size_type>(m_end.m_cur - m_begin.m_cur);
    }

    template <typename T, typename Allocator>
    inline typename deque<T, Allocator>::size_type deque<T, Allocator>::max_size() const
    {
        return alloc_traits::max_size(m_allocator);
    }

    template <typename T, typename Allocator>
    inline void deque<T, Allocator>::resize(size_type count)
    {
        size_type current_size = size();
        if (count == current_size)
            return;
        else if (count < current_size)
        {
            while (current_size > count)
            {
                current_size--;
                pop_back();
            }
        }
        else
        {
            while (current_size < count)
            {
                emplace_back();
                current_size++;
            }
        }
    }

    template <typename T, typename Allocator>
    inline void deque<T, Allocator>::resize(size_type count, const value_type &value)
    {
        size_type current_size = size();
        if (count == current_size)
            return;
        else if (count < current_size)
        {
            while (current_size > count)
            {
                current_size--;
                pop_back();
            }
        }
        else
        {
            while (current_size < count)
            {
                emplace_back(value);
                current_size++;
            }
        }
    }

    template <typename T, typename Allocator>
    inline void deque<T, Allocator>::shrink_to_fit()
    {
        size_type used_slots = m_end.m_map_node - m_begin.m_map_node + 1;
        if (used_slots == m_map_size || used_slots <= DEQUE_DEFAULT_INIT_SIZE)
            return;

        pointer *new_map = map_alloc_traits::allocate(m_map_allocator, used_slots);
        for (size_type i = 0; i < used_slots; i++)
            new_map[i] = nullptr;

        size_type old_start_idx = m_begin.m_map_node - m_map;

        for (size_type i = 0; i < used_slots; ++i)
        {
            new_map[i] = m_map[old_start_idx + i];
        }

        for (size_type i = 0; i < m_map_size; ++i)
        {
            // 检查该槽位是否在使用中
            bool in_use = (i >= old_start_idx) && (i < old_start_idx + used_slots);
            if (!in_use && m_map[i] != nullptr)
                alloc_traits::deallocate(m_allocator, m_map[i], m_buffer_size);
        }

        map_alloc_traits::deallocate(m_map_allocator, m_map, m_map_size);

        m_map = new_map;
        m_map_size = used_slots;
        m_begin.m_map_node = m_map;
        m_end.m_map_node = m_map + used_slots - 1;
    }

    template <typename T, typename Allocator>
    inline void deque<T, Allocator>::clear() noexcept
    {
        if (empty())
            return;

        for (iterator it = m_begin; it != m_end; it++)
            alloc_traits::destroy(m_allocator, it.m_cur);

        size_type middle_slot = m_map_size / 2;
        if (m_map[middle_slot] == nullptr)
            m_map[middle_slot] = alloc_traits::allocate(m_allocator, m_buffer_size);

        m_begin.m_cur = m_map[middle_slot];
        m_begin.m_first = m_map[middle_slot];
        m_begin.m_last = m_map[middle_slot] + m_buffer_size;
        m_begin.m_map_node = m_map + middle_slot;
        m_end = m_begin;
    }

    template <typename T, typename Allocator>
    inline typename deque<T, Allocator>::iterator
    deque<T, Allocator>::insert(const_iterator pos, const value_type &value)
    {
        // 如果是空容器，直接在头部插入
        if (empty())
        {
            push_front(value);
            return begin();
        }

        // 计算插入位置距离两端的距离
        difference_type distance_from_begin = pos - cbegin();
        difference_type distance_from_end = size() - distance_from_begin;

        // 选择移动代价较小的方向
        if (distance_from_begin <= distance_from_end)
        {
            // 从头部插入更优：将头部元素向后移动
            push_front(value);

            // 将插入位置之前的元素向后移动
            iterator first = begin();
            iterator last = begin() + distance_from_begin;

            while (first != last)
            {
                std::swap(*first, *(first + 1));
                ++first;
            }
        }
        else
        {
            // 从尾部插入更优：将尾部元素向前移动
            push_back(value);

            // 将插入位置之后的元素向前移动
            iterator last = end();
            iterator first = end() - distance_from_end;

            while (last != first)
            {
                --last;
                std::swap(*last, *(last - 1));
            }
        }

        // 返回指向插入元素的迭代器
        return begin() + distance_from_begin;
    }

    template <typename T, typename Allocator>
    inline typename deque<T, Allocator>::iterator
    deque<T, Allocator>::insert(const_iterator pos, const value_type &&value)
    {
        // 如果是空容器，直接在头部插入
        if (empty())
        {
            push_front(value);
            return begin();
        }

        // 计算插入位置距离两端的距离
        difference_type distance_from_begin = pos - cbegin();
        difference_type distance_from_end = size() - distance_from_begin;

        // 选择移动代价较小的方向
        if (distance_from_begin <= distance_from_end)
        {
            // 从头部插入更优：将头部元素向后移动
            push_front(value);

            // 将插入位置之前的元素向后移动
            iterator first = begin();
            iterator last = begin() + distance_from_begin;

            while (first != last)
            {
                std::swap(*first, *(first + 1));
                ++first;
            }
        }
        else
        {
            // 从尾部插入更优：将尾部元素向前移动
            push_back(std::move(value));

            // 将插入位置之后的元素向前移动
            iterator last = end();
            iterator first = end() - distance_from_end;

            while (last != first)
            {
                --last;
                std::swap(*last, *(last - 1));
            }
        }

        // 返回指向插入元素的迭代器
        return begin() + distance_from_begin;
    }

    template <typename T, typename Allocator>
    inline typename deque<T, Allocator>::iterator
    deque<T, Allocator>::insert(const_iterator pos, size_type count, const value_type &value)
    {
        // 处理 count = 0 的情况
        if (count == 0)
        {
            return iterator(const_cast<pointer>(pos.m_cur),
                            const_cast<pointer>(pos.m_first),
                            const_cast<pointer>(pos.m_last),
                            const_cast<value_type **>(pos.m_map_node));
        }

        // 如果是空容器，直接填充
        if (empty())
        {
            resize(count, value);
            return begin();
        }

        // 计算插入位置
        difference_type insert_pos = pos - cbegin();
        difference_type old_size = size();

        // 计算需要的新槽位数
        size_type new_slots = count / m_buffer_size;
        if (count % m_buffer_size != 0)
            new_slots++;

        // 计算当前使用的槽位数
        size_type used_slots = m_end.m_map_node - m_begin.m_map_node + 1;

        // 检查是否需要扩容 map
        size_type required_map_size = used_slots + new_slots;
        if (required_map_size > m_map_size)
        {
            // 扩展 map 大小（至少翻倍）
            size_type new_map_size = std::max(m_map_size * 2, required_map_size);
            value_type **new_map = map_alloc_traits::allocate(m_map_allocator, new_map_size);

            // 初始化新 map
            for (size_type i = 0; i < new_map_size; ++i)
                new_map[i] = nullptr;

            // 计算新的起始位置（保持居中）
            size_type old_start_idx = m_begin.m_map_node - m_map;
            size_type new_start_idx = (new_map_size - required_map_size) / 2;

            // 拷贝旧槽位到新位置
            for (size_type i = 0; i < used_slots; ++i)
                new_map[new_start_idx + i] = m_map[old_start_idx + i];

            // 释放旧 map
            map_alloc_traits::deallocate(m_map_allocator, m_map, m_map_size);

            // 更新指针
            m_map = new_map;
            m_map_size = new_map_size;
            m_begin.m_map_node = &m_map[new_start_idx];
            m_end.m_map_node = &m_map[new_start_idx + used_slots - 1];
        }

        // 计算插入位置距离两端的距离
        difference_type distance_from_begin = insert_pos;
        difference_type distance_from_end = old_size - insert_pos;

        // 选择移动代价较小的方向
        if (distance_from_begin <= distance_from_end)
        {
            // 从头部扩展
            // 在头部添加新槽位
            for (size_type i = 0; i < new_slots; ++i)
            {
                m_begin.m_map_node--;
                *m_begin.m_map_node = alloc_traits::allocate(m_allocator, m_buffer_size);
            }

            // 移动头部元素
            iterator src = begin();
            iterator dest = begin() + count;
            for (difference_type i = distance_from_begin - 1; i >= 0; --i)
            {
                *dest = *src;
                --dest;
                --src;
            }

            // 填充新元素
            iterator it = begin();
            for (size_type i = 0; i < count; ++i)
            {
                alloc_traits::construct(m_allocator, it.m_cur, value);
                ++it;
            }
        }
        else
        {
            // 从尾部扩展
            // 在尾部添加新槽位
            iterator old_end = end();
            for (size_type i = 0; i < new_slots; ++i)
            {
                m_end.m_map_node++;
                *m_end.m_map_node = alloc_traits::allocate(m_allocator, m_buffer_size);
            }

            // 移动尾部元素
            iterator src = old_end - 1;
            iterator dest = end() - 1;
            for (difference_type i = distance_from_end - 1; i >= 0; --i)
            {
                *dest = *src;
                --dest;
                --src;
            }

            // 填充新元素
            iterator it = begin() + insert_pos;
            for (size_type i = 0; i < count; ++i)
            {
                alloc_traits::construct(m_allocator, it.m_cur, value);
                ++it;
            }
        }

        // 更新 m_end
        m_end.m_cur += count;
        if (m_end.m_cur >= m_end.m_last)
        {
            m_end.m_map_node++;
            m_end.m_first = *m_end.m_map_node;
            m_end.m_last = m_end.m_first + m_buffer_size;
            m_end.m_cur = m_end.m_first + (m_end.m_cur - (m_end.m_last - m_buffer_size));
        }

        // 返回指向第一个插入元素的迭代器
        return begin() + insert_pos;
    }

    template <typename T, typename Allocator>
    template <typename InputIt,
              std::enable_if_t<
                  !std::is_integral<InputIt>::value, int>>
    inline typename deque<T, Allocator>::iterator
    deque<T, Allocator>::insert(const_iterator pos, InputIt first, InputIt last)
    {
        if (first == last)
        {
            return iterator(const_cast<pointer>(pos.m_cur),
                            const_cast<pointer>(pos.m_first),
                            const_cast<pointer>(pos.m_last),
                            const_cast<value_type **>(pos.m_map_node));
        }

        size_type count = 0;
        for (InputIt it = first; it != last; ++it)
            ++count;

        if (empty())
        {
            resize(count);
            iterator it = begin();
            for (InputIt i = first; i != last; ++i, ++it)
                alloc_traits::construct(m_allocator, it.m_cur, *i);
            return begin();
        }

        difference_type insert_pos = pos - cbegin();
        difference_type old_size = size();

        size_type new_slots = count / m_buffer_size;
        if (count % m_buffer_size != 0)
            new_slots++;

        size_type used_slots = m_end.m_map_node - m_begin.m_map_node + 1;

        size_type required_map_size = used_slots + new_slots;
        if (required_map_size > m_map_size)
        {
            size_type new_map_size = std::max(m_map_size * 2, required_map_size);
            value_type **new_map = map_alloc_traits::allocate(m_map_allocator, new_map_size);

            for (size_type i = 0; i < new_map_size; ++i)
                new_map[i] = nullptr;

            size_type old_start_idx = m_begin.m_map_node - m_map;
            size_type new_start_idx = (new_map_size - required_map_size) / 2;

            for (size_type i = 0; i < used_slots; ++i)
                new_map[new_start_idx + i] = m_map[old_start_idx + i];

            map_alloc_traits::deallocate(m_map_allocator, m_map, m_map_size);

            m_map = new_map;
            m_map_size = new_map_size;
            m_begin.m_map_node = &m_map[new_start_idx];
            m_end.m_map_node = &m_map[new_start_idx + used_slots - 1];
        }

        difference_type distance_from_begin = insert_pos;
        difference_type distance_from_end = old_size - insert_pos;

        if (distance_from_begin <= distance_from_end)
        {
            for (size_type i = 0; i < new_slots; ++i)
            {
                m_begin.m_map_node--;
                *m_begin.m_map_node = alloc_traits::allocate(m_allocator, m_buffer_size);
            }

            iterator src = begin();
            iterator dest = begin() + count;
            for (difference_type i = distance_from_begin - 1; i >= 0; --i)
            {
                *dest = *src;
                --dest;
                --src;
            }

            iterator it = begin();
            for (InputIt i = first; i != last; ++i, ++it)
            {
                alloc_traits::construct(m_allocator, it.m_cur, *i);
            }
        }
        else
        {
            iterator old_end = end();
            for (size_type i = 0; i < new_slots; ++i)
            {
                m_end.m_map_node++;
                *m_end.m_map_node = alloc_traits::allocate(m_allocator, m_buffer_size);
            }

            iterator src = old_end - 1;
            iterator dest = end() - 1;
            for (difference_type i = distance_from_end - 1; i >= 0; --i)
            {
                *dest = *src;
                --dest;
                --src;
            }

            iterator it = begin() + insert_pos;
            for (InputIt i = first; i != last; ++i, ++it)
            {
                alloc_traits::construct(m_allocator, it.m_cur, *i);
            }
        }

        m_end.m_cur += count;
        if (m_end.m_cur >= m_end.m_last)
        {
            m_end.m_map_node++;
            m_end.m_first = *m_end.m_map_node;
            m_end.m_last = m_end.m_first + m_buffer_size;
            m_end.m_cur = m_end.m_first + (m_end.m_cur - (m_end.m_last - m_buffer_size));
        }

        return begin() + insert_pos;
    }

    template <typename T, typename Allocator>
    inline typename deque<T, Allocator>::iterator
    deque<T, Allocator>::insert(const_iterator pos, std::initializer_list<T> ilist)
    {
        return insert(pos, ilist.begin(), ilist.end());
    }

    template <typename T, typename Allocator>
    template <typename... Args>
    inline typename deque<T, Allocator>::iterator
    deque<T, Allocator>::emplace(const_iterator pos, Args &&...args)
    {
        if (empty())
        {
            emplace_front(std::forward<Args>(args)...);
            return begin();
        }

        difference_type distance_from_begin = pos - cbegin();
        difference_type distance_from_end = size() - distance_from_begin;

        if (distance_from_begin <= distance_from_end)
        {
            emplace_front(std::forward<Args>(args)...);

            iterator first = begin();
            iterator last = begin() + distance_from_begin;

            while (first != last)
            {
                std::swap(*first, *(first + 1));
                ++first;
            }
        }
        else
        {
            emplace_back(std::forward<Args>(args)...);

            iterator last = end();
            iterator first = end() - distance_from_end;

            while (last != first)
            {
                --last;
                std::swap(*last, *(last - 1));
            }
        }

        return begin() + distance_from_begin;
    }

    template <typename T, typename Allocator>
    inline typename deque<T, Allocator>::iterator
    deque<T, Allocator>::erase(const_iterator pos)
    {
        if (empty())
            return end();

        // 获取删除位置
        difference_type erase_pos = pos - cbegin();

        // 如果删除的是最后一个元素
        if (pos == cend() - 1)
        {
            pop_back();
            return end();
        }

        // 如果删除的是第一个元素
        if (pos == cbegin())
        {
            pop_front();
            return begin();
        }

        // 计算距离两端的距离
        difference_type distance_from_begin = erase_pos;
        difference_type distance_from_end = size() - erase_pos - 1;

        iterator result(begin() + erase_pos);

        if (distance_from_begin <= distance_from_end)
        {
            // 向前移动元素
            iterator it = begin() + erase_pos;
            iterator prev_it = it - 1;
            while (it != begin())
            {
                *it = std::move(*prev_it);
                --it;
                --prev_it;
            }
            pop_front();
        }
        else
        {
            // 向后移动元素
            iterator it = begin() + erase_pos;
            iterator next_it = it + 1;
            while (next_it != end())
            {
                *it = std::move(*next_it);
                ++it;
                ++next_it;
            }
            pop_back();
        }

        return result;
    }

    template <typename T, typename Allocator>
    inline typename deque<T, Allocator>::iterator
    deque<T, Allocator>::erase(const_iterator first, const_iterator last)
    {
        if (first == last)
        {
            return iterator(const_cast<pointer>(first.m_cur),
                            const_cast<pointer>(first.m_first),
                            const_cast<pointer>(first.m_last),
                            const_cast<value_type **>(first.m_map_node));
        }

        // 如果删除全部元素
        if (first == cbegin() && last == cend())
        {
            clear();
            return end();
        }

        // 计算删除范围
        difference_type erase_begin = first - cbegin();
        difference_type erase_size = last - first;
        difference_type erase_end = erase_begin + erase_size;

        // 计算距离两端的距离
        difference_type distance_from_begin = erase_begin;
        difference_type distance_from_end = size() - erase_end;

        iterator result(begin() + erase_begin);

        if (distance_from_begin <= distance_from_end)
        {
            // 将前面的元素向后移动覆盖删除区域
            iterator dest = begin() + erase_end;
            iterator src = begin() + erase_begin;

            for (difference_type i = 0; i < distance_from_begin; ++i)
            {
                --dest;
                --src;
                *dest = std::move(*src);
            }

            // 从头部删除
            for (difference_type i = 0; i < erase_size; ++i)
                pop_front();
        }
        else
        {
            // 将后面的元素向前移动覆盖删除区域
            iterator dest = begin() + erase_begin;
            iterator src = begin() + erase_end;

            for (difference_type i = 0; i < distance_from_end; ++i)
            {
                *dest = std::move(*src);
                ++dest;
                ++src;
            }

            // 从尾部删除
            for (difference_type i = 0; i < erase_size; ++i)
                pop_back();
        }

        return result;
    }

    template <typename T, typename Allocator>
    inline void deque<T, Allocator>::push_front(const value_type &value)
    {
        if (m_begin.m_cur > m_begin.m_first)
        {
            alloc_traits::construct(m_allocator, m_begin.m_cur - 1, value);
            m_begin.m_cur--;
        }
        else
        {
            value_type **new_slot = m_begin.m_map_node - 1;

            if (new_slot < m_map)
            {
                size_type new_map_size = m_map_size * 2;
                value_type **new_map = map_alloc_traits::allocate(m_map_allocator, new_map_size);
                size_type first_slot = (new_map_size - m_map_size) / 2;
                for (size_type i = 0; i < m_map_size; i++)
                    new_map[first_slot + i] = m_map[i];

                map_alloc_traits::deallocate(m_map_allocator, m_map, m_map_size);

                size_type old_begin_offset = m_begin.m_map_node - m_map;
                size_type old_end_offset = m_end.m_map_node - m_map;

                m_map = new_map;
                m_map_size = new_map_size;
                m_begin.m_map_node = m_map + first_slot + old_begin_offset;
                m_end.m_map_node = m_map + first_slot + old_end_offset;
                new_slot = m_begin.m_map_node - 1;
            }

            if (*new_slot == nullptr)
                *new_slot = alloc_traits::allocate(m_allocator, m_buffer_size);

            alloc_traits::construct(m_allocator, *new_slot + m_buffer_size - 1, value);
            m_begin.m_cur = *new_slot + m_buffer_size - 1;
            m_begin.m_first = *new_slot;
            m_begin.m_last = *new_slot + m_buffer_size;
            m_begin.m_map_node = new_slot;
        }
    }

    template <typename T, typename Allocator>
    inline void deque<T, Allocator>::push_front(const value_type &&value)
    {
        if (m_begin.m_cur > m_begin.m_first)
        {
            alloc_traits::construct(m_allocator, m_begin.m_cur - 1, std::move(value));
            m_begin.m_cur--;
        }
        else
        {
            value_type **new_slot = m_begin.m_map_node - 1;

            if (new_slot < m_map)
            {
                size_type new_map_size = m_map_size * 2;
                value_type **new_map = map_alloc_traits::allocate(m_map_allocator, new_map_size);
                size_type first_slot = (new_map_size - m_map_size) / 2;
                for (size_type i = 0; i < m_map_size; i++)
                    new_map[first_slot + i] = m_map[i];

                map_alloc_traits::deallocate(m_map_allocator, m_map, m_map_size);

                size_type old_begin_offset = m_begin.m_map_node - m_map;
                size_type old_end_offset = m_end.m_map_node - m_map;

                m_map = new_map;
                m_map_size = new_map_size;
                m_begin.m_map_node = m_map + first_slot + old_begin_offset;
                m_end.m_map_node = m_map + first_slot + old_end_offset;
                new_slot = m_begin.m_map_node - 1;
            }

            if (*new_slot == nullptr)
                *new_slot = alloc_traits::allocate(m_allocator, m_buffer_size);

            alloc_traits::construct(m_allocator, *new_slot + m_buffer_size - 1, std::move(value));
            m_begin.m_cur = *new_slot + m_buffer_size - 1;
            m_begin.m_first = *new_slot;
            m_begin.m_last = *new_slot + m_buffer_size;
            m_begin.m_map_node = new_slot;
        }
    }

    template <typename T, typename Allocator>
    inline void deque<T, Allocator>::push_back(const value_type &value)
    {
        if (m_end.m_cur < m_end.m_last)
        {
            alloc_traits::construct(m_allocator, m_end.m_cur, value);
            m_end.m_cur++;
        }
        else
        {
            value_type **new_slot = m_end.m_map_node + 1;

            if (new_slot >= m_map + m_map_size)
            {
                size_type new_map_size = m_map_size * 2;
                value_type **new_map = map_alloc_traits::allocate(m_map_allocator, new_map_size);
                size_type first_slot = (new_map_size - m_map_size) / 2;
                for (size_type i = 0; i < m_map_size; i++)
                    new_map[first_slot + i] = m_map[i];

                map_alloc_traits::deallocate(m_map_allocator, m_map, m_map_size);

                size_type old_begin_offset = m_begin.m_map_node - m_map;
                size_type old_end_offset = m_end.m_map_node - m_map;

                m_map = new_map;
                m_map_size = new_map_size;
                m_begin.m_map_node = m_map + first_slot + old_begin_offset;
                m_end.m_map_node = m_map + first_slot + old_end_offset;
                new_slot = m_end.m_map_node + 1;
            }

            if (*new_slot == nullptr)
                *new_slot = alloc_traits::allocate(m_allocator, m_buffer_size);

            alloc_traits::construct(m_allocator, *new_slot, value);
            m_end.m_cur = *new_slot + 1;
            m_end.m_first = *new_slot;
            m_end.m_last = *new_slot + m_buffer_size;
            m_end.m_map_node = new_slot;
        }
    }
    template <typename T, typename Allocator>
    inline void deque<T, Allocator>::push_back(const value_type &&value)
    {
        if (m_end.m_cur < m_end.m_last)
        {
            alloc_traits::construct(m_allocator, m_end.m_cur, std::move(value));
            m_end.m_cur++;
        }
        else
        {
            value_type **new_slot = m_end.m_map_node + 1;

            if (new_slot >= m_map + m_map_size)
            {
                size_type new_map_size = m_map_size * 2;
                value_type **new_map = map_alloc_traits::allocate(m_map_allocator, new_map_size);
                size_type first_slot = (new_map_size - m_map_size) / 2;
                for (size_type i = 0; i < m_map_size; i++)
                    new_map[first_slot + i] = m_map[i];

                map_alloc_traits::deallocate(m_map_allocator, m_map, m_map_size);

                size_type old_begin_offset = m_begin.m_map_node - m_map;
                size_type old_end_offset = m_end.m_map_node - m_map;

                m_map = new_map;
                m_map_size = new_map_size;
                m_begin.m_map_node = m_map + first_slot + old_begin_offset;
                m_end.m_map_node = m_map + first_slot + old_end_offset;
                new_slot = m_end.m_map_node + 1;
            }

            if (*new_slot == nullptr)
                *new_slot = alloc_traits::allocate(m_allocator, m_buffer_size);

            alloc_traits::construct(m_allocator, *new_slot, std::move(value));
            m_end.m_cur = *new_slot + 1;
            m_end.m_first = *new_slot;
            m_end.m_last = *new_slot + m_buffer_size;
            m_end.m_map_node = new_slot;
        }
    }
    template <typename T, typename Allocator>
    template <typename... Args>
    inline typename deque<T, Allocator>::reference
    deque<T, Allocator>::emplace_back(Args &&...args)
    {
        if (m_end.m_cur < m_end.m_last)
        {
            alloc_traits::construct(m_allocator, m_end.m_cur, std::forward<Args>(args)...);
            reference result = *m_end.m_cur;
            m_end.m_cur++;
            return result;
        }
        else
        {
            value_type **new_slot = m_end.m_map_node + 1;

            if (new_slot >= m_map + m_map_size)
            {
                size_type new_map_size = m_map_size * 2;
                value_type **new_map = map_alloc_traits::allocate(m_map_allocator, new_map_size);
                size_type first_slot = (new_map_size - m_map_size) / 2;
                for (size_type i = 0; i < m_map_size; i++)
                    new_map[first_slot + i] = m_map[i];

                map_alloc_traits::deallocate(m_map_allocator, m_map, m_map_size);

                size_type old_begin_offset = m_begin.m_map_node - m_map;
                size_type old_end_offset = m_end.m_map_node - m_map;

                m_map = new_map;
                m_map_size = new_map_size;
                m_begin.m_map_node = m_map + first_slot + old_begin_offset;
                m_end.m_map_node = m_map + first_slot + old_end_offset;
                new_slot = m_end.m_map_node + 1;
            }

            if (*new_slot == nullptr)
                *new_slot = alloc_traits::allocate(m_allocator, m_buffer_size);

            alloc_traits::construct(m_allocator, *new_slot, std::forward<Args>(args)...);
            reference result = **new_slot;
            m_end.m_cur = *new_slot + 1;
            m_end.m_first = *new_slot;
            m_end.m_last = *new_slot + m_buffer_size;
            m_end.m_map_node = new_slot;
            return result;
        }
    }
    template <typename T, typename Allocator>
    template <typename... Args>
    inline typename deque<T, Allocator>::reference
    deque<T, Allocator>::emplace_front(Args &&...args)
    {
        if (m_begin.m_cur > m_begin.m_first)
        {
            alloc_traits::construct(m_allocator, m_begin.m_cur - 1, std::forward<Args>(args)...);
            reference result = *(m_begin.m_cur - 1);
            m_begin.m_cur--;
            return result;
        }
        else
        {
            value_type **new_slot = m_begin.m_map_node - 1;

            if (new_slot < m_map)
            {
                size_type new_map_size = m_map_size * 2;
                value_type **new_map = map_alloc_traits::allocate(m_map_allocator, new_map_size);
                size_type first_slot = (new_map_size - m_map_size) / 2;
                for (size_type i = 0; i < m_map_size; i++)
                    new_map[first_slot + i] = m_map[i];

                map_alloc_traits::deallocate(m_map_allocator, m_map, m_map_size);

                size_type old_begin_offset = m_begin.m_map_node - m_map;
                size_type old_end_offset = m_end.m_map_node - m_map;

                m_map = new_map;
                m_map_size = new_map_size;
                m_begin.m_map_node = m_map + first_slot + old_begin_offset;
                m_end.m_map_node = m_map + first_slot + old_end_offset;
                new_slot = m_begin.m_map_node - 1;
            }

            if (*new_slot == nullptr)
                *new_slot = alloc_traits::allocate(m_allocator, m_buffer_size);

            alloc_traits::construct(m_allocator, *new_slot + m_buffer_size - 1, std::forward<Args>(args)...);
            reference result = *(*new_slot + m_buffer_size - 1);
            m_begin.m_cur = *new_slot + m_buffer_size - 1;
            m_begin.m_first = *new_slot;
            m_begin.m_last = *new_slot + m_buffer_size;
            m_begin.m_map_node = new_slot;
            return result;
        }
    }

    template <typename T, typename Allocator>
    inline void deque<T, Allocator>::pop_front()
    {
        if (empty())
            throw std::out_of_range("deque::pop_front: deque is empty");

        alloc_traits::destroy(m_allocator, m_begin.m_cur);
        m_begin.m_cur++;
    }

    template <typename T, typename Allocator>
    inline void deque<T, Allocator>::pop_back()
    {
        if (empty())
            throw std::out_of_range("deque::pop_back: deque is empty");

        alloc_traits::destroy(m_allocator, --m_end.m_cur);
    }

    template <typename T, typename Allocator>
    inline void deque<T, Allocator>::swap(deque &other) noexcept
    {
        using std::swap;
        swap(m_begin, other.m_begin);
        swap(m_end, other.m_end);
        swap(m_map, other.m_map);
        swap(m_map_size, other.m_map_size);
    }

    template <typename T, typename Allocator>
    bool operator==(const deque<T, Allocator> &lhs, const deque<T, Allocator> &rhs)
    {
        if (lhs.size() != rhs.size())
            return false;

        typename deque<T, Allocator>::const_iterator lhs_it = lhs.cbegin();
        typename deque<T, Allocator>::const_iterator rhs_it = rhs.cbegin();
        while (lhs_it != lhs.cend() && rhs_it != rhs.cend())
        {
            if (*lhs_it != *rhs_it)
                return false;
            ++lhs_it;
            ++rhs_it;
        }
        return true;
    }

    template <typename T, typename Allocator>
    bool operator!=(const deque<T, Allocator> &lhs, const deque<T, Allocator> &rhs)
    {
        return !(lhs == rhs);
    }
}
