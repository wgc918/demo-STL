//-----------------------------------------------------------------------------
// 版权所有 (C) 2026 demo-STL 项目
//
// 文件: multiset.h
// 作者: wgc
// 创建日期: 2026年6月
// 最后修改: 2026年6月
//
// 描述:
//     本文件实现了一个 STL 风格的 multiset 容器。
//     multiset 是一种关联容器，存储键值，允许键重复，且按键的顺序进行排序。
//
// 基本概念:
//     multiset（多重集合）是一种允许重复键的容器。
//     与 set 的区别在于，multiset 允许同一个键出现多次。
//     multiset 中的元素自动按键排序，支持高效的有序遍历。
//
// 主要特性:
//     - 基于红黑树实现，保证 O(log n) 的插入、删除和查找操作
//     - 自动按键排序，支持有序遍历
//     - 允许键重复，同一个键可以出现多次
//     - 提供双向迭代器支持
//     - 符合 STL 容器规范
//
// 常用操作:
//     - 插入: insert(), emplace(), emplace_hint()
//     - 删除: erase(), clear()
//     - 查找: find(), count(), lower_bound(), upper_bound(), equal_range()
//     - 遍历: begin(), end(), iterator, const_iterator
//     - 容量: size(), empty(), max_size()
//
// 使用场景:
//     - 需要存储可重复值的集合
//     - 需要统计元素出现次数
//     - 需要按键的有序遍历
//     - 需要多重集合操作
//     - 对元素出现次数有要求的场景
//
// 注意事项:
//     - multiset 中的元素可以重复，插入总是成功
//     - multiset 不支持直接访问元素，只能通过迭代器访问
//     - 键的类型必须支持比较操作（默认使用 std::less）
//     - 删除操作会保持树的平衡，性能稳定
//     - 迭代器按中序遍历顺序访问元素
//     - erase(key) 会删除所有匹配的元素，返回删除的数量
//
// 与 set 的区别与联系:
//     - 联系: 都基于红黑树实现，具有相同的查找、插入、删除性能
//     - 区别: set 不允许重复键，multiset 允许重复键
//     - set 的 insert 返回 pair<iterator, bool>，multiset 的 insert 返回 iterator
//     - set 的 count 返回 0 或 1，multiset 的 count 可以返回大于 1 的值
//     - set 的 erase(key) 返回 0 或 1，multiset 的 erase(key) 返回实际删除的数量
//     - 两者都支持按键排序和有序遍历
//
// 实现说明:
//     multiset 使用红黑树（Red-Black Tree）数据结构实现，这是一种自平衡二叉搜索树。
//     红黑树通过维护节点颜色和旋转操作保持树的平衡，确保最坏情况下的时间复杂度。
//     与 set 不同，multiset 在插入时对重复键不做排斥，而是按等价键顺序插入。
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
#include <vector>

#ifdef NDEBUG
#define DEBUG_ONLY(func) (void)0
#define DEBUG_FUNCTION   inline void
#else
#define DEBUG_ONLY(func) func
#define DEBUG_FUNCTION
#endif

namespace demo
{

/// @brief multiset 容器类
/// @details
/// multiset 是一种关联容器，存储键值，允许键重复，且按键的顺序进行排序。
/// multiset 使用红黑树实现，保证 O(log n) 的插入、删除和查找操作。
/// @tparam Key 键类型，必须满足可比较（Compare）要求
/// @tparam Compare 比较函数对象类型，用于键的比较，默认为 std::less<Key>
/// @tparam Allocator 分配器类型，用于内存管理，默认为 std::allocator<Key>
template <typename Key, typename Compare = std::less<Key>,
          typename Allocator = std::allocator<Key>>
class multiset;

/// @brief 比较两个 multiset 是否相等
/// @tparam Key 键类型
/// @tparam Compare 比较函数对象类型
/// @tparam Allocator 分配器类型
/// @param lhs 左操作数
/// @param rhs 右操作数
/// @return 如果两个 multiset 相等返回 true，否则返回 false
template <typename Key, typename Compare, typename Allocator>
bool operator==(const multiset<Key, Compare, Allocator>& lhs,
                const multiset<Key, Compare, Allocator>& rhs);

/// @brief 比较两个 multiset 是否不相等
/// @tparam Key 键类型
/// @tparam Compare 比较函数对象类型
/// @tparam Allocator 分配器类型
/// @param lhs 左操作数
/// @param rhs 右操作数
/// @return 如果两个 multiset 不相等返回 true，否则返回 false
template <typename Key, typename Compare, typename Allocator>
bool operator!=(const multiset<Key, Compare, Allocator>& lhs,
                const multiset<Key, Compare, Allocator>& rhs);

template <typename Key, typename Compare, typename Allocator>
class multiset
{
    friend bool operator== <>(const multiset& lhs, const multiset& rhs);
    friend bool operator!= <>(const multiset& lhs, const multiset& rhs);

public:
    using key_type        = Key;                ///< 键类型
    using value_type      = Key;                ///< 元素类型（与键类型相同）
    using key_compare     = Compare;            ///< 键比较函数类型
    using allocator_type  = Allocator;          ///< 分配器类型
    using size_type       = std::size_t;        ///< 大小类型
    using difference_type = std::ptrdiff_t;     ///< 差值类型
    using pointer         = value_type*;        ///< 元素指针类型
    using const_pointer   = const value_type*;  ///< 常量元素指针类型
    using reference       = value_type&;        ///< 元素引用类型
    using const_reference = const value_type&;  ///< 常量元素引用类型

public:
    /// @brief 已排序标记
    /// @details 用于从已排序的 vector 构造 multiset，可优化构建效率
    struct sorted_tag
    {
    };

    /// @brief 未排序标记
    /// @details 用于从未排序的 vector 构造 multiset
    struct unsorted_tag
    {
    };

private:
    /// @brief 红黑树节点颜色
    /// @details 红黑树通过节点颜色维护平衡性质
    enum class Color : uint8_t
    {
        RED,   ///< 红色节点
        BLACK  ///< 黑色节点
    };

    /// @brief 红黑树节点结构体
    /// @details 每个节点存储键、左右子节点指针、父节点指针和颜色
    struct Node
    {
        value_type key;     ///< 键值
        Node*      left;    ///< 左子节点指针
        Node*      right;   ///< 右子节点指针
        Node*      parent;  ///< 父节点指针
        Color      color;   ///< 节点颜色

        /// @brief 默认构造函数，创建空节点
        Node()
            : left(nullptr),
              right(nullptr),
              parent(nullptr),
              color(Color::BLACK)
        {
        }

        /// @brief 构造函数，从键创建节点
        /// @param k 键值
        explicit Node(value_type k)
            : key(std::move(k)),
              left(nullptr),
              right(nullptr),
              parent(nullptr),
              color(Color::BLACK)
        {
        }
    };

public:
    class const_iterator;

    /// @brief 双向迭代器类
    /// @details multiset 的迭代器，支持双向遍历（++ 和 --），按键的顺序访问元素
    class iterator
    {
        friend class multiset;

    public:
        using iterator_category =
            std::bidirectional_iterator_tag;          ///< 迭代器类别（双向迭代器）
        using value_type      = multiset::value_type;  ///< 元素类型
        using difference_type = multiset::difference_type;  ///< 差值类型
        using pointer         = multiset::pointer;          ///< 元素指针类型
        using reference       = multiset::reference;        ///< 元素引用类型

    public:
        /// @brief 默认构造函数，创建空迭代器
        iterator();
        /// @brief 构造函数，从节点指针和容器指针创建迭代器
        /// @param n 节点指针
        /// @param container multiset 容器指针
        explicit iterator(Node* n, multiset* container);
        /// @brief 拷贝构造函数
        /// @param other 要拷贝的迭代器
        iterator(const iterator& other);

        /// @brief 箭头操作符，返回指向元素的指针
        /// @return 指向键值的指针
        pointer operator->() const;
        /// @brief 解引用操作符，返回元素的引用
        /// @return 键值的引用
        reference operator*() const;

        /// @brief 前置自增，移动到下一个元素（按中序遍历顺序）
        /// @return 自增后的迭代器引用
        iterator& operator++();
        /// @brief 前置自减，移动到前一个元素（按中序遍历顺序）
        /// @return 自减后的迭代器引用
        iterator& operator--();
        /// @brief 后置自增，移动到下一个元素
        /// @return 自增前的迭代器副本
        iterator operator++(int);
        /// @brief 后置自减，移动到前一个元素
        /// @return 自减前的迭代器副本
        iterator operator--(int);

        /// @brief 比较两个迭代器是否相等
        /// @param other 要比较的另一个迭代器
        /// @return 如果相等返回 true，否则返回 false
        bool operator==(const iterator& other) const;
        /// @brief 比较两个迭代器是否不相等
        /// @param other 要比较的另一个迭代器
        /// @return 如果不相等返回 true，否则返回 false
        bool operator!=(const iterator& other) const;

    private:
        Node*     m_node;       ///< 当前节点指针
        multiset* m_container;  ///< 所属容器指针
    };

    /// @brief 常量双向迭代器类
    /// @details multiset 的常量迭代器，不允许修改指向的元素，支持双向遍历
    class const_iterator
    {
        friend class multiset;

    public:
        using iterator_category =
            std::bidirectional_iterator_tag;          ///< 迭代器类别（双向迭代器）
        using value_type      = multiset::value_type;  ///< 元素类型
        using difference_type = multiset::difference_type;  ///< 差值类型
        using pointer         = multiset::const_pointer;    ///< 常量元素指针类型
        using reference       = multiset::const_reference;  ///< 常量元素引用类型

    public:
        /// @brief 默认构造函数，创建空迭代器
        const_iterator();
        /// @brief 构造函数，从节点指针和容器指针创建迭代器
        /// @param n 节点指针
        /// @param container multiset 容器指针（const）
        explicit const_iterator(Node* n, const multiset* container);
        /// @brief 从非 const 迭代器构造
        /// @param other 非 const 迭代器
        const_iterator(const iterator& other);
        /// @brief 拷贝构造函数
        /// @param other 要拷贝的常量迭代器
        const_iterator(const const_iterator& other);

        /// @brief 箭头操作符，返回指向元素的常量指针
        /// @return 指向键值的常量指针
        pointer operator->() const;
        /// @brief 解引用操作符，返回元素的常量引用
        /// @return 键值的常量引用
        reference operator*() const;

        /// @brief 前置自增，移动到下一个元素（按中序遍历顺序）
        /// @return 自增后的迭代器引用
        const_iterator& operator++();
        /// @brief 前置自减，移动到前一个元素（按中序遍历顺序）
        /// @return 自减后的迭代器引用
        const_iterator& operator--();
        /// @brief 后置自增，移动到下一个元素
        /// @return 自增前的迭代器副本
        const_iterator operator++(int);
        /// @brief 后置自减，移动到前一个元素
        /// @return 自减前的迭代器副本
        const_iterator operator--(int);

        /// @brief 比较两个迭代器是否相等
        /// @param other 要比较的另一个迭代器
        /// @return 如果相等返回 true，否则返回 false
        bool operator==(const const_iterator& other) const;
        /// @brief 比较两个迭代器是否不相等
        /// @param other 要比较的另一个迭代器
        /// @return 如果不相等返回 true，否则返回 false
        bool operator!=(const const_iterator& other) const;

    private:
        Node*           m_node;       ///< 当前节点指针
        const multiset* m_container;  ///< 所属容器指针（const）
    };

    /// @brief 反向迭代器类型，使用标准库适配器
    using reverse_iterator = std::reverse_iterator<iterator>;
    /// @brief 常量反向迭代器类型，使用标准库适配器
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

public:
    // 构造函数和析构函数

    /// @brief 默认构造函数，创建空 multiset
    multiset();
    /// @brief 构造函数，使用指定的比较函数创建空 multiset
    /// @param comp 比较函数对象
    explicit multiset(const Compare& comp);
    /// @brief 非标准接口：从已排序的 vector 构造 multiset（优化版本）
    /// @param values 已排序的键值 vector
    /// @param tag 已排序标记（sorted_tag）
    multiset(std::vector<value_type>& values, sorted_tag);
    /// @brief 非标准接口：从未排序的 vector 构造 multiset
    /// @param values 未排序的键值 vector
    /// @param tag 未排序标记（unsorted_tag）
    multiset(std::vector<value_type>& values, unsorted_tag);
    /// @brief 范围构造函数，复制 [first, last) 范围内的元素
    /// @tparam InputIt 输入迭代器类型
    /// @param first 范围起始迭代器
    /// @param last 范围结束迭代器
    /// @param comp 比较函数对象，默认为 Compare()
    template <typename InputIt,
              std::enable_if_t<!std::is_integral<InputIt>::value, int> = 0>
    multiset(InputIt first, InputIt last, const Compare& comp = Compare());
    /// @brief 初始化列表构造函数
    /// @param ilist 初始化列表
    /// @param comp 比较函数对象，默认为 Compare()
    multiset(std::initializer_list<value_type> ilist,
             const Compare&                    comp = Compare());
    /// @brief 拷贝构造函数
    /// @param other 要拷贝的 multiset
    multiset(const multiset& other);
    /// @brief 移动构造函数
    /// @param other 要移动的 multiset
    multiset(multiset&& other) noexcept;
    /// @brief 析构函数，释放所有资源
    ~multiset();

    /// @brief 拷贝赋值运算符
    /// @param other 要拷贝的 multiset
    /// @return 当前 multiset 的引用
    multiset& operator=(const multiset& other);
    /// @brief 移动赋值运算符
    /// @param other 要移动的 multiset
    /// @return 当前 multiset 的引用
    multiset& operator=(multiset&& other) noexcept;
    /// @brief 初始化列表赋值运算符
    /// @param ilist 初始化列表
    /// @return 当前 multiset 的引用
    multiset& operator=(std::initializer_list<value_type> ilist);

    /// @brief 获取分配器
    /// @return 分配器对象
    allocator_type get_allocator() const;
    /// @brief 获取键比较函数对象
    /// @return 比较函数对象
    key_compare key_comp() const;

    // 迭代器

    /// @brief 返回指向第一个元素的迭代器
    /// @return 指向最小键元素的迭代器
    iterator begin() noexcept;
    /// @brief 返回指向第一个元素的常量迭代器
    /// @return 指向最小键元素的常量迭代器
    const_iterator begin() const noexcept;
    /// @brief 返回指向第一个元素的常量迭代器（const 版本）
    /// @return 指向最小键元素的常量迭代器
    const_iterator cbegin() const noexcept;
    /// @brief 返回指向末尾的迭代器
    /// @return 指向末尾的迭代器（不指向任何元素）
    iterator end() noexcept;
    /// @brief 返回指向末尾的常量迭代器
    /// @return 指向末尾的常量迭代器（不指向任何元素）
    const_iterator end() const noexcept;
    /// @brief 返回指向末尾的常量迭代器（const 版本）
    /// @return 指向末尾的常量迭代器（不指向任何元素）
    const_iterator cend() const noexcept;
    /// @brief 返回指向最后一个元素的反向迭代器
    /// @return 指向最大键元素的反向迭代器
    reverse_iterator rbegin() noexcept;
    /// @brief 返回指向最后一个元素的常量反向迭代器
    /// @return 指向最大键元素的常量反向迭代器
    const_reverse_iterator rbegin() const noexcept;
    /// @brief 返回指向最后一个元素的常量反向迭代器（const 版本）
    /// @return 指向最大键元素的常量反向迭代器
    const_reverse_iterator crbegin() const noexcept;
    /// @brief 返回指向第一个元素之前位置的反向迭代器
    /// @return 指向最小键元素之前位置的反向迭代器
    reverse_iterator rend() noexcept;
    /// @brief 返回指向第一个元素之前位置的常量反向迭代器
    /// @return 指向最小键元素之前位置的常量反向迭代器
    const_reverse_iterator rend() const noexcept;
    /// @brief 返回指向第一个元素之前位置的常量反向迭代器（const 版本）
    /// @return 指向最小键元素之前位置的常量反向迭代器
    const_reverse_iterator crend() const noexcept;

    // 容量

    /// @brief 返回 multiset 中的元素数量
    /// @return 元素数量（包含重复键的元素）
    size_type size() const noexcept;
    /// @brief 检查 multiset 是否为空
    /// @return 如果为空返回 true，否则返回 false
    bool empty() const noexcept;
    /// @brief 返回 multiset 能容纳的最大元素数量
    /// @return 最大元素数量
    size_type max_size() const noexcept;

    // 修改器

    /// @brief 清空 multiset，删除所有元素
    void clear() noexcept;
    /// @brief 插入元素（拷贝版本）
    /// @param value 要插入的键值
    /// @return 指向插入元素的迭代器（multiset 总是插入成功）
    iterator insert(const value_type& value);
    /// @brief 插入元素（移动版本）
    /// @param value 要插入的键值（右值）
    /// @return 指向插入元素的迭代器（multiset 总是插入成功）
    iterator insert(value_type&& value);
    /// @brief 在指定位置附近插入元素（拷贝版本）
    /// @param hint 提示位置（可能优化插入性能）
    /// @param value 要插入的键值
    /// @return 指向插入元素的迭代器
    iterator insert(const_iterator hint, const value_type& value);
    /// @brief 在指定位置附近插入元素（移动版本）
    /// @param hint 提示位置（可能优化插入性能）
    /// @param value 要插入的键值（右值）
    /// @return 指向插入元素的迭代器
    iterator insert(const_iterator hint, value_type&& value);
    /// @brief 插入 [first, last) 范围内的元素
    /// @tparam InputIt 输入迭代器类型
    /// @param first 范围起始迭代器
    /// @param last 范围结束迭代器
    template <typename InputIt,
              std::enable_if_t<!std::is_integral<InputIt>::value, int> = 0>
    void insert(InputIt first, InputIt last);
    /// @brief 插入初始化列表中的元素
    /// @param ilist 初始化列表
    void insert(std::initializer_list<value_type> ilist);

    /// @brief 原地构造元素并插入
    /// @tparam Args 构造参数类型
    /// @param args 构造参数
    /// @return 指向插入元素的迭代器（multiset 总是插入成功）
    template <typename... Args>
    iterator emplace(Args&&... args);
    /// @brief 在指定位置附近原地构造元素并插入
    /// @tparam Args 构造参数类型
    /// @param hint 提示位置
    /// @param args 构造参数
    /// @return 指向插入元素的迭代器
    template <typename... Args>
    iterator emplace_hint(const_iterator hint, Args&&... args);

    /// @brief 删除指定位置的元素
    /// @param pos 要删除的元素位置
    /// @return 指向被删除元素之后的迭代器，如果删除的是 end() 则返回 end()
    iterator erase(const_iterator pos);
    /// @brief 删除 [first, last) 范围内的元素
    /// @param first 范围起始迭代器
    /// @param last 范围结束迭代器
    /// @return 指向最后一个被删除元素之后的迭代器
    iterator erase(const_iterator first, const_iterator last);
    /// @brief 删除指定键的所有元素
    /// @param key 要删除的键
    /// @return 被删除的元素数量（multiset 中可能删除多个）
    size_type erase(const key_type& key);

    /// @brief 交换两个 multiset 的内容
    /// @param other 要交换的 multiset
    void swap(multiset& other) noexcept;

    /// @brief 合并另一个 multiset 的元素
    /// @tparam Compare2 另一个 multiset 的比较函数类型
    /// @param other 要合并的 multiset，合并后 other 变为空
    template <typename Compare2>
    void merge(multiset<Key, Compare2, Allocator>& other);

    // 查找

    /// @brief 查找指定键的第一个元素
    /// @param key 要查找的键
    /// @return 指向找到的第一个元素的迭代器，如果未找到则返回 end()
    iterator find(const key_type& key);
    /// @brief 查找指定键的第一个元素（const 版本）
    /// @param key 要查找的键
    /// @return 指向找到的第一个元素的常量迭代器，如果未找到则返回 cend()
    const_iterator find(const key_type& key) const;
    /// @brief 统计指定键的元素数量
    /// @param key 要查找的键
    /// @return 指定键的元素数量（multiset 中可能大于 1）
    size_type count(const key_type& key) const;
    /// @brief 返回第一个不小于指定键的元素迭代器
    /// @param key 键
    /// @return 指向第一个不小于 key 的元素迭代器，如果所有元素都小于 key 则返回
    /// end()
    iterator lower_bound(const key_type& key);
    /// @brief 返回第一个不小于指定键的元素迭代器（const 版本）
    /// @param key 键
    /// @return 指向第一个不小于 key 的元素常量迭代器，如果所有元素都小于 key
    /// 则返回 cend()
    const_iterator lower_bound(const key_type& key) const;
    /// @brief 返回第一个大于指定键的元素迭代器
    /// @param key 键
    /// @return 指向第一个大于 key 的元素迭代器，如果所有元素都小于等于 key
    /// 则返回 end()
    iterator upper_bound(const key_type& key);
    /// @brief 返回第一个大于指定键的元素迭代器（const 版本）
    /// @param key 键
    /// @return 指向第一个大于 key 的元素常量迭代器，如果所有元素都小于等于 key
    /// 则返回 cend()
    const_iterator upper_bound(const key_type& key) const;
    /// @brief 返回等于指定键的元素范围
    /// @param key 键
    /// @return 包含 lower_bound 和 upper_bound 的 pair（multiset
    /// 中范围可能包含多个元素）
    std::pair<iterator, iterator> equal_range(const key_type& key);
    /// @brief 返回等于指定键的元素范围（const 版本）
    /// @param key 键
    /// @return 包含 lower_bound 和 upper_bound 的 pair（multiset
    /// 中范围可能包含多个元素）
    std::pair<const_iterator, const_iterator> equal_range(
        const key_type& key) const;

#ifndef NDEBUG
    /// @brief 验证红黑树的性质（调试用）
    /// @return 如果树结构有效返回 true，否则返回 false
    bool validate_tree() const;
#endif

private:
    /// @brief 从已排序的 vector 构建平衡二叉搜索树
    /// @param values 已排序的键值 vector
    /// @param left 左边界索引
    /// @param right 右边界索引
    /// @param depth 当前深度（用于确定节点颜色）
    /// @return 构建的子树根节点
    Node* build_tree(std::vector<value_type>& values, int left, int right,
                     size_type depth);
    /// @brief 递归拷贝节点
    /// @param cur 当前节点
    /// @param other 源 multiset
    /// @return 拷贝后的节点
    Node* copy_node(Node* cur, const multiset& other);
    /// @brief 递归销毁节点
    /// @param node 要销毁的节点
    void destroy(Node* node);
    /// @brief 插入后平衡红黑树
    /// @param node 新插入的节点
    void insert_balance(Node* node);
    /// @brief 删除后平衡红黑树
    /// @param node 替换被删除节点的节点
    void erase_balance(Node* node);
    /// @brief 左旋操作
    /// @param node 旋转节点
    /// @return 旋转后的子树根节点
    Node* rotate_left(Node* node);
    /// @brief 右旋操作
    /// @param node 旋转节点
    /// @return 旋转后的子树根节点
    Node* rotate_right(Node* node);
    /// @brief 查找指定键的第一个节点
    /// @param key 要查找的键
    /// @return 找到的节点，如果未找到返回 m_nil
    Node* find_node(const key_type& key);
    /// @brief 查找指定键的第一个节点（const 版本）
    /// @param key 要查找的键
    /// @return 找到的节点，如果未找到返回 m_nil
    const Node* find_node(const key_type& key) const;
    /// @brief 获取子树中的最小节点
    /// @param node 子树根节点
    /// @return 子树中的最小节点
    Node* min_node(Node* node);
    /// @brief 获取子树中的最小节点（const 版本）
    /// @param node 子树根节点
    /// @return 子树中的最小节点
    const Node* min_node(const Node* node) const;
    /// @brief 获取子树中的最大节点
    /// @param node 子树根节点
    /// @return 子树中的最大节点
    Node* max_node(Node* node);
    /// @brief 获取子树中的最大节点（const 版本）
    /// @param node 子树根节点
    /// @return 子树中的最大节点
    const Node* max_node(const Node* node) const;

#ifndef NDEBUG
    /// @brief 验证红黑树性质（递归）
    /// @param node 当前节点
    /// @param black_height 黑高度（输出参数）
    /// @return 如果子树有效返回 true，否则返回 false
    bool validate_properties(Node* node, size_type& black_height) const;
#endif

private:
    /// @brief 节点分配器类型（从 value_type 分配器 rebind 而来）
    using node_alloc_type =
        typename std::allocator_traits<Allocator>::template rebind_alloc<Node>;
    /// @brief 节点分配器的 traits 类型
    using alloc_traits = std::allocator_traits<node_alloc_type>;

private:
    Node*           m_root;        ///< 红黑树的根节点
    Node*           m_nil;         ///< 哨兵节点（表示空）
    size_type       m_size;        ///< 元素数量
    Compare         m_comp;        ///< 键比较函数对象
    node_alloc_type m_node_alloc;  ///< 节点分配器
};

template <typename Key, typename Compare, typename Allocator>
multiset<Key, Compare, Allocator>::iterator::iterator()
    : m_node(nullptr), m_container(nullptr)
{
}

template <typename Key, typename Compare, typename Allocator>
multiset<Key, Compare, Allocator>::iterator::iterator(Node*     n,
                                                      multiset* container)
    : m_node(n), m_container(container)
{
}

template <typename Key, typename Compare, typename Allocator>
multiset<Key, Compare, Allocator>::iterator::iterator(const iterator& other)
    : m_node(other.m_node), m_container(other.m_container)
{
}

template <typename Key, typename Compare, typename Allocator>
inline typename multiset<Key, Compare, Allocator>::iterator::pointer
multiset<Key, Compare, Allocator>::iterator::operator->() const
{
    return &m_node->key;
}

template <typename Key, typename Compare, typename Allocator>
inline typename multiset<Key, Compare, Allocator>::iterator::reference
multiset<Key, Compare, Allocator>::iterator::operator*() const
{
    return m_node->key;
}

template <typename Key, typename Compare, typename Allocator>
inline typename multiset<Key, Compare, Allocator>::iterator&
multiset<Key, Compare, Allocator>::iterator::operator++()
{
    if (m_node->right != m_container->m_nil)
    {
        m_node = m_node->right;
        while (m_node->left != m_container->m_nil)
            m_node = m_node->left;
    }
    else
    {
        Node* parent = m_node->parent;
        while (parent != m_container->m_nil && m_node == parent->right)
        {
            m_node = parent;
            parent = parent->parent;
        }
        m_node = parent;
    }
    return *this;
}

template <typename Key, typename Compare, typename Allocator>
inline typename multiset<Key, Compare, Allocator>::iterator&
multiset<Key, Compare, Allocator>::iterator::operator--()
{
    if (m_node == m_container->m_nil)
    {
        m_node = m_container->max_node(m_container->m_root);
        return *this;
    }

    if (m_node->left != m_container->m_nil)
    {
        m_node = m_node->left;
        while (m_node->right != m_container->m_nil)
            m_node = m_node->right;
    }
    else
    {
        Node* parent = m_node->parent;
        while (parent != m_container->m_nil && m_node == parent->left)
        {
            m_node = parent;
            parent = parent->parent;
        }
        m_node = parent;
    }
    return *this;
}

template <typename Key, typename Compare, typename Allocator>
inline typename multiset<Key, Compare, Allocator>::iterator
multiset<Key, Compare, Allocator>::iterator::operator++(int)
{
    iterator temp(*this);
    ++(*this);
    return temp;
}

template <typename Key, typename Compare, typename Allocator>
inline typename multiset<Key, Compare, Allocator>::iterator
multiset<Key, Compare, Allocator>::iterator::operator--(int)
{
    iterator temp(*this);
    --(*this);
    return temp;
}

template <typename Key, typename Compare, typename Allocator>
inline bool multiset<Key, Compare, Allocator>::iterator::operator==(
    const iterator& other) const
{
    return m_node == other.m_node;
}

template <typename Key, typename Compare, typename Allocator>
inline bool multiset<Key, Compare, Allocator>::iterator::operator!=(
    const iterator& other) const
{
    return m_node != other.m_node;
}

template <typename Key, typename Compare, typename Allocator>
multiset<Key, Compare, Allocator>::const_iterator::const_iterator()
    : m_node(nullptr), m_container(nullptr)
{
}

template <typename Key, typename Compare, typename Allocator>
multiset<Key, Compare, Allocator>::const_iterator::const_iterator(
    Node* n, const multiset* container)
    : m_node(n), m_container(container)
{
}

template <typename Key, typename Compare, typename Allocator>
multiset<Key, Compare, Allocator>::const_iterator::const_iterator(
    const const_iterator& other)
    : m_node(other.m_node), m_container(other.m_container)
{
}

template <typename Key, typename Compare, typename Allocator>
multiset<Key, Compare, Allocator>::const_iterator::const_iterator(
    const iterator& other)
    : m_node(other.m_node), m_container(other.m_container)
{
}

template <typename Key, typename Compare, typename Allocator>
inline typename multiset<Key, Compare, Allocator>::const_iterator::pointer
multiset<Key, Compare, Allocator>::const_iterator::operator->() const
{
    return &m_node->key;
}

template <typename Key, typename Compare, typename Allocator>
inline typename multiset<Key, Compare, Allocator>::const_iterator::reference
multiset<Key, Compare, Allocator>::const_iterator::operator*() const
{
    return m_node->key;
}

template <typename Key, typename Compare, typename Allocator>
inline typename multiset<Key, Compare, Allocator>::const_iterator&
multiset<Key, Compare, Allocator>::const_iterator::operator++()
{
    if (m_node->right != m_container->m_nil)
    {
        m_node = m_node->right;
        while (m_node->left != m_container->m_nil)
            m_node = m_node->left;
    }
    else
    {
        Node* parent = m_node->parent;
        while (parent != m_container->m_nil && m_node == parent->right)
        {
            m_node = parent;
            parent = parent->parent;
        }
        m_node = parent;
    }
    return *this;
}

template <typename Key, typename Compare, typename Allocator>
inline typename multiset<Key, Compare, Allocator>::const_iterator&
multiset<Key, Compare, Allocator>::const_iterator::operator--()
{
    if (m_node == m_container->m_nil)
    {
        m_node = const_cast<Node*>(m_container->max_node(m_container->m_root));
        return *this;
    }

    if (m_node->left != m_container->m_nil)
    {
        m_node = m_node->left;
        while (m_node->right != m_container->m_nil)
            m_node = m_node->right;
    }
    else
    {
        Node* parent = m_node->parent;
        while (parent != m_container->m_nil && m_node == parent->left)
        {
            m_node = parent;
            parent = parent->parent;
        }
        m_node = parent;
    }
    return *this;
}

template <typename Key, typename Compare, typename Allocator>
inline typename multiset<Key, Compare, Allocator>::const_iterator
multiset<Key, Compare, Allocator>::const_iterator::operator++(int)
{
    const_iterator temp(*this);
    ++(*this);
    return temp;
}

template <typename Key, typename Compare, typename Allocator>
inline typename multiset<Key, Compare, Allocator>::const_iterator
multiset<Key, Compare, Allocator>::const_iterator::operator--(int)
{
    const_iterator temp(*this);
    --(*this);
    return temp;
}

template <typename Key, typename Compare, typename Allocator>
inline bool multiset<Key, Compare, Allocator>::const_iterator::operator==(
    const const_iterator& other) const
{
    return m_node == other.m_node;
}

template <typename Key, typename Compare, typename Allocator>
inline bool multiset<Key, Compare, Allocator>::const_iterator::operator!=(
    const const_iterator& other) const
{
    return m_node != other.m_node;
}

template <typename Key, typename Compare, typename Allocator>
multiset<Key, Compare, Allocator>::multiset()
    : m_root(nullptr), m_nil(nullptr), m_size(0), m_comp(), m_node_alloc()
{
    m_nil = alloc_traits::allocate(m_node_alloc, 1);
    alloc_traits::construct(m_node_alloc, m_nil);

    m_root         = m_nil;
    m_root->parent = m_nil;
    m_root->left   = m_nil;
    m_root->right  = m_nil;
}

template <typename Key, typename Compare, typename Allocator>
multiset<Key, Compare, Allocator>::multiset(const Compare& comp)
    : m_root(nullptr), m_nil(nullptr), m_size(0), m_comp(comp), m_node_alloc()
{
    m_nil = alloc_traits::allocate(m_node_alloc, 1);
    alloc_traits::construct(m_node_alloc, m_nil);

    m_root         = m_nil;
    m_root->parent = m_nil;
    m_root->left   = m_nil;
    m_root->right  = m_nil;
}

template <typename Key, typename Compare, typename Allocator>
template <typename InputIt,
          std::enable_if_t<!std::is_integral<InputIt>::value, int>>
multiset<Key, Compare, Allocator>::multiset(InputIt first, InputIt last,
                                            const Compare& comp)
    : m_root(nullptr), m_nil(nullptr), m_size(0), m_comp(comp), m_node_alloc()
{
    m_nil = alloc_traits::allocate(m_node_alloc, 1);
    alloc_traits::construct(m_node_alloc, m_nil);

    m_root         = m_nil;
    m_root->parent = m_nil;
    m_root->left   = m_nil;
    m_root->right  = m_nil;

    for (; first != last; ++first)
        insert(*first);
}

template <typename Key, typename Compare, typename Allocator>
multiset<Key, Compare, Allocator>::multiset(
    std::initializer_list<value_type> ilist, const Compare& comp)
    : m_root(nullptr), m_nil(nullptr), m_size(0), m_comp(comp), m_node_alloc()
{
    m_nil = alloc_traits::allocate(m_node_alloc, 1);
    alloc_traits::construct(m_node_alloc, m_nil);

    m_root         = m_nil;
    m_root->parent = m_nil;
    m_root->left   = m_nil;
    m_root->right  = m_nil;

    for (const value_type& val : ilist)
        insert(val);
}

template <typename Key, typename Compare, typename Allocator>
multiset<Key, Compare, Allocator>::multiset(std::vector<value_type>& values,
                                            unsorted_tag)
    : m_root(nullptr), m_nil(nullptr), m_size(0), m_comp(), m_node_alloc()
{
    m_nil = alloc_traits::allocate(m_node_alloc, 1);
    alloc_traits::construct(m_node_alloc, m_nil);

    m_root         = m_nil;
    m_root->parent = m_nil;
    m_root->left   = m_nil;
    m_root->right  = m_nil;

    for (const value_type& val : values)
        insert(val);
}

template <typename Key, typename Compare, typename Allocator>
multiset<Key, Compare, Allocator>::multiset(std::vector<value_type>& values,
                                            sorted_tag)
    : m_root(nullptr),
      m_nil(nullptr),
      m_size(values.size()),
      m_comp(),
      m_node_alloc()
{
    m_nil = alloc_traits::allocate(m_node_alloc, 1);
    alloc_traits::construct(m_node_alloc, m_nil);

    m_root = build_tree(values, 0, static_cast<int>(values.size()) - 1, 0);
    m_root->parent = m_nil;

    if (values.empty())
    {
        m_root->left  = m_nil;
        m_root->right = m_nil;
    }
}

template <typename Key, typename Compare, typename Allocator>
multiset<Key, Compare, Allocator>::multiset(const multiset& other)
    : m_root(nullptr),
      m_nil(nullptr),
      m_size(other.m_size),
      m_comp(other.m_comp),
      m_node_alloc(other.m_node_alloc)
{
    m_nil = alloc_traits::allocate(m_node_alloc, 1);
    alloc_traits::construct(m_node_alloc, m_nil);

    m_root         = copy_node(other.m_root, other);
    m_root->parent = m_nil;
}

template <typename Key, typename Compare, typename Allocator>
multiset<Key, Compare, Allocator>::multiset(multiset&& other) noexcept
    : m_root(other.m_root),
      m_nil(other.m_nil),
      m_size(other.m_size),
      m_comp(std::move(other.m_comp)),
      m_node_alloc(std::move(other.m_node_alloc))
{
    other.m_nil = alloc_traits::allocate(other.m_node_alloc, 1);
    alloc_traits::construct(other.m_node_alloc, other.m_nil);
    other.m_root         = other.m_nil;
    other.m_root->parent = other.m_nil;
    other.m_root->left   = other.m_nil;
    other.m_root->right  = other.m_nil;
    other.m_size         = 0;
}

template <typename Key, typename Compare, typename Allocator>
multiset<Key, Compare, Allocator>::~multiset()
{
    destroy(m_root);
    alloc_traits::destroy(m_node_alloc, m_nil);
    alloc_traits::deallocate(m_node_alloc, m_nil, 1);
}

template <typename Key, typename Compare, typename Allocator>
inline multiset<Key, Compare, Allocator>&
multiset<Key, Compare, Allocator>::operator=(const multiset& other)
{
    if (this == &other)
        return *this;
    clear();
    insert(other.begin(), other.end());
    return *this;
}

template <typename Key, typename Compare, typename Allocator>
inline multiset<Key, Compare, Allocator>&
multiset<Key, Compare, Allocator>::operator=(multiset&& other) noexcept
{
    if (this == &other)
        return *this;

    clear();
    m_root       = other.m_root;
    m_nil        = other.m_nil;
    m_size       = other.m_size;
    m_comp       = std::move(other.m_comp);
    m_node_alloc = std::move(other.m_node_alloc);

    other.m_nil = alloc_traits::allocate(other.m_node_alloc, 1);
    alloc_traits::construct(other.m_node_alloc, other.m_nil);
    other.m_root         = other.m_nil;
    other.m_root->parent = other.m_nil;
    other.m_root->left   = other.m_nil;
    other.m_root->right  = other.m_nil;
    other.m_size         = 0;

    return *this;
}

template <typename Key, typename Compare, typename Allocator>
inline multiset<Key, Compare, Allocator>&
multiset<Key, Compare, Allocator>::operator=(
    std::initializer_list<value_type> ilist)
{
    clear();
    insert(ilist.begin(), ilist.end());
    return *this;
}

template <typename Key, typename Compare, typename Allocator>
inline typename multiset<Key, Compare, Allocator>::allocator_type
multiset<Key, Compare, Allocator>::get_allocator() const
{
    return typename multiset<Key, Compare, Allocator>::allocator_type();
}

template <typename Key, typename Compare, typename Allocator>
inline typename multiset<Key, Compare, Allocator>::key_compare
multiset<Key, Compare, Allocator>::key_comp() const
{
    return m_comp;
}

template <typename Key, typename Compare, typename Allocator>
inline typename multiset<Key, Compare, Allocator>::iterator
multiset<Key, Compare, Allocator>::begin() noexcept
{
    Node* m_node = m_root;
    if (m_node != m_nil)
    {
        while (m_node->left != m_nil)
            m_node = m_node->left;
    }
    return iterator(m_node, this);
}

template <typename Key, typename Compare, typename Allocator>
inline typename multiset<Key, Compare, Allocator>::const_iterator
multiset<Key, Compare, Allocator>::begin() const noexcept
{
    Node* m_node = m_root;
    if (m_node != m_nil)
    {
        while (m_node->left != m_nil)
            m_node = m_node->left;
    }
    return const_iterator(m_node, this);
}

template <typename Key, typename Compare, typename Allocator>
inline typename multiset<Key, Compare, Allocator>::const_iterator
multiset<Key, Compare, Allocator>::cbegin() const noexcept
{
    return begin();
}

template <typename Key, typename Compare, typename Allocator>
inline typename multiset<Key, Compare, Allocator>::iterator
multiset<Key, Compare, Allocator>::end() noexcept
{
    return iterator(m_nil, this);
}

template <typename Key, typename Compare, typename Allocator>
inline typename multiset<Key, Compare, Allocator>::const_iterator
multiset<Key, Compare, Allocator>::end() const noexcept
{
    return const_iterator(m_nil, this);
}

template <typename Key, typename Compare, typename Allocator>
inline typename multiset<Key, Compare, Allocator>::const_iterator
multiset<Key, Compare, Allocator>::cend() const noexcept
{
    return const_iterator(m_nil, this);
}

template <typename Key, typename Compare, typename Allocator>
inline typename multiset<Key, Compare, Allocator>::reverse_iterator
multiset<Key, Compare, Allocator>::rbegin() noexcept
{
    return reverse_iterator(end());
}

template <typename Key, typename Compare, typename Allocator>
inline typename multiset<Key, Compare, Allocator>::const_reverse_iterator
multiset<Key, Compare, Allocator>::rbegin() const noexcept
{
    return const_reverse_iterator(cend());
}

template <typename Key, typename Compare, typename Allocator>
inline typename multiset<Key, Compare, Allocator>::const_reverse_iterator
multiset<Key, Compare, Allocator>::crbegin() const noexcept
{
    return const_reverse_iterator(cend());
}

template <typename Key, typename Compare, typename Allocator>
inline typename multiset<Key, Compare, Allocator>::reverse_iterator
multiset<Key, Compare, Allocator>::rend() noexcept
{
    return reverse_iterator(begin());
}

template <typename Key, typename Compare, typename Allocator>
inline typename multiset<Key, Compare, Allocator>::const_reverse_iterator
multiset<Key, Compare, Allocator>::rend() const noexcept
{
    return const_reverse_iterator(cbegin());
}

template <typename Key, typename Compare, typename Allocator>
inline typename multiset<Key, Compare, Allocator>::const_reverse_iterator
multiset<Key, Compare, Allocator>::crend() const noexcept
{
    return const_reverse_iterator(cbegin());
}

template <typename Key, typename Compare, typename Allocator>
inline typename multiset<Key, Compare, Allocator>::size_type
multiset<Key, Compare, Allocator>::size() const noexcept
{
    return m_size;
}

template <typename Key, typename Compare, typename Allocator>
inline bool multiset<Key, Compare, Allocator>::empty() const noexcept
{
    return m_root == m_nil;
}

template <typename Key, typename Compare, typename Allocator>
inline typename multiset<Key, Compare, Allocator>::size_type
multiset<Key, Compare, Allocator>::max_size() const noexcept
{
    return alloc_traits::max_size(m_node_alloc);
}

template <typename Key, typename Compare, typename Allocator>
inline void multiset<Key, Compare, Allocator>::clear() noexcept
{
    if (empty())
        return;

    destroy(m_root);
    m_root = m_nil;
    m_size = 0;
}

template <typename Key, typename Compare, typename Allocator>
inline typename multiset<Key, Compare, Allocator>::iterator
multiset<Key, Compare, Allocator>::insert(const value_type& value)
{
    value_type val = value;
    return insert(std::move(val));
}

template <typename Key, typename Compare, typename Allocator>
inline typename multiset<Key, Compare, Allocator>::iterator
multiset<Key, Compare, Allocator>::insert(value_type&& value)
{
    Node* parent = m_nil;
    Node* cur    = m_root;

    // multiset: 允许重复键，因此当键相等时，继续向右子树搜索
    while (cur != m_nil)
    {
        parent = cur;
        if (m_comp(value, cur->key))
            cur = cur->left;
        else
            // 键相等或大于时，都插入到右子树
            cur = cur->right;
    }

    Node* new_node = alloc_traits::allocate(m_node_alloc, 1);
    alloc_traits::construct(m_node_alloc, new_node, std::move(value));
    new_node->color  = Color::RED;
    new_node->left   = m_nil;
    new_node->right  = m_nil;
    new_node->parent = parent;

    if (parent == m_nil)
        m_root = new_node;
    else if (m_comp(new_node->key, parent->key))
        parent->left = new_node;
    else
        parent->right = new_node;

    m_size++;
    insert_balance(new_node);

    return iterator(new_node, this);
}

template <typename Key, typename Compare, typename Allocator>
inline typename multiset<Key, Compare, Allocator>::iterator
multiset<Key, Compare, Allocator>::insert(const_iterator    pos,
                                          const value_type& value)
{
    value_type val = value;
    return insert(pos, std::move(val));
}

template <typename Key, typename Compare, typename Allocator>
inline typename multiset<Key, Compare, Allocator>::iterator
multiset<Key, Compare, Allocator>::insert(const_iterator pos, value_type&& value)
{
    Node* parent = m_nil;
    Node* cur    = m_root;

    if (pos != cend())
    {
        cur = pos.m_node;
        if (m_comp(value, cur->key))
        {
            parent = cur;
            cur    = cur->left;
        }
        else
        {
            // multiset: 键相等或大于时，从pos的右子节点开始比较
            parent = cur;
            cur    = cur->right;
        }
    }

    // multiset: 允许重复键，因此当键相等时，继续向右子树搜索
    while (cur != m_nil)
    {
        parent = cur;
        if (m_comp(value, cur->key))
            cur = cur->left;
        else
            cur = cur->right;
    }

    Node* new_node = alloc_traits::allocate(m_node_alloc, 1);
    alloc_traits::construct(m_node_alloc, new_node, std::move(value));
    new_node->color  = Color::RED;
    new_node->left   = m_nil;
    new_node->right  = m_nil;
    new_node->parent = parent;

    if (parent == m_nil)
        m_root = new_node;
    else if (m_comp(new_node->key, parent->key))
        parent->left = new_node;
    else
        parent->right = new_node;

    m_size++;
    insert_balance(new_node);

    return iterator(new_node, this);
}

template <typename Key, typename Compare, typename Allocator>
template <typename InputIt,
          std::enable_if_t<!std::is_integral<InputIt>::value, int>>
inline void multiset<Key, Compare, Allocator>::insert(InputIt first,
                                                      InputIt last)
{
    for (; first != last; ++first)
        insert(*first);
}

template <typename Key, typename Compare, typename Allocator>
inline void multiset<Key, Compare, Allocator>::insert(
    std::initializer_list<value_type> ilist)
{
    for (const value_type& val : ilist)
        insert(val);
}

template <typename Key, typename Compare, typename Allocator>
template <typename... Args>
inline typename multiset<Key, Compare, Allocator>::iterator
multiset<Key, Compare, Allocator>::emplace(Args&&... args)
{
    return insert(value_type(std::forward<Args>(args)...));
}

template <typename Key, typename Compare, typename Allocator>
template <typename... Args>
inline typename multiset<Key, Compare, Allocator>::iterator
multiset<Key, Compare, Allocator>::emplace_hint(const_iterator hint,
                                                Args&&...     args)
{
    return insert(hint, value_type(std::forward<Args>(args)...));
}

template <typename Key, typename Compare, typename Allocator>
inline typename multiset<Key, Compare, Allocator>::iterator
multiset<Key, Compare, Allocator>::erase(const_iterator pos)
{
    Node* cur = pos.m_node;
    if (cur == m_nil)
        return iterator(m_nil, this);

    iterator ret = iterator(cur, this);
    ret++;

    Node* node_to_replace = cur;
    Node* fill_node       = m_nil;
    Color original_color  = node_to_replace->color;

    if (cur->left == m_nil)
    {
        fill_node = cur->right;
        if (cur->parent == m_nil)
        {
            m_root = fill_node;
        }
        else if (cur == cur->parent->left)
        {
            cur->parent->left = fill_node;
        }
        else
        {
            cur->parent->right = fill_node;
        }
        if (fill_node != m_nil)
            fill_node->parent = cur->parent;
    }
    else if (cur->right == m_nil)
    {
        fill_node = cur->left;
        if (cur->parent == m_nil)
        {
            m_root = fill_node;
        }
        else if (cur->parent->left == cur)
        {
            cur->parent->left = fill_node;
        }
        else
        {
            cur->parent->right = fill_node;
        }
        if (fill_node != m_nil)
            fill_node->parent = cur->parent;
    }
    else
    {
        node_to_replace = cur->right;
        while (node_to_replace->left != m_nil)
        {
            node_to_replace = node_to_replace->left;
        }
        original_color = node_to_replace->color;
        fill_node      = node_to_replace->right;

        if (node_to_replace->parent == cur)
        {
        }
        else
        {
            if (node_to_replace->parent->left == node_to_replace)
            {
                node_to_replace->parent->left = fill_node;
            }
            else
            {
                node_to_replace->parent->right = fill_node;
            }
            fill_node->parent = node_to_replace->parent;

            node_to_replace->right         = cur->right;
            node_to_replace->right->parent = node_to_replace;
        }

        if (cur->parent == m_nil)
        {
            m_root = node_to_replace;
        }
        else if (cur == cur->parent->left)
        {
            cur->parent->left = node_to_replace;
        }
        else
        {
            cur->parent->right = node_to_replace;
        }
        node_to_replace->parent       = cur->parent;
        node_to_replace->color        = cur->color;
        node_to_replace->left         = cur->left;
        node_to_replace->left->parent = node_to_replace;
    }

    alloc_traits::destroy(m_node_alloc, cur);
    alloc_traits::deallocate(m_node_alloc, cur, 1);
    m_size--;

    if (original_color == Color::BLACK)
        erase_balance(fill_node);

    return ret;
}

template <typename Key, typename Compare, typename Allocator>
inline typename multiset<Key, Compare, Allocator>::iterator
multiset<Key, Compare, Allocator>::erase(const_iterator first,
                                         const_iterator last)
{
    iterator ret(last.m_node, const_cast<multiset*>(this));
    while (first != last)
        first = erase(first);
    return ret;
}

template <typename Key, typename Compare, typename Allocator>
inline typename multiset<Key, Compare, Allocator>::size_type
multiset<Key, Compare, Allocator>::erase(const key_type& key)
{
    // multiset: 删除所有匹配键的元素，返回删除的元素数量
    size_type count = 0;

    auto range = equal_range(key);
    while (range.first != range.second)
    {
        range.first = erase(range.first);
        count++;
    }

    return count;
}

template <typename Key, typename Compare, typename Allocator>
inline void multiset<Key, Compare, Allocator>::swap(multiset& other) noexcept
{
    std::swap(m_root, other.m_root);
    std::swap(m_nil, other.m_nil);
    std::swap(m_size, other.m_size);
    std::swap(m_comp, other.m_comp);
    std::swap(m_node_alloc, other.m_node_alloc);
}

template <typename Key, typename Compare, typename Allocator>
template <typename Compare2>
inline void multiset<Key, Compare, Allocator>::merge(
    multiset<Key, Compare2, Allocator>& other)
{
    insert(other.begin(), other.end());
}

template <typename Key, typename Compare, typename Allocator>
inline typename multiset<Key, Compare, Allocator>::iterator
multiset<Key, Compare, Allocator>::find(const key_type& key)
{
    Node* cur = find_node(key);
    return iterator(cur, this);
}

template <typename Key, typename Compare, typename Allocator>
inline typename multiset<Key, Compare, Allocator>::const_iterator
multiset<Key, Compare, Allocator>::find(const key_type& key) const
{
    const Node* cur = find_node(key);
    return const_iterator(const_cast<Node*>(cur), this);
}

template <typename Key, typename Compare, typename Allocator>
inline typename multiset<Key, Compare, Allocator>::size_type
multiset<Key, Compare, Allocator>::count(const key_type& key) const
{
    // multiset: 利用 equal_range 统计指定键的元素数量
    auto     range  = equal_range(key);
    size_type result = 0;
    for (auto it = range.first; it != range.second; ++it)
        result++;
    return result;
}

template <typename Key, typename Compare, typename Allocator>
inline typename multiset<Key, Compare, Allocator>::iterator
multiset<Key, Compare, Allocator>::lower_bound(const key_type& key)
{
    Node* cur    = m_root;
    Node* result = m_nil;
    while (cur != m_nil)
    {
        if (!m_comp(cur->key, key))
        {
            result = cur;
            cur    = cur->left;
        }
        else
            cur = cur->right;
    }
    return iterator(result, this);
}

template <typename Key, typename Compare, typename Allocator>
inline typename multiset<Key, Compare, Allocator>::const_iterator
multiset<Key, Compare, Allocator>::lower_bound(const key_type& key) const
{
    Node* cur    = m_root;
    Node* result = m_nil;
    while (cur != m_nil)
    {
        if (!m_comp(cur->key, key))
        {
            result = cur;
            cur    = cur->left;
        }
        else
            cur = cur->right;
    }
    return const_iterator(result, this);
}

template <typename Key, typename Compare, typename Allocator>
inline typename multiset<Key, Compare, Allocator>::iterator
multiset<Key, Compare, Allocator>::upper_bound(const key_type& key)
{
    Node* cur    = m_root;
    Node* result = m_nil;
    while (cur != m_nil)
    {
        if (m_comp(key, cur->key))
        {
            result = cur;
            cur    = cur->left;
        }
        else
            cur = cur->right;
    }
    return iterator(result, this);
}

template <typename Key, typename Compare, typename Allocator>
inline typename multiset<Key, Compare, Allocator>::const_iterator
multiset<Key, Compare, Allocator>::upper_bound(const key_type& key) const
{
    Node* cur    = m_root;
    Node* result = m_nil;
    while (cur != m_nil)
    {
        if (m_comp(key, cur->key))
        {
            result = cur;
            cur    = cur->left;
        }
        else
            cur = cur->right;
    }
    return const_iterator(result, this);
}

template <typename Key, typename Compare, typename Allocator>
inline std::pair<typename multiset<Key, Compare, Allocator>::iterator,
                 typename multiset<Key, Compare, Allocator>::iterator>
multiset<Key, Compare, Allocator>::equal_range(const key_type& key)
{
    return std::make_pair(lower_bound(key), upper_bound(key));
}

template <typename Key, typename Compare, typename Allocator>
inline std::pair<typename multiset<Key, Compare, Allocator>::const_iterator,
                 typename multiset<Key, Compare, Allocator>::const_iterator>
multiset<Key, Compare, Allocator>::equal_range(const key_type& key) const
{
    return std::make_pair(lower_bound(key), upper_bound(key));
}

template <typename Key, typename Compare, typename Allocator>
inline typename multiset<Key, Compare, Allocator>::Node*
multiset<Key, Compare, Allocator>::build_tree(std::vector<value_type>& values,
                                              int left, int right,
                                              size_type depth)
{
    if (values.empty() || left > right)
        return m_nil;

    int               mid = left + (right - left) / 2;
    const value_type& val = values[mid];

    Node* root = alloc_traits::allocate(m_node_alloc, 1);
    alloc_traits::construct(m_node_alloc, root, val);
    root->color = (depth & 1) == 0 ? Color::BLACK : Color::RED;

    root->left  = build_tree(values, left, mid - 1, depth + 1);
    root->right = build_tree(values, mid + 1, right, depth + 1);

    if (root->left != m_nil)
        root->left->parent = root;
    if (root->right != m_nil)
        root->right->parent = root;

    return root;
}

template <typename Key, typename Compare, typename Allocator>
inline typename multiset<Key, Compare, Allocator>::Node*
multiset<Key, Compare, Allocator>::copy_node(Node*             cur,
                                             const multiset& other)
{
    if (cur == other.m_nil)
        return m_nil;

    Node* root = alloc_traits::allocate(m_node_alloc, 1);
    alloc_traits::construct(m_node_alloc, root, cur->key);
    root->color = cur->color;

    root->left  = copy_node(cur->left, other);
    root->right = copy_node(cur->right, other);

    if (root->left != m_nil)
        root->left->parent = root;
    if (root->right != m_nil)
        root->right->parent = root;

    return root;
}

template <typename Key, typename Compare, typename Allocator>
inline void multiset<Key, Compare, Allocator>::destroy(Node* m_node)
{
    if (m_node == m_nil)
        return;

    destroy(m_node->left);
    destroy(m_node->right);

    alloc_traits::destroy(m_node_alloc, m_node);
    alloc_traits::deallocate(m_node_alloc, m_node, 1);
}

template <typename Key, typename Compare, typename Allocator>
inline void multiset<Key, Compare, Allocator>::insert_balance(Node* m_node)
{
    while (m_node != m_root && m_node->parent->color == Color::RED)
    {
        Node* parent  = m_node->parent;
        Node* grandpa = parent->parent;

        if (parent == grandpa->left)
        {
            Node* uncle = grandpa->right;

            if (uncle->color == Color::RED)
            {
                parent->color  = Color::BLACK;
                uncle->color   = Color::BLACK;
                grandpa->color = Color::RED;
                m_node         = grandpa;
            }
            else
            {
                if (m_node == parent->right)
                {
                    m_node = parent;
                    rotate_left(m_node);
                    parent = m_node->parent;
                }
                parent->color  = Color::BLACK;
                grandpa->color = Color::RED;
                rotate_right(grandpa);
            }
        }
        else
        {
            Node* uncle = grandpa->left;

            if (uncle->color == Color::RED)
            {
                parent->color  = Color::BLACK;
                uncle->color   = Color::BLACK;
                grandpa->color = Color::RED;
                m_node         = grandpa;
            }
            else
            {
                if (m_node == parent->left)
                {
                    m_node = parent;
                    rotate_right(m_node);
                    parent = m_node->parent;
                }
                parent->color  = Color::BLACK;
                grandpa->color = Color::RED;
                rotate_left(grandpa);
            }
        }
    }

    m_root->color = Color::BLACK;
}

template <typename Key, typename Compare, typename Allocator>
inline void multiset<Key, Compare, Allocator>::erase_balance(Node* m_node)
{
    while (m_node != m_root && m_node->color == Color::BLACK)
    {
        Node* parent  = m_node->parent;
        Node* sibling = m_nil;

        if (m_node == parent->left)
        {
            sibling = parent->right;
            if (sibling->color == Color::RED)
            {
                sibling->color = Color::BLACK;
                parent->color  = Color::RED;
                rotate_left(parent);
                sibling = parent->right;
            }

            if (sibling->left->color == Color::BLACK &&
                sibling->right->color == Color::BLACK)
            {
                sibling->color = Color::RED;
                m_node         = parent;
            }
            else
            {
                if (sibling->right->color == Color::BLACK)
                {
                    sibling->left->color = Color::BLACK;
                    sibling->color       = Color::RED;
                    rotate_right(sibling);
                    sibling = parent->right;
                }

                sibling->color        = parent->color;
                parent->color         = Color::BLACK;
                sibling->right->color = Color::BLACK;
                rotate_left(parent);
                m_node = m_root;
            }
        }
        else
        {
            sibling = parent->left;

            if (sibling->color == Color::RED)
            {
                sibling->color = Color::BLACK;
                parent->color  = Color::RED;
                rotate_right(parent);
                sibling = parent->left;
            }

            if (sibling->right->color == Color::BLACK &&
                sibling->left->color == Color::BLACK)
            {
                sibling->color = Color::RED;
                m_node         = parent;
            }
            else
            {
                if (sibling->left->color == Color::BLACK)
                {
                    sibling->right->color = Color::BLACK;
                    sibling->color        = Color::RED;
                    rotate_left(sibling);
                    sibling = parent->left;
                }

                sibling->color       = parent->color;
                parent->color        = Color::BLACK;
                sibling->left->color = Color::BLACK;
                rotate_right(parent);
                m_node = m_root;
            }
        }
    }

    m_node->color = Color::BLACK;
}

template <typename Key, typename Compare, typename Allocator>
inline typename multiset<Key, Compare, Allocator>::Node*
multiset<Key, Compare, Allocator>::rotate_left(Node* m_node)
{
    Node* rotation_center = m_node->right;
    Node* parent          = m_node->parent;

    m_node->right = rotation_center->left;
    if (rotation_center->left != m_nil)
        rotation_center->left->parent = m_node;

    rotation_center->parent = parent;
    if (parent == m_nil)
    {
        m_root = rotation_center;
    }
    else if (m_node == parent->left)
    {
        parent->left = rotation_center;
    }
    else
    {
        parent->right = rotation_center;
    }

    rotation_center->left = m_node;
    m_node->parent        = rotation_center;

    return rotation_center;
}

template <typename Key, typename Compare, typename Allocator>
inline typename multiset<Key, Compare, Allocator>::Node*
multiset<Key, Compare, Allocator>::rotate_right(Node* m_node)
{
    Node* parent          = m_node->parent;
    Node* rotation_center = m_node->left;

    m_node->left = rotation_center->right;
    if (rotation_center->right != m_nil)
        rotation_center->right->parent = m_node;

    rotation_center->parent = parent;
    if (parent == m_nil)
    {
        m_root = rotation_center;
    }
    else if (m_node == parent->left)
    {
        parent->left = rotation_center;
    }
    else
    {
        parent->right = rotation_center;
    }

    rotation_center->right = m_node;
    m_node->parent         = rotation_center;

    return rotation_center;
}

template <typename Key, typename Compare, typename Allocator>
inline typename multiset<Key, Compare, Allocator>::Node*
multiset<Key, Compare, Allocator>::find_node(const key_type& key)
{
    Node* cur = m_root;
    while (cur != m_nil)
    {
        if (m_comp(key, cur->key))
            cur = cur->left;
        else if (m_comp(cur->key, key))
            cur = cur->right;
        else
            return cur;
    }

    return m_nil;
}

template <typename Key, typename Compare, typename Allocator>
inline const typename multiset<Key, Compare, Allocator>::Node*
multiset<Key, Compare, Allocator>::find_node(const key_type& key) const
{
    const Node* cur = m_root;
    while (cur != m_nil)
    {
        if (m_comp(key, cur->key))
            cur = cur->left;
        else if (m_comp(cur->key, key))
            cur = cur->right;
        else
            return cur;
    }

    return m_nil;
}

template <typename Key, typename Compare, typename Allocator>
inline typename multiset<Key, Compare, Allocator>::Node*
multiset<Key, Compare, Allocator>::min_node(Node* m_node)
{
    while (m_node->left != m_nil)
        m_node = m_node->left;
    return m_node;
}

template <typename Key, typename Compare, typename Allocator>
inline const typename multiset<Key, Compare, Allocator>::Node*
multiset<Key, Compare, Allocator>::min_node(const Node* m_node) const
{
    while (m_node->left != m_nil)
        m_node = m_node->left;
    return m_node;
}

template <typename Key, typename Compare, typename Allocator>
inline typename multiset<Key, Compare, Allocator>::Node*
multiset<Key, Compare, Allocator>::max_node(Node* m_node)
{
    while (m_node->right != m_nil)
        m_node = m_node->right;
    return m_node;
}

template <typename Key, typename Compare, typename Allocator>
inline const typename multiset<Key, Compare, Allocator>::Node*
multiset<Key, Compare, Allocator>::max_node(const Node* m_node) const
{
    while (m_node->right != m_nil)
        m_node = m_node->right;
    return m_node;
}

#ifndef NDEBUG
template <typename Key, typename Compare, typename Allocator>
inline bool multiset<Key, Compare, Allocator>::validate_tree() const
{
    if (m_root == m_nil)
        return true;

    if (m_root->color == Color::RED)
        return false;

    size_type black_height = 0;
    return validate_properties(m_root, black_height);
}

template <typename Key, typename Compare, typename Allocator>
inline bool multiset<Key, Compare, Allocator>::validate_properties(
    Node* m_node, size_type& black_height) const
{
    if (m_node == m_nil)
    {
        black_height = 1;
        return true;
    }

    if (m_node->color == Color::RED)
    {
        if (m_node->left != m_nil && m_node->left->color == Color::RED)
            return false;
        if (m_node->right != m_nil && m_node->right->color == Color::RED)
            return false;
    }

    size_type left_black_height  = 0;
    size_type right_black_height = 0;
    if (!validate_properties(m_node->left, left_black_height))
        return false;
    if (!validate_properties(m_node->right, right_black_height))
        return false;
    if (left_black_height != right_black_height)
        return false;

    black_height = left_black_height + (m_node->color == Color::BLACK ? 1 : 0);

    return true;
}
#endif

/// @brief 比较两个 multiset 是否相等
/// @tparam Key 键类型
/// @tparam Compare 比较函数对象类型
/// @tparam Allocator 分配器类型
/// @param lhs 左操作数
/// @param rhs 右操作数
/// @return 如果两个 multiset 相等返回 true，否则返回 false
template <typename Key, typename Compare, typename Allocator>
inline bool operator==(const multiset<Key, Compare, Allocator>& lhs,
                       const multiset<Key, Compare, Allocator>& rhs)
{
    if (lhs.size() != rhs.size())
        return false;

    auto it_lhs = lhs.begin();
    auto it_rhs = rhs.begin();
    for (; it_lhs != lhs.end() && it_rhs != rhs.end(); ++it_lhs, ++it_rhs)
    {
        if (*it_lhs != *it_rhs)
            return false;
    }
    return true;
}

/// @brief 比较两个 multiset 是否不相等
/// @tparam Key 键类型
/// @tparam Compare 比较函数对象类型
/// @tparam Allocator 分配器类型
/// @param lhs 左操作数
/// @param rhs 右操作数
/// @return 如果两个 multiset 不相等返回 true，否则返回 false
template <typename Key, typename Compare, typename Allocator>
inline bool operator!=(const multiset<Key, Compare, Allocator>& lhs,
                       const multiset<Key, Compare, Allocator>& rhs)
{
    return !(lhs == rhs);
}

}  // namespace demo