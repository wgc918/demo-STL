#pragma once

#include <pthread.h>

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
template <typename K, typename T, typename Compare = std::less<K>,
          typename Allocator = std::allocator<std::pair<const K, T>>>
class map;

template <typename K, typename T, typename Compare, typename Allocator>
bool operator==(const map<K, T, Compare, Allocator>& lhs,
                const map<K, T, Compare, Allocator>& rhs);
template <typename K, typename T, typename Compare, typename Allocator>
bool operator!=(const map<K, T, Compare, Allocator>& lhs,
                const map<K, T, Compare, Allocator>& rhs);

template <typename K, typename T, typename Compare, typename Allocator>
class map
{
    friend bool operator== <>(const map& lhs, const map& rhs);
    friend bool operator!= <>(const map& lhs, const map& rhs);

public:
    using key_type         = K;
    using key_compare_type = Compare;
    using mapped_type      = T;
    using allocator_type   = Allocator;
    using value_type       = std::pair<const K, T>;
    using size_type        = std::size_t;
    using difference_type  = std::ptrdiff_t;
    using pointer          = value_type*;
    using const_pointer    = const value_type*;
    using reference        = value_type&;
    using const_reference  = const value_type&;

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
        value_type value;
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

        Node(value_type v)
            : value(std::move(v)),
              left(nullptr),
              right(nullptr),
              parent(nullptr),
              color(Color::BLACK)
        {
        }

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

    class iterator
    {
        friend class map;

    public:
        using iterator_category = std::bidirectional_iterator_tag;
        using value_type        = value_type;
        using difference_type   = difference_type;
        using pointer           = pointer;
        using reference         = reference;

    public:
        iterator();
        iterator(Node* node, map* container);
        iterator(const iterator& other);

        pointer operator->() const;
        reference operator*() const;

        iterator& operator++();
        iterator& operator--();
        iterator operator++(int);
        iterator operator--(int);

        bool operator==(const iterator& other) const;
        bool operator!=(const iterator& other) const;

    private:
        Node* m_node;
        map*  m_container;
    };

    class const_iterator
    {
        friend class map;

    public:
        using iterator_category = std::bidirectional_iterator_tag;
        using value_type        = value_type;
        using difference_type   = difference_type;
        using pointer           = const_pointer;
        using reference         = const_reference;

    public:
        const_iterator();
        const_iterator(Node* node, map* container);
        const_iterator(const const_iterator& other);
        const_iterator(const iterator& other);

        pointer operator->() const;
        reference operator*() const;

        const_iterator& operator++();
        const_iterator& operator--();
        const_iterator operator++(int);
        const_iterator operator--(int);

        bool operator==(const const_iterator& other) const;
        bool operator!=(const const_iterator& other) const;

    private:
        Node* m_node;
        map*  m_container;
    };

    using reverse_iterator       = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

public:
    map();
    explicit map(const Compare& comp);
    template <typename InputIt>
    map(InputIt first, InputIt last, const Compare& comp = Compare());
    map(std::initializer_list<value_type> ilist,
        const Compare&                    comp = Compare());
    // 非标准接口
    explicit map(const std::vector<value_type>& vec, sorted_tag);
    explicit map(const std::vector<value_type>& vec, unsorted_tag);
    map(const map& other);
    map(map&& other) noexcept;
    ~map();

    map& operator=(const map& other);
    map& operator=(map&& other) noexcept;
    map& operator=(std::initializer_list<value_type> ilist);

    allocator_type get_allocator() const;

    mapped_type& at(const key_type& key);
    const mapped_type& at(const key_type& key) const;
    mapped_type& operator[](const key_type& key);
    mapped_type& operator[](key_type&& key);

    iterator begin();
    const_iterator begin() const;
    const_iterator cbegin() const noexcept;
    iterator end();
    const_iterator end() const;
    const_iterator cend() const noexcept;
    reverse_iterator rbegin();
    reverse_iterator rbegin() const;
    const_reverse_iterator crbegin() const noexcept;
    reverse_iterator rend();
    reverse_iterator rend() const;
    const_reverse_iterator crend() const noexcept;

    size_type size() const;
    bool empty() const;
    size_type max_size() const;

    void clear();
    std::pair<iterator, bool> insert(const value_type& value);
    std::pair<iterator, bool> insert(value_type&& value);
    iterator insert(const_iterator pos, const value_type& value);
    iterator insert(const_iterator pos, value_type&& value);
    template <typename InputIt>
    void insert(InputIt first, InputIt last);
    template <typename InputIt>
    void insert(std::initializer_list<value_type> ilist);
    std::pair<iterator, bool> insert_or_assign(const key_type&    key,
                                               const mapped_type& value);
    std::pair<iterator, bool> insert_or_assign(const key_type&& key,
                                               mapped_type&&    value);
    template <typename... Args>
    std::pair<iterator, bool> try_emplace(const key_type& key, Args&&... args);
    template <typename... Args>
    std::pair<iterator, bool> try_emplace(key_type&& key, Args&&... args);

    iterator erase(const_iterator pos);
    iterator erase(const key_type& key);
    iterator erase(const_iterator first, const_iterator last);

    void swap(map& other);
    template <typename Compare2>
    void merge(map<K, T, Compare2, Allocator>& other);

    size_type count(const key_type& key) const;
    template <
        typename Key,
        std::enable_if_t<std::is_void<typename Compare::is_transparent>::value,
                         int> = 0>
    size_type count(const Key& key) const;
    iterator find(const key_type& key);
    const_iterator find(const key_type& key) const;
    template <
        typename Key,
        std::enable_if_t<std::is_void<typename Compare::is_transparent>::value,
                         int> = 0>
    iterator find(const Key& key);
    template <
        typename Key,
        std::enable_if_t<std::is_void<typename Compare::is_transparent>::value,
                         int> = 0>
    const_iterator find(const Key& key) const;
    std::pair<iterator, iterator> equal_range(const key_type& key);
    std::pair<const_iterator, const_iterator> equal_range(
        const key_type& key) const;
    template <typename Key>
    std::pair<iterator, iterator> equal_range(const Key& key);
    template <typename Key>
    std::pair<const_iterator, const_iterator> equal_range(const Key& key) const;
    iterator lower_bound(const key_type& key);
    const_iterator lower_bound(const key_type& key) const;
    template <typename Key>
    iterator lower_bound(const Key& key);
    template <typename Key>
    const_iterator lower_bound(const Key& key) const;
    template <typename Key>
    iterator upper_bound(const Key& key);
    template <typename Key>
    const_iterator upper_bound(const Key& key) const;
    iterator upper_bound(const key_type& key);
    const_iterator upper_bound(const key_type& key) const;

    key_compare_type key_comp() const;

#ifndef NDEBUG
    bool validate_tree() const;
#endif

private:
    Node* build_tree(std::vector<value_type>& vec, size_type left,
                     size_type right, size_type depth);
    Node* copy_node(Node* cur, const map& other);
    void destroy(Node* node);
    void insert_balance(Node* node);
    void erase_balance(Node* node);
    Node* find_node(const key_type& key);
#ifndef NDEBUG
    bool validate_properties(Node* node, size_type& black_height) const;
#endif

private:
    using node_alloc_type = typename std::allocator_traits<
        allocator_type>::template rebind_alloc<Node>;
    using alloc_traits = std::allocator_traits<node_alloc_type>;

private:
    Node*           m_root;
    Node*           m_nil;
    size_type       m_size;
    Compare         m_comp;
    node_alloc_type m_node_alloc;
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
map<K, T, Compare, Allocator>::const_iterator::const_iterator(Node* node,
                                                              map*  container)
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
}

template <typename K, typename T, typename Compare, typename Allocator>
map<K, T, Compare, Allocator>::map(const Compare& comp)
    : m_root(nullptr), m_nil(nullptr), m_size(0), m_comp(comp), m_node_alloc()
{
    m_nil = alloc_traits::allocate(m_node_alloc, 1);
    alloc_traits::construct(m_node_alloc, m_nil);

    m_root         = m_nil;
    m_root->parent = m_nil;
}

template <typename K, typename T, typename Compare, typename Allocator>
template <typename InputIt>
map<K, T, Compare, Allocator>::map(InputIt first, InputIt last,
                                   const Compare& comp)
    : m_root(nullptr), m_nil(nullptr), m_size(0), m_comp(comp), m_node_alloc()
{
    m_nil = alloc_traits::allocate(m_node_alloc, 1);
    alloc_traits::construct(m_node_alloc, m_nil);

    m_root         = m_nil;
    m_root->parent = m_nil;

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

    for (const value_type& val : vec)
        insert(val);
}

template <typename K, typename T, typename Compare, typename Allocator>
map<K, T, Compare, Allocator>::map(const std::vector<value_type>& vec,
                                   sorted_tag)
    : m_root(nullptr), m_nil(nullptr), m_size(0), m_comp(), m_node_alloc()
{
    m_nil = alloc_traits::allocate(m_node_alloc, 1);
    alloc_traits::construct(m_node_alloc, m_nil);

    m_root         = build_tree(vec, 0, vec.size() - 1, 0);
    m_root->parent = m_nil;
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

    m_root         = copy_node(other.m_root, m_nil);
    m_root->parent = m_nil;
}

template <typename K, typename T, typename Compare, typename Allocator>
map<K, T, Compare, Allocator>::map(map&& other) noexcept
    : m_root(other.m_nil),
      m_nil(other.m_root),
      m_size(other.m_size),
      m_comp(std::move(other.m_comp)),
      m_node_alloc(std::move(other.m_node_alloc))
{
    other.m_nil = alloc_traits::allocate(other.m_node_alloc, 1);
    alloc_traits::construct(other.m_node_alloc, other.m_nil);
    other.m_root         = m_nil;
    other.m_root->parent = m_nil;
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
    m_root         = other.m_root;
    m_nil          = other.m_nil;
    m_size         = other.m_size;
    m_comp         = std::move(other.m_comp);
    m_node_alloc   = std::move(other.m_node_alloc);
    m_root->parent = m_nil;

    other.m_nil = alloc_traits::allocate(other.m_node_alloc, 1);
    alloc_traits::construct(other.m_node_alloc, other.m_nil);
    other.m_root         = other.m_nil;
    other.m_root->parent = other.m_nil;
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
    return at(key);
}

template <typename K, typename T, typename Compare, typename Allocator>
inline typename map<K, T, Compare, Allocator>::mapped_type&
map<K, T, Compare, Allocator>::operator[](const key_type& key)
{
    iterator it = find(key);
    if (it == end())
        it = try_emplace(key);
    return it->second;
}

template <typename K, typename T, typename Compare, typename Allocator>
inline typename map<K, T, Compare, Allocator>::mapped_type&
map<K, T, Compare, Allocator>::operator[](key_type&& key)
{
    iterator it = find(key);
    if (it == end())
        it = try_emplace(std::move(key));
    return it->second;
}

template <typename K, typename T, typename Compare, typename Allocator>
inline typename map<K, T, Compare, Allocator>::iterator
map<K, T, Compare, Allocator>::begin()
{
    return iterator(m_root, this);
}

template <typename K, typename T, typename Compare, typename Allocator>
inline typename map<K, T, Compare, Allocator>::const_iterator
map<K, T, Compare, Allocator>::begin() const
{
    return const_iterator(m_root, this);
}

template <typename K, typename T, typename Compare, typename Allocator>
inline typename map<K, T, Compare, Allocator>::const_iterator
map<K, T, Compare, Allocator>::cbegin() const noexcept
{
    return const_iterator(m_root, this);
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
inline typename map<K, T, Compare, Allocator>::reverse_iterator
map<K, T, Compare, Allocator>::rbegin() const
{
    return reverse_iterator(cend());
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
inline typename map<K, T, Compare, Allocator>::reverse_iterator
map<K, T, Compare, Allocator>::rend() const
{
    return reverse_iterator(cbegin());
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
template <typename InputIt>
inline void map<K, T, Compare, Allocator>::insert(InputIt first, InputIt last)
{
    for (; first != last; ++first)
        insert(*first);
}

template <typename K, typename T, typename Compare, typename Allocator>
template <typename InputIt>
inline void map<K, T, Compare, Allocator>::insert(
    std::initializer_list<value_type> ilist)
{
    for (const value_type& val : ilist)
        insert(std::move(val));
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
    return insert(value_type(key, std::forward<Args>(args)...));
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
    return insert(value_type(std::move(key), std::forward<Args>(args)...));
}

template <typename K, typename T, typename Compare, typename Allocator>
inline typename map<K, T, Compare, Allocator>::size_type
map<K, T, Compare, Allocator>::count(const key_type& key) const
{
    Node* cur = find_node(key);
    return cur != m_nil ? 1 : 0;
}

template <typename K, typename T, typename Compare, typename Allocator>
template <typename Key,
          std::enable_if_t<
              std::is_void<typename Compare::is_transparent>::value, int>>
inline typename map<K, T, Compare, Allocator>::size_type
map<K, T, Compare, Allocator>::count(const Key& key) const
{
    Node* cur = m_root;
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
    Node* cur = find_node(key);
    return const_iterator(cur, this);
}

template <typename K, typename T, typename Compare, typename Allocator>
template <typename Key,
          std::enable_if_t<
              std::is_void<typename Compare::is_transparent>::value, int>>
inline typename map<K, T, Compare, Allocator>::iterator
map<K, T, Compare, Allocator>::find(const Key& key)
{
    Node* cur = m_root;
    while (cur != m_nil)
    {
        if (m_comp(key, cur->value.first))
            cur = cur->left;
        else if (m_comp(cur->value.first, key))
            cur = cur->right;
        else
            return iterator(cur, this);
    }
    return iterator(m_nil, this);
}

template <typename K, typename T, typename Compare, typename Allocator>
template <typename Key,
          std::enable_if_t<
              std::is_void<typename Compare::is_transparent>::value, int>>
inline typename map<K, T, Compare, Allocator>::const_iterator
map<K, T, Compare, Allocator>::find(const Key& key) const
{
    Node* cur = m_root;
    while (cur != m_nil)
    {
        if (m_comp(key, cur->value.first))
            cur = cur->left;
        else if (m_comp(cur->value.first, key))
            cur = cur->right;
        else
            return const_iterator(cur, this);
    }
    return const_iterator(m_nil, this);
}

template <typename K, typename T, typename Compare, typename Allocator>
inline typename map<K, T, Compare, Allocator>::Node*
map<K, T, Compare, Allocator>::build_tree(std::vector<value_type>& vec,
                                          size_type left, size_type right,
                                          size_type depth)
{
    if (vec.empty())
        return m_nil;

    if (left > right)
        return m_nil;

    size_type         mid = left + (right - left) / 2;
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
}

template <typename K, typename T, typename Compare, typename Allocator>
inline void map<K, T, Compare, Allocator>::erase_balance(Node* node)
{
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

#ifndef NDEBUG
template <typename K, typename T, typename Compare, typename Allocator>
inline bool map<K, T, Compare, Allocator>::validate_tree() const
{
    if (m_root == m_nil)
        return true;

    if (m_root->color == Color::RED)
        return false;

    int black_height = 0;
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

}  // namespace demo