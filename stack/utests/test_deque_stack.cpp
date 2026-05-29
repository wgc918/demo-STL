/**
 * @file test_deque_stack.cpp
 * @brief 基于doctest的工业级栈(stack)测试套件 - 使用deque作为底层容器
 *
 * 测试覆盖范围:
 * - 构造函数(默认、容器、拷贝、移动)
 * - 赋值操作符(拷贝赋值、移动赋值)
 * - 元素访问(top)
 * - 容量操作(empty, size)
 * - 修改器(push, emplace, pop, swap)
 * - 全局比较运算符(==, !=)
 * - 边界条件和异常安全
 * - 性能测试
 */

#define DOCTEST_CONFIG_NO_MULTITHREADING
#include <algorithm>
#include <chrono>
#include <random>
#include <stdexcept>
#include <string>
#include <vector>

#include "../stack.h"
#include "doctest.h"


// ============================================
// 测试辅助工具
// ============================================

template <typename T, typename Container>
std::vector<T> stack_to_vector(const demo::stack<T, Container>& stk)
{
    std::vector<T>            result;
    demo::stack<T, Container> temp = stk;
    while (!temp.empty())
    {
        result.push_back(temp.top());
        temp.pop();
    }
    std::reverse(result.begin(), result.end());
    return result;
}

template <typename T, typename Container>
bool stack_equals_vector(const demo::stack<T, Container>& stk, const std::vector<T>& vec)
{
    if (stk.size() != vec.size())
        return false;

    demo::stack<T, Container> temp = stk;
    for (auto it = vec.rbegin(); it != vec.rend(); ++it)
    {
        if (temp.top() != *it)
            return false;
        temp.pop();
    }
    return true;
}

// ============================================
// 测试套件: 构造函数
// ============================================

TEST_SUITE("Deque Stack Constructors")
{
    TEST_CASE("Default constructor creates empty stack")
    {
        demo::stack<int> stk;
        CHECK(stk.empty());
        CHECK(stk.size() == 0);
    }

    TEST_CASE("Constructor with container")
    {
        demo::deque<int> dq{1, 2, 3, 4, 5};
        demo::stack<int> stk(dq);
        CHECK(stk.size() == 5);
        CHECK_FALSE(stk.empty());
        CHECK(stk.top() == 5);
    }

    TEST_CASE("Copy constructor")
    {
        demo::stack<int> stk1;
        stk1.push(1);
        stk1.push(2);
        stk1.push(3);

        demo::stack<int> stk2(stk1);
        CHECK(stk2.size() == 3);
        CHECK(stk2.top() == 3);

        stk2.pop();
        CHECK(stk2.top() == 2);
        CHECK(stk1.size() == 3);
        CHECK(stk1.top() == 3);
    }

    TEST_CASE("Move constructor")
    {
        demo::stack<int> stk1;
        stk1.push(1);
        stk1.push(2);
        stk1.push(3);
        auto size_before = stk1.size();

        demo::stack<int> stk2(std::move(stk1));
        CHECK(stk2.size() == size_before);
        CHECK(stk2.top() == 3);
        CHECK(stk1.empty());
    }
}

// ============================================
// 测试套件: 赋值操作符
// ============================================

TEST_SUITE("Deque Stack Assignment Operators")
{
    TEST_CASE("Copy assignment operator")
    {
        demo::stack<int> stk1;
        stk1.push(1);
        stk1.push(2);
        demo::stack<int> stk2;
        stk2.push(10);

        stk2 = stk1;

        CHECK(stk2.size() == 2);
        CHECK(stk2.top() == 2);
        stk2.pop();
        CHECK(stk2.top() == 1);
        CHECK(stk1.size() == 2);
        CHECK(stk1.top() == 2);
    }

    TEST_CASE("Move assignment operator")
    {
        demo::stack<int> stk1;
        stk1.push(1);
        stk1.push(2);
        auto             size_before = stk1.size();
        demo::stack<int> stk2;

        stk2 = std::move(stk1);

        CHECK(stk2.size() == size_before);
        CHECK(stk2.top() == 2);
        CHECK(stk1.empty());
    }

    TEST_CASE("Self assignment")
    {
        demo::stack<int> stk;
        stk.push(1);
        stk.push(2);
        stk.push(3);

        stk = stk;

        CHECK(stk.size() == 3);
        CHECK(stk.top() == 3);
    }
}

// ============================================
// 测试套件: 元素访问
// ============================================

TEST_SUITE("Deque Stack Element Access")
{
    TEST_CASE("top() returns reference to top element")
    {
        demo::stack<int> stk;
        stk.push(1);
        stk.push(2);
        stk.push(3);

        int& ref = stk.top();
        CHECK(ref == 3);

        ref = 42;
        CHECK(stk.top() == 42);
    }

    TEST_CASE("const top() returns const reference")
    {
        demo::stack<int> stk;
        stk.push(1);
        stk.push(2);

        const demo::stack<int>& const_stk = stk;
        const int&              ref       = const_stk.top();
        CHECK(ref == 2);
    }
}

// ============================================
// 测试套件: 容量操作
// ============================================

TEST_SUITE("Deque Stack Capacity")
{
    TEST_CASE("empty() returns true for empty stack")
    {
        demo::stack<int> stk;
        CHECK(stk.empty());
    }

    TEST_CASE("empty() returns false for non-empty stack")
    {
        demo::stack<int> stk;
        stk.push(1);
        CHECK_FALSE(stk.empty());
    }

    TEST_CASE("size() returns correct number of elements")
    {
        demo::stack<int> stk;
        CHECK(stk.size() == 0);

        stk.push(1);
        CHECK(stk.size() == 1);

        stk.push(2);
        CHECK(stk.size() == 2);

        stk.pop();
        CHECK(stk.size() == 1);
    }
}

// ============================================
// 测试套件: 修改器
// ============================================

TEST_SUITE("Deque Stack Modifiers")
{
    TEST_CASE("push() with const value")
    {
        demo::stack<int> stk;
        int              val = 42;
        stk.push(val);
        CHECK(stk.size() == 1);
        CHECK(stk.top() == 42);
    }

    TEST_CASE("push() with rvalue")
    {
        demo::stack<int> stk;
        stk.push(42);
        CHECK(stk.size() == 1);
        CHECK(stk.top() == 42);
    }

    TEST_CASE("emplace() constructs element in place")
    {
        demo::stack<std::string> stk;
        stk.emplace("hello");
        auto world = stk.emplace("world");
        CHECK(world == stk.top());

        CHECK(stk.size() == 2);
        CHECK(stk.top() == "world");
    }

    TEST_CASE("pop() removes top element")
    {
        demo::stack<int> stk;
        stk.push(1);
        stk.push(2);
        stk.push(3);

        CHECK(stk.size() == 3);
        stk.pop();
        CHECK(stk.size() == 2);
        CHECK(stk.top() == 2);
        stk.pop();
        CHECK(stk.size() == 1);
        CHECK(stk.top() == 1);
        stk.pop();
        CHECK(stk.size() == 0);
        CHECK(stk.empty());
    }

    TEST_CASE("swap() exchanges contents")
    {
        demo::stack<int> stk1;
        stk1.push(1);
        stk1.push(2);

        demo::stack<int> stk2;
        stk2.push(10);
        stk2.push(20);
        stk2.push(30);

        stk1.swap(stk2);

        CHECK(stk1.size() == 3);
        CHECK(stk1.top() == 30);
        CHECK(stk2.size() == 2);
        CHECK(stk2.top() == 2);
    }
}

// ============================================
// 测试套件: 全局比较运算符
// ============================================

TEST_SUITE("Deque Stack Comparison Operators")
{
    TEST_CASE("operator== compares contents")
    {
        demo::stack<int> stk1;
        stk1.push(1);
        stk1.push(2);

        demo::stack<int> stk2;
        stk2.push(1);
        stk2.push(2);

        demo::stack<int> stk3;
        stk3.push(1);
        stk3.push(3);

        CHECK(stk1 == stk2);
        CHECK_FALSE(stk1 == stk3);
    }

    TEST_CASE("operator!= compares contents")
    {
        demo::stack<int> stk1;
        stk1.push(1);
        stk1.push(2);

        demo::stack<int> stk2;
        stk2.push(1);
        stk2.push(2);

        demo::stack<int> stk3;
        stk3.push(1);
        stk3.push(3);

        CHECK_FALSE(stk1 != stk2);
        CHECK(stk1 != stk3);
    }

    TEST_CASE("operator== and != with different sizes")
    {
        demo::stack<int> stk1;
        stk1.push(1);

        demo::stack<int> stk2;
        stk2.push(1);
        stk2.push(2);

        CHECK_FALSE(stk1 == stk2);
        CHECK(stk1 != stk2);
    }
}

// ============================================
// 测试套件: 边界条件
// ============================================

TEST_SUITE("Deque Stack Edge Cases")
{
    TEST_CASE("push and pop single element")
    {
        demo::stack<int> stk;
        stk.push(42);
        CHECK(stk.size() == 1);
        CHECK(stk.top() == 42);
        stk.pop();
        CHECK(stk.empty());
    }

    TEST_CASE("push and pop many elements")
    {
        demo::stack<int> stk;
        for (int i = 0; i < 100; ++i)
        {
            stk.push(i);
        }
        CHECK(stk.size() == 100);

        for (int i = 99; i >= 0; --i)
        {
            CHECK(stk.top() == i);
            stk.pop();
        }
        CHECK(stk.empty());
    }

    TEST_CASE("stack of strings")
    {
        demo::stack<std::string> stk;
        stk.push("hello");
        stk.push("world");
        stk.push("!");

        CHECK(stk.size() == 3);
        CHECK(stk.top() == "!");

        stk.pop();
        CHECK(stk.top() == "world");
    }
}

// ============================================
// 测试套件: 性能测试
// ============================================

TEST_SUITE("Deque Stack Performance")
{
    TEST_CASE("push performance test")
    {
        demo::stack<int> stk;
        auto             start = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < 100000; ++i)
        {
            stk.push(i);
        }
        auto end      = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

        CHECK(stk.size() == 100000);
        CHECK(duration.count() < 5000);
    }

    TEST_CASE("push and pop performance test")
    {
        demo::stack<int> stk;
        auto             start = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < 100000; ++i)
        {
            stk.push(i);
        }
        for (int i = 0; i < 100000; ++i)
        {
            stk.pop();
        }
        auto end      = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

        CHECK(stk.empty());
        CHECK(duration.count() < 10000);
    }
}
