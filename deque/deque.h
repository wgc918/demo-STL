#pragma once

#include <type_traits>
#include <iterator>

template <typename T, typename Allocator = std::allocator<T>>
class deque;

template <typename T, typename Allocator>
bool operator==(const deque<T, Allocator> &lhs,
                const deque<T, Allocator> &rhs);

template <typename T, typename Allocator>
bool operator!=(const deque<T, Allocator> &lhs,
                const deque<T, Allocator> &rhs);

template <typename T, typename Allocator>
class deque
{
    friend bool operator== <>(const deque<T, Allocator> &lhs,
                              const deque<T, Allocator> &rhs);
    friend bool operator!= <>(const deque<T, Allocator> &lhs,
                              const deque<T, Allocator> &rhs);

public:
    using value_type = T;
    using pointer = T *;
    using const_pointer = const T *;
    using reference = T &;
    using const_reference = const T &;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;
    using allocator_type = Allocator;

    class const_iterator;
    class iterator
    {
    public:
        using iterator_category =
            std::random_access_iterator_tag;
        using value_type = T;
        using pointer = T *;
        using reference = T &;
        using difference_type = std::ptrdiff_t;
        using size_type = std::size_t;

    private:
        pointer m_ptr;
        pointer m_first;
        pointer m_last;
        value_type **m_map_node;
    };

    class const_iterator
    {
    public:
        using iterator_category =
            std::random_access_iterator_tag;
        using value_type = T;
        using pointer = const T *;
        using reference = const T &;
        using difference_type = std::ptrdiff_t;
        using size_type = std::size_t;

    private:
        pointer m_ptr;
        pointer m_first;
        pointer m_last;
        value_type **m_map_node;
    };

    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator =
        std::reverse_iterator<const_iterator>;

    deque();
    explicit deque(size_type count);
    deque(size_type count, const value_type &value);
    template <typename InputIt,
              std::enable_if_t<
                  !std::is_integral<InputIt>::value, int> = 0>
    deque(InputIt first, InputIt last);
    deque(std::initializer_list<T> ilist);
    deque(const deque &other);
    deque &operator=(const deque &other);
    deque &operator=(std::initializer_list<T> ilist);
    deque(deque &&other) noexcept;
    deque &operator=(deque &&other) noexcept;
    ~deque();

    void assign(size_type count, const value_type &value);
    template <typename InputIt,
              std::enable_if_t<
                  !std::is_integral<InputIt>::value, int> = 0>
    void assign(InputIt first, InputIt last);
    void assign(std::initializer_list<T> ilist);

    allocator_type get_allocator() const noexcept;

    reference at(size_type pos);
    const_reference at(size_type pos) const;
    reference operator[](size_type pos);
    const_reference operator[](size_type pos) const;
    reference front();
    const_reference front() const;
    reference back();
    const_reference back() const;

    iterator begin();
    const_iterator begin() const;
    const_iterator cbegin() const;
    iterator end();
    const_iterator end() const;
    const_iterator cend() const;
    reverse_iterator rbegin();
    const_reverse_iterator rbegin() const;
    const_reverse_iterator crbegin() const;
    reverse_iterator rend();
    const_reverse_iterator rend() const;
    const_reverse_iterator crend() const;

    bool empty() const;
    size_type size() const;
    size_type max_size() const;
    void resize(size_type count);
    void resize(size_type count, const value_type &value);
    void shrink_to_fit();

    void clear() noexcept;
    iterator insert(const_iterator pos,
                    const value_type &value);
    iterator insert(const_iterator pos,
                    const value_type &&value);
    iterator insert(const_iterator pos,
                    size_type count, const value_type &value);
    template <typename InputIt,
              std::enable_if_t<
                  !std::is_integral<InputIt>::value, int> = 0>
    iterator insert(const_iterator pos,
                    size_type count, InputIt first, InputIt last);
    iterator insert(const_iterator pos,
                    std::initializer_list<T> ilist);
    template <typename... Args>
    reference emplace(const_iterator pos, Args &&...args);
    iterator erase(const_iterator pos);
    iterator erase(const_iterator first, const_iterator last);
    void push_front(const value_type &value);
    void push_front(const value_type &&value);
    void push_back(const value_type &value);
    void push_back(const value_type &&value);
    template <typename... Args>
    reference emplace_back(Args &&...args);
    template <typename... Args>
    reference emplace_front(Args &&...args);
    void pop_front();
    void pop_back();
    void swap(deque &other) noexcept;

private:
    value_type **m_map;
    size_type m_map_size;
    allocator_type m_allocator;
    iterator m_begin;
    iterator m_end;
    static const size_type m_buffer_size =
        512 / sizeof(value_type);
};