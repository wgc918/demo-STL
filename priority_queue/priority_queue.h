//-----------------------------------------------------------------------------
// 版权所有 (C) 2026 demo-STL 项目
//
// 文件: priority_queue.h
// 作者: wgc
// 创建日期: 2026年5月
// 最后修改: 2026年5月
//
// 描述:
//     本文件实现了一个 STL 风格的 priority_queue（优先队列）容器适配器。
//     priority_queue 是一种容器适配器，它提供优先队列功能，
//     元素按照特定的比较顺序排列，顶部元素始终是优先级最高的元素。
//
// 功能特性:
//     - 提供标准的优先队列操作：push、pop、top
//     - 支持任意序列容器作为底层实现（默认使用 vector）
//     - 支持自定义比较函数
//     - 符合 STL 容器适配器规范
//     - 支持拷贝、移动构造和赋值
//     - 使用 Floyd 算法构建堆，效率更高
//
// 实现说明:
//     priority_queue 是一个容器适配器，它封装了一个序列容器，
//     并使用堆数据结构来实现优先队列功能。默认使用 vector 作为底层容器，
//     也可以使用 deque 等支持随机访问迭代器的容器。
//
// 许可证:
//     MIT License
//
//     版权所有 (c) 2026 wgc
//
//     特此免费授予获得本软件副本和相关文档文件（以下简称"软件"）的任何人以处理软件的权利，
//     包括但不限于使用、复制、修改、合并、出版、分发、再许可和/或出售软件副本，
//     以及允许软件适用者这样做，须在下列条件下：
//
//     上述版权声明和本许可声明应包含在软件的所有副本或实质性部分中。
//
//     软件按"原样"提供，不提供任何形式的明示或暗示的保证，
//     包括但不限于对适销性、特定用途适用性和非侵权性的保证。
//     在任何情况下，作者或版权持有人均不对任何索赔、损害或其他责任负责，
//     无论是在合同诉讼、侵权诉讼或其他诉讼中，
//     由于软件或软件的使用或其他交易产生的。
//-----------------------------------------------------------------------------

#pragma once

#include <functional>
#include <type_traits>

#include "../deque/deque.h"
#include "../vector/vector.h"

namespace demo
{
/// @brief priority_queue（优先队列）容器适配器类
/// @details
/// priority_queue 是一种容器适配器，它提供优先队列功能，
/// 元素按照特定的比较顺序排列，顶部元素始终是优先级最高的元素。
/// priority_queue 封装了一个序列容器，并使用堆数据结构实现。
/// @tparam T 元素类型
/// @tparam Container 底层容器类型，默认为 vector<T>，
///         要求支持随机访问迭代器和 back()、push_back()、pop_back() 操作
/// @tparam Compare 比较函数类型，默认为 std::less，用于确定元素优先级
template <typename T, typename Container = vector<T>,
          typename Compare = std::less<typename Container::value_type>>
class priority_queue
{
public:
    using container_type  = Container;                            ///< 底层容器类型
    using value_type      = typename Container::value_type;       ///< 元素类型
    using size_type       = typename Container::size_type;        ///< 大小类型
    using reference       = typename Container::reference;        ///< 元素引用类型
    using const_reference = typename Container::const_reference;  ///< 常量元素引用类型

public:
    /// @brief 默认构造函数，创建空优先队列
    priority_queue();

    /// @brief 构造函数，从容器拷贝构造
    /// @param container 要拷贝的容器
    explicit priority_queue(const Container& container);

    /// @brief 构造函数，从比较函数构造
    /// @param comp 比较函数对象
    explicit priority_queue(const Compare& comp);

    /// @brief 构造函数，从容器和比较函数拷贝构造
    /// @param container 要拷贝的容器
    /// @param comp 比较函数对象
    priority_queue(const Container& container, const Compare& comp);

    /// @brief 构造函数，从容器移动构造
    /// @param container 要移动的容器
    priority_queue(Container&& container);

    /// @brief 构造函数，从容器和比较函数移动构造
    /// @param container 要移动的容器
    /// @param comp 比较函数对象
    priority_queue(Container&& container, const Compare& comp);

    /// @brief 构造函数，从迭代器范围构造
    /// @tparam InputIt 输入迭代器类型
    /// @param first 范围起始迭代器
    /// @param last 范围结束迭代器
    template <typename InputIt, std::enable_if_t<!std::is_integral<InputIt>::value, int> = 0>
    priority_queue(InputIt first, InputIt last);

    /// @brief 构造函数，从迭代器范围和比较函数构造
    /// @tparam InputIt 输入迭代器类型
    /// @param first 范围起始迭代器
    /// @param last 范围结束迭代器
    /// @param comp 比较函数对象
    template <typename InputIt, std::enable_if_t<!std::is_integral<InputIt>::value, int> = 0>
    priority_queue(InputIt first, InputIt last, const Compare& comp);

    /// @brief 拷贝构造函数
    /// @param other 要拷贝的优先队列
    priority_queue(const priority_queue& other);

    /// @brief 移动构造函数
    /// @param other 要移动的优先队列
    priority_queue(priority_queue&& other) noexcept;

    /// @brief 析构函数
    ~priority_queue();

    /// @brief 拷贝赋值运算符
    /// @param other 要拷贝的优先队列
    /// @return 当前优先队列的引用
    priority_queue& operator=(const priority_queue& other);

    /// @brief 移动赋值运算符
    /// @param other 要移动的优先队列
    /// @return 当前优先队列的引用
    priority_queue& operator=(priority_queue&& other) noexcept;

    /// @brief 获取队顶元素的常量引用
    /// @return 队顶元素的常量引用
    /// @note 在空优先队列上调用此函数会导致未定义行为
    const_reference top() const;

    /// @brief 检查优先队列是否为空
    /// @return 如果优先队列为空返回 true，否则返回 false
    bool empty() const;

    /// @brief 获取优先队列中元素的数量
    /// @return 元素数量
    size_type size() const;

    /// @brief 在优先队列中插入元素（拷贝版本）
    /// @param value 要插入的元素
    void push(const value_type& value);

    /// @brief 在优先队列中插入元素（移动版本）
    /// @param value 要插入的元素（右值）
    void push(value_type&& value);

    /// @brief 在优先队列中原地构造元素
    /// @tparam Args 构造参数类型
    /// @param args 构造参数
    template <typename... Args>
    void emplace(Args&&... agrs);

    /// @brief 移除队顶元素
    /// @note 在空优先队列上调用此函数会导致未定义行为
    void pop();

    /// @brief 交换两个优先队列的内容
    /// @param other 要交换的优先队列
    void swap(priority_queue& other);

protected:
    /// @brief 向上调整堆
    /// @param idx 要调整的元素索引
    void up(int idx);

    /// @brief 向下调整堆
    /// @param idx 要调整的元素索引
    void down(int idx);

    /// @brief 获取父节点索引
    /// @param idx 当前节点索引
    /// @return 父节点索引
    int get_parent(int idx) const;

    /// @brief 获取左子节点索引
    /// @param idx 当前节点索引
    /// @return 左子节点索引
    int get_left_child(int idx) const;

    /// @brief 获取右子节点索引
    /// @param idx 当前节点索引
    /// @return 右子节点索引
    int get_right_child(int idx) const;

    /// @brief 使用 Floyd 算法构建堆
    void build_heap_floyd();

protected:
    Container m_container;  ///< 底层容器，存储优先队列中的元素
    Compare   m_comp;       ///< 比较函数对象，用于确定元素优先级
};

template <typename T, typename Container, typename Compare>
priority_queue<T, Container, Compare>::priority_queue() : m_container(), m_comp()
{
}

template <typename T, typename Container, typename Compare>
priority_queue<T, Container, Compare>::priority_queue(const Container& container)
    : m_container(container), m_comp()
{
    build_heap_floyd();
}

template <typename T, typename Container, typename Compare>
priority_queue<T, Container, Compare>::priority_queue(const Compare& comp)
    : m_container(), m_comp(comp)
{
}

template <typename T, typename Container, typename Compare>
priority_queue<T, Container, Compare>::priority_queue(const Container& container,
                                                      const Compare&   comp)
    : m_container(container), m_comp(comp)
{
    build_heap_floyd();
}

template <typename T, typename Container, typename Compare>
priority_queue<T, Container, Compare>::priority_queue(Container&& container)
    : m_container(std::move(container))
{
    build_heap_floyd();
}

template <typename T, typename Container, typename Compare>
priority_queue<T, Container, Compare>::priority_queue(Container&& container, const Compare& comp)
    : m_container(std::move(container)), m_comp(comp)
{
    build_heap_floyd();
}

template <typename T, typename Container, typename Compare>
template <typename InputIt, std::enable_if_t<!std::is_integral<InputIt>::value, int>>
priority_queue<T, Container, Compare>::priority_queue(InputIt first, InputIt last)
    : m_container(first, last), m_comp()
{
    build_heap_floyd();
}

template <typename T, typename Container, typename Compare>
template <typename InputIt, std::enable_if_t<!std::is_integral<InputIt>::value, int>>
priority_queue<T, Container, Compare>::priority_queue(InputIt first, InputIt last,
                                                      const Compare& comp)
    : m_container(first, last), m_comp(comp)
{
    build_heap_floyd();
}

template <typename T, typename Container, typename Compare>
priority_queue<T, Container, Compare>::priority_queue(const priority_queue& other)
    : m_container(other.m_container), m_comp(other.m_comp)
{
}

template <typename T, typename Container, typename Compare>
priority_queue<T, Container, Compare>::priority_queue(priority_queue&& other) noexcept
    : m_container(std::move(other.m_container)), m_comp(std::move(other.m_comp))
{
}

template <typename T, typename Container, typename Compare>
priority_queue<T, Container, Compare>::~priority_queue()
{
}

template <typename T, typename Container, typename Compare>
inline priority_queue<T, Container, Compare>& priority_queue<T, Container, Compare>::operator=(
    const priority_queue& other)
{
    if (this == &other)
        return *this;

    m_container = other.m_container;
    m_comp      = other.m_comp;
    return *this;
}

template <typename T, typename Container, typename Compare>
inline priority_queue<T, Container, Compare>& priority_queue<T, Container, Compare>::operator=(
    priority_queue&& other) noexcept
{
    if (this == &other)
        return *this;

    m_container = std::move(other.m_container);
    m_comp      = std::move(other.m_comp);
    return *this;
}

template <typename T, typename Container, typename Compare>
inline typename priority_queue<T, Container, Compare>::const_reference
priority_queue<T, Container, Compare>::top() const
{
    return m_container.front();
}

template <typename T, typename Container, typename Compare>
inline bool priority_queue<T, Container, Compare>::empty() const
{
    return m_container.empty();
}

template <typename T, typename Container, typename Compare>
inline typename priority_queue<T, Container, Compare>::size_type
priority_queue<T, Container, Compare>::size() const
{
    return m_container.size();
}

template <typename T, typename Container, typename Compare>
inline void priority_queue<T, Container, Compare>::push(const value_type& value)
{
    m_container.push_back(value);
    up(static_cast<int>(m_container.size() - 1));
}

template <typename T, typename Container, typename Compare>
inline void priority_queue<T, Container, Compare>::push(value_type&& value)
{
    m_container.push_back(std::move(value));
    up(static_cast<int>(m_container.size() - 1));
}

template <typename T, typename Container, typename Compare>
template <typename... Args>
inline void priority_queue<T, Container, Compare>::emplace(Args&&... args)
{
    m_container.emplace_back(std::forward<Args>(args)...);
    up(static_cast<int>(m_container.size() - 1));
}

template <typename T, typename Container, typename Compare>
inline void priority_queue<T, Container, Compare>::pop()
{
    m_container[0] = m_container.back();
    m_container.pop_back();
    down(0);
}

template <typename T, typename Container, typename Compare>
inline void priority_queue<T, Container, Compare>::swap(priority_queue& other)
{
    m_container.swap(other.m_container);
    std::swap(m_comp, other.m_comp);
}

template <typename T, typename Container, typename Compare>
inline void priority_queue<T, Container, Compare>::up(int idx)
{
    while (idx > 0)
    {
        int parent = get_parent(idx);
        if (m_comp(m_container[parent], m_container[idx]))
        {
            std::swap(m_container[idx], m_container[parent]);
            idx = parent;
        }
        else
        {
            break;
        }
    }
}

template <typename T, typename Container, typename Compare>
inline void priority_queue<T, Container, Compare>::down(int idx)
{
    while (true)
    {
        int left_child  = get_left_child(idx);
        int right_child = get_right_child(idx);
        int tag_idx     = idx;

        if (left_child < m_container.size() &&
            m_comp(m_container[tag_idx], m_container[left_child]))
        {
            tag_idx = left_child;
        }
        if (right_child < m_container.size() &&
            m_comp(m_container[tag_idx], m_container[right_child]))
        {
            tag_idx = right_child;
        }

        if (tag_idx == idx)
            break;

        std::swap(m_container[idx], m_container[tag_idx]);
        idx = tag_idx;
    }
}

template <typename T, typename Container, typename Compare>
inline int priority_queue<T, Container, Compare>::get_parent(int idx) const
{
    return (idx - 1) / 2;
}

template <typename T, typename Container, typename Compare>
inline int priority_queue<T, Container, Compare>::get_left_child(int idx) const
{
    return idx * 2 + 1;
}

template <typename T, typename Container, typename Compare>
inline int priority_queue<T, Container, Compare>::get_right_child(int idx) const
{
    return idx * 2 + 2;
}

template <typename T, typename Container, typename Compare>
inline void priority_queue<T, Container, Compare>::build_heap_floyd()
{
    for (int i = static_cast<int>(m_container.size() / 2 - 1); i >= 0; i--)
        down(i);
}

}  // namespace demo
