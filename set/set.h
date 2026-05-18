//-----------------------------------------------------------------------------
// 版权所有 (C) 2026 demo-STL 项目
//
// 文件: set.h
// 作者: wgc
// 创建日期: 2026年5月
// 最后修改: 2026年5月
//
// 描述:
//     本文件实现了一个 STL 风格的 set 容器。
//     set 是一种关联容器，存储唯一的键，按键的顺序进行排序。
//
// 功能特性:
//     - 基于红黑树实现，保证 O(log n) 的插入、删除和查找操作
//     - 自动按键排序，支持有序遍历
//     - 键唯一，不允许重复键
//     - 提供双向迭代器支持
//     - 符合 STL 容器规范
//
// 实现说明:
//     set 使用红黑树（Red-Black Tree）数据结构实现，这是一种自平衡二叉搜索树。
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

template <typename Key, typename Compare = std::less<Key>,
          typename Allocator = std::allocator<Key>>
class set;

template <typename Key, typename Compare, typename Allocator>
bool operator==(const set<Key, Compare, Allocator>& lhs,
                const set<Key, Compare, Allocator>& rhs);

template <typename Key, typename Compare, typename Allocator>
bool operator!=(const set<Key, Compare, Allocator>& lhs,
                const set<Key, Compare, Allocator>& rhs);

template <typename Key, typename Compare, typename Allocator>
class set
{
    friend bool operator== <>(const set& lhs, const set& rhs);
    friend bool operator!= <>(const set& lhs, const set& rhs);

public:
    using key_type        = Key;
    using value_type      = Key;
    using key_compare     = Compare;
    using allocator_type  = Allocator;
    using size_type       = std::size_t;
    using difference_type = std::ptrdiff_t;
    using pointer         = value_type*;
    using const_pointer   = const value_type*;
    using reference       = value_type&;
    using const_reference = const value_type&;

public:
    struct sorted_tag
    {
    };

    struct unsorted_tag
    {
    };

private:
    enum class Color : uint8_t
    {
        RED,
        BLACK
    };

    struct Node
    {
        value_type key;
        Node*      left;
        Node*      right;
        Node*      parent;
        Color      color;

        Node()
            : left(nullptr),
              right(nullptr),
              parent(nullptr),
              color(Color::BLACK)
        {
        }

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

    class iterator
    {
        friend class set;

    public:
        using iterator_category = std::bidirectional_iterator_tag;
        using value_type        = set::value_type;
        using difference_type   = set::difference_type;
        using pointer           = set::pointer;
        using reference         = set::reference;

    public:
        iterator();
        explicit iterator(Node* node, set* container);
        iterator(const iterator& other);

        pointer operator->() const;
        reference operator*() const;

        iterator& operator++();
        iterator& operator--();
        iterator operator++(int);
        iterator operator--(int);

        bool operator==(const iterator&) const;
        bool operator!=(const iterator&) const;

    private:
        Node* node;
        set*  m_container;
    };

    class const_iterator
    {
        friend class set;

    public:
        using iterator_category = std::bidirectional_iterator_tag;
        using value_type        = set::value_type;
        using difference_type   = set::difference_type;
        using pointer           = set::const_pointer;
        using reference         = set::const_reference;

    public:
        const_iterator();
        explicit const_iterator(Node* node, const set* container);
        const_iterator(const iterator& other);
        const_iterator(const const_iterator& other);

        pointer operator->() const;
        reference operator*() const;

        const_iterator& operator++();
        const_iterator& operator--();
        const_iterator operator++(int);
        const_iterator operator--(int);

        bool operator==(const const_iterator&) const;
        bool operator!=(const const_iterator&) const;

    private:
        Node*      node;
        const set* m_container;
    };

    using reverse_iterator       = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

public:
    set();
    explicit set(const Compare& comp);
    set(std::vector<value_type>& values, sorted_tag);
    set(std::vector<value_type>& values, unsorted_tag);
    template <typename InputIt,
              std::enable_if_t<!std::is_integral<InputIt>::value, int> = 0>
    set(InputIt first, InputIt last, const Compare& comp = Compare());
    set(std::initializer_list<value_type> ilist,
        const Compare&                    comp = Compare());
    set(const set& other);
    set(set&& other) noexcept;
    ~set();

    set& operator=(const set& other);
    set& operator=(set&& other) noexcept;
    set& operator=(std::initializer_list<value_type> ilist);

    allocator_type get_allocator() const;
    key_compare key_comp() const;

    iterator begin() noexcept;
    const_iterator begin() const noexcept;
    const_iterator cbegin() const noexcept;
    iterator end() noexcept;
    const_iterator end() const noexcept;
    const_iterator cend() const noexcept;
    reverse_iterator rbegin() noexcept;
    const_reverse_iterator rbegin() const noexcept;
    const_reverse_iterator crbegin() const noexcept;
    reverse_iterator rend() noexcept;
    const_reverse_iterator rend() const noexcept;
    const_reverse_iterator crend() const noexcept;

    size_type size() const noexcept;
    bool empty() const noexcept;
    size_type max_size() const noexcept;

    void clear() noexcept;
    std::pair<iterator, bool> insert(const value_type& value);
    std::pair<iterator, bool> insert(value_type&& value);
    iterator insert(const_iterator hint, const value_type& value);
    iterator insert(const_iterator hint, value_type&& value);
    template <typename InputIt,
              std::enable_if_t<!std::is_integral<InputIt>::value, int> = 0>
    void insert(InputIt first, InputIt last);
    void insert(std::initializer_list<value_type> ilist);

    template <typename... Args>
    std::pair<iterator, bool> emplace(Args&&... args);
    template <typename... Args>
    iterator emplace_hint(const_iterator hint, Args&&... args);

    iterator erase(const_iterator pos);
    iterator erase(const_iterator first, const_iterator last);
    size_type erase(const key_type& key);

    void swap(set& other) noexcept;

    template <typename Compare2>
    void merge(set<Key, Compare2, Allocator>& other);

    iterator find(const key_type& key);
    const_iterator find(const key_type& key) const;
    size_type count(const key_type& key) const;
    iterator lower_bound(const key_type& key);
    const_iterator lower_bound(const key_type& key) const;
    iterator upper_bound(const key_type& key);
    const_iterator upper_bound(const key_type& key) const;
    std::pair<iterator, iterator> equal_range(const key_type& key);
    std::pair<const_iterator, const_iterator> equal_range(
        const key_type& key) const;

#ifndef NDEBUG
    bool validate_tree() const;
#endif

private:
    Node* build_tree(std::vector<value_type>& values, int left, int right,
                     size_type depth);
    Node* copy_node(Node* node, const set& other);
    void destroy(Node* node);
    void insert_balance(Node* node);
    void erase_balance(Node* node);
    Node* rotate_left(Node* node);
    Node* rotate_right(Node* node);
    Node* find_node(const key_type& key);
    const Node* find_node(const key_type& key) const;
    Node* min_node(Node* node);
    const Node* min_node(const Node* node) const;
    Node* max_node(Node* node);
    const Node* max_node(const Node* node) const;

#ifndef NDEBUG
    bool validate_properties(Node* node, size_type& black_height) const;
#endif

private:
    using node_alloc_type =
        typename std::allocator_traits<Allocator>::template rebind_alloc<Node>;
    using alloc_traits = std::allocator_traits<node_alloc_type>;

private:
    Node*           m_root;
    Node*           m_nil;
    size_type       m_size;
    Compare         m_comp;
    node_alloc_type m_node_alloc;
};

template <typename Key, typename Compare, typename Allocator>
set<Key, Compare, Allocator>::iterator::iterator()
    : node(nullptr), m_container(nullptr)
{
}

template <typename Key, typename Compare, typename Allocator>
set<Key, Compare, Allocator>::iterator::iterator(Node* n, set* container)
    : node(n), m_container(container)
{
}

template <typename Key, typename Compare, typename Allocator>
set<Key, Compare, Allocator>::iterator::iterator(const iterator& other)
    : node(other.node), m_container(other.m_container)
{
}

template <typename Key, typename Compare, typename Allocator>
inline typename set<Key, Compare, Allocator>::iterator::pointer
set<Key, Compare, Allocator>::iterator::operator->() const
{
    return &node->key;
}

template <typename Key, typename Compare, typename Allocator>
inline typename set<Key, Compare, Allocator>::iterator::reference
set<Key, Compare, Allocator>::iterator::operator*() const
{
    return node->key;
}

template <typename Key, typename Compare, typename Allocator>
inline typename set<Key, Compare, Allocator>::iterator&
set<Key, Compare, Allocator>::iterator::operator++()
{
    if (node->right != m_container->m_nil)
    {
        node = node->right;
        while (node->left != m_container->m_nil)
            node = node->left;
    }
    else
    {
        Node* parent = node->parent;
        while (parent != m_container->m_nil && node == parent->right)
        {
            node = parent;
            parent = parent->parent;
        }
        node = parent;
    }
    return *this;
}

template <typename Key, typename Compare, typename Allocator>
inline typename set<Key, Compare, Allocator>::iterator&
set<Key, Compare, Allocator>::iterator::operator--()
{
    if (node == m_container->m_nil)
    {
        node = m_container->max_node(m_container->m_root);
        return *this;
    }

    if (node->left != m_container->m_nil)
    {
        node = node->left;
        while (node->right != m_container->m_nil)
            node = node->right;
    }
    else
    {
        Node* parent = node->parent;
        while (parent != m_container->m_nil && node == parent->left)
        {
            node = parent;
            parent = parent->parent;
        }
        node = parent;
    }
    return *this;
}

template <typename Key, typename Compare, typename Allocator>
inline typename set<Key, Compare, Allocator>::iterator
set<Key, Compare, Allocator>::iterator::operator++(int)
{
    iterator temp(*this);
    ++(*this);
    return temp;
}

template <typename Key, typename Compare, typename Allocator>
inline typename set<Key, Compare, Allocator>::iterator
set<Key, Compare, Allocator>::iterator::operator--(int)
{
    iterator temp(*this);
    --(*this);
    return temp;
}

template <typename Key, typename Compare, typename Allocator>
inline bool set<Key, Compare, Allocator>::iterator::operator==(
    const iterator& other) const
{
    return node == other.node;
}

template <typename Key, typename Compare, typename Allocator>
inline bool set<Key, Compare, Allocator>::iterator::operator!=(
    const iterator& other) const
{
    return node != other.node;
}

template <typename Key, typename Compare, typename Allocator>
set<Key, Compare, Allocator>::const_iterator::const_iterator()
    : node(nullptr), m_container(nullptr)
{
}

template <typename Key, typename Compare, typename Allocator>
set<Key, Compare, Allocator>::const_iterator::const_iterator(
    Node* n, const set* container)
    : node(n), m_container(container)
{
}

template <typename Key, typename Compare, typename Allocator>
set<Key, Compare, Allocator>::const_iterator::const_iterator(
    const const_iterator& other)
    : node(other.node), m_container(other.m_container)
{
}

template <typename Key, typename Compare, typename Allocator>
set<Key, Compare, Allocator>::const_iterator::const_iterator(
    const iterator& other)
    : node(other.node), m_container(other.m_container)
{
}

template <typename Key, typename Compare, typename Allocator>
inline typename set<Key, Compare, Allocator>::const_iterator::pointer
set<Key, Compare, Allocator>::const_iterator::operator->() const
{
    return &node->key;
}

template <typename Key, typename Compare, typename Allocator>
inline typename set<Key, Compare, Allocator>::const_iterator::reference
set<Key, Compare, Allocator>::const_iterator::operator*() const
{
    return node->key;
}

template <typename Key, typename Compare, typename Allocator>
inline typename set<Key, Compare, Allocator>::const_iterator&
set<Key, Compare, Allocator>::const_iterator::operator++()
{
    if (node->right != m_container->m_nil)
    {
        node = node->right;
        while (node->left != m_container->m_nil)
            node = node->left;
    }
    else
    {
        Node* parent = node->parent;
        while (parent != m_container->m_nil && node == parent->right)
        {
            node = parent;
            parent = parent->parent;
        }
        node = parent;
    }
    return *this;
}

template <typename Key, typename Compare, typename Allocator>
inline typename set<Key, Compare, Allocator>::const_iterator&
set<Key, Compare, Allocator>::const_iterator::operator--()
{
    if (node == m_container->m_nil)
    {
        node = const_cast<Node*>(m_container->max_node(m_container->m_root));
        return *this;
    }

    if (node->left != m_container->m_nil)
    {
        node = node->left;
        while (node->right != m_container->m_nil)
            node = node->right;
    }
    else
    {
        Node* parent = node->parent;
        while (parent != m_container->m_nil && node == parent->left)
        {
            node = parent;
            parent = parent->parent;
        }
        node = parent;
    }
    return *this;
}

template <typename Key, typename Compare, typename Allocator>
inline typename set<Key, Compare, Allocator>::const_iterator
set<Key, Compare, Allocator>::const_iterator::operator++(int)
{
    const_iterator temp(*this);
    ++(*this);
    return temp;
}

template <typename Key, typename Compare, typename Allocator>
inline typename set<Key, Compare, Allocator>::const_iterator
set<Key, Compare, Allocator>::const_iterator::operator--(int)
{
    const_iterator temp(*this);
    --(*this);
    return temp;
}

template <typename Key, typename Compare, typename Allocator>
inline bool set<Key, Compare, Allocator>::const_iterator::operator==(
    const const_iterator& other) const
{
    return node == other.node;
}

template <typename Key, typename Compare, typename Allocator>
inline bool set<Key, Compare, Allocator>::const_iterator::operator!=(
    const const_iterator& other) const
{
    return node != other.node;
}

template <typename Key, typename Compare, typename Allocator>
set<Key, Compare, Allocator>::set()
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
set<Key, Compare, Allocator>::set(const Compare& comp)
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
set<Key, Compare, Allocator>::set(InputIt first, InputIt last,
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
set<Key, Compare, Allocator>::set(std::initializer_list<value_type> ilist,
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

template <typename Key, typename Compare, typename Allocator>
set<Key, Compare, Allocator>::set(std::vector<value_type>& values,
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
set<Key, Compare, Allocator>::set(std::vector<value_type>& values,
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
set<Key, Compare, Allocator>::set(const set& other)
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
set<Key, Compare, Allocator>::set(set&& other) noexcept
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
set<Key, Compare, Allocator>::~set()
{
    destroy(m_root);
    alloc_traits::destroy(m_node_alloc, m_nil);
    alloc_traits::deallocate(m_node_alloc, m_nil, 1);
}

template <typename Key, typename Compare, typename Allocator>
inline set<Key, Compare, Allocator>& set<Key, Compare, Allocator>::operator=(
    const set& other)
{
    if (this == &other)
        return *this;
    clear();
    insert(other.begin(), other.end());
    return *this;
}

template <typename Key, typename Compare, typename Allocator>
inline set<Key, Compare, Allocator>& set<Key, Compare, Allocator>::operator=(
    set&& other) noexcept
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
inline set<Key, Compare, Allocator>& set<Key, Compare, Allocator>::operator=(
    std::initializer_list<value_type> ilist)
{
    clear();
    insert(ilist.begin(), ilist.end());
    return *this;
}

template <typename Key, typename Compare, typename Allocator>
inline typename set<Key, Compare, Allocator>::allocator_type
set<Key, Compare, Allocator>::get_allocator() const
{
    return typename set<Key, Compare, Allocator>::allocator_type();
}

template <typename Key, typename Compare, typename Allocator>
inline typename set<Key, Compare, Allocator>::key_compare
set<Key, Compare, Allocator>::key_comp() const
{
    return m_comp;
}

template <typename Key, typename Compare, typename Allocator>
inline typename set<Key, Compare, Allocator>::iterator
set<Key, Compare, Allocator>::begin() noexcept
{
    Node* node = m_root;
    if (node != m_nil)
    {
        while (node->left != m_nil)
            node = node->left;
    }
    return iterator(node, this);
}

template <typename Key, typename Compare, typename Allocator>
inline typename set<Key, Compare, Allocator>::const_iterator
set<Key, Compare, Allocator>::begin() const noexcept
{
    Node* node = m_root;
    if (node != m_nil)
    {
        while (node->left != m_nil)
            node = node->left;
    }
    return const_iterator(node, this);
}

template <typename Key, typename Compare, typename Allocator>
inline typename set<Key, Compare, Allocator>::const_iterator
set<Key, Compare, Allocator>::cbegin() const noexcept
{
    return begin();
}

template <typename Key, typename Compare, typename Allocator>
inline typename set<Key, Compare, Allocator>::iterator
set<Key, Compare, Allocator>::end() noexcept
{
    return iterator(m_nil, this);
}

template <typename Key, typename Compare, typename Allocator>
inline typename set<Key, Compare, Allocator>::const_iterator
set<Key, Compare, Allocator>::end() const noexcept
{
    return const_iterator(m_nil, this);
}

template <typename Key, typename Compare, typename Allocator>
inline typename set<Key, Compare, Allocator>::const_iterator
set<Key, Compare, Allocator>::cend() const noexcept
{
    return const_iterator(m_nil, this);
}

template <typename Key, typename Compare, typename Allocator>
inline typename set<Key, Compare, Allocator>::reverse_iterator
set<Key, Compare, Allocator>::rbegin() noexcept
{
    return reverse_iterator(end());
}

template <typename Key, typename Compare, typename Allocator>
inline typename set<Key, Compare, Allocator>::const_reverse_iterator
set<Key, Compare, Allocator>::rbegin() const noexcept
{
    return const_reverse_iterator(cend());
}

template <typename Key, typename Compare, typename Allocator>
inline typename set<Key, Compare, Allocator>::const_reverse_iterator
set<Key, Compare, Allocator>::crbegin() const noexcept
{
    return const_reverse_iterator(cend());
}

template <typename Key, typename Compare, typename Allocator>
inline typename set<Key, Compare, Allocator>::reverse_iterator
set<Key, Compare, Allocator>::rend() noexcept
{
    return reverse_iterator(begin());
}

template <typename Key, typename Compare, typename Allocator>
inline typename set<Key, Compare, Allocator>::const_reverse_iterator
set<Key, Compare, Allocator>::rend() const noexcept
{
    return const_reverse_iterator(cbegin());
}

template <typename Key, typename Compare, typename Allocator>
inline typename set<Key, Compare, Allocator>::const_reverse_iterator
set<Key, Compare, Allocator>::crend() const noexcept
{
    return const_reverse_iterator(cbegin());
}

template <typename Key, typename Compare, typename Allocator>
inline typename set<Key, Compare, Allocator>::size_type
set<Key, Compare, Allocator>::size() const noexcept
{
    return m_size;
}

template <typename Key, typename Compare, typename Allocator>
inline bool set<Key, Compare, Allocator>::empty() const noexcept
{
    return m_root == m_nil;
}

template <typename Key, typename Compare, typename Allocator>
inline typename set<Key, Compare, Allocator>::size_type
set<Key, Compare, Allocator>::max_size() const noexcept
{
    return alloc_traits::max_size(m_node_alloc);
}

template <typename Key, typename Compare, typename Allocator>
inline void set<Key, Compare, Allocator>::clear() noexcept
{
    if (empty())
        return;

    destroy(m_root);
    m_root = m_nil;
    m_size = 0;
}

template <typename Key, typename Compare, typename Allocator>
inline std::pair<typename set<Key, Compare, Allocator>::iterator, bool>
set<Key, Compare, Allocator>::insert(const value_type& value)
{
    value_type val = value;
    return insert(std::move(val));
}

template <typename Key, typename Compare, typename Allocator>
inline std::pair<typename set<Key, Compare, Allocator>::iterator, bool>
set<Key, Compare, Allocator>::insert(value_type&& value)
{
    Node* parent = m_nil;
    Node* cur    = m_root;
    while (cur != m_nil)
    {
        parent = cur;
        if (m_comp(value, cur->key))
            cur = cur->left;
        else if (m_comp(cur->key, value))
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
    else if (m_comp(new_node->key, parent->key))
        parent->left = new_node;
    else
        parent->right = new_node;

    m_size++;
    insert_balance(new_node);

    return std::make_pair(iterator(new_node, this), true);
}

template <typename Key, typename Compare, typename Allocator>
inline typename set<Key, Compare, Allocator>::iterator
set<Key, Compare, Allocator>::insert(const_iterator    pos,
                                     const value_type& value)
{
    value_type val = value;
    return insert(pos, std::move(val));
}

template <typename Key, typename Compare, typename Allocator>
inline typename set<Key, Compare, Allocator>::iterator
set<Key, Compare, Allocator>::insert(const_iterator pos, value_type&& value)
{
    Node* parent = m_nil;
    Node* cur    = m_root;

    if (pos != cend())
    {
        cur = pos.node;
        if (m_comp(value, cur->key))
        {
            parent = cur;
            cur    = cur->left;
        }
        else if (m_comp(cur->key, value))
        {
            parent = cur;
            cur    = cur->right;
        }
        else
            return iterator(cur, this);
    }

    while (cur != m_nil)
    {
        parent = cur;
        if (m_comp(value, cur->key))
            cur = cur->left;
        else if (m_comp(cur->key, value))
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
inline void set<Key, Compare, Allocator>::insert(InputIt first, InputIt last)
{
    for (; first != last; ++first)
        insert(*first);
}

template <typename Key, typename Compare, typename Allocator>
inline void set<Key, Compare, Allocator>::insert(
    std::initializer_list<value_type> ilist)
{
    for (const value_type& val : ilist)
        insert(val);
}

template <typename Key, typename Compare, typename Allocator>
template <typename... Args>
inline std::pair<typename set<Key, Compare, Allocator>::iterator, bool>
set<Key, Compare, Allocator>::emplace(Args&&... args)
{
    return insert(value_type(std::forward<Args>(args)...));
}

template <typename Key, typename Compare, typename Allocator>
template <typename... Args>
inline typename set<Key, Compare, Allocator>::iterator
set<Key, Compare, Allocator>::emplace_hint(const_iterator hint, Args&&... args)
{
    return insert(hint, value_type(std::forward<Args>(args)...));
}

template <typename Key, typename Compare, typename Allocator>
inline typename set<Key, Compare, Allocator>::iterator
set<Key, Compare, Allocator>::erase(const_iterator pos)
{
    Node* cur = pos.node;
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
inline typename set<Key, Compare, Allocator>::iterator
set<Key, Compare, Allocator>::erase(const_iterator first, const_iterator last)
{
    iterator ret(last.node, const_cast<set*>(this));
    while (first != last)
        first = erase(first);
    return ret;
}

template <typename Key, typename Compare, typename Allocator>
inline typename set<Key, Compare, Allocator>::size_type
set<Key, Compare, Allocator>::erase(const key_type& key)
{
    Node* cur = find_node(key);
    if (cur == m_nil)
        return 0;

    erase(const_iterator(cur, this));
    return 1;
}

template <typename Key, typename Compare, typename Allocator>
inline void set<Key, Compare, Allocator>::swap(set& other) noexcept
{
    std::swap(m_root, other.m_root);
    std::swap(m_nil, other.m_nil);
    std::swap(m_size, other.m_size);
    std::swap(m_comp, other.m_comp);
    std::swap(m_node_alloc, other.m_node_alloc);
}

template <typename Key, typename Compare, typename Allocator>
template <typename Compare2>
inline void set<Key, Compare, Allocator>::merge(
    set<Key, Compare2, Allocator>& other)
{
    insert(other.begin(), other.end());
}

template <typename Key, typename Compare, typename Allocator>
inline typename set<Key, Compare, Allocator>::iterator
set<Key, Compare, Allocator>::find(const key_type& key)
{
    Node* cur = find_node(key);
    return iterator(cur, this);
}

template <typename Key, typename Compare, typename Allocator>
inline typename set<Key, Compare, Allocator>::const_iterator
set<Key, Compare, Allocator>::find(const key_type& key) const
{
    const Node* cur = find_node(key);
    return const_iterator(const_cast<Node*>(cur), this);
}

template <typename Key, typename Compare, typename Allocator>
inline typename set<Key, Compare, Allocator>::size_type
set<Key, Compare, Allocator>::count(const key_type& key) const
{
    const Node* cur = m_root;
    while (cur != m_nil)
    {
        if (m_comp(key, cur->key))
            cur = cur->left;
        else if (m_comp(cur->key, key))
            cur = cur->right;
        else
            return 1;
    }
    return 0;
}

template <typename Key, typename Compare, typename Allocator>
inline typename set<Key, Compare, Allocator>::iterator
set<Key, Compare, Allocator>::lower_bound(const key_type& key)
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
inline typename set<Key, Compare, Allocator>::const_iterator
set<Key, Compare, Allocator>::lower_bound(const key_type& key) const
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
inline typename set<Key, Compare, Allocator>::iterator
set<Key, Compare, Allocator>::upper_bound(const key_type& key)
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
inline typename set<Key, Compare, Allocator>::const_iterator
set<Key, Compare, Allocator>::upper_bound(const key_type& key) const
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
inline std::pair<typename set<Key, Compare, Allocator>::iterator,
                 typename set<Key, Compare, Allocator>::iterator>
set<Key, Compare, Allocator>::equal_range(const key_type& key)
{
    return std::make_pair(lower_bound(key), upper_bound(key));
}

template <typename Key, typename Compare, typename Allocator>
inline std::pair<typename set<Key, Compare, Allocator>::const_iterator,
                 typename set<Key, Compare, Allocator>::const_iterator>
set<Key, Compare, Allocator>::equal_range(const key_type& key) const
{
    return std::make_pair(lower_bound(key), upper_bound(key));
}

template <typename Key, typename Compare, typename Allocator>
inline typename set<Key, Compare, Allocator>::Node*
set<Key, Compare, Allocator>::build_tree(std::vector<value_type>& values,
                                         int left, int right, size_type depth)
{
    if (values.empty() || left > right)
        return m_nil;

    int             mid = left + (right - left) / 2;
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
inline typename set<Key, Compare, Allocator>::Node*
set<Key, Compare, Allocator>::copy_node(Node* cur, const set& other)
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
inline void set<Key, Compare, Allocator>::destroy(Node* node)
{
    if (node == m_nil)
        return;

    destroy(node->left);
    destroy(node->right);

    alloc_traits::destroy(m_node_alloc, node);
    alloc_traits::deallocate(m_node_alloc, node, 1);
}

template <typename Key, typename Compare, typename Allocator>
inline void set<Key, Compare, Allocator>::insert_balance(Node* node)
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

template <typename Key, typename Compare, typename Allocator>
inline void set<Key, Compare, Allocator>::erase_balance(Node* node)
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

template <typename Key, typename Compare, typename Allocator>
inline typename set<Key, Compare, Allocator>::Node*
set<Key, Compare, Allocator>::rotate_left(Node* node)
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

template <typename Key, typename Compare, typename Allocator>
inline typename set<Key, Compare, Allocator>::Node*
set<Key, Compare, Allocator>::rotate_right(Node* node)
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

template <typename Key, typename Compare, typename Allocator>
inline typename set<Key, Compare, Allocator>::Node*
set<Key, Compare, Allocator>::find_node(const key_type& key)
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
inline const typename set<Key, Compare, Allocator>::Node*
set<Key, Compare, Allocator>::find_node(const key_type& key) const
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
inline typename set<Key, Compare, Allocator>::Node*
set<Key, Compare, Allocator>::min_node(Node* node)
{
    while (node->left != m_nil)
        node = node->left;
    return node;
}

template <typename Key, typename Compare, typename Allocator>
inline const typename set<Key, Compare, Allocator>::Node*
set<Key, Compare, Allocator>::min_node(const Node* node) const
{
    while (node->left != m_nil)
        node = node->left;
    return node;
}

template <typename Key, typename Compare, typename Allocator>
inline typename set<Key, Compare, Allocator>::Node*
set<Key, Compare, Allocator>::max_node(Node* node)
{
    while (node->right != m_nil)
        node = node->right;
    return node;
}

template <typename Key, typename Compare, typename Allocator>
inline const typename set<Key, Compare, Allocator>::Node*
set<Key, Compare, Allocator>::max_node(const Node* node) const
{
    while (node->right != m_nil)
        node = node->right;
    return node;
}

#ifndef NDEBUG
template <typename Key, typename Compare, typename Allocator>
inline bool set<Key, Compare, Allocator>::validate_tree() const
{
    if (m_root == m_nil)
        return true;

    if (m_root->color == Color::RED)
        return false;

    size_type black_height = 0;
    return validate_properties(m_root, black_height);
}

template <typename Key, typename Compare, typename Allocator>
inline bool set<Key, Compare, Allocator>::validate_properties(
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

template <typename Key, typename Compare, typename Allocator>
inline bool operator==(const set<Key, Compare, Allocator>& lhs,
                       const set<Key, Compare, Allocator>& rhs)
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

template <typename Key, typename Compare, typename Allocator>
inline bool operator!=(const set<Key, Compare, Allocator>& lhs,
                       const set<Key, Compare, Allocator>& rhs)
{
    return !(lhs == rhs);
}

}  // namespace demo