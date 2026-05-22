#pragma once

#include <functional>

namespace demo
{

#define UNORDERED_MAP_DEFAULT_BUCKET_COUNT 16

template <typename Key, typename T, typename Hash = std::hash<Key>,
          typename KeyEqual  = std::equal_to<Key>,
          typename Allocator = std::allocator<std::pair<const Key, T>>>
class unordered_map;

template <typename Key, typename T, typename Hash, typename KeyEqual,
          typename Allocator>
bool operator==(const unordered_map<Key, T, Hash, KeyEqual, Allocator>& lhs,
                const unordered_map<Key, T, Hash, KeyEqual, Allocator>& rhs);
template <typename Key, typename T, typename Hash, typename KeyEqual,
          typename Allocator>
bool operator!=(const unordered_map<Key, T, Hash, KeyEqual, Allocator>& lhs,
                const unordered_map<Key, T, Hash, KeyEqual, Allocator>& rhs);

template <typename Key, typename T, typename Hash, typename KeyEqual,
          typename Allocator>
class unordered_map
{
    friend bool operator==
        <>(const unordered_map& lhs, const unordered_map& rhs);
    friend bool operator!=
        <>(const unordered_map& lhs, const unordered_map& rhs);

public:
    using key_type        = Key;
    using mapped_type     = T;
    using hash_type       = Hash;
    using key_equal       = KeyEqual;
    using allocator_type  = Allocator;
    using value_type      = std::pair<const Key, T>;
    using size_type       = std::size_t;
    using difference_type = std::ptrdiff_t;
    using pointer         = T*;
    using const_pointer   = const T*;
    using reference       = T&;
    using const_reference = const T&;

public:
    class const_itrator;

    class iterator
    {
        friend class unordered_map;

    public:
        using iterator_category = std::forward_iterator_tag;
        using value_type        = value_type;
        using pointer           = pointer;
        using reference         = reference;
        using difference_type   = difference_type;
    };

    class const_iterator
    {
        friend class unordered_map;

    public:
        using iterator_category = std::forward_iterator_tag;
        using value_type        = value_type;
        using pointer           = const_pointer;
        using reference         = const_reference;
        using difference_type   = difference_type;
    };

    using local_iterator       = iterator;
    using const_local_iterator = const_iterator;

public:
    unordered_map();
    explicit unordered_map(
        size_type bucket_count = UNORDERED_MAP_DEFAULT_BUCKET_COUNT);
    template <typename InputIt>
    unordered_map(InputIt first, InputIt last,
                  size_type bucket_count = UNORDERED_MAP_DEFAULT_BUCKET_COUNT);
    unordered_map(std::initializer_list<value_type> ilist,
                  size_type bucket_count = UNORDERED_MAP_DEFAULT_BUCKET_COUNT);
    unordered_map(const unordered_map& other);
    unordered_map(unordered_map&& other) noexcept;
    unordered_map& operator=(const unordered_map& other);
    unordered_map& operator=(unordered_map&& other) noexcept;
    ~unordered_map();

    allocator_type get_allocator() const noexcept;

    iterator begin() noexcept;
    const_iterator begin() const noexcept;
    const_iterator cbegin() const noexcept;
    iterator end() noexcept;
    const_iterator end() const noexcept;
    const_iterator cend() const noexcept;

    bool empty() const noexcept;
    size_type size() const noexcept;
    size_type max_size() const noexcept;

    void clear() noexcept;

    std::pair<iterator, bool> insert(const value_type& value);
    std::pair<iterator, bool> insert(value_type&& value);
    template <class P,
              std::enable_if_t<std::is_constructible_v<value_type, P>> = 0>
    std::pair<iterator, bool> insert(P&& value);
    iterator insert(const_iterator hint, const value_type& value);
    iterator insert(const_iterator hint, value_type&& value);
    template <class P,
              std::enable_if_t<std::is_constructible_v<value_type, P>> = 0>
    iterator insert(const_iterator hint, P&& value);
    template <class InputIt>
    void insert(InputIt first, InputIt last);
    void insert(std::initializer_list<value_type> ilist);

    template <class M>
    std::pair<iterator, bool> insert_or_assign(const Key& k, M&& obj);
    template <class M>
    std::pair<iterator, bool> insert_or_assign(Key&& k, M&& obj);
    template <class M>
    iterator insert_or_assign(const_iterator hint, const Key& k, M&& obj);
    template <class M>
    iterator insert_or_assign(const_iterator hint, Key&& k, M&& obj);
    template <class... Args>
    std::pair<iterator, bool> emplace(Args&&... args);
    template <class... Args>
    iterator emplace_hint(const_iterator hint, Args&&... args);
    template <class... Args>
    std::pair<iterator, bool> try_emplace(const Key& k, Args&&... args);
    template <class... Args>
    std::pair<iterator, bool> try_emplace(Key&& k, Args&&... args);
    template <class... Args>
    iterator try_emplace(const_iterator hint, const Key& k, Args&&... args);
    template <class... Args>
    iterator try_emplace(const_iterator hint, Key&& k, Args&&... args);
    iterator erase(iterator pos);
    iterator erase(const_iterator pos);
    iterator erase(const_iterator first, const_iterator last);
    size_type erase(const Key& key);

    void merge(const unordered_map& other);
    template <class H2, class P2>
    void merge(std::unordered_map<Key, T, H2, P2, Allocator>& source);
    template <class H2, class P2>
    void merge(std::unordered_map<Key, T, H2, P2, Allocator>&& source);
    template <class H2, class P2>
    void merge(std::unordered_multimap<Key, T, H2, P2, Allocator>& source);
    template <class H2, class P2>
    void merge(std::unordered_multimap<Key, T, H2, P2, Allocator>&& source);

    T& at(const Key& key);
    const T& at(const Key& key) const;
    T& operator[](const Key& key);
    T& operator[](Key&& key);

    size_type count(const Key& key) const;
    iterator find(const Key& key);
    const_iterator find(const Key& key) const;
    std::pair<iterator, iterator> equal_range(const Key& key);
    std::pair<const_iterator, const_iterator> equal_range(const Key& key) const;

    local_iterator begin(size_type n);
    const_local_iterator begin(size_type n) const;
    const_local_iterator cbegin(size_type n) const;
    local_iterator end(size_type n);
    const_local_iterator end(size_type n) const;
    const_local_iterator cend(size_type n) const;

    size_type bucket_count() const;

private:
};
}  // namespace demo