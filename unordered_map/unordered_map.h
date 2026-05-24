//-----------------------------------------------------------------------------
// 版权所有 (C) 2026 demo-STL 项目
//
// 文件: unordered_map.h
// 作者: wgc
// 创建日期: 2026年5月
// 最后修改: 2026年5月
//
// 描述:
//     本文件实现了一个 STL 风格的 unordered_map 容器。
//     unordered_map 是一种关联容器，存储键值对（key-value pairs），
//     键是唯一的，但不保证任何顺序（通常使用哈希表实现）。
//
// 功能特性:
//     - 基于哈希表实现，平均 O(1) 的插入、删除和查找操作
//     - 键唯一，不允许重复键
//     - 不保证元素顺序，不支持有序遍历
//     - 提供前向迭代器支持
//     - 支持负载因子调节和桶重哈希
//     - 符合 STL 容器规范
//
// 实现说明:
//     unordered_map 使用带链地址法（Separate Chaining）的哈希表实现。
//     每个桶存储一个链表，用于处理哈希冲突。当负载因子超过阈值时，
//     会自动进行重哈希（rehash），扩展桶数量以保持性能。
//
//     哈希函数：将键转换为桶索引，使用 hash_type 进行哈希计算。
//     冲突解决：使用链地址法，在同桶内使用链表存储多个节点。
//     负载因子：元素数量与桶数量的比值，默认为 1.0，
//              当 load_factor() > max_load_factor() 时触发重哈希。
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

#include <cstdint>
#include <functional>
#include <memory>
#include <type_traits>

namespace demo
{

#define UNORDERED_MAP_DEFAULT_BUCKET_COUNT 16   ///< 默认桶数量
#define UNORDERED_MAP_DEFAULT_LOAD_FACTOR  1.0  ///< 默认负载因子阈值

/// @brief unordered_map 容器类
/// @details
/// unordered_map 是一种关联容器，存储键值对（key-value pairs），
/// 键是唯一的，但不保证任何顺序。 unordered_map 使用哈希表实现，
/// 保证平均 O(1) 的插入、删除和查找操作。
/// @tparam Key 键类型，必须满足可哈希（Hashable）要求
/// @tparam T 值类型
/// @tparam Hash 哈希函数对象类型，用于计算键的哈希值，默认为 std::hash<Key>
/// @tparam KeyEqual 键比较函数对象类型，用于判断键是否相等，默认为
/// std::equal_to<Key>
/// @tparam Allocator 分配器类型，用于内存管理，默认为
/// std::allocator<std::pair<const Key, T>>
template <typename Key, typename T, typename Hash = std::hash<Key>,
          typename KeyEqual  = std::equal_to<Key>,
          typename Allocator = std::allocator<std::pair<const Key, T>>>
class unordered_map;

/// @brief 比较两个 unordered_map 是否相等
/// @tparam Key 键类型
/// @tparam T 值类型
/// @tparam Hash 哈希函数对象类型
/// @tparam KeyEqual 键比较函数对象类型
/// @tparam Allocator 分配器类型
/// @param lhs 左操作数
/// @param rhs 右操作数
/// @return 如果两个 unordered_map 相等返回 true，否则返回 false
/// @details
/// 两个 unordered_map 相等当且仅当它们具有相同数量的元素，
/// 且对于每个键值对 (k, v) 在 lhs 中，都存在一个键值对 (k, v') 在 rhs 中
/// 使得 k == k' 且 v == v'。
template <typename Key, typename T, typename Hash, typename KeyEqual, typename Allocator>
bool operator==(const unordered_map<Key, T, Hash, KeyEqual, Allocator>& lhs,
                const unordered_map<Key, T, Hash, KeyEqual, Allocator>& rhs);

/// @brief 比较两个 unordered_map 是否不相等
/// @tparam Key 键类型
/// @tparam T 值类型
/// @tparam Hash 哈希函数对象类型
/// @tparam KeyEqual 键比较函数对象类型
/// @tparam Allocator 分配器类型
/// @param lhs 左操作数
/// @param rhs 右操作数
/// @return 如果两个 unordered_map 不相等返回 true，否则返回 false
template <typename Key, typename T, typename Hash, typename KeyEqual, typename Allocator>
bool operator!=(const unordered_map<Key, T, Hash, KeyEqual, Allocator>& lhs,
                const unordered_map<Key, T, Hash, KeyEqual, Allocator>& rhs);

/// @brief unordered_map 类模板
/// @details
/// 基于哈希表实现的关联容器，提供常数时间平均复杂度的查找、插入和删除操作。
template <typename Key, typename T, typename Hash, typename KeyEqual, typename Allocator>
class unordered_map
{
    friend bool operator== <>(const unordered_map& lhs, const unordered_map& rhs);
    friend bool operator!= <>(const unordered_map& lhs, const unordered_map& rhs);

public:
    using key_type        = Key;                      ///< 键类型
    using mapped_type     = T;                        ///< 值类型
    using hash_type       = Hash;                     ///< 哈希函数类型
    using key_equal       = KeyEqual;                 ///< 键比较函数类型
    using allocator_type  = Allocator;                ///< 分配器类型
    using value_type      = std::pair<const Key, T>;  ///< 元素类型（键值对）
    using size_type       = std::size_t;              ///< 大小类型
    using difference_type = std::ptrdiff_t;           ///< 差值类型
    using pointer         = T*;                       ///< 元素指针类型
    using const_pointer   = const T*;                 ///< 常量元素指针类型
    using reference       = T&;                       ///< 元素引用类型
    using const_reference = const T&;                 ///< 常量元素引用类型

public:
    /// @brief 哈希桶节点结构体
    /// @details
    /// 存储键值对的节点，通过 next 指针链接到同桶的其他节点。
    /// hash_code 缓存键的哈希值，避免重复计算。
    struct Node
    {
        value_type value;      ///< 键值对
        Node*      next;       ///< 指向同桶下一个节点的指针
        uint64_t   hash_code;  ///< 键的哈希值缓存

        /// @brief 默认构造函数
        Node() : value(), next(nullptr), hash_code(0)
        {
        }

        /// @brief 从键值对构造节点
        /// @param val 键值对
        /// @param hash 哈希值
        Node(value_type val, uint64_t hash) : value(std::move(val)), next(nullptr), hash_code(hash)
        {
        }

        /// @brief 从键值对构造节点，移动构造
        /// @param val 键值对
        /// @param hash 哈希值
        Node(value_type&& val, uint64_t hash)
            : value(std::move(val)), next(nullptr), hash_code(hash)
        {
        }
    };

public:
    class const_itrator;

    /// @brief 前向迭代器类
    /// @details
    /// unordered_map 的迭代器，支持前向遍历（++），按键的哈希值顺序访问元素。
    /// 迭代器遍历所有桶中的节点，不保证任何特定顺序。
    /// @note 迭代器在迭代过程中，如果容器被修改（插入或删除），迭代器可能失效。
    class iterator
    {
        friend class unordered_map;

    public:
        using iterator_category = std::forward_iterator_tag;  ///< 迭代器类别（前向迭代器）
        using value_type        = value_type;                 ///< 元素类型
        using pointer           = pointer;                    ///< 元素指针类型
        using reference         = reference;                  ///< 元素引用类型
        using difference_type   = difference_type;            ///< 差值类型

    public:
        /// @brief 默认构造函数，创建空迭代器
        iterator();

        /// @brief 构造函数，从节点指针和容器指针创建迭代器
        /// @param node 节点指针
        /// @param container unordered_map 容器指针
        iterator(Node* node, unordered_map* container);

        /// @brief 拷贝构造函数
        /// @param other 要拷贝的迭代器
        iterator(const iterator& other);

        /// @brief 箭头操作符，返回指向元素的指针
        /// @return 指向键值对的指针
        pointer operator->() const;

        /// @brief 解引用操作符，返回元素的引用
        /// @return 键值对的引用
        reference operator*() const;

        /// @brief 前置自增，移动到下一个元素
        /// @return 自增后的迭代器引用
        iterator& operator++();

        /// @brief 后置自增，移动到下一个元素
        /// @return 自增前的迭代器副本
        iterator operator++(int);

        /// @brief 比较两个迭代器是否相等
        /// @param other 要比较的另一个迭代器
        /// @return 如果相等返回 true，否则返回 false
        bool operator==(const iterator& other) const;

        /// @brief 比较两个迭代器是否不相等
        /// @param other 要比较的另一个迭代器
        /// @return 如果不相等返回 true，否则返回 false
        bool operator!=(const iterator& other) const;

    protected:
        Node*          m_node;       ///< 当前节点指针
        unordered_map* m_container;  ///< 所属容器指针
    };

    /// @brief 常量前向迭代器类
    /// @details
    /// unordered_map 的常量迭代器，不允许修改指向的元素，支持前向遍历。
    /// @note 迭代器在迭代过程中，如果容器被修改（插入或删除），迭代器可能失效。
    class const_iterator
    {
        friend class unordered_map;

    public:
        using iterator_category = std::forward_iterator_tag;  ///< 迭代器类别（前向迭代器）
        using value_type        = value_type;                 ///< 元素类型
        using pointer           = const_pointer;              ///< 常量元素指针类型
        using reference         = const_reference;            ///< 常量元素引用类型
        using difference_type   = difference_type;            ///< 差值类型

    public:
        /// @brief 默认构造函数，创建空迭代器
        const_iterator();

        /// @brief 构造函数，从节点指针和容器指针创建迭代器
        /// @param node 节点指针
        /// @param container unordered_map 容器指针（const）
        const_iterator(Node* node, const unordered_map* container);

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

        /// @brief 前置自增，移动到下一个元素
        /// @return 自增后的迭代器引用
        const_iterator& operator++();

        /// @brief 后置自增，移动到下一个元素
        /// @return 自增前的迭代器副本
        const_iterator operator++(int);

        /// @brief 比较两个迭代器是否相等
        /// @param other 要比较的另一个迭代器
        /// @return 如果相等返回 true，否则返回 false
        bool operator==(const const_iterator& other) const;

        /// @brief 比较两个迭代器是否不相等
        /// @param other 要比较的另一个迭代器
        /// @return 如果不相等返回 true，否则返回 false
        bool operator!=(const const_iterator& other) const;

    protected:
        Node*                m_node;       ///< 当前节点指针
        const unordered_map* m_container;  ///< 所属容器指针（const）
    };

    /// @brief 桶内前向迭代器类
    /// @details
    /// unordered_map 的桶内迭代器，用于遍历单个桶中的所有元素。
    /// 该迭代器只能在当前桶内移动，不能跨越到其他桶。
    /// 继承自 iterator，复用了大部分操作。
    /// @note 迭代器在迭代过程中，如果容器被修改（插入或删除），迭代器可能失效。
    class local_iterator : public iterator
    {
        friend class unordered_map;

    public:
        /// @brief 默认构造函数，创建空迭代器
        local_iterator();

        /// @brief 构造函数，从节点指针创建迭代器
        /// @param node 节点指针
        /// @param container unordered_map 容器指针（当前实现中未使用）
        /// @details
        /// 创建一个指向指定节点的桶内迭代器。
        /// container 参数仅为兼容性保留，桶内迭代器不跨桶移动。
        local_iterator(Node* node, unordered_map* container);

        /// @brief 拷贝构造函数
        /// @param other 要拷贝的迭代器
        local_iterator(const local_iterator& other);

        /// @brief 前置自增，移动到桶内下一个元素
        /// @return 自增后的迭代器引用
        /// @details
        /// 将迭代器移动到当前桶内的下一个节点。
        /// 如果已经到达桶末尾（链表尾部），则变为尾后迭代器。
        /// @note 该操作不会跨越到其他桶，与普通迭代器不同。
        local_iterator& operator++();

        /// @brief 后置自增，移动到桶内下一个元素
        /// @return 自增前的迭代器副本
        /// @details
        /// 将迭代器移动到当前桶内的下一个节点，返回自增前的状态。
        /// 如果已经到达桶末尾，则变为尾后迭代器。
        /// @note 该操作不会跨越到其他桶，与普通迭代器不同。
        local_iterator operator++(int);

        /// @brief 比较两个桶内迭代器是否相等
        /// @param other 要比较的另一个迭代器
        /// @return 如果相等返回 true，否则返回 false
        /// @details
        /// 两个迭代器相等当且仅当它们指向同一个节点。
        bool operator==(const local_iterator& other) const;

        /// @brief 比较两个桶内迭代器是否不相等
        /// @param other 要比较的另一个迭代器
        /// @return 如果不相等返回 true，否则返回 false
        bool operator!=(const local_iterator& other) const;
    };

    /// @brief 桶内常量前向迭代器类
    /// @details
    /// unordered_map 的桶内常量迭代器，用于遍历单个桶中的所有元素，
    /// 且不允许修改元素。该迭代器只能在当前桶内移动，不能跨越到其他桶。
    /// 继承自 const_iterator，复用了大部分操作。
    /// @note 迭代器在迭代过程中，如果容器被修改（插入或删除），迭代器可能失效。
    class const_local_iterator : public const_iterator
    {
        friend class unordered_map;

    public:
        /// @brief 默认构造函数，创建空迭代器
        const_local_iterator();

        /// @brief 构造函数，从节点指针创建迭代器
        /// @param node 节点指针
        /// @param container unordered_map 容器指针（const）
        /// @details
        /// 创建一个指向指定节点的桶内常量迭代器。
        /// container 参数仅为兼容性保留，桶内迭代器不跨桶移动。
        const_local_iterator(Node* node, const unordered_map* container);

        /// @brief 拷贝构造函数
        /// @param other 要拷贝的迭代器
        const_local_iterator(const const_local_iterator& other);

        /// @brief 从非 const 桶内迭代器构造
        /// @param other 非 const 桶内迭代器
        /// @details
        /// 从非 const 的 local_iterator 构造一个对应的常量迭代器。
        const_local_iterator(const local_iterator& other);

        /// @brief 前置自增，移动到桶内下一个元素
        /// @return 自增后的迭代器引用
        /// @details
        /// 将迭代器移动到当前桶内的下一个节点。
        /// 如果已经到达桶末尾（链表尾部），则变为尾后迭代器。
        /// @note 该操作不会跨越到其他桶，与普通迭代器不同。
        const_local_iterator& operator++();

        /// @brief 后置自增，移动到桶内下一个元素
        /// @return 自增前的迭代器副本
        /// @details
        /// 将迭代器移动到当前桶内的下一个节点，返回自增前的状态。
        /// 如果已经到达桶末尾，则变为尾后迭代器。
        /// @note 该操作不会跨越到其他桶，与普通迭代器不同。
        const_local_iterator operator++(int);

        /// @brief 比较两个桶内常量迭代器是否相等
        /// @param other 要比较的另一个迭代器
        /// @return 如果相等返回 true，否则返回 false
        /// @details
        /// 两个迭代器相等当且仅当它们指向同一个节点。
        bool operator==(const const_local_iterator& other) const;

        /// @brief 比较两个桶内常量迭代器是否不相等
        /// @param other 要比较的另一个迭代器
        /// @return 如果不相等返回 true，否则返回 false
        bool operator!=(const const_local_iterator& other) const;
    };

public:
    /// @brief 默认构造函数
    /// @details
    /// 创建一个空的 unordered_map，初始桶数量为
    /// UNORDERED_MAP_DEFAULT_BUCKET_COUNT (16)。
    unordered_map();

    /// @brief 构造函数，指定初始桶数量
    /// @param bucket_count 初始桶数量
    /// @details
    /// 创建一个空的 unordered_map，使用指定的初始桶数量。
    /// 如果 bucket_count 为 0，则使用默认桶数量。
    explicit unordered_map(size_type bucket_count = UNORDERED_MAP_DEFAULT_BUCKET_COUNT);

    /// @brief 范围构造函数
    /// @tparam InputIt 输入迭代器类型
    /// @param first 范围起始迭代器
    /// @param last 范围结束迭代器
    /// @param bucket_count 初始桶数量，默认为
    /// UNORDERED_MAP_DEFAULT_BUCKET_COUNT
    /// @details
    /// 从 [first, last) 范围内的元素创建 unordered_map。
    /// 范围中的每个元素都会被插入到容器中。
    template <typename InputIt>
    unordered_map(InputIt first, InputIt last,
                  size_type bucket_count = UNORDERED_MAP_DEFAULT_BUCKET_COUNT);

    /// @brief 初始化列表构造函数
    /// @param ilist 初始化列表
    /// @param bucket_count 初始桶数量，默认为
    /// UNORDERED_MAP_DEFAULT_BUCKET_COUNT
    /// @details 从初始化列表中的元素创建 unordered_map。
    unordered_map(std::initializer_list<value_type> ilist,
                  size_type bucket_count = UNORDERED_MAP_DEFAULT_BUCKET_COUNT);

    /// @brief 拷贝构造函数
    /// @param other 要拷贝的 unordered_map
    /// @details 创建一个 unordered_map，作为 other 的副本。
    unordered_map(const unordered_map& other);

    /// @brief 移动构造函数
    /// @param other 要移动的 unordered_map
    /// @details 创建一个 unordered_map，移走 other 的内容。
    /// 移动后 other 处于有效但未定义的状态。
    unordered_map(unordered_map&& other) noexcept;

    /// @brief 拷贝赋值运算符
    /// @param other 要拷贝的 unordered_map
    /// @return 当前 unordered_map 的引用
    unordered_map& operator=(const unordered_map& other);

    /// @brief 移动赋值运算符
    /// @param other 要移动的 unordered_map
    /// @return 当前 unordered_map 的引用
    /// @details 移动后 other 处于有效但未定义的状态。
    unordered_map& operator=(unordered_map&& other) noexcept;

    /// @brief 析构函数，释放所有资源
    ~unordered_map();

    /// @brief 获取分配器
    /// @return 分配器对象
    allocator_type get_allocator() const noexcept;

    // 迭代器

    /// @brief 返回指向第一个元素的迭代器
    /// @return 指向第一个元素的迭代器
    iterator begin() noexcept;

    /// @brief 返回指向第一个元素的常量迭代器
    /// @return 指向第一个元素的常量迭代器
    const_iterator begin() const noexcept;

    /// @brief 返回指向第一个元素的常量迭代器（const 版本）
    /// @return 指向第一个元素的常量迭代器
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

    // 容量

    /// @brief 检查 unordered_map 是否为空
    /// @return 如果为空返回 true，否则返回 false
    bool empty() const noexcept;

    /// @brief 返回 unordered_map 中的元素数量
    /// @return 元素数量
    size_type size() const noexcept;

    /// @brief 返回 unordered_map 能容纳的最大元素数量
    /// @return 最大元素数量
    size_type max_size() const noexcept;

    // 修改器

    /// @brief 清空 unordered_map，删除所有元素
    /// @details 所有节点被销毁，桶数组保留但置空。
    void clear() noexcept;

    /// @brief 插入元素（拷贝版本）
    /// @param value 要插入的键值对
    /// @return 包含迭代器和布尔值的 pair，迭代器指向插入的元素，
    ///         布尔值表示是否成功插入（false 表示键已存在）
    /// @details
    /// 如果键已存在，则不进行插入，返回指向现有元素的迭代器。
    /// 如果键不存在，则插入新元素，返回指向新插入元素的迭代器。
    std::pair<iterator, bool> insert(const value_type& value);

    /// @brief 插入元素（移动版本）
    /// @param value 要插入的键值对（右值）
    /// @return 包含迭代器和布尔值的 pair，迭代器指向插入的元素，
    ///         布尔值表示是否成功插入（false 表示键已存在）
    std::pair<iterator, bool> insert(value_type&& value);

    /// @brief 插入兼容类型的元素
    /// @tparam P 兼容类型
    /// @param value 要插入的元素
    /// @return 包含迭代器和布尔值的 pair，迭代器指向插入的元素，
    ///         布尔值表示是否成功插入
    /// @details
    /// 使用 SFINAE 启用，只有当 value_type 可以从 P 构造时才启用。
    template <typename P, std::enable_if_t<std::is_constructible_v<value_type, P>, int> = 0>
    std::pair<iterator, bool> insert(P&& value);

    /// @brief 在指定位置附近插入元素（拷贝版本）
    /// @param hint 提示位置（当前实现中 hint 仅用于兼容性，不提供优化）
    /// @param value 要插入的键值对
    /// @return 指向插入元素的迭代器，如果键已存在则返回指向现有元素的迭代器
    iterator insert(const_iterator hint, const value_type& value);

    /// @brief 在指定位置附近插入元素（移动版本）
    /// @param hint 提示位置（当前实现中 hint 仅用于兼容性，不提供优化）
    /// @param value 要插入的键值对（右值）
    /// @return 指向插入元素的迭代器，如果键已存在则返回指向现有元素的迭代器
    iterator insert(const_iterator hint, value_type&& value);

    /// @brief 在指定位置附近插入兼容类型的元素
    /// @tparam P 兼容类型
    /// @param hint 提示位置
    /// @param value 要插入的元素
    /// @return 指向插入元素的迭代器，如果键已存在则返回指向现有元素的迭代器
    template <typename P, std::enable_if_t<std::is_constructible_v<value_type, P>, int> = 0>
    iterator insert(const_iterator hint, P&& value);

    /// @brief 插入 [first, last) 范围内的元素
    /// @tparam InputIt 输入迭代器类型
    /// @param first 范围起始迭代器
    /// @param last 范围结束迭代器
    /// @details
    /// 依次插入范围内的每个元素。如果某个键已存在，则跳过该元素。
    template <typename InputIt>
    void insert(InputIt first, InputIt last);

    /// @brief 插入初始化列表中的元素
    /// @param ilist 初始化列表
    void insert(std::initializer_list<value_type> ilist);

    /// @brief 插入或赋值（拷贝版本）
    /// @tparam M 值类型
    /// @param k 键
    /// @param obj 值
    /// @return 包含迭代器和布尔值的 pair，迭代器指向元素，
    ///         布尔值表示是否插入了新元素（false 表示已赋值）
    /// @details
    /// 如果键 k 存在，则将现有值替换为 obj。
    /// 如果键 k 不存在，则插入新的键值对 (k, obj)。
    template <typename M>
    std::pair<iterator, bool> insert_or_assign(const Key& k, M&& obj);

    /// @brief 插入或赋值（移动版本）
    /// @tparam M 值类型
    /// @param k 键（右值）
    /// @param obj 值（右值）
    /// @return 包含迭代器和布尔值的 pair，迭代器指向元素，
    ///         布尔值表示是否插入了新元素（false 表示已赋值）
    template <typename M>
    std::pair<iterator, bool> insert_or_assign(Key&& k, M&& obj);

    /// @brief 在指定位置附近插入或赋值（拷贝版本）
    /// @tparam M 值类型
    /// @param hint 提示位置（不提供优化）
    /// @param k 键
    /// @param obj 值
    /// @return 指向元素的迭代器
    template <typename M>
    iterator insert_or_assign(const_iterator hint, const Key& k, M&& obj);

    /// @brief 在指定位置附近插入或赋值（移动版本）
    /// @tparam M 值类型
    /// @param hint 提示位置（不提供优化）
    /// @param k 键（右值）
    /// @param obj 值（右值）
    /// @return 指向元素的迭代器
    template <typename M>
    iterator insert_or_assign(const_iterator hint, Key&& k, M&& obj);

    /// @brief 原位构造元素
    /// @tparam Args 构造参数类型
    /// @param args 构造参数
    /// @return 包含迭代器和布尔值的 pair，迭代器指向元素，
    ///         布尔值表示是否插入了新元素
    /// @details
    /// 使用 args 构造新的键值对并插入。如果键已存在，则不进行任何操作。
    /// 元素在容器内直接构造，避免不必要的拷贝或移动。
    template <typename... Args>
    std::pair<iterator, bool> emplace(Args&&... args);

    /// @brief 原位构造元素（带提示）
    /// @tparam Args 构造参数类型
    /// @param hint 提示位置（不提供优化）
    /// @param args 构造参数
    /// @return 指向元素的迭代器
    template <typename... Args>
    iterator emplace_hint(const_iterator hint, Args&&... args);

    /// @brief 如果键不存在则原地构造值（拷贝版本）
    /// @tparam Args 值构造参数类型
    /// @param k 键
    /// @param args 值构造参数
    /// @return 包含迭代器和布尔值的 pair，迭代器指向元素，
    ///         布尔值表示是否插入了新元素（false 表示键已存在）
    /// @details
    /// 如果键 k 不存在，则使用 (k, T(args...)) 构造新元素。
    /// 如果键 k 已存在，则不进行任何操作。
    template <typename... Args>
    std::pair<iterator, bool> try_emplace(const Key& k, Args&&... args);

    /// @brief 如果键不存在则原地构造值（移动版本）
    /// @tparam Args 值构造参数类型
    /// @param k 键（右值）
    /// @param args 值构造参数
    /// @return 包含迭代器和布尔值的 pair，迭代器指向元素，
    ///         布尔值表示是否插入了新元素（false 表示键已存在）
    template <typename... Args>
    std::pair<iterator, bool> try_emplace(Key&& k, Args&&... args);

    /// @brief 如果键不存在则原地构造值（带提示，拷贝版本）
    /// @tparam Args 值构造参数类型
    /// @param hint 提示位置（不提供优化）
    /// @param k 键
    /// @param args 值构造参数
    /// @return 指向元素的迭代器
    template <typename... Args>
    iterator try_emplace(const_iterator hint, const Key& k, Args&&... args);

    /// @brief 如果键不存在则原地构造值（带提示，移动版本）
    /// @tparam Args 值构造参数类型
    /// @param hint 提示位置（不提供优化）
    /// @param k 键（右值）
    /// @param args 值构造参数
    /// @return 指向元素的迭代器
    template <typename... Args>
    iterator try_emplace(const_iterator hint, Key&& k, Args&&... args);

    /// @brief 删除指定位置的元素
    /// @param pos 要删除的元素位置
    /// @return 指向被删除元素之后的迭代器
    /// @details
    /// 从容器中删除 pos 指向的元素。被删除节点的内存会被释放。
    iterator erase(iterator pos);

    /// @brief 删除指定位置的元素（常量版本）
    /// @param pos 要删除的元素位置
    /// @return 指向被删除元素之后的迭代器
    iterator erase(const_iterator pos);

    /// @brief 删除 [first, last) 范围内的元素
    /// @param first 范围起始迭代器
    /// @param last 范围结束迭代器
    /// @return 指向最后一个被删除元素之后的迭代器
    iterator erase(const_iterator first, const_iterator last);

    /// @brief 删除指定键的元素
    /// @param key 要删除的键
    /// @return 被删除的元素数量（0 或 1）
    size_type erase(const Key& key);

    /// @brief 合并另一个 unordered_map 的元素
    /// @param other 要合并的 unordered_map
    /// @details
    /// 将 other 中的所有元素移动到当前容器。
    /// 如果某个键在两个容器中都存在，则保留当前容器的元素，跳过来自 other
    /// 的元素。 合并后 other 变为空。
    void merge(const unordered_map& other);

    /// @brief 合并异类型 unordered_map 的元素
    /// @tparam H2 另一个 map 的哈希函数类型
    /// @tparam P2 另一个 map 的键比较函数类型
    /// @param source 要合并的 unordered_map
    /// @details
    /// 将 source 中的所有元素移动到当前容器。
    /// source 的键和值类型必须与当前容器兼容。
    template <typename H2, typename P2>
    void merge(unordered_map<Key, T, H2, P2, Allocator>& source);

    /// @brief 合并异类型 unordered_map 的元素（右值版本）
    /// @tparam H2 另一个 map 的哈希函数类型
    /// @tparam P2 另一个 map 的键比较函数类型
    /// @param source 要合并的 unordered_map（右值）
    template <typename H2, typename P2>
    void merge(unordered_map<Key, T, H2, P2, Allocator>&& source);

    // 元素访问

    /// @brief 获取指定键对应的值引用（带边界检查）
    /// @param key 键
    /// @return 指定键对应的值引用
    /// @throw std::out_of_range 如果键不存在
    /// @details
    /// 如果键存在，返回对应值的引用。
    /// 如果键不存在，抛出 std::out_of_range 异常。
    T& at(const Key& key);

    /// @brief 获取指定键对应的值常量引用（带边界检查）
    /// @param key 键
    /// @return 指定键对应的值常量引用
    /// @throw std::out_of_range 如果键不存在
    const T& at(const Key& key) const;

    /// @brief 获取或插入指定键对应的值引用（无边界检查）
    /// @param key 键（左值）
    /// @return 指定键对应的值引用
    /// @details
    /// 如果键存在，返回对应值的引用。
    /// 如果键不存在，则插入默认构造的值，并返回其引用。
    /// @note 如果 T 没有默认构造函数，此操作可能导致未定义行为。
    T& operator[](const Key& key);

    /// @brief 获取或插入指定键对应的值引用（移动版本）
    /// @param key 键（右值）
    /// @return 指定键对应的值引用
    /// @details
    /// 如果键存在，返回对应值的引用。
    /// 如果键不存在，则移动 key 构造新键，并插入默认构造的值，返回其引用。
    T& operator[](Key&& key);

    // 查找

    /// @brief 统计指定键的元素数量（unordered_map 中键唯一，返回 0 或 1）
    /// @param key 要查找的键
    /// @return 如果键存在返回 1，否则返回 0
    size_type count(const Key& key) const;

    /// @brief 查找指定键的元素
    /// @param key 要查找的键
    /// @return 指向找到的元素的迭代器，如果未找到则返回 end()
    iterator find(const Key& key);

    /// @brief 查找指定键的元素（const 版本）
    /// @param key 要查找的键
    /// @return 指向找到的元素的常量迭代器，如果未找到则返回 cend()
    const_iterator find(const Key& key) const;

    /// @brief 返回等于指定键的元素范围
    /// @param key 键
    /// @return 包含 lower_bound 和 upper_bound 的 pair（unordered_map
    /// 中范围最多包含一个元素）
    std::pair<iterator, iterator> equal_range(const Key& key);

    /// @brief 返回等于指定键的元素范围（const 版本）
    /// @param key 键
    /// @return 包含 lower_bound 和 upper_bound 的 pair（unordered_map
    /// 中范围最多包含一个元素）
    std::pair<const_iterator, const_iterator> equal_range(const Key& key) const;

    // 桶接口

    /// @brief 返回指定桶的开始迭代器
    /// @param n 桶索引
    /// @return 指向桶中第一个元素的迭代器
    /// @details
    /// 返回一个 local_iterator，指向桶 n 中的第一个元素。
    /// 如果桶为空，返回的迭代器等于 end(n)。
    /// @pre n < bucket_count()
    local_iterator begin(size_type n);

    /// @brief 返回指定桶的开始迭代器（const 版本）
    /// @param n 桶索引
    /// @return 指向桶中第一个元素的常量迭代器
    /// @details
    /// 返回一个 const_local_iterator，指向桶 n 中的第一个元素。
    /// 如果桶为空，返回的迭代器等于 cend(n)。
    /// @pre n < bucket_count()
    const_local_iterator begin(size_type n) const;

    /// @brief 返回指定桶的开始迭代器（const 版本）
    /// @param n 桶索引
    /// @return 指向桶中第一个元素的常量迭代器
    /// @details
    /// 返回一个 const_local_iterator，指向桶 n 中的第一个元素。
    /// 如果桶为空，返回的迭代器等于 cend(n)。
    /// @pre n < bucket_count()
    const_local_iterator cbegin(size_type n) const;

    /// @brief 返回指定桶的结束迭代器
    /// @param n 桶索引
    /// @return 指向桶中末尾的迭代器（尾后迭代器）
    /// @details
    /// 返回一个 local_iterator，表示桶 n 的尾后位置。
    /// 该迭代器不指向任何有效元素，仅作为遍历结束标记。
    /// @pre n < bucket_count()
    local_iterator end(size_type n);

    /// @brief 返回指定桶的结束迭代器（const 版本）
    /// @param n 桶索引
    /// @return 指向桶中末尾的常量迭代器（尾后迭代器）
    /// @details
    /// 返回一个 const_local_iterator，表示桶 n 的尾后位置。
    /// 该迭代器不指向任何有效元素，仅作为遍历结束标记。
    /// @pre n < bucket_count()
    const_local_iterator end(size_type n) const;

    /// @brief 返回指定桶的结束迭代器（const 版本）
    /// @param n 桶索引
    /// @return 指向桶中末尾的常量迭代器（尾后迭代器）
    /// @details
    /// 返回一个 const_local_iterator，表示桶 n 的尾后位置。
    /// 该迭代器不指向任何有效元素，仅作为遍历结束标记。
    /// @pre n < bucket_count()
    const_local_iterator cend(size_type n) const;

    /// @brief 返回桶数量
    /// @return 当前桶数组的大小
    size_type bucket_count() const;

    /// @brief 返回最大桶数量
    /// @return 能够容纳的最大桶数量（受限于 size_type）
    size_type max_bucket_count() const;

    /// @brief 返回指定桶中的元素数量
    /// @param n 桶索引
    /// @return 桶 n 中存储的元素数量
    /// @pre n < bucket_count()
    size_type bucket_size(size_type n) const;

    /// @brief 返回指定键所在的桶索引
    /// @param k 键
    /// @return 键 k 所在的桶索引
    size_type bucket(const Key& k) const;

    // 哈希策略

    /// @brief 返回当前负载因子
    /// @return 负载因子（元素数量 / 桶数量）
    /// @details
    /// 负载因子是衡量哈希表填充程度的重要指标。
    /// 较高的负载因子意味着更低的内存使用但可能更长的查找时间。
    float load_factor() const;

    /// @brief 返回最大负载因子
    /// @return 最大负载因子
    float max_load_factor() const;

    /// @brief 设置最大负载因子
    /// @param new_max_load_factor 新的最大负载因子
    /// @return 原来的最大负载因子
    /// @details
    /// 当 load_factor() > max_load_factor() 时，会自动触发 rehash。
    /// 较大的 max_load_factor 会减少内存使用，但可能增加查找时间。
    /// 较小的 max_load_factor 会提高查找性能，但增加内存使用。
    float max_load_factor(float new_max_load_factor);

    /// @brief 重新哈希，改变桶数量
    /// @param new_bucket_count 新的桶数量
    /// @details
    /// 将桶的数量更改为不小于 new_bucket_count 且满足 n >= size() /
    /// max_load_factor() 的值 n， 然后重新将所有元素插入到新的桶数组中。
    /// 这会改变元素的分布，可能影响迭代顺序。
    void rehash(size_type new_bucket_count);

    /// @brief 预留空间
    /// @param count 要预留的元素数量
    /// @details
    /// 将桶的数量设置为能够容纳至少 count 个元素且不超过最大负载因子的数量，
    /// 并对容器进行重新哈希。
    /// 实际上调用 rehash(std::ceil(count / max_load_factor()))。
    /// @note 这是一个提示性操作，实现可以选择更优的桶数量。
    void reserve(size_type count);

    // 观察器

    /// @brief 获取哈希函数对象
    /// @return 哈希函数对象的副本
    hash_type hash_function() const;

    /// @brief 获取键比较函数对象
    /// @return 键比较函数对象的副本
    key_equal key_eq() const;

private:
    using node_allocator_type =
        typename std::allocator_traits<allocator_type>::template rebind<Node>;  ///< 节点分配器类型

    using alloc_traits = std::allocator_traits<node_allocator_type>;  ///< 分配器 traits 类型

private:
    /// @brief 计算键的桶索引
    /// @param key 键
    /// @return 桶索引
    size_type bucket_index(const Key& key) const;

    /// @brief 计算键的桶索引（使用预计算的哈希值）
    /// @param hash 哈希值
    /// @return 桶索引
    size_type bucket_index(uint64_t hash) const;

    /// @brief 查找键对应的节点
    /// @param key 键
    /// @return 找到的节点指针，如果未找到返回 nullptr
    Node* find_node(const Key& key) const;

    /// @brief 在指定桶中查找键对应的节点
    /// @param bucket_idx 桶索引
    /// @param key 键
    /// @param hash 哈希值
    /// @return 找到的节点指针，如果未找到返回 nullptr
    Node* find_node_in_bucket(size_type bucket_idx, const Key& key, uint64_t hash);

    /// @brief 检查是否需要进行重哈希
    /// @return 如果需要重哈希返回 true，否则返回 false
    bool need_rehash() const;

    /// @brief 执行重哈希
    /// @param new_bucket_count 新的桶数量
    void do_rehash(size_type new_bucket_count);

    /// @brief 销毁所有桶中的节点
    void destroy_all_nodes();

private:
    Node**              m_table;            ///< 桶数组指针
    size_type           m_mask;             ///< 桶数组掩码
    size_type           m_bucket_count;     ///< 桶数量
    size_type           m_size;             ///< 元素数量
    float               m_max_load_factor;  ///< 最大负载因子
    hash_type           m_hash_function;    ///< 哈希函数对象
    key_equal           m_key_eq;           ///< 键比较函数对象
    node_allocator_type m_node_allocator;   ///< 节点分配器
};

//------------------------iterator 实现------------------------
template <typename Key, typename T, typename Hash, typename KeyEqual, typename Allocator>
unordered_map<Key, T, Hash, KeyEqual, Allocator>::iterator::iterator()
    : m_node(nullptr), m_container(nullptr)
{
}

template <typename Key, typename T, typename Hash, typename KeyEqual, typename Allocator>
unordered_map<Key, T, Hash, KeyEqual, Allocator>::iterator::iterator(Node*          node,
                                                                     unordered_map* container)
    : m_node(node), m_container(container)
{
}

template <typename Key, typename T, typename Hash, typename KeyEqual, typename Allocator>
unordered_map<Key, T, Hash, KeyEqual, Allocator>::iterator::iterator(const iterator& other)
    : m_node(other.m_node), m_container(other.m_container)
{
}

template <typename Key, typename T, typename Hash, typename KeyEqual, typename Allocator>
inline typename unordered_map<Key, T, Hash, KeyEqual, Allocator>::iterator::pointer
unordered_map<Key, T, Hash, KeyEqual, Allocator>::iterator::operator->() const
{
    return &m_node->value;
}

template <typename Key, typename T, typename Hash, typename KeyEqual, typename Allocator>
inline typename unordered_map<Key, T, Hash, KeyEqual, Allocator>::iterator::reference
unordered_map<Key, T, Hash, KeyEqual, Allocator>::iterator::operator*() const
{
    return m_node->value;
}

template <typename Key, typename T, typename Hash, typename KeyEqual, typename Allocator>
inline typename unordered_map<Key, T, Hash, KeyEqual, Allocator>::iterator&
unordered_map<Key, T, Hash, KeyEqual, Allocator>::iterator::operator++()
{
    m_node               = m_node->next;
    size_type bucket_idx = 0;
    while (m_node == nullptr && bucket_idx < m_container->m_bucket_count)
    {
        bucket_idx++;
        m_node = m_container->m_table[bucket_idx];
    }
    return *this;
}

template <typename Key, typename T, typename Hash, typename KeyEqual, typename Allocator>
inline typename unordered_map<Key, T, Hash, KeyEqual, Allocator>::iterator
unordered_map<Key, T, Hash, KeyEqual, Allocator>::iterator::operator++(int)
{
    iterator temp(*this);
    ++(*this);
    return temp;
}

template <typename Key, typename T, typename Hash, typename KeyEqual, typename Allocator>
inline bool unordered_map<Key, T, Hash, KeyEqual, Allocator>::iterator::operator==(
    const iterator& other) const
{
    return m_node == other.m_node;
}

template <typename Key, typename T, typename Hash, typename KeyEqual, typename Allocator>
inline bool unordered_map<Key, T, Hash, KeyEqual, Allocator>::iterator::operator!=(
    const iterator& other) const
{
    return m_node != other.m_node;
}

//------------------------const_iterator 实现------------------------
template <typename Key, typename T, typename Hash, typename KeyEqual, typename Allocator>
unordered_map<Key, T, Hash, KeyEqual, Allocator>::const_iterator::const_iterator()
    : m_node(nullptr), m_container(nullptr)
{
}

template <typename Key, typename T, typename Hash, typename KeyEqual, typename Allocator>
unordered_map<Key, T, Hash, KeyEqual, Allocator>::const_iterator::const_iterator(
    Node* node, const unordered_map* container)
    : m_node(node), m_container(container)
{
}

template <typename Key, typename T, typename Hash, typename KeyEqual, typename Allocator>
unordered_map<Key, T, Hash, KeyEqual, Allocator>::const_iterator::const_iterator(
    const const_iterator& other)
    : m_node(other.m_node), m_container(other.m_container)
{
}

template <typename Key, typename T, typename Hash, typename KeyEqual, typename Allocator>
unordered_map<Key, T, Hash, KeyEqual, Allocator>::const_iterator::const_iterator(
    const iterator& other)
    : m_node(other.m_node), m_container(other.m_container)
{
}

template <typename Key, typename T, typename Hash, typename KeyEqual, typename Allocator>
inline typename unordered_map<Key, T, Hash, KeyEqual, Allocator>::const_iterator::pointer
unordered_map<Key, T, Hash, KeyEqual, Allocator>::const_iterator::operator->() const
{
    return &m_node->value;
}

template <typename Key, typename T, typename Hash, typename KeyEqual, typename Allocator>
typename unordered_map<Key, T, Hash, KeyEqual, Allocator>::const_iterator::reference
unordered_map<Key, T, Hash, KeyEqual, Allocator>::const_iterator::operator*() const
{
    return m_node->value;
}

template <typename Key, typename T, typename Hash, typename KeyEqual, typename Allocator>
inline typename unordered_map<Key, T, Hash, KeyEqual, Allocator>::const_iterator&
unordered_map<Key, T, Hash, KeyEqual, Allocator>::const_iterator::operator++()
{
    m_node               = m_node->next;
    size_type bucket_idx = 0;
    while (m_node == nullptr && bucket_idx < m_container->m_bucket_count)
    {
        bucket_idx++;
        m_node = m_container->m_table[bucket_idx];
    }
    return *this;
}

template <typename Key, typename T, typename Hash, typename KeyEqual, typename Allocator>
inline typename unordered_map<Key, T, Hash, KeyEqual, Allocator>::const_iterator
unordered_map<Key, T, Hash, KeyEqual, Allocator>::const_iterator::operator++(int)
{
    const_iterator temp(*this);
    ++(*this);
    return temp;
}

template <typename Key, typename T, typename Hash, typename KeyEqual, typename Allocator>
inline bool unordered_map<Key, T, Hash, KeyEqual, Allocator>::const_iterator::operator==(
    const const_iterator& other) const
{
    return m_node == other.m_node;
}

template <typename Key, typename T, typename Hash, typename KeyEqual, typename Allocator>
inline bool unordered_map<Key, T, Hash, KeyEqual, Allocator>::const_iterator::operator!=(
    const const_iterator& other) const
{
    return m_node != other.m_node;
}

//------------------------local_iterator 实现------------------------
template <typename Key, typename T, typename Hash, typename KeyEqual, typename Allocator>
unordered_map<Key, T, Hash, KeyEqual, Allocator>::local_iterator::local_iterator() : iterator()
{
}

template <typename Key, typename T, typename Hash, typename KeyEqual, typename Allocator>
unordered_map<Key, T, Hash, KeyEqual, Allocator>::local_iterator::local_iterator(
    Node* node, unordered_map* container)
    : iterator(node, container)
{
}

template <typename Key, typename T, typename Hash, typename KeyEqual, typename Allocator>
unordered_map<Key, T, Hash, KeyEqual, Allocator>::local_iterator::local_iterator(
    const local_iterator& other)
    : iterator(other)
{
}

template <typename Key, typename T, typename Hash, typename KeyEqual, typename Allocator>
typename unordered_map<Key, T, Hash, KeyEqual, Allocator>::local_iterator&
unordered_map<Key, T, Hash, KeyEqual, Allocator>::local_iterator::operator++()
{
    this->m_node = this->m_node->next;
    return *this;
}

template <typename Key, typename T, typename Hash, typename KeyEqual, typename Allocator>
typename unordered_map<Key, T, Hash, KeyEqual, Allocator>::local_iterator
unordered_map<Key, T, Hash, KeyEqual, Allocator>::local_iterator::operator++(int)
{
    local_iterator temp(*this);
    ++(*this);
    return temp;
}

template <typename Key, typename T, typename Hash, typename KeyEqual, typename Allocator>
bool unordered_map<Key, T, Hash, KeyEqual, Allocator>::local_iterator::operator==(
    const local_iterator& other) const
{
    return this->m_node == other.m_node;
}

template <typename Key, typename T, typename Hash, typename KeyEqual, typename Allocator>
bool unordered_map<Key, T, Hash, KeyEqual, Allocator>::local_iterator::operator!=(
    const local_iterator& other) const
{
    return this->m_node != other.m_node;
}

//------------------------const_local_iterator 实现------------------------
template <typename Key, typename T, typename Hash, typename KeyEqual, typename Allocator>
unordered_map<Key, T, Hash, KeyEqual, Allocator>::const_local_iterator::const_local_iterator()
    : const_iterator()
{
}

template <typename Key, typename T, typename Hash, typename KeyEqual, typename Allocator>
unordered_map<Key, T, Hash, KeyEqual, Allocator>::const_local_iterator::const_local_iterator(
    Node* node, const unordered_map* container)
    : const_iterator(node, container)
{
}

template <typename Key, typename T, typename Hash, typename KeyEqual, typename Allocator>
unordered_map<Key, T, Hash, KeyEqual, Allocator>::const_local_iterator::const_local_iterator(
    const const_local_iterator& other)
    : const_iterator(other)
{
}

template <typename Key, typename T, typename Hash, typename KeyEqual, typename Allocator>
unordered_map<Key, T, Hash, KeyEqual, Allocator>::const_local_iterator::const_local_iterator(
    const local_iterator& other)
    : const_iterator(other)
{
}

template <typename Key, typename T, typename Hash, typename KeyEqual, typename Allocator>
inline typename unordered_map<Key, T, Hash, KeyEqual, Allocator>::const_local_iterator&
unordered_map<Key, T, Hash, KeyEqual, Allocator>::const_local_iterator::operator++()
{
    this->m_node = this->m_node->next;
    return *this;
}

template <typename Key, typename T, typename Hash, typename KeyEqual, typename Allocator>
inline typename unordered_map<Key, T, Hash, KeyEqual, Allocator>::const_local_iterator
unordered_map<Key, T, Hash, KeyEqual, Allocator>::const_local_iterator::operator++(int)
{
    const_local_iterator temp(*this);
    ++(*this);
    return temp;
}

template <typename Key, typename T, typename Hash, typename KeyEqual, typename Allocator>
bool unordered_map<Key, T, Hash, KeyEqual, Allocator>::const_local_iterator::operator==(
    const const_local_iterator& other) const
{
    return this->m_node == other.m_node;
}

template <typename Key, typename T, typename Hash, typename KeyEqual, typename Allocator>
bool unordered_map<Key, T, Hash, KeyEqual, Allocator>::const_local_iterator::operator!=(
    const const_local_iterator& other) const
{
    return this->m_node != other.m_node;
}

// ---------------------------- unordered_map 实现 ------------------------
template <typename Key, typename T, typename Hash, typename KeyEqual, typename Allocator>
unordered_map<Key, T, Hash, KeyEqual, Allocator>::unordered_map()
    : m_table(nullptr),
      m_mask(UNORDERED_MAP_DEFAULT_BUCKET_COUNT - 1),
      m_bucket_count(UNORDERED_MAP_DEFAULT_BUCKET_COUNT),
      m_size(0),
      m_max_load_factor(UNORDERED_MAP_DEFAULT_LOAD_FACTOR),
      m_hash_function(),
      m_key_eq(),
      m_node_allocator()
{
    m_table = alloc_traits::allocate(m_node_allocator, m_bucket_count);
    for (size_type i = 0; i < m_bucket_count; ++i)
    {
        m_table[i] = nullptr;
    }
}

template <typename Key, typename T, typename Hash, typename KeyEqual, typename Allocator>
unordered_map<Key, T, Hash, KeyEqual, Allocator>::unordered_map(size_type bucket_count)
    : m_table(nullptr),
      m_mask(bucket_count - 1),
      m_bucket_count(bucket_count),
      m_size(0),
      m_max_load_factor(UNORDERED_MAP_DEFAULT_LOAD_FACTOR),
      m_hash_function(),
      m_key_eq(),
      m_node_allocator()
{
    m_table = alloc_traits::allocate(m_node_allocator, m_bucket_count);
    for (size_type i = 0; i < m_bucket_count; ++i)
    {
        m_table[i] = nullptr;
    }
}

template <typename Key, typename T, typename Hash, typename KeyEqual, typename Allocator>
template <typename InputIt>
unordered_map<Key, T, Hash, KeyEqual, Allocator>::unordered_map(InputIt first, InputIt last,
                                                                size_type bucket_count)
    : m_table(nullptr),
      m_mask(bucket_count - 1),
      m_bucket_count(bucket_count),
      m_size(0),
      m_max_load_factor(UNORDERED_MAP_DEFAULT_LOAD_FACTOR),
      m_hash_function(),
      m_key_eq(),
      m_node_allocator()
{
    m_table = alloc_traits::allocate(m_node_allocator, m_bucket_count);
    for (size_type i = 0; i < m_bucket_count; ++i)
    {
        m_table[i] = nullptr;
    }
    insert(first, last);
}

template <typename Key, typename T, typename Hash, typename KeyEqual, typename Allocator>
unordered_map<Key, T, Hash, KeyEqual, Allocator>::unordered_map(
    std::initializer_list<value_type> ilist, size_type bucket_count)
    : m_table(nullptr),
      m_mask(bucket_count - 1),
      m_bucket_count(bucket_count),
      m_size(0),
      m_max_load_factor(UNORDERED_MAP_DEFAULT_LOAD_FACTOR),
      m_hash_function(),
      m_key_eq(),
      m_node_allocator()
{
    m_table = alloc_traits::allocate(m_node_allocator, m_bucket_count);
    for (size_type i = 0; i < m_bucket_count; ++i)
    {
        m_table[i] = nullptr;
    }
    insert(ilist);
}

template <typename Key, typename T, typename Hash, typename KeyEqual, typename Allocator>
unordered_map<Key, T, Hash, KeyEqual, Allocator>::unordered_map(const unordered_map& other)
    : m_table(nullptr),
      m_mask(other.m_mask),
      m_bucket_count(other.m_bucket_count),
      m_size(other.m_size),
      m_max_load_factor(other.m_max_load_factor),
      m_hash_function(other.m_hash_function),
      m_key_eq(other.m_key_eq),
      m_node_allocator(other.m_node_allocator)
{
    m_table = alloc_traits::allocate(m_node_allocator, m_bucket_count);
    for (size_type i = 0; i < m_bucket_count; ++i)
    {
        m_table[i] = nullptr;
    }
    insert(other.begin(), other.end());
}

template <typename Key, typename T, typename Hash, typename KeyEqual, typename Allocator>
unordered_map<Key, T, Hash, KeyEqual, Allocator>::unordered_map(unordered_map&& other) noexcept
    : m_table(other.m_table),
      m_mask(other.m_mask),
      m_bucket_count(other.m_bucket_count),
      m_size(other.m_size),
      m_max_load_factor(other.m_max_load_factor),
      m_hash_function(other.m_hash_function),
      m_key_eq(other.m_key_eq),
      m_node_allocator(other.m_node_allocator)
{
    other.m_table = alloc_traits::allocate(other.m_node_allocator, other.m_bucket_count);
    for (size_type i = 0; i < other.m_bucket_count; i++)
    {
        other.m_table[i] = nullptr;
    }
    other.m_mask            = UNORDERED_MAP_DEFAULT_BUCKET_COUNT - 1;
    other.m_bucket_count    = UNORDERED_MAP_DEFAULT_BUCKET_COUNT;
    other.m_size            = 0;
    other.m_max_load_factor = UNORDERED_MAP_DEFAULT_LOAD_FACTOR;
    other.m_hash_function   = hash_type();
    other.m_key_eq          = key_equal();
    other.m_node_allocator  = allocator_type();
}

template <typename Key, typename T, typename Hash, typename KeyEqual, typename Allocator>
inline unordered_map<Key, T, Hash, KeyEqual, Allocator>&
unordered_map<Key, T, Hash, KeyEqual, Allocator>::operator=(const unordered_map& other)
{
    if (this == &other)
        return *this;

    clear();
    insert(other.begin(), other.end());
    return *this;
}

template <typename Key, typename T, typename Hash, typename KeyEqual, typename Allocator>
inline unordered_map<Key, T, Hash, KeyEqual, Allocator>&
unordered_map<Key, T, Hash, KeyEqual, Allocator>::operator=(unordered_map&& other) noexcept
{
    if (this == &other)
        return *this;

    clear();
    alloc_traits::deallocate(m_node_allocator, m_table, m_bucket_count);
    m_table           = other.m_table;
    m_mask            = other.m_mask;
    m_bucket_count    = other.m_bucket_count;
    m_size            = other.m_size;
    m_max_load_factor = other.m_max_load_factor;
    m_hash_function   = other.m_hash_function;
    m_key_eq          = other.m_key_eq;
    m_node_allocator  = other.m_node_allocator;

    other.m_table = alloc_traits::allocate(other.m_node_allocator, other.m_bucket_count);
    for (size_type i = 0; i < other.m_bucket_count; i++)
    {
        other.m_table[i] = nullptr;
    }
    other.m_mask            = UNORDERED_MAP_DEFAULT_BUCKET_COUNT - 1;
    other.m_bucket_count    = UNORDERED_MAP_DEFAULT_BUCKET_COUNT;
    other.m_size            = 0;
    other.m_max_load_factor = UNORDERED_MAP_DEFAULT_LOAD_FACTOR;
    other.m_hash_function   = hash_type();
    other.m_key_eq          = key_equal();
    other.m_node_allocator  = allocator_type();
    return *this;
}

template <typename Key, typename T, typename Hash, typename KeyEqual, typename Allocator>
unordered_map<Key, T, Hash, KeyEqual, Allocator>::~unordered_map()
{
    clear();
    alloc_traits::deallocate(m_node_allocator, m_table, m_bucket_count);
}

template <typename Key, typename T, typename Hash, typename KeyEqual, typename Allocator>
typename unordered_map<Key, T, Hash, KeyEqual, Allocator>::allocator_type
unordered_map<Key, T, Hash, KeyEqual, Allocator>::get_allocator() const noexcept
{
    return m_node_allocator;
}

template <typename Key, typename T, typename Hash, typename KeyEqual, typename Allocator>
inline typename unordered_map<Key, T, Hash, KeyEqual, Allocator>::iterator
unordered_map<Key, T, Hash, KeyEqual, Allocator>::begin() noexcept
{
    for (size_type i = 0; i < m_bucket_count; ++i)
    {
        if (m_table[i] != nullptr)
            return iterator(m_table[i], this);
    }
    return iterator(nullptr, this);
}

template <typename Key, typename T, typename Hash, typename KeyEqual, typename Allocator>
inline typename unordered_map<Key, T, Hash, KeyEqual, Allocator>::const_iterator
unordered_map<Key, T, Hash, KeyEqual, Allocator>::begin() const noexcept
{
    for (size_type i = 0; i < m_bucket_count; ++i)
    {
        if (m_table[i] != nullptr)
            return const_iterator(m_table[i], this);
    }
    return const_iterator(nullptr, this);
}

template <typename Key, typename T, typename Hash, typename KeyEqual, typename Allocator>
inline typename unordered_map<Key, T, Hash, KeyEqual, Allocator>::const_iterator
unordered_map<Key, T, Hash, KeyEqual, Allocator>::cbegin() const noexcept
{
    return begin();
}

template <typename Key, typename T, typename Hash, typename KeyEqual, typename Allocator>
typename unordered_map<Key, T, Hash, KeyEqual, Allocator>::iterator
unordered_map<Key, T, Hash, KeyEqual, Allocator>::end() noexcept
{
    return iterator(nullptr, this);
}

template <typename Key, typename T, typename Hash, typename KeyEqual, typename Allocator>
inline typename unordered_map<Key, T, Hash, KeyEqual, Allocator>::const_iterator
unordered_map<Key, T, Hash, KeyEqual, Allocator>::end() const noexcept
{
    return const_iterator(nullptr, this);
}

template <typename Key, typename T, typename Hash, typename KeyEqual, typename Allocator>
inline typename unordered_map<Key, T, Hash, KeyEqual, Allocator>::const_iterator
unordered_map<Key, T, Hash, KeyEqual, Allocator>::cend() const noexcept
{
    return end();
}

template <typename Key, typename T, typename Hash, typename KeyEqual, typename Allocator>
inline bool unordered_map<Key, T, Hash, KeyEqual, Allocator>::empty() const noexcept
{
    return m_size == 0;
}

template <typename Key, typename T, typename Hash, typename KeyEqual, typename Allocator>
inline typename unordered_map<Key, T, Hash, KeyEqual, Allocator>::size_type
unordered_map<Key, T, Hash, KeyEqual, Allocator>::size() const noexcept
{
    return m_size;
}

template <typename Key, typename T, typename Hash, typename KeyEqual, typename Allocator>
inline typename unordered_map<Key, T, Hash, KeyEqual, Allocator>::size_type
unordered_map<Key, T, Hash, KeyEqual, Allocator>::max_size() const noexcept
{
    return alloc_traits::max_size(m_node_allocator);
}

template <typename Key, typename T, typename Hash, typename KeyEqual, typename Allocator>
inline void unordered_map<Key, T, Hash, KeyEqual, Allocator>::clear() noexcept
{
    for (size_type i = 0; i < m_bucket_count; i++)
    {
        Node* node = m_table[i];
        while (node != nullptr)
        {
            Node* next = node->next;
            alloc_traits::destroy(m_node_allocator, node);
            alloc_traits::deallocate(m_node_allocator, node, 1);
            node = next;
        }
    }
    m_size = 0;
}

template <typename Key, typename T, typename Hash, typename KeyEqual, typename Allocator>
inline std::pair<typename unordered_map<Key, T, Hash, KeyEqual, Allocator>::iterator, bool>
unordered_map<Key, T, Hash, KeyEqual, Allocator>::insert(const value_type& value)
{
    value_type val = value;
    return insert(std::move(val));
}

template <typename Key, typename T, typename Hash, typename KeyEqual, typename Allocator>
inline std::pair<typename unordered_map<Key, T, Hash, KeyEqual, Allocator>::iterator, bool>
unordered_map<Key, T, Hash, KeyEqual, Allocator>::insert(value_type&& value)
{
    iterator it = find(value.first);
    if (it != end())
        return std::make_pair(it, false);

    uint64_t hcode    = m_hash_function(value.first);
    Node*    new_node = alloc_traits::allocate(m_node_allocator, 1);
    alloc_traits::construct(m_node_allocator, new_node, std::move(value), hcode);
    size_type index = bucket_index(hcode);
    new_node->next  = m_table[index];
    m_table[index]  = new_node;
    m_size++;
    return std::make_pair(iterator(new_node, this), true);
}

template <typename Key, typename T, typename Hash, typename KeyEqual, typename Allocator>
template <
    typename P,
    std::enable_if_t<std::is_constructible_v<
                         typename unordered_map<Key, T, Hash, KeyEqual, Allocator>::value_type, P>,
                     int>>
inline std::pair<typename unordered_map<Key, T, Hash, KeyEqual, Allocator>::iterator, bool>
unordered_map<Key, T, Hash, KeyEqual, Allocator>::insert(P&& value)
{
    return insert(std::forward<P>(value));
}

template <typename Key, typename T, typename Hash, typename KeyEqual, typename Allocator>
typename unordered_map<Key, T, Hash, KeyEqual, Allocator>::iterator
unordered_map<Key, T, Hash, KeyEqual, Allocator>::insert(const_iterator    hint,
                                                         const value_type& value)
{
    (void)hint;
    return insert(value);
}

template <typename Key, typename T, typename Hash, typename KeyEqual, typename Allocator>
typename unordered_map<Key, T, Hash, KeyEqual, Allocator>::iterator
unordered_map<Key, T, Hash, KeyEqual, Allocator>::insert(const_iterator hint, value_type&& value)
{
    (void)hint;
    return insert(std::move(value));
}

template <typename Key, typename T, typename Hash, typename KeyEqual, typename Allocator>
template <
    typename P,
    std::enable_if_t<std::is_constructible_v<
                         typename unordered_map<Key, T, Hash, KeyEqual, Allocator>::value_type, P>,
                     int>>
typename unordered_map<Key, T, Hash, KeyEqual, Allocator>::iterator
unordered_map<Key, T, Hash, KeyEqual, Allocator>::insert(const_iterator hint, P&& value)
{
    (void)hint;
    return insert(std::forward<P>(value));
}

template <typename Key, typename T, typename Hash, typename KeyEqual, typename Allocator>
template <typename InputIt>
void unordered_map<Key, T, Hash, KeyEqual, Allocator>::insert(InputIt first, InputIt last)
{
    for (InputIt it = first; last != it; it++)
        insert(*it);
}

template <typename Key, typename T, typename Hash, typename KeyEqual, typename Allocator>
void unordered_map<Key, T, Hash, KeyEqual, Allocator>::insert(
    std::initializer_list<value_type> ilist)
{
    for (auto& value : ilist)
        insert(value);
}

template <typename Key, typename T, typename Hash, typename KeyEqual, typename Allocator>
template <typename M>
inline std::pair<typename unordered_map<Key, T, Hash, KeyEqual, Allocator>::iterator, bool>
unordered_map<Key, T, Hash, KeyEqual, Allocator>::insert_or_assign(const Key& k, M&& obj)
{
    const Key key = k;
    return insert_or_assign(std::move(key), std::forward<M>(obj));
}

template <typename Key, typename T, typename Hash, typename KeyEqual, typename Allocator>
template <typename M>
inline std::pair<typename unordered_map<Key, T, Hash, KeyEqual, Allocator>::iterator, bool>
unordered_map<Key, T, Hash, KeyEqual, Allocator>::insert_or_assign(Key&& k, M&& obj)
{
    iterator it = find(k);
    if (it != end())
    {
        it->second = std::forward<M>(obj);
        return std::make_pair(it, false);
    }
    return insert(value_type(std::forward<Key>(k), std::forward<M>(obj)));
}

template <typename Key, typename T, typename Hash, typename KeyEqual, typename Allocator>
template <typename M>
typename unordered_map<Key, T, Hash, KeyEqual, Allocator>::iterator
unordered_map<Key, T, Hash, KeyEqual, Allocator>::insert_or_assign(const_iterator hint,
                                                                   const Key& k, M&& obj)
{
    (void)hint;
    return insert_or_assign(k, std::forward<M>(obj));
}

template <typename Key, typename T, typename Hash, typename KeyEqual, typename Allocator>
template <typename M>
typename unordered_map<Key, T, Hash, KeyEqual, Allocator>::iterator
unordered_map<Key, T, Hash, KeyEqual, Allocator>::insert_or_assign(const_iterator hint, Key&& k,
                                                                   M&& obj)
{
    (void)hint;
    return insert_or_assign(std::forward<Key>(k), std::forward<M>(obj));
}

template <typename Key, typename T, typename Hash, typename KeyEqual, typename Allocator>
template <typename... Args>
std::pair<typename unordered_map<Key, T, Hash, KeyEqual, Allocator>::iterator, bool>
unordered_map<Key, T, Hash, KeyEqual, Allocator>::emplace(Args&&... args)
{
    return insert(value_type(std::forward<Args>(args)...));
}

template <typename Key, typename T, typename Hash, typename KeyEqual, typename Allocator>
template <typename... Args>
typename unordered_map<Key, T, Hash, KeyEqual, Allocator>::iterator
unordered_map<Key, T, Hash, KeyEqual, Allocator>::emplace_hint(const_iterator hint, Args&&... args)
{
    return insert(hint, value_type(std::forward<Args>(args)...));
}

template <typename Key, typename T, typename Hash, typename KeyEqual, typename Allocator>
template <typename... Args>
std::pair<typename unordered_map<Key, T, Hash, KeyEqual, Allocator>::iterator, bool>
unordered_map<Key, T, Hash, KeyEqual, Allocator>::try_emplace(const Key& k, Args&&... args)
{
    return insert_or_assign(k, std::forward<Args>(args)...);
}

template <typename Key, typename T, typename Hash, typename KeyEqual, typename Allocator>
template <typename... Args>
std::pair<typename unordered_map<Key, T, Hash, KeyEqual, Allocator>::iterator, bool>
unordered_map<Key, T, Hash, KeyEqual, Allocator>::try_emplace(Key&& k, Args&&... args)
{
    return insert_or_assign(std::forward<Key>(k), std::forward<Args>(args)...);
}

template <typename Key, typename T, typename Hash, typename KeyEqual, typename Allocator>
template <typename... Args>
typename unordered_map<Key, T, Hash, KeyEqual, Allocator>::iterator
unordered_map<Key, T, Hash, KeyEqual, Allocator>::try_emplace(const_iterator hint, const Key& k,
                                                              Args&&... args)
{
    return insert_or_assign(hint, k, std::forward<Args>(args)...);
}

template <typename Key, typename T, typename Hash, typename KeyEqual, typename Allocator>
template <typename... Args>
typename unordered_map<Key, T, Hash, KeyEqual, Allocator>::iterator
unordered_map<Key, T, Hash, KeyEqual, Allocator>::try_emplace(const_iterator hint, Key&& k,
                                                              Args&&... args)
{
    return insert_or_assign(hint, std::move(k), std::forward<Args>(args)...);
}

}  // namespace demo