#pragma once

#include <type_traits>
#include <iterator>

namespace demo
{
    /**
     * @brief deque 容器默认初始化大小
     * @details deque 容器默认初始化大小为 8
     */
#define DEQUE_DEFAULT_INIT_SIZE 8

    /**
     * @brief deque 容器
     * @details deque 容器是一个双端队列，支持在头尾两端高效插入和删除元素。
     */
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

            iterator();
            iterator(pointer cur, pointer first, pointer last, value_type **map_node);
            reference operator*() const;
            pointer operator->() const;
            difference_type operator-(const iterator &other) const;
            iterator operator+(difference_type n) const;
            iterator operator-(difference_type n) const;
            iterator &operator+=(difference_type n);
            iterator &operator-=(difference_type n);
            iterator &operator++();
            iterator &operator--();
            iterator operator++(int);
            iterator operator--(int);
            iterator operator[](difference_type n) const;
            iterator operator[](difference_type n);
            bool operator==(const iterator &other) const;
            bool operator!=(const iterator &other) const;
            bool operator<(const iterator &other) const;
            bool operator>(const iterator &other) const;
            bool operator<=(const iterator &other) const;
            bool operator>=(const iterator &other) const;

        private:
            pointer m_cur;           // 当前元素指针
            pointer m_first;         // 当前槽的第一个元素指针
            pointer m_last;          // 当前槽的最后一个元素指针
            value_type **m_map_node; // 当前槽的指针
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

            const_iterator();
            const_iterator(pointer ptr, pointer first, pointer last, value_type **map_node);
            const_iterator(const iterator &other);

            reference operator*() const;
            pointer operator->() const;
            difference_type operator-(const const_iterator &other) const;
            const_iterator operator+(difference_type n) const;
            const_iterator operator-(difference_type n) const;
            const_iterator &operator+=(difference_type n);
            const_iterator &operator-=(difference_type n);
            const_iterator &operator++();
            const_iterator &operator--();
            const_iterator operator++(int);
            const_iterator operator--(int);
            const_iterator operator[](difference_type n) const;
            const_iterator operator[](difference_type n);
            bool operator==(const const_iterator &other) const;
            bool operator!=(const const_iterator &other) const;
            bool operator<(const const_iterator &other) const;
            bool operator>(const const_iterator &other) const;
            bool operator<=(const const_iterator &other) const;
            bool operator>=(const const_iterator &other) const;

        private:
            pointer m_cur;           // 当前元素指针
            pointer m_first;         // 当前槽的第一个元素指针
            pointer m_last;          // 当前槽的最后一个元素指针
            value_type **m_map_node; // 当前槽的指针
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
        iterator insert(const_iterator pos, InputIt first, InputIt last);
        iterator insert(const_iterator pos, std::initializer_list<T> ilist);
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
        using map_allocator_type =
            typename std::allocator_traits<allocator_type>::
                template rebind_alloc<pointer>;
        using alloc_traits = std::allocator_traits<allocator_type>;
        using map_alloc_traits =
            std::allocator_traits<map_allocator_type>;

    private:
        value_type **m_map;
        size_type m_map_size;
        allocator_type m_allocator;
        map_allocator_type m_map_allocator;
        iterator m_begin;
        iterator m_end;
        static const size_type m_buffer_size =
            512 / sizeof(value_type);
    };

    // ----------------------- iterator 实现 ---------------------------
    template <typename T, typename Allocator>
    inline deque<T, Allocator>::iterator::iterator()
        : m_cur(nullptr), m_first(nullptr),
          m_last(nullptr), m_map_node(nullptr)
    {
    }

    template <typename T, typename Allocator>
    inline deque<T, Allocator>::iterator::
        iterator(pointer cur, pointer first, pointer last, value_type **map_node)
        : m_cur(cur), m_first(first), m_last(last), m_map_node(map_node)
    {
    }

    template <typename T, typename Allocator>
    inline typename deque<T, Allocator>::reference
    deque<T, Allocator>::iterator::operator*() const
    {
        return *m_cur;
    }
    template <typename T, typename Allocator>
    inline typename deque<T, Allocator>::pointer
    deque<T, Allocator>::iterator::operator->() const
    {
        return m_cur;
    }

    template <typename T, typename Allocator>
    inline typename deque<T, Allocator>::difference_type
    deque<T, Allocator>::iterator::operator-(const iterator &other) const
    {
        // 如果两个迭代器指向同一个槽，直接返回元素之间的距离
        if (m_map_node == other.m_map_node)
        {
            return m_cur - other.m_cur;
        }
        // 否则，返回跨槽的距离
        else
        {
            difference_type distance = 0;
            size_type slot_distance = m_map_node - other.m_map_node - 1;
            distance += slot_distance * m_buffer_size;
            distance += m_cur - m_first;
            distance += other.m_last - other.m_cur;

            return distance;
        }
    }

    template <typename T, typename Allocator>
    inline typename deque<T, Allocator>::iterator
    deque<T, Allocator>::iterator::operator+(difference_type n) const
    {
        if (m_cur + n < m_last)
        {
            return iterator(m_cur + n, m_first, m_last, m_map_node);
        }
        else
        {
            n -= m_last - m_cur - 1;
            // 计算新的槽指针和元素指针
            value_type **new_map_node = m_map_node + (n / m_buffer_size + 1);
            pointer new_cur = *new_map_node + n % m_buffer_size;
            return iterator(new_cur, *new_map_node, *new_map_node + m_buffer_size, new_map_node);
        }
    }

    template <typename T, typename Allocator>
    inline typename deque<T, Allocator>::iterator
    deque<T, Allocator>::iterator::operator-(difference_type n) const
    {
        if (m_cur - n >= m_first)
        {
            return iterator(m_cur - n, m_first, m_last, m_map_node);
        }
        else
        {
            n -= m_cur - m_first;
            // 计算新的槽指针和元素指针
            value_type **new_map_node = m_map_node - (n / m_buffer_size + 1);
            pointer new_cur = *new_map_node + m_buffer_size - n % m_buffer_size;
            return iterator(new_cur, *new_map_node, *new_map_node + m_buffer_size, new_map_node);
        }
    }

    template <typename T, typename Allocator>
    inline typename deque<T, Allocator>::iterator &
    deque<T, Allocator>::iterator::operator+=(difference_type n)
    {
        *this = *this + n;
        return *this;
    }

    template <typename T, typename Allocator>
    inline typename deque<T, Allocator>::iterator &
    deque<T, Allocator>::iterator::operator-=(difference_type n)
    {
        *this = *this - n;
        return *this;
    }

    template <typename T, typename Allocator>
    inline typename deque<T, Allocator>::iterator &
    deque<T, Allocator>::iterator::operator++()
    {
        if (m_cur + 1 < m_last)
            m_cur++;
        else
        {
            m_map_node++;
            m_cur = *m_map_node;
        }
        return *this;
    }

    template <typename T, typename Allocator>
    inline typename deque<T, Allocator>::iterator &
    deque<T, Allocator>::iterator::operator--()
    {
        if (m_cur - 1 >= m_first)
            m_cur--;
        else
        {
            m_map_node--;
            m_cur = *m_map_node + m_buffer_size - 1;
        }
        return *this;
    }

    template <typename T, typename Allocator>
    inline typename deque<T, Allocator>::iterator
    deque<T, Allocator>::iterator::operator++(int)
    {
        iterator temp(*this);
        if (m_cur + 1 < m_last)
            m_cur++;
        else
        {
            m_map_node++;
            m_cur = *m_map_node;
        }
        return temp;
    }

    template <typename T, typename Allocator>
    inline typename deque<T, Allocator>::iterator
    deque<T, Allocator>::iterator::operator--(int)
    {
        iterator temp(*this);
        if (m_cur - 1 >= m_first)
            m_cur--;
        else
        {
            m_map_node--;
            m_cur = *m_map_node + m_buffer_size - 1;
        }
        return temp;
    }

    template <typename T, typename Allocator>
    inline typename deque<T, Allocator>::iterator
    deque<T, Allocator>::iterator::operator[](difference_type n) const
    {
        return *this + n;
    }
    template <typename T, typename Allocator>
    inline typename deque<T, Allocator>::iterator
    deque<T, Allocator>::iterator::operator[](difference_type n)
    {
        return *this + n;
    }

    template <typename T, typename Allocator>
    inline bool deque<T, Allocator>::iterator::
    operator==(const iterator &other) const
    {
        return (m_cur == other.m_cur) && (m_first == other.m_first) &&
               (m_last == other.m_last) && (m_map_node == other.m_map_node);
    }

    template <typename T, typename Allocator>
    inline bool deque<T, Allocator>::iterator::
    operator!=(const iterator &other) const
    {
        return !(*this == other);
    }

    template <typename T, typename Allocator>
    inline bool deque<T, Allocator>::iterator::
    operator<(const iterator &other) const
    {
        if (m_map_node == other.m_map_node)
        {
            return m_cur < other.m_cur;
        }
        else
        {
            return m_map_node < other.m_map_node;
        }
    }

    template <typename T, typename Allocator>
    inline bool deque<T, Allocator>::iterator::
    operator>(const iterator &other) const
    {
        return !(*this < other) && *this != other;
    }

    template <typename T, typename Allocator>
    inline bool deque<T, Allocator>::iterator::
    operator<=(const iterator &other) const
    {
        return !(*this > other);
    }

    template <typename T, typename Allocator>
    inline bool deque<T, Allocator>::iterator::
    operator>=(const iterator &other) const
    {
        return !(*this < other);
    }

    // ---------------------- const_iterator 实现 -------------------
    template <typename T, typename Allocator>
    inline deque<T, Allocator>::const_iterator::const_iterator()
        : m_cur(nullptr), m_first(nullptr),
          m_last(nullptr), m_map_node(nullptr)
    {
    }

    template <typename T, typename Allocator>
    inline deque<T, Allocator>::const_iterator::
        const_iterator(pointer ptr, pointer first,
                       pointer last, value_type **map_node)
        : m_cur(ptr), m_first(first), m_last(last), m_map_node(map_node)
    {
    }

    template <typename T, typename Allocator>
    inline deque<T, Allocator>::const_iterator::
        const_iterator(const iterator &other)
        : m_cur(other.m_cur), m_first(other.m_first),
          m_last(other.m_last), m_map_node(other.m_map_node)
    {
    }

    template <typename T, typename Allocator>
    inline typename deque<T, Allocator>::const_iterator::reference
    deque<T, Allocator>::const_iterator::operator*() const
    {
        return *m_cur;
    }
    template <typename T, typename Allocator>
    inline typename deque<T, Allocator>::const_iterator::pointer
    deque<T, Allocator>::const_iterator::operator->() const
    {
        return m_cur;
    }

    template <typename T, typename Allocator>
    inline typename deque<T, Allocator>::difference_type
    deque<T, Allocator>::const_iterator::operator-(const const_iterator &other) const
    {
        // 如果两个迭代器指向同一个槽，直接返回元素之间的距离
        if (m_map_node == other.m_map_node)
        {
            return m_cur - other.m_cur;
        }
        // 否则，返回跨槽的距离
        else
        {
            difference_type distance = 0;
            size_type slot_distance = m_map_node - other.m_map_node - 1;
            distance += slot_distance * m_buffer_size;
            distance += m_cur - m_first;
            distance += other.m_last - other.m_cur;

            return distance;
        }
    }

    template <typename T, typename Allocator>
    inline typename deque<T, Allocator>::const_iterator
    deque<T, Allocator>::const_iterator::operator+(difference_type n) const
    {
        if (m_cur + n < m_last)
        {
            return const_iterator(m_cur + n, m_first, m_last, m_map_node);
        }
        else
        {
            n -= m_last - m_cur - 1;
            // 计算新的槽指针和元素指针
            value_type **new_map_node = m_map_node + (n / m_buffer_size + 1);
            pointer new_cur = *new_map_node + n % m_buffer_size;
            return const_iterator(new_cur, *new_map_node, *new_map_node + m_buffer_size, new_map_node);
        }
    }

    template <typename T, typename Allocator>
    inline typename deque<T, Allocator>::const_iterator
    deque<T, Allocator>::const_iterator::operator-(difference_type n) const
    {
        if (m_cur - n >= m_first)
        {
            return const_iterator(m_cur - n, m_first, m_last, m_map_node);
        }
        else
        {
            n -= m_cur - m_first;
            // 计算新的槽指针和元素指针
            value_type **new_map_node = m_map_node - (n / m_buffer_size + 1);
            pointer new_cur = *new_map_node + m_buffer_size - n % m_buffer_size;
            return const_iterator(new_cur, *new_map_node, *new_map_node + m_buffer_size, new_map_node);
        }
    }

    template <typename T, typename Allocator>
    inline typename deque<T, Allocator>::const_iterator &
    deque<T, Allocator>::const_iterator::operator+=(difference_type n)
    {
        *this = *this + n;
        return *this;
    }

    template <typename T, typename Allocator>
    inline typename deque<T, Allocator>::const_iterator &
    deque<T, Allocator>::const_iterator::operator-=(difference_type n)
    {
        *this = *this - n;
        return *this;
    }

    template <typename T, typename Allocator>
    inline typename deque<T, Allocator>::const_iterator &
    deque<T, Allocator>::const_iterator::operator++()
    {
        if (m_cur + 1 < m_last)
            m_cur++;
        else
        {
            m_map_node++;
            m_cur = *m_map_node;
        }
        return *this;
    }

    template <typename T, typename Allocator>
    inline typename deque<T, Allocator>::const_iterator &
    deque<T, Allocator>::const_iterator::operator--()
    {
        if (m_cur - 1 >= m_first)
            m_cur--;
        else
        {
            m_map_node--;
            m_cur = *m_map_node + m_buffer_size - 1;
        }
        return *this;
    }

    template <typename T, typename Allocator>
    inline typename deque<T, Allocator>::const_iterator
    deque<T, Allocator>::const_iterator::operator++(int)
    {
        const_iterator temp(*this);
        if (m_cur + 1 < m_last)
            m_cur++;
        else
        {
            m_map_node++;
            m_cur = *m_map_node;
        }
        return temp;
    }

    template <typename T, typename Allocator>
    inline typename deque<T, Allocator>::const_iterator
    deque<T, Allocator>::const_iterator::operator--(int)
    {
        const_iterator temp(*this);
        if (m_cur - 1 >= m_first)
            m_cur--;
        else
        {
            m_map_node--;
            m_cur = *m_map_node + m_buffer_size - 1;
        }
        return temp;
    }

    template <typename T, typename Allocator>
    inline typename deque<T, Allocator>::const_iterator
    deque<T, Allocator>::const_iterator::operator[](difference_type n) const
    {
        return *this + n;
    }

    template <typename T, typename Allocator>
    inline typename deque<T, Allocator>::const_iterator
    deque<T, Allocator>::const_iterator::operator[](difference_type n)
    {
        return *this + n;
    }

    template <typename T, typename Allocator>
    inline bool deque<T, Allocator>::const_iterator::
    operator==(const const_iterator &other) const
    {
        return (m_cur == other.m_cur) && (m_first == other.m_first) &&
               (m_last == other.m_last) && (m_map_node == other.m_map_node);
    }

    template <typename T, typename Allocator>
    inline bool deque<T, Allocator>::const_iterator::
    operator!=(const const_iterator &other) const
    {
        return !(*this == other);
    }

    template <typename T, typename Allocator>
    inline bool deque<T, Allocator>::const_iterator::
    operator<(const const_iterator &other) const
    {
        if (m_map_node == other.m_map_node)
        {
            return m_cur < other.m_cur;
        }
        else
        {
            return m_map_node < other.m_map_node;
        }
    }

    template <typename T, typename Allocator>
    inline bool deque<T, Allocator>::const_iterator::
    operator>(const const_iterator &other) const
    {
        return !(*this < other) && *this != other;
    }

    template <typename T, typename Allocator>
    inline bool deque<T, Allocator>::const_iterator::
    operator<=(const const_iterator &other) const
    {
        return !(*this > other);
    }

    template <typename T, typename Allocator>
    inline bool deque<T, Allocator>::const_iterator::
    operator>=(const const_iterator &other) const
    {
        return !(*this < other);
    }

    // ----------------------- deque 实现 ---------------------------
    template <typename T, typename Allocator>
    deque<T, Allocator>::deque()
        : m_map(nullptr), m_map_size(DEQUE_DEFAULT_INIT_SIZE),
          m_allocator(), m_map_allocator(), m_begin(), m_end()
    {
        m_map = map_alloc_traits::allocate(m_map_allocator, m_map_size);
        for (size_type i = 0; i < m_map_size; i++)
            m_map[i] = nullptr;

        m_begin.m_cur = nullptr;
        m_begin.m_first = nullptr;
        m_begin.m_last = nullptr;
        m_begin.m_map_node = m_map + m_map_size / 2;

        m_end = m_begin;
    }

    template <typename T, typename Allocator>
    deque<T, Allocator>::deque(size_type count)
        : m_map(nullptr), m_map_size(DEQUE_DEFAULT_INIT_SIZE),
          m_allocator(), m_map_allocator(), m_begin(), m_end()
    {
        size_type slots = (count + m_buffer_size - 1) / m_buffer_size;
        m_map_size = std::max(slots, m_map_size);

        m_map = map_alloc_traits::allocate(m_map_allocator, m_map_size);
        for (size_type i = 0; i < m_map_size; i++)
            m_map[i] = nullptr;

        size_type first_slot = (m_map_size - slots) / 2;
        size_type remaining = count;

        // 默认从 first_slot 开始填充， 每个槽位填充 m_buffer_size 个元素
        // 最后一个槽位可能填充不足 m_buffer_size 个元素
        for (size_type i = first_slot; i < first_slot + slots; i++)
        {
            m_map[i] = alloc_traits::allocate(m_allocator, m_buffer_size);

            for (size_type j = 0; j < std::min(m_buffer_size, remaining); j++)
                alloc_traits::construct(m_allocator, m_map[i] + j);

            remaining -= m_buffer_size;
        }

        // m_begin 指向第一个元素
        m_begin.m_cur = m_map[first_slot];
        m_begin.m_first = m_map[first_slot];
        m_begin.m_last = m_map[first_slot] + m_buffer_size;
        m_begin.m_map_node = m_map + first_slot;

        // m_end 指向最后一个元素的下一个位置
        const size_type last_slot = first_slot + slots - 1;
        m_end.m_cur = m_map[last_slot] + (count % m_buffer_size == 0
                                              ? m_buffer_size
                                              : count % m_buffer_size);
        m_end.m_first = m_map[last_slot];
        m_end.m_last = m_map[last_slot] + m_buffer_size;
        m_end.m_map_node = m_map + last_slot;
    }

    template <typename T, typename Allocator>
    deque<T, Allocator>::deque(size_type count, const value_type &value)
        : m_map(nullptr), m_map_size(DEQUE_DEFAULT_INIT_SIZE),
          m_allocator(), m_map_allocator(), m_begin(), m_end()
    {
        assign(count, value);
    }

    template <typename T, typename Allocator>
    template <typename InputIt,
              std::enable_if_t<
                  !std::is_integral<InputIt>::value, int>>
    deque<T, Allocator>::deque(InputIt first, InputIt last)
        : m_map(nullptr), m_map_size(8), m_allocator(),
          m_map_allocator(), m_begin(), m_end()
    {
        assign(first, last);
    }

    template <typename T, typename Allocator>
    deque<T, Allocator>::deque(std::initializer_list<T> ilist)
        : m_map(nullptr), m_map_size(DEQUE_DEFAULT_INIT_SIZE),
          m_allocator(), m_map_allocator(), m_begin(), m_end()
    {
        assign(ilist);
    }

    template <typename T, typename Allocator>
    deque<T, Allocator>::deque(const deque &other)
        : m_map(nullptr), m_map_size(other.m_map_size),
          m_allocator(), m_map_allocator(), m_begin(), m_end()
    {
        m_map = map_alloc_traits::allocate(m_map_allocator, m_map_size);
        size_type first_slot = other.m_begin.m_map_node - m_map;
        size_type slots = other.m_end.m_map_node - other.m_begin.m_map_node + 1;

        // 单独处理第一个槽
        m_map[first_slot] = alloc_traits::allocate(m_allocator, m_buffer_size);
        size_type first_slot_size = std::min(m_buffer_size,
                                             other.m_begin.m_last - other.m_begin.m_cur);
        for (size_type j = (other.m_begin.m_first - other.m_map[first_slot]);
             j < first_slot_size; j++)
            alloc_traits::construct(m_allocator, m_map[first_slot] + j,
                                    other.m_map[first_slot] + j);

        // 单独处理最后一个槽
        size_type last_slot = first_slot + slots - 1;
        m_map[last_slot] = alloc_traits::allocate(m_allocator, m_buffer_size);
        size_type last_slot_size = std::min(m_buffer_size,
                                            other.m_end.m_cur - other.m_end.m_first);
        for (size_type j = 0; j < last_slot_size; j++)
            alloc_traits::construct(m_allocator, m_map[last_slot] + j,
                                    other.m_map[last_slot] + j);

        // 中间槽位一定都是满的
        for (size_type i = first_slot + 1; i < first_slot + slots - 1; i++)
        {
            m_map[i] = alloc_traits::allocate(m_allocator, m_buffer_size);
            for (size_type j = 0; j < m_buffer_size; j++)
                alloc_traits::construct(m_allocator, m_map[i] + j,
                                        other.m_map[i] + j);
        }

        // m_begin 指向第一个元素
        m_begin.m_cur = m_map[first_slot] + (other.m_begin.m_first -
                                             other.m_map[first_slot]);
        m_begin.m_first = m_map[first_slot];
        m_begin.m_last = m_map[first_slot] + m_buffer_size;
        m_begin.m_map_node = m_map + first_slot;

        // m_end 指向最后一个元素的下一个位置
        m_end.m_cur = m_map[last_slot] + (other.m_end.m_cur -
                                          other.m_end.m_first);
        m_end.m_first = m_map[last_slot];
        m_end.m_last = m_map[last_slot] + m_buffer_size;
        m_end.m_map_node = m_map + last_slot;
    }

    template <typename T, typename Allocator>
    deque<T, Allocator> &deque<T, Allocator>::operator=(const deque &other)
    {
        if (this != &other)
        {
            clear();
            assign(other);
        }
        return *this;
    }

    template <typename T, typename Allocator>
    deque<T, Allocator> &deque<T, Allocator>::
    operator=(std::initializer_list<T> ilist)
    {
        clear();
        assign(ilist);
        return *this;
    }

    template <typename T, typename Allocator>
    deque<T, Allocator>::deque(deque &&other) noexcept
        : m_map(other.m_map), m_map_size(other.m_map_size),
          m_allocator(std::move(other.m_allocator)),
          m_map_allocator(std::move(other.m_map_allocator)),
          m_begin(other.m_begin), m_end(other.m_end)
    {
        // other 重置为有效空状态
        other.m_map_size = DEQUE_DEFAULT_INIT_SIZE;
        other.m_map = m_map_allocator::allocate(m_map_allocator, m_map_size);
        other.m_begin = iterator();
        other.m_end = iterator();
    }

    template <typename T, typename Allocator>
    deque<T, Allocator> &deque<T, Allocator>::operator=(deque &&other) noexcept
    {
        if (this != &other)
        {
            clear();
            m_map = other.m_map;
            m_map_size = other.m_map_size;
            m_allocator = std::move(other.m_allocator);
            m_map_allocator = std::move(other.m_map_allocator);
            m_begin = other.m_begin;
            m_end = other.m_end;

            // other 重置为有效空状态
            other.m_map_size = DEQUE_DEFAULT_INIT_SIZE;
            other.m_map = m_map_allocator::allocate(m_map_allocator, m_map_size);
            other.m_begin = iterator();
            other.m_end = iterator();
        }
        return *this;
    }

    template <typename T, typename Allocator>
    deque<T, Allocator>::~deque()
    {
        clear();
        map_alloc_traits::deallocate(m_map_allocator, m_map, m_map_size);
    }

    template <typename T, typename Allocator>
    void deque<T, Allocator>::assign(size_type count,
                                     const value_type &value)
    {
        clear();

        size_type slots = (count + m_buffer_size - 1) / m_buffer_size;
        m_map_size = std::max(slots, m_map_size);
        m_map = map_alloc_traits::allocate(m_map_allocator, m_map_size);
        for (size_type i = 0; i < m_map_size; i++)
            m_map[i] = nullptr;

        size_type first_slot = (m_map_size - slots) / 2;
        size_type remaining = count;
        for (size_type i = first_slot; i < first_slot + slots; i++)
        {
            m_map[i] = alloc_traits::allocate(m_allocator, m_buffer_size);
            for (size_type j = 0; j < std::min(m_buffer_size, remaining); j++)
                alloc_traits::construct(m_allocator, m_map[i] + j, value);
            remaining -= m_buffer_size;
        }

        m_begin.m_cur = m_map[first_slot];
        m_begin.m_first = m_map[first_slot];
        m_begin.m_last = m_map[first_slot] + m_buffer_size;
        m_begin.m_map_node = m_map + first_slot;

        size_type last_slot = first_slot + slots - 1;
        m_end.m_cur = m_map[last_slot] + (count % m_buffer_size == 0
                                              ? m_buffer_size
                                              : count % m_buffer_size);
        m_end.m_first = m_map[last_slot];
        m_end.m_last = m_map[last_slot] + m_buffer_size;
        m_end.m_map_node = m_map + last_slot;
    }

    template <typename T, typename Allocator>
    template <typename InputIt,
              std::enable_if_t<
                  !std::is_integral<InputIt>::value, int>>
    void deque<T, Allocator>::assign(InputIt first, InputIt last)
    {
        clear();

        if (first == last)
            return;

        size_type count = std::distance(first, last);
        size_type slots = (count + m_buffer_size - 1) / m_buffer_size;
        m_map_size = std::max(slots, m_map_size);
        m_map = map_alloc_traits::allocate(m_map_allocator, m_map_size);
        for (size_type i = 0; i < m_map_size; i++)
            m_map[i] = nullptr;

        size_type first_slot = (m_map_size - slots) / 2;
        size_type remaining = count;

        for (size_type i = first_slot; i < first_slot + slots; i++)
        {
            m_map[i] = alloc_traits::allocate(m_allocator, m_buffer_size);
            for (size_type j = 0;
                 j < std::min(m_buffer_size, remaining) && first != last; j++)
                alloc_traits::construct(m_allocator, m_map[i] + j, *first++);
            remaining -= m_buffer_size;
        }

        m_begin.m_cur = m_map[first_slot];
        m_begin.m_first = m_map[first_slot];
        m_begin.m_last = m_map[first_slot] + m_buffer_size;
        m_begin.m_map_node = m_map + first_slot;

        size_type last_slot = first_slot + slots - 1;
        m_end.m_cur = m_map[last_slot] + (count % m_buffer_size == 0
                                              ? m_buffer_size
                                              : count % m_buffer_size);
        m_end.m_first = m_map[last_slot];
        m_end.m_last = m_map[last_slot] + m_buffer_size;
        m_end.m_map_node = m_map + last_slot;
    }
    template <typename T, typename Allocator>
    void deque<T, Allocator>::assign(std::initializer_list<T> ilist)
    {
        assign(ilist.begin(), ilist.end());
    }

    template <typename T, typename Allocator>
    inline typename deque<T, Allocator>::allocator_type
    deque<T, Allocator>::get_allocator() const noexcept
    {
        return m_allocator;
    }

    template <typename T, typename Allocator>
    inline typename deque<T, Allocator>::reference
    deque<T, Allocator>::at(size_type pos)
    {
        size_type slot = pos / m_map_size;
        if (slot >= m_map_size)
            throw std::out_of_range("deque::at: pos out of range");

        size_type offset = pos % m_map_size;
        if (offset >= m_buffer_size)
            throw std::out_of_range("deque::at: pos out of range");

        return m_map[slot][offset];
    }

    template <typename T, typename Allocator>
    inline typename deque<T, Allocator>::const_reference
    deque<T, Allocator>::at(size_type pos) const
    {
        size_type slot = pos / m_map_size;
        if (slot >= m_map_size)
            throw std::out_of_range("deque::at: pos out of range");

        size_type offset = pos % m_map_size;
        if (offset >= m_buffer_size)
            throw std::out_of_range("deque::at: pos out of range");

        return m_map[slot][offset];
    }

    template <typename T, typename Allocator>
    inline typename deque<T, Allocator>::reference
    deque<T, Allocator>::operator[](size_type pos)
    {
        size_type slot = pos / m_map_size;
        size_type offset = pos % m_map_size;

        return m_map[slot][offset];
    }
    template <typename T, typename Allocator>
    inline typename deque<T, Allocator>::const_reference
    deque<T, Allocator>::operator[](size_type pos) const
    {
        size_type slot = pos / m_map_size;
        size_type offset = pos % m_map_size;

        return m_map[slot][offset];
    }
    template <typename T, typename Allocator>
    inline typename deque<T, Allocator>::reference
    deque<T, Allocator>::front()
    {
        if (empty())
            throw std::out_of_range("deque::front: deque is empty");
        return *m_begin.m_cur;
    }
    template <typename T, typename Allocator>
    inline typename deque<T, Allocator>::const_reference
    deque<T, Allocator>::front() const
    {
        if (empty())
            throw std::out_of_range("deque::front: deque is empty");
        return *m_begin.m_cur;
    }
    template <typename T, typename Allocator>
    inline typename deque<T, Allocator>::reference
    deque<T, Allocator>::back()
    {
        if (empty())
            throw std::out_of_range("deque::back: deque is empty");
        return *(m_end.m_cur - 1);
    }
    template <typename T, typename Allocator>
    inline typename deque<T, Allocator>::const_reference
    deque<T, Allocator>::back() const
    {
        if (empty())
            throw std::out_of_range("deque::back: deque is empty");
        return *(m_end.m_cur - 1);
    }

    template <typename T, typename Allocator>
    inline typename deque<T, Allocator>::iterator deque<T, Allocator>::begin()
    {
        return m_begin;
    }

    template <typename T, typename Allocator>
    inline typename deque<T, Allocator>::const_iterator deque<T, Allocator>::begin() const
    {
        return m_begin;
    }

    template <typename T, typename Allocator>
    inline typename deque<T, Allocator>::const_iterator deque<T, Allocator>::cbegin() const
    {
        return m_begin;
    }
    template <typename T, typename Allocator>
    inline typename deque<T, Allocator>::iterator deque<T, Allocator>::end()
    {
        return m_end;
    }

    template <typename T, typename Allocator>
    inline typename deque<T, Allocator>::const_iterator deque<T, Allocator>::end() const
    {
        return m_end;
    }

    template <typename T, typename Allocator>
    inline typename deque<T, Allocator>::const_iterator deque<T, Allocator>::cend() const
    {
        return m_end;
    }
    template <typename T, typename Allocator>
    inline typename deque<T, Allocator>::reverse_iterator deque<T, Allocator>::rbegin()
    {
        return reverse_iterator(m_end);
    }

    template <typename T, typename Allocator>
    inline typename deque<T, Allocator>::const_reverse_iterator deque<T, Allocator>::rbegin() const
    {
        return const_reverse_iterator(m_end);
    }

    template <typename T, typename Allocator>
    inline typename deque<T, Allocator>::const_reverse_iterator deque<T, Allocator>::crbegin() const
    {
        return const_reverse_iterator(m_end);
    }

    template <typename T, typename Allocator>
    inline typename deque<T, Allocator>::reverse_iterator deque<T, Allocator>::rend()
    {
        return reverse_iterator(m_begin);
    }

    template <typename T, typename Allocator>
    inline typename deque<T, Allocator>::const_reverse_iterator deque<T, Allocator>::rend() const
    {
        return const_reverse_iterator(m_begin);
    }

    template <typename T, typename Allocator>
    inline typename deque<T, Allocator>::const_reverse_iterator deque<T, Allocator>::crend() const
    {
        return const_reverse_iterator(m_begin);
    }

    template <typename T, typename Allocator>
    inline bool deque<T, Allocator>::empty() const
    {
        return m_begin.m_cur == nullptr;
    }
    template <typename T, typename Allocator>
    inline typename deque<T, Allocator>::size_type deque<T, Allocator>::size() const
    {
        return static_cast<size_type>(m_end.m_cur - m_begin.m_cur);
    }

    template <typename T, typename Allocator>
    inline typename deque<T, Allocator>::size_type deque<T, Allocator>::max_size() const
    {
        return alloc_traits::max_size(m_allocator);
    }

    template <typename T, typename Allocator>
    inline void deque<T, Allocator>::resize(size_type count)
    {
        size_type current_size = size();
        if (count == current_size)
            return;
        else if (count < current_size)
        {
            while (current_size > count)
            {
                current_size--;
                pop_back();
            }
        }
        else
        {
            while (current_size < count)
            {
                emplace_back();
                current_size++;
            }
        }
    }

    template <typename T, typename Allocator>
    inline void deque<T, Allocator>::resize(size_type count, const value_type &value)
    {
        size_type current_size = size();
        if (count == current_size)
            return;
        else if (count < current_size)
        {
            while (current_size > count)
            {
                current_size--;
                pop_back();
            }
        }
        else
        {
            while (current_size < count)
            {
                emplace_back(value);
                current_size++;
            }
        }
    }

    template <typename T, typename Allocator>
    inline void deque<T, Allocator>::shrink_to_fit()
    {
        size_type used_slots = m_end.m_map_node - m_begin.m_map_node + 1;
        if (used_slots == m_map_size || used_slots <= DEQUE_DEFAULT_INIT_SIZE)
            return;

        pointer *new_map = map_alloc_traits::allocate(m_map_allocator, used_slots);
        for (size_type i = 0; i < used_slots; i++)
            new_map[i] = nullptr;

        size_type old_start_idx = m_begin.m_map_node - m_map;

        for (size_type i = 0; i < used_slots; ++i)
        {
            new_map[i] = m_map[old_start_idx + i];
        }

        for (size_type i = 0; i < m_map_size; ++i)
        {
            // 检查该槽位是否在使用中
            bool in_use = (i >= old_start_idx) && (i < old_start_idx + used_slots);
            if (!in_use && m_map[i] != nullptr)
                alloc_traits::deallocate(m_allocator, m_map[i], m_buffer_size);
        }

        map_alloc_traits::deallocate(m_map_allocator, m_map, m_map_size);

        m_map = new_map;
        m_map_size = used_slots;
        m_begin.m_map_node = m_map;
        m_end.m_map_node = m_map + used_slots - 1;
    }

    template <typename T, typename Allocator>
    inline void deque<T, Allocator>::clear() noexcept
    {
        // 清空所有元素 并重置迭代器指向中间槽位
        if (empty())
            return;

        for (iterator it = m_begin; it != m_end; it++)
            alloc_traits::destroy(m_allocator, it.m_cur);

        m_begin.m_cur = nullptr;
        m_begin.m_first = m_map + m_map_size / 2;
        m_begin.m_last = m_map + m_map_size / 2 + m_buffer_size;
        m_begin.m_map_node = m_map + m_map_size / 2;
        m_end = m_begin;
    }

    template <typename T, typename Allocator>
    inline typename deque<T, Allocator>::iterator
    deque<T, Allocator>::insert(const_iterator pos, const value_type &value)
    {
        // 如果是空容器，直接在头部插入
        if (empty())
        {
            push_front(value);
            return begin();
        }

        // 计算插入位置距离两端的距离
        difference_type distance_from_begin = pos - cbegin();
        difference_type distance_from_end = size() - distance_from_begin;

        // 选择移动代价较小的方向
        if (distance_from_begin <= distance_from_end)
        {
            // 从头部插入更优：将头部元素向后移动
            push_front(value);

            // 将插入位置之前的元素向后移动
            iterator first = begin();
            iterator last = begin() + distance_from_begin;

            while (first != last)
            {
                std::swap(*first, *(first + 1));
                ++first;
            }
        }
        else
        {
            // 从尾部插入更优：将尾部元素向前移动
            push_back(value);

            // 将插入位置之后的元素向前移动
            iterator last = end();
            iterator first = end() - distance_from_end;

            while (last != first)
            {
                --last;
                std::swap(*last, *(last - 1));
            }
        }

        // 返回指向插入元素的迭代器
        return begin() + distance_from_begin;
    }

    template <typename T, typename Allocator>
    inline typename deque<T, Allocator>::iterator
    deque<T, Allocator>::insert(const_iterator pos, const value_type &&value)
    {
        // 如果是空容器，直接在头部插入
        if (empty())
        {
            push_front(value);
            return begin();
        }

        // 计算插入位置距离两端的距离
        difference_type distance_from_begin = pos - cbegin();
        difference_type distance_from_end = size() - distance_from_begin;

        // 选择移动代价较小的方向
        if (distance_from_begin <= distance_from_end)
        {
            // 从头部插入更优：将头部元素向后移动
            push_front(value);

            // 将插入位置之前的元素向后移动
            iterator first = begin();
            iterator last = begin() + distance_from_begin;

            while (first != last)
            {
                std::swap(*first, *(first + 1));
                ++first;
            }
        }
        else
        {
            // 从尾部插入更优：将尾部元素向前移动
            push_back(std::move(value));

            // 将插入位置之后的元素向前移动
            iterator last = end();
            iterator first = end() - distance_from_end;

            while (last != first)
            {
                --last;
                std::swap(*last, *(last - 1));
            }
        }

        // 返回指向插入元素的迭代器
        return begin() + distance_from_begin;
    }

    template <typename T, typename Allocator>
    inline typename deque<T, Allocator>::iterator
    deque<T, Allocator>::insert(const_iterator pos, size_type count, const value_type &value)
    {
        // 处理 count = 0 的情况
        if (count == 0)
        {
            return iterator(const_cast<pointer>(pos.m_cur),
                            const_cast<pointer>(pos.m_first),
                            const_cast<pointer>(pos.m_last),
                            const_cast<value_type **>(pos.m_map_node));
        }

        // 如果是空容器，直接填充
        if (empty())
        {
            resize(count, value);
            return begin();
        }

        // 计算插入位置
        difference_type insert_pos = pos - cbegin();
        difference_type old_size = size();

        // 计算需要的新槽位数
        size_type new_slots = count / m_buffer_size;
        if (count % m_buffer_size != 0)
            new_slots++;

        // 计算当前使用的槽位数
        size_type used_slots = m_end.m_map_node - m_begin.m_map_node + 1;

        // 检查是否需要扩容 map
        size_type required_map_size = used_slots + new_slots;
        if (required_map_size > m_map_size)
        {
            // 扩展 map 大小（至少翻倍）
            size_type new_map_size = std::max(m_map_size * 2, required_map_size);
            value_type **new_map = map_alloc_traits::allocate(m_map_allocator, new_map_size);

            // 初始化新 map
            for (size_type i = 0; i < new_map_size; ++i)
                new_map[i] = nullptr;

            // 计算新的起始位置（保持居中）
            size_type old_start_idx = m_begin.m_map_node - m_map;
            size_type new_start_idx = (new_map_size - required_map_size) / 2;

            // 拷贝旧槽位到新位置
            for (size_type i = 0; i < used_slots; ++i)
                new_map[new_start_idx + i] = m_map[old_start_idx + i];

            // 释放旧 map
            map_alloc_traits::deallocate(m_map_allocator, m_map, m_map_size);

            // 更新指针
            m_map = new_map;
            m_map_size = new_map_size;
            m_begin.m_map_node = &m_map[new_start_idx];
            m_end.m_map_node = &m_map[new_start_idx + used_slots - 1];
        }

        // 计算插入位置距离两端的距离
        difference_type distance_from_begin = insert_pos;
        difference_type distance_from_end = old_size - insert_pos;

        // 选择移动代价较小的方向
        if (distance_from_begin <= distance_from_end)
        {
            // 从头部扩展
            // 在头部添加新槽位
            for (size_type i = 0; i < new_slots; ++i)
            {
                m_begin.m_map_node--;
                *m_begin.m_map_node = alloc_traits::allocate(m_allocator, m_buffer_size);
            }

            // 移动头部元素
            iterator src = begin();
            iterator dest = begin() + count;
            for (difference_type i = distance_from_begin - 1; i >= 0; --i)
            {
                *dest = *src;
                --dest;
                --src;
            }

            // 填充新元素
            iterator it = begin();
            for (size_type i = 0; i < count; ++i)
            {
                alloc_traits::construct(m_allocator, it.m_cur, value);
                ++it;
            }
        }
        else
        {
            // 从尾部扩展
            // 在尾部添加新槽位
            iterator old_end = end();
            for (size_type i = 0; i < new_slots; ++i)
            {
                m_end.m_map_node++;
                *m_end.m_map_node = alloc_traits::allocate(m_allocator, m_buffer_size);
            }

            // 移动尾部元素
            iterator src = old_end - 1;
            iterator dest = end() - 1;
            for (difference_type i = distance_from_end - 1; i >= 0; --i)
            {
                *dest = *src;
                --dest;
                --src;
            }

            // 填充新元素
            iterator it = begin() + insert_pos;
            for (size_type i = 0; i < count; ++i)
            {
                alloc_traits::construct(m_allocator, it.m_cur, value);
                ++it;
            }
        }

        // 更新 m_end
        m_end.m_cur += count;
        if (m_end.m_cur >= m_end.m_last)
        {
            m_end.m_map_node++;
            m_end.m_first = *m_end.m_map_node;
            m_end.m_last = m_end.m_first + m_buffer_size;
            m_end.m_cur = m_end.m_first + (m_end.m_cur - (m_end.m_last - m_buffer_size));
        }

        // 返回指向第一个插入元素的迭代器
        return begin() + insert_pos;
    }

    template <typename T, typename Allocator>
    template <typename InputIt,
              std::enable_if_t<
                  !std::is_integral<InputIt>::value, int>>
    inline typename deque<T, Allocator>::iterator
    deque<T, Allocator>::insert(const_iterator pos, InputIt first, InputIt last)
    {
        // 处理 count = 0 的情况
        if (count == 0)
        {
            return iterator(const_cast<pointer>(pos.m_cur),
                            const_cast<pointer>(pos.m_first),
                            const_cast<pointer>(pos.m_last),
                            const_cast<value_type **>(pos.m_map_node));
        }

        // 如果是空容器，直接填充
        if (empty())
        {
            resize(count, value);
            return begin();
        }

        // 计算插入位置
        difference_type insert_pos = pos - cbegin();
        difference_type old_size = size();

        // 计算需要的新槽位数
        size_type new_slots = count / m_buffer_size;
        if (count % m_buffer_size != 0)
            new_slots++;

        // 计算当前使用的槽位数
        size_type used_slots = m_end.m_map_node - m_begin.m_map_node + 1;

        // 检查是否需要扩容 map
        size_type required_map_size = used_slots + new_slots;
        if (required_map_size > m_map_size)
        {
            // 扩展 map 大小（至少翻倍）
            size_type new_map_size = std::max(m_map_size * 2, required_map_size);
            value_type **new_map = map_alloc_traits::allocate(m_map_allocator, new_map_size);

            // 初始化新 map
            for (size_type i = 0; i < new_map_size; ++i)
                new_map[i] = nullptr;

            // 计算新的起始位置（保持居中）
            size_type old_start_idx = m_begin.m_map_node - m_map;
            size_type new_start_idx = (new_map_size - required_map_size) / 2;

            // 拷贝旧槽位到新位置
            for (size_type i = 0; i < used_slots; ++i)
                new_map[new_start_idx + i] = m_map[old_start_idx + i];

            // 释放旧 map
            map_alloc_traits::deallocate(m_map_allocator, m_map, m_map_size);

            // 更新指针
            m_map = new_map;
            m_map_size = new_map_size;
            m_begin.m_map_node = &m_map[new_start_idx];
            m_end.m_map_node = &m_map[new_start_idx + used_slots - 1];
        }

        // 计算插入位置距离两端的距离
        difference_type distance_from_begin = insert_pos;
        difference_type distance_from_end = old_size - insert_pos;

        // 选择移动代价较小的方向
        if (distance_from_begin <= distance_from_end)
        {
            // 从头部扩展
            // 在头部添加新槽位
            for (size_type i = 0; i < new_slots; ++i)
            {
                m_begin.m_map_node--;
                *m_begin.m_map_node = alloc_traits::allocate(m_allocator, m_buffer_size);
            }

            // 移动头部元素
            iterator src = begin();
            iterator dest = begin() + count;
            for (difference_type i = distance_from_begin - 1; i >= 0; --i)
            {
                *dest = *src;
                --dest;
                --src;
            }

            // 填充新元素
            iterator it = begin();
            for (size_type i = 0; i < count; ++i)
            {
                alloc_traits::construct(m_allocator, it.m_cur, value);
                ++it;
            }
        }
        else
        {
            // 从尾部扩展
            // 在尾部添加新槽位
            iterator old_end = end();
            for (size_type i = 0; i < new_slots; ++i)
            {
                m_end.m_map_node++;
                *m_end.m_map_node = alloc_traits::allocate(m_allocator, m_buffer_size);
            }

            // 移动尾部元素
            iterator src = old_end - 1;
            iterator dest = end() - 1;
            for (difference_type i = distance_from_end - 1; i >= 0; --i)
            {
                *dest = *src;
                --dest;
                --src;
            }

            // 填充新元素
            iterator it = begin() + insert_pos;
            for (InputIt i = first; i != last; ++i)
            {
                alloc_traits::construct(m_allocator, it.m_cur, *i);
                ++it;
            }
        }

        // 更新 m_end
        m_end.m_cur += count;
        if (m_end.m_cur >= m_end.m_last)
        {
            m_end.m_map_node++;
            m_end.m_first = *m_end.m_map_node;
            m_end.m_last = m_end.m_first + m_buffer_size;
            m_end.m_cur = m_end.m_first + (m_end.m_cur - (m_end.m_last - m_buffer_size));
        }

        // 返回指向第一个插入元素的迭代器
        return begin() + insert_pos;
    }

    template <typename T, typename Allocator>
    inline typename deque<T, Allocator>::iterator
    deque<T, Allocator>::insert(const_iterator pos, std::initializer_list<T> ilist)
    {
        return insert(pos, ilist.begin(), ilist.end());
    }

    template <typename T, typename Allocator>
    template <typename... Args>
    inline typename deque<T, Allocator>::reference
    deque<T, Allocator>::emplace(const_iterator pos, Args &&...args)
    {
        // 如果是空容器，直接在头部插入
        if (empty())
        {
            push_front(value);
            return begin();
        }

        // 计算插入位置距离两端的距离
        difference_type distance_from_begin = pos - cbegin();
        difference_type distance_from_end = size() - distance_from_begin;

        // 选择移动代价较小的方向
        if (distance_from_begin <= distance_from_end)
        {
            // 从头部插入更优：将头部元素向后移动
            emplace_front(std::forward<Args>(args)...);

            // 将插入位置之前的元素向后移动
            iterator first = begin();
            iterator last = begin() + distance_from_begin;

            while (first != last)
            {
                std::swap(*first, *(first + 1));
                ++first;
            }
        }
        else
        {
            // 从尾部插入更优：将尾部元素向前移动
            emplace_back(std::forward<Args>(args)...);

            // 将插入位置之后的元素向前移动
            iterator last = end();
            iterator first = end() - distance_from_end;

            while (last != first)
            {
                --last;
                std::swap(*last, *(last - 1));
            }
        }

        // 返回指向插入元素的迭代器
        return begin() + distance_from_begin;
    }

    template <typename T, typename Allocator>
    inline typename deque<T, Allocator>::iterator
    deque<T, Allocator>::erase(const_iterator pos)
    {
        if (empty())
            return end();

        // 获取删除位置
        difference_type erase_pos = pos - cbegin();

        // 如果删除的是最后一个元素
        if (pos == cend() - 1)
        {
            pop_back();
            return end();
        }

        // 如果删除的是第一个元素
        if (pos == cbegin())
        {
            pop_front();
            return begin();
        }

        // 计算距离两端的距离
        difference_type distance_from_begin = erase_pos;
        difference_type distance_from_end = size() - erase_pos - 1;

        iterator result(begin() + erase_pos);

        if (distance_from_begin <= distance_from_end)
        {
            // 向前移动元素
            iterator it = begin() + erase_pos;
            iterator prev_it = it - 1;
            while (it != begin())
            {
                *it = std::move(*prev_it);
                --it;
                --prev_it;
            }
            pop_front();
        }
        else
        {
            // 向后移动元素
            iterator it = begin() + erase_pos;
            iterator next_it = it + 1;
            while (next_it != end())
            {
                *it = std::move(*next_it);
                ++it;
                ++next_it;
            }
            pop_back();
        }

        return result;
    }

    template <typename T, typename Allocator>
    inline typename deque<T, Allocator>::iterator
    deque<T, Allocator>::erase(const_iterator first, const_iterator last)
    {
        if (first == last)
        {
            return iterator(const_cast<pointer>(first.m_cur),
                            const_cast<pointer>(first.m_first),
                            const_cast<pointer>(first.m_last),
                            const_cast<value_type **>(first.m_map_node));
        }

        // 如果删除全部元素
        if (first == cbegin() && last == cend())
        {
            clear();
            return end();
        }

        // 计算删除范围
        difference_type erase_begin = first - cbegin();
        difference_type erase_size = last - first;
        difference_type erase_end = erase_begin + erase_size;

        // 计算距离两端的距离
        difference_type distance_from_begin = erase_begin;
        difference_type distance_from_end = size() - erase_end;

        iterator result(begin() + erase_begin);

        if (distance_from_begin <= distance_from_end)
        {
            // 将前面的元素向后移动覆盖删除区域
            iterator dest = begin() + erase_end;
            iterator src = begin() + erase_begin;

            for (difference_type i = 0; i < distance_from_begin; ++i)
            {
                --dest;
                --src;
                *dest = std::move(*src);
            }

            // 从头部删除
            for (difference_type i = 0; i < erase_size; ++i)
                pop_front();
        }
        else
        {
            // 将后面的元素向前移动覆盖删除区域
            iterator dest = begin() + erase_begin;
            iterator src = begin() + erase_end;

            for (difference_type i = 0; i < distance_from_end; ++i)
            {
                *dest = std::move(*src);
                ++dest;
                ++src;
            }

            // 从尾部删除
            for (difference_type i = 0; i < erase_size; ++i)
                pop_back();
        }

        return result;
    }

    template <typename T, typename Allocator>
    inline void deque<T, Allocator>::push_front(const value_type &value)
    {
        // 当前buffer有空间
        if (m_begin.m_cur > m_begin.m_first)
        {
            alloc_traits::construct(m_allocator, m_begin.m_cur - 1, value);
            m_begin.m_cur--;
        }
        else
        {
            value_type **new_slot = m_begin.m_map_node - 1;

            // 需要扩展槽位
            if (new_slot < m_map)
            {
                new_map_size = m_map_size * 2;
                value_type **new_map = map_alloc_traits::allocate(m_map_allocator, new_map_size);
                size_type first_slot = (new_map_size - m_map_size) / 2;
                for (size_type i = 0; i < m_map_size; i++)
                    new_map[first_slot++] = m_map[i];

                m_map_allocator::deallocate(m_map_allocator, m_map, m_map_size);
                m_map = new_map;
                m_map_size = new_map_size;
                m_end.m_map_node = m_map + m_end.m_map_node - m_begin.m_map_node;
                m_begin.m_map_node = (new_map_size - m_map_size) / 2;
                new_slot = m_end.m_map_node + 1;
            }

            // 新槽位为空，需要分配内存
            if (new_slot == nullptr)
                new_slot = map_alloc_traits::allocate(m_map_allocator, m_buffer_size);

            alloc_traits::construct(m_allocator, *new_slot + m_buffer_size - 1, value);
            m_begin.m_cur = *new_slot + m_buffer_size - 1;
            m_begin.m_first = *new_slot;
            m_begin.m_last = *new_slot + m_buffer_size;
            m_begin.m_map_node = new_slot;
        }
    }

    template <typename T, typename Allocator>
    inline void deque<T, Allocator>::push_front(const value_type &&value)
    {
        // 当前buffer有空间
        if (m_begin.m_cur > m_begin.m_first)
        {
            alloc_traits::construct(m_allocator, m_begin.m_cur - 1, std::move(value));
            m_begin.m_cur--;
        }
        else
        {
            value_type **new_slot = m_begin.m_map_node - 1;

            // 需要扩展槽位
            if (new_slot < m_map)
            {
                new_map_size = m_map_size * 2;
                value_type **new_map = map_alloc_traits::allocate(m_map_allocator, new_map_size);
                size_type first_slot = (new_map_size - m_map_size) / 2;
                for (size_type i = 0; i < m_map_size; i++)
                    new_map[first_slot++] = m_map[i];

                m_map_allocator::deallocate(m_map_allocator, m_map, m_map_size);
                m_map = new_map;
                m_map_size = new_map_size;
                m_end.m_map_node = m_map + m_end.m_map_node - m_begin.m_map_node;
                m_begin.m_map_node = (new_map_size - m_map_size) / 2;
                new_slot = m_end.m_map_node + 1;
            }

            // 新槽位为空，需要分配内存
            if (new_slot == nullptr)
                new_slot = map_alloc_traits::allocate(m_map_allocator, m_buffer_size);

            alloc_traits::construct(m_allocator, *new_slot + m_buffer_size - 1, std::move(value));
            m_begin.m_cur = *new_slot + m_buffer_size - 1;
            m_begin.m_first = *new_slot;
            m_begin.m_last = *new_slot + m_buffer_size;
            m_begin.m_map_node = new_slot;
        }
    }

    template <typename T, typename Allocator>
    inline void deque<T, Allocator>::push_back(const value_type &value)
    {
        if (m_end.m_cur < m_end.m_last)
        {
            alloc_traits::construct(m_allocator, m_end.m_cur, value);
            m_end.m_cur++;
        }
        else
        {
            value_type **new_slot = m_end.m_map_node + 1;

            // 需要扩展槽位
            if (new_slot >= m_map + m_map_size)
            {
                new_map_size = m_map_size * 2;
                value_type **new_map = map_alloc_traits::allocate(m_map_allocator, new_map_size);
                size_type first_slot = (new_map_size - m_map_size) / 2;
                for (size_type i = 0; i < m_map_size; i++)
                    new_map[first_slot++] = m_map[i];

                map_alloc_traits::deallocate(m_map_allocator, m_map, m_map_size);
                m_map = new_map;
                m_map_size = new_map_size;
                m_end.m_map_node = m_map + m_end.m_map_node - m_begin.m_map_node;
                m_begin.m_map_node = (new_map_size - m_map_size) / 2;
                new_slot = m_end.m_map_node + 1;
            }

            // 新槽位为空，需要分配内存
            if (new_slot == nullptr)
                new_slot = map_alloc_traits::allocate(m_map_allocator, m_buffer_size);

            alloc_traits::construct(m_allocator, *new_slot, value);
            m_end.m_cur = *new_slot;
            m_end.m_first = *new_slot;
            m_end.m_last = *new_slot + m_buffer_size;
            m_end.m_map_node = new_slot;
        }
    }
    template <typename T, typename Allocator>
    inline void deque<T, Allocator>::push_back(const value_type &&value)
    {
        if (m_end.m_cur < m_end.m_last)
        {
            alloc_traits::construct(m_allocator, m_end.m_cur, std::move(value));
            m_end.m_cur++;
        }
        else
        {
            value_type **new_slot = m_end.m_map_node + 1;

            // 需要扩展槽位
            if (new_slot >= m_map + m_map_size)
            {
                new_map_size = m_map_size * 2;
                value_type **new_map = map_alloc_traits::allocate(m_map_allocator, new_map_size);
                size_type first_slot = (new_map_size - m_map_size) / 2;
                for (size_type i = 0; i < m_map_size; i++)
                    new_map[first_slot++] = m_map[i];

                map_alloc_traits::deallocate(m_map_allocator, m_map, m_map_size);
                m_map = new_map;
                m_map_size = new_map_size;
                m_end.m_map_node = m_map + m_end.m_map_node - m_begin.m_map_node;
                m_begin.m_map_node = (new_map_size - m_map_size) / 2;
                new_slot = m_end.m_map_node + 1;
            }

            // 新槽位为空，需要分配内存
            if (new_slot == nullptr)
                new_slot = map_alloc_traits::allocate(m_map_allocator, m_buffer_size);

            alloc_traits::construct(m_allocator, *new_slot, std::move(value));
            m_end.m_cur = *new_slot;
            m_end.m_first = *new_slot;
            m_end.m_last = *new_slot + m_buffer_size;
            m_end.m_map_node = new_slot;
        }
    }
    template <typename T, typename Allocator>
    template <typename... Args>
    inline typename deque<T, Allocator>::reference
    deque<T, Allocator>::emplace_back(Args &&...args)
    {
        if (m_end.m_cur < m_end.m_last)
        {
            alloc_traits::construct(m_allocator, m_end.m_cur, std::forward<Args>(args)...);
            m_end.m_cur++;
        }
        else
        {
            value_type **new_slot = m_end.m_map_node + 1;

            // 需要扩展槽位
            if (new_slot >= m_map + m_map_size)
            {
                new_map_size = m_map_size * 2;
                value_type **new_map = map_alloc_traits::allocate(m_map_allocator, new_map_size);
                size_type first_slot = (new_map_size - m_map_size) / 2;
                for (size_type i = 0; i < m_map_size; i++)
                    new_map[first_slot++] = m_map[i];

                map_alloc_traits::deallocate(m_map_allocator, m_map, m_map_size);
                m_map = new_map;
                m_map_size = new_map_size;
                m_end.m_map_node = m_map + m_end.m_map_node - m_begin.m_map_node;
                m_begin.m_map_node = (new_map_size - m_map_size) / 2;
                new_slot = m_end.m_map_node + 1;
            }

            // 新槽位为空，需要分配内存
            if (new_slot == nullptr)
                new_slot = map_alloc_traits::allocate(m_map_allocator, m_buffer_size);

            alloc_traits::construct(m_allocator, *new_slot, std::forward<Args>(args)...);
            m_end.m_cur = *new_slot;
            m_end.m_first = *new_slot;
            m_end.m_last = *new_slot + m_buffer_size;
            m_end.m_map_node = new_slot;
        }
    }
    template <typename T, typename Allocator>
    template <typename... Args>
    inline typename deque<T, Allocator>::reference
    deque<T, Allocator>::emplace_front(Args &&...args)
    {
        // 当前buffer有空间
        if (m_being.m_cur > m_begin.m_first)
        {
            alloc_traits::construct(m_allocator, m_begin.m_cur - 1, std::forward<Args>(args)...);
            m_begin.m_cur--;
        }
        else
        {
            value_type **new_slot = m_begin.m_map_node - 1;

            // 需要扩展槽位
            if (new_slot < m_map)
            {
                new_map_size = m_map_size * 2;
                value_type **new_map = map_alloc_traits::allocate(m_map_allocator, new_map_size);
                size_type first_slot = (new_map_size - m_map_size) / 2;
                for (size_type i = 0; i < m_map_size; i++)
                    new_map[first_slot++] = m_map[i];

                m_map_allocator::deallocate(m_map_allocator, m_map, m_map_size);
                m_map = new_map;
                m_map_size = new_map_size;
                m_end.m_map_node = m_map + m_end.m_map_node - m_begin.m_map_node;
                m_begin.m_map_node = (new_map_size - m_map_size) / 2;
                new_slot = m_end.m_map_node + 1;
            }

            // 新槽位为空，需要分配内存
            if (new_slot == nullptr)
                new_slot = map_alloc_traits::allocate(m_map_allocator, m_buffer_size);

            alloc_traits::construct(m_allocator, *new_slot + m_buffer_size - 1, std::forward<Args>(args)...);
            m_begin.m_cur = *new_slot + m_buffer_size - 1;
            m_begin.m_first = *new_slot;
            m_begin.m_last = *new_slot + m_buffer_size;
            m_begin.m_map_node = new_slot;
        }
    }

    template <typename T, typename Allocator>
    inline void deque<T, Allocator>::pop_front()
    {
        if (empty())
            throw std::out_of_range("deque::pop_front: deque is empty");

        alloc_traits::destroy(m_allocator, m_begin.m_cur);
        m_begin.m_cur++;
    }

    template <typename T, typename Allocator>
    inline void deque<T, Allocator>::pop_back()
    {
        if (empty())
            throw std::out_of_range("deque::pop_back: deque is empty");

        alloc_traits::destroy(m_allocator, --m_end.m_cur);
    }

    template <typename T, typename Allocator>
    inline void deque<T, Allocator>::swap(deque &other) noexcept
    {
        using std::swap;
        swap(m_begin, other.m_begin);
        swap(m_end, other.m_end);
        swap(m_map, other.m_map);
        swap(m_map_size, other.m_map_size);
    }

    template <typename T, typename Allocator>
    bool operator==(const deque<T, Allocator> &lhs, const deque<T, Allocator> &rhs)
    {
        if (lhs.size() != rhs.size())
            return false;

        iterator lhs_it = lhs.begin();
        iterator rhs_it = rhs.begin();
        while (lhs_it != lhs.end() && rhs_it != rhs.end())
        {
            if (*lhs_it != *rhs_it)
                return false;
            ++lhs_it;
            ++rhs_it;
        }
        return true;
    }

    template <typename T, typename Allocator>
    bool operator!=(const deque<T, Allocator> &lhs, const deque<T, Allocator> &rhs)
    {
        return !(lhs == rhs);
    }
}
