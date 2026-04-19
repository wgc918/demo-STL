//-----------------------------------------------------------------------------
// 版权所有 (C) 2026 demo-STL 项目
//
// 文件: vector.h
// 作者: wgc
// 创建日期: 2026年1月
// 最后修改: 2026年4月
//
// 描述:
//     本文件实现了一个 STL 风格的动态数组容器 (vector)。
//     该容器支持随机访问、动态扩容、高效的尾部插入/删除操作。
//
// 功能特性:
//     - 随机访问迭代器支持
//     - 动态内存分配与自动扩容
//     - 高效的尾部操作（push_back/pop_back）
//     - 支持移动语义
//     - 符合 STL 容器规范
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

#include <utility>
#include <iterator>
#include <memory>
#include <type_traits>
#include <exception>
#include <initializer_list>

namespace demo
{
    template <typename T, typename Allocator = std::allocator<T>>
    class vector;

    /// @brief 比较两个vector是否相等
    /// @tparam T 元素类型
    /// @tparam Allocator 分配器类型
    /// @param lhs 左操作数
    /// @param rhs 右操作数
    /// @return 如果两个vector相等返回true，否则返回false
    template <typename T, typename Allocator>
    bool operator==(const vector<T, Allocator> &lhs,
                    const vector<T, Allocator> &rhs);

    /// @brief 比较两个vector是否不相等
    /// @tparam T 元素类型
    /// @tparam Allocator 分配器类型
    /// @param lhs 左操作数
    /// @param rhs 右操作数
    /// @return 如果两个vector不相等返回true，否则返回false
    template <typename T, typename Allocator>
    bool operator!=(const vector<T, Allocator> &lhs,
                    const vector<T, Allocator> &rhs);

    /// @brief 动态数组容器类
    /// @tparam T 元素类型
    /// @tparam Allocator 分配器类型，默认为 std::allocator<T>
    /// @note vector是一个连续存储的动态数组，支持随机访问，尾部插入/删除效率高
    template <typename T, typename Allocator>
    class vector
    {
    public:
        using value_type = T;                       ///< 元素类型
        using pointer = T *;                        ///< 元素指针类型
        using const_pointer = const T *;            ///< 常量元素指针类型
        using reference = value_type &;             ///< 元素引用类型
        using const_reference = const value_type &; ///< 常量元素引用类型
        using allocator_type = Allocator;           ///< 分配器类型
        using size_type = std::size_t;              ///< 大小类型
        using difference_type = std::ptrdiff_t;     ///< 差值类型

        class const_iterator;

        /// @brief 非 const 迭代器类，支持随机访问
        /// @note 迭代器失效：当vector发生扩容、插入或删除操作时，可能导致迭代器失效
        class iterator
        {
            friend class const_iterator;

        public:
            using iterator_category =
                std::random_access_iterator_tag;                      ///< 迭代器类别（随机访问迭代器）
            using value_type = typename vector::value_type;           ///< 元素类型
            using difference_type = typename vector::difference_type; ///< 差值类型
            using pointer = typename vector::pointer;                 ///< 指针类型
            using reference = typename vector::reference;             ///< 引用类型

            /// @brief 默认构造函数，创建空迭代器
            iterator() noexcept;

            /// @brief 构造函数，从指针创建迭代器
            /// @param ptr 指向元素的指针
            explicit iterator(pointer ptr) noexcept;

            /// @brief 解引用操作符，返回当前元素的引用
            /// @return 当前元素的引用
            reference operator*() const noexcept;

            /// @brief 箭头操作符，返回当前元素的指针
            /// @return 当前元素的指针
            pointer operator->() const noexcept;

            /// @brief 下标操作符，返回偏移位置元素的引用
            /// @param offset 相对于当前位置的偏移量
            /// @return 偏移位置元素的引用
            reference operator[](difference_type offset) const;

            /// @brief 前置自增，移动到下一个元素
            /// @return 自增后的迭代器引用
            iterator &operator++() noexcept;

            /// @brief 后置自增，移动到下一个元素
            /// @return 自增前的迭代器副本
            iterator operator++(int) noexcept;

            /// @brief 前置自减，移动到前一个元素
            /// @return 自减后的迭代器引用
            iterator &operator--() noexcept;

            /// @brief 后置自减，移动到前一个元素
            /// @return 自减前的迭代器副本
            iterator operator--(int) noexcept;

            /// @brief 加法操作，移动指定偏移量
            /// @param offset 要移动的偏移量
            /// @return 移动后的迭代器
            iterator operator+(difference_type offset) const noexcept;

            /// @brief 减法操作，移动指定偏移量
            /// @param offset 要移动的偏移量
            /// @return 移动后的迭代器
            iterator operator-(difference_type offset) const noexcept;

            /// @brief 复合赋值加法，移动指定偏移量
            /// @param offset 要移动的偏移量
            /// @return 移动后的迭代器引用
            iterator &operator+=(difference_type offset) noexcept;

            /// @brief 复合赋值减法，移动指定偏移量
            /// @param offset 要移动的偏移量
            /// @return 移动后的迭代器引用
            iterator &operator-=(difference_type offset) noexcept;

            /// @brief 计算两个迭代器之间的差值
            /// @param other 另一个迭代器
            /// @return 两个迭代器之间的元素个数
            difference_type operator-(
                const iterator &other) const noexcept;

            /// @brief 比较两个迭代器是否相等
            /// @param other 要比较的另一个迭代器
            /// @return 如果相等返回true，否则返回false
            bool operator==(const iterator &other) const noexcept;

            /// @brief 比较两个迭代器是否不相等
            /// @param other 要比较的另一个迭代器
            /// @return 如果不相等返回true，否则返回false
            bool operator!=(const iterator &other) const noexcept;

            /// @brief 比较迭代器是否大于另一个迭代器
            /// @param other 要比较的另一个迭代器
            /// @return 如果大于返回true，否则返回false
            bool operator>(const iterator &other) const noexcept;

            /// @brief 比较迭代器是否小于另一个迭代器
            /// @param other 要比较的另一个迭代器
            /// @return 如果小于返回true，否则返回false
            bool operator<(const iterator &other) const noexcept;

            /// @brief 比较迭代器是否大于等于另一个迭代器
            /// @param other 要比较的另一个迭代器
            /// @return 如果大于等于返回true，否则返回false
            bool operator>=(const iterator &other) const noexcept;

            /// @brief 比较迭代器是否小于等于另一个迭代器
            /// @param other 要比较的另一个迭代器
            /// @return 如果小于等于返回true，否则返回false
            bool operator<=(const iterator &other) const noexcept;

            /// @brief 获取底层指针（供reverse_iterator使用）
            /// @return 底层指针
            pointer base() const noexcept;

        private:
            pointer m_ptr; ///< 指向当前元素的指针
        };

        /// @brief const 迭代器类，支持随机访问，不能修改元素
        /// @note 迭代器失效：当vector发生扩容、插入或删除操作时，可能导致迭代器失效
        class const_iterator
        {
            friend class iterator;

        public:
            using iterator_category = std::random_access_iterator_tag; ///< 迭代器类别（随机访问迭代器）
            using value_type = const typename vector::value_type;      ///< 常量元素类型
            using reference = const typename vector::value_type &;     ///< 常量引用类型
            using pointer = const typename vector::value_type *;       ///< 常量指针类型
            using difference_type = typename vector::difference_type;  ///< 差值类型

            /// @brief 默认构造函数，创建空迭代器
            const_iterator() noexcept;

            /// @brief 构造函数，从指针创建迭代器
            /// @param ptr 指向元素的指针
            explicit const_iterator(typename vector::pointer ptr) noexcept;

            /// @brief 从非 const 迭代器构造
            /// @param other 非 const 迭代器
            const_iterator(const iterator &other) noexcept;

            /// @brief 解引用操作符，返回当前元素的常量引用
            /// @return 当前元素的常量引用
            reference operator*() const noexcept;

            /// @brief 箭头操作符，返回当前元素的常量指针
            /// @return 当前元素的常量指针
            pointer operator->() const noexcept;

            /// @brief 下标操作符，返回偏移位置元素的常量引用
            /// @param offset 相对于当前位置的偏移量
            /// @return 偏移位置元素的常量引用
            reference operator[](difference_type offset) const;

            /// @brief 前置自增，移动到下一个元素
            /// @return 自增后的迭代器引用
            const_iterator &operator++() noexcept;

            /// @brief 后置自增，移动到下一个元素
            /// @return 自增前的迭代器副本
            const_iterator operator++(int) noexcept;

            /// @brief 前置自减，移动到前一个元素
            /// @return 自减后的迭代器引用
            const_iterator &operator--() noexcept;

            /// @brief 后置自减，移动到前一个元素
            /// @return 自减前的迭代器副本
            const_iterator operator--(int) noexcept;

            /// @brief 加法操作，移动指定偏移量
            /// @param offset 要移动的偏移量
            /// @return 移动后的迭代器
            const_iterator operator+(difference_type offset) const noexcept;

            /// @brief 减法操作，移动指定偏移量
            /// @param offset 要移动的偏移量
            /// @return 移动后的迭代器
            const_iterator operator-(difference_type offset) const noexcept;

            /// @brief 复合赋值加法，移动指定偏移量
            /// @param offset 要移动的偏移量
            /// @return 移动后的迭代器引用
            const_iterator &operator+=(difference_type offset) noexcept;

            /// @brief 复合赋值减法，移动指定偏移量
            /// @param offset 要移动的偏移量
            /// @return 移动后的迭代器引用
            const_iterator &operator-=(difference_type offset) noexcept;

            /// @brief 计算两个迭代器之间的差值
            /// @param other 另一个迭代器
            /// @return 两个迭代器之间的元素个数
            difference_type operator-(
                const const_iterator &other) const noexcept;

            /// @brief 比较两个 const_iterator 是否相等
            /// @param other 要比较的另一个迭代器
            /// @return 如果相等返回true，否则返回false
            bool operator==(const const_iterator &other) const noexcept;

            /// @brief 比较两个 const_iterator 是否不相等
            /// @param other 要比较的另一个迭代器
            /// @return 如果不相等返回true，否则返回false
            bool operator!=(const const_iterator &other) const noexcept;

            /// @brief 比较迭代器是否小于另一个迭代器
            /// @param other 要比较的另一个迭代器
            /// @return 如果小于返回true，否则返回false
            bool operator<(const const_iterator &other) const noexcept;

            /// @brief 比较迭代器是否大于另一个迭代器
            /// @param other 要比较的另一个迭代器
            /// @return 如果大于返回true，否则返回false
            bool operator>(const const_iterator &other) const noexcept;

            /// @brief 比较迭代器是否大于等于另一个迭代器
            /// @param other 要比较的另一个迭代器
            /// @return 如果大于等于返回true，否则返回false
            bool operator>=(const const_iterator &other) const noexcept;

            /// @brief 比较迭代器是否小于等于另一个迭代器
            /// @param other 要比较的另一个迭代器
            /// @return 如果小于等于返回true，否则返回false
            bool operator<=(const const_iterator &other) const noexcept;

            /// @brief 与 iterator 比较是否相等
            /// @param other 要比较的 iterator
            /// @return 如果相等返回true，否则返回false
            bool operator==(const iterator &other) const noexcept;

            /// @brief 与 iterator 比较是否不相等
            /// @param other 要比较的 iterator
            /// @return 如果不相等返回true，否则返回false
            bool operator!=(const iterator &other) const noexcept;

            /// @brief 获取底层指针（供reverse_iterator使用）
            /// @return 底层指针
            typename vector::pointer base() const noexcept;

        private:
            typename vector::pointer m_ptr; ///< 指向当前元素的指针
        };

        /// @brief 反向迭代器类型，使用标准库适配器
        using reverse_iterator = std::reverse_iterator<iterator>;

        /// @brief 常量反向迭代器类型，使用标准库适配器
        using const_reverse_iterator =
            std::reverse_iterator<const_iterator>;

        // ============================================
        // 构造函数与析构函数
        // ============================================

        /// @brief 默认构造函数，创建空vector
        /// @post size() == 0, capacity() == 0, data() == nullptr
        vector();

        /// @brief 填充构造函数，创建包含count个默认值元素的vector
        /// @param count 元素数量
        /// @throw std::length_error 如果count超过max_size()
        /// @post size() == count, capacity() == count
        explicit vector(size_type count);

        /// @brief 填充构造函数，创建包含count个指定值元素的vector
        /// @param count 元素数量
        /// @param val 元素值
        /// @throw std::length_error 如果count超过max_size()
        /// @post size() == count, capacity() == count
        explicit vector(size_type size, const_reference val);

        /// @brief 范围构造函数，复制[first, last)范围内的元素
        /// @tparam InputIt 输入迭代器类型
        /// @param first 范围起始迭代器
        /// @param last 范围结束迭代器
        /// @post size() == std::distance(first, last)
        template <typename InputIt,
                  std::enable_if_t<
                      !std::is_integral<InputIt>::value, int> = 0>
        vector(InputIt first, InputIt last);

        /// @brief 初始化列表构造函数
        /// @param ilist 初始化列表
        /// @post size() == ilist.size(), capacity() == ilist.size()
        vector(std::initializer_list<value_type> ilist);

        /// @brief 拷贝构造函数
        /// @param other 要拷贝的vector
        /// @post *this 是 other 的深拷贝
        vector(const vector &other);

        /// @brief 拷贝赋值运算符
        /// @param other 要拷贝的vector
        /// @return vector引用
        /// @post *this 是 other 的深拷贝
        vector &operator=(const vector &other);

        /// @brief 初始化列表赋值运算符
        /// @param ilist 初始化列表
        /// @return vector引用
        /// @post size() == ilist.size()
        vector &operator=(std::initializer_list<value_type> ilist);

        /// @brief 移动构造函数
        /// @param other 要移动的vector
        /// @post other 变为空vector
        vector(vector &&other) noexcept;

        /// @brief 移动赋值运算符
        /// @param other 要移动的vector
        /// @return vector引用
        /// @post other 变为空vector
        vector &operator=(vector &&other) noexcept;

        /// @brief 析构函数，释放所有资源
        ~vector();

        // ============================================
        // 赋值操作
        // ============================================

        /// @brief 用count个val替换当前vector内容
        /// @param count 元素数量
        /// @param val 元素值
        /// @throw std::length_error 如果count超过max_size()
        /// @post size() == count
        void assign(size_type count, const_reference val);

        /// @brief 用[first, last)范围内的元素替换当前vector内容
        /// @tparam InputIt 输入迭代器类型
        /// @param first 范围起始迭代器
        /// @param last 范围结束迭代器
        /// @post size() == std::distance(first, last)
        template <typename InputIt,
                  std::enable_if_t<
                      !std::is_integral<InputIt>::value, int> = 0>
        void assign(InputIt first, InputIt last);

        /// @brief 用初始化列表替换当前vector内容
        /// @param ilist 初始化列表
        /// @post size() == ilist.size()
        void assign(std::initializer_list<value_type> ilist);

        /// @brief 获取分配器
        /// @return 分配器对象
        Allocator get_allocator() const;

        // ============================================
        // 元素访问
        // ============================================

        /// @brief 访问指定位置的元素（带边界检查）
        /// @param index 元素索引（从0开始）
        /// @return 指定位置元素的引用
        /// @throw std::out_of_range 如果index >= size()
        reference at(size_type index);

        /// @brief 访问指定位置的元素（带边界检查，const版本）
        /// @param index 元素索引（从0开始）
        /// @return 指定位置元素的常量引用
        /// @throw std::out_of_range 如果index >= size()
        const_reference at(size_type index) const;

        /// @brief 下标访问（无边界检查）
        /// @param index 元素索引（从0开始）
        /// @return 指定位置元素的引用
        /// @note 不进行边界检查，访问越界行为未定义
        reference operator[](size_type index);

        /// @brief 下标访问（无边界检查，const版本）
        /// @param index 元素索引（从0开始）
        /// @return 指定位置元素的常量引用
        /// @note 不进行边界检查，访问越界行为未定义
        const_reference operator[](size_type index) const;

        /// @brief 获取第一个元素的引用
        /// @return 第一个元素的引用
        /// @throw std::out_of_range 如果vector为空
        reference front();

        /// @brief 获取第一个元素的常量引用
        /// @return 第一个元素的常量引用
        /// @throw std::out_of_range 如果vector为空
        const_reference front() const;

        /// @brief 获取最后一个元素的引用
        /// @return 最后一个元素的引用
        /// @throw std::out_of_range 如果vector为空
        reference back();

        /// @brief 获取最后一个元素的常量引用
        /// @return 最后一个元素的常量引用
        /// @throw std::out_of_range 如果vector为空
        const_reference back() const;

        /// @brief 获取指向底层数组的指针
        /// @return 指向第一个元素的指针
        pointer data();

        /// @brief 获取指向底层数组的常量指针
        /// @return 指向第一个元素的常量指针
        const_pointer data() const;

        // ============================================
        // 迭代器
        // ============================================

        /// @brief 返回指向第一个元素的迭代器
        /// @return 指向第一个元素的迭代器
        iterator begin();

        /// @brief 返回指向第一个元素的 const 迭代器
        /// @return 指向第一个元素的 const 迭代器
        const_iterator begin() const;

        /// @brief 返回指向第一个元素的 const 迭代器（const版本）
        /// @return 指向第一个元素的 const 迭代器
        const_iterator cbegin() const;

        /// @brief 返回指向末尾的迭代器
        /// @return 指向末尾的迭代器（不指向任何元素）
        iterator end();

        /// @brief 返回指向末尾的 const 迭代器
        /// @return 指向末尾的 const 迭代器（不指向任何元素）
        const_iterator end() const;

        /// @brief 返回指向末尾的 const 迭代器（const版本）
        /// @return 指向末尾的 const 迭代器（不指向任何元素）
        const_iterator cend() const;

        /// @brief 返回指向最后一个元素的反向迭代器
        /// @return 指向最后一个元素的反向迭代器
        reverse_iterator rbegin();

        /// @brief 返回指向最后一个元素的 const 反向迭代器
        /// @return 指向最后一个元素的 const 反向迭代器
        const_reverse_iterator rbegin() const;

        /// @brief 返回指向最后一个元素的 const 反向迭代器（const版本）
        /// @return 指向最后一个元素的 const 反向迭代器
        const_reverse_iterator crbegin() const;

        /// @brief 返回指向第一个元素之前位置的反向迭代器
        /// @return 指向第一个元素之前位置的反向迭代器
        reverse_iterator rend();

        /// @brief 返回指向第一个元素之前位置的 const 反向迭代器
        /// @return 指向第一个元素之前位置的 const 反向迭代器
        const_reverse_iterator rend() const;

        /// @brief 返回指向第一个元素之前位置的 const 反向迭代器（const版本）
        /// @return 指向第一个元素之前位置的 const 反向迭代器
        const_reverse_iterator crend() const;

        // ============================================
        // 容量
        // ============================================

        /// @brief 检查vector是否为空
        /// @return 如果vector为空返回true，否则返回false
        bool empty() const;

        /// @brief 返回vector中的元素数量
        /// @return 当前元素数量
        size_type size() const;

        /// @brief 返回vector的最大可能大小
        /// @return 最大可能的元素数量
        size_type max_size() const;

        /// @brief 预留存储空间
        /// @param new_capacity 新的容量
        /// @throw std::length_error 如果new_capacity超过max_size()
        /// @post capacity() >= new_capacity
        /// @note 该操作不会改变size()，仅预分配内存
        /// @complexity O(n)，可能需要复制所有元素
        void reserve(size_type new_capacity);

        /// @brief 返回当前分配的存储空间大小
        /// @return 当前容量
        size_type capacity() const;

        /// @brief 释放未使用的存储空间
        /// @post capacity() == size()
        /// @note 该操作是提示性的，实现可能选择不释放内存
        /// @complexity O(n)，需要复制所有元素
        void shrink_to_fit();

        // ============================================
        // 修改器
        // ============================================

        /// @brief 清空vector，移除所有元素
        /// @post size() == 0
        /// @note capacity()保持不变
        void clear();

        /// @brief 在指定位置插入一个元素的拷贝
        /// @param iter 插入位置之前的迭代器
        /// @param val 要插入的值
        /// @return 指向新插入元素的迭代器
        /// @throw std::out_of_range 如果iter无效
        /// @complexity O(n)，可能需要移动元素
        iterator insert(const_iterator iter, const_reference val);

        /// @brief 在指定位置插入一个元素（移动语义）
        /// @param iter 插入位置之前的迭代器
        /// @param val 要插入的值（右值引用）
        /// @return 指向新插入元素的迭代器
        /// @throw std::out_of_range 如果iter无效
        /// @complexity O(n)，可能需要移动元素
        iterator insert(const_iterator iter, value_type &&val);

        /// @brief 在指定位置插入count个相同元素
        /// @param iter 插入位置之前的迭代器
        /// @param count 要插入的元素数量
        /// @param val 要插入的值
        /// @return 指向第一个插入元素的迭代器
        /// @throw std::out_of_range 如果iter无效
        /// @complexity O(n + count)
        iterator insert(const_iterator iter,
                        size_type count, const_reference val);

        /// @brief 在指定位置插入[first, last)范围内的元素
        /// @tparam InputIt 输入迭代器类型
        /// @param iter 插入位置之前的迭代器
        /// @param first 范围起始迭代器
        /// @param last 范围结束迭代器
        /// @return 指向第一个插入元素的迭代器
        /// @throw std::out_of_range 如果iter无效
        /// @complexity O(n + count)
        template <typename InputIt,
                  std::enable_if_t<
                      !std::is_integral<InputIt>::value, int> = 0>
        iterator insert(const_iterator iter,
                        InputIt first, InputIt last);

        /// @brief 在指定位置插入初始化列表中的元素
        /// @param iter 插入位置之前的迭代器
        /// @param ilist 初始化列表
        /// @return 指向第一个插入元素的迭代器
        /// @throw std::out_of_range 如果iter无效
        /// @complexity O(n + ilist.size())
        iterator insert(const_iterator iter,
                        std::initializer_list<value_type> ilist);

        /// @brief 在指定位置原地构造一个元素
        /// @tparam Args 构造参数类型
        /// @param iter 插入位置之前的迭代器
        /// @param args 构造参数
        /// @throw std::out_of_range 如果iter无效
        /// @complexity O(n)，可能需要移动元素
        template <typename... Args>
        void emplace(const_iterator iter, Args &&...args);

        /// @brief 删除指定位置的元素
        /// @param iter 指向要删除元素的迭代器
        /// @return 指向被删除元素之后元素的迭代器
        /// @throw std::out_of_range 如果iter无效
        /// @complexity O(n)，需要移动元素
        iterator erase(const_iterator iter);

        /// @brief 删除[first, last)范围内的元素
        /// @param first 要删除范围的起始迭代器
        /// @param last 要删除范围的结束迭代器
        /// @return 指向最后一个被删除元素之后元素的迭代器
        /// @throw std::out_of_range 如果范围无效
        /// @complexity O(n)，需要移动元素
        iterator erase(const_iterator first, const_iterator last);

        /// @brief 在vector尾部插入一个元素的拷贝
        /// @param val 要插入的值
        /// @complexity 均摊O(1)，可能需要扩容
        void push_back(const_reference val);

        /// @brief 在vector尾部插入一个元素（移动语义）
        /// @param val 要插入的值（右值引用）
        /// @complexity 均摊O(1)，可能需要扩容
        void push_back(value_type &&val);

        /// @brief 在vector尾部原地构造一个元素
        /// @tparam Args 构造参数类型
        /// @param args 构造参数
        /// @return 新构造元素的引用
        /// @complexity 均摊O(1)，可能需要扩容
        template <typename... Args>
        reference emplace_back(Args &&...args);

        /// @brief 删除vector尾部的元素
        /// @throw std::out_of_range 如果vector为空
        /// @complexity O(1)
        void pop_back();

        /// @brief 调整vector大小
        /// @param size 新的元素数量
        /// @param val 用于填充新元素的值（默认值初始化）
        /// @throw std::length_error 如果size超过max_size()
        /// @complexity O(n)
        void resize(size_type size, const_reference val = value_type());

        /// @brief 交换两个vector的内容
        /// @param other 要交换的另一个vector
        /// @complexity O(1)，仅交换内部指针
        void swap(vector &other);

    private:
        pointer m_data;        ///< 指向底层数组的指针
        size_type m_size;      ///< 当前元素数量
        size_type m_capacity;  ///< 当前分配的存储空间大小
        Allocator m_allocator; ///< 分配器实例

        using alloc_traits = std::allocator_traits<allocator_type>; ///< 分配器特性类型

        friend bool operator!= <>(const vector<T, Allocator> &lhs,
                                  const vector<T, Allocator> &rhs);
        friend bool operator== <>(const vector<T, Allocator> &lhs,
                                  const vector<T, Allocator> &rhs);
    };

    // ------------------------------ iterator 实现 ------------------------------
    template <typename T, typename Allocator>
    inline vector<T, Allocator>::iterator::iterator() noexcept
        : m_ptr(nullptr)
    {
    }

    template <typename T, typename Allocator>
    inline vector<T, Allocator>::iterator::
        iterator(pointer ptr) noexcept
        : m_ptr(ptr)
    {
    }

    template <typename T, typename Allocator>
    inline typename vector<T, Allocator>::reference
    vector<T, Allocator>::iterator::operator*() const noexcept
    {
        return *m_ptr;
    }

    template <typename T, typename Allocator>
    inline typename vector<T, Allocator>::pointer
    vector<T, Allocator>::iterator::operator->() const noexcept
    {
        return m_ptr;
    }

    template <typename T, typename Allocator>
    inline typename vector<T, Allocator>::reference
    vector<T, Allocator>::iterator::operator[](difference_type n) const
    {
        return m_ptr[n];
    }

    template <typename T, typename Allocator>
    inline typename vector<T, Allocator>::iterator &
    vector<T, Allocator>::iterator::operator++() noexcept
    {
        if (m_ptr != nullptr)
            m_ptr++;
        return *this;
    }

    template <typename T, typename Allocator>
    inline typename vector<T, Allocator>::iterator
    vector<T, Allocator>::iterator::operator++(int) noexcept
    {
        iterator temp(m_ptr);
        if (m_ptr != nullptr)
            m_ptr++;
        return temp;
    }

    template <typename T, typename Allocator>
    inline typename vector<T, Allocator>::iterator &
    vector<T, Allocator>::iterator::operator--() noexcept
    {
        m_ptr--;
        return *this;
    }

    template <typename T, typename Allocator>
    inline typename vector<T, Allocator>::iterator
    vector<T, Allocator>::iterator::operator--(int) noexcept
    {
        iterator temp(m_ptr);
        if (m_ptr != nullptr)
            m_ptr--;
        return temp;
    }

    template <typename T, typename Allocator>
    inline typename vector<T, Allocator>::iterator
    vector<T, Allocator>::iterator::operator+(difference_type n) const noexcept
    {
        return iterator(m_ptr + n);
    }

    template <typename T, typename Allocator>
    inline typename vector<T, Allocator>::iterator
    vector<T, Allocator>::iterator::operator-(difference_type n) const noexcept
    {
        return iterator(m_ptr - n);
    }

    template <typename T, typename Allocator>
    inline typename vector<T, Allocator>::iterator &
    vector<T, Allocator>::iterator::operator+=(difference_type n) noexcept
    {
        if (m_ptr != nullptr)
            m_ptr += n;
        return *this;
    }

    template <typename T, typename Allocator>
    inline typename vector<T, Allocator>::iterator &
    vector<T, Allocator>::iterator::operator-=(difference_type n) noexcept
    {
        if (m_ptr != nullptr)
            m_ptr -= n;
        return *this;
    }

    template <typename T, typename Allocator>
    inline typename vector<T, Allocator>::difference_type
    vector<T, Allocator>::iterator::operator-(const iterator &other) const noexcept
    {
        return static_cast<difference_type>(m_ptr - other.m_ptr);
    }

    template <typename T, typename Allocator>
    inline bool vector<T, Allocator>::iterator::
    operator==(const iterator &other) const noexcept
    {
        return m_ptr == other.m_ptr;
    }

    template <typename T, typename Allocator>
    inline bool vector<T, Allocator>::iterator::
    operator!=(const iterator &other) const noexcept
    {
        return m_ptr != other.m_ptr;
    }

    template <typename T, typename Allocator>
    inline bool vector<T, Allocator>::iterator::
    operator>(const iterator &other) const noexcept
    {
        return m_ptr > other.m_ptr;
    }

    template <typename T, typename Allocator>
    inline bool vector<T, Allocator>::iterator::
    operator<(const iterator &other) const noexcept
    {
        return m_ptr < other.m_ptr;
    }

    template <typename T, typename Allocator>
    inline bool vector<T, Allocator>::iterator::
    operator>=(const iterator &other) const noexcept
    {
        return m_ptr >= other.m_ptr;
    }

    template <typename T, typename Allocator>
    inline bool vector<T, Allocator>::iterator::
    operator<=(const iterator &other) const noexcept
    {
        return m_ptr <= other.m_ptr;
    }

    template <typename T, typename Allocator>
    inline typename vector<T, Allocator>::pointer
    vector<T, Allocator>::iterator::base() const noexcept
    {
        return m_ptr;
    }

    // ------------------------------ const_iterator 实现 ------------------------------
    template <typename T, typename Allocator>
    inline vector<T, Allocator>::const_iterator::
        const_iterator() noexcept
        : m_ptr(nullptr)
    {
    }

    template <typename T, typename Allocator>
    inline vector<T, Allocator>::const_iterator::
        const_iterator(typename vector::pointer ptr) noexcept
        : m_ptr(ptr)
    {
    }

    template <typename T, typename Allocator>
    inline vector<T, Allocator>::const_iterator::
        const_iterator(const iterator &other) noexcept
        : m_ptr(other.m_ptr)
    {
    }

    template <typename T, typename Allocator>
    inline typename vector<T, Allocator>::const_reference
    vector<T, Allocator>::const_iterator::operator*() const noexcept
    {
        return *m_ptr;
    }

    template <typename T, typename Allocator>
    inline typename vector<T, Allocator>::const_pointer
    vector<T, Allocator>::const_iterator::operator->() const noexcept
    {
        return m_ptr;
    }

    template <typename T, typename Allocator>
    inline typename vector<T, Allocator>::const_reference
    vector<T, Allocator>::const_iterator::
    operator[](difference_type offset) const
    {
        return m_ptr[offset];
    }

    template <typename T, typename Allocator>
    inline typename vector<T, Allocator>::const_iterator &
    vector<T, Allocator>::const_iterator::operator++() noexcept
    {
        if (m_ptr != nullptr)
            m_ptr++;
        return *this;
    }

    template <typename T, typename Allocator>
    inline typename vector<T, Allocator>::const_iterator
    vector<T, Allocator>::const_iterator::operator++(int) noexcept
    {
        const_iterator temp(m_ptr);
        if (m_ptr != nullptr)
            m_ptr++;
        return temp;
    }

    template <typename T, typename Allocator>
    inline typename vector<T, Allocator>::const_iterator &
    vector<T, Allocator>::const_iterator::operator--() noexcept
    {
        if (m_ptr != nullptr)
            m_ptr--;
        return *this;
    }

    template <typename T, typename Allocator>
    inline typename vector<T, Allocator>::const_iterator
    vector<T, Allocator>::const_iterator::operator--(int) noexcept
    {
        const_iterator temp(m_ptr);
        if (m_ptr != nullptr)
            m_ptr--;
        return temp;
    }

    template <typename T, typename Allocator>
    inline typename vector<T, Allocator>::const_iterator
    vector<T, Allocator>::const_iterator::
    operator+(difference_type offset) const noexcept
    {
        return const_iterator(m_ptr + offset);
    }

    template <typename T, typename Allocator>
    inline typename vector<T, Allocator>::const_iterator
    vector<T, Allocator>::const_iterator::
    operator-(difference_type offset) const noexcept
    {
        return const_iterator(m_ptr - offset);
    }

    template <typename T, typename Allocator>
    inline typename vector<T, Allocator>::const_iterator &
    vector<T, Allocator>::const_iterator::
    operator+=(difference_type offset) noexcept
    {
        if (m_ptr != nullptr)
            m_ptr += offset;
        return *this;
    }

    template <typename T, typename Allocator>
    inline typename vector<T, Allocator>::const_iterator &
    vector<T, Allocator>::const_iterator::
    operator-=(difference_type offset) noexcept
    {
        if (m_ptr != nullptr)
            m_ptr -= offset;
        return *this;
    }

    template <typename T, typename Allocator>
    inline typename vector<T, Allocator>::difference_type
    vector<T, Allocator>::const_iterator::
    operator-(const const_iterator &other) const noexcept
    {
        return static_cast<difference_type>(m_ptr - other.m_ptr);
    }

    template <typename T, typename Allocator>
    inline bool vector<T, Allocator>::const_iterator::
    operator==(const const_iterator &other) const noexcept
    {
        return m_ptr == other.m_ptr;
    }

    template <typename T, typename Allocator>
    inline bool vector<T, Allocator>::const_iterator::
    operator!=(const const_iterator &other) const noexcept
    {
        return m_ptr != other.m_ptr;
    }

    template <typename T, typename Allocator>
    inline bool vector<T, Allocator>::const_iterator::
    operator<(const const_iterator &other) const noexcept
    {
        return m_ptr < other.m_ptr;
    }

    template <typename T, typename Allocator>
    inline bool vector<T, Allocator>::const_iterator::
    operator>(const const_iterator &other) const noexcept
    {
        return m_ptr > other.m_ptr;
    }

    template <typename T, typename Allocator>
    inline bool vector<T, Allocator>::const_iterator::
    operator>=(const const_iterator &other) const noexcept
    {
        return m_ptr >= other.m_ptr;
    }

    template <typename T, typename Allocator>
    inline bool vector<T, Allocator>::const_iterator::
    operator<=(const const_iterator &other) const noexcept
    {
        return m_ptr <= other.m_ptr;
    }

    template <typename T, typename Allocator>
    inline bool vector<T, Allocator>::const_iterator::
    operator==(const iterator &other) const noexcept
    {
        return m_ptr == other.m_ptr;
    }

    template <typename T, typename Allocator>
    inline bool vector<T, Allocator>::const_iterator::
    operator!=(const iterator &other) const noexcept
    {
        return m_ptr != other.m_ptr;
    }

    template <typename T, typename Allocator>
    inline typename vector<T, Allocator>::pointer
    vector<T, Allocator>::const_iterator::base() const noexcept
    {
        return m_ptr;
    }

    // ------------------------------ vector 核心实现 ------------------------------
    template <typename T, typename Allocator>
    inline vector<T, Allocator>::vector()
        : m_data(nullptr), m_size(0), m_capacity(0), m_allocator()
    {
    }

    template <typename T, typename Allocator>
    inline demo::vector<T, Allocator>::vector(size_type size)
    {
        if (size > max_size())
            throw std::length_error("forward_list count exceeds max_size");

        m_data = alloc_traits::allocate(m_allocator, size);
        m_size = size;
        m_capacity = size;

        for (size_type i = 0; i < m_size; i++)
        {
            alloc_traits::construct(m_allocator, m_data + i);
        }
    }

    template <typename T, typename Allocator>
    inline vector<T, Allocator>::vector(size_type size, const_reference val)
    {
        if (size > max_size())
            throw std::length_error("forward_list count exceeds max_size");

        m_data = alloc_traits::allocate(m_allocator, size);
        m_size = size;
        m_capacity = size;

        for (size_t i = 0; i < m_size; i++)
        {
            alloc_traits::construct(m_allocator, m_data + i, val);
        }
    }

    template <typename T, typename Allocator>
    template <typename InputIt,
              std::enable_if_t<
                  !std::is_integral<InputIt>::value, int>>
    inline vector<T, Allocator>::vector(InputIt first, InputIt last)
        : m_data(nullptr), m_size(0), m_capacity(0)
    {
        using iter_category =
            typename std::iterator_traits<InputIt>::iterator_category;
        constexpr bool is_random_access = std::is_base_of<
            std::random_access_iterator_tag, iter_category>::value;

        if constexpr (is_random_access)
        {
            size_type count = std::distance(first, last);
            if (count > 0)
                reserve(count);
            for (size_type i = 0; i < count; i++, first++)
            {
                alloc_traits::construct(m_allocator, m_data + i, *first);
            }
            m_size = count;
        }
        else
        {
            for (; first != last; first++)
                push_back(*first);
        }
    }

    template <typename T, typename Allocator>
    inline vector<T, Allocator>::vector(
        std::initializer_list<value_type> ilist)
        : m_data(nullptr), m_size(0), m_capacity(0)
    {
        size_type count = ilist.size();
        if (count > 0)
            reserve(count);
        size_type i = 0;
        for (const_reference val : ilist)
        {
            alloc_traits::construct(m_allocator, m_data + i, val);
            i++;
        }
        m_size = count;
    }

    template <typename T, typename Allocator>
    inline vector<T, Allocator>::vector(const vector &other)
        : m_data(alloc_traits::allocate(m_allocator, other.m_capacity)),
          m_size(other.m_size), m_capacity(other.m_capacity)
    {
        for (size_type i = 0; i < m_size; i++)
        {
            alloc_traits::construct(m_allocator, m_data + i,
                                    other.m_data[i]);
        }
    }
    template <typename T, typename Allocator>
    inline vector<T, Allocator> &vector<T, Allocator>::
    operator=(const vector &other)
    {
        if (this == &other)
            return *this;

        // 先析构当前对象的所有元素
        clear();

        if (m_capacity >= other.m_size)
        {
            // 已有空间足够，直接在原空间上构造
            for (size_type i = 0; i < other.m_size; i++)
            {
                alloc_traits::construct(m_allocator,
                                        m_data + i, other.m_data[i]);
            }
            m_size = other.m_size;
        }
        else
        {
            // 原空间不够，需要扩容
            if (m_data != nullptr)
                alloc_traits::deallocate(m_allocator, m_data, m_capacity);
            m_data = alloc_traits::allocate(m_allocator, other.m_capacity);
            m_capacity = other.m_capacity;
            for (size_type i = 0; i < other.m_size; i++)
            {
                alloc_traits::construct(m_allocator,
                                        m_data + i, other.m_data[i]);
            }
            m_size = other.m_size;
        }
        return *this;
    }

    template <typename T, typename Allocator>
    inline vector<T, Allocator> &vector<T, Allocator>::
    operator=(std::initializer_list<value_type> ilist)
    {
        // 先清空当前元素
        clear();

        // 预分配内存
        size_type count = ilist.size();
        if (count > 0)
            reserve(count);

        // 构造新元素
        size_type i = 0;
        for (const value_type &val : ilist)
        {
            alloc_traits::construct(m_allocator, m_data + i, val);
            i++;
        }
        m_size = count;

        return *this;
    }

    template <typename T, typename Allocator>
    inline vector<T, Allocator>::vector(vector &&other) noexcept
        : m_data(other.m_data), m_size(other.m_size),
          m_capacity(other.m_capacity), m_allocator(std::move(other.m_allocator))
    {
        other.m_data = nullptr;
        other.m_size = 0;
        other.m_capacity = 0;
    }

    template <typename T, typename Allocator>
    inline vector<T, Allocator> &vector<T, Allocator>::
    operator=(vector &&other) noexcept
    {
        if (this == &other)
            return *this;

        clear();
        if (m_data != nullptr)
            alloc_traits::deallocate(m_allocator, m_data, m_capacity);

        m_data = other.m_data;
        m_size = other.m_size;
        m_capacity = other.m_capacity;
        m_allocator = std::move(other.m_allocator);

        other.m_data = nullptr;
        other.m_size = 0;
        other.m_capacity = 0;

        return *this;
    }

    template <typename T, typename Allocator>
    inline vector<T, Allocator>::~vector()
    {
        clear(); // 先销毁所有元素

        if (m_data != nullptr)
            alloc_traits::deallocate(m_allocator, m_data, m_capacity);

        m_data = nullptr;
        m_size = 0;
        m_capacity = 0;
    }

    template <typename T, typename Allocator>
    inline void demo::vector<T, Allocator>::
        assign(size_type count, const_reference val)
    {
        if (count > max_size())
            throw std::length_error("vector::assign: count exceeds max_size");

        clear();

        if (count > m_capacity)
            reserve(count);

        for (size_type i = 0; i < count; i++)
        {
            alloc_traits::construct(m_allocator, m_data + i, val);
        }

        m_size = count;
    }

    template <typename T, typename Allocator>
    template <typename InputIt,
              std::enable_if_t<
                  !std::is_integral<InputIt>::value, int>>
    inline void vector<T, Allocator>::assign(InputIt first, InputIt last)
    {
        clear();

        using iter_category =
            typename std::iterator_traits<InputIt>::iterator_category;
        constexpr bool is_random_access_iterator = std::is_base_of<
            std::random_access_iterator_tag, iter_category>::value;

        if constexpr (is_random_access_iterator)
        {
            size_type count = std::distance(first, last);
            if (count > 0)
                reserve(count);

            for (size_type i = 0; i < count; i++, first++)
            {
                alloc_traits::construct(m_allocator, m_data + i, *first);
            }
            m_size = count;
        }
        else
        {
            for (; first != last; first++)
                push_back(*first);
        }
    }

    template <typename T, typename Allocator>
    inline void demo::vector<T, Allocator>::
        assign(std::initializer_list<value_type> ilist)
    {
        clear();

        size_type count = ilist.size();
        if (count > 0)
            reserve(count);

        size_type i = 0;
        for (auto &val : ilist)
        {
            alloc_traits::construct(m_allocator, m_data + i, val);
            i++;
        }
        m_size = ilist.size();
    }

    template <typename T, typename Allocator>
    inline Allocator vector<T, Allocator>::get_allocator() const
    {
        return m_allocator;
    }

    template <typename T, typename Allocator>
    inline typename vector<T, Allocator>::reference
    vector<T, Allocator>::at(size_type index)
    {
        if (index >= m_size)
            throw std::out_of_range("vector::at: index out of range.");

        return m_data[index];
    }

    template <typename T, typename Allocator>
    inline typename vector<T, Allocator>::const_reference
    vector<T, Allocator>::at(size_type index) const
    {
        if (index >= m_size)
            throw std::out_of_range("vector::at: index out of range.");

        return m_data[index];
    }

    template <typename T, typename Allocator>
    inline typename vector<T, Allocator>::reference
    vector<T, Allocator>::operator[](size_type index)
    {
        return m_data[index];
    }

    template <typename T, typename Allocator>
    inline typename vector<T, Allocator>::const_reference
    vector<T, Allocator>::operator[](size_type index) const
    {
        return m_data[index]; // []不做越界检查
    }

    template <typename T, typename Allocator>
    inline typename vector<T, Allocator>::reference
    vector<T, Allocator>::front()
    {
        if (m_size == 0)
            throw std::out_of_range("vector::front: empty vector");
        return m_data[0];
    }

    template <typename T, typename Allocator>
    inline typename vector<T, Allocator>::const_reference
    vector<T, Allocator>::front() const
    {
        if (m_size == 0)
            throw std::out_of_range("vector::front: empty vector");
        return m_data[0];
    }

    template <typename T, typename Allocator>
    inline typename vector<T, Allocator>::reference
    vector<T, Allocator>::back()
    {
        if (m_size == 0)
            throw std::out_of_range("vector::back: empty vector");
        return m_data[m_size - 1];
    }

    template <typename T, typename Allocator>
    inline typename vector<T, Allocator>::const_reference
    vector<T, Allocator>::back() const
    {
        if (m_size == 0)
            throw std::out_of_range("vector::back: empty vector");
        return m_data[m_size - 1];
    }

    template <typename T, typename Allocator>
    inline typename vector<T, Allocator>::pointer
    vector<T, Allocator>::data()
    {
        return m_data;
    }

    template <typename T, typename Allocator>
    inline typename vector<T, Allocator>::const_pointer
    vector<T, Allocator>::data() const
    {
        return m_data;
    }

    template <typename T, typename Allocator>
    inline typename vector<T, Allocator>::iterator
    vector<T, Allocator>::begin()
    {
        return iterator(m_data);
    }

    template <typename T, typename Allocator>
    inline typename vector<T, Allocator>::const_iterator
    vector<T, Allocator>::begin() const
    {
        return const_iterator(m_data);
    }

    template <typename T, typename Allocator>
    inline typename vector<T, Allocator>::const_iterator
    vector<T, Allocator>::cbegin() const
    {
        return const_iterator(m_data);
    }

    template <typename T, typename Allocator>
    inline typename vector<T, Allocator>::iterator
    vector<T, Allocator>::end()
    {
        return iterator(m_data + m_size);
    }

    template <typename T, typename Allocator>
    inline typename vector<T, Allocator>::const_iterator
    vector<T, Allocator>::end() const
    {
        return const_iterator(m_data + m_size);
    }

    template <typename T, typename Allocator>
    inline typename vector<T, Allocator>::const_iterator
    vector<T, Allocator>::cend() const
    {
        return const_iterator(m_data + m_size);
    }

    template <typename T, typename Allocator>
    inline typename vector<T, Allocator>::reverse_iterator
    vector<T, Allocator>::rbegin()
    {
        return reverse_iterator(end());
    }

    template <typename T, typename Allocator>
    inline typename vector<T, Allocator>::const_reverse_iterator
    vector<T, Allocator>::rbegin() const
    {
        return const_reverse_iterator(end());
    }

    template <typename T, typename Allocator>
    inline typename vector<T, Allocator>::const_reverse_iterator
    vector<T, Allocator>::crbegin() const
    {
        return const_reverse_iterator(end());
    }

    template <typename T, typename Allocator>
    inline typename vector<T, Allocator>::reverse_iterator
    vector<T, Allocator>::rend()
    {
        return reverse_iterator(begin());
    }

    template <typename T, typename Allocator>
    inline typename vector<T, Allocator>::const_reverse_iterator
    vector<T, Allocator>::crend() const
    {
        return const_reverse_iterator(begin());
    }

    template <typename T, typename Allocator>
    inline bool vector<T, Allocator>::empty() const
    {
        return m_size == 0;
    }

    template <typename T, typename Allocator>
    inline typename vector<T, Allocator>::size_type
    vector<T, Allocator>::size() const
    {
        return m_size;
    }

    template <typename T, typename Allocator>
    inline typename vector<T, Allocator>::size_type
    vector<T, Allocator>::max_size() const
    {
        return alloc_traits::max_size(m_allocator);
    }

    template <typename T, typename Allocator>
    inline void vector<T, Allocator>::reserve(size_type new_capacity)
    {
        if (new_capacity > max_size())
            throw std::length_error("vector::reserve: capacity exceeds max_size");

        if (new_capacity <= m_capacity)
            return;

        pointer new_data = alloc_traits::allocate(m_allocator, new_capacity);

        size_type constructed_count = 0;
        try
        {
            for (size_type i = 0; i < m_size; i++)
            {
                alloc_traits::construct(m_allocator, new_data + i, std::move_if_noexcept(m_data[i]));
                constructed_count++;
            }
        }
        catch (...)
        {
            for (size_type i = 0; i < constructed_count; i++)
                alloc_traits::destroy(m_allocator, new_data + i);
            alloc_traits::deallocate(m_allocator, new_data, new_capacity);
            throw;
        }

        for (size_type i = 0; i < m_size; i++)
            alloc_traits::destroy(m_allocator, m_data + i);

        if (m_data != nullptr)
            alloc_traits::deallocate(m_allocator, m_data, m_capacity);

        m_data = new_data;
        m_capacity = new_capacity;
    }

    template <typename T, typename Allocator>
    inline typename vector<T, Allocator>::size_type
    vector<T, Allocator>::capacity() const
    {
        return m_capacity;
    }

    template <typename T, typename Allocator>
    inline void vector<T, Allocator>::shrink_to_fit()
    {
        if (m_size < m_capacity)
        {
            pointer new_data = alloc_traits::allocate(m_allocator, m_size);
            for (size_type i = 0; i < m_size; ++i)
            {
                alloc_traits::construct(m_allocator,
                                        new_data + i,
                                        std::move_if_noexcept(m_data[i]));
                alloc_traits::destroy(m_allocator, &m_data[i]);
            }
            alloc_traits::deallocate(m_allocator, m_data, m_capacity);
            m_data = new_data;
            m_capacity = m_size;
        }
    }

    template <typename T, typename Allocator>
    inline void vector<T, Allocator>::clear()
    {
        for (size_type i = 0; i < m_size; i++)
            alloc_traits::destroy(m_allocator, m_data + i);
        m_size = 0;
    }

    template <typename T, typename Allocator>
    inline typename vector<T, Allocator>::iterator
    vector<T, Allocator>::insert(const_iterator pos, const_reference val)
    {
        difference_type diff = pos - cbegin();
        if (diff < 0 || static_cast<size_type>(diff) > m_size)
            throw std::out_of_range("vector::insert: invalid position");

        if (m_capacity < m_size + 1)
        {
            size_type new_capacity = (m_capacity + 2) * 2;
            reserve(new_capacity);
        }

        // for (size_type i = m_size; i > static_cast<size_type>(diff); i--)
        // {
        //     alloc_traits::construct(m_allocator, m_data + i, std::move_if_noexcept(m_data[i - 1]));
        //     alloc_traits::destroy(m_allocator, m_data + i - 1);
        // }

        size_type moved_count = 0;
        try
        {
            for (size_type i = m_size; i > static_cast<size_type>(diff); i--)
            {
                alloc_traits::construct(m_allocator,
                                        m_data + i,
                                        std::move_if_noexcept(m_data[i - 1]));
                moved_count++;
                alloc_traits::destroy(m_allocator, m_data + i - 1);
            }
        }
        catch (...)
        {
            // 异常回滚：销毁已构造的新元素，恢复原状态
            for (size_type i = 0; i < moved_count; ++i)
            {
                size_type idx = m_size - i;
                alloc_traits::construct(m_allocator,
                                        m_data + idx - 1,
                                        std::move_if_noexcept(m_data[idx]));
                alloc_traits::destroy(m_allocator, m_data + idx);
            }
            throw;
        }

        alloc_traits::construct(m_allocator, m_data + diff, val);
        m_size++;

        return iterator(m_data + diff);
    }

    template <typename T, typename Allocator>
    inline typename vector<T, Allocator>::iterator
    vector<T, Allocator>::insert(const_iterator pos, value_type &&val)
    {
        difference_type diff = pos - cbegin();
        if (diff < 0 || static_cast<size_type>(diff) > m_size)
            throw std::out_of_range("vector::insert: invalid position");

        if (m_capacity < m_size + 1)
        {
            size_type new_capacity = (m_capacity + 2) * 2;
            reserve(new_capacity);
        }

        size_type moved_count = 0;
        try
        {
            for (size_type i = m_size; i > static_cast<size_type>(diff); i--)
            {
                alloc_traits::construct(m_allocator,
                                        m_data + i,
                                        std::move_if_noexcept(m_data[i - 1]));
                moved_count++;
                alloc_traits::destroy(m_allocator, m_data + i - 1);
            }
        }
        catch (...)
        {
            // 异常回滚：销毁已构造的新元素，恢复原状态
            for (size_type i = 0; i < moved_count; ++i)
            {
                size_type idx = m_size - i;
                alloc_traits::construct(m_allocator,
                                        m_data + idx - 1,
                                        std::move_if_noexcept(m_data[idx]));
                alloc_traits::destroy(m_allocator, m_data + idx);
            }
            throw;
        }

        alloc_traits::construct(m_allocator, m_data + diff, std::move(val));
        m_size++;

        return iterator(m_data + diff);
    }

    template <typename T, typename Allocator>
    inline typename vector<T, Allocator>::iterator vector<T, Allocator>::
        insert(const_iterator pos, size_type count, const_reference val)
    {
        if (count == 0)
            return iterator(const_cast<pointer>(pos.operator->()));

        difference_type diff = pos - cbegin();
        if (diff < 0 || static_cast<size_type>(diff) > m_size)
            throw std::out_of_range("vector::insert: invalid position");

        if (m_capacity < m_size + count)
        {
            size_type new_capacity = std::max((m_capacity + 2) * 2, m_size + count);
            reserve(new_capacity);
        }

        size_type moved_count = 0;
        try
        {
            for (size_type i = m_size + count - 1;
                 i >= static_cast<size_type>(diff) + count; i--)
            {
                alloc_traits::construct(m_allocator,
                                        m_data + i,
                                        std::move_if_noexcept(m_data[i - count]));
                moved_count++;
                alloc_traits::destroy(m_allocator, m_data + i - count);
            }
        }
        catch (...)
        {
            // 异常回滚：销毁已构造的新元素，恢复原状态
            for (size_type i = 0; i < moved_count; ++i)
            {
                size_type idx = m_size + count - 1 - i;
                alloc_traits::construct(m_allocator,
                                        m_data + idx - count,
                                        std::move_if_noexcept(m_data[idx]));
                alloc_traits::destroy(m_allocator, m_data + idx);
            }
            throw;
        }

        for (size_type i = 0; i < count; i++)
            alloc_traits::construct(m_allocator, m_data + diff + i, val);
        m_size += count;

        return iterator(m_data + diff);
    }

    template <typename T, typename Allocator>
    inline typename vector<T, Allocator>::iterator vector<T, Allocator>::
        insert(const_iterator pos, std::initializer_list<value_type> ilist)
    {
        if (ilist.size() == 0)
            return iterator(const_cast<pointer>(pos.operator->()));

        size_type count = ilist.size();

        difference_type diff = pos - cbegin();
        if (diff < 0 || static_cast<size_type>(diff) > m_size)
            throw std::out_of_range("vector::insert: invalid position");

        if (m_capacity < m_size + count)
        {
            size_type new_capacity =
                std::max((m_capacity + 2) * 2, m_size + count);
            reserve(new_capacity);
        }

        size_type moved_count = 0;
        try
        {
            for (size_type i = m_size + count - 1;
                 i >= static_cast<size_type>(diff) + count; i--)
            {
                alloc_traits::construct(m_allocator,
                                        m_data + i,
                                        std::move_if_noexcept(m_data[i - count]));
                moved_count++;
                alloc_traits::destroy(m_allocator, m_data + i - count);
            }
        }
        catch (...)
        {
            // 异常回滚：销毁已构造的新元素，恢复原状态
            for (size_type i = 0; i < moved_count; ++i)
            {
                size_type idx = m_size + count - 1 - i;
                alloc_traits::construct(m_allocator,
                                        m_data + idx - count,
                                        std::move_if_noexcept(m_data[idx]));
                alloc_traits::destroy(m_allocator, m_data + idx);
            }
            throw;
        }

        size_type i = 0;
        for (auto &val : ilist)
        {
            alloc_traits::construct(m_allocator, m_data + diff + i, val);
            i++;
        }
        m_size += count;
        return iterator(m_data + diff);
    }

    template <typename T, typename Allocator>
    template <typename InputIt,
              std::enable_if_t<
                  !std::is_integral<InputIt>::value, int>>
    inline typename vector<T, Allocator>::iterator vector<T, Allocator>::
        insert(const_iterator pos, InputIt first, InputIt last)
    {
        if (first == last)
            return iterator(const_cast<pointer>(pos.operator->()));

        difference_type diff = pos - cbegin();
        if (diff < 0 || static_cast<size_type>(diff) > m_size)
            throw std::out_of_range("vector::insert: invalid position");

        using iter_category =
            typename std::iterator_traits<InputIt>::iterator_category;
        constexpr bool is_random_access_iterator = std::is_base_of<
            std::random_access_iterator_tag, iter_category>::value;

        if constexpr (is_random_access_iterator)
        {
            size_type count = std::distance(first, last);

            if (m_capacity < m_size + count)
            {
                size_type new_capacity =
                    std::max((m_capacity + 2) * 2, m_size + count);
                reserve(new_capacity);
            }

            size_type moved_count = 0;
            try
            {
                for (size_type i = m_size + count - 1;
                     i >= static_cast<size_type>(diff) + count; i--)
                {
                    alloc_traits::construct(
                        m_allocator,
                        m_data + i,
                        std::move_if_noexcept(m_data[i - count]));
                    moved_count++;
                    alloc_traits::destroy(m_allocator, m_data + i - count);
                }
            }
            catch (...)
            {
                // 异常回滚：销毁已构造的新元素，恢复原状态
                for (size_type i = 0; i < moved_count; ++i)
                {
                    size_type idx = m_size + count - 1 - i;
                    alloc_traits::construct(m_allocator, m_data + idx - count,
                                            std::move_if_noexcept(m_data[idx]));
                    alloc_traits::destroy(m_allocator, m_data + idx);
                }
                throw;
            }

            for (size_type i = 0; i < count; i++, first++)
                alloc_traits::construct(m_allocator,
                                        m_data + diff + i, *first);

            m_size += count;
            return iterator(m_data + diff);
        }
        else
        {
            size_type cur_diff = static_cast<size_type>(diff);
            while (first != last)
            {
                if (m_capacity == m_size)
                {
                    size_type new_capacity = (m_capacity + 2) * 2;
                    reserve(new_capacity);
                }

                size_type moved_count = 0;
                try
                {
                    for (size_type i = m_size; i > cur_diff; i--)
                    {
                        alloc_traits::construct(
                            m_allocator, m_data + i,
                            std::move_if_noexcept(m_data[i - 1]));
                        moved_count++;
                        alloc_traits::destroy(m_allocator, m_data + i - 1);
                    }
                }
                catch (...)
                {
                    // 异常回滚：销毁已构造的新元素，恢复原状态
                    for (size_type i = 0; i < moved_count; ++i)
                    {
                        size_type idx = m_size - i;
                        alloc_traits::construct(m_allocator, m_data + idx - 1,
                                                std::move_if_noexcept(m_data[idx]));
                        alloc_traits::destroy(m_allocator, m_data + idx);
                    }
                    throw;
                }

                alloc_traits::construct(m_allocator, m_data + cur_diff, *first);
                m_size++;
                first++;
                cur_diff++;
            }

            return iterator(m_data + diff);
        }
    }

    template <typename T, typename Allocator>
    template <typename... Args>
    inline void vector<T, Allocator>::emplace(const_iterator pos, Args &&...args)
    {
        difference_type diff = pos - cbegin();
        if (diff < 0 || static_cast<size_type>(diff) > m_size)
            throw std::out_of_range("vector::emplace: invalid position");

        if (m_capacity < m_size + 1)
        {
            size_type new_capacity = (m_capacity + 2) * 2;
            reserve(new_capacity);
        }

        size_type moved_count = 0;
        try
        {
            for (size_type i = m_size; i > static_cast<size_type>(diff); i--)
            {
                alloc_traits::construct(m_allocator, m_data + i,
                                        std::move_if_noexcept(m_data[i - 1]));
                moved_count++;
                alloc_traits::destroy(m_allocator, m_data + i - 1);
            }
        }
        catch (...)
        {
            // 异常回滚：销毁已构造的新元素，恢复原状态
            for (size_type i = 0; i < moved_count; ++i)
            {
                size_type idx = m_size - i;
                alloc_traits::construct(m_allocator, m_data + idx - 1,
                                        std::move_if_noexcept(m_data[idx]));
                alloc_traits::destroy(m_allocator, m_data + idx);
            }
            throw;
        }

        alloc_traits::construct(m_allocator, m_data + diff,
                                std::forward<Args>(args)...);
        m_size++;
    }

    template <typename T, typename Allocator>
    inline typename vector<T, Allocator>::iterator
    vector<T, Allocator>::erase(const_iterator pos)
    {
        difference_type diff = pos - cbegin();
        if (diff < 0 || static_cast<size_type>(diff) >= m_size)
            throw std::out_of_range("vector::erase: invalid position");

        alloc_traits::destroy(m_allocator, m_data + diff);

        for (size_type i = static_cast<size_type>(diff); i < m_size - 1; i++)
        {
            alloc_traits::construct(m_allocator,
                                    m_data + i,
                                    std::move_if_noexcept(m_data[i + 1]));
            alloc_traits::destroy(m_allocator, m_data + i + 1);
        }

        m_size--;
        return iterator(m_data + diff);
    }

    template <typename T, typename Allocator>
    inline typename vector<T, Allocator>::iterator vector<T, Allocator>::
        erase(const_iterator first, const_iterator last)
    {
        if (last < first)
            throw std::out_of_range("vector::erase: last < first (invalid range)");

        difference_type diff = first - cbegin();
        difference_type diff_last = last - cbegin();
        if (diff < 0 || static_cast<size_type>(diff) >= m_size ||
            diff_last < 0 || static_cast<size_type>(diff_last) > m_size)
            throw std::out_of_range("vector::erase: invalid position (out of range)");

        size_type count = static_cast<size_type>(last - first);

        if (count == 0)
            return iterator(const_cast<pointer>(first.base()));

        for (size_type i = 0; i < count; i++)
            alloc_traits::destroy(m_allocator, m_data + diff + i);

        for (size_type i = static_cast<size_type>(diff_last); i < m_size; i++)
        {
            alloc_traits::construct(m_allocator,
                                    m_data + i - count,
                                    std::move_if_noexcept(m_data[i]));
            alloc_traits::destroy(m_allocator, m_data + i);
        }

        m_size -= count;
        return iterator(m_data + diff);
    }

    template <typename T, typename Allocator>
    inline void vector<T, Allocator>::push_back(const_reference val)
    {
        if (m_capacity < m_size + 1)
        {
            size_type new_capacity = (m_capacity + 2) * 2;
            reserve(new_capacity);
        }

        alloc_traits::construct(m_allocator, m_data + m_size, val);
        m_size++;
    }

    template <typename T, typename Allocator>
    inline void vector<T, Allocator>::push_back(value_type &&val)
    {
        if (m_capacity < m_size + 1)
        {
            size_type new_capacity = (m_capacity + 2) * 2;
            reserve(new_capacity);
        }

        alloc_traits::construct(m_allocator,
                                m_data + m_size, std::move(val));
        m_size++;
    }

    template <typename T, typename Allocator>
    template <typename... Args>
    inline typename vector<T, Allocator>::reference
    vector<T, Allocator>::emplace_back(Args &&...args)
    {
        if (m_capacity < m_size + 1)
        {
            size_type new_capacity = (m_capacity + 2) * 2;
            reserve(new_capacity);
        }

        alloc_traits::construct(m_allocator,
                                m_data + m_size,
                                std::forward<Args>(args)...);
        m_size++;

        return m_data[m_size - 1];
    }

    template <typename T, typename Allocator>
    inline void vector<T, Allocator>::pop_back()
    {
        if (empty())
            throw std::out_of_range("vector::pop_back: empty vector");
        m_size--;
        alloc_traits::destroy(m_allocator, m_data + m_size - 1);
    }

    template <typename T, typename Allocator>
    inline void vector<T, Allocator>::resize(size_type size,
                                             const_reference val)
    {
        if (size > max_size())
            throw std::length_error("vector::resize: capacity exceeds max_size");

        if (m_size == size)
            return;
        else if (m_size < size)
        {
            if (m_capacity < size)
                reserve(size);

            for (size_type i = m_size; i < size; i++)
            {
                alloc_traits::construct(m_allocator, m_data + i, val);
            }
            m_size = size;
        }
        else
        {
            for (size_type i = size; i < m_size; i++)
            {
                alloc_traits::destroy(m_allocator, m_data + i);
            }
            m_size = size;
        }
    }

    template <typename T, typename Allocator>
    inline void vector<T, Allocator>::swap(vector &other)
    {
        using std::swap;
        swap(m_data, other.m_data);
        swap(m_size, other.m_size);
        swap(m_capacity, other.m_capacity);
        swap(m_allocator, other.m_allocator);
    }

    template <typename T, typename Allocator>
    bool operator==(const vector<T, Allocator> &lhs,
                    const vector<T, Allocator> &rhs)
    {
        if (lhs.size() != rhs.size())
            return false;

        for (std::size_t i = 0; i < lhs.size(); i++)
        {
            if (lhs[i] != rhs[i])
                return false;
        }

        return true;
    }

    template <typename T, typename Allocator>
    bool operator!=(const vector<T, Allocator> &lhs,
                    const vector<T, Allocator> &rhs)
    {
        return !operator==(lhs, rhs);
    }

}; // namespace demo
