//-----------------------------------------------------------------------------
// 版权所有 (C) 2026 demo-STL 项目
//
// 文件: list.h
// 作者: wgc
// 创建日期: 2026年1月
// 最后修改: 2026年4月
//
// 描述:
//     本文件实现了一个 STL 风格的双向链表容器 (list)。
//     该容器支持高效的插入、删除操作，以及各种标准的链表操作。
//
// 功能特性:
//     - 双向迭代器支持
//     - 高效的任意位置插入/删除
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

#include <initializer_list>
#include <iterator>
#include <memory>
#include <type_traits>

namespace demo
{
template <typename T, typename Allocator = std::allocator<T>>
class list;

/// @brief 比较两个链表是否相等
/// @tparam T 元素类型
/// @tparam Allocator 分配器类型
/// @param lhs 左操作数
/// @param rhs 右操作数
/// @return 如果两个链表相等返回true，否则返回false
template <typename T, typename Allocator>
bool operator==(const list<T, Allocator>& lhs, const list<T, Allocator>& rhs);

/// @brief 比较两个链表是否不相等
/// @tparam T 元素类型
/// @tparam Allocator 分配器类型
/// @param lhs 左操作数
/// @param rhs 右操作数
/// @return 如果两个链表不相等返回true，否则返回false
template <typename T, typename Allocator>
bool operator!=(const list<T, Allocator>& lhs, const list<T, Allocator>& rhs);

template <typename T, typename Allocator>
class list
{
    friend bool operator==
        <>(const list<T, Allocator>& lhs, const list<T, Allocator>& rhs);
    friend bool operator!=
        <>(const list<T, Allocator>& lhs, const list<T, Allocator>& rhs);

public:
    using value_type      = T;               ///< 元素类型
    using allocator_type  = Allocator;       ///< 分配器类型
    using reference       = T&;              ///< 元素引用类型
    using const_reference = const T&;        ///< 常量元素引用类型
    using pointer         = T*;              ///< 元素指针类型
    using const_pointr    = const T*;        ///< 常量元素指针类型
    using size_type       = std::size_t;     ///< 大小类型
    using difference_type = std::ptrdiff_t;  ///< 差值类型

private:
    /// @brief 链表节点结构体
    struct Node
    {
        value_type val;   ///< 节点存储的值
        Node*      prev;  ///< 指向前一个节点的指针
        Node*      next;  ///< 指向后一个节点的指针

        /// @brief 构造函数，使用完美转发构造节点值
        /// @tparam Args 构造参数类型
        /// @param args 构造参数
        template <typename... Args>
        Node(Args&&... args)
            : val(std::forward<Args>(args)...), prev(nullptr), next(nullptr)
        {
        }
    };

public:
    class const_iterator;

    /// @brief 双向迭代器类
    class iterator
    {
        friend class list;

    public:
        using iterator_category =
            std::bidirectional_iterator_tag;     ///< 迭代器类别
        using value_type      = T;               ///< 元素类型
        using pointer         = T*;              ///< 元素指针类型
        using reference       = T&;              ///< 元素引用类型
        using difference_type = std::ptrdiff_t;  ///< 差值类型

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

        /// @brief 前置自减，移动到前一个元素
        /// @return 自减后的迭代器引用
        iterator& operator--() noexcept;

        /// @brief 后置自减，移动到前一个元素
        /// @return 自减前的迭代器副本
        iterator operator--(int) noexcept;

        /// @brief 比较两个迭代器是否相等
        /// @param other 要比较的另一个迭代器
        /// @return 如果相等返回true，否则返回false
        bool operator==(const iterator other) const noexcept;

        /// @brief 比较两个迭代器是否不相等
        /// @param other 要比较的另一个迭代器
        /// @return 如果不相等返回true，否则返回false
        bool operator!=(const iterator other) const noexcept;

        /// @brief 与const_iterator比较是否相等
        /// @param other 要比较的const_iterator
        /// @return 如果相等返回true，否则返回false
        bool operator==(const const_iterator other) const noexcept;

        /// @brief 与const_iterator比较是否不相等
        /// @param other 要比较的const_iterator
        /// @return 如果不相等返回true，否则返回false
        bool operator!=(const const_iterator other) const noexcept;

    private:
        Node* m_ptr;  ///< 指向当前节点的指针
    };

    /// @brief 常量双向迭代器类
    class const_iterator
    {
        friend class list;

    public:
        using iterator_category =
            std::bidirectional_iterator_tag;     ///< 迭代器类别
        using value_type      = T;               ///< 元素类型
        using pointer         = const T*;        ///< 常量元素指针类型
        using reference       = const T&;        ///< 常量元素引用类型
        using difference_type = std::ptrdiff_t;  ///< 差值类型

        /// @brief 默认构造函数，创建空迭代器
        const_iterator() noexcept;

        /// @brief 构造函数，从节点指针创建迭代器
        /// @param ptr 指向节点的指针
        explicit const_iterator(Node* ptr) noexcept;

        /// @brief 从非const迭代器构造
        /// @param it 非const迭代器
        const_iterator(iterator it) noexcept;

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

        /// @brief 前置自减，移动到前一个元素
        /// @return 自减后的迭代器引用
        const_iterator& operator--() noexcept;

        /// @brief 后置自减，移动到前一个元素
        /// @return 自减前的迭代器副本
        const_iterator operator--(int) noexcept;

        /// @brief 比较两个const_iterator是否相等
        /// @param other 要比较的另一个迭代器
        /// @return 如果相等返回true，否则返回false
        bool operator==(const const_iterator other) const noexcept;

        /// @brief 比较两个const_iterator是否不相等
        /// @param other 要比较的另一个迭代器
        /// @return 如果不相等返回true，否则返回false
        bool operator!=(const const_iterator other) const noexcept;

        /// @brief 与iterator比较是否相等
        /// @param other 要比较的iterator
        /// @return 如果相等返回true，否则返回false
        bool operator==(const iterator other) const noexcept;

        /// @brief 与iterator比较是否不相等
        /// @param other 要比较的iterator
        /// @return 如果不相等返回true，否则返回false
        bool operator!=(const iterator other) const noexcept;

    private:
        Node* m_ptr;  ///< 指向当前节点的指针
    };

    /// @brief 反向迭代器类型，使用标准库适配器
    using reverse_iterator = std::reverse_iterator<iterator>;

    /// @brief 常量反向迭代器类型，使用标准库适配器
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

public:
    /// @brief 默认构造函数，创建空链表
    explicit list();

    /// @brief 填充构造函数，创建包含count个value的链表
    /// @param count 元素数量
    /// @param value 元素值
    explicit list(size_type count, const_reference value = value_type());

    /// @brief 初始化列表构造函数
    /// @param ilist 初始化列表
    list(std::initializer_list<value_type> ilist);

    /// @brief 范围构造函数，复制[first, last)范围内的元素
    /// @tparam InputIt 输入迭代器类型
    /// @param first 范围起始迭代器
    /// @param last 范围结束迭代器
    template <typename InputIt,
              std::enable_if_t<!std::is_integral<InputIt>::value, int> = 0>
    list(InputIt first, InputIt last);

    /// @brief 拷贝构造函数
    /// @param other 要拷贝的链表
    list(const list<T, Allocator>& other);

    /// @brief 拷贝赋值运算符
    /// @param other 要拷贝的链表
    /// @return 当前链表的引用
    list<T, Allocator>& operator=(const list<T, Allocator>& other);

    /// @brief 初始化列表赋值运算符
    /// @param ilist 初始化列表
    /// @return 当前链表的引用
    list<T, Allocator>& operator=(std::initializer_list<value_type> ilist);

    /// @brief 移动构造函数
    /// @param other 要移动的链表
    list(list<T, Allocator>&& other) noexcept;

    /// @brief 移动赋值运算符
    /// @param other 要移动的链表
    /// @return 当前链表的引用
    list<T, Allocator>& operator=(list<T, Allocator>&& other) noexcept;

    /// @brief 析构函数，释放所有资源
    ~list();

    /// @brief 将链表内容替换为count个value
    /// @param count 元素数量
    /// @param value 元素值
    void assign(size_type count, const_reference value);

    /// @brief 将链表内容替换为初始化列表中的元素
    /// @param ilist 初始化列表
    void assign(std::initializer_list<value_type> ilist);

    /// @brief 将链表内容替换为[first, last)范围内的元素
    /// @tparam InputIt 输入迭代器类型
    /// @param first 范围起始迭代器
    /// @param last 范围结束迭代器
    template <typename InputIt,
              std::enable_if_t<!std::is_integral<InputIt>::value, int> = 0>
    void assign(InputIt first, InputIt last);

    /// @brief 获取分配器
    /// @return 分配器对象
    allocator_type get_allocator() const;

    // 元素访问
    /// @brief 获取第一个元素的引用
    /// @return 第一个元素的引用
    /// @throw std::out_of_range 如果链表为空
    reference front();

    /// @brief 获取第一个元素的常量引用
    /// @return 第一个元素的常量引用
    /// @throw std::out_of_range 如果链表为空
    const_reference front() const;

    /// @brief 获取最后一个元素的引用
    /// @return 最后一个元素的引用
    /// @throw std::out_of_range 如果链表为空
    reference back();

    /// @brief 获取最后一个元素的常量引用
    /// @return 最后一个元素的常量引用
    /// @throw std::out_of_range 如果链表为空
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
    const_iterator cbegin() const noexcept;

    /// @brief 返回指向末尾的迭代器
    /// @return 指向末尾的迭代器
    iterator end();

    /// @brief 返回指向末尾的常量迭代器
    /// @return 指向末尾的常量迭代器
    const_iterator end() const;

    /// @brief 返回指向末尾的常量迭代器（const版本）
    /// @return 指向末尾的常量迭代器
    const_iterator cend() const noexcept;

    /// @brief 返回指向最后一个元素的反向迭代器
    /// @return 指向末尾的反向迭代器
    reverse_iterator rbegin();

    /// @brief 返回指向最后一个元素的常量反向迭代器
    /// @return 指向末尾的常量反向迭代器
    const_reverse_iterator rbegin() const;

    /// @brief 返回指向最后一个元素的常量反向迭代器（const版本）
    /// @return 指向末尾的常量反向迭代器
    const_reverse_iterator crbegin() const noexcept;

    /// @brief 返回指向第一个元素之前位置的反向迭代器
    /// @return 指向首元素之前位置的反向迭代器
    reverse_iterator rend();

    /// @brief 返回指向第一个元素之前位置的常量反向迭代器
    /// @return 指向首元素之前位置的常量反向迭代器
    const_reverse_iterator rend() const;

    /// @brief 返回指向第一个元素之前位置的常量反向迭代器（const版本）
    /// @return 指向首元素之前位置的常量反向迭代器
    const_reverse_iterator crend() const noexcept;

    // 容量
    /// @brief 检查链表是否为空
    /// @return 如果为空返回true，否则返回false
    bool empty() const;

    /// @brief 返回链表中的元素数量
    /// @return 元素数量
    size_type size() const noexcept;

    /// @brief 返回链表能容纳的最大元素数量
    /// @return 最大元素数量
    size_type max_size() const;

    // 修改器
    /// @brief 清空链表，删除所有元素
    void clear();

    /// @brief 在指定位置插入元素（拷贝版本）
    /// @param pos 插入位置
    /// @param value 要插入的值
    /// @return 指向新插入元素的迭代器
    iterator insert(const_iterator pos, const_reference value);

    /// @brief 在指定位置插入元素（移动版本）
    /// @param pos 插入位置
    /// @param value 要插入的值（右值）
    /// @return 指向新插入元素的迭代器
    iterator insert(const_iterator pos, value_type&& value);

    /// @brief 在指定位置插入count个相同元素
    /// @param pos 插入位置
    /// @param count 插入数量
    /// @param value 要插入的值
    /// @return 指向第一个新插入元素的迭代器
    iterator insert(const_iterator pos, size_type count, const_reference value);

    /// @brief 在指定位置插入[first, last)范围内的元素
    /// @tparam InputIt 输入迭代器类型
    /// @param pos 插入位置
    /// @param first 范围起始迭代器
    /// @param last 范围结束迭代器
    /// @return 指向第一个新插入元素的迭代器
    template <typename InputIt,
              std::enable_if_t<!std::is_integral<InputIt>::value, int> = 0>
    iterator insert(const_iterator pos, InputIt first, InputIt last);

    /// @brief 在指定位置插入初始化列表中的元素
    /// @param pos 插入位置
    /// @param ilist 初始化列表
    /// @return 指向第一个新插入元素的迭代器
    iterator insert(const_iterator                    pos,
                    std::initializer_list<value_type> ilist);

    /// @brief 在指定位置原地构造元素
    /// @tparam Args 构造参数类型
    /// @param pos 插入位置
    /// @param args 构造参数
    /// @return 指向新构造元素的迭代器
    template <typename... Args>
    iterator emplace(const_iterator pos, Args&&... args);

    /// @brief 删除指定位置的元素
    /// @param pos 要删除的元素位置
    /// @return 指向被删除元素之后的迭代器
    iterator erase(iterator pos);

    /// @brief 删除指定位置的元素（const版本）
    /// @param pos 要删除的元素位置
    /// @return 指向被删除元素之后的迭代器
    iterator erase(const_iterator pos);

    /// @brief 删除[first, last)范围内的元素
    /// @param first 范围起始迭代器
    /// @param last 范围结束迭代器
    /// @return 指向最后一个被删除元素之后的迭代器
    iterator erase(iterator first, iterator last);

    /// @brief 删除[first, last)范围内的元素（const版本）
    /// @param first 范围起始迭代器
    /// @param last 范围结束迭代器
    /// @return 指向最后一个被删除元素之后的迭代器
    iterator erase(const_iterator first, const_iterator last);

    /// @brief 在末尾添加元素（拷贝版本）
    /// @param value 要添加的值
    void push_back(const_reference value);

    /// @brief 在末尾添加元素（移动版本）
    /// @param value 要添加的值（右值）
    void push_back(value_type&& value);

    /// @brief 在末尾原地构造元素
    /// @tparam Args 构造参数类型
    /// @param args 构造参数
    /// @return 指向新构造元素的引用
    template <typename... Args>
    reference emplace_back(Args&&... args);

    /// @brief 删除末尾元素
    /// @throw std::out_of_range 如果链表为空
    void pop_back();

    /// @brief 在开头添加元素（拷贝版本）
    /// @param value 要添加的值
    void push_front(const_reference value);

    /// @brief 在开头添加元素（移动版本）
    /// @param value 要添加的值（右值）
    void push_front(value_type&& value);

    /// @brief 在开头原地构造元素
    /// @tparam Args 构造参数类型
    /// @param args 构造参数
    /// @return 指向新构造元素的引用
    template <typename... Args>
    reference emplace_front(Args&&... argrs);

    /// @brief 删除开头元素
    /// @throw std::out_of_range 如果链表为空
    void pop_front();

    /// @brief 调整链表大小，新元素默认构造
    /// @param count 新的大小
    void resize(size_type count);

    /// @brief 调整链表大小，新元素用指定值填充
    /// @param count 新的大小
    /// @param value 用于填充新元素的值
    void resize(size_type count, const_reference value);

    /// @brief 交换两个链表的内容
    /// @param other 要交换的链表
    void swap(list<T, Allocator>& other) noexcept;

    // 操作
    /// @brief 合并两个已排序的链表（使用默认比较器）
    /// @param other 要合并的链表，合并后other为空
    void merge(list<T, Allocator>& other);

    /// @brief 合并两个已排序的链表（移动版本）
    /// @param other 要合并的链表，合并后other为空
    void merge(list<T, Allocator>&& other);

    /// @brief 使用指定比较器合并两个已排序的链表
    /// @tparam Compare 比较函数类型
    /// @param other 要合并的链表，合并后other为空
    /// @param comp 比较函数
    template <typename Compare>
    void merge(list<T, Allocator>& other, Compare comp);

    /// @brief 使用指定比较器合并两个已排序的链表（移动版本）
    /// @tparam Compare 比较函数类型
    /// @param other 要合并的链表，合并后other为空
    /// @param comp 比较函数
    template <typename Compare>
    void merge(list<T, Allocator>&& other, Compare comp);

    /// @brief 将other链表的所有元素移动到pos位置之前
    /// @param pos 插入位置
    /// @param other 要移动的链表，移动后other为空
    void splice(const_iterator pos, list<T, Allocator>& other);

    /// @brief 将other链表的所有元素移动到pos位置之前（移动版本）
    /// @param pos 插入位置
    /// @param other 要移动的链表，移动后other为空
    void splice(const_iterator pos, list<T, Allocator>&& other);

    /// @brief 将other中指定位置的元素移动到pos位置之前
    /// @param pos 插入位置
    /// @param other 源链表
    /// @param it 要移动的元素位置
    void splice(const_iterator pos, list& other, const_iterator it);

    /// @brief 将other中指定位置的元素移动到pos位置之前（移动版本）
    /// @param pos 插入位置
    /// @param other 源链表
    /// @param it 要移动的元素位置
    void splice(const_iterator pos, list&& other, const_iterator it);

    /// @brief 将other中[first, last)范围内的元素移动到pos位置之前
    /// @param pos 插入位置
    /// @param other 源链表
    /// @param first 范围起始迭代器
    /// @param last 范围结束迭代器
    void splice(const_iterator pos, list& other, const_iterator first,
                const_iterator last);

    /// @brief 将other中[first,
    /// last)范围内的元素移动到pos位置之前（移动版本）
    /// @param pos 插入位置
    /// @param other 源链表
    /// @param first 范围起始迭代器
    /// @param last 范围结束迭代器
    void splice(const_iterator pos, list&& other, const_iterator first,
                const_iterator last);

    /// @brief 删除所有等于value的元素
    /// @param value 要删除的值
    /// @return 删除的元素数量
    size_type remove(const T& value);

    /// @brief 删除所有满足条件的元素
    /// @tparam UnaryPredicate 一元谓词类型
    /// @param p 谓词函数，返回true表示删除该元素
    /// @return 删除的元素数量
    template <class UnaryPredicate>
    size_type remove_if(UnaryPredicate p);

    /// @brief 反转链表
    void reverse();

    /// @brief 移除连续重复的元素（使用默认相等比较）
    /// @return 移除的元素数量
    size_type unique();

    /// @brief 移除连续重复的元素（使用指定比较器）
    /// @tparam BinaryPredicate 二元谓词类型
    /// @param p 比较函数，返回true表示两个元素相等
    /// @return 移除的元素数量
    template <class BinaryPredicate>
    size_type unique(BinaryPredicate p);

    /// @brief 排序链表（使用默认比较器）
    void sort();

    /// @brief 使用指定比较器排序链表
    /// @tparam Compare 比较函数类型
    /// @param comp 比较函数
    template <class Compare>
    void sort(Compare comp);

private:
    /// @brief 将链表从指定位置拆分为两部分
    /// @param head 链表头
    /// @param len 第一部分的长度
    /// @return 第二部分的头节点
    Node* split(Node* head, size_type len);

    /// @brief 合并两个已排序的链表（使用默认比较器）
    /// @param la 第一个链表头
    /// @param lb 第二个链表头
    /// @return 合并后的链表头
    Node* merge_tow(Node* la, Node* lb);

    /// @brief 使用指定比较器合并两个已排序的链表
    /// @tparam Compare 比较函数类型
    /// @param la 第一个链表头
    /// @param lb 第二个链表头
    /// @param comp 比较函数
    /// @return 合并后的链表头
    template <typename Compare>
    Node* merge_tow(Node* la, Node* lb, Compare comp);

private:
    using node_allocator =
        typename std::allocator_traits<Allocator>::template rebind_alloc<Node>;

    Node*          m_head;
    size_type      m_size;
    node_allocator m_node_alloc;

    using alloc_traits = std::allocator_traits<node_allocator>;
};

//------------------- iterator 实现 -------------------------------
template <typename T, typename Allocator>
inline list<T, Allocator>::iterator::iterator() noexcept : m_ptr(nullptr)
{
}

template <typename T, typename Allocator>
inline list<T, Allocator>::iterator::iterator(Node* ptr) noexcept : m_ptr(ptr)
{
}

template <typename T, typename Allocator>
inline list<T, Allocator>::iterator::iterator(const iterator& other) noexcept
    : m_ptr(other.m_ptr)
{
}

// 解引用操作
template <typename T, typename Allocator>
inline typename list<T, Allocator>::iterator::reference
list<T, Allocator>::iterator::operator*() const noexcept
{
    // c++标准库判空吗？？  答案是不判空
    return m_ptr->val;
}

template <typename T, typename Allocator>
inline typename list<T, Allocator>::iterator::pointer
list<T, Allocator>::iterator::operator->() const noexcept
{
    return &m_ptr->val;
}

// 自增/自减
template <typename T, typename Allocator>
inline typename list<T, Allocator>::iterator&
list<T, Allocator>::iterator::operator++() noexcept
{
    m_ptr = m_ptr->next;
    return *this;
}

template <typename T, typename Allocator>
inline typename list<T, Allocator>::iterator
list<T, Allocator>::iterator::operator++(int) noexcept
{
    iterator temp(m_ptr);
    m_ptr = m_ptr->next;
    return temp;
}

template <typename T, typename Allocator>
inline typename list<T, Allocator>::iterator&
list<T, Allocator>::iterator::operator--() noexcept
{
    m_ptr = m_ptr->prev;
    return *this;
}

template <typename T, typename Allocator>
inline typename list<T, Allocator>::iterator
list<T, Allocator>::iterator::operator--(int) noexcept
{
    iterator temp(m_ptr);
    m_ptr = m_ptr->prev;
    return temp;
}

// 比较
template <typename T, typename Allocator>
inline bool list<T, Allocator>::iterator::operator==(
    const iterator other) const noexcept
{
    return m_ptr == other.m_ptr;
}

template <typename T, typename Allocator>
inline bool list<T, Allocator>::iterator::operator!=(
    const iterator other) const noexcept
{
    return !(*this == other);
}

template <typename T, typename Allocator>
inline bool list<T, Allocator>::iterator::operator==(
    const const_iterator other) const noexcept
{
    return m_ptr == other.m_ptr;
}

template <typename T, typename Allocator>
inline bool list<T, Allocator>::iterator::operator!=(
    const const_iterator other) const noexcept
{
    return !(*this == other);
}

//--------------- const_iterator 实现 -----------------------
template <typename T, typename Allocator>
inline list<T, Allocator>::const_iterator::const_iterator() noexcept
    : m_ptr(nullptr)
{
}

template <typename T, typename Allocator>
inline list<T, Allocator>::const_iterator::const_iterator(Node* ptr) noexcept
    : m_ptr(ptr)
{
}

template <typename T, typename Allocator>
inline list<T, Allocator>::const_iterator::const_iterator(iterator it) noexcept
    : m_ptr(it.m_ptr)
{
}

template <typename T, typename Allocator>
inline list<T, Allocator>::const_iterator::const_iterator(
    const const_iterator& other) noexcept
    : m_ptr(other.m_ptr)
{
}

// 解引用
template <typename T, typename Allocator>
inline typename list<T, Allocator>::const_iterator::reference
list<T, Allocator>::const_iterator::operator*() const noexcept
{
    return m_ptr->val;
}

template <typename T, typename Allocator>
inline typename list<T, Allocator>::const_iterator::pointer
list<T, Allocator>::const_iterator::operator->() const noexcept
{
    return &m_ptr->val;
}

// 自增/自减
template <typename T, typename Allocator>
inline typename list<T, Allocator>::const_iterator&
list<T, Allocator>::const_iterator::operator++() noexcept
{
    m_ptr = m_ptr->next;
    return *this;
}

template <typename T, typename Allocator>
inline typename list<T, Allocator>::const_iterator
list<T, Allocator>::const_iterator::operator++(int) noexcept
{
    const_iterator temp(m_ptr);
    m_ptr = m_ptr->next;
    return temp;
}

template <typename T, typename Allocator>
inline typename list<T, Allocator>::const_iterator&
list<T, Allocator>::const_iterator::operator--() noexcept
{
    m_ptr = m_ptr->prev;
    return *this;
}

template <typename T, typename Allocator>
inline typename list<T, Allocator>::const_iterator
list<T, Allocator>::const_iterator::operator--(int) noexcept
{
    const_iterator temp(m_ptr);
    m_ptr = m_ptr->prev;
    return temp;
}

// 比较
template <typename T, typename Allocator>
inline bool list<T, Allocator>::const_iterator::operator==(
    const const_iterator other) const noexcept
{
    return m_ptr == other.m_ptr;
}

template <typename T, typename Allocator>
inline bool list<T, Allocator>::const_iterator::operator!=(
    const const_iterator other) const noexcept
{
    return !(*this == other);
}

template <typename T, typename Allocator>
inline bool list<T, Allocator>::const_iterator::operator==(
    const iterator other) const noexcept
{
    return m_ptr == other.m_ptr;
}

template <typename T, typename Allocator>
inline bool list<T, Allocator>::const_iterator::operator!=(
    const iterator other) const noexcept
{
    return !(*this == other);
}

//------------------ list 实现 -----------------------
template <typename T, typename Allocator>
inline list<T, Allocator>::list() : m_head(nullptr), m_size(0), m_node_alloc()
{
    m_head = alloc_traits::allocate(m_node_alloc, 1);
    alloc_traits::construct(m_node_alloc, m_head);
    m_head->prev = m_head;
    m_head->next = m_head;
}

template <typename T, typename Allocator>
inline list<T, Allocator>::list(size_type count, const_reference value)
    : m_head(nullptr), m_size(0), m_node_alloc()
{
    // if (count > max_size())
    //     throw std::length_error("list count exceeds max_size");

    // m_head = alloc_traits::allocate(m_node_alloc, 1);
    // alloc_traits::construct(m_node_alloc, m_head);

    // Node *cur = m_head;
    // Node *pre = m_head;
    // for (size_type i = 0; i < count; ++i)
    // {
    //     cur->next = alloc_traits::allocate(m_node_alloc, 1);
    //     alloc_traits::construct(m_node_alloc, cur->next, value);
    //     pre = cur;
    //     cur = cur->next;
    //     cur->prev = pre;
    //     m_size++;
    // }

    // // 最后一个节点与头节点相连成环
    // cur->next = m_head;
    // m_head->pre = cur;

    m_head = alloc_traits::allocate(m_node_alloc, 1);
    alloc_traits::construct(m_node_alloc, m_head);
    m_head->prev = m_head;
    m_head->next = m_head;
    assign(count, value);
}

template <typename T, typename Allocator>
inline list<T, Allocator>::list(std::initializer_list<value_type> ilist)
    : m_head(nullptr), m_size(0), m_node_alloc()
{
    m_head = alloc_traits::allocate(m_node_alloc, 1);
    alloc_traits::construct(m_node_alloc, m_head);
    m_head->prev = m_head;
    m_head->next = m_head;
    assign(ilist);
}

template <typename T, typename Allocator>
template <typename InputIt,
          std::enable_if_t<!std::is_integral<InputIt>::value, int>>
inline list<T, Allocator>::list(InputIt first, InputIt last)
    : m_head(nullptr), m_size(0), m_node_alloc()
{
    m_head = alloc_traits::allocate(m_node_alloc, 1);
    alloc_traits::construct(m_node_alloc, m_head);
    m_head->prev = m_head;
    m_head->next = m_head;
    assign(first, last);
}

template <typename T, typename Allocator>
inline list<T, Allocator>::list(const list<T, Allocator>& other)
    : m_head(nullptr), m_size(0), m_node_alloc()
{
    m_head = alloc_traits::allocate(m_node_alloc, 1);
    alloc_traits::construct(m_node_alloc, m_head);

    Node* cur = m_head;
    Node* pre = m_head;
    for (Node* i = other.m_head->next; i != other.m_head; i = i->next)
    {
        cur->next = alloc_traits::allocate(m_node_alloc, 1);
        alloc_traits::construct(m_node_alloc, cur->next, i->val);
        pre       = cur;
        cur       = cur->next;
        cur->prev = pre;
        m_size++;
    }

    // 最后一个节点与头节点相连成环
    cur->next    = m_head;
    m_head->prev = cur;
}

template <typename T, typename Allocator>
inline list<T, Allocator>& list<T, Allocator>::operator=(
    const list<T, Allocator>& other)
{
    if (this != &other)
    {
        clear();

        Node* cur = m_head;
        Node* pre = m_head;
        for (Node* i = other.m_head->next; i != other.m_head; i = i->next)
        {
            cur->next = alloc_traits::allocate(m_node_alloc, 1);
            alloc_traits::construct(m_node_alloc, cur->next, i->val);
            pre       = cur;
            cur       = cur->next;
            cur->prev = pre;
            m_size++;
        }

        // 最后一个节点与头节点相连成环
        cur->next    = m_head;
        m_head->prev = cur;
    }
    return *this;
}

template <typename T, typename Allocator>
inline list<T, Allocator>& list<T, Allocator>::operator=(
    std::initializer_list<value_type> ilist)
{
    assign(ilist);
    return *this;
}

template <typename T, typename Allocator>
inline list<T, Allocator>::list(list<T, Allocator>&& other) noexcept
    : m_head(nullptr), m_size(0), m_node_alloc()
{
    m_head = other.m_head;
    m_size = other.m_size;

    other.m_size = 0;
    other.m_head = alloc_traits::allocate(m_node_alloc, 1);
    alloc_traits::construct(m_node_alloc, other.m_head);
    other.m_head->prev = other.m_head;
    other.m_head->next = other.m_head;
}

template <typename T, typename Allocator>
inline list<T, Allocator>& list<T, Allocator>::operator=(
    list<T, Allocator>&& other) noexcept
{
    if (this != &other)
    {
        clear();

        m_head = other.m_head;
        m_size = other.m_size;

        other.m_size = 0;
        other.m_head = alloc_traits::allocate(m_node_alloc, 1);
        alloc_traits::construct(m_node_alloc, other.m_head);
        other.m_head->prev = other.m_head;
        other.m_head->next = other.m_head;
    }
    return *this;
}

template <typename T, typename Allocator>
inline list<T, Allocator>::~list()
{
    clear();
    if (m_head != nullptr)
    {
        alloc_traits::destroy(m_node_alloc, m_head);
        alloc_traits::deallocate(m_node_alloc, m_head, 1);
    }
}

template <typename T, typename Allocator>
inline void list<T, Allocator>::assign(size_type count, const_reference value)
{
    if (count > max_size())
        throw std::length_error("list count exceeds max_size");

    clear();

    Node* pre = m_head;
    Node* cur = m_head;
    for (size_type i = 0; i < count; i++)
    {
        cur->next = alloc_traits::allocate(m_node_alloc, 1);
        alloc_traits::construct(m_node_alloc, cur->next, value);
        pre       = cur;
        cur       = cur->next;
        cur->prev = pre;
        m_size++;
    }

    // 最后一个节点与头节点相连成环
    cur->next    = m_head;
    m_head->prev = cur;
}

template <typename T, typename Allocator>
inline void list<T, Allocator>::assign(std::initializer_list<value_type> ilist)
{
    clear();

    Node* pre = m_head;
    Node* cur = m_head;
    for (const_reference item : ilist)
    {
        cur->next = alloc_traits::allocate(m_node_alloc, 1);
        alloc_traits::construct(m_node_alloc, cur->next, item);
        pre       = cur;
        cur       = cur->next;
        cur->prev = pre;
        m_size++;
    }

    // 最后一个节点与头节点相连成环
    cur->next    = m_head;
    m_head->prev = cur;
}

template <typename T, typename Allocator>
template <typename InputIt,
          std::enable_if_t<!std::is_integral<InputIt>::value, int>>
void list<T, Allocator>::assign(InputIt first, InputIt last)
{
    clear();

    Node* pre = m_head;
    Node* cur = m_head;
    for (; first != last; first++)
    {
        cur->next = alloc_traits::allocate(m_node_alloc, 1);
        alloc_traits::construct(m_node_alloc, cur->next, *first);
        pre       = cur;
        cur       = cur->next;
        cur->prev = pre;
        m_size++;
    }

    // 最后一个节点与头节点相连成环
    cur->next    = m_head;
    m_head->prev = cur;
}

template <typename T, typename Allocator>
inline typename list<T, Allocator>::allocator_type
list<T, Allocator>::get_allocator() const
{
    return typename std::allocator_traits<
        node_allocator>::template rebind_alloc<T>(m_node_alloc);
}

template <typename T, typename Allocator>
inline typename list<T, Allocator>::reference list<T, Allocator>::front()
{
    if (empty())
        throw std::out_of_range("list::front: empty list");
    return m_head->next->val;
}

template <typename T, typename Allocator>
inline typename list<T, Allocator>::const_reference list<T, Allocator>::front()
    const
{
    if (empty())
        throw std::out_of_range("list::front: empty list");
    return m_head->next->val;
}

template <typename T, typename Allocator>
inline typename list<T, Allocator>::reference list<T, Allocator>::back()
{
    if (empty())
        throw std::out_of_range("list::back: empty list");
    return m_head->prev->val;
}

template <typename T, typename Allocator>
inline typename list<T, Allocator>::const_reference list<T, Allocator>::back()
    const
{
    if (empty())
        throw std::out_of_range("list::back: empty list");
    return m_head->prev->val;
}

template <typename T, typename Allocator>
inline typename list<T, Allocator>::iterator list<T, Allocator>::begin()
{
    return iterator(m_head->next);
}

template <typename T, typename Allocator>
inline typename list<T, Allocator>::const_iterator list<T, Allocator>::begin()
    const
{
    return const_iterator(m_head->next);
}

template <typename T, typename Allocator>
inline typename list<T, Allocator>::const_iterator list<T, Allocator>::cbegin()
    const noexcept
{
    return begin();
}

template <typename T, typename Allocator>
inline typename list<T, Allocator>::iterator list<T, Allocator>::end()
{
    return iterator(m_head);
}

template <typename T, typename Allocator>
inline typename list<T, Allocator>::const_iterator list<T, Allocator>::end()
    const
{
    return const_iterator(m_head);
}

template <typename T, typename Allocator>
inline typename list<T, Allocator>::const_iterator list<T, Allocator>::cend()
    const noexcept
{
    return end();
}

template <typename T, typename Allocator>
inline typename list<T, Allocator>::reverse_iterator
list<T, Allocator>::rbegin()
{
    return reverse_iterator(end());
}

template <typename T, typename Allocator>
inline typename list<T, Allocator>::const_reverse_iterator
list<T, Allocator>::rbegin() const
{
    return const_reverse_iterator(end());
}

template <typename T, typename Allocator>
inline typename list<T, Allocator>::const_reverse_iterator
list<T, Allocator>::crbegin() const noexcept
{
    return rbegin();
}

template <typename T, typename Allocator>
inline typename list<T, Allocator>::reverse_iterator list<T, Allocator>::rend()
{
    return reverse_iterator(begin());
}

template <typename T, typename Allocator>
inline typename list<T, Allocator>::const_reverse_iterator
list<T, Allocator>::rend() const
{
    return const_reverse_iterator(begin());
}

template <typename T, typename Allocator>
inline typename list<T, Allocator>::const_reverse_iterator
list<T, Allocator>::crend() const noexcept
{
    return rend();
}

template <typename T, typename Allocator>
inline bool list<T, Allocator>::empty() const
{
    return m_head->next == m_head;
}

template <typename T, typename Allocator>
inline typename list<T, Allocator>::size_type list<T, Allocator>::size()
    const noexcept
{
    return m_size;
}

template <typename T, typename Allocator>
inline typename list<T, Allocator>::size_type list<T, Allocator>::max_size()
    const
{
    return alloc_traits::max_size(m_node_alloc);
}

template <typename T, typename Allocator>
inline void list<T, Allocator>::clear()
{
    Node* cur      = m_head->next;
    Node* del_node = nullptr;
    while (cur != m_head)
    {
        del_node = cur;
        // del_node->next->prev = pre; 单向遍历删除就好，可以不用维护 prev
        cur = del_node->next;

        alloc_traits::destroy(m_node_alloc, del_node);
        alloc_traits::deallocate(m_node_alloc, del_node, 1);
    }

    m_head->next = m_head;
    m_head->prev = m_head;
    m_size       = 0;
}

template <typename T, typename Allocator>
inline typename list<T, Allocator>::iterator list<T, Allocator>::insert(
    const_iterator pos, const_reference value)
{
    Node* pre  = pos.m_ptr->prev;
    Node* next = pos.m_ptr;

    Node* new_node = alloc_traits::allocate(m_node_alloc, 1);
    alloc_traits::construct(m_node_alloc, new_node, value);
    // 与前节点相连
    new_node->prev = pre;
    pre->next      = new_node;
    // 与后节点相连
    new_node->next = next;
    next->prev     = new_node;

    m_size++;

    return iterator(new_node);
}

template <typename T, typename Allocator>
inline typename list<T, Allocator>::iterator list<T, Allocator>::insert(
    const_iterator pos, value_type&& value)
{
    Node* pre  = pos.m_ptr->prev;
    Node* next = pos.m_ptr;

    Node* new_node = alloc_traits::allocate(m_node_alloc, 1);
    alloc_traits::construct(m_node_alloc, new_node, std::move(value));
    // 与前节点相连
    new_node->prev = pre;
    pre->next      = new_node;
    // 与后节点相连
    new_node->next = next;
    next->prev     = new_node;

    m_size++;

    return iterator(new_node);
}

template <typename T, typename Allocator>
inline typename list<T, Allocator>::iterator list<T, Allocator>::insert(
    const_iterator pos, size_type count, const_reference value)
{
    if (count == 0)
        return iterator(pos.m_ptr);

    if (count + m_size > max_size())
        throw std::length_error("list::insert: count exceeds max_size");

    Node* pre  = pos.m_ptr->prev;
    Node* next = pos.m_ptr;

    Node*    new_node = nullptr;
    iterator ret(pos.m_ptr);

    for (size_type i = 0; i < count; i++)
    {
        new_node = alloc_traits::allocate(m_node_alloc, 1);
        alloc_traits::construct(m_node_alloc, new_node, value);
        new_node->prev = pre;
        pre->next      = new_node;

        pre = pre->next;

        m_size++;

        if (i == 0)
            ret.m_ptr = new_node;
    }

    // 插入的最后一个节点与pos节点相连
    new_node->next = next;
    next->prev     = new_node;

    return ret;
}

template <typename T, typename Allocator>
template <typename InputIt,
          std::enable_if_t<!std::is_integral<InputIt>::value, int>>
typename list<T, Allocator>::iterator list<T, Allocator>::insert(
    const_iterator pos, InputIt first, InputIt last)
{
    Node* pre  = pos.m_ptr->prev;
    Node* next = pos.m_ptr;

    Node*    new_node = nullptr;
    iterator ret(pos.m_ptr);
    bool     is = true;  // 标记第一个节点

    for (; first != last; first++)
    {
        new_node = alloc_traits::allocate(m_node_alloc, 1);
        alloc_traits::construct(m_node_alloc, new_node, *first);
        new_node->prev = pre;
        pre->next      = new_node;

        pre = pre->next;

        m_size++;

        if (is)
        {
            ret.m_ptr = new_node;
            is        = false;
        }
    }

    new_node->next = next;
    next->prev     = new_node;

    return ret;
}

template <typename T, typename Allocator>
inline typename list<T, Allocator>::iterator list<T, Allocator>::insert(
    const_iterator pos, std::initializer_list<value_type> ilist)
{
    if (ilist.size() == 0)
        return iterator(pos.m_ptr);

    Node* pre  = pos.m_ptr->prev;
    Node* next = pos.m_ptr;

    Node*    new_node = nullptr;
    iterator ret(pos.m_ptr);
    bool     is = true;

    for (const_reference value : ilist)
    {
        new_node = alloc_traits::allocate(m_node_alloc, 1);
        alloc_traits::construct(m_node_alloc, new_node, value);
        new_node->prev = pre;
        pre->next      = new_node;

        pre = pre->next;

        m_size++;

        if (is)
        {
            ret.m_ptr = new_node;
            is        = false;
        }
    }

    new_node->next = next;
    next->prev     = new_node;

    return ret;
}

template <typename T, typename Allocator>
template <typename... Args>
inline typename list<T, Allocator>::iterator list<T, Allocator>::emplace(
    const_iterator pos, Args&&... args)
{
    Node* pre  = pos.m_ptr->prev;
    Node* next = pos.m_ptr;

    Node* new_node = alloc_traits::allocate(m_node_alloc, 1);
    alloc_traits::construct(m_node_alloc, new_node,
                            std::forward<Args>(args)...);
    new_node->prev = pre;
    pre->next      = new_node;
    new_node->next = next;
    next->prev     = new_node;

    m_size++;

    return iterator(new_node);
}

template <typename T, typename Allocator>
inline typename list<T, Allocator>::iterator list<T, Allocator>::erase(
    iterator pos)
{
    return erase(const_iterator(pos));
}

template <typename T, typename Allocator>
inline typename list<T, Allocator>::iterator list<T, Allocator>::erase(
    const_iterator pos)
{
    if (pos == end())
        return iterator(pos.m_ptr);

    Node* pre      = pos.m_ptr->prev;
    Node* next     = pos.m_ptr->next;
    Node* del_node = pos.m_ptr;

    pre->next  = next;
    next->prev = pre;

    alloc_traits::destroy(m_node_alloc, del_node);
    alloc_traits::deallocate(m_node_alloc, del_node, 1);

    m_size--;

    return iterator(next);
}

template <typename T, typename Allocator>
inline typename list<T, Allocator>::iterator list<T, Allocator>::erase(
    iterator first, iterator last)
{
    return erase(const_iterator(first), const_iterator(last));
}

template <typename T, typename Allocator>
inline typename list<T, Allocator>::iterator list<T, Allocator>::erase(
    const_iterator first, const_iterator last)
{
    if (first == last)
        return iterator(first.m_ptr);

    Node* pre      = first.m_ptr->prev;
    Node* next     = last.m_ptr;
    Node* cur      = first.m_ptr;
    Node* del_node = nullptr;

    pre->next  = next;
    next->prev = pre;

    while (cur != last.m_ptr)
    {
        del_node = cur;
        cur      = cur->next;

        alloc_traits::destroy(m_node_alloc, del_node);
        alloc_traits::deallocate(m_node_alloc, del_node, 1);

        m_size--;
    }

    return iterator(last.m_ptr);
}

template <typename T, typename Allocator>
inline void list<T, Allocator>::push_back(const_reference value)
{
    // push_back(value_type(value));
    emplace_back(value);
}

template <typename T, typename Allocator>
inline void list<T, Allocator>::push_back(value_type&& value)
{
    Node* pre      = m_head->prev;
    Node* new_node = alloc_traits::allocate(m_node_alloc, 1);
    alloc_traits::construct(m_node_alloc, new_node, std::move(value));

    pre->next      = new_node;
    new_node->prev = pre;

    new_node->next = m_head;
    m_head->prev   = new_node;

    m_size++;
}

template <typename T, typename Allocator>
template <typename... Args>
inline typename list<T, Allocator>::reference list<T, Allocator>::emplace_back(
    Args&&... args)
{
    Node* pre      = m_head->prev;
    Node* new_node = alloc_traits::allocate(m_node_alloc, 1);
    alloc_traits::construct(m_node_alloc, new_node,
                            std::forward<Args>(args)...);

    pre->next      = new_node;
    new_node->prev = pre;

    new_node->next = m_head;
    m_head->prev   = new_node;

    m_size++;

    return new_node->val;
}

template <typename T, typename Allocator>
inline void list<T, Allocator>::pop_back()
{
    if (empty())
        throw std::out_of_range("list::pop_back: list is empty");

    Node* del_node       = m_head->prev;
    m_head->prev         = del_node->prev;
    del_node->prev->next = m_head;

    alloc_traits::destroy(m_node_alloc, del_node);
    alloc_traits::deallocate(m_node_alloc, del_node, 1);

    m_size--;
}

template <typename T, typename Allocator>
inline void list<T, Allocator>::push_front(const_reference value)
{
    push_front(value_type(value));
}

template <typename T, typename Allocator>
inline void list<T, Allocator>::push_front(value_type&& value)
{
    Node* pre  = m_head;
    Node* next = m_head->next;

    Node* new_node = alloc_traits::allocate(m_node_alloc, 1);
    alloc_traits::construct(m_node_alloc, new_node, std::move(value));

    pre->next      = new_node;
    new_node->prev = pre;

    new_node->next = next;
    next->prev     = new_node;

    m_size++;
}

template <typename T, typename Allocator>
template <typename... Args>
inline typename list<T, Allocator>::reference list<T, Allocator>::emplace_front(
    Args&&... args)
{
    Node* pre  = m_head;
    Node* next = m_head->next;

    Node* new_node = alloc_traits::allocate(m_node_alloc, 1);
    alloc_traits::construct(m_node_alloc, new_node,
                            std::forward<Args>(args)...);

    pre->next      = new_node;
    new_node->prev = pre;

    new_node->next = next;
    next->prev     = new_node;

    m_size++;

    return new_node->val;
}

template <typename T, typename Allocator>
inline void list<T, Allocator>::pop_front()
{
    if (empty())
        throw std::out_of_range("list::pop_front: list is empty");

    Node* pre      = m_head;
    Node* del_node = m_head->next;

    pre->next            = del_node->next;
    del_node->next->prev = pre;

    alloc_traits::destroy(m_node_alloc, del_node);
    alloc_traits::deallocate(m_node_alloc, del_node, 1);

    m_size--;
}

template <typename T, typename Allocator>
inline void list<T, Allocator>::resize(size_type count)
{
    if (m_size == count)
        return;
    else if (m_size < count)
    {
        Node* new_node = nullptr;
        Node* pre      = m_head->prev;

        size_type diff =
            count - m_size;  // 一定不要把 count-m_size 写在循环条件里

        for (size_type i = 0; i < diff; i++)
        {
            new_node = alloc_traits::allocate(m_node_alloc, 1);
            alloc_traits::construct(m_node_alloc, new_node);
            pre->next      = new_node;
            new_node->prev = pre;

            pre = pre->next;
            m_size++;
        }

        new_node->next = m_head;
        m_head->prev   = new_node;
    }
    else
    {
        size_type diff =
            m_size - count;  // 一定不要把 m_size-count 写在循环条件里
        for (size_type i = 0; i < diff; i++)
            pop_back();
    }
}

template <typename T, typename Allocator>
inline void list<T, Allocator>::resize(size_type count, const_reference value)
{
    if (m_size == count)
        return;
    else if (m_size < count)
    {
        insert(end(), count - m_size, value);
    }
    else
    {
        size_type diff =
            m_size - count;  // 一定不要把 m_size-count 写在循环条件里
        for (size_type i = 0; i < diff; i++)
            pop_back();
    }
}

template <typename T, typename Allocator>
inline void list<T, Allocator>::swap(list<T, Allocator>& other) noexcept
{
    using std::swap;
    swap(m_head, other.m_head);
    swap(m_node_alloc, other.m_node_alloc);
    swap(m_size, other.m_size);
}

// 操作
template <typename T, typename Allocator>
inline void list<T, Allocator>::merge(list<T, Allocator>& other)
{
    merge(other, std::less<>());
}

template <typename T, typename Allocator>
inline void list<T, Allocator>::merge(list<T, Allocator>&& other)
{
    merge(other, std::less<>());
}

template <typename T, typename Allocator>
template <typename Compare>
inline void list<T, Allocator>::merge(list<T, Allocator>& other, Compare comp)
{
    if (this == &other)
        return;

    if (other.empty())
        return;

    if (this->empty())
    {
        m_head->next             = other.m_head->next;
        m_head->prev             = other.m_head->prev;
        other.m_head->next->prev = m_head;
        other.m_head->prev->next = m_head;
        m_size                   = other.m_size;

        //  other 置空
        other.m_head->next = other.m_head;
        other.m_head->prev = other.m_head;
        other.m_size       = 0;

        return;
    }

    // 默认从 other 合并到 this
    Node* cur       = m_head;
    Node* cur_this  = m_head->next;
    Node* cur_other = other.m_head->next;

    while (cur_this != m_head && cur_other != other.m_head)
    {
        if (comp(cur_this->val, cur_other->val))
        {
            cur      = cur->next;
            cur_this = cur_this->next;
        }
        else
        {
            cur->next       = cur_other;
            cur_other->prev = cur;
            Node* next      = cur_other->next;

            // 将 cur_other 嵌入到 this 中
            cur_other->next = cur_this;
            cur_this->prev  = cur_other;

            cur       = cur->next;
            cur_other = next;
        }
    }

    if (cur_other != other.m_head)
    {
        cur->next       = cur_other;
        cur_other->prev = cur;

        Node* last_node_other = other.m_head->prev;
        last_node_other->next = m_head;
        m_head->prev          = last_node_other;
    }

    m_size += other.m_size;

    // other 恢复有效空态
    other.m_head->next = other.m_head;
    other.m_head->prev = other.m_head;
    other.m_size       = 0;
}

template <typename T, typename Allocator>
template <typename Compare>
inline void list<T, Allocator>::merge(list<T, Allocator>&& other, Compare comp)
{
    merge(other, comp);
}

template <typename T, typename Allocator>
inline void list<T, Allocator>::splice(const_iterator      pos,
                                       list<T, Allocator>& other)
{
    if (other.empty() || this == &other)
        return;

    Node* pre  = pos.m_ptr->prev;
    Node* next = pos.m_ptr;
    Node* cur  = other.m_head->next;

    while (cur != other.m_head)
    {
        pre->next = cur;
        cur->prev = pre;

        pre = pre->next;
        cur = cur->next;
    }

    pre->next  = next;
    next->prev = pre;

    m_size += other.m_size;

    // other置空
    other.m_head->next = other.m_head;
    other.m_head->prev = other.m_head;
    other.m_size       = 0;
}

template <typename T, typename Allocator>
inline void list<T, Allocator>::splice(const_iterator       pos,
                                       list<T, Allocator>&& other)
{
    splice(pos, other);
}

template <typename T, typename Allocator>
inline void list<T, Allocator>::splice(const_iterator pos, list& other,
                                       const_iterator it)
{
    if (it == other.end() || this == &other)
        return;

    Node* pre  = pos.m_ptr->prev;
    Node* next = pos.m_ptr;

    // 将it从other中分离出来
    it.m_ptr->prev->next = it.m_ptr->next;
    it.m_ptr->next->prev = it.m_ptr->prev;

    pre->next      = it.m_ptr;
    it.m_ptr->prev = pre;
    it.m_ptr->next = next;
    next->prev     = it.m_ptr;

    m_size++;
    other.m_size--;
}

template <typename T, typename Allocator>
inline void list<T, Allocator>::splice(const_iterator pos, list&& other,
                                       const_iterator it)
{
    splice(pos, other, it);
}

template <typename T, typename Allocator>
inline void list<T, Allocator>::splice(const_iterator pos, list& other,
                                       const_iterator first,
                                       const_iterator last)
{
    if (other.empty() || this == &other)
        return;

    Node* pre       = pos.m_ptr->prev;
    Node* next      = pos.m_ptr;
    Node* pre_other = first.m_ptr->prev;

    while (first != last)
    {
        pre->next         = first.m_ptr;
        first.m_ptr->prev = pre;

        pre = pre->next;
        first++;

        m_size++;
        other.m_size--;
    }

    pre->next  = next;
    next->prev = pre;

    pre_other->next  = last.m_ptr;
    last.m_ptr->prev = pre_other;
}

template <typename T, typename Allocator>
inline void list<T, Allocator>::splice(const_iterator pos, list&& other,
                                       const_iterator first,
                                       const_iterator last)
{
    splice(pos, other, first, last);
}

template <typename T, typename Allocator>
inline typename list<T, Allocator>::size_type list<T, Allocator>::remove(
    const T& value)
{
    return remove_if([&](const T& val) { return val == value; });
}

template <typename T, typename Allocator>
template <class UnaryPredicate>
inline typename list<T, Allocator>::size_type list<T, Allocator>::remove_if(
    UnaryPredicate p)
{
    if (empty())
        return 0;

    size_type count = 0;
    Node*     pre   = m_head;
    Node*     cur   = m_head->next;
    Node*     next  = nullptr;

    while (cur != m_head)
    {
        if (p(cur->val))
        {
            next = cur->next;

            pre->next  = next;
            next->prev = pre;

            alloc_traits::destroy(m_node_alloc, cur);
            alloc_traits::deallocate(m_node_alloc, cur, 1);
            m_size--;
            count++;
            cur = next;
        }
        else
        {
            cur = cur->next;
            pre = pre->next;
        }
    }

    return count;
}

template <typename T, typename Allocator>
inline void list<T, Allocator>::reverse()
{
    if (empty())
        return;

    // 不能直接交换前哨节点的prev和next,虽然这样做是O(1)
    // 但是会破坏链表结构，导致迭代器的++变为了--

    Node* cur  = m_head->next;
    Node* next = nullptr;

    m_head->prev = cur;
    while (cur != m_head)
    {
        next      = cur->next;
        cur->next = cur->prev;
        cur->prev = next;

        if (next == m_head)
            m_head->next = cur;

        cur = next;
    }
}

template <typename T, typename Allocator>
inline typename list<T, Allocator>::size_type list<T, Allocator>::unique()
{
    return unique(std::equal_to<T>());
}

template <typename T, typename Allocator>
template <class BinaryPredicate>
inline typename list<T, Allocator>::size_type list<T, Allocator>::unique(
    BinaryPredicate p)
{
    if (empty())
        return 0;

    size_type count = 0;
    Node*     pre   = m_head->next;
    Node*     cur   = pre->next;

    while (cur != m_head)
    {
        if (p(cur->val, pre->val))
        {
            Node* next = cur->next;

            pre->next  = next;
            next->prev = pre;

            alloc_traits::destroy(m_node_alloc, cur);
            alloc_traits::deallocate(m_node_alloc, cur, 1);
            m_size--;
            count++;
            cur = next;
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
inline void list<T, Allocator>::sort()
{
    sort(std::less<>());
}

template <typename T, typename Allocator>
template <class Compare>
inline void list<T, Allocator>::sort(Compare comp)
{
    if (empty())
        return;

    Node* head  = m_head->next;
    Node* cur   = nullptr;
    Node* left  = nullptr;
    Node* right = nullptr;

    Node* temp = alloc_traits::allocate(m_node_alloc, 1);  // 临时前哨节点
    alloc_traits::construct(m_node_alloc, temp);

    for (size_type i = 1; i < m_size; i *= 2)
    {
        cur        = head;
        Node* tail = temp;

        while (cur != m_head)
        {
            left       = cur;
            right      = split(left, i);
            cur        = split(right, i);
            tail->next = merge_tow(left, right, comp);
            while (tail->next != m_head)
                tail = tail->next;
        }

        head = temp->next;
    }

    m_head->next = head;

    // 排序最后，统一修复所有 prev 指针
    Node* p = m_head->next;
    p->prev = m_head;
    while (p != m_head)
    {
        p->next->prev = p;
        p             = p->next;
    }

    alloc_traits::destroy(m_node_alloc, temp);
    alloc_traits::deallocate(m_node_alloc, temp, 1);
}

template <typename T, typename Allocator>
inline typename list<T, Allocator>::Node* list<T, Allocator>::split(
    Node* head, size_type len)
{
    if (head == m_head)
        return head;

    Node* cur = head;
    // 向前走 n-1 步（拆分出前 n 个节点）
    for (size_type i = 1; i < len && cur->next != m_head; i++)
        cur = cur->next;

    Node* next_part = cur->next;
    cur->next       = m_head;

    return next_part;
}

template <typename T, typename Allocator>
inline typename list<T, Allocator>::Node* list<T, Allocator>::merge_tow(
    Node* la, Node* lb)
{
    return merge_tow(la, lb, std::less<T>());
}

template <typename T, typename Allocator>
template <typename Compare>
inline typename list<T, Allocator>::Node* list<T, Allocator>::merge_tow(
    Node* la, Node* lb, Compare comp)
{
    if (la == m_head)
        return lb;
    if (lb == m_head)
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

    while (cur_a != m_head && cur_b != m_head)
    {
        if (comp(cur_a->val, cur_b->val))
        {
            pre   = cur_a;
            cur_a = cur_a->next;
        }
        else
        {
            Node* next  = cur_b->next;
            pre->next   = cur_b;
            cur_b->next = cur_a;

            pre   = pre->next;
            cur_b = next;
        }
    }

    if (cur_b != m_head)
        pre->next = cur_b;

    Node* res = temp->next;
    alloc_traits::destroy(m_node_alloc, temp);
    alloc_traits::deallocate(m_node_alloc, temp, 1);

    return res;
}

template <typename T, typename Allocator>
bool operator==(const list<T, Allocator>& lhs, const list<T, Allocator>& rhs)
{
    if (lhs.size() != rhs.size())
        return false;

    typename list<T, Allocator>::Node* cur_a = lhs.m_head->next;
    typename list<T, Allocator>::Node* cur_b = rhs.m_head->next;

    for (; cur_a != lhs.m_head && cur_b != rhs.m_head;
         cur_a = cur_a->next, cur_b = cur_b->next)
    {
        if (cur_a->val != cur_b->val)
            return false;
    }

    return true;
}

template <typename T, typename Allocator>
bool operator!=(const list<T, Allocator>& lhs, const list<T, Allocator>& rhs)
{
    return !(lhs == rhs);
}

}  // namespace demo
