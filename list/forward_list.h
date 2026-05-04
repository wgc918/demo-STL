//-----------------------------------------------------------------------------
// 版权所有 (C) 2026 demo-STL 项目
//
// 文件: forward_list.h
// 作者: wgc
// 创建日期: 2026年1月
// 最后修改: 2026年4月
//
// 描述:
//     本文件实现了一个 STL 风格的单向链表容器 (forward_list)。
//     该容器支持高效的前端插入、删除操作，以及各种标准的链表操作。
//
// 功能特性:
//     - 前向迭代器支持
//     - 高效的前端插入/删除
//     - before_begin 迭代器支持在头部之前插入
//     - 合并、拼接、排序等操作
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

#include <functional>
#include <initializer_list>
#include <iterator>
#include <memory>
#include <type_traits>

namespace demo
{
template <typename T, typename Allocator = std::allocator<T>>
class forward_list;
/// @brief 比较两个单向链表是否相等
/// @tparam T 元素类型
/// @tparam Allocator 分配器类型
/// @param lhs 左操作数
/// @param rhs 右操作数
/// @return 如果两个链表相等返回true，否则返回false
template <typename T, typename Allocator>
bool operator==(const forward_list<T, Allocator>& lhs,
                const forward_list<T, Allocator>& rhs);

/// @brief 比较两个单向链表是否不相等
/// @tparam T 元素类型
/// @tparam Allocator 分配器类型
/// @param lhs 左操作数
/// @param rhs 右操作数
/// @return 如果两个链表不相等返回true，否则返回false
template <typename T, typename Allocator>
bool operator!=(const forward_list<T, Allocator>& lhs,
                const forward_list<T, Allocator>& rhs);

/// @brief 单向链表容器类
/// @tparam T 元素类型
/// @tparam Allocator 分配器类型，默认为 std::allocator<T>
template <typename T, typename Allocator>
class forward_list
{
    public:
        using value_type      = T;               ///< 元素类型
        using pointer         = T*;              ///< 元素指针类型
        using const_pointer   = const T*;        ///< 常量元素指针类型
        using reference       = T&;              ///< 元素引用类型
        using const_reference = const T&;        ///< 常量元素引用类型
        using allocator_type  = Allocator;       ///< 分配器类型
        using size_type       = std::size_t;     ///< 大小类型
        using difference_type = std::ptrdiff_t;  ///< 差值类型

    private:
        /// @brief 链表节点结构体
        struct Node
        {
                value_type value;  ///< 节点存储的值
                Node*      next;   ///< 指向下一个节点的指针

                /// @brief 构造函数，使用完美转发构造节点值
                /// @tparam Args 构造参数类型
                /// @param args 构造参数
                template <typename... Args>
                Node(Args&&... args)
                    : value(std::forward<Args>(args)...), next(nullptr)
                {
                }

                ~Node() = default;
        };

    public:
        class const_iterator;

        /// @brief 非 const 迭代器类，支持前向遍历
        class iterator
        {
                friend class forward_list;

            public:
                using iterator_category =
                    std::forward_iterator_tag;           ///< 迭代器类别
                using value_type      = T;               ///< 元素类型
                using difference_type = std::ptrdiff_t;  ///< 差值类型
                using pointer         = T*;              ///< 指针类型
                using reference       = T&;              ///< 引用类型

                /// @brief 默认构造函数，创建空迭代器
                iterator() noexcept;

                /// @brief 构造函数，从节点指针创建迭代器
                /// @param ptr 指向节点的指针
                explicit iterator(Node* ptr) noexcept;

                /// @brief 拷贝构造函数
                /// @param other 要拷贝的迭代器
                iterator(const iterator& other) noexcept;

                /// @brief 解引用操作符，返回当前元素的引用
                /// @return 当前元素的引用
                reference operator*() const noexcept;

                /// @brief 箭头操作符，返回当前元素的指针
                /// @return 当前元素的指针
                pointer operator->() const noexcept;

                /// @brief 前置自增，移动到下一个元素
                /// @return 自增后的迭代器引用
                iterator& operator++() noexcept;

                /// @brief 后置自增，移动到下一个元素
                /// @return 自增前的迭代器副本
                iterator operator++(int) noexcept;

                /// @brief 比较两个迭代器是否相等
                /// @param other 要比较的另一个迭代器
                /// @return 如果相等返回true，否则返回false
                bool operator==(const iterator& other) const noexcept;

                /// @brief 比较两个迭代器是否不相等
                /// @param other 要比较的另一个迭代器
                /// @return 如果不相等返回true，否则返回false
                bool operator!=(const iterator& other) const noexcept;

            private:
                Node* m_ptr;  ///< 指向当前节点的指针
        };

        /// @brief const 迭代器类，支持前向遍历，不能修改元素
        class const_iterator
        {
                friend class forward_list;

            public:
                using iterator_category =
                    std::forward_iterator_tag;           ///< 迭代器类别
                using value_type      = const T;         ///< 常量元素类型
                using difference_type = std::ptrdiff_t;  ///< 差值类型
                using pointer         = const T*;        ///< 常量指针类型
                using reference       = const T&;        ///< 常量引用类型

                /// @brief 默认构造函数，创建空迭代器
                const_iterator() noexcept;

                /// @brief 从非 const 迭代器构造
                /// @param it 非 const 迭代器
                const_iterator(const iterator& it) noexcept;

                /// @brief 构造函数，从节点指针创建迭代器
                /// @param ptr 指向节点的指针
                explicit const_iterator(Node* ptr) noexcept;

                /// @brief 拷贝构造函数
                /// @param other 要拷贝的迭代器
                const_iterator(const const_iterator& other) noexcept;

                /// @brief 解引用操作符，返回当前元素的常量引用
                /// @return 当前元素的常量引用
                reference operator*() const noexcept;

                /// @brief 箭头操作符，返回当前元素的常量指针
                /// @return 当前元素的常量指针
                pointer operator->() const noexcept;

                /// @brief 前置自增，移动到下一个元素
                /// @return 自增后的迭代器引用
                const_iterator& operator++() noexcept;

                /// @brief 后置自增，移动到下一个元素
                /// @return 自增前的迭代器副本
                const_iterator operator++(int) noexcept;

                /// @brief 比较两个 const_iterator 是否相等
                /// @param other 要比较的另一个迭代器
                /// @return 如果相等返回true，否则返回false
                bool operator==(const const_iterator& other) const noexcept;

                /// @brief 与 iterator 比较是否相等
                /// @param other 要比较的 iterator
                /// @return 如果相等返回true，否则返回false
                bool operator==(const iterator& other) const noexcept;

                /// @brief 比较两个 const_iterator 是否不相等
                /// @param other 要比较的另一个迭代器
                /// @return 如果不相等返回true，否则返回false
                bool operator!=(const const_iterator& other) const noexcept;

                /// @brief 与 iterator 比较是否不相等
                /// @param other 要比较的 iterator
                /// @return 如果不相等返回true，否则返回false
                bool operator!=(const iterator& other) const noexcept;

            private:
                Node* m_ptr;  ///< 指向当前节点的指针
        };

        // ============================================
        // 构造函数与析构函数
        // ============================================

        /// @brief 默认构造函数，创建空链表
        forward_list();

        /// @brief 填充构造函数，创建包含count个默认值元素的链表
        /// @param count 元素数量
        explicit forward_list(size_type count);

        /// @brief 填充构造函数，创建包含count个指定值元素的链表
        /// @param count 元素数量
        /// @param val 元素值
        forward_list(size_type count, const_reference val);

        /// @brief 范围构造函数，复制[first, last)范围内的元素
        /// @tparam InputIt 输入迭代器类型
        /// @param first 范围起始迭代器
        /// @param last 范围结束迭代器
        template <typename InputIt,
                  std::enable_if_t<!std::is_integral<InputIt>::value, int> = 0>
        forward_list(InputIt first, InputIt last);

        /// @brief 初始化列表构造函数
        /// @param ilist 初始化列表
        forward_list(std::initializer_list<value_type> ilist);

        /// @brief 拷贝构造函数
        /// @param other 要拷贝的链表
        forward_list(const forward_list& other);

        /// @brief 拷贝赋值运算符
        /// @param other 要拷贝的链表
        /// @return 链表引用
        forward_list& operator=(const forward_list& other);

        /// @brief 初始化列表赋值运算符
        /// @param ilist 初始化列表
        /// @return 链表引用
        forward_list& operator=(std::initializer_list<value_type> ilist);

        /// @brief 移动构造函数
        /// @param other 要移动的链表
        forward_list(forward_list&& other) noexcept;

        /// @brief 移动赋值运算符
        /// @param other 要移动的链表
        /// @return 链表引用
        forward_list& operator=(forward_list&& other) noexcept;

        /// @brief 析构函数，释放所有资源
        ~forward_list();

        // ============================================
        // 赋值操作
        // ============================================

        /// @brief 用count个val替换当前链表内容
        /// @param count 元素数量
        /// @param val 元素值
        void assign(size_type count, const_reference val);

        /// @brief 用[first, last)范围内的元素替换当前链表内容
        /// @tparam InputIt 输入迭代器类型
        /// @param first 范围起始迭代器
        /// @param last 范围结束迭代器
        template <typename InputIt,
                  std::enable_if_t<!std::is_integral<InputIt>::value, int> = 0>
        void assign(InputIt first, InputIt last);

        /// @brief 用初始化列表替换当前链表内容
        /// @param ilist 初始化列表
        void assign(std::initializer_list<value_type> ilist);

        /// @brief 获取分配器
        /// @return 分配器对象
        Allocator get_allocator() const;

        // ============================================
        // 元素访问
        // ============================================

        /// @brief 获取第一个元素的引用
        /// @return 第一个元素的引用
        reference front() noexcept;

        /// @brief 获取第一个元素的常量引用
        /// @return 第一个元素的常量引用
        const_reference front() const noexcept;

        // ============================================
        // 迭代器
        // ============================================

        /// @brief 返回指向第一个元素之前位置的迭代器
        /// @return 指向第一个元素之前的迭代器
        iterator before_begin() noexcept;

        /// @brief 返回指向第一个元素之前位置的 const 迭代器
        /// @return 指向第一个元素之前的 const 迭代器
        const_iterator before_begin() const noexcept;

        /// @brief 返回指向第一个元素之前位置的 const 迭代器（const版本）
        /// @return 指向第一个元素之前的 const 迭代器
        const_iterator cbefore_begin() const noexcept;

        /// @brief 返回指向第一个元素的迭代器
        /// @return 指向第一个元素的迭代器
        iterator begin() noexcept;

        /// @brief 返回指向第一个元素的 const 迭代器
        /// @return 指向第一个元素的 const 迭代器
        const_iterator begin() const noexcept;

        /// @brief 返回指向第一个元素的 const 迭代器（const版本）
        /// @return 指向第一个元素的 const 迭代器
        const_iterator cbegin() const noexcept;

        /// @brief 返回指向末尾的迭代器
        /// @return 指向末尾的迭代器
        iterator end() noexcept;

        /// @brief 返回指向末尾的 const 迭代器
        /// @return 指向末尾的 const 迭代器
        const_iterator end() const noexcept;

        /// @brief 返回指向末尾的 const 迭代器（const版本）
        /// @return 指向末尾的 const 迭代器
        const_iterator cend() const noexcept;

        // ============================================
        // 容量
        // ============================================

        /// @brief 检查链表是否为空
        /// @return 如果链表为空返回true，否则返回false
        bool empty() const noexcept;

        /// @brief 返回链表的最大可能大小
        /// @return 最大可能的元素数量
        size_type max_size() const noexcept;

        // ============================================
        // 修改器
        // ============================================

        /// @brief 清空链表，移除所有元素
        void clear() noexcept;

        /// @brief 在指定位置之后插入一个元素的拷贝
        /// @param pos 插入位置之前的迭代器
        /// @param val 要插入的值
        /// @return 指向新插入元素的迭代器
        iterator insert_after(const_iterator pos, const_reference val);

        /// @brief 在指定位置之后插入一个元素（移动语义）
        /// @param pos 插入位置之前的迭代器
        /// @param val 要插入的值（右值引用）
        /// @return 指向新插入元素的迭代器
        iterator insert_after(const_iterator pos, value_type&& val);

        /// @brief 在指定位置之后插入count个相同元素
        /// @param pos 插入位置之前的迭代器
        /// @param count 要插入的元素数量
        /// @param value 要插入的值
        /// @return 指向最后一个插入元素的迭代器
        iterator insert_after(const_iterator pos, size_type count,
                              const_reference value);

        /// @brief 在指定位置之后插入[first, last)范围内的元素
        /// @tparam InputIt 输入迭代器类型
        /// @param pos 插入位置之前的迭代器
        /// @param first 范围起始迭代器
        /// @param last 范围结束迭代器
        /// @return 指向最后一个插入元素的迭代器
        template <typename InputIt,
                  std::enable_if_t<!std::is_integral<InputIt>::value, int> = 0>
        iterator insert_after(const_iterator pos, InputIt first, InputIt last);

        /// @brief 在指定位置之后插入初始化列表中的元素
        /// @param pos 插入位置之前的迭代器
        /// @param ilist 初始化列表
        /// @return 指向最后一个插入元素的迭代器
        iterator insert_after(const_iterator           pos,
                              std::initializer_list<T> ilist);

        /// @brief 在指定位置之后原地构造一个元素
        /// @tparam Args 构造参数类型
        /// @param pos 插入位置之前的迭代器
        /// @param args 构造参数
        /// @return 指向新构造元素的迭代器
        template <typename... Args>
        iterator emplace_after(const_iterator pos, Args&&... args);

        /// @brief 删除指定位置之后的元素
        /// @param pos 要删除元素之前的迭代器
        /// @return 指向被删除元素之后元素的迭代器
        iterator erase_after(const_iterator pos);

        /// @brief 删除[first, last)范围内的元素
        /// @param first 要删除范围的起始位置之前的迭代器
        /// @param last 要删除范围的结束位置
        /// @return 指向最后一个被删除元素之后元素的迭代器
        iterator erase_after(const_iterator first, const_iterator last);

        /// @brief 在链表头部插入一个元素的拷贝
        /// @param val 要插入的值
        void push_front(const_reference val);

        /// @brief 在链表头部插入一个元素（移动语义）
        /// @param val 要插入的值（右值引用）
        void push_front(value_type&& val);

        /// @brief 在链表头部原地构造一个元素
        /// @tparam Args 构造参数类型
        /// @param args 构造参数
        /// @return 新构造元素的引用
        template <typename... Args>
        reference emplace_front(Args&&... args);

        /// @brief 删除链表头部的元素
        /// @throw std::out_of_range 如果链表为空
        void pop_front();

        /// @brief 调整链表大小
        /// @param count 新的元素数量
        /// @param val 用于填充新元素的值（默认值初始化）
        void resize(size_type count, const_reference val = value_type());

        /// @brief 交换两个链表的内容
        /// @param other 要交换的另一个链表
        void swap(forward_list& other) noexcept;

        // ============================================
        // 操作
        // ============================================

        /// @brief 合并两个已排序的链表（使用默认比较器）
        /// @param other 要合并的另一个链表（左值引用）
        void merge(forward_list& other);

        /// @brief 合并两个已排序的链表（移动语义，使用默认比较器）
        /// @param other 要合并的另一个链表（右值引用）
        void merge(forward_list&& other);

        /// @brief 合并两个已排序的链表（使用自定义比较器）
        /// @tparam Compare 比较函数类型
        /// @param other 要合并的另一个链表（左值引用）
        /// @param comp 比较函数
        template <typename Compare>
        void merge(forward_list& other, Compare comp);

        /// @brief 合并两个已排序的链表（移动语义，使用自定义比较器）
        /// @tparam Compare 比较函数类型
        /// @param other 要合并的另一个链表（右值引用）
        /// @param comp 比较函数
        template <typename Compare>
        void merge(forward_list&& other, Compare comp);

        /// @brief 将另一个链表的所有元素拼接到指定位置之后
        /// @param pos 拼接位置之前的迭代器
        /// @param other 要拼接的另一个链表（左值引用）
        void splice_after(const_iterator pos, forward_list& other);

        /// @brief 将另一个链表的所有元素拼接到指定位置之后（移动语义）
        /// @param pos 拼接位置之前的迭代器
        /// @param other 要拼接的另一个链表（右值引用）
        void splice_after(const_iterator pos, forward_list&& other);

        /// @brief 将另一个链表中指定元素拼接到指定位置之后
        /// @param pos 拼接位置之前的迭代器
        /// @param other 要拼接的另一个链表（左值引用）
        /// @param it 指向要拼接元素之前位置的迭代器
        void splice_after(const_iterator pos, forward_list& other,
                          const_iterator it);

        /// @brief 将另一个链表中指定元素拼接到指定位置之后（移动语义）
        /// @param pos 拼接位置之前的迭代器
        /// @param other 要拼接的另一个链表（右值引用）
        /// @param it 指向要拼接元素之前位置的迭代器
        void splice_after(const_iterator pos, forward_list&& other,
                          const_iterator it);

        /// @brief 将另一个链表中指定范围内的元素拼接到指定位置之后
        /// @param pos 拼接位置之前的迭代器
        /// @param other 要拼接的另一个链表（左值引用）
        /// @param first 要拼接范围的起始位置之前的迭代器
        /// @param last 要拼接范围的结束位置
        void splice_after(const_iterator pos, forward_list& other,
                          const_iterator first, const_iterator last);

        /// @brief 将另一个链表中指定范围内的元素拼接到指定位置之后（移动语义）
        /// @param pos 拼接位置之前的迭代器
        /// @param other 要拼接的另一个链表（右值引用）
        /// @param first 要拼接范围的起始位置之前的迭代器
        /// @param last 要拼接范围的结束位置
        void splice_after(const_iterator pos, forward_list&& other,
                          const_iterator first, const_iterator last);

        /// @brief 移除所有等于指定值的元素
        /// @param value 要移除的值
        /// @return 移除的元素数量
        size_type remove(const T& value);

        /// @brief 移除所有满足条件的元素
        /// @tparam UnaryPredicate 一元谓词类型
        /// @param p 谓词函数，返回true表示移除该元素
        /// @return 移除的元素数量
        template <typename UnaryPredicate>
        size_type remove_if(UnaryPredicate p);

        /// @brief 反转链表中元素的顺序
        void reverse() noexcept;

        /// @brief 移除连续重复的元素（使用默认比较器）
        /// @return 移除的元素数量
        size_type unique();

        /// @brief 移除连续重复的元素（使用自定义比较器）
        /// @tparam BinaryPredicate 二元谓词类型
        /// @param p 谓词函数，判断两个元素是否相等
        /// @return 移除的元素数量
        template <typename BinaryPredicate>
        size_type unique(BinaryPredicate p);

        /// @brief 对链表进行排序（使用默认比较器）
        void sort();

        /// @brief 对链表进行排序（使用自定义比较器）
        /// @tparam Compare 比较函数类型
        /// @param comp 比较函数
        template <typename Compare>
        void sort(Compare comp);

    private:
        /// @brief 将链表按指定长度拆分
        /// @param head 链表头节点
        /// @param n 拆分长度
        /// @return 拆分后第二部分链表的头节点
        Node* split(Node* head, size_type n);

        /// @brief 合并两个已排序的链表（使用默认比较器）
        /// @param la 第一个链表的头节点
        /// @param lb 第二个链表的头节点
        /// @return 合并后链表的头节点
        Node* merge_two(Node* la, Node* lb);

        /// @brief 合并两个已排序的链表（使用自定义比较器）
        /// @tparam Compare 比较函数类型
        /// @param la 第一个链表的头节点
        /// @param lb 第二个链表的头节点
        /// @param comp 比较函数
        /// @return 合并后链表的头节点
        template <typename Compare>
        Node* merge_two(Node* la, Node* lb, Compare comp);

    private:
        /// @brief 节点分配器类型（rebind 到 Node）
        using node_allocator = typename std::allocator_traits<
            Allocator>::template rebind_alloc<Node>;

        node_allocator m_node_alloc;  ///< 节点分配器实例
        Node*          m_head;        ///< 前哨节点（不存储数据）

        using alloc_traits = std::allocator_traits<node_allocator>;

        friend bool operator== <>(const forward_list<T, Allocator>& lhs,
                                  const forward_list<T, Allocator>& rhs);
        friend bool operator!= <>(const forward_list<T, Allocator>& lhs,
                                  const forward_list<T, Allocator>& rhs);
};

// ------------------------------ iterator 实现 ------------------------------
template <typename T, typename Allocator>
inline forward_list<T, Allocator>::iterator::iterator() noexcept
    : m_ptr(nullptr)
{
}

template <typename T, typename Allocator>
inline forward_list<T, Allocator>::iterator::iterator(Node* ptr) noexcept
    : m_ptr(ptr)
{
}

template <typename T, typename Allocator>
inline forward_list<T, Allocator>::iterator::iterator(
    const iterator& other) noexcept
    : m_ptr(other.m_ptr)
{
}

template <typename T, typename Allocator>
inline typename forward_list<T, Allocator>::iterator::reference
forward_list<T, Allocator>::iterator::operator*() const noexcept
{
    return m_ptr->value;
}

template <typename T, typename Allocator>
inline typename forward_list<T, Allocator>::iterator::pointer
forward_list<T, Allocator>::iterator::operator->() const noexcept
{
    return &m_ptr->value;
}

template <typename T, typename Allocator>
inline typename forward_list<T, Allocator>::iterator&
forward_list<T, Allocator>::iterator::operator++() noexcept
{
    m_ptr = m_ptr->next;
    return *this;
}

template <typename T, typename Allocator>
inline typename forward_list<T, Allocator>::iterator
forward_list<T, Allocator>::iterator::operator++(int) noexcept
{
    iterator temp(m_ptr);
    m_ptr = m_ptr->next;
    return temp;
}

template <typename T, typename Allocator>
inline bool forward_list<T, Allocator>::iterator::operator==(
    const iterator& other) const noexcept
{
    return m_ptr == other.m_ptr;
}

template <typename T, typename Allocator>
inline bool forward_list<T, Allocator>::iterator::operator!=(
    const iterator& other) const noexcept
{
    return m_ptr != other.m_ptr;
}

// ------------------------------ const_iterator 实现
// ------------------------------
template <typename T, typename Allocator>
inline forward_list<T, Allocator>::const_iterator::const_iterator() noexcept
    : m_ptr(nullptr)
{
}

template <typename T, typename Allocator>
inline forward_list<T, Allocator>::const_iterator::const_iterator(
    const iterator& it) noexcept
    : m_ptr(it.m_ptr)
{
}

template <typename T, typename Allocator>
inline forward_list<T, Allocator>::const_iterator::const_iterator(
    Node* ptr) noexcept
    : m_ptr(ptr)
{
}

template <typename T, typename Allocator>
inline forward_list<T, Allocator>::const_iterator::const_iterator(
    const const_iterator& other) noexcept
    : m_ptr(other.m_ptr)
{
}

template <typename T, typename Allocator>
inline typename forward_list<T, Allocator>::const_iterator::reference
forward_list<T, Allocator>::const_iterator::operator*() const noexcept
{
    return m_ptr->value;
}

template <typename T, typename Allocator>
inline typename forward_list<T, Allocator>::const_iterator::pointer
forward_list<T, Allocator>::const_iterator::operator->() const noexcept
{
    return &m_ptr->value;
}

template <typename T, typename Allocator>
inline typename forward_list<T, Allocator>::const_iterator&
forward_list<T, Allocator>::const_iterator::operator++() noexcept
{
    m_ptr = m_ptr->next;
    return *this;
}

template <typename T, typename Allocator>
inline typename forward_list<T, Allocator>::const_iterator
forward_list<T, Allocator>::const_iterator::operator++(int) noexcept
{
    const_iterator temp(m_ptr);
    m_ptr = m_ptr->next;
    return temp;
}

template <typename T, typename Allocator>
inline bool forward_list<T, Allocator>::const_iterator::operator==(
    const const_iterator& other) const noexcept
{
    return m_ptr == other.m_ptr;
}

template <typename T, typename Allocator>
inline bool forward_list<T, Allocator>::const_iterator::operator==(
    const iterator& other) const noexcept
{
    return m_ptr == other.m_ptr;
}

template <typename T, typename Allocator>
inline bool forward_list<T, Allocator>::const_iterator::operator!=(
    const const_iterator& other) const noexcept
{
    return m_ptr != other.m_ptr;
}

template <typename T, typename Allocator>
inline bool forward_list<T, Allocator>::const_iterator::operator!=(
    const iterator& other) const noexcept
{
    return m_ptr != other.m_ptr;
}

// ------------------------------ forward_list 实现
// ------------------------------

template <typename T, typename Allocator>
inline forward_list<T, Allocator>::forward_list()
{
    m_head = alloc_traits::allocate(m_node_alloc, 1);
    alloc_traits::construct(m_node_alloc, m_head);
}

template <typename T, typename Allocator>
inline forward_list<T, Allocator>::forward_list(size_type count)
{
    if (count > max_size())
        throw std::length_error("forward_list count exceeds max_size");

    m_head = alloc_traits::allocate(m_node_alloc, 1);
    alloc_traits::construct(m_node_alloc, m_head);

    Node* cur = m_head;
    for (size_type i = 0; i < count; i++)
    {
        cur->next = alloc_traits::allocate(m_node_alloc, 1);
        alloc_traits::construct(m_node_alloc, cur->next);
        cur = cur->next;
    }
}

template <typename T, typename Allocator>
inline forward_list<T, Allocator>::forward_list(size_type       count,
                                                const_reference val)
{
    if (count > max_size())
        throw std::length_error("forward_list count exceeds max_size");

    m_head = alloc_traits::allocate(m_node_alloc, 1);
    alloc_traits::construct(m_node_alloc, m_head);

    Node* cur = m_head;
    for (size_type i = 0; i < count; i++)
    {
        cur->next = alloc_traits::allocate(m_node_alloc, 1);
        alloc_traits::construct(m_node_alloc, cur->next, val);
        cur = cur->next;
    }
}

template <typename T, typename Allocator>
template <typename InputIt,
          std::enable_if_t<!std::is_integral<InputIt>::value, int>>
inline forward_list<T, Allocator>::forward_list(InputIt first, InputIt last)
{
    m_head = alloc_traits::allocate(m_node_alloc, 1);
    alloc_traits::construct(m_node_alloc, m_head);

    Node* cur = m_head;
    for (; first != last; first++)
    {
        cur->next = alloc_traits::allocate(m_node_alloc, 1);
        alloc_traits::construct(m_node_alloc, cur->next, *first);
        cur = cur->next;
    }
}

template <typename T, typename Allocator>
inline forward_list<T, Allocator>::forward_list(
    std::initializer_list<value_type> ilist)
{
    m_head = alloc_traits::allocate(m_node_alloc, 1);
    alloc_traits::construct(m_node_alloc, m_head);

    Node* cur = m_head;
    for (auto& val : ilist)
    {
        cur->next = alloc_traits::allocate(m_node_alloc, 1);
        alloc_traits::construct(m_node_alloc, cur->next, val);
        cur = cur->next;
    }
}

template <typename T, typename Allocator>
inline forward_list<T, Allocator>::forward_list(const forward_list& other)
{
    m_head = alloc_traits::allocate(m_node_alloc, 1);
    alloc_traits::construct(m_node_alloc, m_head);

    Node* cur = m_head;
    for (Node* i = other.m_head->next; i != nullptr; i = i->next)
    {
        cur->next = alloc_traits::allocate(m_node_alloc, 1);
        alloc_traits::construct(m_node_alloc, cur->next, i->value);
        cur = cur->next;
    }
}

template <typename T, typename Allocator>
inline forward_list<T, Allocator>& forward_list<T, Allocator>::operator=(
    const forward_list& other)
{
    if (this == &other)
        return *this;

    // 释放原对象的所有节点
    clear();

    Node* cur = m_head;
    for (Node* i = other.m_head->next; i != nullptr; i = i->next)
    {
        cur->next = alloc_traits::allocate(m_node_alloc, 1);
        alloc_traits::construct(m_node_alloc, cur->next, i->value);
        cur = cur->next;
    }

    return *this;
}

template <typename T, typename Allocator>
inline forward_list<T, Allocator>& forward_list<T, Allocator>::operator=(
    std::initializer_list<value_type> ilist)
{
    // 释放原对象的所有节点
    clear();

    Node* cur = m_head;
    for (auto& val : ilist)
    {
        cur->next = alloc_traits::allocate(m_node_alloc, 1);
        alloc_traits::construct(m_node_alloc, cur->next, val);
        cur = cur->next;
    }

    return *this;
}

template <typename T, typename Allocator>
inline forward_list<T, Allocator>::forward_list(forward_list&& other) noexcept
    : m_node_alloc(std::move(other.m_node_alloc))
{
    m_head = other.m_head;

    // 为原对象重新创建一个前哨节点
    other.m_head = alloc_traits::allocate(m_node_alloc, 1);
    alloc_traits::construct(m_node_alloc, other.m_head);
}

template <typename T, typename Allocator>
inline forward_list<T, Allocator>& forward_list<T, Allocator>::operator=(
    forward_list&& other) noexcept
{
    if (this == &other)
        return *this;

    clear();

    m_node_alloc = std::move(other.m_node_alloc);
    m_head       = other.m_head;

    // 为原对象重新创建一个前哨节点
    other.m_head = alloc_traits::allocate(m_node_alloc, 1);
    alloc_traits::construct(m_node_alloc, other.m_head);

    return *this;
}

template <typename T, typename Allocator>
inline forward_list<T, Allocator>::~forward_list()
{
    clear();
    if (m_head != nullptr)
    {
        alloc_traits::destroy(m_node_alloc, m_head);
        alloc_traits::deallocate(m_node_alloc, m_head, 1);
        m_head = nullptr;
    }
}

template <typename T, typename Allocator>
inline void forward_list<T, Allocator>::assign(size_type       count,
                                               const_reference val)
{
    clear();

    Node* cur = m_head;
    for (size_type i = 0; i < count; i++)
    {
        cur->next = alloc_traits::allocate(m_node_alloc, 1);
        alloc_traits::construct(m_node_alloc, cur->next, val);
        cur = cur->next;
    }
}

template <typename T, typename Allocator>
template <typename InputIt,
          std::enable_if_t<!std::is_integral<InputIt>::value, int>>
inline void forward_list<T, Allocator>::assign(InputIt first, InputIt last)
{
    clear();

    Node* cur = m_head;
    for (; first != last; first++)
    {
        cur->next = alloc_traits::allocate(m_node_alloc, 1);
        alloc_traits::construct(m_node_alloc, cur->next, *first);
        cur = cur->next;
    }
}

template <typename T, typename Allocator>
inline void forward_list<T, Allocator>::assign(
    std::initializer_list<value_type> ilist)
{
    clear();

    Node* cur = m_head;
    for (auto& val : ilist)
    {
        cur->next = alloc_traits::allocate(m_node_alloc, 1);
        alloc_traits::construct(m_node_alloc, cur->next, val);
        cur = cur->next;
    }
}

template <typename T, typename Allocator>
inline Allocator forward_list<T, Allocator>::get_allocator() const
{
    using elem_allocator = typename std::allocator_traits<
        node_allocator>::template rebind_alloc<T>;
    return elem_allocator(m_node_alloc);
}

template <typename T, typename Allocator>
inline typename forward_list<T, Allocator>::reference
forward_list<T, Allocator>::front() noexcept
{
    return m_head->next->value;
}

template <typename T, typename Allocator>
inline typename forward_list<T, Allocator>::const_reference
forward_list<T, Allocator>::front() const noexcept
{
    return m_head->next->value;
}

template <typename T, typename Allocator>
inline typename forward_list<T, Allocator>::iterator
forward_list<T, Allocator>::before_begin() noexcept
{
    return iterator(m_head);
}

template <typename T, typename Allocator>
inline typename forward_list<T, Allocator>::const_iterator
forward_list<T, Allocator>::before_begin() const noexcept
{
    return const_iterator(m_head);
}

template <typename T, typename Allocator>
inline typename forward_list<T, Allocator>::const_iterator
forward_list<T, Allocator>::cbefore_begin() const noexcept
{
    return const_iterator(m_head);
}

template <typename T, typename Allocator>
inline typename forward_list<T, Allocator>::iterator
forward_list<T, Allocator>::begin() noexcept
{
    return iterator(m_head->next);
}

template <typename T, typename Allocator>
inline typename forward_list<T, Allocator>::const_iterator
forward_list<T, Allocator>::begin() const noexcept
{
    return const_iterator(m_head->next);
}

template <typename T, typename Allocator>
inline typename forward_list<T, Allocator>::const_iterator
forward_list<T, Allocator>::cbegin() const noexcept
{
    return const_iterator(m_head->next);
}

template <typename T, typename Allocator>
inline typename forward_list<T, Allocator>::iterator
forward_list<T, Allocator>::end() noexcept
{
    return iterator(nullptr);
}

template <typename T, typename Allocator>
inline typename forward_list<T, Allocator>::const_iterator
forward_list<T, Allocator>::end() const noexcept
{
    return const_iterator(nullptr);
}

template <typename T, typename Allocator>
inline typename forward_list<T, Allocator>::const_iterator
forward_list<T, Allocator>::cend() const noexcept
{
    return const_iterator(nullptr);
}

template <typename T, typename Allocator>
inline bool forward_list<T, Allocator>::empty() const noexcept
{
    return m_head->next == nullptr;
}

template <typename T, typename Allocator>
inline typename forward_list<T, Allocator>::size_type
forward_list<T, Allocator>::max_size() const noexcept
{
    return alloc_traits::max_size(m_node_alloc);
}

template <typename T, typename Allocator>
inline void forward_list<T, Allocator>::clear() noexcept
{
    Node* cur = m_head->next;
    while (cur != nullptr)
    {
        Node* next = cur->next;
        alloc_traits::destroy(m_node_alloc, cur);
        alloc_traits::deallocate(m_node_alloc, cur, 1);
        cur = next;
    }
    m_head->next = nullptr;
}

template <typename T, typename Allocator>
inline typename forward_list<T, Allocator>::iterator
forward_list<T, Allocator>::insert_after(const_iterator  pos,
                                         const_reference val)
{
    Node* cur  = pos.m_ptr;
    Node* next = cur->next;

    cur->next = alloc_traits::allocate(m_node_alloc, 1);
    alloc_traits::construct(m_node_alloc, cur->next, val);

    cur       = cur->next;
    cur->next = next;

    return iterator(cur);
}

template <typename T, typename Allocator>
inline typename forward_list<T, Allocator>::iterator
forward_list<T, Allocator>::insert_after(const_iterator pos, value_type&& val)
{
    Node* cur  = pos.m_ptr;
    Node* next = cur->next;

    cur->next = alloc_traits::allocate(m_node_alloc, 1);
    alloc_traits::construct(m_node_alloc, cur->next, std::move(val));

    cur       = cur->next;
    cur->next = next;

    return iterator(cur);
}

template <typename T, typename Allocator>
inline typename forward_list<T, Allocator>::iterator
forward_list<T, Allocator>::insert_after(const_iterator pos, size_type count,
                                         const_reference val)
{
    Node* cur  = pos.m_ptr;
    Node* next = cur->next;

    for (size_type i = 0; i < count; i++)
    {
        cur->next = alloc_traits::allocate(m_node_alloc, 1);
        alloc_traits::construct(m_node_alloc, cur->next, val);

        cur = cur->next;
    }
    cur->next = next;

    return iterator(cur);
}

template <typename T, typename Allocator>
template <typename InputIt,
          std::enable_if_t<!std::is_integral<InputIt>::value, int>>
inline typename forward_list<T, Allocator>::iterator
forward_list<T, Allocator>::insert_after(const_iterator pos, InputIt first,
                                         InputIt last)
{
    Node* cur  = pos.m_ptr;
    Node* next = cur->next;

    for (; first != last; first++)
    {
        cur->next = alloc_traits::allocate(m_node_alloc, 1);
        alloc_traits::construct(m_node_alloc, cur->next, *first);

        cur = cur->next;
    }
    cur->next = next;

    return iterator(cur);
}

template <typename T, typename Allocator>
inline typename forward_list<T, Allocator>::iterator
forward_list<T, Allocator>::insert_after(const_iterator           pos,
                                         std::initializer_list<T> ilist)
{
    Node* cur  = pos.m_ptr;
    Node* next = cur->next;

    for (auto& val : ilist)
    {
        cur->next = alloc_traits::allocate(m_node_alloc, 1);
        alloc_traits::construct(m_node_alloc, cur->next, val);

        cur = cur->next;
    }
    cur->next = next;

    return iterator(cur);
}

template <typename T, typename Allocator>
template <typename... Args>
inline typename forward_list<T, Allocator>::iterator
forward_list<T, Allocator>::emplace_after(const_iterator pos, Args&&... args)
{
    Node* cur  = pos.m_ptr;
    Node* next = cur->next;

    cur->next = alloc_traits::allocate(m_node_alloc, 1);
    alloc_traits::construct(m_node_alloc, cur->next,
                            std::forward<Args>(args)...);

    cur       = cur->next;
    cur->next = next;

    return iterator(cur);
}

template <typename T, typename Allocator>
inline typename forward_list<T, Allocator>::iterator
forward_list<T, Allocator>::erase_after(const_iterator pos)
{
    if (pos == end())
        return end();

    Node* cur      = pos.m_ptr;
    Node* del_node = cur->next;

    if (del_node == nullptr)
        return end();

    Node* next = del_node->next;
    cur->next  = next;

    alloc_traits::destroy(m_node_alloc, del_node);
    alloc_traits::deallocate(m_node_alloc, del_node, 1);

    return iterator(next);
}

template <typename T, typename Allocator>
inline typename forward_list<T, Allocator>::iterator
forward_list<T, Allocator>::erase_after(const_iterator first,
                                        const_iterator last)
{
    Node* pre = first.m_ptr;
    Node* cur = pre->next;

    while (cur != last.m_ptr)
    {
        Node* next = cur->next;
        alloc_traits::destroy(m_node_alloc, cur);
        alloc_traits::deallocate(m_node_alloc, cur, 1);
        cur = next;
    }

    pre->next = last.m_ptr;

    return iterator(last.m_ptr);
}

template <typename T, typename Allocator>
inline void forward_list<T, Allocator>::push_front(const_reference val)
{
    Node* next   = m_head->next;
    m_head->next = alloc_traits::allocate(m_node_alloc, 1);
    alloc_traits::construct(m_node_alloc, m_head->next, val);
    m_head->next->next = next;
}

template <typename T, typename Allocator>
inline void forward_list<T, Allocator>::push_front(value_type&& val)
{
    Node* next   = m_head->next;
    m_head->next = alloc_traits::allocate(m_node_alloc, 1);
    alloc_traits::construct(m_node_alloc, m_head->next, std::move(val));
    m_head->next->next = next;
}

template <typename T, typename Allocator>
template <typename... Args>
inline typename forward_list<T, Allocator>::reference
forward_list<T, Allocator>::emplace_front(Args&&... args)
{
    Node* next   = m_head->next;
    m_head->next = alloc_traits::allocate(m_node_alloc, 1);
    alloc_traits::construct(m_node_alloc, m_head->next,
                            std::forward<Args>(args)...);
    m_head->next->next = next;

    return m_head->next->value;
}

template <typename T, typename Allocator>
inline void forward_list<T, Allocator>::pop_front()
{
    if (empty())
        throw std::out_of_range(
            "forward_list::pop_front:\
                 empty forward_list");

    Node* del_node = m_head->next;
    Node* next     = del_node->next;

    alloc_traits::destroy(m_node_alloc, del_node);
    alloc_traits::deallocate(m_node_alloc, del_node, 1);

    m_head->next = next;
}

template <typename T, typename Allocator>
inline void forward_list<T, Allocator>::resize(size_type       count,
                                               const_reference val)
{
    size_type size = std::distance(begin(), end());

    if (size == count)
        return;
    else if (size > count)
    {
        Node* cur = m_head;
        for (int i = 0; i < count; i++)
            cur = cur->next;

        Node* del_node = cur->next;
        cur->next      = nullptr;
        while (del_node != nullptr)
        {
            Node* next = del_node->next;
            alloc_traits::destroy(m_node_alloc, del_node);
            alloc_traits::deallocate(m_node_alloc, del_node, 1);
            del_node = next;
        }
    }
    else
    {
        Node* cur = m_head;
        for (size_type i = 0; i < size; i++)
            cur = cur->next;

        while (size != count)
        {
            cur->next = alloc_traits::allocate(m_node_alloc, 1);
            alloc_traits::construct(m_node_alloc, cur->next, val);
            cur = cur->next;
            size++;
        }
    }
}

template <typename T, typename Allocator>
inline void forward_list<T, Allocator>::swap(forward_list& other) noexcept
{
    using std::swap;
    swap(m_head, other.m_head);
    swap(m_node_alloc, other.m_node_alloc);
}

template <typename T, typename Allocator>
inline void forward_list<T, Allocator>::merge(forward_list& other)
{
    // if (this == &other)
    //     return;

    // if (other.empty())
    //     return;

    // if (this->empty())
    // {
    //     m_head->next = other.m_head->next;
    //     other.m_head->next = nullptr;
    //     return;
    // }

    // using Comp = std::less<>;
    // Comp comp;

    // Node *prev = m_head;
    // Node *curr = m_head->next;
    // Node *other_curr = other.m_head->next;
    // other.m_head->next = nullptr;

    // while (curr != nullptr && other_curr != nullptr)
    // {
    //     if (comp(curr->value, other_curr->value))
    //     {
    //         prev = curr;
    //         curr = curr->next;
    //     }
    //     else
    //     {
    //         prev->next = other_curr;
    //         other_curr = other_curr->next;
    //         prev->next->next = curr;
    //         prev = prev->next;
    //     }
    // }

    // if (other_curr != nullptr)
    //     prev->next = other_curr;

    merge(other, std::less<>());
}

template <typename T, typename Allocator>
inline void forward_list<T, Allocator>::merge(forward_list&& other)
{
    merge(other);
}

template <typename T, typename Allocator>
template <typename Compare>
inline void forward_list<T, Allocator>::merge(forward_list& other, Compare comp)
{
    if (this == &other)
        return;

    if (other.empty())
        return;

    if (this->empty())
    {
        m_head->next       = other.m_head->next;
        other.m_head->next = nullptr;
        return;
    }

    Node* prev         = m_head;
    Node* curr         = m_head->next;
    Node* other_curr   = other.m_head->next;
    other.m_head->next = nullptr;

    while (curr != nullptr && other_curr != nullptr)
    {
        if (comp(curr->value, other_curr->value))
        {
            prev = curr;
            curr = curr->next;
        }
        else
        {
            prev->next       = other_curr;
            other_curr       = other_curr->next;
            prev->next->next = curr;
            prev             = prev->next;
        }
    }

    if (other_curr != nullptr)
        prev->next = other_curr;
}

template <typename T, typename Allocator>
template <typename Compare>
inline void forward_list<T, Allocator>::merge(forward_list&& other,
                                              Compare        comp)
{
    merge(other, comp);
}

template <typename T, typename Allocator>
inline void forward_list<T, Allocator>::splice_after(const_iterator pos,
                                                     forward_list&  other)
{
    if (this == &other || other.empty())
        return;

    Node* cur  = pos.m_ptr;
    Node* next = cur->next;
    Node* tail = other.m_head->next;

    while (tail->next != nullptr)
        tail = tail->next;

    cur->next  = other.m_head->next;
    tail->next = next;

    other.m_head->next = nullptr;
}

template <typename T, typename Allocator>
inline void forward_list<T, Allocator>::splice_after(const_iterator pos,
                                                     forward_list&& other)
{
    splice_after(pos, other);
}

template <typename T, typename Allocator>
inline void forward_list<T, Allocator>::splice_after(const_iterator pos,
                                                     forward_list&  other,
                                                     const_iterator it)
{
    if (this == &other || it.m_ptr == nullptr || it.m_ptr->next == nullptr)
        return;

    Node* cur  = pos.m_ptr;
    Node* next = cur->next;
    Node* node = it.m_ptr->next;

    it.m_ptr->next = node->next;

    cur->next  = node;
    node->next = next;
}

template <typename T, typename Allocator>
inline void forward_list<T, Allocator>::splice_after(const_iterator pos,
                                                     forward_list&& other,
                                                     const_iterator it)
{
    splice_after(pos, other, it);
}

template <typename T, typename Allocator>
inline void forward_list<T, Allocator>::splice_after(const_iterator pos,
                                                     forward_list&  other,
                                                     const_iterator first,
                                                     const_iterator last)
{
    if (this == &other || other.empty())
        return;

    Node* cur  = pos.m_ptr;
    Node* next = cur->next;
    Node* head = first.m_ptr->next;
    Node* tail = head;

    if (head == nullptr)
        return;

    while (tail->next != last.m_ptr)
        tail = tail->next;

    cur->next  = head;
    tail->next = next;

    first.m_ptr->next = last.m_ptr;
}

template <typename T, typename Allocator>
inline void forward_list<T, Allocator>::splice_after(const_iterator pos,
                                                     forward_list&& other,
                                                     const_iterator first,
                                                     const_iterator last)
{
    splice_after(pos, other, first, last);
}

template <typename T, typename Allocator>
inline typename forward_list<T, Allocator>::size_type
forward_list<T, Allocator>::remove(const T& value)
{
    // size_type count = 0;
    // Node *pre = m_head;
    // Node *cur = m_head->next;
    // while (cur != nullptr)
    // {
    //     if (cur->value == value)
    //     {
    //         Node *next = cur->next;
    //         pre->next = cur->next;
    //         alloc_traits::destroy(m_node_alloc, cur);
    //         alloc_traits::deallocate(m_node_alloc, cur, 1);

    //         cur = next;
    //         count++;
    //     }
    //     else
    //     {
    //         pre = pre->next;
    //         cur = cur->next;
    //     }
    // }

    // return count;

    return remove_if([&](const T& val) { return val == value; });
}

template <typename T, typename Allocator>
template <typename UnaryPredicate>
inline typename forward_list<T, Allocator>::size_type
forward_list<T, Allocator>::remove_if(UnaryPredicate p)
{
    size_type count = 0;
    Node*     pre   = m_head;
    Node*     cur   = m_head->next;
    while (cur != nullptr)
    {
        if (p(cur->value))
        {
            Node* next = cur->next;
            pre->next  = cur->next;
            alloc_traits::destroy(m_node_alloc, cur);
            alloc_traits::deallocate(m_node_alloc, cur, 1);

            cur = next;
            count++;
        }
        else
        {
            pre = pre->next;
            cur = cur->next;
        }
    }

    return count;
}

template <typename T, typename Allocator>
inline void forward_list<T, Allocator>::reverse() noexcept
{
    if (empty())
        return;

    Node* pre = m_head->next;
    Node* cur = pre->next;
    pre->next = nullptr;  // 将第一个节点置为最后一个节点

    while (cur != nullptr)
    {
        Node* next = cur->next;
        cur->next  = pre;

        pre = cur;
        cur = next;
    }
    m_head->next = pre;
}

template <typename T, typename Allocator>
inline typename forward_list<T, Allocator>::size_type
forward_list<T, Allocator>::unique()
{
    // if (empty())
    //     return 0;

    // size_type count = 0;
    // Node *pre = m_head->next;
    // Node *cur = pre->next;
    // while (cur != nullptr)
    // {
    //     if (pre->value == cur->value)
    //     {
    //         Node *next = cur->next;
    //         pre->next = next;

    //         alloc_traits::destroy(m_node_alloc, cur);
    //         alloc_traits::deallocate(m_node_alloc, cur, 1);

    //         cur = next;
    //         count++;
    //     }
    //     else
    //     {
    //         pre = pre->next;
    //         cur = cur->next;
    //     }
    // }

    // return count;

    return unique(std::equal_to<>());
}

template <typename T, typename Allocator>
template <typename BinaryPredicate>
inline typename forward_list<T, Allocator>::size_type
forward_list<T, Allocator>::unique(BinaryPredicate p)
{
    if (empty())
        return 0;

    size_type count = 0;
    Node*     pre   = m_head->next;
    Node*     cur   = pre->next;
    while (cur != nullptr)
    {
        if (p(pre->value, cur->value))
        {
            Node* next = cur->next;
            pre->next  = next;

            alloc_traits::destroy(m_node_alloc, cur);
            alloc_traits::deallocate(m_node_alloc, cur, 1);

            cur = next;
            count++;
        }
        else
        {
            pre = pre->next;
            cur = cur->next;
        }
    }

    return count;
}

template <typename T, typename Allocator>
inline void forward_list<T, Allocator>::sort()
{
    std::less<> comp;
    sort(comp);
}

template <typename T, typename Allocator>
template <typename Compare>
inline void forward_list<T, Allocator>::sort(Compare comp)
{
    if (empty() || m_head->next->next == nullptr)
        return;

    Node* head  = m_head->next;
    Node* cur   = nullptr;
    Node* left  = nullptr;
    Node* right = nullptr;

    size_type len = 0;
    cur           = head;
    while (cur != nullptr)
    {
        len++;
        cur = cur->next;
    }

    Node* temp = alloc_traits::allocate(m_node_alloc, 1);  // 临时前哨节点
    alloc_traits::construct(m_node_alloc, temp);

    for (size_type i = 1; i < len; i *= 2)
    {
        cur        = head;
        Node* tail = temp;

        while (cur != nullptr)
        {
            left       = cur;
            right      = split(left, i);
            cur        = split(right, i);
            tail->next = merge_two(left, right, comp);

            while (tail->next != nullptr)
                tail = tail->next;
        }

        head = temp->next;
    }

    alloc_traits::destroy(m_node_alloc, temp);
    alloc_traits::deallocate(m_node_alloc, temp, 1);

    m_head->next = head;
}

template <typename T, typename Allocator>
inline typename forward_list<T, Allocator>::Node*
forward_list<T, Allocator>::split(Node* head, size_type n)
{
    if (head == nullptr)
        return nullptr;

    Node* cur = head;
    // 向前走 n-1 步（拆分出前 n 个节点）
    for (size_type i = 1; i < n && cur->next != nullptr; i++)
        cur = cur->next;

    Node* next_part = cur->next;
    cur->next       = nullptr;

    return next_part;
}

template <typename T, typename Allocator>
inline typename forward_list<T, Allocator>::Node*
forward_list<T, Allocator>::merge_two(Node* la, Node* lb)
{
    std::less<> comp;
    return merge_two(la, lb, comp);
}

template <typename T, typename Allocator>
template <typename Compare>
inline typename forward_list<T, Allocator>::Node*
forward_list<T, Allocator>::merge_two(Node* la, Node* lb, Compare comp)
{
    if (la == nullptr)
        return lb;
    if (lb == nullptr)
        return la;
    if (la == lb)
        return la;

    Node* cur_a = la;
    Node* cur_b = lb;
    Node* temp  = alloc_traits::allocate(m_node_alloc,
                                         1);  // 创建一个临时前哨节点用于合并
    alloc_traits::construct(m_node_alloc, temp);
    temp->next = la;
    Node* pre  = temp;

    while (cur_a != nullptr && cur_b != nullptr)
    {
        if (comp(cur_a->value, cur_b->value))
        {
            pre   = cur_a;
            cur_a = cur_a->next;
        }
        else
        {
            Node* next   = pre->next;
            Node* next_b = cur_b->next;

            pre->next   = cur_b;
            cur_b->next = next;

            pre   = pre->next;
            cur_b = next_b;
        }
    }

    if (cur_b != nullptr)
        pre->next = cur_b;

    Node* res = temp->next;
    alloc_traits::destroy(m_node_alloc, temp);
    alloc_traits::deallocate(m_node_alloc, temp, 1);

    return res;
}

template <typename T, typename Allocator>
bool operator==(const forward_list<T, Allocator>& lhs,
                const forward_list<T, Allocator>& rhs)
{
    typename forward_list<T, Allocator>::Node* s_lhs = lhs.m_head->next;
    typename forward_list<T, Allocator>::Node* s_rhs = rhs.m_head->next;

    for (; s_lhs != nullptr && s_rhs != nullptr;
         s_lhs = s_lhs->next, s_rhs = s_rhs->next)
    {
        if (s_lhs->value != s_rhs->value)
            return false;
    }

    return (s_lhs == nullptr && s_rhs == nullptr);
}

template <typename T, typename Allocator>
bool operator!=(const forward_list<T, Allocator>& lhs,
                const forward_list<T, Allocator>& rhs)
{
    return !(lhs == rhs);
}

}  // namespace demo
