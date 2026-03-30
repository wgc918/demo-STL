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
        using node_allocator = typename std::allocator_traits<Allocator>::template rebind_alloc<Node>;

        Node *m_head;
        node_allocator m_node_alloc;

        using alloc_traits = std::allocator_traits<node_allocator>;
    };
} // namespace demo
