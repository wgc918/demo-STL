//-----------------------------------------------------------------------------
// 版权所有 (C) 2026 demo-STL 项目
//
// 文件: queue.h
// 作者: wgc
// 创建日期: 2026年5月
// 最后修改: 2026年5月
//
// 描述:
//     本文件实现了一个 STL 风格的 queue（队列）容器适配器。
//     queue 是一种先进先出（FIFO）的数据结构，只允许在一端（队尾）插入元素，
//     在另一端（队首）删除元素。
//
// 功能特性:
//     - 提供标准的队列操作：push、pop、front、back
//     - 支持任意序列容器作为底层实现（默认使用 deque）
//     - 符合 STL 容器适配器规范
//     - 支持拷贝、移动构造和赋值
//     - 提供比较运算符支持
//
// 实现说明:
//     queue 是一个容器适配器，它封装了一个序列容器，
//     并提供了受限的访问接口。默认使用 deque 作为底层容器，
//     也可以使用 list 等支持 front()、back()、push_back()、pop_front() 的容器。
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

#include <climits>
#include <queue>
#include <utility>

#include "../deque/deque.h"
#include "../list/list.h"

namespace demo
{
template <typename T, typename Container = deque<T>>
class queue;

template <typename T, typename Container>
bool operator!=(const queue<T, Container>& lhs, const queue<T, Container>& rhs);

template <typename T, typename Container>
bool operator==(const queue<T, Container>& lhs, const queue<T, Container>& rhs);

/// @brief queue（队列）容器适配器类
/// @details
/// queue 是一种先进先出（FIFO）的数据结构适配器，
/// 允许在队尾插入元素，在队首删除元素。
/// queue 封装了一个序列容器，并提供了受限的访问接口。
/// @tparam T 元素类型
/// @tparam Container 底层容器类型，默认为 deque<T>，
///         要求支持 front()、back()、push_back()、pop_front() 操作
template <typename T, typename Container>
class queue
{
    friend bool operator!= <>(const queue<T, Container>& lhs, const queue<T, Container>& rhs);
    friend bool operator== <>(const queue<T, Container>& lhs, const queue<T, Container>& rhs);

public:
    using container_type  = Container;                            ///< 底层容器类型
    using value_type      = typename Container::value_type;       ///< 元素类型
    using reference       = typename Container::reference;        ///< 元素引用类型
    using const_reference = typename Container::const_reference;  ///< 常量元素引用类型
    using size_type       = typename Container::size_type;        ///< 大小类型

public:
    /// @brief 默认构造函数，创建空队列
    queue();

    /// @brief 构造函数，从容器拷贝构造
    /// @param container 要拷贝的容器
    explicit queue(const Container& container);

    /// @brief 构造函数，从容器移动构造
    /// @param container 要移动的容器
    explicit queue(Container&& container);

    /// @brief 拷贝构造函数
    /// @param other 要拷贝的队列
    queue(const queue& other);

    /// @brief 移动构造函数
    /// @param other 要移动的队列
    queue(queue&& other) noexcept;

    /// @brief 析构函数
    ~queue();

    /// @brief 拷贝赋值运算符
    /// @param other 要拷贝的队列
    /// @return 当前队列的引用
    queue& operator=(const queue& other);

    /// @brief 移动赋值运算符
    /// @param other 要移动的队列
    /// @return 当前队列的引用
    queue& operator=(queue&& other) noexcept;

    /// @brief 获取队首元素的引用
    /// @return 队首元素的引用
    /// @note 在空队列上调用此函数会导致未定义行为
    reference front();

    /// @brief 获取队首元素的常量引用
    /// @return 队首元素的常量引用
    /// @note 在空队列上调用此函数会导致未定义行为
    const_reference front() const;

    /// @brief 获取队尾元素的引用
    /// @return 队尾元素的引用
    /// @note 在空队列上调用此函数会导致未定义行为
    reference back();

    /// @brief 获取队尾元素的常量引用
    /// @return 队尾元素的常量引用
    /// @note 在空队列上调用此函数会导致未定义行为
    const_reference back() const;

    /// @brief 获取队列中元素的数量
    /// @return 元素数量
    size_type size() const;

    /// @brief 检查队列是否为空
    /// @return 如果队列为空返回 true，否则返回 false
    bool empty() const;

    /// @brief 在队尾插入元素（拷贝版本）
    /// @param value 要插入的元素
    void push(const value_type& value);

    /// @brief 在队尾插入元素（移动版本）
    /// @param value 要插入的元素（右值）
    void push(value_type&& value);

    /// @brief 在队尾原地构造元素
    /// @tparam Args 构造参数类型
    /// @param args 构造参数
    /// @return 构造的元素的引用（取决于底层容器的 emplace_back 返回值）
    template <class... Args>
    decltype(auto) emplace(Args&&... args);

    /// @brief 移除队首元素
    /// @note 在空队列上调用此函数会导致未定义行为
    void pop();

    /// @brief 交换两个队列的内容
    /// @param other 要交换的队列
    void swap(queue& other);

protected:
    Container m_container;  ///< 底层容器，存储队列中的元素
};

template <typename T, typename Container>
queue<T, Container>::queue() : m_container()
{
}

template <typename T, typename Container>
queue<T, Container>::queue(const Container& container) : m_container(container)
{
}

template <typename T, typename Container>
queue<T, Container>::queue(Container&& container) : m_container(std::move(container))
{
}

template <typename T, typename Container>
queue<T, Container>::queue(const queue& other) : m_container(other.m_container)
{
}

template <typename T, typename Container>
queue<T, Container>::queue(queue&& other) noexcept : m_container(std::move(other.m_container))
{
}

template <typename T, typename Container>
queue<T, Container>::~queue()
{
}

template <typename T, typename Container>
inline queue<T, Container>& queue<T, Container>::operator=(const queue& other)
{
    if (this == &other)
        return *this;

    m_container = other.m_container;
    return *this;
}

template <typename T, typename Container>
inline queue<T, Container>& queue<T, Container>::operator=(queue&& other) noexcept
{
    if (this == &other)
        return *this;

    m_container = std::move(other.m_container);
    return *this;
}

template <typename T, typename Container>
inline typename queue<T, Container>::reference queue<T, Container>::front()
{
    return m_container.front();
}

template <typename T, typename Container>
inline typename queue<T, Container>::const_reference queue<T, Container>::front() const
{
    return m_container.front();
}

template <typename T, typename Container>
inline typename queue<T, Container>::reference queue<T, Container>::back()
{
    return m_container.back();
}

template <typename T, typename Container>
inline typename queue<T, Container>::const_reference queue<T, Container>::back() const
{
    return m_container.back();
}

template <typename T, typename Container>
inline typename queue<T, Container>::size_type queue<T, Container>::size() const
{
    return m_container.size();
}

template <typename T, typename Container>
inline bool queue<T, Container>::empty() const
{
    return m_container.empty();
}

template <typename T, typename Container>
inline void queue<T, Container>::push(const value_type& value)
{
    m_container.push_back(value);
}

template <typename T, typename Container>
inline void queue<T, Container>::push(value_type&& value)
{
    m_container.push_back(std::move(value));
}

template <typename T, typename Container>
template <class... Args>
decltype(auto) queue<T, Container>::emplace(Args&&... args)
{
    return m_container.emplace_back(std::forward<Args>(args)...);
}

template <typename T, typename Container>
inline void queue<T, Container>::pop()
{
    m_container.pop_front();
}

template <typename T, typename Container>
inline void queue<T, Container>::swap(queue& other)
{
    m_container.swap(other.m_container);
}

/// @brief 比较两个 queue 是否不相等
/// @tparam T 元素类型
/// @tparam Container 底层容器类型
/// @param lhs 左操作数
/// @param rhs 右操作数
/// @return 如果两个 queue 不相等返回 true，否则返回 false
template <typename T, typename Container>
inline bool operator!=(const queue<T, Container>& lhs, const queue<T, Container>& rhs)
{
    return lhs.m_container != rhs.m_container;
}

/// @brief 比较两个 queue 是否相等
/// @tparam T 元素类型
/// @tparam Container 底层容器类型
/// @param lhs 左操作数
/// @param rhs 右操作数
/// @return 如果两个 queue 相等返回 true，否则返回 false
template <typename T, typename Container>
inline bool operator==(const queue<T, Container>& lhs, const queue<T, Container>& rhs)
{
    return lhs.m_container == rhs.m_container;
}

}  // namespace demo