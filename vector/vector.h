#pragma once

#include <utility>
#include <iterator>
#include <memory>
#include <type_traits>
#include <exception>
#include <initializer_list>

namespace demo
{
    template <typename T, typename Allocator = std::allocator<T>>
    class vector
    {
    public:
        using value_type = T;
        using pointer = T *;
        using const_pointer = const T *;
        using reference = value_type &;
        using const_reference = const value_type &;
        using allocator_type = Allocator;
        using size_type = std::size_t;
        using difference_type = std::ptrdiff_t;

        class const_iterator;
        class iterator
        {
        public:
            using iterator_category = std::random_access_iterator_tag;
            using value_type = typename vector::value_type;
            using difference_type = typename vector::difference_type;
            using pointer = typename vector::pointer;
            using reference = typename vector::reference;

            iterator() noexcept;
            explicit iterator(pointer ptr) noexcept;

            // 解引用操作
            reference operator*() const noexcept;
            pointer operator->() const noexcept;
            reference operator[](difference_type offset) const;

            // 递增/递减
            iterator &operator++() noexcept;
            iterator operator++(int) noexcept;
            iterator &operator--() noexcept;
            iterator operator--(int) noexcept;

            // 算术运算
            iterator operator+(difference_type offset) const noexcept;
            iterator operator-(difference_type offset) const noexcept;
            iterator &operator+=(difference_type offset) noexcept;
            iterator &operator-=(difference_type offset) noexcept;

            // 差值
            difference_type operator-(const iterator &other) const noexcept;

            // 比较运算符
            bool operator==(const iterator &other) const noexcept;
            bool operator!=(const iterator &other) const noexcept;
            bool operator>(const iterator &other) const noexcept;
            bool operator<(const iterator &other) const noexcept;
            bool operator>=(const iterator &other) const noexcept;
            bool operator<=(const iterator &other) const noexcept;

            // 获取底层指针（供reverse_iterator使用）
            pointer base() const noexcept;

            friend class const_iterator;

        private:
            pointer m_ptr;
        };

        class const_iterator
        {
        public:
            using iterator_category = std::random_access_iterator_tag;
            using value_type = typename vector::value_type;
            using reference = const typename vector::value_type &;
            using pointer = const typename vector::value_type *;
            using difference_type = typename vector::difference_type;

            const_iterator() noexcept;
            explicit const_iterator(typename vector::pointer ptr) noexcept;
            const_iterator(const iterator &other) noexcept;

            // 解引用操作
            reference operator*() const noexcept;
            pointer operator->() const noexcept;
            reference operator[](difference_type offset) const;

            // 递增/递减
            const_iterator &operator++() noexcept;
            const_iterator operator++(int) noexcept;
            const_iterator &operator--() noexcept;
            const_iterator operator--(int) noexcept;

            // 算术运算
            const_iterator operator+(difference_type offset) const noexcept;
            const_iterator operator-(difference_type offset) const noexcept;
            const_iterator &operator+=(difference_type offset) noexcept;
            const_iterator &operator-=(difference_type offset) noexcept;

            // 差值
            difference_type operator-(const const_iterator &other) const noexcept;

            // 比较运算符
            bool operator==(const const_iterator &other) const noexcept;
            bool operator!=(const const_iterator &other) const noexcept;
            bool operator<(const const_iterator &other) const noexcept;
            bool operator>(const const_iterator &other) const noexcept;
            bool operator>=(const const_iterator &other) const noexcept;
            bool operator<=(const const_iterator &other) const noexcept;

            // 与 iterator 比较
            bool operator==(const iterator &other) const noexcept;
            bool operator!=(const iterator &other) const noexcept;

            typename vector::pointer base() const noexcept;

        private:
            typename vector::pointer m_ptr;
        };

        // 使用标准库的反向迭代器适配器
        using reverse_iterator = std::reverse_iterator<iterator>;
        using const_reverse_iterator = std::reverse_iterator<const_iterator>;

        // 构造
        vector();
        explicit vector(size_type size);
        explicit vector(size_type size, const_reference val);
        template <typename InputIt>
        vector(InputIt first, InputIt last);
        vector(std::initializer_list<value_type> ilist);
        vector(const vector &other);
        vector &operator=(const vector &other);
        vector &operator=(std::initializer_list<value_type> ilist);
        vector(vector &&other) noexcept;
        vector &operator=(vector &&other) noexcept;
        ~vector();
        void assign(size_type count, const_reference val);
        template <typename InputIt>
        void assign(InputIt first, InputIt last);
        void assign(std::initializer_list<value_type> ilist);
        Allocator get_allocator() const;

        // 元素访问
        reference at(size_type index);
        const_reference at(size_type index) const;
        reference operator[](size_type index);
        const_reference operator[](size_type index) const;
        reference front();
        const_reference front() const;
        reference back();
        const_reference back() const;
        pointer data();
        const_pointer data() const;

        // 迭代器
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

        // 容量
        bool empty() const;
        size_type size() const;
        size_type max_size() const;
        void reserve(size_type new_capacity);
        size_type capacity() const;
        void shrink_to_fit();

        // 修改器
        void clear();
        iterator insert(const_iterator iter, const_reference val);
        iterator insert(const_iterator iter, value_type &&val);
        iterator insert(const_iterator iter, size_type count, const_reference val);
        template <typename InputIt>
        iterator insert(const_iterator iter, InputIt first, InputIt last);
        iterator insert(const_iterator iter, std::initializer_list<value_type> ilist);
        template <typename... Args>
        void emplace(const_iterator iter, Args &&...args);
        iterator erase(const_iterator iter);
        iterator erase(const_iterator first, const_iterator last);
        void push_back(const_reference val);
        void push_back(value_type &&val);
        template <typename... Args>
        reference emplace_back(Args &&...args);
        void pop_back();
        void resize(size_type size, value_type val = value_type());
        void swap(vector &other);

    private:
        pointer m_data;
        size_type m_size;
        size_type m_capacity;
        Allocator m_allocator;

        using alloc_traits = std::allocator_traits<allocator_type>;
    };

    template <class T, class Alloc>
    bool operator==(const vector<T, Alloc> &lhs,
                    const vector<T, Alloc> &rhs);
    template <class T, class Alloc>
    bool operator!=(const vector<T, Alloc> &lhs,
                    const vector<T, Alloc> &rhs);

    // ------------------------------ iterator 实现 ------------------------------
    template <typename T, typename Allocator>
    inline vector<T, Allocator>::iterator::iterator() noexcept
        : m_ptr(nullptr)
    {
    }

    template <typename T, typename Allocator>
    inline vector<T, Allocator>::iterator::iterator(pointer ptr) noexcept
        : m_ptr(ptr)
    {
    }

    template <typename T, typename Allocator>
    inline typename vector<T, Allocator>::reference vector<T, Allocator>::iterator::operator*() const noexcept
    {
        return *m_ptr;
    }

    template <typename T, typename Allocator>
    inline typename vector<T, Allocator>::pointer vector<T, Allocator>::iterator::operator->() const noexcept
    {
        return m_ptr;
    }

    template <typename T, typename Allocator>
    inline typename vector<T, Allocator>::reference vector<T, Allocator>::iterator::operator[](difference_type n) const
    {
        return m_ptr[n];
    }

    template <typename T, typename Allocator>
    inline typename vector<T, Allocator>::iterator &vector<T, Allocator>::iterator::operator++() noexcept
    {
        m_ptr++;
        return *this;
    }

    template <typename T, typename Allocator>
    inline typename vector<T, Allocator>::iterator vector<T, Allocator>::iterator::operator++(int) noexcept
    {
        iterator temp(m_ptr);
        m_ptr++;
        return temp;
    }

    template <typename T, typename Allocator>
    inline typename vector<T, Allocator>::iterator &vector<T, Allocator>::iterator::operator--() noexcept
    {
        m_ptr--;
        return *this;
    }

    template <typename T, typename Allocator>
    inline typename vector<T, Allocator>::iterator vector<T, Allocator>::iterator::operator--(int) noexcept
    {
        iterator temp(m_ptr);
        m_ptr--;
        return temp;
    }

    template <typename T, typename Allocator>
    inline typename vector<T, Allocator>::iterator vector<T, Allocator>::iterator::operator+(difference_type n) const noexcept
    {
        return iterator(m_ptr + n);
    }

    template <typename T, typename Allocator>
    inline typename vector<T, Allocator>::iterator vector<T, Allocator>::iterator::operator-(difference_type n) const noexcept
    {
        return iterator(m_ptr - n);
    }

    template <typename T, typename Allocator>
    inline typename vector<T, Allocator>::iterator &vector<T, Allocator>::iterator::operator+=(difference_type n) noexcept
    {
        m_ptr += n;
        return *this;
    }

    template <typename T, typename Allocator>
    inline typename vector<T, Allocator>::iterator &vector<T, Allocator>::iterator::operator-=(difference_type n) noexcept
    {
        m_ptr -= n;
        return *this;
    }

    template <typename T, typename Allocator>
    inline typename vector<T, Allocator>::difference_type vector<T, Allocator>::iterator::operator-(const iterator &other) const noexcept
    {
        return static_cast<difference_type>(m_ptr - other.m_ptr);
    }

    template <typename T, typename Allocator>
    inline bool vector<T, Allocator>::iterator::operator==(const iterator &other) const noexcept
    {
        return m_ptr == other.m_ptr;
    }

    template <typename T, typename Allocator>
    inline bool vector<T, Allocator>::iterator::operator!=(const iterator &other) const noexcept
    {
        return m_ptr != other.m_ptr;
    }

    template <typename T, typename Allocator>
    inline bool vector<T, Allocator>::iterator::operator>(const iterator &other) const noexcept
    {
        return m_ptr > other.m_ptr;
    }

    template <typename T, typename Allocator>
    inline bool vector<T, Allocator>::iterator::operator<(const iterator &other) const noexcept
    {
        return m_ptr < other.m_ptr;
    }

    template <typename T, typename Allocator>
    inline bool vector<T, Allocator>::iterator::operator>=(const iterator &other) const noexcept
    {
        return m_ptr >= other.m_ptr;
    }

    template <typename T, typename Allocator>
    inline bool vector<T, Allocator>::iterator::operator<=(const iterator &other) const noexcept
    {
        return m_ptr <= other.m_ptr;
    }

    template <typename T, typename Allocator>
    inline typename vector<T, Allocator>::pointer vector<T, Allocator>::iterator::base() const noexcept
    {
        return m_ptr;
    }

    // ------------------------------ const_iterator 实现 ------------------------------
    template <typename T, typename Allocator>
    inline vector<T, Allocator>::const_iterator::const_iterator() noexcept
        : m_ptr(nullptr)
    {
    }

    template <typename T, typename Allocator>
    inline vector<T, Allocator>::const_iterator::const_iterator(typename vector::pointer ptr) noexcept
        : m_ptr(ptr)
    {
    }

    template <typename T, typename Allocator>
    inline vector<T, Allocator>::const_iterator::const_iterator(const iterator &other) noexcept
        : m_ptr(other.m_ptr)
    {
    }

    template <typename T, typename Allocator>
    inline typename vector<T, Allocator>::const_reference vector<T, Allocator>::const_iterator::operator*() const noexcept
    {
        return *m_ptr;
    }

    template <typename T, typename Allocator>
    inline typename vector<T, Allocator>::const_pointer vector<T, Allocator>::const_iterator::operator->() const noexcept
    {
        return m_ptr;
    }

    template <typename T, typename Allocator>
    inline typename vector<T, Allocator>::const_reference vector<T, Allocator>::const_iterator::operator[](difference_type offset) const
    {
        return m_ptr[offset];
    }

    template <typename T, typename Allocator>
    inline typename vector<T, Allocator>::const_iterator &vector<T, Allocator>::const_iterator::operator++() noexcept
    {
        m_ptr++;
        return *this;
    }

    template <typename T, typename Allocator>
    inline typename vector<T, Allocator>::const_iterator vector<T, Allocator>::const_iterator::operator++(int) noexcept
    {
        const_iterator temp(m_ptr);
        m_ptr++;
        return temp;
    }

    template <typename T, typename Allocator>
    inline typename vector<T, Allocator>::const_iterator &vector<T, Allocator>::const_iterator::operator--() noexcept
    {
        m_ptr--;
        return *this;
    }

    template <typename T, typename Allocator>
    inline typename vector<T, Allocator>::const_iterator vector<T, Allocator>::const_iterator::operator--(int) noexcept
    {
        const_iterator temp(m_ptr);
        m_ptr--;
        return temp;
    }

    template <typename T, typename Allocator>
    inline typename vector<T, Allocator>::const_iterator vector<T, Allocator>::const_iterator::operator+(difference_type offset) const noexcept
    {
        return const_iterator(m_ptr + offset);
    }

    template <typename T, typename Allocator>
    inline typename vector<T, Allocator>::const_iterator vector<T, Allocator>::const_iterator::operator-(difference_type offset) const noexcept
    {
        return const_iterator(m_ptr - offset);
    }

    template <typename T, typename Allocator>
    inline typename vector<T, Allocator>::const_iterator &vector<T, Allocator>::const_iterator::operator+=(difference_type offset) noexcept
    {
        m_ptr += offset;
        return *this;
    }

    template <typename T, typename Allocator>
    inline typename vector<T, Allocator>::const_iterator &vector<T, Allocator>::const_iterator::operator-=(difference_type offset) noexcept
    {
        m_ptr -= offset;
        return *this;
    }

    template <typename T, typename Allocator>
    inline typename vector<T, Allocator>::difference_type vector<T, Allocator>::const_iterator::operator-(const const_iterator &other) const noexcept
    {
        return static_cast<difference_type>(m_ptr - other.m_ptr);
    }

    template <typename T, typename Allocator>
    inline bool vector<T, Allocator>::const_iterator::operator==(const const_iterator &other) const noexcept
    {
        return m_ptr == other.m_ptr;
    }

    template <typename T, typename Allocator>
    inline bool vector<T, Allocator>::const_iterator::operator!=(const const_iterator &other) const noexcept
    {
        return m_ptr != other.m_ptr;
    }

    template <typename T, typename Allocator>
    inline bool vector<T, Allocator>::const_iterator::operator<(const const_iterator &other) const noexcept
    {
        return m_ptr < other.m_ptr;
    }

    template <typename T, typename Allocator>
    inline bool vector<T, Allocator>::const_iterator::operator>(const const_iterator &other) const noexcept
    {
        return m_ptr > other.m_ptr;
    }

    template <typename T, typename Allocator>
    inline bool vector<T, Allocator>::const_iterator::operator>=(const const_iterator &other) const noexcept
    {
        return m_ptr >= other.m_ptr;
    }

    template <typename T, typename Allocator>
    inline bool vector<T, Allocator>::const_iterator::operator<=(const const_iterator &other) const noexcept
    {
        return m_ptr <= other.m_ptr;
    }

    template <typename T, typename Allocator>
    inline bool vector<T, Allocator>::const_iterator::operator==(const iterator &other) const noexcept
    {
        return m_ptr == other.m_ptr;
    }

    template <typename T, typename Allocator>
    inline bool vector<T, Allocator>::const_iterator::operator!=(const iterator &other) const noexcept
    {
        return m_ptr != other.m_ptr;
    }

    template <typename T, typename Allocator>
    inline typename vector<T, Allocator>::pointer vector<T, Allocator>::const_iterator::base() const noexcept
    {
        return m_ptr;
    }

    // ------------------------------ vector 核心实现 ------------------------------
    template <typename T, typename Allocator>
    inline vector<T, Allocator>::vector()
        : m_data(nullptr), m_size(0), m_capacity(0), m_allocator(Allocator())
    {
    }

    template <typename T, typename Allocator>
    inline demo::vector<T, Allocator>::vector(size_type size)
        : m_data(alloc_traits::allocate(m_allocator, size)), m_size(size), m_capacity(size)
    {
        for (size_type i = 0; i < m_size; i++)
        {
            alloc_traits::construct(m_allocator, m_data + i, value_type());
        }
    }

    template <typename T, typename Allocator>
    inline vector<T, Allocator>::vector(size_type size, const_reference val)
        : m_data(alloc_traits::allocate(m_allocator, size)), m_size(size), m_capacity(size)
    {
        for (size_t i = 0; i < m_size; i++)
        {
            alloc_traits::construct(m_allocator, m_data + i, val);
        }
    }

    template <typename T, typename Allocator>
    template <typename InputIt>
    inline vector<T, Allocator>::vector(InputIt first, InputIt last)
        : m_data(nullptr), m_size(0), m_capacity(0)
    {
        using iter_category = typename std::iterator_traits<InputIt>::iterator_category;
        constexpr bool is_random_access = std::is_base_of<
            std::random_access_iterator_tag, iter_category>::value;

        if constexpr (is_random_access)
        {
            size_type count = std::distance(first, last);
            if (count > 0)
                reserve(count);
            for (size_type i = 0; i < count; ++i, ++first)
            {
                alloc_traits::construct(m_allocator, m_data + i, *first);
            }
            m_size = count;
        }
        else
        {
            for (; first != last; first++)
                push_back(*first);
        }
    }

    template <typename T, typename Allocator>
    inline vector<T, Allocator>::vector(std::initializer_list<value_type> ilist)
        : m_data(nullptr), m_size(0), m_capacity(0)
    {
        size_type count = ilist.size();
        if (count > 0)
            reserve(count);
        size_type i = 0;
        for (const_reference val : ilist)
        {
            alloc_traits::construct(m_allocator, m_data + i, val);
            i++;
        }
        m_size = count;
    }

    template <typename T, typename Allocator>
    inline vector<T, Allocator>::vector(const vector &other)
        : m_data(alloc_traits::allocate(m_allocator, other.m_size)), m_size(other.m_size),
          m_capacity(other.m_capacity)
    {
        for (size_type i = 0; i < m_size; i++)
        {
            alloc_traits::construct(m_allocator, m_data + i, other.m_data[i]);
        }
    }
    template <typename T, typename Allocator>
    inline vector<T, Allocator> &vector<T, Allocator>::operator=(const vector &other)
    {
        if (this == &other)
            return *this;

        // 先析构当前对象的所有元素
        clear();

        if (m_capacity >= other.m_size)
        {
            // 已有空间足够，直接在原空间上构造
            for (size_type i = 0; i < other.m_size; i++)
            {
                alloc_traits::construct(m_allocator, m_data + i, other.m_data[i]);
            }
            m_size = other.m_size;
        }
        else
        {
            // 原空间不够，需要扩容
            if (m_data != nullptr)
                alloc_traits::deallocate(m_allocator, m_data, m_capacity);
            m_data = alloc_traits::allocate(m_allocator, other.m_capacity);
            m_capacity = other.m_capacity;
            for (size_type i = 0; i < other.m_size; i++)
            {
                alloc_traits::construct(m_allocator, m_data + i, other.m_data[i]);
            }
            m_size = other.m_size;
        }
        return *this;
    }

    template <typename T, typename Allocator>
    inline vector<T, Allocator> &vector<T, Allocator>::operator=(std::initializer_list<value_type> ilist)
    {
        // 先清空当前元素
        clear();

        // 预分配内存
        size_type count = ilist.size();
        if (count > 0)
            reserve(count);

        // 构造新元素
        size_type i = 0;
        for (const value_type &val : ilist)
        {
            alloc_traits::construct(m_allocator, m_data + i, val);
            ++i;
        }
        m_size = count;

        return *this;
    }

    template <typename T, typename Allocator>
    inline vector<T, Allocator>::vector(vector &&other) noexcept
        : m_data(other.m_data), m_size(other.m_size), m_capacity(other.m_capacity)
    {
        other.m_data = nullptr;
        other.m_size = 0;
        other.m_capacity = 0;
    }
    template <typename T, typename Allocator>
    inline vector<T, Allocator> &vector<T, Allocator>::operator=(vector &&other) noexcept
    {
        if (this == &other)
            return *this;

        clear();
        if (m_data != nullptr)
            alloc_traits::deallocate(m_allocator, m_data, m_capacity);

        m_data = other.m_data;
        m_size = other.m_size;
        m_capacity = other.m_capacity;

        other.m_data = nullptr;
        other.m_size = 0;
        other.m_capacity = 0;

        return *this;
    }

    template <typename T, typename Allocator>
    inline vector<T, Allocator>::~vector()
    {
        clear(); // 先销毁所有元素

        if (m_data != nullptr)
            alloc_traits::deallocate(m_allocator, m_data, m_capacity);

        m_data = nullptr;
        m_size = 0;
        m_capacity = 0;
    }

    template <typename T, typename Allocator>
    inline void demo::vector<T, Allocator>::assign(size_type count, const_reference val)
    {
        clear();

        if (count > m_capacity)
            reserve(count);

        for (size_type i = 0; i < count; i++)
        {
            alloc_traits::construct(m_allocator, m_data + i, val);
        }

        m_size = count;
    }

    template <typename T, typename Allocator>
    template <typename InputIt>
    inline void vector<T, Allocator>::assign(InputIt first, InputIt last)
    {
        clear();

        using iter_category = typename std::iterator_traits<InputIt>::iterator_category;
        constexpr bool is_random_access_iterator = std::is_base_of<
            std::random_access_iterator_tag, iter_category>::value;

        if constexpr (is_random_access_iterator)
        {
            size_type count = std::distance(first, last);
            if (count > 0)
                reserve(count);

            for (size_type i = 0; i < count; i++, first++)
            {
                alloc_traits::construct(m_allocator, m_data + i, *first);
            }
            m_size = count;
        }
        else
        {
            for (; first != last; first++)
                push_back(*first);
        }
    }

    template <typename T, typename Allocator>
    inline void demo::vector<T, Allocator>::assign(std::initializer_list<value_type> ilist)
    {
        clear();

        size_type count = ilist.size();
        if (count > 0)
            reserve(count);

        size_type i = 0;
        for (auto &val : ilist)
        {
            alloc_traits::construct(m_allocator, m_data + i, val);
            i++;
        }
        m_size = ilist.size();
    }

    template <typename T, typename Allocator>
    inline Allocator vector<T, Allocator>::get_allocator() const
    {
        return m_allocator;
    }

    template <typename T, typename Allocator>
    inline typename vector<T, Allocator>::reference vector<T, Allocator>::at(size_type index)
    {
        if (index >= m_size)
            throw std::out_of_range("vector::at: index out of range.");

        return m_data[index];
    }

    template <typename T, typename Allocator>
    inline typename vector<T, Allocator>::const_reference vector<T, Allocator>::at(size_type index) const
    {
        if (index >= m_size)
            throw std::out_of_range("vector::at: index out of range.");

        return m_data[index];
    }

    template <typename T, typename Allocator>
    inline typename vector<T, Allocator>::reference vector<T, Allocator>::operator[](size_type index)
    {
        return m_data[index];
    }

    template <typename T, typename Allocator>
    inline typename vector<T, Allocator>::const_reference vector<T, Allocator>::operator[](size_type index) const
    {
        return m_data[index]; // []不做越界检查
    }

    template <typename T, typename Allocator>
    inline typename vector<T, Allocator>::reference vector<T, Allocator>::front()
    {
        if (m_size == 0)
            throw std::out_of_range("vector::front: empty vector");
        return m_data[0];
    }

    template <typename T, typename Allocator>
    inline typename vector<T, Allocator>::const_reference vector<T, Allocator>::front() const
    {
        if (m_size == 0)
            throw std::out_of_range("vector::front: empty vector");
        return m_data[0];
    }

    template <typename T, typename Allocator>
    inline typename vector<T, Allocator>::reference vector<T, Allocator>::back()
    {
        if (m_size == 0)
            throw std::out_of_range("vector::back: empty vector");
        return m_data[m_size - 1];
    }

    template <typename T, typename Allocator>
    inline typename vector<T, Allocator>::const_reference vector<T, Allocator>::back() const
    {
        if (m_size == 0)
            throw std::out_of_range("vector::back: empty vector");
        return m_data[m_size - 1];
    }

    template <typename T, typename Allocator>
    inline typename vector<T, Allocator>::pointer vector<T, Allocator>::data()
    {
        return m_data;
    }

    template <typename T, typename Allocator>
    inline typename vector<T, Allocator>::const_pointer vector<T, Allocator>::data() const
    {
        return m_data;
    }

    template <typename T, typename Allocator>
    inline typename vector<T, Allocator>::iterator vector<T, Allocator>::begin()
    {
        return iterator(m_data);
    }

    template <typename T, typename Allocator>
    inline typename vector<T, Allocator>::const_iterator vector<T, Allocator>::begin() const
    {
        return const_iterator(m_data);
    }

    template <typename T, typename Allocator>
    inline typename vector<T, Allocator>::const_iterator vector<T, Allocator>::cbegin() const
    {
        return const_iterator(m_data);
    }

    template <typename T, typename Allocator>
    inline typename vector<T, Allocator>::iterator vector<T, Allocator>::end()
    {
        return iterator(m_data + m_size);
    }

    template <typename T, typename Allocator>
    inline typename vector<T, Allocator>::const_iterator vector<T, Allocator>::end() const
    {
        return const_iterator(m_data + m_size);
    }

    template <typename T, typename Allocator>
    inline typename vector<T, Allocator>::const_iterator vector<T, Allocator>::cend() const
    {
        return const_iterator(m_data + m_size);
    }

    template <typename T, typename Allocator>
    inline typename vector<T, Allocator>::reverse_iterator vector<T, Allocator>::rbegin()
    {
        return reverse_iterator(end());
    }

    template <typename T, typename Allocator>
    inline typename vector<T, Allocator>::const_reverse_iterator vector<T, Allocator>::rbegin() const
    {
        return const_reverse_iterator(end());
    }

    template <typename T, typename Allocator>
    inline typename vector<T, Allocator>::const_reverse_iterator vector<T, Allocator>::crbegin() const
    {
        return const_reverse_iterator(end());
    }

    template <typename T, typename Allocator>
    inline typename vector<T, Allocator>::reverse_iterator vector<T, Allocator>::rend()
    {
        return reverse_iterator(begin());
    }

    template <typename T, typename Allocator>
    inline typename vector<T, Allocator>::const_reverse_iterator vector<T, Allocator>::crend() const
    {
        return const_reverse_iterator(begin());
    }

    template <typename T, typename Allocator>
    inline bool vector<T, Allocator>::empty() const
    {
        return m_size == 0;
    }

    template <typename T, typename Allocator>
    inline typename vector<T, Allocator>::size_type vector<T, Allocator>::size() const
    {
        return m_size;
    }

    template <typename T, typename Allocator>
    inline typename vector<T, Allocator>::size_type vector<T, Allocator>::max_size() const
    {
        return alloc_traits::max_size(m_allocator);
    }

    template <typename T, typename Allocator>
    inline void vector<T, Allocator>::reserve(size_type new_capacity)
    {
        if (new_capacity <= m_capacity)
            return;

        pointer new_data = alloc_traits::allocate(m_allocator, new_capacity);

        size_type constructed_count = 0;
        try
        {
            for (size_type i = 0; i < m_size; i++)
            {
                alloc_traits::construct(m_allocator, new_data + i, std::move_if_noexcept(m_data[i]));
                constructed_count++;
            }
        }
        catch (...)
        {
            for (size_type i = 0; i < constructed_count; i++)
                alloc_traits::destroy(m_allocator, new_data + i);
            alloc_traits::deallocate(m_allocator, new_data, new_capacity);
            throw;
        }

        for (size_type i = 0; i < m_size; i++)
            alloc_traits::destroy(m_allocator, m_data + i);

        if (m_data != nullptr)
            alloc_traits::deallocate(m_allocator, m_data, m_capacity);

        m_data = new_data;
        m_capacity = new_capacity;
    }

    template <typename T, typename Allocator>
    inline typename vector<T, Allocator>::size_type vector<T, Allocator>::capacity() const
    {
        return m_capacity;
    }

    template <typename T, typename Allocator>
    inline void vector<T, Allocator>::shrink_to_fit()
    {
        if (m_size < m_capacity)
        {
            pointer new_data = alloc_traits::allocate(m_allocator, m_size);
            for (size_type i = 0; i < m_size; ++i)
            {
                alloc_traits::construct(m_allocator, new_data + i, std::move_if_noexcept(m_data[i]));
                alloc_traits::destroy(m_allocator, &m_data[i]);
            }
            alloc_traits::deallocate(m_allocator, m_data, m_capacity);
            m_data = new_data;
            m_capacity = m_size;
        }
    }

    template <typename T, typename Allocator>
    inline void vector<T, Allocator>::clear()
    {
        for (size_type i = 0; i < m_size; i++)
            alloc_traits::destroy(m_allocator, m_data + i);
        m_size = 0;
    }

    template <typename T, typename Allocator>
    inline typename vector<T, Allocator>::iterator vector<T, Allocator>::
        insert(const_iterator pos, const_reference val)
    {
        difference_type diff = pos - cbegin();
        if (diff < 0 || static_cast<size_type>(diff) > m_size)
            throw std::out_of_range("vector::insert: invalid position");

        if (m_capacity < m_size + 1)
        {
            size_type new_capacity = (m_capacity + 2) * 2;
            reserve(new_capacity);
        }

        // for (size_type i = m_size; i > static_cast<size_type>(diff); i--)
        // {
        //     alloc_traits::construct(m_allocator, m_data + i, std::move_if_noexcept(m_data[i - 1]));
        //     alloc_traits::destroy(m_allocator, m_data + i - 1);
        // }

        size_type moved_count = 0;
        try
        {
            for (size_type i = m_size; i > static_cast<size_type>(diff); i--)
            {
                alloc_traits::construct(m_allocator, m_data + i, std::move_if_noexcept(m_data[i - 1]));
                moved_count++;
                alloc_traits::destroy(m_allocator, m_data + i - 1);
            }
        }
        catch (...)
        {
            // 异常回滚：销毁已构造的新元素，恢复原状态
            for (size_type i = 0; i < moved_count; ++i)
            {
                size_type idx = m_size - i;
                alloc_traits::construct(m_allocator, m_data + idx - 1, std::move_if_noexcept(m_data[idx]));
                alloc_traits::destroy(m_allocator, m_data + idx);
            }
            throw;
        }

        alloc_traits::construct(m_allocator, m_data + diff, val);
        m_size++;

        return iterator(m_data + diff);
    }

    template <typename T, typename Allocator>
    inline typename vector<T, Allocator>::iterator vector<T, Allocator>::
        insert(const_iterator pos, value_type &&val)
    {
        difference_type diff = pos - cbegin();
        if (diff < 0 || static_cast<size_type>(diff) > m_size)
            throw std::out_of_range("vector::insert: invalid position");

        if (m_capacity < m_size + 1)
        {
            size_type new_capacity = (m_capacity + 2) * 2;
            reserve(new_capacity);
        }

        size_type moved_count = 0;
        try
        {
            for (size_type i = m_size; i > static_cast<size_type>(diff); i--)
            {
                alloc_traits::construct(m_allocator, m_data + i, std::move_if_noexcept(m_data[i - 1]));
                moved_count++;
                alloc_traits::destroy(m_allocator, m_data + i - 1);
            }
        }
        catch (...)
        {
            // 异常回滚：销毁已构造的新元素，恢复原状态
            for (size_type i = 0; i < moved_count; ++i)
            {
                size_type idx = m_size - i;
                alloc_traits::construct(m_allocator, m_data + idx - 1, std::move_if_noexcept(m_data[idx]));
                alloc_traits::destroy(m_allocator, m_data + idx);
            }
            throw;
        }

        alloc_traits::construct(m_allocator, m_data + diff, std::move(val));
        m_size++;

        return iterator(m_data + diff);
    }

    template <typename T, typename Allocator>
    inline typename vector<T, Allocator>::iterator vector<T, Allocator>::
        insert(const_iterator pos, size_type count, const_reference val)
    {
        if (count == 0)
            return iterator(const_cast<pointer>(pos.operator->()));

        difference_type diff = pos - cbegin();
        if (diff < 0 || static_cast<size_type>(diff) > m_size)
            throw std::out_of_range("vector::insert: invalid position");

        if (m_capacity < m_size + count)
        {
            size_type new_capacity = std::max((m_capacity + 2) * 2, m_size + count);
            reserve(new_capacity);
        }

        size_type moved_count = 0;
        try
        {
            for (size_type i = m_size + count - 1; i >= static_cast<size_type>(diff) + count; i--)
            {
                alloc_traits::construct(m_allocator, m_data + i, std::move_if_noexcept(m_data[i - count]));
                moved_count++;
                alloc_traits::destroy(m_allocator, m_data + i - count);
            }
        }
        catch (...)
        {
            // 异常回滚：销毁已构造的新元素，恢复原状态
            for (size_type i = 0; i < moved_count; ++i)
            {
                size_type idx = m_size + count - 1 - i;
                alloc_traits::construct(m_allocator, m_data + idx - count, std::move_if_noexcept(m_data[idx]));
                alloc_traits::destroy(m_allocator, m_data + idx);
            }
            throw;
        }

        for (size_type i = 0; i < count; i++)
            alloc_traits::construct(m_allocator, m_data + diff + i, val);
        m_size += count;

        return iterator(m_data + diff);
    }

    template <typename T, typename Allocator>
    inline typename vector<T, Allocator>::iterator vector<T, Allocator>::
        insert(const_iterator pos, std::initializer_list<value_type> ilist)
    {
        if (ilist.size() == 0)
            return iterator(const_cast<pointer>(pos.operator->()));

        size_type count = ilist.size();

        difference_type diff = pos - cbegin();
        if (diff < 0 || static_cast<size_type>(diff) > m_size)
            throw std::out_of_range("vector::insert: invalid position");

        if (m_capacity < m_size + count)
        {
            size_type new_capacity = std::max((m_capacity + 2) * 2, m_size + count);
            reserve(new_capacity);
        }

        size_type moved_count = 0;
        try
        {
            for (size_type i = m_size + count - 1; i >= static_cast<size_type>(diff) + count; i--)
            {
                alloc_traits::construct(m_allocator, m_data + i, std::move_if_noexcept(m_data[i - count]));
                moved_count++;
                alloc_traits::destroy(m_allocator, m_data + i - count);
            }
        }
        catch (...)
        {
            // 异常回滚：销毁已构造的新元素，恢复原状态
            for (size_type i = 0; i < moved_count; ++i)
            {
                size_type idx = m_size + count - 1 - i;
                alloc_traits::construct(m_allocator, m_data + idx - count, std::move_if_noexcept(m_data[idx]));
                alloc_traits::destroy(m_allocator, m_data + idx);
            }
            throw;
        }

        size_type i = 0;
        for (auto &val : ilist)
        {
            alloc_traits::construct(m_allocator, m_data + diff + i, val);
            i++;
        }
        m_size += count;
        return iterator(m_data + diff);
    }

    template <typename T, typename Allocator>
    template <typename InputIt>
    inline typename vector<T, Allocator>::iterator vector<T, Allocator>::insert(const_iterator pos, InputIt first, InputIt last)
    {
        if (first == last)
            return iterator(const_cast<pointer>(pos.operator->()));

        difference_type diff = pos - cbegin();
        if (diff < 0 || static_cast<size_type>(diff) > m_size)
            throw std::out_of_range("vector::insert: invalid position");

        using iter_category = typename std::iterator_traits<InputIt>::iterator_category;
        constexpr bool is_random_access_iterator = std::is_base_of<
            std::random_access_iterator_tag, iter_category>::value;

        if constexpr (is_random_access_iterator)
        {
            size_type count = std::distance(first, last);

            if (m_capacity < m_size + count)
            {
                size_type new_capacity = std::max((m_capacity + 2) * 2, m_size + count);
                reserve(new_capacity);
            }

            size_type moved_count = 0;
            try
            {
                for (size_type i = m_size + count - 1; i >= static_cast<size_type>(diff) + count; i--)
                {
                    alloc_traits::construct(m_allocator, m_data + i, std::move_if_noexcept(m_data[i - count]));
                    moved_count++;
                    alloc_traits::destroy(m_allocator, m_data + i - count);
                }
            }
            catch (...)
            {
                // 异常回滚：销毁已构造的新元素，恢复原状态
                for (size_type i = 0; i < moved_count; ++i)
                {
                    size_type idx = m_size + count - 1 - i;
                    alloc_traits::construct(m_allocator, m_data + idx - count, std::move_if_noexcept(m_data[idx]));
                    alloc_traits::destroy(m_allocator, m_data + idx);
                }
                throw;
            }

            for (size_type i = 0; i < count; i++, first++)
                alloc_traits::construct(m_allocator, m_data + diff + i, *first);

            m_size += count;
            return iterator(m_data + diff);
        }
        else
        {
            size_type cur_diff = static_cast<size_type>(diff);
            while (first != last)
            {
                if (m_capacity == m_size)
                {
                    size_type new_capacity = (m_capacity + 2) * 2;
                    reserve(new_capacity);
                }

                size_type moved_count = 0;
                try
                {
                    for (size_type i = m_size; i > cur_diff; i--)
                    {
                        alloc_traits::construct(m_allocator, m_data + i, std::move_if_noexcept(m_data[i - 1]));
                        moved_count++;
                        alloc_traits::destroy(m_allocator, m_data + i - 1);
                    }
                }
                catch (...)
                {
                    // 异常回滚：销毁已构造的新元素，恢复原状态
                    for (size_type i = 0; i < moved_count; ++i)
                    {
                        size_type idx = m_size - i;
                        alloc_traits::construct(m_allocator, m_data + idx - 1, std::move_if_noexcept(m_data[idx]));
                        alloc_traits::destroy(m_allocator, m_data + idx);
                    }
                    throw;
                }

                alloc_traits::construct(m_allocator, m_data + cur_diff, *first);
                m_size++;
                first++;
                cur_diff++;
            }

            return iterator(m_data + diff);
        }
    }

    template <typename T, typename Allocator>
    template <typename... Args>
    inline void vector<T, Allocator>::emplace(const_iterator pos, Args &&...args)
    {
        difference_type diff = pos - cbegin();
        if (diff < 0 || static_cast<size_type>(diff) > m_size)
            throw std::out_of_range("vector::emplace: invalid position");

        if (m_capacity < m_size + 1)
        {
            size_type new_capacity = (m_capacity + 2) * 2;
            reserve(new_capacity);
        }

        size_type moved_count = 0;
        try
        {
            for (size_type i = m_size; i > static_cast<size_type>(diff); i--)
            {
                alloc_traits::construct(m_allocator, m_data + i, std::move_if_noexcept(m_data[i - 1]));
                moved_count++;
                alloc_traits::destroy(m_allocator, m_data + i - 1);
            }
        }
        catch (...)
        {
            // 异常回滚：销毁已构造的新元素，恢复原状态
            for (size_type i = 0; i < moved_count; ++i)
            {
                size_type idx = m_size - i;
                alloc_traits::construct(m_allocator, m_data + idx - 1, std::move_if_noexcept(m_data[idx]));
                alloc_traits::destroy(m_allocator, m_data + idx);
            }
            throw;
        }

        alloc_traits::construct(m_allocator, m_data + diff, std::forward<Args>(args)...);
        m_size++;
    }

    template <typename T, typename Allocator>
    inline typename vector<T, Allocator>::iterator vector<T, Allocator>::erase(const_iterator pos)
    {
        difference_type diff = pos - cbegin();
        if (diff < 0 || static_cast<size_type>(diff) >= m_size)
            throw std::out_of_range("vector::erase: invalid position");

        alloc_traits::destroy(m_allocator, m_data + diff);

        for (size_type i = static_cast<size_type>(diff); i < m_size - 1; i++)
        {
            alloc_traits::construct(m_allocator, m_data + i, std::move_if_noexcept(m_data[i + 1]));
            alloc_traits::destroy(m_allocator, m_data + i + 1);
        }

        m_size--;
        return iterator(m_data + diff);
    }

    template <typename T, typename Allocator>
    inline typename vector<T, Allocator>::iterator vector<T, Allocator>::
        erase(const_iterator first, const_iterator last)
    {
        if (last < first)
            throw std::out_of_range("vector::erase: last < first (invalid range)");

        difference_type diff = first - cbegin();
        difference_type diff_last = last - cbegin();
        if (diff < 0 || static_cast<size_type>(diff) >= m_size ||
            diff_last < 0 || static_cast<size_type>(diff_last) > m_size)
            throw std::out_of_range("vector::erase: invalid position (out of range)");

        size_type count = static_cast<size_type>(last - first);

        if (count == 0)
            return iterator(const_cast<pointer>(first.base()));

        for (size_type i = 0; i < count; i++)
            alloc_traits::destroy(m_allocator, m_data + diff + i);

        for (size_type i = static_cast<size_type>(diff_last); i < m_size; i++)
        {
            alloc_traits::construct(m_allocator, m_data + i - count, std::move_if_noexcept(m_data[i]));
            alloc_traits::destroy(m_allocator, m_data + i);
        }

        m_size -= count;
        return iterator(m_data + diff);
    }

    template <typename T, typename Allocator>
    inline void vector<T, Allocator>::push_back(const_reference val)
    {
        if (m_capacity < m_size + 1)
        {
            size_type new_capacity = (m_capacity + 2) * 2;
            reserve(new_capacity);
        }

        alloc_traits::construct(m_allocator, m_data + m_size, val);
        m_size++;
    }

    template <typename T, typename Allocator>
    inline void vector<T, Allocator>::push_back(value_type &&val)
    {
        if (m_capacity < m_size + 1)
        {
            size_type new_capacity = (m_capacity + 2) * 2;
            reserve(new_capacity);
        }

        alloc_traits::construct(m_allocator, m_data + m_size, std::move(val));
        m_size++;
    }

    template <typename T, typename Allocator>
    template <typename... Args>
    inline typename vector<T, Allocator>::reference vector<T, Allocator>::emplace_back(Args &&...args)
    {
        if (m_capacity < m_size + 1)
        {
            size_type new_capacity = (m_capacity + 2) * 2;
            reserve(new_capacity);
        }

        alloc_traits::construct(m_allocator, m_data + m_size, std::forward<Args>(args)...);
        m_size++;

        return m_data[m_size - 1];
    }

    template <typename T, typename Allocator>
    inline void vector<T, Allocator>::pop_back()
    {
        if (empty())
            throw std::out_of_range("vector::pop_back: empty vector");
        alloc_traits::destroy(m_allocator, m_data + m_size - 1);
        m_size--;
    }

    template <typename T, typename Allocator>
    inline void vector<T, Allocator>::resize(size_type size, value_type val)
    {
        if (m_size == size)
            return;
        else if (m_size < size)
        {
            if (m_capacity < size)
                reserve(size);

            for (size_type i = m_size; i < size; i++)
            {
                alloc_traits::construct(m_allocator, m_data + i, val);
            }
            m_size = size;
        }
        else
        {
            for (size_type i = size; i < m_size; i++)
            {
                alloc_traits::destroy(m_allocator, m_data + i);
            }
            m_size = size;
        }
    }

    template <typename T, typename Allocator>
    inline void vector<T, Allocator>::swap(vector &other)
    {
        using std::swap;
        swap(m_data, other.m_data);
        swap(m_size, other.m_size);
        swap(m_capacity, other.m_capacity);
        swap(m_allocator, other.m_allocator);
    }

    template <class T, class Alloc>
    bool operator==(const vector<T, Alloc> &lhs, const vector<T, Alloc> &rhs)
    {
        if (lhs.size() != rhs.size())
            return false;

        for (std::size_t i = 0; i < lhs.size(); i++)
        {
            if (lhs[i] != rhs[i])
                return false;
        }

        return true;
    }

    template <class T, class Alloc>
    bool operator!=(const vector<T, Alloc> &lhs, const vector<T, Alloc> &rhs)
    {
        return !operator==(lhs, rhs);
    }

}; // namespace demo
