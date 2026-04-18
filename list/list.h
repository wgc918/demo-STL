#pragma once

#include <type_traits>
#include <iterator>
#include <memory>
#include <initializer_list>

namespace demo
{
    template <typename T,
              typename Allocator = std::allocator<T>>
    class list;

    template <typename T, typename Allocator>
    bool operator==(const list<T, Allocator> &lhs,
                    const list<T, Allocator> &rhs);
    template <typename T, typename Allocator>
    bool operator!=(const list<T, Allocator> &lhs,
                    const list<T, Allocator> &rhs);

    template <typename T, typename Allocator>
    class list
    {
        friend bool operator== <>(
            const list<T, Allocator> &lhs,
            const list<T, Allocator> &rhs);
        friend bool operator!= <>(
            const list<T, Allocator> &lhs,
            const list<T, Allocator> &rhs);

    public:
        using value_type = T;
        using allocator_type = Allocator;
        using reference = T &;
        using const_reference = const T &;
        using pointer = T *;
        using const_pointr = const T *;
        using size_type = std::size_t;
        using difference_type = std::ptrdiff_t;

    private:
        struct Node
        {
            value_type val;
            Node *prev;
            Node *next;

            template <typename... Args>
            Node(Args &&...args)
                : val(std::forward<Args>(args)...),
                  prev(nullptr), next(nullptr) {}
        };

    public:
        class const_iterator;
        class iterator
        {
            friend class list;

        public:
            using iterator_category =
                std::bidirectional_iterator_tag;
            using value_type = T;
            using pointer = T *;
            using reference = T &;
            using difference_type = std::ptrdiff_t;

            iterator() noexcept;
            explicit iterator(Node *ptr) noexcept;
            iterator(const iterator &other) noexcept;

            // 解引用操作
            reference operator*() const noexcept;
            pointer operator->() const noexcept;

            // 自增/自减
            iterator &operator++() noexcept;
            iterator operator++(int) noexcept;
            iterator &operator--() noexcept;
            iterator operator--(int) noexcept;

            // 比较
            bool operator==(
                const iterator other) const noexcept;
            bool operator!=(
                const iterator other) const noexcept;
            bool operator==(
                const const_iterator other) const noexcept;
            bool operator!=(
                const const_iterator other) const noexcept;

        private:
            Node *m_ptr;
        };

        class const_iterator
        {
            friend class list;

        public:
            using iterator_category =
                std::bidirectional_iterator_tag;
            using value_type = T;
            using pointer = const T *;
            using reference = const T &;
            using difference_type = std::ptrdiff_t;

            const_iterator() noexcept;
            explicit const_iterator(Node *ptr) noexcept;
            const_iterator(iterator it) noexcept;
            const_iterator(const const_iterator &other) noexcept;

            // 解引用
            reference operator*() const noexcept;
            pointer operator->() const noexcept;

            // 自增/自减
            const_iterator &operator++() noexcept;
            const_iterator operator++(int) noexcept;
            const_iterator &operator--() noexcept;
            const_iterator operator--(int) noexcept;

            // 比较
            bool operator==(
                const const_iterator other) const noexcept;
            bool operator!=(
                const const_iterator other) const noexcept;
            bool operator==(const iterator other) const noexcept;
            bool operator!=(const iterator other) const noexcept;

        private:
            Node *m_ptr;
        };

        // 使用标准库的反向迭代器适配器
        using reverse_iterator =
            std::reverse_iterator<iterator>;
        using const_reverse_iterator =
            std::reverse_iterator<const_iterator>;

    public:
        explicit list();
        explicit list(size_type count,
                      const_reference value = value_type());
        list(std::initializer_list<value_type> ilist);
        template <typename InputIt>
        list(InputIt first, InputIt last);
        list(const list<T, Allocator> &other);
        list<T, Allocator> &operator=(
            const list<T, Allocator> &other);
        list<T, Allocator> &operator=(
            std::initializer_list<value_type> ilist);
        list(list<T, Allocator> &&other) noexcept;
        list<T, Allocator> &operator=(
            list<T, Allocator> &&other) noexcept;
        ~list();

        void assign(size_type count, const_reference value);
        void assign(std::initializer_list<value_type> ilist);
        template <typename InputIt,
                  std::enable_if_t<!std::is_integral<T>::value, int> = 0>
        void assign(InputIt first, InputIt last);

        allocator_type get_allocator() const;

        // 元素访问
        reference front();
        const_reference front() const;
        reference back();
        const_reference back() const;

        // 迭代器
        iterator begin();
        const_iterator begin() const;
        const_iterator cbegin() const noexcept;
        iterator end();
        const_iterator end() const;
        const_iterator cend() const noexcept;
        reverse_iterator rbegin();
        const_reverse_iterator rbegin() const;
        const_reverse_iterator crbegin() const noexcept;
        reverse_iterator rend();
        const_reverse_iterator rend() const;
        const_reverse_iterator crend() const noexcept;

        // 容量
        bool empty() const;
        size_type size() const noexcept;
        size_type max_size() const;

        // 修改器
        void clear();
        iterator insert(const_iterator pos, const_reference value);
        iterator insert(const_iterator pos, value_type &&value);
        iterator insert(const_iterator pos,
                        size_type count, const_reference value);
        template <typename InputIt,
                  std::enable_if_t<!std::is_integral<T>::value, int> = 0>
        iterator insert(const_iterator pos, InputIt first, InputIt last);
        iterator insert(const_iterator pos,
                        std::initializer_list<value_type> ilist);
        template <typename... Args>
        iterator emplace(const_iterator pos, Args &&...args);
        iterator erase(iterator pos);
        iterator erase(const_iterator pos);
        iterator erase(iterator first, iterator last);
        iterator erase(const_iterator first, const_iterator last);
        void push_back(const_reference value);
        void push_back(value_type &&value);
        template <typename... Args>
        reference empalce_back(Args &&...args);
        void pop_back();
        void push_front(const_reference value);
        void push_front(value_type &&value);
        template <typename... Args>
        reference emplace_front(Args &&...argrs);
        void pop_front();
        void resize(size_type count);
        void resize(size_type count, const_reference value);
        void swap(list<T, Allocator> &other) noexcept;

        // 操作
        void merge(list<T, Allocator> &other);
        void merge(list<T, Allocator> &&other);
        template <typename Compare>
        void merge(list<T, Allocator> &other, Compare comp);
        template <typename Compare>
        void merge(list<T, Allocator> &&other, Compare comp);
        void splice(const_iterator pos, list<T, Allocator> &other);
        void splice(const_iterator pos, list<T, Allocator> &&other);
        void splice(const_iterator pos, list &other, const_iterator it);
        void splice(const_iterator pos, list &&other, const_iterator it);
        void splice(const_iterator pos, list &other,
                    const_iterator first, const_iterator last);
        void splice(const_iterator pos, list &&other,
                    const_iterator first, const_iterator last);
        size_type remove(const T &value);
        template <class UnaryPredicate>
        size_type remove_if(UnaryPredicate p);
        void reverse();
        size_type unique();
        template <class BinaryPredicate>
        size_type unique(BinaryPredicate p);
        void sort();
        template <class Compare>
        void sort(Compare comp);

    private:
        Node *split(Node *head, size_type len);
        Node *merge_tow(Node *la, Node *lb);
        template <typename Compare>
        Node *merge_tow(Node *la, Node *lb, Compare comp);

    private:
        using node_allocator = typename std::allocator_traits<
            Allocator>::template rebind_alloc<Node>;

        Node *m_head;
        size_type m_size;
        node_allocator m_node_alloc;

        using alloc_traits = std::allocator_traits<node_allocator>;
    };

    //------------------- iterator 实现 -------------------------------
    template <typename T, typename Allocator>
    inline list<T, Allocator>::iterator::iterator() noexcept
        : m_ptr(nullptr)
    {
    }

    template <typename T, typename Allocator>
    inline list<T, Allocator>::iterator::iterator(Node *ptr) noexcept
        : m_ptr(ptr)
    {
    }

    template <typename T, typename Allocator>
    inline list<T, Allocator>::iterator::iterator(const iterator &
                                                      other) noexcept
        : m_ptr(other.m_ptr)
    {
    }

    // 解引用操作
    template <typename T, typename Allocator>
    inline typename list<T, Allocator>::iterator::reference
    list<T, Allocator>::iterator::operator*() const noexcept
    {
        // c++标准库判空吗？？  答案是不判空
        return m_ptr->val;
    }

    template <typename T, typename Allocator>
    inline typename list<T, Allocator>::iterator::pointer
    list<T, Allocator>::iterator::operator->() const noexcept
    {
        return &m_ptr->val;
    }

    // 自增/自减
    template <typename T, typename Allocator>
    inline typename list<T, Allocator>::iterator &
    list<T, Allocator>::iterator::operator++() noexcept
    {
        m_ptr = m_ptr->next;
        return *this;
    }

    template <typename T, typename Allocator>
    inline typename list<T, Allocator>::iterator list<T, Allocator>::
        iterator::operator++(int) noexcept
    {
        iterator temp(m_ptr);
        m_ptr = m_ptr->next;
        return temp;
    }

    template <typename T, typename Allocator>
    inline typename list<T, Allocator>::iterator &
    list<T, Allocator>::iterator::operator--() noexcept
    {
        m_ptr = m_ptr->prev;
        return *this;
    }

    template <typename T, typename Allocator>
    inline typename list<T, Allocator>::iterator
    list<T, Allocator>::iterator::operator--(int) noexcept
    {
        iterator temp(m_ptr);
        m_ptr = m_ptr->prev;
        return temp;
    }

    // 比较
    template <typename T, typename Allocator>
    inline bool list<T, Allocator>::iterator::
    operator==(const iterator other) const noexcept
    {
        return m_ptr == other.m_ptr;
    }

    template <typename T, typename Allocator>
    inline bool list<T, Allocator>::iterator::
    operator!=(const iterator other) const noexcept
    {
        return !(*this == other);
    }

    template <typename T, typename Allocator>
    inline bool list<T, Allocator>::iterator::
    operator==(const const_iterator other) const noexcept
    {
        return m_ptr == other.m_ptr;
    }

    template <typename T, typename Allocator>
    inline bool list<T, Allocator>::iterator::
    operator!=(const const_iterator other) const noexcept
    {
        return !(*this == other);
    }

    //--------------- const_iterator 实现 -----------------------
    template <typename T, typename Allocator>
    inline list<T, Allocator>::const_iterator::
        const_iterator() noexcept
        : m_ptr(nullptr)
    {
    }

    template <typename T, typename Allocator>
    inline list<T, Allocator>::const_iterator::
        const_iterator(Node *ptr) noexcept
        : m_ptr(ptr)
    {
    }

    template <typename T, typename Allocator>
    inline list<T, Allocator>::const_iterator::
        const_iterator(iterator it) noexcept
        : m_ptr(it.m_ptr)
    {
    }

    template <typename T, typename Allocator>
    inline list<T, Allocator>::const_iterator::
        const_iterator(const const_iterator &other) noexcept
        : m_ptr(other.m_ptr)
    {
    }

    // 解引用
    template <typename T, typename Allocator>
    inline typename list<T, Allocator>::const_iterator::reference
    list<T, Allocator>::const_iterator::operator*() const noexcept
    {
        return m_ptr->val;
    }

    template <typename T, typename Allocator>
    inline typename list<T, Allocator>::const_iterator::pointer
    list<T, Allocator>::const_iterator::operator->() const noexcept
    {
        return &m_ptr->val;
    }

    // 自增/自减
    template <typename T, typename Allocator>
    inline typename list<T, Allocator>::const_iterator &
    list<T, Allocator>::const_iterator::operator++() noexcept
    {
        m_ptr = m_ptr->next;
        return *this;
    }

    template <typename T, typename Allocator>
    inline typename list<T, Allocator>::const_iterator
    list<T, Allocator>::const_iterator::operator++(int) noexcept
    {
        const_iterator temp(m_ptr);
        m_ptr = m_ptr->next;
        return temp;
    }

    template <typename T, typename Allocator>
    inline typename list<T, Allocator>::const_iterator &
    list<T, Allocator>::const_iterator::operator--() noexcept
    {
        m_ptr = m_ptr->prev;
        return *this;
    }

    template <typename T, typename Allocator>
    inline typename list<T, Allocator>::const_iterator
    list<T, Allocator>::const_iterator::operator--(int) noexcept
    {
        const_iterator temp(m_ptr);
        m_ptr = m_ptr->prev;
        return temp;
    }

    // 比较
    template <typename T, typename Allocator>
    inline bool list<T, Allocator>::const_iterator::
    operator==(const const_iterator other) const noexcept
    {
        return m_ptr == other.m_ptr;
    }

    template <typename T, typename Allocator>
    inline bool list<T, Allocator>::const_iterator::
    operator!=(const const_iterator other) const noexcept
    {
        return !(*this == other);
    }

    template <typename T, typename Allocator>
    inline bool list<T, Allocator>::const_iterator::
    operator==(const iterator other) const noexcept
    {
        return m_ptr == other.m_ptr;
    }

    template <typename T, typename Allocator>
    inline bool list<T, Allocator>::const_iterator::
    operator!=(const iterator other) const noexcept
    {
        return !(*this == other);
    }

    //------------------ list 实现 -----------------------
    template <typename T, typename Allocator>
    inline list<T, Allocator>::list()
        : m_head(nullptr), m_size(0), m_node_alloc()
    {
        m_head = alloc_traits::allocate(m_node_alloc, 1);
        alloc_traits::construct(m_node_alloc, m_head);
        m_head->prev = m_head;
        m_head->next = m_head;
    }

    template <typename T, typename Allocator>
    inline list<T, Allocator>::list(size_type count, const_reference value)
        : m_head(nullptr), m_size(0), m_node_alloc()
    {
        // if (count > max_size())
        //     throw std::length_error("list count exceeds max_size");

        // m_head = alloc_traits::allocate(m_node_alloc, 1);
        // alloc_traits::construct(m_node_alloc, m_head);

        // Node *cur = m_head;
        // Node *pre = m_head;
        // for (size_type i = 0; i < count; ++i)
        // {
        //     cur->next = alloc_traits::allocate(m_node_alloc, 1);
        //     alloc_traits::construct(m_node_alloc, cur->next, value);
        //     pre = cur;
        //     cur = cur->next;
        //     cur->prev = pre;
        //     m_size++;
        // }

        // // 最后一个节点与头节点相连成环
        // cur->next = m_head;
        // m_head->pre = cur;

        m_head = alloc_traits::allocate(m_node_alloc, 1);
        alloc_traits::construct(m_node_alloc, m_head);
        m_head->prev = m_head;
        m_head->next = m_head;
        assign(count, value);
    }

    template <typename T, typename Allocator>
    inline list<T, Allocator>::list(std::initializer_list<value_type> ilist)
        : m_head(nullptr), m_size(0), m_node_alloc()
    {
        m_head = alloc_traits::allocate(m_node_alloc, 1);
        alloc_traits::construct(m_node_alloc, m_head);
        m_head->prev = m_head;
        m_head->next = m_head;
        assign(ilist);
    }

    template <typename T, typename Allocator>
    template <typename InputIt>
    inline list<T, Allocator>::list(InputIt first, InputIt last)
        : m_head(nullptr), m_size(0), m_node_alloc()
    {
        m_head = alloc_traits::allocate(m_node_alloc, 1);
        alloc_traits::construct(m_node_alloc, m_head);
        m_head->prev = m_head;
        m_head->next = m_head;
        assign(first, last);
    }

    template <typename T, typename Allocator>
    inline list<T, Allocator>::list(const list<T, Allocator> &other)
        : m_head(nullptr), m_size(0), m_node_alloc()
    {
        m_head = alloc_traits::allocate(m_node_alloc, 1);
        alloc_traits::construct(m_node_alloc, m_head);

        Node *cur = m_head;
        Node *pre = m_head;
        for (Node *i = other.m_head->next; i != other.m_head; i = i->next)
        {
            cur->next = alloc_traits::allocate(m_node_alloc, 1);
            alloc_traits::construct(m_node_alloc, cur->next, i->val);
            pre = cur;
            cur = cur->next;
            cur->prev = pre;
            m_size++;
        }

        // 最后一个节点与头节点相连成环
        cur->next = m_head;
        m_head->prev = cur;
    }

    template <typename T, typename Allocator>
    inline list<T, Allocator> &list<T, Allocator>::
    operator=(const list<T, Allocator> &other)
    {
        if (this != &other)
        {
            clear();

            Node *cur = m_head;
            Node *pre = m_head;
            for (Node *i = other.m_head->next; i != other.m_head; i = i->next)
            {
                cur->next = alloc_traits::allocate(m_node_alloc, 1);
                alloc_traits::construct(m_node_alloc, cur->next, i->val);
                pre = cur;
                cur = cur->next;
                cur->prev = pre;
                m_size++;
            }

            // 最后一个节点与头节点相连成环
            cur->next = m_head;
            m_head->prev = cur;
        }
        return *this;
    }

    template <typename T, typename Allocator>
    inline list<T, Allocator> &list<T, Allocator>::
    operator=(std::initializer_list<value_type> ilist)
    {
        assign(ilist);
        return *this;
    }

    template <typename T, typename Allocator>
    inline list<T, Allocator>::list(list<T, Allocator> &&other) noexcept
        : m_head(nullptr), m_size(0), m_node_alloc()
    {
        m_head = other.m_head;
        m_size = other.m_size;

        other.m_size = 0;
        other.m_head = alloc_traits::allocate(m_node_alloc, 1);
        alloc_traits::construct(m_node_alloc, other.m_head);
        other.m_head->prev = other.m_head;
        other.m_head->next = other.m_head;
    }
    template <typename T, typename Allocator>
    inline list<T, Allocator> &list<T, Allocator>::
    operator=(list<T, Allocator> &&other) noexcept
    {
        if (this != &other)
        {
            clear();

            m_head = other.m_head;
            m_size = other.m_size;

            other.m_size = 0;
            other.m_head = alloc_traits::allocate(m_node_alloc, 1);
            alloc_traits::construct(m_node_alloc, other.m_head);
            other.m_head->prev = other.m_head;
            other.m_head->next = other.m_head;
        }
        return *this;
    }

    template <typename T, typename Allocator>
    inline list<T, Allocator>::~list()
    {
        clear();
        if (m_head != nullptr)
        {
            alloc_traits::destroy(m_node_alloc, m_head);
            alloc_traits::deallocate(m_node_alloc, m_head, 1);
        }
    }

    template <typename T, typename Allocator>
    inline void list<T, Allocator>::assign(size_type count,
                                           const_reference value)
    {
        if (count > max_size())
            throw std::length_error("list count exceeds max_size");

        clear();

        Node *pre = m_head;
        Node *cur = m_head;
        for (size_type i = 0; i < count; i++)
        {
            cur->next = alloc_traits::allocate(m_node_alloc, 1);
            alloc_traits::construct(m_node_alloc, cur->next, value);
            pre = cur;
            cur = cur->next;
            cur->prev = pre;
            m_size++;
        }

        // 最后一个节点与头节点相连成环
        cur->next = m_head;
        m_head->prev = cur;
    }

    template <typename T, typename Allocator>
    inline void list<T, Allocator>::assign(
        std::initializer_list<value_type> ilist)
    {
        clear();

        Node *pre = m_head;
        Node *cur = m_head;
        for (const_reference item : ilist)
        {
            cur->next = alloc_traits::allocate(m_node_alloc, 1);
            alloc_traits::construct(m_node_alloc, cur->next, item);
            pre = cur;
            cur = cur->next;
            cur->prev = pre;
            m_size++;
        }

        // 最后一个节点与头节点相连成环
        cur->next = m_head;
        m_head->prev = cur;
    }

    template <typename T, typename Allocator>
    template <typename InputIt,
              std::enable_if_t<!std::is_integral<T>::value, int>>
    void list<T, Allocator>::assign(InputIt first, InputIt last)
    {
        clear();

        Node *pre = m_head;
        Node *cur = m_head;
        for (; first != last; first++)
        {
            cur->next = alloc_traits::allocate(m_node_alloc, 1);
            alloc_traits::construct(m_node_alloc, cur->next, *first);
            pre = cur;
            cur = cur->next;
            cur->prev = pre;
            m_size++;
        }

        // 最后一个节点与头节点相连成环
        cur->next = m_head;
        m_head->prev = cur;
    }

    template <typename T, typename Allocator>
    inline typename list<T, Allocator>::allocator_type
    list<T, Allocator>::get_allocator() const
    {
        return typename std::allocator_traits<node_allocator>::
            template rebind_alloc<T>(m_node_alloc);
    }

    template <typename T, typename Allocator>
    inline typename list<T, Allocator>::reference
    list<T, Allocator>::front()
    {
        if (empty())
            throw std::out_of_range("list::front: empty list");
        return m_head->next->val;
    }

    template <typename T, typename Allocator>
    inline typename list<T, Allocator>::const_reference
    list<T, Allocator>::front() const
    {
        if (empty())
            throw std::out_of_range("list::front: empty list");
        return m_head->next->val;
    }

    template <typename T, typename Allocator>
    inline typename list<T, Allocator>::reference
    list<T, Allocator>::back()
    {
        if (empty())
            throw std::out_of_range("list::back: empty list");
        return m_head->prev->val;
    }
    template <typename T, typename Allocator>
    inline typename list<T, Allocator>::const_reference
    list<T, Allocator>::back() const
    {
        if (empty())
            throw std::out_of_range("list::back: empty list");
        return m_head->prev->val;
    }

    template <typename T, typename Allocator>
    inline typename list<T, Allocator>::iterator
    list<T, Allocator>::begin()
    {
        return iterator(m_head->next);
    }

    template <typename T, typename Allocator>
    inline typename list<T, Allocator>::const_iterator
    list<T, Allocator>::begin() const
    {
        return const_iterator(m_head->next);
    }

    template <typename T, typename Allocator>
    inline typename list<T, Allocator>::const_iterator
    list<T, Allocator>::cbegin() const noexcept
    {
        return begin();
    }

    template <typename T, typename Allocator>
    inline typename list<T, Allocator>::iterator
    list<T, Allocator>::end()
    {
        return iterator(m_head);
    }

    template <typename T, typename Allocator>
    inline typename list<T, Allocator>::const_iterator
    list<T, Allocator>::end() const
    {
        return const_iterator(m_head);
    }

    template <typename T, typename Allocator>
    inline typename list<T, Allocator>::const_iterator
    list<T, Allocator>::cend() const noexcept
    {
        return end();
    }

    template <typename T, typename Allocator>
    inline typename list<T, Allocator>::reverse_iterator
    list<T, Allocator>::rbegin()
    {
        return reverse_iterator(end());
    }
    template <typename T, typename Allocator>
    inline typename list<T, Allocator>::const_reverse_iterator
    list<T, Allocator>::rbegin() const
    {
        return const_reverse_iterator(end());
    }

    template <typename T, typename Allocator>
    inline typename list<T, Allocator>::const_reverse_iterator
    list<T, Allocator>::crbegin() const noexcept
    {
        return rbegin();
    }

    template <typename T, typename Allocator>
    inline typename list<T, Allocator>::reverse_iterator
    list<T, Allocator>::rend()
    {
        return reverse_iterator(begin());
    }
    template <typename T, typename Allocator>
    inline typename list<T, Allocator>::const_reverse_iterator
    list<T, Allocator>::rend() const
    {
        return const_reverse_iterator(begin());
    }
    template <typename T, typename Allocator>
    inline typename list<T, Allocator>::const_reverse_iterator
    list<T, Allocator>::crend() const noexcept
    {
        return rend();
    }

    template <typename T, typename Allocator>
    inline bool list<T, Allocator>::empty() const
    {
        return m_head->next == m_head;
    }

    template <typename T, typename Allocator>
    inline typename list<T, Allocator>::size_type
    list<T, Allocator>::size() const noexcept
    {
        return m_size;
    }

    template <typename T, typename Allocator>
    inline typename list<T, Allocator>::size_type
    list<T, Allocator>::max_size() const
    {
        return alloc_traits::max_size(m_node_alloc);
    }

    template <typename T, typename Allocator>
    inline void list<T, Allocator>::clear()
    {
        Node *cur = m_head->next;
        Node *del_node = nullptr;
        while (cur != m_head)
        {
            del_node = cur;
            // del_node->next->prev = pre; 单向遍历删除就好，可以不用维护 prev
            cur = del_node->next;

            alloc_traits::destroy(m_node_alloc, del_node);
            alloc_traits::deallocate(m_node_alloc, del_node, 1);
        }

        m_head->next = m_head;
        m_head->prev = m_head;
        m_size = 0;
    }

    template <typename T, typename Allocator>
    inline typename list<T, Allocator>::iterator
    list<T, Allocator>::insert(const_iterator pos,
                               const_reference value)
    {
        Node *pre = pos.m_ptr->prev;
        Node *next = pos.m_ptr;

        Node *new_node = alloc_traits::allocate(m_node_alloc, 1);
        alloc_traits::construct(m_node_alloc, new_node, value);
        // 与前节点相连
        new_node->prev = pre;
        pre->next = new_node;
        // 与后节点相连
        new_node->next = next;
        next->prev = new_node;

        m_size++;

        return iterator(new_node);
    }

    template <typename T, typename Allocator>
    inline typename list<T, Allocator>::iterator
    list<T, Allocator>::insert(const_iterator pos, value_type &&value)
    {
        Node *pre = pos.m_ptr->prev;
        Node *next = pos.m_ptr;

        Node *new_node = alloc_traits::allocate(m_node_alloc, 1);
        alloc_traits::construct(m_node_alloc, new_node,
                                std::move(value));
        // 与前节点相连
        new_node->prev = pre;
        pre->next = new_node;
        // 与后节点相连
        new_node->next = next;
        next->prev = new_node;

        m_size++;

        return iterator(new_node);
    }

    template <typename T, typename Allocator>
    inline typename list<T, Allocator>::iterator
    list<T, Allocator>::insert(const_iterator pos,
                               size_type count, const_reference value)
    {
        if (count == 0)
            return iterator(pos.m_ptr);

        if (count + m_size > max_size())
            throw std::length_error("list::insert: count exceeds max_size");

        Node *pre = pos.m_ptr->prev;
        Node *next = pos.m_ptr;

        Node *new_node = nullptr;
        iterator ret(pos.m_ptr);

        for (size_type i = 0; i < count; i++)
        {
            new_node = alloc_traits::allocate(m_node_alloc, 1);
            alloc_traits::construct(m_node_alloc, new_node, value);
            new_node->prev = pre;
            pre->next = new_node;

            pre = pre->next;

            m_size++;

            if (i == 0)
                ret.m_ptr = new_node;
        }

        // 插入的最后一个节点与pos节点相连
        new_node->next = next;
        next->prev = new_node;

        return ret;
    }

    template <typename T, typename Allocator>
    template <typename InputIt,
              std::enable_if_t<!std::is_integral<T>::value, int>>
    typename list<T, Allocator>::iterator list<T, Allocator>::
        insert(const_iterator pos, InputIt first, InputIt last)
    {
        Node *pre = pos.m_ptr->prev;
        Node *next = pos.m_ptr;

        Node *new_node = nullptr;
        iterator ret(pos.m_ptr);
        bool is = true; // 标记第一个节点

        for (; first != last; first++)
        {
            new_node = alloc_traits::allocate(m_node_alloc, 1);
            alloc_traits::construct(m_node_alloc, new_node, *first);
            new_node->prev = pre;
            pre->next = new_node;

            pre = pre->next;

            m_size++;

            if (is)
            {
                ret.m_ptr = new_node;
                is = false;
            }
        }

        new_node->next = next;
        next->prev = new_node;

        return ret;
    }

    template <typename T, typename Allocator>
    inline typename list<T, Allocator>::iterator
    list<T, Allocator>::insert(const_iterator pos,
                               std::initializer_list<value_type> ilist)
    {
        if (ilist.size() == 0)
            return iterator(pos.m_ptr);

        Node *pre = pos.m_ptr->prev;
        Node *next = pos.m_ptr;

        Node *new_node = nullptr;
        iterator ret(pos.m_ptr);
        bool is = true;

        for (const_reference value : ilist)
        {
            new_node = alloc_traits::allocate(m_node_alloc, 1);
            alloc_traits::construct(m_node_alloc, new_node, value);
            new_node->prev = pre;
            pre->next = new_node;

            pre = pre->next;

            m_size++;

            if (is)
            {
                ret.m_ptr = new_node;
                is = false;
            }
        }

        new_node->next = next;
        next->prev = new_node;

        return ret;
    }

    template <typename T, typename Allocator>
    template <typename... Args>
    inline typename list<T, Allocator>::iterator
    list<T, Allocator>::emplace(const_iterator pos, Args &&...args)
    {

        Node *pre = pos.m_ptr->prev;
        Node *next = pos.m_ptr;

        Node *new_node = alloc_traits::allocate(m_node_alloc, 1);
        alloc_traits::construct(m_node_alloc, new_node,
                                std::forward<Args>(args)...);
        new_node->prev = pre;
        pre->next = new_node;
        new_node->next = next;
        next->prev = new_node;

        m_size++;

        return iterator(new_node);
    }

    template <typename T, typename Allocator>
    inline typename list<T, Allocator>::iterator
    list<T, Allocator>::erase(iterator pos)
    {
        return erase(const_iterator(pos));
    }

    template <typename T, typename Allocator>
    inline typename list<T, Allocator>::iterator
    list<T, Allocator>::erase(const_iterator pos)
    {
        if (pos == end())
            return iterator(pos.m_ptr);

        Node *pre = pos.m_ptr->prev;
        Node *next = pos.m_ptr->next;
        Node *del_node = pos.m_ptr;

        pre->next = next;
        next->prev = pre;

        alloc_traits::destroy(m_node_alloc, del_node);
        alloc_traits::deallocate(m_node_alloc, del_node, 1);

        m_size--;

        return iterator(next);
    }

    template <typename T, typename Allocator>
    inline typename list<T, Allocator>::iterator
    list<T, Allocator>::erase(iterator first, iterator last)
    {
        return erase(const_iterator(first), const_iterator(last));
    }

    template <typename T, typename Allocator>
    inline typename list<T, Allocator>::iterator
    list<T, Allocator>::erase(const_iterator first,
                              const_iterator last)
    {
        if (first == last)
            return iterator(first.m_ptr);

        Node *pre = first.m_ptr->prev;
        Node *next = last.m_ptr;
        Node *cur = first.m_ptr;
        Node *del_node = nullptr;

        pre->next = next;
        next->prev = pre;

        while (cur != last.m_ptr)
        {
            del_node = cur;
            cur = cur->next;

            alloc_traits::destroy(m_node_alloc, del_node);
            alloc_traits::deallocate(m_node_alloc, del_node, 1);

            m_size--;
        }

        return iterator(last.m_ptr);
    }

    template <typename T, typename Allocator>
    inline void list<T, Allocator>::push_back(const_reference value)
    {
        // push_back(value_type(value));
        empalce_back(value);
    }

    template <typename T, typename Allocator>
    inline void list<T, Allocator>::push_back(value_type &&value)
    {
        Node *pre = m_head->prev;
        Node *new_node = alloc_traits::allocate(m_node_alloc, 1);
        alloc_traits::construct(m_node_alloc, new_node,
                                std::move(value));

        pre->next = new_node;
        new_node->prev = pre;

        new_node->next = m_head;
        m_head->prev = new_node;

        m_size++;
    }

    template <typename T, typename Allocator>
    template <typename... Args>
    inline typename list<T, Allocator>::reference
    list<T, Allocator>::empalce_back(Args &&...args)
    {
        Node *pre = m_head->prev;
        Node *new_node = alloc_traits::allocate(m_node_alloc, 1);
        alloc_traits::construct(m_node_alloc, new_node,
                                std::forward<Args>(args)...);

        pre->next = new_node;
        new_node->prev = pre;

        new_node->next = m_head;
        m_head->prev = new_node;

        m_size++;

        return new_node->val;
    }

    template <typename T, typename Allocator>
    inline void list<T, Allocator>::pop_back()
    {
        if (empty())
            throw std::out_of_range("list::pop_back: list is empty");

        Node *del_node = m_head->prev;
        m_head->prev = del_node->prev;
        del_node->prev->next = m_head;

        alloc_traits::destroy(m_node_alloc, del_node);
        alloc_traits::deallocate(m_node_alloc, del_node, 1);

        m_size--;
    }

    template <typename T, typename Allocator>
    inline void list<T, Allocator>::push_front(const_reference value)
    {
        push_front(value_type(value));
    }

    template <typename T, typename Allocator>
    inline void list<T, Allocator>::push_front(value_type &&value)
    {
        Node *pre = m_head;
        Node *next = m_head->next;

        Node *new_node = alloc_traits::allocate(m_node_alloc, 1);
        alloc_traits::construct(m_node_alloc, new_node, std::move(value));

        pre->next = new_node;
        new_node->prev = pre;

        new_node->next = next;
        next->prev = new_node;

        m_size++;
    }

    template <typename T, typename Allocator>
    template <typename... Args>
    inline typename list<T, Allocator>::reference
    list<T, Allocator>::emplace_front(Args &&...args)
    {
        Node *pre = m_head;
        Node *next = m_head->next;

        Node *new_node = alloc_traits::allocate(m_node_alloc, 1);
        alloc_traits::construct(m_node_alloc, new_node,
                                std::forward<Args>(args)...);

        pre->next = new_node;
        new_node->prev = pre;

        new_node->next = next;
        next->prev = new_node;

        m_size++;

        return new_node->val;
    }

    template <typename T, typename Allocator>
    inline void list<T, Allocator>::pop_front()
    {
        if (empty())
            throw std::out_of_range("list::pop_front: list is empty");

        Node *pre = m_head;
        Node *del_node = m_head->next;

        pre->next = del_node->next;
        del_node->next->prev = pre;

        alloc_traits::destroy(m_node_alloc, del_node);
        alloc_traits::deallocate(m_node_alloc, del_node, 1);

        m_size--;
    }

    template <typename T, typename Allocator>
    inline void list<T, Allocator>::resize(size_type count)
    {
        if (m_size == count)
            return;
        else if (m_size < count)
        {
            Node *new_node = nullptr;
            Node *pre = m_head->prev;

            size_type diff = count - m_size; // 一定不要把 count-m_size 写在循环条件里

            for (size_type i = 0; i < diff; i++)
            {
                new_node = alloc_traits::allocate(m_node_alloc, 1);
                alloc_traits::construct(m_node_alloc, new_node);
                pre->next = new_node;
                new_node->prev = pre;

                pre = pre->next;
                m_size++;
            }

            new_node->next = m_head;
            m_head->prev = new_node;
        }
        else
        {
            size_type diff = m_size - count; // 一定不要把 m_size-count 写在循环条件里
            for (size_type i = 0; i < diff; i++)
                pop_back();
        }
    }
    template <typename T, typename Allocator>
    inline void list<T, Allocator>::resize(size_type count,
                                           const_reference value)
    {
        if (m_size == count)
            return;
        else if (m_size < count)
        {
            insert(end(), count - m_size, value);
        }
        else
        {
            size_type diff = m_size - count; // 一定不要把 m_size-count 写在循环条件里
            for (size_type i = 0; i < diff; i++)
                pop_back();
        }
    }

    template <typename T, typename Allocator>
    inline void list<T, Allocator>::swap(list<T, Allocator> &other) noexcept
    {
        using std::swap;
        swap(m_head, other.m_head);
        swap(m_node_alloc, other.m_node_alloc);
        swap(m_size, other.m_size);
    }

    // 操作
    template <typename T, typename Allocator>
    inline void list<T, Allocator>::merge(list<T, Allocator> &other)
    {
        merge(other, std::less<>());
    }

    template <typename T, typename Allocator>
    inline void list<T, Allocator>::merge(list<T, Allocator> &&other)
    {
        merge(other, std::less<>());
    }

    template <typename T, typename Allocator>
    template <typename Compare>
    inline void list<T, Allocator>::merge(list<T, Allocator> &other,
                                          Compare comp)
    {
        if (this == &other)
            return;

        if (other.empty())
            return;

        if (this->empty())
        {
            m_head->next = other.m_head->next;
            m_head->prev = other.m_head->prev;
            other.m_head->next->prev = m_head;
            other.m_head->prev->next = m_head;
            m_size = other.m_size;

            //  other 置空
            other.m_head->next = other.m_head;
            other.m_head->prev = other.m_head;
            other.m_size = 0;

            return;
        }

        // 默认从 other 合并到 this
        Node *cur = m_head;
        Node *cur_this = m_head->next;
        Node *cur_other = other.m_head->next;

        while (cur_this != m_head && cur_other != other.m_head)
        {
            if (comp(cur_this->val, cur_other->val))
            {
                cur = cur->next;
                cur_this = cur_this->next;
            }
            else
            {
                cur->next = cur_other;
                cur_other->prev = cur;
                Node *next = cur_other->next;

                // 将 cur_other 嵌入到 this 中
                cur_other->next = cur_this;
                cur_this->prev = cur_other;

                cur = cur->next;
                cur_other = next;
            }
        }

        if (cur_other != other.m_head)
        {
            cur->next = cur_other;
            cur_other->prev = cur;

            Node *last_node_other = other.m_head->prev;
            last_node_other->next = m_head;
            m_head->prev = last_node_other;
        }

        m_size += other.m_size;

        // other 恢复有效空态
        other.m_head->next = other.m_head;
        other.m_head->prev = other.m_head;
        other.m_size = 0;
    }

    template <typename T, typename Allocator>
    template <typename Compare>
    inline void list<T, Allocator>::merge(list<T, Allocator> &&other,
                                          Compare comp)
    {
        merge(other, comp);
    }

    template <typename T, typename Allocator>
    inline void list<T, Allocator>::splice(
        const_iterator pos,
        list<T, Allocator> &other)
    {
        if (other.empty() || this == &other)
            return;

        Node *pre = pos.m_ptr->prev;
        Node *next = pos.m_ptr;
        Node *cur = other.m_head->next;

        while (cur != other.m_head)
        {
            pre->next = cur;
            cur->prev = pre;

            pre = pre->next;
            cur = cur->next;
        }

        pre->next = next;
        next->prev = pre;

        m_size += other.m_size;

        // other置空
        other.m_head->next = other.m_head;
        other.m_head->prev = other.m_head;
        other.m_size = 0;
    }

    template <typename T, typename Allocator>
    inline void list<T, Allocator>::splice(
        const_iterator pos,
        list<T, Allocator> &&other)
    {
        splice(pos, other);
    }

    template <typename T, typename Allocator>
    inline void list<T, Allocator>::splice(
        const_iterator pos,
        list &other, const_iterator it)
    {
        if (it == other.end() || this == &other)
            return;

        Node *pre = pos.m_ptr->prev;
        Node *next = pos.m_ptr;

        // 将it从other中分离出来
        it.m_ptr->prev->next = it.m_ptr->next;
        it.m_ptr->next->prev = it.m_ptr->prev;

        pre->next = it.m_ptr;
        it.m_ptr->prev = pre;
        it.m_ptr->next = next;
        next->prev = it.m_ptr;

        m_size++;
        other.m_size--;
    }

    template <typename T, typename Allocator>
    inline void list<T, Allocator>::splice(
        const_iterator pos,
        list &&other, const_iterator it)
    {
        splice(pos, other, it);
    }

    template <typename T, typename Allocator>
    inline void list<T, Allocator>::splice(
        const_iterator pos, list &other,
        const_iterator first, const_iterator last)
    {
        if (other.empty() || this == &other)
            return;

        Node *pre = pos.m_ptr->prev;
        Node *next = pos.m_ptr;
        Node *pre_other = first.m_ptr->prev;

        while (first != last)
        {
            pre->next = first.m_ptr;
            first.m_ptr->prev = pre;

            pre = pre->next;
            first++;

            m_size++;
            other.m_size--;
        }

        pre->next = next;
        next->prev = pre;

        pre_other->next = last.m_ptr;
        last.m_ptr->prev = pre_other;
    }

    template <typename T, typename Allocator>
    inline void list<T, Allocator>::splice(
        const_iterator pos, list &&other,
        const_iterator first, const_iterator last)
    {
        splice(pos, other, first, last);
    }

    template <typename T, typename Allocator>
    inline typename list<T, Allocator>::size_type
    list<T, Allocator>::remove(const T &value)
    {
        return remove_if([&](const T &val)
                         { return val == value; });
    }

    template <typename T, typename Allocator>
    template <class UnaryPredicate>
    inline typename list<T, Allocator>::size_type
    list<T, Allocator>::remove_if(UnaryPredicate p)
    {
        if (empty())
            return 0;

        size_type count = 0;
        Node *pre = m_head;
        Node *cur = m_head->next;
        Node *next = nullptr;

        while (cur != m_head)
        {
            if (p(cur->val))
            {
                next = cur->next;

                pre->next = next;
                next->prev = pre;

                alloc_traits::destroy(m_node_alloc, cur);
                alloc_traits::deallocate(m_node_alloc, cur, 1);
                m_size--;
                count++;
                cur = next;
            }
            else
            {
                cur = cur->next;
                pre = pre->next;
            }
        }

        return count;
    }

    template <typename T, typename Allocator>
    inline void list<T, Allocator>::reverse()
    {
        if (empty())
            return;

        // 不能直接交换前哨节点的prev和next,虽然这样做是O(1)
        // 但是会破坏链表结构，导致迭代器的++变为了--

        Node *cur = m_head->next;
        Node *next = nullptr;

        m_head->prev = cur;
        while (cur != m_head)
        {
            next = cur->next;
            cur->next = cur->prev;
            cur->prev = next;

            if (next == m_head)
                m_head->next = cur;

            cur = next;
        }
    }

    template <typename T, typename Allocator>
    inline typename list<T, Allocator>::size_type
    list<T, Allocator>::unique()
    {
        return unique(std::equal_to<T>());
    }

    template <typename T, typename Allocator>
    template <class BinaryPredicate>
    inline typename list<T, Allocator>::size_type
    list<T, Allocator>::unique(BinaryPredicate p)
    {
        if (empty())
            return 0;

        size_type count = 0;
        Node *pre = m_head->next;
        Node *cur = pre->next;

        while (cur != m_head)
        {
            if (p(cur->val, pre->val))
            {
                Node *next = cur->next;

                pre->next = next;
                next->prev = pre;

                alloc_traits::destroy(m_node_alloc, cur);
                alloc_traits::deallocate(m_node_alloc, cur, 1);
                m_size--;
                count++;
                cur = next;
            }
            else
            {
                pre = pre->next;
                cur = cur->next;
            }
        }

        return count;
    }

    template <typename T, typename Allocator>
    inline void list<T, Allocator>::sort()
    {
        sort(std::less<>());
    }

    template <typename T, typename Allocator>
    template <class Compare>
    inline void list<T, Allocator>::sort(Compare comp)
    {
        if (empty())
            return;

        Node *head = m_head->next;
        Node *cur = nullptr;
        Node *left = nullptr;
        Node *right = nullptr;

        Node *temp = alloc_traits::allocate(m_node_alloc, 1); // 临时前哨节点
        alloc_traits::construct(m_node_alloc, temp);

        for (size_type i = 1; i < m_size; i *= 2)
        {
            cur = head;
            Node *tail = temp;

            while (cur != m_head)
            {
                left = cur;
                right = split(left, i);
                cur = split(right, i);
                tail->next = merge_tow(left, right, comp);
                while (tail->next != m_head)
                    tail = tail->next;
            }

            head = temp->next;
        }

        m_head->next = head;

        // 排序最后，统一修复所有 prev 指针
        Node *p = m_head->next;
        p->prev = m_head;
        while (p != m_head)
        {
            p->next->prev = p;
            p = p->next;
        }

        alloc_traits::destroy(m_node_alloc, temp);
        alloc_traits::deallocate(m_node_alloc, temp, 1);
    }

    template <typename T, typename Allocator>
    inline typename list<T, Allocator>::Node *
    list<T, Allocator>::split(Node *head, size_type len)
    {
        if (head == m_head)
            return head;

        Node *cur = head;
        // 向前走 n-1 步（拆分出前 n 个节点）
        for (size_type i = 1; i < len && cur->next != m_head; i++)
            cur = cur->next;

        Node *next_part = cur->next;
        cur->next = m_head;

        return next_part;
    }

    template <typename T, typename Allocator>
    inline typename list<T, Allocator>::Node *
    list<T, Allocator>::merge_tow(Node *la, Node *lb)
    {
        return merge_tow(la, lb, std::less<T>());
    }

    template <typename T, typename Allocator>
    template <typename Compare>
    inline typename list<T, Allocator>::Node *
    list<T, Allocator>::merge_tow(Node *la, Node *lb,
                                  Compare comp)
    {
        if (la == m_head)
            return lb;
        if (lb == m_head)
            return la;
        if (la == lb)
            return la;

        Node *cur_a = la;
        Node *cur_b = lb;
        Node *temp = alloc_traits::allocate(m_node_alloc, 1); // 创建一个临时前哨节点用于合并
        alloc_traits::construct(m_node_alloc, temp);
        temp->next = la;
        Node *pre = temp;

        while (cur_a != m_head && cur_b != m_head)
        {
            if (comp(cur_a->val, cur_b->val))
            {
                pre = cur_a;
                cur_a = cur_a->next;
            }
            else
            {
                Node *next = cur_b->next;
                pre->next = cur_b;
                cur_b->next = cur_a;

                pre = pre->next;
                cur_b = next;
            }
        }

        if (cur_b != m_head)
            pre->next = cur_b;

        Node *res = temp->next;
        alloc_traits::destroy(m_node_alloc, temp);
        alloc_traits::deallocate(m_node_alloc, temp, 1);

        return res;
    }

    template <typename T, typename Allocator>
    bool operator==(const list<T, Allocator> &lhs,
                    const list<T, Allocator> &rhs)
    {
        if (lhs.size() != rhs.size())
            return false;

        typename list<T, Allocator>::Node *cur_a = lhs.m_head->next;
        typename list<T, Allocator>::Node *cur_b = rhs.m_head->next;

        for (; cur_a != lhs.m_head && cur_b != rhs.m_head;
             cur_a = cur_a->next, cur_b = cur_b->next)
        {
            if (cur_a->val != cur_b->val)
                return false;
        }

        return true;
    }

    template <typename T, typename Allocator>
    bool operator!=(const list<T, Allocator> &lhs,
                    const list<T, Allocator> &rhs)
    {
        return !(lhs == rhs);
    }

} // namespace demo
