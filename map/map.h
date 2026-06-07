//-----------------------------------------------------------------------------
// 版权所有 (C) 2026 demo-STL 项目
//
// 文件: map.h
// 作者: wgc
// 创建日期: 2026年5月
// 最后修改: 2026年5月
//
// 描述:
//     本文件实现了一个 STL 风格的 map 容器。
//     map 是一种关联容器，存储键值对（key-value pairs），
//     键是唯一的，且按键的顺序进行排序。
//
// 功能特性:
//     - 基于红黑树实现，保证 O(log n) 的插入、删除和查找操作
//     - 自动按键排序，支持有序遍历
//     - 键唯一，不允许重复键
//     - 提供双向迭代器支持
//     - 符合 STL 容器规范
//
// 实现说明:
//     map 使用红黑树（Red-Black Tree）数据结构实现，这是一种自平衡二叉搜索树。
//     红黑树通过维护节点颜色和旋转操作保持树的平衡，确保最坏情况下的时间复杂度。
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
#include <stdexcept>
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

/// @brief map 容器类
/// @details
/// map 是一种关联容器，存储键值对（key-value
/// pairs），键是唯一的，且按键的顺序进行排序。 map 使用红黑树实现，保证 O(log
/// n) 的插入、删除和查找操作。
/// @tparam K 键类型，必须满足可比较（Compare）要求
/// @tparam T 值类型
/// @tparam Compare 比较函数对象类型，用于键的比较，默认为 std::less<K>
/// @tparam Allocator 分配器类型，用于内存管理，默认为
/// std::allocator<std::pair<const K, T>>
template <typename K, typename T, typename Compare = std::less<K>,
          typename Allocator = std::allocator<std::pair<const K, T>>>
class map;

/// @brief 比较两个 map 是否相等
/// @tparam K 键类型
/// @tparam T 值类型
/// @tparam Compare 比较函数对象类型
/// @tparam Allocator 分配器类型
/// @param lhs 左操作数
/// @param rhs 右操作数
/// @return 如果两个 map 相等返回 true，否则返回 false
template <typename K, typename T, typename Compare, typename Allocator>
bool operator==(const map<K, T, Compare, Allocator>& lhs,
                const map<K, T, Compare, Allocator>& rhs);

/// @brief 比较两个 map 是否不相等
/// @tparam K 键类型
/// @tparam T 值类型
/// @tparam Compare 比较函数对象类型
/// @tparam Allocator 分配器类型
/// @param lhs 左操作数
/// @param rhs 右操作数
/// @return 如果两个 map 不相等返回 true，否则返回 false
template <typename K, typename T, typename Compare, typename Allocator>
bool operator!=(const map<K, T, Compare, Allocator>& lhs,
                const map<K, T, Compare, Allocator>& rhs);

template <typename K, typename T, typename Compare, typename Allocator>
class map
{
    friend bool operator== <>(const map& lhs, const map& rhs);
    friend bool operator!= <>(const map& lhs, const map& rhs);

public:
    using key_type         = K;                      ///< 键类型
    using key_compare_type = Compare;                ///< 键比较函数类型
    using mapped_type      = T;                      ///< 值类型
    using allocator_type   = Allocator;              ///< 分配器类型
    using value_type       = std::pair<const K, T>;  ///< 元素类型（键值对）
    using size_type        = std::size_t;            ///< 大小类型
    using difference_type  = std::ptrdiff_t;         ///< 差值类型
    using pointer          = value_type*;            ///< 元素指针类型
    using const_pointer    = const value_type*;      ///< 常量元素指针类型
    using reference        = value_type&;            ///< 元素引用类型
    using const_reference  = const value_type&;      ///< 常量元素引用类型

public:
    /// @brief 已排序标记
    /// @details 用于从已排序的 vector 构造 map，可优化构建效率
    struct sorted_tag
    {
    };

    /// @brief 未排序标记
    /// @details 用于从未排序的 vector 构造 map
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
    /// @details 每个节点存储键值对、左右子节点指针、父节点指针和颜色
    struct Node
    {
        value_type value;   ///< 键值对
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

        /// @brief 构造函数，从键值对创建节点
        /// @param v 键值对
        Node(value_type v)
            : value(std::move(v)),
              left(nullptr),
              right(nullptr),
              parent(nullptr),
              color(Color::BLACK)
        {
        }

        /// @brief 分段构造函数，分别构造键和值
        /// @tparam KeyArgs 键构造参数类型
        /// @tparam ValueArgs 值构造参数类型
        /// @param key_args 键构造参数元组
        /// @param value_args 值构造参数元组
        template <typename... KeyArgs, typename... ValueArgs>
        Node(std::piecewise_construct_t, std::tuple<KeyArgs...> key_args,
             std::tuple<ValueArgs...> value_args)
            : value(std::piecewise_construct, std::move(key_args),
                    std::move(value_args)),
              left(nullptr),
              right(nullptr),
              parent(nullptr),
              color(Color::BLACK)
        {
        }

        /// @brief 构造函数，从键和值创建节点
        /// @param k 键
        /// @param v 值
        Node(key_type k, mapped_type v)
            : value(std::move(k), std::move(v)),
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
    /// @details map 的迭代器，支持双向遍历（++ 和 --），按键的顺序访问元素
    class iterator
    {
        friend class map;

    public:
        using iterator_category =
            std::bidirectional_iterator_tag;      ///< 迭代器类别（双向迭代器）
        using value_type      = value_type;       ///< 元素类型
        using difference_type = difference_type;  ///< 差值类型
        using pointer         = pointer;          ///< 元素指针类型
        using reference       = reference;        ///< 元素引用类型

    public:
        /// @brief 默认构造函数，创建空迭代器
        iterator();

        /// @brief 构造函数，从节点指针和容器指针创建迭代器
        /// @param node 节点指针
        /// @param container map 容器指针
        iterator(Node* node, map* container);

        /// @brief 拷贝构造函数
        /// @param other 要拷贝的迭代器
        iterator(const iterator& other);

        /// @brief 箭头操作符，返回指向元素的指针
        /// @return 指向键值对的指针
        pointer operator->() const;

        /// @brief 解引用操作符，返回元素的引用
        /// @return 键值对的引用
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
        Node* m_node;       ///< 当前节点指针
        map*  m_container;  ///< 所属容器指针
    };

    /// @brief 常量双向迭代器类
    /// @details map 的常量迭代器，不允许修改指向的元素，支持双向遍历
    class const_iterator
    {
        friend class map;

    public:
        using iterator_category =
            std::bidirectional_iterator_tag;      ///< 迭代器类别（双向迭代器）
        using value_type      = value_type;       ///< 元素类型
        using difference_type = difference_type;  ///< 差值类型
        using pointer         = const_pointer;    ///< 常量元素指针类型
        using reference       = const_reference;  ///< 常量元素引用类型

    public:
        /// @brief 默认构造函数，创建空迭代器
        const_iterator();

        /// @brief 构造函数，从节点指针和容器指针创建迭代器
        /// @param node 节点指针
        /// @param container map 容器指针（const）
        const_iterator(Node* node, const map* container);

        /// @brief 拷贝构造函数
        /// @param other 要拷贝的常量迭代器
        const_iterator(const const_iterator& other);

        /// @brief 从非 const 迭代器构造
        /// @param other 非 const 迭代器
        const_iterator(const iterator& other);

        /// @brief 箭头操作符，返回指向元素的常量指针
        /// @return 指向键值对的常量指针
        pointer operator->() const;

        /// @brief 解引用操作符，返回元素的常量引用
        /// @return 键值对的常量引用
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
        Node*      m_node;       ///< 当前节点指针
        const map* m_container;  ///< 所属容器指针（const）
    };

    /// @brief 反向迭代器类型，使用标准库适配器
    using reverse_iterator = std::reverse_iterator<iterator>;

    /// @brief 常量反向迭代器类型，使用标准库适配器
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

public:
    // 构造函数和析构函数

    /// @brief 默认构造函数，创建空 map
    map();

    /// @brief 构造函数，使用指定的比较函数创建空 map
    /// @param comp 比较函数对象
    explicit map(const Compare& comp);

    /// @brief 范围构造函数，复制 [first, last) 范围内的元素
    /// @tparam InputIt 输入迭代器类型
    /// @param first 范围起始迭代器
    /// @param last 范围结束迭代器
    /// @param comp 比较函数对象，默认为 Compare()
    template <typename InputIt,
              std::enable_if_t<!std::is_integral<InputIt>::value, int> = 0>
    map(InputIt first, InputIt last, const Compare& comp = Compare());

    /// @brief 初始化列表构造函数
    /// @param ilist 初始化列表
    /// @param comp 比较函数对象，默认为 Compare()
    map(std::initializer_list<value_type> ilist,
        const Compare&                    comp = Compare());

    /// @brief 非标准接口：从已排序的 vector 构造 map（优化版本）
    /// @param vec 已排序的键值对 vector
    /// @param tag 已排序标记（sorted_tag）
    explicit map(const std::vector<value_type>& vec, sorted_tag);

    /// @brief 非标准接口：从未排序的 vector 构造 map
    /// @param vec 未排序的键值对 vector
    /// @param tag 未排序标记（unsorted_tag）
    explicit map(const std::vector<value_type>& vec, unsorted_tag);

    /// @brief 拷贝构造函数
    /// @param other 要拷贝的 map
    map(const map& other);

    /// @brief 移动构造函数
    /// @param other 要移动的 map
    map(map&& other) noexcept;

    /// @brief 析构函数，释放所有资源
    ~map();

    /// @brief 拷贝赋值运算符
    /// @param other 要拷贝的 map
    /// @return 当前 map 的引用
    map& operator=(const map& other);

    /// @brief 移动赋值运算符
    /// @param other 要移动的 map
    /// @return 当前 map 的引用
    map& operator=(map&& other) noexcept;

    /// @brief 初始化列表赋值运算符
    /// @param ilist 初始化列表
    /// @return 当前 map 的引用
    map& operator=(std::initializer_list<value_type> ilist);

    /// @brief 获取分配器
    /// @return 分配器对象
    allocator_type get_allocator() const;

    // 元素访问

    /// @brief 获取指定键对应的值引用（带边界检查）
    /// @param key 键
    /// @return 指定键对应的值引用
    /// @throw std::out_of_range 如果键不存在
    mapped_type& at(const key_type& key);

    /// @brief 获取指定键对应的值常量引用（带边界检查）
    /// @param key 键
    /// @return 指定键对应的值常量引用
    /// @throw std::out_of_range 如果键不存在
    const mapped_type& at(const key_type& key) const;

    /// @brief 获取或插入指定键对应的值引用（无边界检查）
    /// @param key 键（左值）
    /// @return 指定键对应的值引用，如果键不存在则插入默认构造的值
    mapped_type& operator[](const key_type& key);

    /// @brief 获取或插入指定键对应的值引用（无边界检查，移动版本）
    /// @param key 键（右值）
    /// @return 指定键对应的值引用，如果键不存在则插入默认构造的值
    mapped_type& operator[](key_type&& key);

    // 迭代器

    /// @brief 返回指向第一个元素的迭代器
    /// @return 指向最小键元素的迭代器
    iterator begin();

    /// @brief 返回指向第一个元素的常量迭代器
    /// @return 指向最小键元素的常量迭代器
    const_iterator begin() const;

    /// @brief 返回指向第一个元素的常量迭代器（const 版本）
    /// @return 指向最小键元素的常量迭代器
    const_iterator cbegin() const noexcept;

    /// @brief 返回指向末尾的迭代器
    /// @return 指向末尾的迭代器（不指向任何元素）
    iterator end();

    /// @brief 返回指向末尾的常量迭代器
    /// @return 指向末尾的常量迭代器（不指向任何元素）
    const_iterator end() const;

    /// @brief 返回指向末尾的常量迭代器（const 版本）
    /// @return 指向末尾的常量迭代器（不指向任何元素）
    const_iterator cend() const noexcept;

    /// @brief 返回指向最后一个元素的反向迭代器
    /// @return 指向最大键元素的反向迭代器
    reverse_iterator rbegin();

    /// @brief 返回指向最后一个元素的常量反向迭代器
    /// @return 指向最大键元素的常量反向迭代器
    const_reverse_iterator rbegin() const;

    /// @brief 返回指向最后一个元素的常量反向迭代器（const 版本）
    /// @return 指向最大键元素的常量反向迭代器
    const_reverse_iterator crbegin() const noexcept;

    /// @brief 返回指向第一个元素之前位置的反向迭代器
    /// @return 指向最小键元素之前位置的反向迭代器
    reverse_iterator rend();

    /// @brief 返回指向第一个元素之前位置的常量反向迭代器
    /// @return 指向最小键元素之前位置的常量反向迭代器
    const_reverse_iterator rend() const;

    /// @brief 返回指向第一个元素之前位置的常量反向迭代器（const 版本）
    /// @return 指向最小键元素之前位置的常量反向迭代器
    const_reverse_iterator crend() const noexcept;

    // 容量

    /// @brief 返回 map 中的元素数量
    /// @return 元素数量
    size_type size() const;

    /// @brief 检查 map 是否为空
    /// @return 如果为空返回 true，否则返回 false
    bool empty() const;

    /// @brief 返回 map 能容纳的最大元素数量
    /// @return 最大元素数量
    size_type max_size() const;

    // 修改器

    /// @brief 清空 map，删除所有元素
    void clear();

    /// @brief 插入元素（拷贝版本）
    /// @param value 要插入的键值对
    /// @return 包含迭代器和布尔值的 pair，迭代器指向插入的元素，
    ///         布尔值表示是否成功插入（false 表示键已存在）
    std::pair<iterator, bool> insert(const value_type& value);

    /// @brief 插入元素（移动版本）
    /// @param value 要插入的键值对（右值）
    /// @return 包含迭代器和布尔值的 pair，迭代器指向插入的元素，
    ///         布尔值表示是否成功插入（false 表示键已存在）
    std::pair<iterator, bool> insert(value_type&& value);

    /// @brief 在指定位置附近插入元素（拷贝版本）
    /// @param pos 提示位置（可能优化插入性能）
    /// @param value 要插入的键值对
    /// @return 指向插入元素的迭代器，如果键已存在则返回指向现有元素的迭代器
    iterator insert(const_iterator pos, const value_type& value);

    /// @brief 在指定位置附近插入元素（移动版本）
    /// @param pos 提示位置（可能优化插入性能）
    /// @param value 要插入的键值对（右值）
    /// @return 指向插入元素的迭代器，如果键已存在则返回指向现有元素的迭代器
    iterator insert(const_iterator pos, value_type&& value);

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

    /// @brief 插入或赋值（拷贝版本）
    /// @param key 键
    /// @param value 值
    /// @return 包含迭代器和布尔值的 pair，迭代器指向元素，
    ///         布尔值表示是否插入了新元素（false 表示已赋值）
    std::pair<iterator, bool> insert_or_assign(const key_type&    key,
                                               const mapped_type& value);

    /// @brief 插入或赋值（移动版本）
    /// @param key 键（右值）
    /// @param value 值（右值）
    /// @return 包含迭代器和布尔值的 pair，迭代器指向元素，
    ///         布尔值表示是否插入了新元素（false 表示已赋值）
    std::pair<iterator, bool> insert_or_assign(const key_type&& key,
                                               mapped_type&&    value);

    /// @brief 如果键不存在则插入并原地构造值（拷贝版本）
    /// @tparam Args 值构造参数类型
    /// @param key 键
    /// @param args 值构造参数
    /// @return 包含迭代器和布尔值的 pair，迭代器指向元素，
    ///         布尔值表示是否插入了新元素（false 表示键已存在）
    template <typename... Args>
    std::pair<iterator, bool> try_emplace(const key_type& key, Args&&... args);

    /// @brief 如果键不存在则插入并原地构造值（移动版本）
    /// @tparam Args 值构造参数类型
    /// @param key 键（右值）
    /// @param args 值构造参数
    /// @return 包含迭代器和布尔值的 pair，迭代器指向元素，
    ///         布尔值表示是否插入了新元素（false 表示键已存在）
    template <typename... Args>
    std::pair<iterator, bool> try_emplace(key_type&& key, Args&&... args);

    /// @brief 删除指定位置的元素
    /// @param pos 要删除的元素位置
    /// @return 指向被删除元素之后的迭代器，如果键不存在则返回 end()
    iterator erase(const_iterator pos);

    /// @brief 删除指定键的元素
    /// @param key 要删除的键
    /// @return 指向被删除元素之后的迭代器，如果键不存在则返回 end()
    iterator erase(const key_type& key);

    /// @brief 删除 [first, last) 范围内的元素
    /// @param first 范围起始迭代器
    /// @param last 范围结束迭代器
    /// @return 指向最后一个被删除元素之后的迭代器
    iterator erase(const_iterator first, const_iterator last);

    /// @brief 交换两个 map 的内容
    /// @param other 要交换的 map
    void swap(map& other);

    /// @brief 合并另一个 map 的元素
    /// @tparam Compare2 另一个 map 的比较函数类型
    /// @param other 要合并的 map，合并后 other 变为空
    template <typename Compare2>
    void merge(map<K, T, Compare2, Allocator>& other);

    // 查找

    /// @brief 统计指定键的元素数量（map 中键唯一，返回 0 或 1）
    /// @param key 要查找的键
    /// @return 如果键存在返回 1，否则返回 0
    size_type count(const key_type& key) const;

    /// @brief 查找指定键的元素
    /// @param key 要查找的键
    /// @return 指向找到的元素的迭代器，如果未找到则返回 end()
    iterator find(const key_type& key);

    /// @brief 查找指定键的元素（const 版本）
    /// @param key 要查找的键
    /// @return 指向找到的元素的常量迭代器，如果未找到则返回 cend()
    const_iterator find(const key_type& key) const;

    /// @brief 返回等于指定键的元素范围
    /// @param key 键
    /// @return 包含 lower_bound 和 upper_bound 的 pair（map
    /// 中范围最多包含一个元素）
    std::pair<iterator, iterator> equal_range(const key_type& key);

    /// @brief 返回等于指定键的元素范围（const 版本）
    /// @param key 键
    /// @return 包含 lower_bound 和 upper_bound 的 pair（map
    /// 中范围最多包含一个元素）
    std::pair<const_iterator, const_iterator> equal_range(
        const key_type& key) const;

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

    /// @brief 获取键比较函数对象
    /// @return 比较函数对象
    key_compare_type key_comp() const;

#ifndef NDEBUG
    /// @brief 验证红黑树的性质（调试用）
    /// @return 如果树结构有效返回 true，否则返回 false
    bool validate_tree() const;
#endif

private:
    /// @brief 从已排序的 vector 构建平衡二叉搜索树
    /// @param vec 已排序的键值对 vector
    /// @param left 左边界索引
    /// @param right 右边界索引
    /// @param depth 当前深度（用于确定节点颜色）
    /// @return 构建的子树根节点
    Node* build_tree(std::vector<value_type>& vec, int left, int right,
                     size_type depth);

    /// @brief 递归拷贝节点
    /// @param cur 当前节点
    /// @param other 源 map
    /// @return 拷贝后的节点
    Node* copy_node(Node* cur, const map& other);

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

    /// @brief 查找指定键的节点
    /// @param key 要查找的键
    /// @return 找到的节点，如果未找到返回 m_nil
    Node* find_node(const key_type& key);

    /// @brief 查找指定键的节点（const 版本）
    /// @param key 要查找的键
    /// @return 找到的节点，如果未找到返回 m_nil
    const Node* find_node(const key_type& key) const;

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
    using node_alloc_type = typename std::allocator_traits<
        allocator_type>::template rebind_alloc<Node>;

    /// @brief 节点分配器的 traits 类型
    using alloc_traits = std::allocator_traits<node_alloc_type>;

private:
    Node*           m_root;        ///< 红黑树的根节点
    Node*           m_nil;         ///< 哨兵节点（表示空）
    size_type       m_size;        ///< 元素数量
    Compare         m_comp;        ///< 键比较函数对象
    node_alloc_type m_node_alloc;  ///< 节点分配器
};

//------------------------- iterator 实现 -----------------------------
template <typename K, typename T, typename Compare, typename Allocator>
map<K, T, Compare, Allocator>::iterator::iterator()
    : m_node(nullptr), m_container(nullptr)
{
}

template <typename K, typename T, typename Compare, typename Allocator>
map<K, T, Compare, Allocator>::iterator::iterator(Node* node, map* container)
    : m_node(node), m_container(container)
{
}

template <typename K, typename T, typename Compare, typename Allocator>
map<K, T, Compare, Allocator>::iterator::iterator(const iterator& other)
    : m_node(other.m_node), m_container(other.m_container)
{
}

template <typename K, typename T, typename Compare, typename Allocator>
inline typename map<K, T, Compare, Allocator>::iterator::pointer
map<K, T, Compare, Allocator>::iterator::operator->() const
{
    return &m_node->value;
}

template <typename K, typename T, typename Compare, typename Allocator>
inline typename map<K, T, Compare, Allocator>::iterator::reference
map<K, T, Compare, Allocator>::iterator::operator*() const
{
    return m_node->value;
}

template <typename K, typename T, typename Compare, typename Allocator>
inline typename map<K, T, Compare, Allocator>::iterator&
map<K, T, Compare, Allocator>::iterator::operator++()
{
    if (m_node->right != m_container->m_nil)
    {
        m_node = m_node->right;
        while (m_node->left != m_container->m_nil)
            m_node = m_node->left;
    }
    else  // 当前子树中没有右子树，需要向上查找父节点
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

template <typename K, typename T, typename Compare, typename Allocator>
inline typename map<K, T, Compare, Allocator>::iterator&
map<K, T, Compare, Allocator>::iterator::operator--()
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
    else  // 当前子树中没有左子树，需要向上查找父节点
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

template <typename K, typename T, typename Compare, typename Allocator>
inline typename map<K, T, Compare, Allocator>::iterator
map<K, T, Compare, Allocator>::iterator::operator++(int)
{
    iterator temp(*this);
    ++(*this);
    return temp;
}

template <typename K, typename T, typename Compare, typename Allocator>
inline typename map<K, T, Compare, Allocator>::iterator
map<K, T, Compare, Allocator>::iterator::operator--(int)
{
    iterator temp(*this);
    --(*this);
    return temp;
}

template <typename K, typename T, typename Compare, typename Allocator>
inline bool map<K, T, Compare, Allocator>::iterator::operator==(
    const iterator& other) const
{
    return m_node == other.m_node;
}

template <typename K, typename T, typename Compare, typename Allocator>
inline bool map<K, T, Compare, Allocator>::iterator::operator!=(
    const iterator& other) const
{
    return m_node != other.m_node;
}

//---------------- const_iterator 实现 --------------------------------
template <typename K, typename T, typename Compare, typename Allocator>
map<K, T, Compare, Allocator>::const_iterator::const_iterator()
    : m_node(nullptr), m_container(nullptr)
{
}

template <typename K, typename T, typename Compare, typename Allocator>
map<K, T, Compare, Allocator>::const_iterator::const_iterator(
    Node* node, const map* container)
    : m_node(node), m_container(container)
{
}

template <typename K, typename T, typename Compare, typename Allocator>
map<K, T, Compare, Allocator>::const_iterator::const_iterator(
    const const_iterator& other)
    : m_node(other.m_node), m_container(other.m_container)
{
}

template <typename K, typename T, typename Compare, typename Allocator>
map<K, T, Compare, Allocator>::const_iterator::const_iterator(
    const iterator& other)
    : m_node(other.m_node), m_container(other.m_container)
{
}

template <typename K, typename T, typename Compare, typename Allocator>
inline typename map<K, T, Compare, Allocator>::const_iterator::pointer
map<K, T, Compare, Allocator>::const_iterator::operator->() const
{
    return &m_node->value;
}

template <typename K, typename T, typename Compare, typename Allocator>
inline typename map<K, T, Compare, Allocator>::const_iterator::reference
map<K, T, Compare, Allocator>::const_iterator::operator*() const
{
    return m_node->value;
}

template <typename K, typename T, typename Compare, typename Allocator>
inline typename map<K, T, Compare, Allocator>::const_iterator&
map<K, T, Compare, Allocator>::const_iterator::operator++()
{
    if (m_node->right != m_container->m_nil)
    {
        m_node = m_node->right;
        while (m_node->left != m_container->m_nil)
            m_node = m_node->left;
    }
    else  // 当前子树中没有右子树，需要向上查找父节点
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

template <typename K, typename T, typename Compare, typename Allocator>
inline typename map<K, T, Compare, Allocator>::const_iterator&
map<K, T, Compare, Allocator>::const_iterator::operator--()
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
    else  // 当前子树中没有左子树，需要向上查找父节点
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

template <typename K, typename T, typename Compare, typename Allocator>
inline typename map<K, T, Compare, Allocator>::const_iterator
map<K, T, Compare, Allocator>::const_iterator::operator++(int)
{
    const_iterator temp(*this);
    ++(*this);
    return temp;
}

template <typename K, typename T, typename Compare, typename Allocator>
inline typename map<K, T, Compare, Allocator>::const_iterator
map<K, T, Compare, Allocator>::const_iterator::operator--(int)
{
    const_iterator temp(*this);
    --(*this);
    return temp;
}

template <typename K, typename T, typename Compare, typename Allocator>
inline bool map<K, T, Compare, Allocator>::const_iterator::operator==(
    const const_iterator& other) const
{
    return m_node == other.m_node;
}

template <typename K, typename T, typename Compare, typename Allocator>
inline bool map<K, T, Compare, Allocator>::const_iterator::operator!=(
    const const_iterator& other) const
{
    return m_node != other.m_node;
}

//--------------------------------- map 实现 --------------------------------
template <typename K, typename T, typename Compare, typename Allocator>
map<K, T, Compare, Allocator>::map()
    : m_root(nullptr), m_nil(nullptr), m_size(0), m_comp(), m_node_alloc()
{
    m_nil = alloc_traits::allocate(m_node_alloc, 1);
    alloc_traits::construct(m_node_alloc, m_nil);

    m_root         = m_nil;
    m_root->parent = m_nil;
    m_root->left   = m_nil;
    m_root->right  = m_nil;
}

template <typename K, typename T, typename Compare, typename Allocator>
map<K, T, Compare, Allocator>::map(const Compare& comp)
    : m_root(nullptr), m_nil(nullptr), m_size(0), m_comp(comp), m_node_alloc()
{
    m_nil = alloc_traits::allocate(m_node_alloc, 1);
    alloc_traits::construct(m_node_alloc, m_nil);

    m_root         = m_nil;
    m_root->parent = m_nil;
    m_root->left   = m_nil;
    m_root->right  = m_nil;
}

template <typename K, typename T, typename Compare, typename Allocator>
template <typename InputIt,
          std::enable_if_t<!std::is_integral<InputIt>::value, int>>
map<K, T, Compare, Allocator>::map(InputIt first, InputIt last,
                                   const Compare& comp)
    : m_root(nullptr), m_nil(nullptr), m_size(0), m_comp(comp), m_node_alloc()
{
    m_nil = alloc_traits::allocate(m_node_alloc, 1);
    alloc_traits::construct(m_node_alloc, m_nil);

    m_root         = m_nil;
    m_root->parent = m_nil;
    m_root->left   = m_nil;
    m_root->right  = m_nil;

    for (; first != last; first++)
        insert(*first);
}

template <typename K, typename T, typename Compare, typename Allocator>
map<K, T, Compare, Allocator>::map(std::initializer_list<value_type> ilist,
                                   const Compare&                    comp)
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

template <typename K, typename T, typename Compare, typename Allocator>
map<K, T, Compare, Allocator>::map(const std::vector<value_type>& vec,
                                   unsorted_tag)
    : m_root(nullptr), m_nil(nullptr), m_size(0), m_comp(), m_node_alloc()
{
    m_nil = alloc_traits::allocate(m_node_alloc, 1);
    alloc_traits::construct(m_node_alloc, m_nil);

    m_root         = m_nil;
    m_root->parent = m_nil;
    m_root->left   = m_nil;
    m_root->right  = m_nil;

    for (const value_type& val : vec)
        insert(val);
}

template <typename K, typename T, typename Compare, typename Allocator>
map<K, T, Compare, Allocator>::map(const std::vector<value_type>& vec,
                                   sorted_tag)
    : m_root(nullptr),
      m_nil(nullptr),
      m_size(vec.size()),
      m_comp(),
      m_node_alloc()
{
    m_nil = alloc_traits::allocate(m_node_alloc, 1);
    alloc_traits::construct(m_node_alloc, m_nil);

    m_nil->parent = m_nil;
    m_nil->left   = m_nil;
    m_nil->right  = m_nil;

    std::vector<value_type> vec_copy(vec);
    m_root = build_tree(vec_copy, 0, static_cast<int>(vec_copy.size()) - 1, 0);
    m_root->parent = m_nil;

    if (vec.empty())
    {
        m_root->left  = m_nil;
        m_root->right = m_nil;
    }
}

template <typename K, typename T, typename Compare, typename Allocator>
map<K, T, Compare, Allocator>::map(const map& other)
    : m_root(nullptr),
      m_nil(nullptr),
      m_size(other.m_size),
      m_comp(other.m_comp),
      m_node_alloc(other.m_node_alloc)
{
    m_nil = alloc_traits::allocate(m_node_alloc, 1);
    alloc_traits::construct(m_node_alloc, m_nil);

    m_nil->parent = m_nil;
    m_nil->left   = m_nil;
    m_nil->right  = m_nil;

    m_root         = copy_node(other.m_root, other);
    m_root->parent = m_nil;
}

template <typename K, typename T, typename Compare, typename Allocator>
map<K, T, Compare, Allocator>::map(map&& other) noexcept
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

template <typename K, typename T, typename Compare, typename Allocator>
map<K, T, Compare, Allocator>::~map()
{
    destroy(m_root);
    alloc_traits::destroy(m_node_alloc, m_nil);
    alloc_traits::deallocate(m_node_alloc, m_nil, 1);
}

template <typename K, typename T, typename Compare, typename Allocator>
inline map<K, T, Compare, Allocator>& map<K, T, Compare, Allocator>::operator=(
    const map& other)
{
    if (this == &other)
        return *this;
    clear();
    insert(other.begin(), other.end());
    return *this;
}

template <typename K, typename T, typename Compare, typename Allocator>
inline map<K, T, Compare, Allocator>& map<K, T, Compare, Allocator>::operator=(
    map&& other) noexcept
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

template <typename K, typename T, typename Compare, typename Allocator>
inline map<K, T, Compare, Allocator>& map<K, T, Compare, Allocator>::operator=(
    std::initializer_list<value_type> ilist)
{
    clear();
    insert(ilist.begin(), ilist.end());
    return *this;
}

template <typename K, typename T, typename Compare, typename Allocator>
inline typename map<K, T, Compare, Allocator>::allocator_type
map<K, T, Compare, Allocator>::get_allocator() const
{
    return typename map<K, T, Compare, Allocator>::allocator_type();
}

template <typename K, typename T, typename Compare, typename Allocator>
inline typename map<K, T, Compare, Allocator>::mapped_type&
map<K, T, Compare, Allocator>::at(const key_type& key)
{
    iterator it = find(key);
    if (it == end())
        throw std::out_of_range("map::at: key not found");
    return it->second;
}

template <typename K, typename T, typename Compare, typename Allocator>
inline const typename map<K, T, Compare, Allocator>::mapped_type&
map<K, T, Compare, Allocator>::at(const key_type& key) const
{
    const_iterator it = find(key);
    if (it == end())
        throw std::out_of_range("map::at: key not found");
    return it->second;
}

template <typename K, typename T, typename Compare, typename Allocator>
inline typename map<K, T, Compare, Allocator>::mapped_type&
map<K, T, Compare, Allocator>::operator[](const key_type& key)
{
    iterator it = find(key);
    if (it == end())
        it = insert(value_type(key, T())).first;
    return it->second;
}

template <typename K, typename T, typename Compare, typename Allocator>
inline typename map<K, T, Compare, Allocator>::mapped_type&
map<K, T, Compare, Allocator>::operator[](key_type&& key)
{
    iterator it = find(key);
    if (it == end())
        it = insert(value_type(std::move(key), T())).first;
    return it->second;
}

template <typename K, typename T, typename Compare, typename Allocator>
inline typename map<K, T, Compare, Allocator>::iterator
map<K, T, Compare, Allocator>::begin()
{
    Node* node = m_root;
    if (node != m_nil)
    {
        while (node->left != m_nil)
            node = node->left;
    }
    return iterator(node, this);
}

template <typename K, typename T, typename Compare, typename Allocator>
inline typename map<K, T, Compare, Allocator>::const_iterator
map<K, T, Compare, Allocator>::begin() const
{
    Node* node = m_root;
    if (node != m_nil)
    {
        while (node->left != m_nil)
            node = node->left;
    }
    return const_iterator(node, this);
}

template <typename K, typename T, typename Compare, typename Allocator>
inline typename map<K, T, Compare, Allocator>::const_iterator
map<K, T, Compare, Allocator>::cbegin() const noexcept
{
    return begin();
}

template <typename K, typename T, typename Compare, typename Allocator>
inline typename map<K, T, Compare, Allocator>::iterator
map<K, T, Compare, Allocator>::end()
{
    return iterator(m_nil, this);
}

template <typename K, typename T, typename Compare, typename Allocator>
inline typename map<K, T, Compare, Allocator>::const_iterator
map<K, T, Compare, Allocator>::end() const
{
    return const_iterator(m_nil, this);
}

template <typename K, typename T, typename Compare, typename Allocator>
inline typename map<K, T, Compare, Allocator>::const_iterator
map<K, T, Compare, Allocator>::cend() const noexcept
{
    return const_iterator(m_nil, this);
}

template <typename K, typename T, typename Compare, typename Allocator>
inline typename map<K, T, Compare, Allocator>::reverse_iterator
map<K, T, Compare, Allocator>::rbegin()
{
    return reverse_iterator(end());
}

template <typename K, typename T, typename Compare, typename Allocator>
inline typename map<K, T, Compare, Allocator>::const_reverse_iterator
map<K, T, Compare, Allocator>::rbegin() const
{
    return const_reverse_iterator(cend());
}

template <typename K, typename T, typename Compare, typename Allocator>
inline typename map<K, T, Compare, Allocator>::const_reverse_iterator
map<K, T, Compare, Allocator>::crbegin() const noexcept
{
    return const_reverse_iterator(cend());
}

template <typename K, typename T, typename Compare, typename Allocator>
inline typename map<K, T, Compare, Allocator>::reverse_iterator
map<K, T, Compare, Allocator>::rend()
{
    return reverse_iterator(begin());
}

template <typename K, typename T, typename Compare, typename Allocator>
inline typename map<K, T, Compare, Allocator>::const_reverse_iterator
map<K, T, Compare, Allocator>::rend() const
{
    return const_reverse_iterator(cbegin());
}

template <typename K, typename T, typename Compare, typename Allocator>
inline typename map<K, T, Compare, Allocator>::const_reverse_iterator
map<K, T, Compare, Allocator>::crend() const noexcept
{
    return const_reverse_iterator(cbegin());
}

template <typename K, typename T, typename Compare, typename Allocator>
inline typename map<K, T, Compare, Allocator>::size_type
map<K, T, Compare, Allocator>::size() const
{
    return m_size;
}

template <typename K, typename T, typename Compare, typename Allocator>
inline bool map<K, T, Compare, Allocator>::empty() const
{
    return m_root == m_nil;
}

template <typename K, typename T, typename Compare, typename Allocator>
inline typename map<K, T, Compare, Allocator>::size_type
map<K, T, Compare, Allocator>::max_size() const
{
    return alloc_traits::max_size(m_node_alloc);
}

template <typename K, typename T, typename Compare, typename Allocator>
inline void map<K, T, Compare, Allocator>::clear()
{
    if (empty())
        return;

    destroy(m_root);
    m_root = m_nil;
    m_size = 0;
}

template <typename K, typename T, typename Compare, typename Allocator>
inline std::pair<typename map<K, T, Compare, Allocator>::iterator, bool>
map<K, T, Compare, Allocator>::insert(const value_type& value)
{
    value_type val = value;
    return insert(std::move(val));
}

template <typename K, typename T, typename Compare, typename Allocator>
inline std::pair<typename map<K, T, Compare, Allocator>::iterator, bool>
map<K, T, Compare, Allocator>::insert(value_type&& value)
{
    Node* parent = m_nil;
    Node* cur    = m_root;
    while (cur != m_nil)
    {
        parent = cur;
        if (m_comp(value.first, cur->value.first))
            cur = cur->left;
        else if (m_comp(cur->value.first, value.first))
            cur = cur->right;
        else
            return std::make_pair(iterator(cur, this), false);
    }

    Node* new_node = alloc_traits::allocate(m_node_alloc, 1);
    alloc_traits::construct(m_node_alloc, new_node, std::move(value));
    new_node->color  = Color::RED;
    new_node->left   = m_nil;
    new_node->right  = m_nil;
    new_node->parent = parent;

    if (parent == m_nil)
        m_root = new_node;
    else if (m_comp(value.first, parent->value.first))
        parent->left = new_node;
    else
        parent->right = new_node;

    m_size++;
    insert_balance(new_node);

    return std::make_pair(iterator(new_node, this), true);
}

template <typename K, typename T, typename Compare, typename Allocator>
inline typename map<K, T, Compare, Allocator>::iterator
map<K, T, Compare, Allocator>::insert(const_iterator    pos,
                                      const value_type& value)
{
    value_type val = value;
    return insert(pos, std::move(val));
}

template <typename K, typename T, typename Compare, typename Allocator>
inline typename map<K, T, Compare, Allocator>::iterator
map<K, T, Compare, Allocator>::insert(const_iterator pos, value_type&& value)
{
    // 默认从根节点开始比较
    Node* parent = m_nil;
    Node* cur    = m_root;

    if (pos != cend())
    {
        cur = pos.m_node;
        if (m_comp(value.first, cur->value.first))
        {
            // 指定的pos有效，从pos的左子节点开始比较
            parent = cur;
            cur    = cur->left;
        }
        else if (!m_comp(value.first, cur->value.first))
            return iterator(cur, this);
    }

    while (cur != m_nil)
    {
        parent = cur;
        if (m_comp(value.first, cur->value.first))
            cur = cur->left;
        else if (m_comp(cur->value.first, value.first))
            cur = cur->right;
        else
            return iterator(cur, this);
    }

    Node* new_node = alloc_traits::allocate(m_node_alloc, 1);
    alloc_traits::construct(m_node_alloc, new_node, std::move(value));
    new_node->color  = Color::RED;
    new_node->left   = m_nil;
    new_node->right  = m_nil;
    new_node->parent = parent;

    if (parent == m_nil)
        m_root = new_node;
    else if (m_comp(value.first, parent->value.first))
        parent->left = new_node;
    else
        parent->right = new_node;

    m_size++;
    insert_balance(new_node);

    return iterator(new_node, this);
}

template <typename K, typename T, typename Compare, typename Allocator>
template <typename InputIt,
          std::enable_if_t<!std::is_integral<InputIt>::value, int>>
inline void map<K, T, Compare, Allocator>::insert(InputIt first, InputIt last)
{
    for (; first != last; ++first)
        insert(*first);
}

template <typename K, typename T, typename Compare, typename Allocator>
inline void map<K, T, Compare, Allocator>::insert(
    std::initializer_list<value_type> ilist)
{
    for (const value_type& val : ilist)
        insert(val);
}

template <typename K, typename T, typename Compare, typename Allocator>
inline std::pair<typename map<K, T, Compare, Allocator>::iterator, bool>
map<K, T, Compare, Allocator>::insert_or_assign(const key_type&    key,
                                                const mapped_type& value)
{
    Node* cur = find_node(key);
    if (cur != m_nil)
    {
        cur->value.second = value;
        return std::make_pair(iterator(cur, this), false);
    }
    return insert(value_type(key, value));
}

template <typename K, typename T, typename Compare, typename Allocator>
inline std::pair<typename map<K, T, Compare, Allocator>::iterator, bool>
map<K, T, Compare, Allocator>::insert_or_assign(const key_type&& key,
                                                mapped_type&&    value)
{
    Node* cur = find_node(key);
    if (cur != m_nil)
    {
        cur->value.second = std::move(value);
        return std::make_pair(iterator(cur, this), false);
    }
    return insert(value_type(std::move(key), std::move(value)));
}

template <typename K, typename T, typename Compare, typename Allocator>
template <typename... Args>
inline std::pair<typename map<K, T, Compare, Allocator>::iterator, bool>
map<K, T, Compare, Allocator>::try_emplace(const key_type& key, Args&&... args)
{
    Node* cur = find_node(key);
    if (cur != m_nil)
    {
        return std::make_pair(iterator(cur, this), false);
    }
    return insert(value_type(key, T(std::forward<Args>(args)...)));
}

template <typename K, typename T, typename Compare, typename Allocator>
template <typename... Args>
inline std::pair<typename map<K, T, Compare, Allocator>::iterator, bool>
map<K, T, Compare, Allocator>::try_emplace(key_type&& key, Args&&... args)
{
    Node* cur = find_node(key);
    if (cur != m_nil)
    {
        return std::make_pair(iterator(cur, this), false);
    }
    return insert(value_type(std::move(key), T(std::forward<Args>(args)...)));
}

template <typename K, typename T, typename Compare, typename Allocator>
inline typename map<K, T, Compare, Allocator>::iterator
map<K, T, Compare, Allocator>::erase(const_iterator pos)
{
    Node* cur = pos.m_node;
    if (cur == m_nil)
        return iterator(m_nil, this);

    iterator ret = iterator(cur, this);
    ret++;

    Node* node_to_replace = cur;
    Node* fill_node       = m_nil;
    Color original_color  = node_to_replace->color;

    // 删除的节点最多只有一个子节点时
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
    else  // 删除的节点有两个孩子
    {
        // 找中序遍历中的后面一个节点来进行替换
        node_to_replace = cur->right;
        while (node_to_replace->left != m_nil)
        {
            node_to_replace = node_to_replace->left;
        }
        original_color = node_to_replace->color;
        fill_node      = node_to_replace->right;

        // 把 node_to_replace 分离出来，并用 fill_node 去代替它
        if (node_to_replace->parent == cur)
        {
            // fill_node->parent = node_to_replace;
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

        // 把 cur 与 node_to_replace 进行替换
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

template <typename K, typename T, typename Compare, typename Allocator>
inline typename map<K, T, Compare, Allocator>::iterator
map<K, T, Compare, Allocator>::erase(const key_type& key)
{
    Node* cur = find_node(key);
    if (cur == m_nil)
        return iterator(m_nil, this);

    iterator ret = iterator(cur, this);
    ret++;

    Node* node_to_replace = cur;
    Node* fill_node       = m_nil;
    Color original_color  = node_to_replace->color;

    // 删除的节点最多只有一个子节点时
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
    else  // 删除的节点有两个孩子
    {
        // 找中序遍历中的后面一个节点来进行替换
        node_to_replace = cur->right;
        while (node_to_replace->left != m_nil)
        {
            node_to_replace = node_to_replace->left;
        }
        original_color = node_to_replace->color;
        fill_node      = node_to_replace->right;

        // 把 node_to_replace 分离出来，并用 fill_node 去代替它
        if (node_to_replace->parent == cur)
        {
            // fill_node->parent = node_to_replace;
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

        // 把 cur 与 node_to_replace 进行替换
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

template <typename K, typename T, typename Compare, typename Allocator>
inline typename map<K, T, Compare, Allocator>::iterator
map<K, T, Compare, Allocator>::erase(const_iterator first, const_iterator last)
{
    iterator ret(last.m_node, const_cast<map*>(this));
    while (first != last)
        first = erase(first);
    return ret;
}

template <typename K, typename T, typename Compare, typename Allocator>
inline void map<K, T, Compare, Allocator>::swap(map& other)
{
    std::swap(m_root, other.m_root);
    std::swap(m_nil, other.m_nil);
    std::swap(m_size, other.m_size);
    std::swap(m_comp, other.m_comp);
    std::swap(m_node_alloc, other.m_node_alloc);
}

template <typename K, typename T, typename Compare, typename Allocator>
template <typename Compare2>
inline void map<K, T, Compare, Allocator>::merge(
    map<K, T, Compare2, Allocator>& other)
{
    insert(other.begin(), other.end());
}

template <typename K, typename T, typename Compare, typename Allocator>
inline typename map<K, T, Compare, Allocator>::size_type
map<K, T, Compare, Allocator>::count(const key_type& key) const
{
    const Node* cur = m_root;
    while (cur != m_nil)
    {
        if (m_comp(key, cur->value.first))
            cur = cur->left;
        else if (m_comp(cur->value.first, key))
            cur = cur->right;
        else
            return 1;
    }
    return 0;
}

template <typename K, typename T, typename Compare, typename Allocator>
inline typename map<K, T, Compare, Allocator>::iterator
map<K, T, Compare, Allocator>::find(const key_type& key)
{
    Node* cur = find_node(key);
    return iterator(cur, this);
}

template <typename K, typename T, typename Compare, typename Allocator>
inline typename map<K, T, Compare, Allocator>::const_iterator
map<K, T, Compare, Allocator>::find(const key_type& key) const
{
    const Node* cur = find_node(key);
    return const_iterator(const_cast<Node*>(cur), this);
}

template <typename K, typename T, typename Compare, typename Allocator>
inline typename map<K, T, Compare, Allocator>::Node*
map<K, T, Compare, Allocator>::build_tree(std::vector<value_type>& vec,
                                          int left, int right, size_type depth)
{
    if (vec.empty() || left > right)
        return m_nil;

    int               mid = left + (right - left) / 2;
    const value_type& val = vec[mid];

    Node* root = alloc_traits::allocate(m_node_alloc, 1);
    alloc_traits::construct(m_node_alloc, root, val.first, val.second);
    root->color = (depth & 1) == 0 ? Color::BLACK : Color::RED;

    root->left  = build_tree(vec, left, mid - 1, depth + 1);
    root->right = build_tree(vec, mid + 1, right, depth + 1);

    if (root->left != m_nil)
        root->left->parent = root;
    if (root->right != m_nil)
        root->right->parent = root;

    return root;
}

template <typename K, typename T, typename Compare, typename Allocator>
inline typename map<K, T, Compare, Allocator>::Node*
map<K, T, Compare, Allocator>::copy_node(Node* cur, const map& other)
{
    if (cur == other.m_nil)
        return m_nil;

    Node* root = alloc_traits::allocate(m_node_alloc, 1);
    alloc_traits::construct(m_node_alloc, root, cur->value);
    root->color = cur->color;

    root->left  = copy_node(cur->left, other);
    root->right = copy_node(cur->right, other);

    if (root->left != m_nil)
        root->left->parent = root;
    if (root->right != m_nil)
        root->right->parent = root;

    return root;
}

template <typename K, typename T, typename Compare, typename Allocator>
inline void map<K, T, Compare, Allocator>::destroy(Node* node)
{
    if (node == m_nil)
        return;

    destroy(node->left);
    destroy(node->right);

    alloc_traits::destroy(m_node_alloc, node);
    alloc_traits::deallocate(m_node_alloc, node, 1);
}

template <typename K, typename T, typename Compare, typename Allocator>
inline void map<K, T, Compare, Allocator>::insert_balance(Node* node)
{
    while (node != m_root && node->parent->color == Color::RED)
    {
        Node* parent  = node->parent;
        Node* grandpa = parent->parent;

        if (parent == grandpa->left)
        {
            Node* uncle = grandpa->right;

            if (uncle->color == Color::RED)
            {
                parent->color  = Color::BLACK;
                uncle->color   = Color::BLACK;
                grandpa->color = Color::RED;
                node           = grandpa;
            }
            else
            {
                if (node == parent->right)
                {
                    node = parent;
                    rotate_left(node);
                    parent = node->parent;
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
                node           = grandpa;
            }
            else
            {
                if (node == parent->left)
                {
                    node = parent;
                    rotate_right(node);
                    parent = node->parent;
                }
                parent->color  = Color::BLACK;
                grandpa->color = Color::RED;
                rotate_left(grandpa);
            }
        }
    }

    m_root->color = Color::BLACK;
}

template <typename K, typename T, typename Compare, typename Allocator>
inline void map<K, T, Compare, Allocator>::erase_balance(Node* node)
{
    while (node != m_root && node->color == Color::BLACK)
    {
        Node* parent  = node->parent;
        Node* sibling = m_nil;

        if (node == parent->left)
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
                node           = parent;
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
                node = m_root;
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
                node           = parent;
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
                node = m_root;
            }
        }
    }

    node->color = Color::BLACK;
}

template <typename K, typename T, typename Compare, typename Allocator>
inline typename map<K, T, Compare, Allocator>::Node*
map<K, T, Compare, Allocator>::rotate_left(Node* node)
{
    Node* rotation_center = node->right;
    Node* parent          = node->parent;

    node->right = rotation_center->left;
    if (rotation_center->left != m_nil)
        rotation_center->left->parent = node;

    rotation_center->parent = parent;
    if (parent == m_nil)
    {
        m_root = rotation_center;
    }
    else if (node == parent->left)
    {
        parent->left = rotation_center;
    }
    else
    {
        parent->right = rotation_center;
    }

    rotation_center->left = node;
    node->parent          = rotation_center;

    return rotation_center;
}

template <typename K, typename T, typename Compare, typename Allocator>
inline typename map<K, T, Compare, Allocator>::Node*
map<K, T, Compare, Allocator>::rotate_right(Node* node)
{
    Node* parent          = node->parent;
    Node* rotation_center = node->left;

    node->left = rotation_center->right;
    if (rotation_center->right != m_nil)
        rotation_center->right->parent = node;

    rotation_center->parent = parent;
    if (parent == m_nil)
    {
        m_root = rotation_center;
    }
    else if (node == parent->left)
    {
        parent->left = rotation_center;
    }
    else
    {
        parent->right = rotation_center;
    }

    rotation_center->right = node;
    node->parent           = rotation_center;

    return rotation_center;
}

template <typename K, typename T, typename Compare, typename Allocator>
inline typename map<K, T, Compare, Allocator>::Node*
map<K, T, Compare, Allocator>::find_node(const key_type& key)
{
    Node* cur = m_root;
    while (cur != m_nil)
    {
        if (m_comp(key, cur->value.first))
            cur = cur->left;
        else if (m_comp(cur->value.first, key))
            cur = cur->right;
        else
            return cur;
    }

    return m_nil;
}

template <typename K, typename T, typename Compare, typename Allocator>
inline const typename map<K, T, Compare, Allocator>::Node*
map<K, T, Compare, Allocator>::find_node(const key_type& key) const
{
    const Node* cur = m_root;
    while (cur != m_nil)
    {
        if (m_comp(key, cur->value.first))
            cur = cur->left;
        else if (m_comp(cur->value.first, key))
            cur = cur->right;
        else
            return cur;
    }

    return m_nil;
}

template <typename K, typename T, typename Compare, typename Allocator>
inline typename map<K, T, Compare, Allocator>::Node*
map<K, T, Compare, Allocator>::max_node(Node* node)
{
    while (node->right != m_nil)
        node = node->right;
    return node;
}

template <typename K, typename T, typename Compare, typename Allocator>
inline const typename map<K, T, Compare, Allocator>::Node*
map<K, T, Compare, Allocator>::max_node(const Node* node) const
{
    while (node->right != m_nil)
        node = node->right;
    return node;
}

template <typename K, typename T, typename Compare, typename Allocator>
inline std::pair<typename map<K, T, Compare, Allocator>::iterator,
                 typename map<K, T, Compare, Allocator>::iterator>
map<K, T, Compare, Allocator>::equal_range(const key_type& key)
{
    return std::make_pair(lower_bound(key), upper_bound(key));
}

template <typename K, typename T, typename Compare, typename Allocator>
inline std::pair<typename map<K, T, Compare, Allocator>::const_iterator,
                 typename map<K, T, Compare, Allocator>::const_iterator>
map<K, T, Compare, Allocator>::equal_range(const key_type& key) const
{
    return std::make_pair(lower_bound(key), upper_bound(key));
}

template <typename K, typename T, typename Compare, typename Allocator>
inline typename map<K, T, Compare, Allocator>::iterator
map<K, T, Compare, Allocator>::lower_bound(const key_type& key)
{
    Node* cur    = m_root;
    Node* result = m_nil;
    while (cur != m_nil)
    {
        // 找到第一个不小于key的节点
        if (!m_comp(cur->value.first, key))
        {
            result = cur;
            cur    = cur->left;
        }
        else
            cur = cur->right;
    }
    return iterator(result, this);
}

template <typename K, typename T, typename Compare, typename Allocator>
inline typename map<K, T, Compare, Allocator>::const_iterator
map<K, T, Compare, Allocator>::lower_bound(const key_type& key) const
{
    Node* cur    = m_root;
    Node* result = m_nil;
    while (cur != m_nil)
    {
        // 找到第一个不小于key的节点
        if (!m_comp(cur->value.first, key))
        {
            result = cur;
            cur    = cur->left;
        }
        else
            cur = cur->right;
    }
    return const_iterator(result, this);
}

template <typename K, typename T, typename Compare, typename Allocator>
inline typename map<K, T, Compare, Allocator>::iterator
map<K, T, Compare, Allocator>::upper_bound(const key_type& key)
{
    Node* cur    = m_root;
    Node* result = m_nil;
    while (cur != m_nil)
    {
        // 找到第一个大于key的节点
        if (m_comp(key, cur->value.first))
        {
            result = cur;
            cur    = cur->left;
        }
        else
            cur = cur->right;
    }
    return iterator(result, this);
}

template <typename K, typename T, typename Compare, typename Allocator>
inline typename map<K, T, Compare, Allocator>::const_iterator
map<K, T, Compare, Allocator>::upper_bound(const key_type& key) const
{
    Node* cur    = m_root;
    Node* result = m_nil;
    while (cur != m_nil)
    {
        // 找到第一个大于key的节点
        if (m_comp(key, cur->value.first))
        {
            result = cur;
            cur    = cur->left;
        }
        else
            cur = cur->right;
    }
    return const_iterator(result, this);
}

template <typename K, typename T, typename Compare, typename Allocator>
inline typename map<K, T, Compare, Allocator>::key_compare_type
map<K, T, Compare, Allocator>::key_comp() const
{
    return m_comp;
}

#ifndef NDEBUG
template <typename K, typename T, typename Compare, typename Allocator>
inline bool map<K, T, Compare, Allocator>::validate_tree() const
{
    if (m_root == m_nil)
        return true;

    if (m_root->color == Color::RED)
        return false;

    size_type black_height = 0;
    return validate_properties(m_root, black_height);
}
#endif

#ifndef NDEBUG
template <typename K, typename T, typename Compare, typename Allocator>
inline bool map<K, T, Compare, Allocator>::validate_properties(
    Node* node, size_type& black_height) const
{
    if (node == m_nil)
    {
        black_height = 1;
        return true;
    }

    if (node->color == Color::RED)
    {
        if (node->left != m_nil && node->left->color == Color::RED)
            return false;
        if (node->right != m_nil && node->right->color == Color::RED)
            return false;
    }

    size_type left_black_height  = 0;
    size_type right_black_height = 0;
    if (!validate_properties(node->left, left_black_height))
        return false;
    if (!validate_properties(node->right, right_black_height))
        return false;
    if (left_black_height != right_black_height)
        return false;

    black_height = left_black_height + (node->color == Color::BLACK ? 1 : 0);

    return true;
}
#endif

template <typename K, typename T, typename Compare, typename Allocator>
inline bool operator==(const map<K, T, Compare, Allocator>& lhs,
                       const map<K, T, Compare, Allocator>& rhs)
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

template <typename K, typename T, typename Compare, typename Allocator>
inline bool operator!=(const map<K, T, Compare, Allocator>& lhs,
                       const map<K, T, Compare, Allocator>& rhs)
{
    return !(lhs == rhs);
}

}  // namespace demo