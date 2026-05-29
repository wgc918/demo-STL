//-----------------------------------------------------------------------------
// 版权所有 (C) 2026 demo-STL 项目
//
// 文件: stack.h
// 作者: wgc
// 创建日期: 2026年5月
// 最后修改: 2026年5月
//
// 描述:
//     本文件实现了一个 STL 风格的 stack（栈）容器适配器。
//     stack 是一种后进先出（LIFO）的数据结构，只能在容器的一端（栈顶）进行插入和删除操作。
//
// 功能特性:
//     - 提供标准的栈操作：push、pop、top
//     - 支持任意序列容器作为底层实现（默认使用 deque）
//     - 符合 STL 容器适配器规范
//     - 支持拷贝、移动构造和赋值
//     - 提供比较运算符支持
//
// 实现说明:
//     stack 是一个容器适配器，它封装了一个序列容器，
//     并提供了受限的访问接口。默认使用 deque 作为底层容器，
//     也可以使用 list 或 vector 等支持 back()、push_back()、pop_back() 的容器。
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

#include <vcruntime_typeinfo.h>

#include <climits>
#include <utility>

#include "../deque/deque.h"
#include "../list/list.h"
#include "../vector/vector.h"

namespace demo
{
/// @brief stack（栈）容器适配器类
/// @details
/// stack 是一种后进先出（LIFO）的数据结构适配器，
/// 只能在容器的一端（栈顶）进行插入和删除操作。
/// stack 封装了一个序列容器，并提供了受限的访问接口。
/// @tparam T 元素类型
/// @tparam Container 底层容器类型，默认为 deque<T>，
///         要求支持 back()、push_back()、pop_back() 操作
template <typename T, class Container = deque<T>>
class stack;

/// @brief 比较两个 stack 是否相等
/// @tparam T 元素类型
/// @tparam Container 底层容器类型
/// @param lhs 左操作数
/// @param rhs 右操作数
/// @return 如果两个 stack 相等返回 true，否则返回 false
template <typename T, class Container>
bool operator==(const stack<T, Container>& lhs, const stack<T, Container>& rhs);

/// @brief 比较两个 stack 是否不相等
/// @tparam T 元素类型
/// @tparam Container 底层容器类型
/// @param lhs 左操作数
/// @param rhs 右操作数
/// @return 如果两个 stack 不相等返回 true，否则返回 false
template <typename T, class Container>
bool operator!=(const stack<T, Container>& lhs, const stack<T, Container>& rhs);

template <typename T, class Container>
class stack
{
    friend bool operator== <>(const stack<T, Container>& lhs, const stack<T, Container>& rhs);
    friend bool operator!= <>(const stack<T, Container>& lhs, const stack<T, Container>& rhs);

public:
    using container_type  = Container;                            ///< 底层容器类型
    using value_type      = typename Container::value_type;       ///< 元素类型
    using reference       = typename Container::reference;        ///< 元素引用类型
    using const_reference = typename Container::const_reference;  ///< 常量元素引用类型
    using size_type       = typename Container::size_type;        ///< 大小类型
    using difference_type = typename Container::difference_type;  ///< 差值类型

public:
    /// @brief 默认构造函数，创建空栈
    stack();

    /// @brief 构造函数，从容器拷贝构造
    /// @param container 要拷贝的容器
    explicit stack(const Container& container);

    /// @brief 构造函数，从容器移动构造
    /// @param container 要移动的容器
    explicit stack(Container&& container);

    /// @brief 拷贝构造函数
    /// @param other 要拷贝的栈
    stack(const stack& other);

    /// @brief 移动构造函数
    /// @param other 要移动的栈
    stack(stack&& other) noexcept;

    /// @brief 析构函数
    ~stack();

    /// @brief 拷贝赋值运算符
    /// @param other 要拷贝的栈
    /// @return 当前栈的引用
    stack& operator=(const stack& other);

    /// @brief 移动赋值运算符
    /// @param other 要移动的栈
    /// @return 当前栈的引用
    stack& operator=(stack&& other) noexcept;

    /// @brief 获取栈顶元素的引用
    /// @return 栈顶元素的引用
    /// @note 在空栈上调用此函数会导致未定义行为
    reference top();

    /// @brief 获取栈顶元素的常量引用
    /// @return 栈顶元素的常量引用
    /// @note 在空栈上调用此函数会导致未定义行为
    const_reference top() const;

    /// @brief 获取栈中元素的数量
    /// @return 元素数量
    size_type size() const;

    /// @brief 检查栈是否为空
    /// @return 如果栈为空返回 true，否则返回 false
    bool empty() const;

    /// @brief 在栈顶插入元素（拷贝版本）
    /// @param value 要插入的元素
    void push(const value_type& value);

    /// @brief 在栈顶插入元素（移动版本）
    /// @param value 要插入的元素（右值）
    void push(value_type&& value);

    /// @brief 在栈顶原地构造元素
    /// @tparam Args 构造参数类型
    /// @param args 构造参数
    /// @return 构造的元素的引用（取决于底层容器的 emplace_back 返回值）
    template <class... Args>
    decltype(auto) emplace(Args&&... args);

    /// @brief 移除栈顶元素
    /// @note 在空栈上调用此函数会导致未定义行为
    void pop();

    /// @brief 交换两个栈的内容
    /// @param other 要交换的栈
    void swap(stack& other);

protected:
    Container m_container;  ///< 底层容器，存储栈中的元素
};

template <typename T, class Container>
stack<T, Container>::stack() : m_container()
{
}

template <typename T, class Container>
stack<T, Container>::stack(const Container& container) : m_container(container)
{
}

template <typename T, class Container>
stack<T, Container>::stack(Container&& container) : m_container(std::move(container))
{
}

template <typename T, class Container>
stack<T, Container>::stack(const stack& other) : m_container(other.m_container)
{
}

template <typename T, class Container>
stack<T, Container>::stack(stack&& other) noexcept : m_container(std::move(other.m_container))
{
}

template <typename T, class Container>
stack<T, Container>::~stack()
{
}

template <typename T, class Container>
inline stack<T, Container>& stack<T, Container>::operator=(const stack& other)
{
    if (this == &other)
        return *this;

    m_container = other.m_container;
    return *this;
}

template <typename T, class Container>
inline stack<T, Container>& stack<T, Container>::operator=(stack&& other) noexcept
{
    if (this == &other)
        return *this;

    m_container = std::move(other.m_container);
    return *this;
}

template <typename T, class Container>
inline typename stack<T, Container>::reference stack<T, Container>::top()
{
    return m_container.back();
}

template <typename T, class Container>
inline typename stack<T, Container>::const_reference stack<T, Container>::top() const
{
    return m_container.back();
}

template <typename T, class Container>
inline typename stack<T, Container>::size_type stack<T, Container>::size() const
{
    return m_container.size();
}

template <typename T, class Container>
inline bool stack<T, Container>::empty() const
{
    return m_container.empty();
}

template <typename T, class Container>
inline void stack<T, Container>::push(const value_type& value)
{
    m_container.push_back(value);
}

template <typename T, class Container>
inline void stack<T, Container>::push(value_type&& value)
{
    m_container.push_back(std::move(value));
}

template <typename T, class Container>
template <class... Args>
inline decltype(auto) stack<T, Container>::emplace(Args&&... args)
{
    return m_container.emplace_back(std::forward<Args>(args)...);
}

template <typename T, class Container>
inline void stack<T, Container>::pop()
{
    m_container.pop_back();
}

template <typename T, class Container>
inline void stack<T, Container>::swap(stack& other)
{
    m_container.swap(other.m_container);
}

template <typename T, class Container>
bool operator==(const stack<T, Container>& lhs, const stack<T, Container>& rhs)
{
    return lhs.m_container == rhs.m_container;
}

template <typename T, class Container>
bool operator!=(const stack<T, Container>& lhs, const stack<T, Container>& rhs)
{
    return lhs.m_container != rhs.m_container;
}

}  // namespace demo
