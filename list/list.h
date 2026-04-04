#pragma once

#include <type_traits>
#include <iterator>
#include <memory>
#include <initializer_list>

namespace demo
{
    template <typename T, typename Allocator = std::allocator<T>>
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
        friend bool operator== <>(const list<T, Allocator> &lhs,
                                  const list<T, Allocator> &rhs);
        friend bool operator!= <>(const list<T, Allocator> &lhs,
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
            pointer prev;
            pointer next;

            template <typename... Args>
            Node(Args &&...args) : val(std::forward<Args>(args)...),
                                   prev(nullptr), next(nullptr) {}
        };

    public:
        class const_iterator;
        class iterator
        {
            friend class list;

        public:
            using iterator_category = std::bidirectional_iterator_tag;
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
            bool operator==(const iterator other) const noexcept;
            bool operator!=(const iterator other) const noexcept;
            bool operator==(const const_iterator other) const noexcept;
            bool operator!=(const const_iterator other) const noexcept;

        private:
            Node *m_ptr;
        };

        class const_iterator
        {
        public:
            using iterator_category = std::bidirectional_iterator_tag;
            using value_type = T;
            using pointer = const T *;
            using reference = const T &;
            using difference_type = std::ptrdiff_t;

            const_iterator() noexcept;
            explicit const_iterator(Node *ptr) noexcept;
            explicit const_iterator(iterator it) noexcept;
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
            bool operator==(const const_iterator other) const noexcept;
            bool operator!=(const const_iterator other) const noexcept;
            bool operator==(const iterator other) const noexcept;
            bool operator!=(const iterator other) const noexcept;

        private:
            Node *m_ptr;
        };

        // 使用标准库的反向迭代器适配器
        using reverse_iterator = std::reverse_iterator<iterator>;
        using const_reverse_iterator = std::reverse_iterator<const_iterator>;

    public:
        explicit list();
        explicit list(size_type count, value_type value = value_type());
        list(std::initializer_list<value_type> ilist);
        template <typename InputIt>
        list(InputIt first, InputIt last);
        list(const list<T, Allocator> &other);
        list<T, Allocator> &operator=(const list<T, Allocator> &other);
        list(list<T, Allocator> &&other) noexcept;
        list<T, Allocator> &operator=(list<T, Allocator> &&other) noexcept;
        ~list();

        void assign(size_type count, const_reference value);
        void assign(std::initializer_list<value_type> ilist);
        template <typename InputIt>
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
        size_type max() const;

        // 修改器
        void clear();
        iterator insert(const_iterator pos, const_reference value);
        iterator insert(const_iterator pos, value_type &&value);
        iterator insert(const_iterator pos, size_type count, const_reference value);
        template <typename InputIt>
        iterator insert(const_iterator pos, InputIt first, InputIt last);
        iterator insert(const_iterator pos, std::initializer_list<value_type> ilist);
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
        using node_allocator = typename std::allocator_traits<
            Allocator>::template rebind_alloc<Node>;

        Node *m_head;
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
        // c++标准库判空吗？？
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
        if (m_ptr != nullptr)
            m_ptr = m_ptr->next;
        return *this;
    }

    template <typename T, typename Allocator>
    inline typename list<T, Allocator>::iterator list<T, Allocator>::
        iterator::operator++(int) noexcept
    {
        iterator temp(m_ptr);
        if (m_ptr != nullptr)
            m_ptr = m_ptr->next;
        return temp;
    }

    template <typename T, typename Allocator>
    inline typename list<T, Allocator>::iterator &
    list<T, Allocator>::iterator::operator--() noexcept
    {
        if (m_ptr != nullptr)
            m_ptr = m_ptr->prev;
        return *this;
    }
    template <typename T, typename Allocator>
    inline typename list<T, Allocator>::iterator
    list<T, Allocator>::iterator::operator--(int) noexcept
    {
        iterator temp(m_ptr);
        if (m_ptr != nullptr)
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
        if (m_ptr != nullptr)
            return m_ptr->val;
    }

    template <typename T, typename Allocator>
    inline typename list<T, Allocator>::const_iterator::pointer
    list<T, Allocator>::const_iterator::operator->() const noexcept
    {
        if (m_ptr != nullptr)
            return &m_ptr->val;
    }

    // 自增/自减
    template <typename T, typename Allocator>
    inline typename list<T, Allocator>::const_iterator &
    list<T, Allocator>::const_iterator::operator++() noexcept
    {
        if (m_ptr != nullptr)
            m_ptr = m_ptr->next;
        return *this;
    }

    template <typename T, typename Allocator>
    inline typename list<T, Allocator>::const_iterator
    list<T, Allocator>::const_iterator::operator++(int) noexcept
    {
        const_iterator temp(m_ptr);
        if (m_ptr != nullptr)
            m_ptr = m_ptr->next;
        return temp;
    }

    template <typename T, typename Allocator>
    inline typename list<T, Allocator>::const_iterator &
    list<T, Allocator>::const_iterator::operator--() noexcept
    {
        if (m_ptr != nullptr)
            m_ptr = m_ptr->prev;
        return *this;
    }

    template <typename T, typename Allocator>
    inline typename list<T, Allocator>::const_iterator
    list<T, Allocator>::const_iterator::operator--(int) noexcept
    {
        const_iterator temp(m_ptr);
        if (m_ptr != nullptr)
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
        : m_head(nullptr), m_node_alloc()
    {
        m_head = alloc_traits::allocate(m_node_alloc, 1);
        alloc_traits::construct(m_node_alloc, m_head);
        m_head->prev = m_head;
        m_head->next = m_head;
    }

    template <typename T, typename Allocator>
    inline list<T, Allocator>::list(size_type count, value_type value)
        : m_head(nullptr), m_node_alloc()
    {
        if (count > max_size())
            throw std::length_error("list count exceeds max_size");

        m_head = alloc_traits::allocate(m_node_alloc, 1);
        alloc_traits::construct(m_node_alloc, m_head);

        Node *cur = m_head;
        Node *pre = m_head;
        for (size_type i = 0; i < count; ++i)
        {
            cur->next = alloc_traits::allocate(m_node_alloc, 1);
            alloc_traits::construct(m_node_alloc, cur->next, value);
            pre = cur;
            cur = cur->next;
            cur->prev = pre;
        }

        // 最后一个节点与头节点相连成环
        cur->next = m_head;
        m_head->pre = cur;
    }

    template <typename T, typename Allocator>
    inline list<T, Allocator>::list(std::initializer_list<value_type> ilist)
        : m_head(nullptr), m_node_alloc()
    {
        m_head = alloc_traits::allocate(m_node_alloc, 1);
        alloc_traits::construct(m_node_alloc, m_head);
        assign(ilist);
    }

    template <typename T, typename Allocator>
    template <typename InputIt>
    inline list<T, Allocator>::list(InputIt first, InputIt last)
        : m_head(nullptr), m_node_alloc()
    {
        m_head = alloc_traits::allocate(m_node_alloc, 1);
        alloc_traits::construct(m_node_alloc, m_head);
        assign(first, last);
    }

    template <typename T, typename Allocator>
    inline list<T, Allocator>::list(const list<T, Allocator> &other)
        : m_head(nullptr), m_node_alloc()
    {
        m_head = alloc_traits::allocate(m_node_alloc, 1);
        alloc_traits::construct(m_node_alloc, m_head);
        m_head->next = nullptr;
        m_head->prev = nullptr;

        Node *cur = m_head;
        Node *pre = m_head;
        for (Node *i = other.m_head->next; i != other.m_head; i = i->next)
        {
            cur->next = alloc_traits::allocate(m_node_alloc, 1);
            alloc_traits::construct(m_node_alloc, cur->next, i->val);
            pre = cur;
            cur = cur->next;
            cur->prev = pre;
        }

        // 最后一个节点与头节点相连成环
        cur->next = m_head;
        m_head->pre = cur;
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
            }

            // 最后一个节点与头节点相连成环
            cur->next = m_head;
            m_head->pre = cur;
        }
        return *this;
    }

    template <typename T, typename Allocator>
    inline list<T, Allocator>::list(list<T, Allocator> &&other) noexcept
        : m_head(nullptr), m_node_alloc()
    {
        m_head = other.m_head;

        other.m_head = alloc_traits::allocate(m_node_alloc, 1);
        alloc_traits::construct(m_node_alloc, other.m_head);
    }
    template <typename T, typename Allocator>
    inline list<T, Allocator> &list<T, Allocator>::
    operator=(list<T, Allocator> &&other) noexcept
    {
        if (this != &other)
        {
            clear();

            m_head = other.m_head;

            other.m_head = alloc_traits::allocate(m_node_alloc, 1);
            alloc_traits::construct(m_node_alloc, other.m_head);
        }
        return *this;
    }

    template <typename T, typename Allocator>
    inline list<T, Allocator>::~list()
    {
        clear();
        if (m_head != nullptr)
            delete m_head;
        m_head = nullptr;
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
        for (int i = 0; i < count; i++)
        {
            cur->next = alloc_traits::allocate(m_node_alloc, 1);
            alloc_traits::construct(m_node_alloc, cur->next, value);
            pre = cur;
            cur = cur->next;
            cur->prev = pre;
        }

        // 最后一个节点与头节点相连成环
        cur->next = m_head;
        m_head->pre = cur;
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
        }

        // 最后一个节点与头节点相连成环
        cur->next = m_head;
        m_head->pre = cur;
    }

    template <typename T, typename Allocator>
    template <typename InputIt>
    inline void list<T, Allocator>::assign(InputIt first, InputIt last)
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
        }

        // 最后一个节点与头节点相连成环
        cur->next = m_head;
        m_head->pre = cur;
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
        return iterator(m_head);
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
        return reverse_iterator(end());
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
        return reverse_iterator(begin());
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
        size_type count = 0;
        Node *cur = m_head->next;
        while (cur != m_head)
        {
            count++;
            cur = cur->next;
        }
        return count;
    }

    template <typename T, typename Allocator>
    inline typename list<T, Allocator>::size_type
    list<T, Allocator>::max() const
    {
        return alloc_traits::max_size(m_node_alloc);
    }

    template <typename T, typename Allocator>
    inline void list<T, Allocator>::clear()
    {
        Node *pre = m_head;
        Node *cur = m_head->next;
        Node *del_node = nullptr;
        while (cur != m_head)
        {
            del_node = cur;
            pre->next = cur->next;
            cur->next = del_node->next;
            del_node->next->prev = pre;

            alloc_traits::destroy(m_node_alloc, del_node);
            alloc_traits::deallocate(m_node_alloc, del_node, 1);
        }

        m_head->next = m_head;
        m_head->prev = m_head;
    }

} // namespace demo
