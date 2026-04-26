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

        private:
            pointer m_ptr;           // 当前元素指针
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

        if(first == last)
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

    template<typename T, typename Allocator>
    inline typename deque<T, Allocator>::reference
    deque<T, Allocator>::at(size_type pos)
    {
        size_type slot = pos / m_map_size;
        if(slot >= m_map_size)
            throw std::out_of_range("deque::at: pos out of range");

        size_type offset = pos % m_map_size;
        if(offset >= m_buffer_size)
            throw std::out_of_range("deque::at: pos out of range");

        return m_map[slot][offset];
    }

    template<typename T, typename Allocator>
    inline typename deque<T, Allocator>::const_reference
    deque<T, Allocator>::at(size_type pos) const
    {
        size_type slot = pos / m_map_size;
        if(slot >= m_map_size)
            throw std::out_of_range("deque::at: pos out of range");

        size_type offset = pos % m_map_size;
        if(offset >= m_buffer_size)
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
        if(empty())
            throw std::out_of_range("deque::front: deque is empty");
        return *m_begin.m_cur;
    }
    template <typename T, typename Allocator>
    inline typename deque<T, Allocator>::const_reference
    deque<T, Allocator>::front() const
    {
        if(empty())
            throw std::out_of_range("deque::front: deque is empty");
        return *m_begin.m_cur;
    }
    template <typename T, typename Allocator>
    inline typename deque<T, Allocator>::reference
    deque<T, Allocator>::back()
    {
        if(empty())
            throw std::out_of_range("deque::back: deque is empty");
        return *(m_end.m_cur-1);
    }
    template <typename T, typename Allocator>
    inline typename deque<T, Allocator>::const_reference
    deque<T, Allocator>::back() const
    {
        if(empty())
            throw std::out_of_range("deque::back: deque is empty");
        return *(m_end.m_cur-1);
    }
}