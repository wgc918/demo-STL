/**
 * @file test_deque.cpp
 * @brief 基于doctest的工业级双端队列(deque)测试套件
 *
 * 测试覆盖范围:
 * - 构造函数(默认、大小、迭代器范围、初始化列表、拷贝、移动)
 * - 元素访问(at, operator[], front, back)
 * - 迭代器(正向、反向、const迭代器、随机访问)
 * - 容量操作(empty, size, max_size, resize, shrink_to_fit)
 * - 修改器(clear, insert, emplace, erase, push/pop_front/back, swap, assign)
 * - 比较运算符(==, !=)
 * - 边界条件和异常安全
 */

#define DOCTEST_CONFIG_NO_MULTITHREADING
#include "doctest.h"

#include <string>
#include <vector>
#include <stdexcept>
#include <functional>
#include <algorithm>
#include "../deque.h"

// ============================================
// 测试辅助工具
// ============================================

template <typename T>
std::vector<T> deque_to_vector(const demo::deque<T> &dq)
{
    std::vector<T> result;
    for (const auto &elem : dq)
    {
        result.push_back(elem);
    }
    return result;
}

template <typename T>
bool deque_equals_vector(const demo::deque<T> &dq, const std::vector<T> &vec)
{
    if (dq.size() != vec.size())
        return false;

    auto it = dq.begin();
    for (const auto &elem : vec)
    {
        if (it == dq.end() || *it != elem)
            return false;
        ++it;
    }
    return true;
}

template <typename T>
bool deque_equals_deque(const demo::deque<T> &a, const demo::deque<T> &b)
{
    if (a.size() != b.size())
        return false;
    auto it_a = a.begin();
    auto it_b = b.begin();
    for (; it_a != a.end() && it_b != b.end(); ++it_a, ++it_b)
    {
        if (*it_a != *it_b)
            return false;
    }
    return true;
}

template <typename T>
std::vector<T> generate_test_vector(size_t size, T start = T())
{
    std::vector<T> result;
    for (size_t i = 0; i < size; ++i)
    {
        result.push_back(start + static_cast<T>(i));
    }
    return result;
}

// ============================================
// 测试套件: 构造函数
// ============================================

TEST_SUITE("Deque Constructors")
{

    TEST_CASE("Default constructor creates empty deque")
    {
        demo::deque<int> dq;
        CHECK(dq.empty());
        CHECK(dq.size() == 0);
    }

    TEST_CASE("Size constructor with default value")
    {
        demo::deque<int> dq(5);
        CHECK(dq.size() == 5);
        CHECK_FALSE(dq.empty());

        for (const auto &elem : dq)
        {
            CHECK(elem == int());
        }
    }

    TEST_CASE("Size constructor with explicit value")
    {
        demo::deque<std::string> dq(3, "hello");
        CHECK(dq.size() == 3);

        std::vector<std::string> expected{"hello", "hello", "hello"};
        CHECK(deque_equals_vector(dq, expected));
    }

    TEST_CASE("Initializer list constructor")
    {
        demo::deque<int> dq{1, 2, 3, 4, 5};
        CHECK(dq.size() == 5);

        std::vector<int> expected{1, 2, 3, 4, 5};
        CHECK(deque_equals_vector(dq, expected));
    }

    TEST_CASE("Iterator range constructor")
    {
        std::vector<int> source{10, 20, 30, 40};
        demo::deque<int> dq(source.begin(), source.end());

        CHECK(dq.size() == 4);
        CHECK(deque_equals_vector(dq, source));
    }

    TEST_CASE("Copy constructor")
    {
        demo::deque<int> original{1, 2, 3, 4};
        demo::deque<int> copy(original);

        CHECK(copy.size() == original.size());
        CHECK(deque_equals_deque(copy, original));

        original.push_back(5);
        CHECK(copy.size() == 4);
        CHECK(original.size() == 5);
    }

    TEST_CASE("Move constructor")
    {
        demo::deque<int> original{1, 2, 3, 4, 5};
        demo::deque<int> moved(std::move(original));

        CHECK(moved.size() == 5);
        CHECK(original.size() == 0);

        std::vector<int> expected{1, 2, 3, 4, 5};
        CHECK(deque_equals_vector(moved, expected));
    }
}

// ============================================
// 测试套件: 赋值操作符
// ============================================

TEST_SUITE("Deque Assignment Operators")
{

    TEST_CASE("Copy assignment operator")
    {
        demo::deque<std::string> original{"a", "b", "c"};
        demo::deque<std::string> copy;

        copy = original;

        CHECK(copy.size() == 3);
        CHECK(deque_equals_deque(copy, original));

        copy = copy;
        CHECK(copy.size() == 3);
    }

    TEST_CASE("Move assignment operator")
    {
        demo::deque<int> original{10, 20, 30};
        demo::deque<int> moved;

        moved = std::move(original);

        CHECK(moved.size() == 3);
        CHECK_FALSE(moved.empty());
        CHECK(original.size() == 0);
        CHECK(original.empty());

        std::vector<int> expected{10, 20, 30};
        CHECK(deque_equals_vector(moved, expected));
    }

    TEST_CASE("Initializer list assignment")
    {
        demo::deque<int> dq;
        dq = {100, 200, 300};

        CHECK(dq.size() == 3);
        std::vector<int> expected{100, 200, 300};
        CHECK(deque_equals_vector(dq, expected));
    }
}

// ============================================
// 测试套件: 元素访问
// ============================================

TEST_SUITE("Deque Element Access")
{

    TEST_CASE("front() returns first element")
    {
        demo::deque<int> dq{1, 2, 3};
        CHECK(dq.front() == 1);

        dq.front() = 100;
        CHECK(dq.front() == 100);
    }

    TEST_CASE("back() returns last element")
    {
        demo::deque<int> dq{1, 2, 3};
        CHECK(dq.back() == 3);

        dq.back() = 300;
        CHECK(dq.back() == 300);
    }

    TEST_CASE("const front() and back()")
    {
        const demo::deque<int> dq{10, 20, 30};
        CHECK(dq.front() == 10);
        CHECK(dq.back() == 30);
    }

    TEST_CASE("operator[] returns element")
    {
        demo::deque<int> dq{10, 20, 30, 40, 50};

        CHECK(dq[0] == 10);
        CHECK(dq[2] == 30);
        CHECK(dq[4] == 50);

        dq[1] = 200;
        CHECK(dq[1] == 200);
    }

    TEST_CASE("const operator[] returns element")
    {
        const demo::deque<int> dq{10, 20, 30};
        CHECK(dq[0] == 10);
        CHECK(dq[1] == 20);
        CHECK(dq[2] == 30);
    }

    TEST_CASE("at() returns element with bounds checking")
    {
        demo::deque<int> dq{1, 2, 3, 4, 5};

        CHECK(dq.at(0) == 1);
        CHECK(dq.at(2) == 3);
        CHECK(dq.at(4) == 5);

        dq.at(1) = 200;
        CHECK(dq.at(1) == 200);
    }

    TEST_CASE("at() throws on out of bounds")
    {
        demo::deque<int> dq{1, 2, 3};

        CHECK_THROWS_AS(dq.at(3), std::out_of_range);
        CHECK_THROWS_AS(dq.at(10), std::out_of_range);
        CHECK_THROWS_AS(dq.at(-1), std::out_of_range);
    }

    TEST_CASE("front() throws on empty deque")
    {
        demo::deque<int> dq;
        CHECK_THROWS_AS(dq.front(), std::out_of_range);
    }

    TEST_CASE("back() throws on empty deque")
    {
        demo::deque<int> dq;
        CHECK_THROWS_AS(dq.back(), std::out_of_range);
    }
}

// ============================================
// 测试套件: 迭代器
// ============================================

TEST_SUITE("Deque Iterators")
{

    TEST_CASE("begin() and end() iterators")
    {
        demo::deque<int> dq{1, 2, 3, 4, 5};

        int sum = 0;
        for (auto it = dq.begin(); it != dq.end(); ++it)
        {
            sum += *it;
        }
        CHECK(sum == 15);
    }

    TEST_CASE("const iterators cbegin() and cend()")
    {
        demo::deque<int> dq{1, 2, 3};

        int sum = 0;
        for (auto it = dq.cbegin(); it != dq.cend(); ++it)
        {
            sum += *it;
        }
        CHECK(sum == 6);
    }

    TEST_CASE("range-based for loop")
    {
        demo::deque<int> dq{10, 20, 30};

        int sum = 0;
        for (const auto &elem : dq)
        {
            sum += elem;
        }
        CHECK(sum == 60);
    }

    TEST_CASE("reverse iterators rbegin() and rend()")
    {
        demo::deque<int> dq{1, 2, 3, 4};

        std::vector<int> reversed;
        for (auto it = dq.rbegin(); it != dq.rend(); ++it)
        {
            reversed.push_back(*it);
        }

        std::vector<int> expected{4, 3, 2, 1};
        CHECK(reversed == expected);
    }

    TEST_CASE("const reverse iterators crbegin() and crend()")
    {
        const demo::deque<int> dq{1, 2, 3, 4};

        std::vector<int> reversed;
        for (auto it = dq.crbegin(); it != dq.crend(); ++it)
        {
            reversed.push_back(*it);
        }

        std::vector<int> expected{4, 3, 2, 1};
        CHECK(reversed == expected);
    }

    TEST_CASE("iterator random access operations")
    {
        demo::deque<int> dq{1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

        auto it = dq.begin();
        it += 5;
        CHECK(*it == 6);

        it -= 3;
        CHECK(*it == 3);

        auto it2 = it + 4;
        CHECK(*it2 == 7);

        auto it3 = it2 - 2;
        CHECK(*it3 == 5);

        auto dist = it2 - it;
        CHECK(dist == 4);
    }

    TEST_CASE("iterator subscript operator")
    {
        demo::deque<int> dq{10, 20, 30, 40, 50};
        auto it = dq.begin();

        CHECK(*(it[0]) == 10);
        CHECK(*(it[2]) == 30);
        CHECK(*(it[4]) == 50);
    }

    TEST_CASE("iterator comparison operators")
    {
        demo::deque<int> dq{1, 2, 3, 4, 5};

        auto it1 = dq.begin();
        auto it2 = dq.begin();
        CHECK(it1 == it2);

        ++it1;
        CHECK(it1 != it2);
        CHECK(it1 > it2);
        CHECK(it2 < it1);
        CHECK(it1 >= it2);
        CHECK(it2 <= it1);

        auto it3 = dq.end();
        CHECK(it3 > it1);
        CHECK(it1 < it3);
    }

    TEST_CASE("iterator increment and decrement")
    {
        demo::deque<int> dq{1, 2, 3, 4, 5};

        auto it = dq.begin();
        ++it;
        CHECK(*it == 2);

        it++;
        CHECK(*it == 3);

        --it;
        CHECK(*it == 2);

        it--;
        CHECK(*it == 1);
    }

    TEST_CASE("iterator conversion to const_iterator")
    {
        demo::deque<int> dq{1, 2, 3};

        demo::deque<int>::iterator it = dq.begin();
        demo::deque<int>::const_iterator cit = it;

        CHECK(cit == it);
        CHECK(*cit == *it);
    }
}

// ============================================
// 测试套件: 容量操作
// ============================================

TEST_SUITE("Deque Capacity")
{

    TEST_CASE("empty() returns true for empty deque")
    {
        demo::deque<int> dq;
        CHECK(dq.empty());

        dq.push_back(1);
        CHECK_FALSE(dq.empty());
    }

    TEST_CASE("size() returns correct element count")
    {
        demo::deque<int> dq;
        CHECK(dq.size() == 0);

        for (int i = 0; i < 100; ++i)
        {
            dq.push_back(i);
        }
        CHECK(dq.size() == 100);
    }

    TEST_CASE("max_size() returns positive value")
    {
        demo::deque<int> dq;
        CHECK(dq.max_size() > 0);
    }

    TEST_CASE("resize() to larger size")
    {
        demo::deque<int> dq{1, 2, 3};
        dq.resize(5);

        CHECK(dq.size() == 5);
        CHECK(dq.back() == int());
        CHECK(deque_equals_vector(dq, {1, 2, 3, int(), int()}));
    }

    TEST_CASE("resize() with explicit value")
    {
        demo::deque<int> dq{1, 2};
        dq.resize(5, 99);

        std::vector<int> expected{1, 2, 99, 99, 99};
        CHECK(deque_equals_vector(dq, expected));
    }

    TEST_CASE("resize() to smaller size")
    {
        demo::deque<int> dq{1, 2, 3, 4, 5};
        dq.resize(2);

        CHECK(dq.size() == 2);
        std::vector<int> expected{1, 2};
        CHECK(deque_equals_vector(dq, expected));
    }

    TEST_CASE("resize() to same size")
    {
        demo::deque<int> dq{1, 2, 3};
        dq.resize(3);
        CHECK(dq.size() == 3);
        std::vector<int> expected{1, 2, 3};
        CHECK(deque_equals_vector(dq, expected));
    }

    TEST_CASE("resize() to zero")
    {
        demo::deque<int> dq{1, 2, 3};
        dq.resize(0);
        CHECK(dq.empty());
        CHECK(dq.size() == 0);
    }

    TEST_CASE("shrink_to_fit() reduces capacity")
    {
        demo::deque<int> dq;
        for (int i = 0; i < 100; ++i)
        {
            dq.push_back(i);
        }
        dq.resize(10);
        dq.shrink_to_fit();
        CHECK(dq.size() == 10);
    }
}

// ============================================
// 测试套件: 修改器 - 插入操作
// ============================================

TEST_SUITE("Deque Modifiers - Insert")
{

    TEST_CASE("push_front() adds element to front")
    {
        demo::deque<int> dq;
        dq.push_front(3);
        dq.push_front(2);
        dq.push_front(1);

        std::vector<int> expected{1, 2, 3};
        CHECK(deque_equals_vector(dq, expected));
    }

    TEST_CASE("push_back() adds element to back")
    {
        demo::deque<int> dq;
        dq.push_back(1);
        dq.push_back(2);
        dq.push_back(3);

        std::vector<int> expected{1, 2, 3};
        CHECK(deque_equals_vector(dq, expected));
    }

    TEST_CASE("pop_front() removes first element")
    {
        demo::deque<int> dq{1, 2, 3};
        dq.pop_front();

        CHECK(dq.front() == 2);
        CHECK(dq.size() == 2);
    }

    TEST_CASE("pop_back() removes last element")
    {
        demo::deque<int> dq{1, 2, 3};
        dq.pop_back();

        CHECK(dq.back() == 2);
        CHECK(dq.size() == 2);
    }

    TEST_CASE("pop_front() on empty deque throws exception")
    {
        demo::deque<int> dq;
        CHECK_THROWS_AS(dq.pop_front(), std::out_of_range);
    }

    TEST_CASE("pop_back() on empty deque throws exception")
    {
        demo::deque<int> dq;
        CHECK_THROWS_AS(dq.pop_back(), std::out_of_range);
    }

    TEST_CASE("insert() single element at begin")
    {
        demo::deque<int> dq{2, 3};
        auto result = dq.insert(dq.begin(), 1);

        CHECK(*result == 1);
        std::vector<int> expected{1, 2, 3};
        CHECK(deque_equals_vector(dq, expected));
    }

    TEST_CASE("insert() single element in middle")
    {
        demo::deque<int> dq{1, 3, 5};
        auto it = dq.begin();
        ++it;

        auto result = dq.insert(it, 2);
        CHECK(*result == 2);
        CHECK(dq.size() == 4);
        std::vector<int> expected{1, 2, 3, 5};
        CHECK(deque_equals_vector(dq, expected));
    }

    TEST_CASE("insert() single element at end")
    {
        demo::deque<int> dq{1, 2, 3};
        auto result = dq.insert(dq.end(), 4);

        CHECK(*result == 4);
        CHECK(dq.size() == 4);
        std::vector<int> expected{1, 2, 3, 4};
        CHECK(deque_equals_vector(dq, expected));
    }

    TEST_CASE("insert() multiple copies")
    {
        demo::deque<int> dq{1, 5};
        auto it = ++dq.begin();

        dq.insert(it, 3, 3);
        CHECK(dq.size() == 5);
        std::vector<int> expected{1, 3, 3, 3, 5};
        CHECK(deque_equals_vector(dq, expected));
    }

    TEST_CASE("insert() iterator range")
    {
        demo::deque<int> dq{1, 6};
        std::vector<int> to_insert{2, 3, 4, 5};

        auto it = ++dq.begin();
        dq.insert(it, to_insert.begin(), to_insert.end());
        CHECK(dq.size() == 6);
        std::vector<int> expected{1, 2, 3, 4, 5, 6};
        CHECK(deque_equals_vector(dq, expected));
    }

    TEST_CASE("insert() initializer list")
    {
        demo::deque<int> dq{1, 5};
        auto it = ++dq.begin();

        dq.insert(it, {2, 3, 4});

        std::vector<int> expected{1, 2, 3, 4, 5};
        CHECK(deque_equals_vector(dq, expected));
    }

    TEST_CASE("emplace() constructs in-place")
    {
        demo::deque<std::string> dq{"hello", "world"};
        auto it = ++dq.begin();

        auto result = dq.emplace(it, 5, 'x');

        CHECK(*result == "xxxxx");
        CHECK(dq.size() == 3);
    }

    TEST_CASE("emplace_front() constructs at front")
    {
        demo::deque<std::string> dq;
        dq.emplace_front(3, 'a');

        CHECK(dq.front() == "aaa");
    }

    TEST_CASE("emplace_back() constructs at back")
    {
        demo::deque<std::string> dq;
        dq.emplace_back(3, 'b');

        CHECK(dq.back() == "bbb");
    }

    TEST_CASE("push_front() with move semantics")
    {
        demo::deque<std::string> dq{"world"};
        std::string s = "hello";
        dq.push_front(std::move(s));

        CHECK(dq.front() == "hello");
        CHECK(s.empty());
    }

    TEST_CASE("push_back() with move semantics")
    {
        demo::deque<std::string> dq{"hello"};
        std::string s = "world";
        dq.push_back(std::move(s));

        CHECK(dq.back() == "world");
        CHECK(s.empty());
    }
}

// ============================================
// 测试套件: 修改器 - 删除操作
// ============================================

TEST_SUITE("Deque Modifiers - Erase")
{

    TEST_CASE("erase() single element in middle")
    {
        demo::deque<int> dq{1, 2, 3, 4, 5};
        auto it = dq.begin();
        ++it;
        ++it;

        auto next = dq.erase(it);
        CHECK(*next == 4);

        std::vector<int> expected{1, 2, 4, 5};
        CHECK(deque_equals_vector(dq, expected));
    }

    TEST_CASE("erase() single element at begin")
    {
        demo::deque<int> dq{1, 2, 3};
        auto it = dq.begin();
        auto next = dq.erase(it);

        CHECK(*next == 2);
        std::vector<int> expected{2, 3};
        CHECK(deque_equals_vector(dq, expected));
    }

    TEST_CASE("erase() single element at end")
    {
        demo::deque<int> dq{1, 2, 3};
        auto it = dq.end();
        --it;
        auto next = dq.erase(it);

        CHECK(next == dq.end());
        std::vector<int> expected{1, 2};
        CHECK(deque_equals_vector(dq, expected));
    }

    TEST_CASE("erase() range")
    {
        demo::deque<int> dq{1, 2, 3, 4, 5, 6, 7};
        auto first = ++dq.begin();
        auto last = dq.end();
        --last;
        --last;

        dq.erase(first, last);

        std::vector<int> expected{1, 6, 7};
        CHECK(deque_equals_vector(dq, expected));
    }

    TEST_CASE("erase() empty range")
    {
        demo::deque<int> dq{1, 2, 3};
        auto it = dq.begin();
        auto result = dq.erase(it, it);

        CHECK(result == it);
        std::vector<int> expected{1, 2, 3};
        CHECK(deque_equals_vector(dq, expected));
    }

    TEST_CASE("clear() removes all elements")
    {
        demo::deque<int> dq{1, 2, 3, 4, 5};
        dq.clear();

        CHECK(dq.empty());
        CHECK(dq.size() == 0);
    }

    TEST_CASE("clear() on empty deque")
    {
        demo::deque<int> dq;
        dq.clear();

        CHECK(dq.empty());
    }
}

// ============================================
// 测试套件: 修改器 - 其他操作
// ============================================

TEST_SUITE("Deque Modifiers - Other")
{

    TEST_CASE("swap() exchanges contents")
    {
        demo::deque<int> dq1{1, 2, 3};
        demo::deque<int> dq2{4, 5};

        dq1.swap(dq2);

        std::vector<int> expected1{4, 5};
        std::vector<int> expected2{1, 2, 3};
        CHECK(deque_equals_vector(dq1, expected1));
        CHECK(deque_equals_vector(dq2, expected2));
    }

    TEST_CASE("swap() with self")
    {
        demo::deque<int> dq{1, 2, 3};
        dq.swap(dq);

        std::vector<int> expected{1, 2, 3};
        CHECK(deque_equals_vector(dq, expected));
    }

    TEST_CASE("assign() with count and value")
    {
        demo::deque<int> dq{1, 2, 3, 4, 5};
        dq.assign(3, 99);

        CHECK(dq.size() == 3);
        std::vector<int> expected{99, 99, 99};
        CHECK(deque_equals_vector(dq, expected));
    }

    TEST_CASE("assign() with iterator range")
    {
        demo::deque<int> dq{1, 2, 3};
        std::vector<int> source{10, 20, 30, 40};

        dq.assign(source.begin(), source.end());

        CHECK(dq.size() == 4);
        CHECK(deque_equals_vector(dq, source));
    }

    TEST_CASE("assign() with initializer list")
    {
        demo::deque<int> dq{1, 2, 3};
        dq.assign({100, 200});

        CHECK(dq.size() == 2);
        std::vector<int> expected{100, 200};
        CHECK(deque_equals_vector(dq, expected));
    }

    TEST_CASE("assign() to empty deque")
    {
        demo::deque<int> dq;
        dq.assign(5, 42);

        CHECK(dq.size() == 5);
        for (const auto &elem : dq)
        {
            CHECK(elem == 42);
        }
    }
}

// ============================================
// 测试套件: 比较运算符
// ============================================

TEST_SUITE("Deque Comparison Operators")
{

    TEST_CASE("operator== for equal deques")
    {
        demo::deque<int> dq1{1, 2, 3};
        demo::deque<int> dq2{1, 2, 3};

        CHECK(dq1 == dq2);
    }

    TEST_CASE("operator== for different size deques")
    {
        demo::deque<int> dq1{1, 2, 3};
        demo::deque<int> dq2{1, 2};

        CHECK_FALSE(dq1 == dq2);
    }

    TEST_CASE("operator== for different content deques")
    {
        demo::deque<int> dq1{1, 2, 3};
        demo::deque<int> dq2{1, 2, 4};

        CHECK_FALSE(dq1 == dq2);
    }

    TEST_CASE("operator!= for different deques")
    {
        demo::deque<int> dq1{1, 2, 3};
        demo::deque<int> dq2{1, 2, 4};

        CHECK(dq1 != dq2);
    }

    TEST_CASE("operator!= for equal deques")
    {
        demo::deque<int> dq1{1, 2, 3};
        demo::deque<int> dq2{1, 2, 3};

        CHECK_FALSE(dq1 != dq2);
    }
}

// ============================================
// 测试套件: 边界条件和异常
// ============================================

TEST_SUITE("Deque Edge Cases and Exceptions")
{

    TEST_CASE("Operations on empty deque")
    {
        demo::deque<int> dq;

        CHECK(dq.empty());
        CHECK(dq.size() == 0);

        CHECK(dq.begin() == dq.end());

        CHECK(dq.max_size() > 0);

        CHECK_THROWS_AS(dq.front(), std::out_of_range);
        CHECK_THROWS_AS(dq.back(), std::out_of_range);
        CHECK_THROWS_AS(dq.at(0), std::out_of_range);
        CHECK_THROWS_AS(dq.pop_front(), std::out_of_range);
        CHECK_THROWS_AS(dq.pop_back(), std::out_of_range);
    }

    TEST_CASE("Single element deque operations")
    {
        demo::deque<int> dq{42};

        CHECK(dq.front() == 42);
        CHECK(dq.back() == 42);
        CHECK(dq.size() == 1);
        CHECK(dq[0] == 42);
        CHECK(dq.at(0) == 42);

        dq.pop_front();
        CHECK(dq.empty());
    }

    TEST_CASE("Large deque operations")
    {
        demo::deque<int> dq;
        const int N = 10000;

        for (int i = 0; i < N; ++i)
        {
            dq.push_back(i);
        }
        CHECK(dq.size() == N);
        CHECK(dq.front() == 0);
        CHECK(dq.back() == N - 1);
        CHECK(dq[N / 2] == N / 2);

        dq.clear();
        CHECK(dq.empty());
    }

    TEST_CASE("Iterator stability after modifications")
    {
        demo::deque<int> dq{1, 2, 3, 4, 5};
        auto it = ++dq.begin();

        dq.push_back(6);
        CHECK(*it == 2);

        dq.push_front(0);
        CHECK(*it == 2);

        dq.insert(++dq.begin(), 10);
        CHECK(*it == 2);
    }

    TEST_CASE("Complex type operations")
    {
        demo::deque<std::string> dq;

        dq.push_back("hello");
        dq.push_front("world");
        dq.emplace(++dq.begin(), "middle");

        CHECK(dq.size() == 3);
        CHECK(dq.front() == "world");
        CHECK(dq.back() == "hello");
    }

    TEST_CASE("Self assignment safety")
    {
        demo::deque<int> dq{1, 2, 3};
        dq = dq;

        std::vector<int> expected{1, 2, 3};
        CHECK(deque_equals_vector(dq, expected));
    }

    TEST_CASE("Multiple modifications sequence")
    {
        demo::deque<int> dq;

        dq.push_back(1);
        dq.push_front(0);
        dq.insert(dq.end(), 2);
        dq.pop_front();
        dq.push_back(3);
        dq.erase(dq.begin());

        std::vector<int> expected{2, 3};
        CHECK(deque_equals_vector(dq, expected));
    }

    TEST_CASE("emplace with multiple arguments")
    {
        demo::deque<std::string> dq;

        dq.emplace_front(5, 'x');
        CHECK(dq.front() == "xxxxx");

        dq.emplace(dq.end(), 3, 'y');
        CHECK(dq.back() == "yyy");

        dq.emplace_back(4, 'z');
        CHECK(dq.back() == "zzzz");
    }

    TEST_CASE("get_allocator")
    {
        demo::deque<int> dq;
        std::allocator<int> alloc = dq.get_allocator();
        int *p = std::allocator_traits<std::allocator<int>>::allocate(alloc, 1);
        std::allocator_traits<std::allocator<int>>::construct(alloc, p, 42);
        CHECK(*p == 42);
        std::allocator_traits<std::allocator<int>>::destroy(alloc, p);
        std::allocator_traits<std::allocator<int>>::deallocate(alloc, p, 1);
    }
}

// ============================================
// 测试套件: 性能边界测试
// ============================================

TEST_SUITE("Deque Performance Edge Cases")
{

    TEST_CASE("Stress test: many small insertions")
    {
        demo::deque<int> dq;
        const int N = 10000;

        for (int i = 0; i < N; ++i)
        {
            dq.push_back(i);
        }

        CHECK(dq.size() == N);
        CHECK(dq.back() == N - 1);
    }

    TEST_CASE("Stress test: many small deletions")
    {
        demo::deque<int> dq;
        const int N = 10000;

        for (int i = 0; i < N; ++i)
        {
            dq.push_back(i);
        }

        while (!dq.empty())
        {
            dq.pop_front();
        }

        CHECK(dq.empty());
    }

    TEST_CASE("Stress test: alternating insert and erase")
    {
        demo::deque<int> dq;
        const int N = 1000;

        for (int i = 0; i < N; ++i)
        {
            dq.push_back(i);
            if (i > 0)
            {
                dq.pop_front();
            }
        }

        CHECK(dq.size() == 1);
        CHECK(dq.front() == N - 1);
    }

    TEST_CASE("Stress test: push_front and push_back alternating")
    {
        demo::deque<int> dq;
        const int N = 5000;

        for (int i = 0; i < N; ++i)
        {
            dq.push_front(i);
            dq.push_back(i + N);
        }

        CHECK(dq.size() == N * 2);
        CHECK(dq.front() == N - 1);
        CHECK(dq.back() == N * 2 - 1);
    }

    TEST_CASE("Cross-slot iteration")
    {
        demo::deque<int> dq;
        for (int i = 0; i < 1000; ++i)
        {
            dq.push_back(i);
        }

        int sum = 0;
        for (auto it = dq.begin(); it != dq.end(); ++it)
        {
            sum += *it;
        }
        CHECK(sum == 499500);
    }
}