#pragma once

#include <type_traits>
#include <iterator>
#include <initializer_list>
#include <memory>
#include <functional>

namespace demo
{
    template <typename T, typename Allocator = std::allocator<T>>
    class forward_list;
    template <typename T, typename Allocator>
    bool operator==(const forward_list<T, Allocator> &lhs,
                    const forward_list<T, Allocator> &rhs);
    template <typename T, typename Allocator>
    bool operator!=(const forward_list<T, Allocator> &lhs,
                    const forward_list<T, Allocator> &rhs);

    template <typename T, typename Allocator>
    class forward_list
    {
    public:
        using value_type = T;
        using pointer = T *;
        using const_pointer = const T *;
        using reference = T &;
        using const_reference = const T &;
        using allocator_type = Allocator;
        using size_type = std::size_t;
        using difference_type = std::ptrdiff_t;

    private:
        struct Node
        {
            value_type value;
            Node *next;

            template <typename... Args>
            Node(Args &&...args) : value(std::forward<Args>(args)...), next(nullptr) {}
            ~Node() = default;
        };

    public:
        class const_iterator;
        class iterator
        {
        public:
            using iterator_category = std::forward_iterator_tag;
            using value_type = T;
            using difference_type = std::ptrdiff_t;
            using pointer = T *;
            using reference = T &;

            iterator() noexcept;
            explicit iterator(Node *ptr) noexcept;
            iterator(const iterator &other) noexcept;

            // 解引用操作
            reference operator*() const noexcept;
            pointer operator->() const noexcept;

            // 递增（前向迭代器不支持递减）
            iterator &operator++() noexcept;
            iterator operator++(int) noexcept;

            // 比较
            bool operator==(const iterator &other) const noexcept;
            bool operator!=(const iterator &other) const noexcept;

        private:
            Node *m_ptr;

            friend class forward_list; // 便于直接访问 m_ptr
        };

        class const_iterator
        {
        public:
            using iterator_category = std::forward_iterator_tag;
            using value_type = const T;
            using difference_type = std::ptrdiff_t;
            using pointer = const T *;
            using reference = const T &;

            const_iterator() noexcept;
            const_iterator(const iterator &it) noexcept;
            explicit const_iterator(Node *ptr) noexcept;
            const_iterator(const const_iterator &other) noexcept;

            // 解引用
            reference operator*() const noexcept;
            pointer operator->() const noexcept;

            // 自增
            const_iterator &operator++() noexcept;
            const_iterator operator++(int) noexcept;

            // 比较
            bool operator==(const const_iterator &other) const noexcept;
            bool operator==(const iterator &other) const noexcept;
            bool operator!=(const const_iterator &other) const noexcept;
            bool operator!=(const iterator &other) const noexcept;

        private:
            Node *m_ptr;

            friend class forward_list; // 便于直接访问 m_ptr
        };

        // 构造
        forward_list();
        explicit forward_list(size_type count);
        forward_list(size_type count, const_reference val);
        template <typename InputIt>
        forward_list(InputIt first, InputIt last);
        forward_list(std::initializer_list<value_type> ilist);
        forward_list(const forward_list &other);
        forward_list &operator=(const forward_list &other);
        forward_list &operator=(std::initializer_list<value_type> ilist);
        forward_list(forward_list &&other) noexcept;
        forward_list &operator=(forward_list &&other) noexcept;
        ~forward_list();

        void assign(size_type count, const_reference val);
        template <typename InputIt>
        void assign(InputIt first, InputIt last);
        void assign(std::initializer_list<value_type> ilist);

        Allocator get_allocator() const;

        // 迭代器
        iterator before_begin() noexcept;
        const_iterator before_begin() const noexcept;
        const_iterator cbefore_begin() const noexcept;
        iterator begin() noexcept;
        const_iterator begin() const noexcept;
        const_iterator cbegin() const noexcept;
        iterator end() noexcept;
        const_iterator end() const noexcept;
        const_iterator cend() const noexcept;

        bool empty() const noexcept;
        size_type max_size() const noexcept;

        // 修改器
        void clear() noexcept;
        iterator insert_after(const_iterator pos, const_reference val);
        iterator insert_after(const_iterator pos, value_type &&val);
        iterator insert_after(const_iterator pos, size_type count,
                              const_reference value);
        template <class InputIt>
        iterator insert_after(const_iterator pos, InputIt first, InputIt last);
        iterator insert_after(const_iterator pos, std::initializer_list<T> ilist);
        template <typename... Args>
        iterator emplace_after(const_iterator pos, Args &&...args);
        iterator erase_after(const_iterator pos);
        iterator erase_after(const_iterator first, const_iterator last);
        void push_front(const_reference val);
        void push_front(value_type &&val);
        template <typename... Args>
        reference emplace_front(Args &&...args);
        void pop_front();
        void resize(size_type count, const_reference val = value_type());
        void swap(forward_list &other) noexcept;

        // 操作
        void merge(forward_list &other);
        void merge(forward_list &&other);
        template <typename Compare>
        void merge(forward_list &other, Compare comp);
        template <typename Compare>
        void merge(forward_list &&other, Compare comp);
        void splice_after(const_iterator pos, forward_list &other);
        void splice_after(const_iterator pos, forward_list &&other);
        void splice_after(const_iterator pos, forward_list &other,
                          const_iterator it);
        void splice_after(const_iterator pos, forward_list &&other,
                          const_iterator it);
        void splice_after(const_iterator pos, forward_list &other,
                          const_iterator first, const_iterator last);
        void splice_after(const_iterator pos, forward_list &&other,
                          const_iterator first, const_iterator last);
        size_type remove(const T &value);
        template <typename UnaryPredicate>
        size_type remove_if(UnaryPredicate p);
        void reverse() noexcept;
        size_type unique();
        template <typename BinaryPredicate>
        size_type unique(BinaryPredicate p);
        void sort();
        template <typename Compare>
        void sort(Compare comp);

    private:
        // 将链表按指定长度拆分，返回拆分后第二部分链表的头节点
        Node *split(Node *head, size_type n);
        Node *merge_two(Node *la, Node *lb);
        template <typename Compare>
        Node *merge_two(Node *la, Node *lb, Compare comp);

    private:
        // 节点分配器（rebind 到 Node）
        using node_allocator = typename std::allocator_traits<Allocator>::
            template rebind_alloc<Node>;
        node_allocator m_node_alloc;
        Node *m_head; // 前哨节点（不存储数据）

        using alloc_traits = std::allocator_traits<node_allocator>;
        friend bool operator== <>(const forward_list<T, Allocator> &lhs,
                                  const forward_list<T, Allocator> &rhs);
        friend bool operator!= <>(const forward_list<T, Allocator> &lhs,
                                  const forward_list<T, Allocator> &rhs);
    };

    // ------------------------------ iterator 实现 ------------------------------
    template <typename T, typename Allocator>
    inline forward_list<T, Allocator>::iterator::iterator() noexcept
        : m_ptr(nullptr)
    {
    }

    template <typename T, typename Allocator>
    inline forward_list<T, Allocator>::iterator::iterator(Node *ptr) noexcept
        : m_ptr(ptr)
    {
    }

    template <typename T, typename Allocator>
    inline forward_list<T, Allocator>::iterator::iterator(const iterator &other) noexcept
        : m_ptr(other.m_ptr)
    {
    }

    template <typename T, typename Allocator>
    inline typename forward_list<T, Allocator>::iterator::reference
    forward_list<T, Allocator>::iterator::operator*() const noexcept
    {
        return m_ptr->value;
    }
    template <typename T, typename Allocator>
    inline typename forward_list<T, Allocator>::iterator::pointer
    forward_list<T, Allocator>::iterator::operator->() const noexcept
    {
        return &m_ptr->value;
    }

    template <typename T, typename Allocator>
    inline typename forward_list<T, Allocator>::iterator &forward_list<T, Allocator>::iterator::
    operator++() noexcept
    {
        if (m_ptr != nullptr)
            m_ptr = m_ptr->next;
        return *this;
    }

    template <typename T, typename Allocator>
    inline typename forward_list<T, Allocator>::iterator forward_list<T, Allocator>::iterator::
    operator++(int) noexcept
    {
        iterator temp(m_ptr);
        if (m_ptr != nullptr)
            m_ptr = m_ptr->next;

        return temp;
    }

    template <typename T, typename Allocator>
    inline bool forward_list<T, Allocator>::iterator::operator==(const iterator &other) const noexcept
    {
        return m_ptr == other.m_ptr;
    }
    template <typename T, typename Allocator>
    inline bool forward_list<T, Allocator>::iterator::operator!=(const iterator &other) const noexcept
    {
        return m_ptr != other.m_ptr;
    }

    // ------------------------------ const_iterator 实现 ------------------------------
    template <typename T, typename Allocator>
    inline forward_list<T, Allocator>::const_iterator::const_iterator() noexcept
        : m_ptr(nullptr)
    {
    }

    template <typename T, typename Allocator>
    inline forward_list<T, Allocator>::const_iterator::const_iterator(const iterator &it) noexcept
        : m_ptr(it.m_ptr)
    {
    }

    template <typename T, typename Allocator>
    inline forward_list<T, Allocator>::const_iterator::const_iterator(Node *ptr) noexcept
        : m_ptr(ptr)
    {
    }

    template <typename T, typename Allocator>
    inline forward_list<T, Allocator>::const_iterator::const_iterator(const const_iterator &other) noexcept
        : m_ptr(other.m_ptr)
    {
    }

    template <typename T, typename Allocator>
    inline typename forward_list<T, Allocator>::const_iterator::reference forward_list<T, Allocator>::
        const_iterator::operator*() const noexcept
    {
        return m_ptr->value;
    }

    template <typename T, typename Allocator>
    inline typename forward_list<T, Allocator>::const_iterator::pointer forward_list<T, Allocator>::
        const_iterator::operator->() const noexcept
    {
        return &m_ptr->value;
    }

    template <typename T, typename Allocator>
    inline typename forward_list<T, Allocator>::const_iterator &forward_list<T, Allocator>::
        const_iterator::operator++() noexcept
    {
        if (m_ptr != nullptr)
            m_ptr = m_ptr->next;
        return *this;
    }

    template <typename T, typename Allocator>
    inline typename forward_list<T, Allocator>::const_iterator forward_list<T, Allocator>::
        const_iterator::operator++(int) noexcept
    {
        const_iterator temp(m_ptr);
        if (m_ptr != nullptr)
            m_ptr = m_ptr->next;

        return temp;
    }

    template <typename T, typename Allocator>
    inline bool forward_list<T, Allocator>::
        const_iterator::operator==(const const_iterator &other) const noexcept
    {
        return m_ptr == other.m_ptr;
    }

    template <typename T, typename Allocator>
    inline bool forward_list<T, Allocator>::
        const_iterator::operator==(const iterator &other) const noexcept
    {
        return m_ptr == other.m_ptr;
    }

    template <typename T, typename Allocator>
    inline bool forward_list<T, Allocator>::
        const_iterator::operator!=(const const_iterator &other) const noexcept
    {
        return m_ptr != other.m_ptr;
    }
    template <typename T, typename Allocator>
    inline bool forward_list<T, Allocator>::
        const_iterator::operator!=(const iterator &other) const noexcept
    {
        return m_ptr != other.m_ptr;
    }

    // ------------------------------ forward_list 实现 ------------------------------

    template <typename T, typename Allocator>
    inline forward_list<T, Allocator>::forward_list()
    {
        m_head = alloc_traits::allocate(m_node_alloc, 1);
        alloc_traits::construct(m_node_alloc, m_head);
    }

    template <typename T, typename Allocator>
    inline forward_list<T, Allocator>::forward_list(size_type count)
    {
        if (count > max_size())
        {
            throw std::length_error("forward_list count exceeds max_size");
        }

        m_head = alloc_traits::allocate(m_node_alloc, 1);
        alloc_traits::construct(m_node_alloc, m_head);

        Node *cur = m_head;
        for (size_type i = 0; i < count; i++)
        {
            cur->next = alloc_traits::allocate(m_node_alloc, 1);
            alloc_traits::construct(m_node_alloc, cur->next);
            cur = cur->next;
        }
    }

    template <typename T, typename Allocator>
    inline forward_list<T, Allocator>::forward_list(size_type count, const_reference val)
    {
        if (count > max_size())
        {
            throw std::length_error("forward_list count exceeds max_size");
        }

        m_head = alloc_traits::allocate(m_node_alloc, 1);
        alloc_traits::construct(m_node_alloc, m_head);

        Node *cur = m_head;
        for (size_type i = 0; i < count; i++)
        {
            cur->next = alloc_traits::allocate(m_node_alloc, 1);
            alloc_traits::construct(m_node_alloc, cur->next, val);
            cur = cur->next;
        }
    }

    template <typename T, typename Allocator>
    template <typename InputIt>
    inline forward_list<T, Allocator>::forward_list(InputIt first, InputIt last)
    {
        m_head = alloc_traits::allocate(m_node_alloc, 1);
        alloc_traits::construct(m_node_alloc, m_head);

        Node *cur = m_head;
        for (; first != last; first++)
        {
            cur->next = alloc_traits::allocate(m_node_alloc, 1);
            alloc_traits::construct(m_node_alloc, cur->next, *first);
            cur = cur->next;
        }
    }

    template <typename T, typename Allocator>
    inline forward_list<T, Allocator>::forward_list(std::initializer_list<value_type> ilist)
    {
        m_head = alloc_traits::allocate(m_node_alloc, 1);
        alloc_traits::construct(m_node_alloc, m_head);

        Node *cur = m_head;
        for (auto &val : ilist)
        {
            cur->next = alloc_traits::allocate(m_node_alloc, 1);
            alloc_traits::construct(m_node_alloc, cur->next, val);
            cur = cur->next;
        }
    }

    template <typename T, typename Allocator>
    inline forward_list<T, Allocator>::forward_list(const forward_list &other)
    {
        m_head = alloc_traits::allocate(m_node_alloc, 1);
        alloc_traits::construct(m_node_alloc, m_head);

        Node *cur = m_head;
        for (Node *i = other.m_head->next; i != nullptr; i = i->next)
        {
            cur->next = alloc_traits::allocate(m_node_alloc, 1);
            alloc_traits::construct(m_node_alloc, cur->next, i->value);
            cur = cur->next;
        }
    }

    template <typename T, typename Allocator>
    inline forward_list<T, Allocator> &forward_list<T, Allocator>::
    operator=(const forward_list &other)
    {
        if (this == &other)
            return *this;

        // 释放原对象的所有节点
        clear();

        Node *cur = m_head;
        for (Node *i = other.m_head->next; i != nullptr; i = i->next)
        {
            cur->next = alloc_traits::allocate(m_node_alloc, 1);
            alloc_traits::construct(m_node_alloc, cur->next, i->value);
            cur = cur->next;
        }

        return *this;
    }

    template <typename T, typename Allocator>
    inline forward_list<T, Allocator> &forward_list<T, Allocator>::
    operator=(std::initializer_list<value_type> ilist)
    {
        // 释放原对象的所有节点
        clear();

        Node *cur = m_head;
        for (auto &val : ilist)
        {
            cur->next = alloc_traits::allocate(m_node_alloc, 1);
            alloc_traits::construct(m_node_alloc, cur->next, val);
            cur = cur->next;
        }

        return *this;
    }

    template <typename T, typename Allocator>
    inline forward_list<T, Allocator>::forward_list(forward_list &&other) noexcept
        : m_node_alloc(std::move(other.m_node_alloc))
    {
        m_head = other.m_head;

        // 为原对象重新创建一个前哨节点
        other.m_head = alloc_traits::allocate(m_node_alloc, 1);
        alloc_traits::construct(m_node_alloc, other.m_head);
    }

    template <typename T, typename Allocator>
    inline forward_list<T, Allocator> &forward_list<T, Allocator>::
    operator=(forward_list &&other) noexcept
    {
        if (this == &other)
            return *this;

        clear();

        m_node_alloc = std::move(other.m_node_alloc);
        m_head = other.m_head;

        // 为原对象重新创建一个前哨节点
        other.m_head = alloc_traits::allocate(m_node_alloc, 1);
        alloc_traits::construct(m_node_alloc, other.m_head);

        return *this;
    }

    template <typename T, typename Allocator>
    inline forward_list<T, Allocator>::~forward_list()
    {
        clear();
        if (m_head != nullptr)
        {
            alloc_traits::destroy(m_node_alloc, m_head);
            alloc_traits::deallocate(m_node_alloc, m_head, 1);
            m_head = nullptr;
        }
    }

    template <typename T, typename Allocator>
    inline void forward_list<T, Allocator>::assign(size_type count, const_reference val)
    {
        clear();

        Node *cur = m_head;
        for (size_type i = 0; i < count; i++)
        {
            cur->next = alloc_traits::allocate(m_node_alloc, 1);
            alloc_traits::construct(m_node_alloc, cur->next, val);
            cur = cur->next;
        }
    }

    template <typename T, typename Allocator>
    template <typename InputIt>
    inline void forward_list<T, Allocator>::assign(InputIt first, InputIt last)
    {
        clear();

        Node *cur = m_head;
        for (; first != last; first++)
        {
            cur->next = alloc_traits::allocate(m_node_alloc, 1);
            alloc_traits::construct(m_node_alloc, cur->next, *first);
            cur = cur->next;
        }
    }

    template <typename T, typename Allocator>
    inline void forward_list<T, Allocator>::assign(std::initializer_list<value_type> ilist)
    {
        clear();

        Node *cur = m_head;
        for (auto &val : ilist)
        {
            cur->next = alloc_traits::allocate(m_node_alloc, 1);
            alloc_traits::construct(m_node_alloc, cur->next, val);
            cur = cur->next;
        }
    }

    template <typename T, typename Allocator>
    inline Allocator forward_list<T, Allocator>::get_allocator() const
    {
        using elem_allocator = typename std::allocator_traits<node_allocator>::
            template rebind_alloc<T>;
        return elem_allocator(m_node_alloc);
    }

    template <typename T, typename Allocator>
    inline typename forward_list<T, Allocator>::iterator forward_list<T, Allocator>::
        before_begin() noexcept
    {
        return iterator(m_head);
    }

    template <typename T, typename Allocator>
    inline typename forward_list<T, Allocator>::const_iterator forward_list<T, Allocator>::
        before_begin() const noexcept
    {
        return const_iterator(m_head);
    }

    template <typename T, typename Allocator>
    inline typename forward_list<T, Allocator>::const_iterator forward_list<T, Allocator>::
        cbefore_begin() const noexcept
    {
        return const_iterator(m_head);
    }

    template <typename T, typename Allocator>
    inline typename forward_list<T, Allocator>::iterator forward_list<T, Allocator>::
        begin() noexcept
    {
        return iterator(m_head->next);
    }

    template <typename T, typename Allocator>
    inline typename forward_list<T, Allocator>::const_iterator forward_list<T, Allocator>::
        begin() const noexcept
    {
        return const_iterator(m_head->next);
    }

    template <typename T, typename Allocator>
    inline typename forward_list<T, Allocator>::const_iterator forward_list<T, Allocator>::
        cbegin() const noexcept
    {
        return const_iterator(m_head->next);
    }

    template <typename T, typename Allocator>
    inline typename forward_list<T, Allocator>::iterator forward_list<T, Allocator>::
        end() noexcept
    {
        return iterator(nullptr);
    }

    template <typename T, typename Allocator>
    inline typename forward_list<T, Allocator>::const_iterator forward_list<T, Allocator>::
        end() const noexcept
    {
        return const_iterator(nullptr);
    }

    template <typename T, typename Allocator>
    inline typename forward_list<T, Allocator>::const_iterator forward_list<T, Allocator>::
        cend() const noexcept
    {
        return const_iterator(nullptr);
    }

    template <typename T, typename Allocator>
    inline bool forward_list<T, Allocator>::empty() const noexcept
    {
        return m_head->next == nullptr;
    }

    template <typename T, typename Allocator>
    inline typename forward_list<T, Allocator>::size_type forward_list<T, Allocator>::
        max_size() const noexcept
    {
        return alloc_traits::max_size(m_node_alloc);
    }

    template <typename T, typename Allocator>
    inline void forward_list<T, Allocator>::clear() noexcept
    {
        Node *cur = m_head->next;
        while (cur != nullptr)
        {
            Node *next = cur->next;
            alloc_traits::destroy(m_node_alloc, cur);
            alloc_traits::deallocate(m_node_alloc, cur, 1);
            cur = next;
        }
        m_head->next = nullptr;
    }

    template <typename T, typename Allocator>
    inline typename forward_list<T, Allocator>::iterator forward_list<T, Allocator>::
        insert_after(const_iterator pos, const_reference val)
    {
        Node *cur = pos.m_ptr;
        Node *next = cur->next;

        cur->next = alloc_traits::allocate(m_node_alloc, 1);
        alloc_traits::construct(m_node_alloc, cur->next, val);

        cur = cur->next;
        cur->next = next;

        return iterator(cur);
    }

    template <typename T, typename Allocator>
    inline typename forward_list<T, Allocator>::iterator forward_list<T, Allocator>::
        insert_after(const_iterator pos, value_type &&val)
    {
        Node *cur = pos.m_ptr;
        Node *next = cur->next;

        cur->next = alloc_traits::allocate(m_node_alloc, 1);
        alloc_traits::construct(m_node_alloc, cur->next, std::move(val));

        cur = cur->next;
        cur->next = next;

        return iterator(cur);
    }

    template <typename T, typename Allocator>
    inline typename forward_list<T, Allocator>::iterator forward_list<T, Allocator>::
        insert_after(const_iterator pos, size_type count, const_reference val)
    {
        Node *cur = pos.m_ptr;
        Node *next = cur->next;

        for (size_type i = 0; i < count; i++)
        {
            cur->next = alloc_traits::allocate(m_node_alloc, 1);
            alloc_traits::construct(m_node_alloc, cur->next, val);

            cur = cur->next;
        }
        cur->next = next;

        return iterator(cur);
    }

    template <typename T, typename Allocator>
    template <class InputIt>
    inline typename forward_list<T, Allocator>::iterator forward_list<T, Allocator>::
        insert_after(const_iterator pos, InputIt first, InputIt last)
    {
        Node *cur = pos.m_ptr;
        Node *next = cur->next;

        for (; first != last; first++)
        {
            cur->next = alloc_traits::allocate(m_node_alloc, 1);
            alloc_traits::construct(m_node_alloc, cur->next, *first);

            cur = cur->next;
        }
        cur->next = next;

        return iterator(cur);
    }

    template <typename T, typename Allocator>
    inline typename forward_list<T, Allocator>::iterator forward_list<T, Allocator>::
        insert_after(const_iterator pos, std::initializer_list<T> ilist)
    {
        Node *cur = pos.m_ptr;
        Node *next = cur->next;

        for (auto &val : ilist)
        {
            cur->next = alloc_traits::allocate(m_node_alloc, 1);
            alloc_traits::construct(m_node_alloc, cur->next, val);

            cur = cur->next;
        }
        cur->next = next;

        return iterator(cur);
    }

    template <typename T, typename Allocator>
    template <typename... Args>
    inline typename forward_list<T, Allocator>::iterator forward_list<T, Allocator>::
        emplace_after(const_iterator pos, Args &&...args)
    {
        Node *cur = pos.m_ptr;
        Node *next = cur->next;

        cur->next = alloc_traits::allocate(m_node_alloc, 1);
        alloc_traits::construct(m_node_alloc, cur->next, std::forward<Args>(args)...);

        cur = cur->next;
        cur->next = next;

        return iterator(cur);
    }

    template <typename T, typename Allocator>
    inline typename forward_list<T, Allocator>::iterator forward_list<T, Allocator>::
        erase_after(const_iterator pos)
    {
        if (pos == end())
            return end();

        Node *cur = pos.m_ptr;
        Node *del_node = cur->next;

        if (del_node == nullptr)
            return end();

        Node *next = del_node->next;
        cur->next = next;

        alloc_traits::destroy(m_node_alloc, del_node);
        alloc_traits::deallocate(m_node_alloc, del_node, 1);

        return iterator(next);
    }

    template <typename T, typename Allocator>
    inline typename forward_list<T, Allocator>::iterator forward_list<T, Allocator>::
        erase_after(const_iterator first, const_iterator last)
    {
        Node *pre = first.m_ptr;
        Node *cur = pre->next;

        while (cur != last.m_ptr)
        {
            Node *next = cur->next;
            alloc_traits::destroy(m_node_alloc, cur);
            alloc_traits::deallocate(m_node_alloc, cur, 1);
            cur = next;
        }

        pre->next = last.m_ptr;

        return iterator(last.m_ptr);
    }

    template <typename T, typename Allocator>
    inline void forward_list<T, Allocator>::push_front(const_reference val)
    {
        Node *next = m_head->next;
        m_head->next = alloc_traits::allocate(m_node_alloc, 1);
        alloc_traits::construct(m_node_alloc, m_head->next, val);
        m_head->next->next = next;
    }

    template <typename T, typename Allocator>
    inline void forward_list<T, Allocator>::push_front(value_type &&val)
    {
        Node *next = m_head->next;
        m_head->next = alloc_traits::allocate(m_node_alloc, 1);
        alloc_traits::construct(m_node_alloc, m_head->next, std::move(val));
        m_head->next->next = next;
    }

    template <typename T, typename Allocator>
    template <typename... Args>
    inline typename forward_list<T, Allocator>::reference forward_list<T, Allocator>::
        emplace_front(Args &&...args)
    {
        Node *next = m_head->next;
        m_head->next = alloc_traits::allocate(m_node_alloc, 1);
        alloc_traits::construct(m_node_alloc, m_head->next, std::forward<Args>(args)...);
        m_head->next->next = next;

        return m_head->next->value;
    }

    template <typename T, typename Allocator>
    inline void forward_list<T, Allocator>::pop_front()
    {
        if (empty())
            throw std::out_of_range("forward_list::pop_front: empty forward_list");

        Node *del_node = m_head->next;
        Node *next = del_node->next;

        alloc_traits::destroy(m_node_alloc, del_node);
        alloc_traits::deallocate(m_node_alloc, del_node, 1);

        m_head->next = next;
    }

    template <typename T, typename Allocator>
    inline void forward_list<T, Allocator>::resize(size_type count, const_reference val)
    {
        size_type size = std::distance(begin(), end());

        if (size == count)
            return;
        else if (size > count)
        {
            Node *cur = m_head;
            for (int i = 0; i < count; i++)
                cur = cur->next;

            Node *del_node = cur->next;
            cur->next = nullptr;
            while (del_node != nullptr)
            {
                Node *next = del_node->next;
                alloc_traits::destroy(m_node_alloc, del_node);
                alloc_traits::deallocate(m_node_alloc, del_node, 1);
                del_node = next;
            }
        }
        else
        {
            Node *cur = m_head;
            for (size_type i = 0; i < size; i++)
                cur = cur->next;

            while (size != count)
            {
                cur->next = alloc_traits::allocate(m_node_alloc, 1);
                alloc_traits::construct(m_node_alloc, cur->next, val);
                cur = cur->next;
                size++;
            }
        }
    }

    template <typename T, typename Allocator>
    inline void forward_list<T, Allocator>::swap(forward_list &other) noexcept
    {
        using std::swap;
        swap(m_head, other.m_head);
        swap(m_node_alloc, other.m_node_alloc);
    }

    template <typename T, typename Allocator>
    inline void forward_list<T, Allocator>::merge(forward_list &other)
    {
        if (this == &other)
            return;

        if (other.empty())
            return;

        if (this->empty())
        {
            std::swap(m_head->next, other.m_head->next);
            return;
        }

        using Comp = std::less<>;
        Comp comp;

        Node *prev = m_head;
        Node *curr = m_head->next;
        Node *other_curr = other.m_head->next;
        other.m_head->next = nullptr;

        while (curr != nullptr && other_curr != nullptr)
        {
            if (comp(curr->value, other_curr->value))
            {
                prev = curr;
                curr = curr->next;
            }
            else
            {
                prev->next = other_curr;
                other_curr = other_curr->next;
                prev->next->next = curr;
                prev = prev->next;
            }
        }

        if (other_curr != nullptr)
            prev->next = other_curr;
    }

    template <typename T, typename Allocator>
    inline void forward_list<T, Allocator>::merge(forward_list &&other)
    {
        merge(other);
    }

    template <typename T, typename Allocator>
    template <typename Compare>
    inline void forward_list<T, Allocator>::merge(forward_list &other, Compare comp)
    {
        if (this == &other)
            return;

        if (other.empty())
            return;

        if (this->empty())
        {
            std::swap(m_head->next, other.m_head->next);
            return;
        }

        Node *prev = m_head;
        Node *curr = m_head->next;
        Node *other_curr = other.m_head->next;
        other.m_head->next = nullptr;

        while (curr != nullptr && other_curr != nullptr)
        {
            if (comp(curr->value, other_curr->value))
            {
                prev = curr;
                curr = curr->next;
            }
            else
            {
                prev->next = other_curr;
                other_curr = other_curr->next;
                prev->next->next = curr;
                prev = prev->next;
            }
        }

        if (other_curr != nullptr)
            prev->next = other_curr;
    }

    template <typename T, typename Allocator>
    template <typename Compare>
    inline void forward_list<T, Allocator>::merge(forward_list &&other, Compare comp)
    {
        merge(other, comp);
    }

    template <typename T, typename Allocator>
    inline void forward_list<T, Allocator>::splice_after(const_iterator pos,
                                                         forward_list &other)
    {
        if (this == &other || other.empty())
            return;

        Node *cur = pos.m_ptr;
        Node *next = cur->next;
        Node *tail = other.m_head->next;

        while (tail->next != nullptr)
            tail = tail->next;

        cur->next = other.m_head->next;
        tail->next = next;

        other.m_head->next = nullptr;
    }

    template <typename T, typename Allocator>
    inline void forward_list<T, Allocator>::splice_after(const_iterator pos,
                                                         forward_list &&other)
    {
        splice_after(pos, other);
    }

    template <typename T, typename Allocator>
    inline void forward_list<T, Allocator>::splice_after(
        const_iterator pos, forward_list &other, const_iterator it)
    {
        if (this == &other || it.m_ptr == nullptr || it.m_ptr->next == nullptr)
            return;

        Node *cur = pos.m_ptr;
        Node *next = cur->next;
        Node *node = it.m_ptr->next;

        it.m_ptr->next = node->next;

        cur->next = node;
        node->next = next;
    }

    template <typename T, typename Allocator>
    inline void forward_list<T, Allocator>::splice_after(
        const_iterator pos, forward_list &&other, const_iterator it)
    {
        splice_after(pos, other, it);
    }

    template <typename T, typename Allocator>
    inline void forward_list<T, Allocator>::splice_after(
        const_iterator pos, forward_list &other, const_iterator first,
        const_iterator last)
    {

        if (this == &other || other.empty())
            return;

        Node *cur = pos.m_ptr;
        Node *next = cur->next;
        Node *head = first.m_ptr->next;
        Node *tail = head;

        if (head == nullptr)
            return;

        while (tail->next != last.m_ptr)
            tail = tail->next;

        cur->next = head;
        tail->next = next;

        first.m_ptr->next = last.m_ptr;
    }

    template <typename T, typename Allocator>
    inline void forward_list<T, Allocator>::splice_after(
        const_iterator pos, forward_list &&other,
        const_iterator first, const_iterator last)
    {
        splice_after(pos, other, first, last);
    }

    template <typename T, typename Allocator>
    inline typename forward_list<T, Allocator>::size_type
    forward_list<T, Allocator>::remove(const T &value)
    {
        size_type count = 0;
        Node *pre = m_head;
        Node *cur = m_head->next;
        while (cur != nullptr)
        {
            if (cur->value == value)
            {
                Node *next = cur->next;
                pre->next = cur->next;
                alloc_traits::destroy(m_node_alloc, cur);
                alloc_traits::deallocate(m_node_alloc, cur, 1);

                cur = next;
                count++;
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
    template <typename UnaryPredicate>
    inline typename forward_list<T, Allocator>::size_type forward_list<T, Allocator>::
        remove_if(UnaryPredicate p)
    {
        size_type count = 0;
        Node *pre = m_head;
        Node *cur = m_head->next;
        while (cur != nullptr)
        {
            if (p(cur->value))
            {
                Node *next = cur->next;
                pre->next = cur->next;
                alloc_traits::destroy(m_node_alloc, cur);
                alloc_traits::deallocate(m_node_alloc, cur, 1);

                cur = next;
                count++;
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
    inline void forward_list<T, Allocator>::reverse() noexcept
    {
        if (empty())
            return;

        Node *pre = m_head->next;
        Node *cur = pre->next;
        pre->next = nullptr; // 将第一个节点置为最后一个节点

        while (cur != nullptr)
        {
            Node *next = cur->next;
            cur->next = pre;

            pre = cur;
            cur = next;
        }
        m_head->next = pre;
    }

    template <typename T, typename Allocator>
    inline typename forward_list<T, Allocator>::size_type forward_list<T, Allocator>::unique()
    {
        if (empty())
            return 0;

        size_type count = 0;
        Node *pre = m_head->next;
        Node *cur = pre->next;
        while (cur != nullptr)
        {
            if (pre->value == cur->value)
            {
                Node *next = cur->next;
                pre->next = next;

                alloc_traits::destroy(m_node_alloc, cur);
                alloc_traits::deallocate(m_node_alloc, cur, 1);

                cur = next;
                count++;
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
    template <typename BinaryPredicate>
    inline typename forward_list<T, Allocator>::size_type forward_list<T, Allocator>::
        unique(BinaryPredicate p)
    {
        if (empty())
            return 0;

        size_type count = 0;
        Node *pre = m_head->next;
        Node *cur = pre->next;
        while (cur != nullptr)
        {
            if (p(pre->value, cur->value))
            {
                Node *next = cur->next;
                pre->next = next;

                alloc_traits::destroy(m_node_alloc, cur);
                alloc_traits::deallocate(m_node_alloc, cur, 1);

                cur = next;
                count++;
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
    inline void forward_list<T, Allocator>::sort()
    {
        std::less<> comp;
        sort(comp);
    }

    template <typename T, typename Allocator>
    template <typename Compare>
    inline void forward_list<T, Allocator>::sort(Compare comp)
    {
        if (empty() || m_head->next->next == nullptr)
            return;

        Node *head = m_head->next;
        Node *cur = nullptr;
        Node *left = nullptr;
        Node *right = nullptr;

        size_type len = 0;
        cur = head;
        while (cur != nullptr)
        {
            len++;
            cur = cur->next;
        }

        for (size_type i = 1; i < len; i *= 2)
        {
            cur = head;
            Node *temp = alloc_traits::allocate(m_node_alloc, 1); // 临时前哨节点
            alloc_traits::construct(m_node_alloc, temp);
            Node *tail = temp;

            while (cur != nullptr)
            {
                left = cur;
                right = split(left, i);
                cur = split(right, i);
                tail->next = merge_two(left, right, comp);

                while (tail->next != nullptr)
                    tail = tail->next;
            }

            head = temp->next;
            alloc_traits::destroy(m_node_alloc, temp);
            alloc_traits::deallocate(m_node_alloc, temp, 1);
        }

        m_head->next = head;
    }

    template <typename T, typename Allocator>
    inline typename forward_list<T, Allocator>::Node *forward_list<T, Allocator>::
        split(Node *head, size_type n)
    {
        if (head == nullptr)
            return nullptr;

        Node *cur = head;
        // 向前走 n-1 步（拆分出前 n 个节点）
        for (size_type i = 1; i < n && cur->next != nullptr; i++)
            cur = cur->next;

        Node *next_part = cur->next;
        cur->next = nullptr;

        return next_part;
    }

    template <typename T, typename Allocator>
    inline typename forward_list<T, Allocator>::Node *forward_list<T, Allocator>::
        merge_two(Node *la, Node *lb)
    {
        std::less<> comp;
        return merge_two(la, lb, comp);
    }

    template <typename T, typename Allocator>
    template <typename Compare>
    inline typename forward_list<T, Allocator>::Node *forward_list<T, Allocator>::
        merge_two(Node *la, Node *lb, Compare comp)
    {
        if (la == nullptr)
            return lb;
        if (lb == nullptr)
            return la;
        if (la == lb)
            return la;

        Node *cur_a = la;
        Node *cur_b = lb;
        Node *temp = alloc_traits::allocate(m_node_alloc, 1); // 创建一个临时前哨节点用于合并
        alloc_traits::construct(m_node_alloc, temp);
        temp->next = la;
        Node *pre = temp;

        while (cur_a != nullptr && cur_b != nullptr)
        {
            if (comp(cur_a->value, cur_b->value))
            {
                pre = cur_a;
                cur_a = cur_a->next;
            }
            else
            {
                Node *next = pre->next;
                Node *next_b = cur_b->next;

                pre->next = cur_b;
                cur_b->next = next;

                pre = pre->next;
                cur_b = next_b;
            }
        }

        if (cur_b != nullptr)
            pre->next = cur_b;

        Node *res = temp->next;
        alloc_traits::destroy(m_node_alloc, temp);
        alloc_traits::deallocate(m_node_alloc, temp, 1);

        return res;
    }

    template <typename T, typename Allocator>
    bool operator==(const forward_list<T, Allocator> &lhs,
                    const forward_list<T, Allocator> &rhs)
    {
        typename forward_list<T, Allocator>::Node *s_lhs = lhs.m_head->next;
        typename forward_list<T, Allocator>::Node *s_rhs = rhs.m_head->next;

        for (; s_lhs != nullptr && s_rhs != nullptr; s_lhs = s_lhs->next,
                                                     s_rhs = s_rhs->next)
        {
            if (s_lhs->value != s_rhs->value)
                return false;
        }

        return (s_lhs == nullptr && s_rhs == nullptr);
    }

    template <typename T, typename Allocator>
    bool operator!=(const forward_list<T, Allocator> &lhs,
                    const forward_list<T, Allocator> &rhs)
    {
        return !(lhs == rhs);
    }

} // namespace demo
