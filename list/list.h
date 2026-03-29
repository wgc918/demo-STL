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

            //解引用
            reference operator*() const noexcept;
            pointer operator->() const noexcept;

            //自增/自减
            const_iterator &operator++() noexcept;
            const_iterator operator++(int) noexcept;
            const_iterator &operator--() noexcept;
            const_iterator operator--(int) noexcept;

            //比较
            bool operator==(const const_iterator other) const noexcept;
            bool operator!=(const const_iterator other) const noexcept;
            bool operator==(const iterator other) const noexcept;
            bool operator!=(const iterator other) const noexcept;

        private:
            Node *m_ptr;
        };

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

        //迭代器
        

    private:
        using node_allocator = typename std::allocator_traits<Allocator>::template rebind_alloc<Node>;

        Node *m_head;
        node_allocator m_node_alloc;

        using alloc_traits = std::allocator_traits<node_allocator>;
    };
} // namespace demo
