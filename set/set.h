#pragma once

#include <initializer_list>
#include <iterator>
#include <memory>
#include <type_traits>
#include <vector>

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

private:
    enum Color : bool
    {
        RED   = false,
        BLACK = true,
    };

    struct Node
    {
        value_type key;
        Node*      left;
        Node*      right;
        Node*      parent;
        Color      color;
    };

public:
    struct sorted_tag
    {
    };

    struct unsorted_tag
    {
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
        explicit iterator(Node* node);
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
        explicit const_iterator(Node* node);
        const_iterator(const iterator& other);

        pointer operator->() const;
        reference operator*() const;

        const_iterator& operator++();
        const_iterator& operator--();
        const_iterator operator++(int);
        const_iterator operator--(int);

        bool operator==(const const_iterator&) const;
        bool operator!=(const const_iterator&) const;

    private:
        Node* node;
        set*  m_container;
    };

    using reverse_iterator       = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

public:
    set();
    explicit set(const Compare& comp);
    // 非标准构造函数，用于从排序的vector创建set
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

private:
    Node* build_tree(std::vector<value_type>& values, int left, int right,
                     size_type depth);
    Node* copy_node(Node* node);
    void destroy(Node* node);
    void insert_balance(Node* node);
    void erase_balance(Node* node);
    Node* rotate_left(Node* node);
    Node* rotate_right(Node* node);
    Node* find_node(const key_type& key) const;
    Node* max_node(Node* node) const;

private:
    using node_alloc_type =
        typename std::allocator_traits<Allocator>::template rebind_alloc<Node>;
    using alloc_traits = std::allocator_traits<node_alloc_type>;

private:
    Node*           m_nil;
    Node*           m_root;
    size_type       m_size;
    Compare         m_comp;
    node_alloc_type m_node_alloc;
};

}  // namespace demo
