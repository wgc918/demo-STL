#pragma once

#include <initializer_list>
#include <iterator>
#include <memory>
#include <type_traits>
#include <vector>

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
    using pointer          = T*;
    using const_pointer    = const T*;
    using reference        = T&;
    using const_reference  = const T&;

public:
    struct sorted_tag;
    struct unsorted_tag;

private:
    struct Node
    {
        key_type    key;
        mapped_type value;
        Node*       left;
        Node*       right;
        Node*       parent;
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
        explicit iterator(Node* node);
        ~iterator();
        iterator(const iterator&);

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
        explicit const_iterator(Node* node);
        ~const_iterator();
        const_iterator(const const_iterator&);

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
    //新增接口
    explicit map(const std::vector<value_type>& vec, sorted_tag);
    explicit map(const std::vector<value_type>& vec, unsorted_tag);
    map(const map& other);
    map(map&& other) noexcept;
    ~map();

    map& operator=(const map& other);
    map& operator=(map&& other) noexcept;
    map& operator=(std::initializer_list<value_type> ilist);

    allocator_type get_allocator() const;

    reference at(const key_type& key);
    const_reference at(const key_type& key) const;
    reference operator[](const key_type& key);
    reference operator[](const key_type&& key);

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

private:
    using node_alloc_type = typename std::allocator_traits<
        allocator_type>::template rebind_alloc<Node>;
    using alloc_traits = std::allocator_traits<node_alloc_type>;

private:
    Node*           m_root;
    size_type       m_size;
    Compare         m_comp;
    Allocator       m_alloc;
    node_alloc_type m_node_alloc;
};

}  // namespace demo