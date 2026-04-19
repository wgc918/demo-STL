/**
 * @file test_vector.cpp
 * @brief 基于doctest的工业级向量(vector)测试套件
 *
 * 测试覆盖范围:
 * - 构造函数(默认、大小、大小+值、迭代器范围、初始化列表、拷贝、移动)
 * - 赋值操作符(拷贝赋值、移动赋值、初始化列表赋值)
 * - 元素访问(at、operator[]、front、back、data)
 * - 迭代器(正向、const迭代器、反向迭代器)
 * - 容量操作(empty, size, max_size, capacity, reserve, shrink_to_fit)
 * - 修改器(clear, insert, emplace, erase, push_back, emplace_back, pop_back, resize, swap, assign)
 * - 全局比较运算符(==, !=)
 * - 边界条件和异常安全
 */

#define DOCTEST_CONFIG_NO_MULTITHREADING
#include "doctest.h"

#include <string>
#include <vector>
#include <stdexcept>
#include <algorithm>
#include "../vector.h"

// ============================================
// 测试辅助工具
// ============================================

/**
 * @brief 将demo::vector转换为std::vector便于比较
 */
template <typename T>
std::vector<T> to_std_vector(const demo::vector<T> &vec)
{
    std::vector<T> result;
    for (const auto &elem : vec)
    {
        result.push_back(elem);
    }
    return result;
}

/**
 * @brief 比较demo::vector与std::vector内容是否一致
 */
template <typename T>
bool vector_equals_std(const demo::vector<T> &demo_vec, const std::vector<T> &std_vec)
{
    if (demo_vec.size() != std_vec.size())
        return false;

    for (size_t i = 0; i < demo_vec.size(); ++i)
    {
        if (demo_vec[i] != std_vec[i])
            return false;
    }
    return true;
}

// ============================================
// 测试套件: 构造函数
// ============================================

TEST_SUITE("Vector Constructors")
{
    TEST_CASE("Default constructor creates empty vector")
    {
        demo::vector<int> vec;
        CHECK(vec.empty());
        CHECK(vec.size() == 0);
        CHECK(vec.capacity() == 0);
    }

    TEST_CASE("Size constructor with default value")
    {
        demo::vector<int> vec(5);
        CHECK_FALSE(vec.empty());
        CHECK(vec.size() == 5);
        CHECK(vec.capacity() == 5);

        for (size_t i = 0; i < vec.size(); ++i)
        {
            CHECK(vec[i] == int());
        }
    }

    TEST_CASE("Size constructor with explicit value")
    {
        demo::vector<std::string> vec(3, "hello");
        CHECK(vec.size() == 3);
        CHECK(vec.capacity() == 3);

        std::vector<std::string> expected{"hello", "hello", "hello"};
        CHECK(vector_equals_std(vec, expected));
    }

    TEST_CASE("Initializer list constructor")
    {
        demo::vector<int> vec{1, 2, 3, 4, 5};
        CHECK(vec.size() == 5);
        CHECK(vec.capacity() == 5);

        std::vector<int> expected{1, 2, 3, 4, 5};
        CHECK(vector_equals_std(vec, expected));
    }

    TEST_CASE("Iterator range constructor")
    {
        std::vector<int> source{10, 20, 30, 40};
        demo::vector<int> vec(source.begin(), source.end());

        CHECK(vec.size() == 4);
        CHECK(vec.capacity() == 4);
        CHECK(vector_equals_std(vec, source));
    }

    TEST_CASE("Copy constructor")
    {
        demo::vector<int> original{1, 2, 3, 4};
        demo::vector<int> copy(original);

        CHECK(copy.size() == original.size());
        CHECK(copy.capacity() == original.capacity());
        CHECK(vector_equals_std(copy, to_std_vector(original)));

        original.push_back(5);
        CHECK(copy.size() == 4);
        CHECK(original.size() == 5);
    }

    TEST_CASE("Move constructor")
    {
        demo::vector<int> original{1, 2, 3, 4, 5};
        demo::vector<int> moved(std::move(original));

        CHECK(moved.size() == 5);
        CHECK(moved.capacity() == 5);
        CHECK(original.empty());
        CHECK(original.capacity() == 0);

        std::vector<int> expected{1, 2, 3, 4, 5};
        CHECK(vector_equals_std(moved, expected));
    }
}

// ============================================
// 测试套件: 赋值操作符
// ============================================

TEST_SUITE("Vector Assignment Operators")
{
    TEST_CASE("Copy assignment operator")
    {
        demo::vector<std::string> original{"a", "b", "c"};
        demo::vector<std::string> copy;

        copy = original;

        CHECK(copy.size() == 3);
        CHECK(vector_equals_std(copy, to_std_vector(original)));

        copy = copy;
        CHECK(copy.size() == 3);
    }

    TEST_CASE("Move assignment operator")
    {
        demo::vector<int> original{10, 20, 30};
        demo::vector<int> moved;

        moved = std::move(original);

        CHECK(moved.size() == 3);
        CHECK_FALSE(moved.empty());
        CHECK(original.empty());

        std::vector<int> expected{10, 20, 30};
        CHECK(vector_equals_std(moved, expected));
    }

    TEST_CASE("Initializer list assignment")
    {
        demo::vector<int> vec;
        vec = {100, 200, 300};

        CHECK(vec.size() == 3);
        std::vector<int> expected{100, 200, 300};
        CHECK(vector_equals_std(vec, expected));
    }
}

// ============================================
// 测试套件: 元素访问
// ============================================

TEST_SUITE("Vector Element Access")
{
    TEST_CASE("operator[] returns element")
    {
        demo::vector<int> vec{1, 2, 3, 4, 5};
        CHECK(vec[0] == 1);
        CHECK(vec[2] == 3);
        CHECK(vec[4] == 5);

        vec[1] = 20;
        CHECK(vec[1] == 20);
    }

    TEST_CASE("const operator[]")
    {
        const demo::vector<int> vec{10, 20, 30};
        CHECK(vec[0] == 10);
        CHECK(vec[2] == 30);
    }

    TEST_CASE("at() returns element with bounds checking")
    {
        demo::vector<int> vec{1, 2, 3};
        CHECK(vec.at(0) == 1);
        CHECK(vec.at(1) == 2);
        CHECK(vec.at(2) == 3);

        CHECK_THROWS_AS(vec.at(3), std::out_of_range);
        CHECK_THROWS_AS(vec.at(-1), std::out_of_range);
    }

    TEST_CASE("const at()")
    {
        const demo::vector<int> vec{10, 20, 30};
        CHECK(vec.at(0) == 10);
        CHECK_THROWS_AS(vec.at(3), std::out_of_range);
    }

    TEST_CASE("front() returns first element")
    {
        demo::vector<int> vec{1, 2, 3};
        CHECK(vec.front() == 1);

        vec.front() = 100;
        CHECK(vec.front() == 100);
    }

    TEST_CASE("const front()")
    {
        const demo::vector<int> vec{10, 20, 30};
        CHECK(vec.front() == 10);
    }

    TEST_CASE("front() on empty vector throws")
    {
        demo::vector<int> vec;
        CHECK_THROWS_AS(vec.front(), std::out_of_range);
    }

    TEST_CASE("back() returns last element")
    {
        demo::vector<int> vec{1, 2, 3};
        CHECK(vec.back() == 3);

        vec.back() = 300;
        CHECK(vec.back() == 300);
    }

    TEST_CASE("const back()")
    {
        const demo::vector<int> vec{10, 20, 30};
        CHECK(vec.back() == 30);
    }

    TEST_CASE("back() on empty vector throws")
    {
        demo::vector<int> vec;
        CHECK_THROWS_AS(vec.back(), std::out_of_range);
    }

    TEST_CASE("data() returns pointer to underlying array")
    {
        demo::vector<int> vec{1, 2, 3, 4, 5};
        int *ptr = vec.data();

        CHECK(ptr != nullptr);
        CHECK(*ptr == 1);
        CHECK(*(ptr + 2) == 3);
        CHECK(*(ptr + 4) == 5);
    }

    TEST_CASE("const data()")
    {
        const demo::vector<int> vec{1, 2, 3};
        const int *ptr = vec.data();

        CHECK(ptr != nullptr);
        CHECK(*ptr == 1);
    }
}

// ============================================
// 测试套件: 迭代器
// ============================================

TEST_SUITE("Vector Iterators")
{
    TEST_CASE("begin() and end() iterators")
    {
        demo::vector<int> vec{1, 2, 3, 4, 5};

        int sum = 0;
        for (auto it = vec.begin(); it != vec.end(); ++it)
        {
            sum += *it;
        }
        CHECK(sum == 15);
    }

    TEST_CASE("const iterators cbegin() and cend()")
    {
        demo::vector<int> vec{1, 2, 3};

        int sum = 0;
        for (auto it = vec.cbegin(); it != vec.cend(); ++it)
        {
            sum += *it;
        }
        CHECK(sum == 6);
    }

    TEST_CASE("range-based for loop")
    {
        demo::vector<int> vec{10, 20, 30};

        int sum = 0;
        for (const auto &elem : vec)
        {
            sum += elem;
        }
        CHECK(sum == 60);
    }

    TEST_CASE("reverse iterators rbegin() and rend()")
    {
        demo::vector<int> vec{1, 2, 3, 4, 5};

        int sum = 0;
        for (auto it = vec.rbegin(); it != vec.rend(); ++it)
        {
            sum += *it;
        }
        CHECK(sum == 15);
    }

    TEST_CASE("const reverse iterators crbegin() and crend()")
    {
        const demo::vector<int> vec{1, 2, 3};

        int sum = 0;
        for (auto it = vec.crbegin(); it != vec.crend(); ++it)
        {
            sum += *it;
        }
        CHECK(sum == 6);
    }

    TEST_CASE("iterator arithmetic operations")
    {
        demo::vector<int> vec{1, 2, 3, 4, 5};

        auto it = vec.begin();
        CHECK(*it == 1);

        ++it;
        CHECK(*it == 2);

        it++;
        CHECK(*it == 3);

        --it;
        CHECK(*it == 2);

        it--;
        CHECK(*it == 1);

        it += 2;
        CHECK(*it == 3);

        it -= 1;
        CHECK(*it == 2);

        auto it2 = it + 2;
        CHECK(*it2 == 4);

        auto it3 = it2 - 1;
        CHECK(*it3 == 3);

        auto diff = it2 - it;
        CHECK(diff == 2);
    }

    TEST_CASE("iterator comparison operators")
    {
        demo::vector<int> vec{1, 2, 3};

        auto it1 = vec.begin();
        auto it2 = vec.begin();
        CHECK(it1 == it2);

        ++it1;
        CHECK(it1 != it2);
        CHECK(it1 > it2);
        CHECK(it2 < it1);
        CHECK(it1 >= it2);
        CHECK(it2 <= it1);

        auto it3 = vec.end();
        CHECK(it3 != it1);
        CHECK(it3 > it1);
    }

    TEST_CASE("iterator conversion to const_iterator")
    {
        demo::vector<int> vec{1, 2, 3};

        demo::vector<int>::iterator it = vec.begin();
        demo::vector<int>::const_iterator cit = it;

        CHECK(cit == it);
        CHECK(*cit == *it);
    }

    TEST_CASE("iterator operator[]")
    {
        demo::vector<int> vec{10, 20, 30, 40, 50};
        auto it = vec.begin();

        CHECK(it[0] == 10);
        CHECK(it[2] == 30);
        CHECK(it[4] == 50);
    }
}

// ============================================
// 测试套件: 容量操作
// ============================================

TEST_SUITE("Vector Capacity")
{
    TEST_CASE("empty() returns true for empty vector")
    {
        demo::vector<int> vec;
        CHECK(vec.empty());

        vec.push_back(1);
        CHECK_FALSE(vec.empty());
    }

    TEST_CASE("size() returns number of elements")
    {
        demo::vector<int> vec;
        CHECK(vec.size() == 0);

        vec.push_back(1);
        CHECK(vec.size() == 1);

        vec.push_back(2);
        vec.push_back(3);
        CHECK(vec.size() == 3);
    }

    TEST_CASE("max_size() returns positive value")
    {
        demo::vector<int> vec;
        CHECK(vec.max_size() > 0);
    }

    TEST_CASE("capacity() returns allocated space")
    {
        demo::vector<int> vec;
        CHECK(vec.capacity() == 0);

        vec.push_back(1);
        CHECK(vec.capacity() >= 1);

        vec.push_back(2);
        vec.push_back(3);
        CHECK(vec.capacity() >= 3);
    }

    TEST_CASE("reserve() increases capacity")
    {
        demo::vector<int> vec;
        vec.reserve(10);

        CHECK(vec.capacity() >= 10);
        CHECK(vec.size() == 0);
    }

    TEST_CASE("reserve() with smaller value does nothing")
    {
        demo::vector<int> vec{1, 2, 3, 4, 5};
        size_t old_capacity = vec.capacity();

        vec.reserve(3);

        CHECK(vec.capacity() == old_capacity);
    }

    TEST_CASE("shrink_to_fit() reduces capacity to size")
    {
        demo::vector<int> vec{1, 2, 3, 4, 5};
        vec.reserve(20);
        CHECK(vec.capacity() >= 20);

        vec.shrink_to_fit();
        CHECK(vec.capacity() == vec.size());
    }

    TEST_CASE("shrink_to_fit() on empty vector")
    {
        demo::vector<int> vec;
        vec.shrink_to_fit();
        CHECK(vec.capacity() == 0);
    }

    TEST_CASE("resize() to larger size")
    {
        demo::vector<int> vec{1, 2, 3};
        vec.resize(5);

        CHECK(vec.size() == 5);
        std::vector<int> expected{1, 2, 3, 0, 0};
        CHECK(vector_equals_std(vec, expected));
    }

    TEST_CASE("resize() with explicit value")
    {
        demo::vector<int> vec{1, 2};
        vec.resize(5, 99);

        CHECK(vec.size() == 5);
        std::vector<int> expected{1, 2, 99, 99, 99};
        CHECK(vector_equals_std(vec, expected));
    }

    TEST_CASE("resize() to smaller size")
    {
        demo::vector<int> vec{1, 2, 3, 4, 5};
        vec.resize(2);

        CHECK(vec.size() == 2);
        std::vector<int> expected{1, 2};
        CHECK(vector_equals_std(vec, expected));
    }

    TEST_CASE("resize() to same size")
    {
        demo::vector<int> vec{1, 2, 3};
        vec.resize(3);
        CHECK(vec.size() == 3);
        std::vector<int> expected{1, 2, 3};
        CHECK(vector_equals_std(vec, expected));
    }

    TEST_CASE("resize() to zero")
    {
        demo::vector<int> vec{1, 2, 3};
        vec.resize(0);
        CHECK(vec.empty());
        CHECK(vec.size() == 0);
    }
}

// ============================================
// 测试套件: 修改器 - 添加和插入
// ============================================

TEST_SUITE("Vector Modifiers - Insert")
{
    TEST_CASE("push_back() adds element to end")
    {
        demo::vector<int> vec;
        vec.push_back(1);
        vec.push_back(2);
        vec.push_back(3);

        std::vector<int> expected{1, 2, 3};
        CHECK(vector_equals_std(vec, expected));
    }

    TEST_CASE("push_back() with move semantics")
    {
        demo::vector<std::string> vec;
        std::string s = "hello";
        vec.push_back(std::move(s));

        CHECK(vec.size() == 1);
        CHECK(vec[0] == "hello");
        CHECK(s.empty());
    }

    TEST_CASE("emplace_back() constructs in-place")
    {
        demo::vector<std::string> vec;
        vec.emplace_back(5, 'x');

        CHECK(vec.size() == 1);
        CHECK(vec[0] == "xxxxx");
    }

    TEST_CASE("pop_back() removes last element")
    {
        demo::vector<int> vec{1, 2, 3};
        vec.pop_back();

        CHECK(vec.size() == 2);
        std::vector<int> expected{1, 2};
        CHECK(vector_equals_std(vec, expected));
    }

    TEST_CASE("pop_back() on empty vector throws")
    {
        demo::vector<int> vec;
        CHECK_THROWS_AS(vec.pop_back(), std::out_of_range);
    }

    TEST_CASE("insert() single element")
    {
        demo::vector<int> vec{1, 3, 4};
        auto it = vec.insert(vec.begin() + 1, 2);

        CHECK(*it == 2);
        std::vector<int> expected{1, 2, 3, 4};
        CHECK(vector_equals_std(vec, expected));
    }

    TEST_CASE("insert() at beginning")
    {
        demo::vector<int> vec{2, 3};
        vec.insert(vec.begin(), 1);

        std::vector<int> expected{1, 2, 3};
        CHECK(vector_equals_std(vec, expected));
    }

    TEST_CASE("insert() at end")
    {
        demo::vector<int> vec{1, 2};
        vec.insert(vec.end(), 3);

        std::vector<int> expected{1, 2, 3};
        CHECK(vector_equals_std(vec, expected));
    }

    TEST_CASE("insert() multiple copies")
    {
        demo::vector<int> vec{1, 5};
        auto it = vec.insert(vec.begin() + 1, 3, 2);

        CHECK(*it == 2);
        CHECK(vec.size() == 5);
        std::vector<int> expected{1, 2, 2, 2, 5};
        CHECK(vector_equals_std(vec, expected));
    }

    TEST_CASE("insert() iterator range")
    {
        demo::vector<int> vec{1, 6};
        std::vector<int> to_insert{2, 3, 4, 5};

        auto it = vec.insert(vec.begin() + 1, to_insert.begin(), to_insert.end());
        CHECK(*it == 2);
        CHECK(vec.size() == 6);
        std::vector<int> expected{1, 2, 3, 4, 5, 6};
        CHECK(vector_equals_std(vec, expected));
    }

    TEST_CASE("insert() initializer list")
    {
        demo::vector<int> vec{1, 6};
        auto it = vec.insert(vec.begin() + 1, {2, 3, 4, 5});

        CHECK(*it == 2);
        std::vector<int> expected{1, 2, 3, 4, 5, 6};
        CHECK(vector_equals_std(vec, expected));
    }

    TEST_CASE("emplace() constructs in-place")
    {
        demo::vector<std::string> vec{"hello", "world"};
        vec.emplace(vec.begin() + 1, 5, 'x');

        CHECK(vec.size() == 3);
        std::vector<std::string> expected{"hello", "xxxxx", "world"};
        CHECK(vector_equals_std(vec, expected));
    }

    TEST_CASE("insert() with invalid position throws")
    {
        demo::vector<int> vec{1, 2, 3};
        CHECK_THROWS_AS(vec.insert(vec.cbegin() + 5, 4), std::out_of_range);
    }
}

// ============================================
// 测试套件: 修改器 - 删除
// ============================================

TEST_SUITE("Vector Modifiers - Erase")
{
    TEST_CASE("erase() single element")
    {
        demo::vector<int> vec{1, 2, 3, 4};
        auto it = vec.erase(vec.begin() + 1);

        CHECK(*it == 3);
        std::vector<int> expected{1, 3, 4};
        CHECK(vector_equals_std(vec, expected));
    }

    TEST_CASE("erase() first element")
    {
        demo::vector<int> vec{1, 2, 3, 4};
        auto it = vec.erase(vec.begin());

        CHECK(*it == 2);
        std::vector<int> expected{2, 3, 4};
        CHECK(vector_equals_std(vec, expected));
    }

    TEST_CASE("erase() last element")
    {
        demo::vector<int> vec{1, 2, 3, 4};
        auto it = vec.erase(vec.end() - 1);

        CHECK(it == vec.end());
        std::vector<int> expected{1, 2, 3};
        CHECK(vector_equals_std(vec, expected));
    }

    TEST_CASE("erase() range")
    {
        demo::vector<int> vec{1, 2, 3, 4, 5};
        auto first = vec.begin() + 1;
        auto last = vec.begin() + 4;

        auto it = vec.erase(first, last);

        CHECK(*it == 5);
        std::vector<int> expected{1, 5};
        CHECK(vector_equals_std(vec, expected));
    }

    TEST_CASE("erase() empty range")
    {
        demo::vector<int> vec{1, 2, 3};
        auto it = vec.erase(vec.begin(), vec.begin());

        CHECK(it == vec.begin());
        CHECK(vec.size() == 3);
    }

    TEST_CASE("erase() with invalid position throws")
    {
        demo::vector<int> vec{1, 2, 3};
        CHECK_THROWS_AS(vec.erase(vec.cbegin() + 5), std::out_of_range);
    }

    TEST_CASE("erase() with invalid range throws")
    {
        demo::vector<int> vec{1, 2, 3};
        CHECK_THROWS_AS(vec.erase(vec.cbegin() + 2, vec.cbegin()), std::out_of_range);
    }

    TEST_CASE("clear() removes all elements")
    {
        demo::vector<int> vec{1, 2, 3, 4, 5};
        vec.clear();

        CHECK(vec.empty());
        CHECK(vec.size() == 0);
    }

    TEST_CASE("clear() on empty vector")
    {
        demo::vector<int> vec;
        vec.clear();
        CHECK(vec.empty());
    }
}

// ============================================
// 测试套件: 修改器 - 其他操作
// ============================================

TEST_SUITE("Vector Modifiers - Other")
{
    TEST_CASE("assign() with count and value")
    {
        demo::vector<int> vec{1, 2, 3};
        vec.assign(4, 5);

        std::vector<int> expected{5, 5, 5, 5};
        CHECK(vector_equals_std(vec, expected));
    }

    TEST_CASE("assign() with iterator range")
    {
        demo::vector<int> vec{1, 2, 3};
        std::vector<int> src{4, 5, 6, 7};
        vec.assign(src.begin(), src.end());

        CHECK(vector_equals_std(vec, src));
    }

    TEST_CASE("assign() with initializer list")
    {
        demo::vector<int> vec{1, 2, 3};
        vec.assign({8, 9, 10});

        std::vector<int> expected{8, 9, 10};
        CHECK(vector_equals_std(vec, expected));
    }

    TEST_CASE("swap() exchanges contents")
    {
        demo::vector<int> vec1{1, 2, 3};
        demo::vector<int> vec2{4, 5, 6, 7};

        vec1.swap(vec2);

        std::vector<int> expected1{4, 5, 6, 7};
        std::vector<int> expected2{1, 2, 3};
        CHECK(vector_equals_std(vec1, expected1));
        CHECK(vector_equals_std(vec2, expected2));
    }

    TEST_CASE("get_allocator() returns allocator")
    {
        demo::vector<int> vec;
        auto alloc = vec.get_allocator();
        (void)alloc;
    }
}

// ============================================
// 测试套件: 全局比较运算符
// ============================================

TEST_SUITE("Vector Comparison Operators")
{
    TEST_CASE("operator== compares contents")
    {
        demo::vector<int> vec1{1, 2, 3};
        demo::vector<int> vec2{1, 2, 3};
        demo::vector<int> vec3{1, 2, 3, 4};

        CHECK(vec1 == vec2);
        CHECK_FALSE(vec1 == vec3);
    }

    TEST_CASE("operator!= compares contents")
    {
        demo::vector<int> vec1{1, 2, 3};
        demo::vector<int> vec2{1, 2, 3};
        demo::vector<int> vec3{1, 2, 4};

        CHECK_FALSE(vec1 != vec2);
        CHECK(vec1 != vec3);
    }
}

// ============================================
// 测试套件: 边界情况和异常场景
// ============================================

TEST_SUITE("Vector Boundary Cases")
{
    TEST_CASE("single element vector operations")
    {
        demo::vector<int> vec{42};

        CHECK(vec.size() == 1);
        CHECK(vec.front() == 42);
        CHECK(vec.back() == 42);
        CHECK(vec[0] == 42);

        vec.pop_back();
        CHECK(vec.empty());
    }

    TEST_CASE("insert duplicate elements")
    {
        demo::vector<int> vec{1, 2, 3};
        vec.push_back(2);
        vec.insert(vec.begin() + 1, 2);

        CHECK(vec.size() == 5);
        std::vector<int> expected{1, 2, 2, 3, 2};
        CHECK(vector_equals_std(vec, expected));
    }

    TEST_CASE("move semantics with temporary")
    {
        demo::vector<int> vec = std::move(demo::vector<int>{1, 2, 3});

        std::vector<int> expected{1, 2, 3};
        CHECK(vector_equals_std(vec, expected));
    }

    TEST_CASE("reserve and push_back causes reallocation")
    {
        demo::vector<int> vec;
        vec.reserve(3);
        CHECK(vec.capacity() >= 3);

        vec.push_back(1);
        vec.push_back(2);
        vec.push_back(3);
        CHECK(vec.capacity() >= 3);
        CHECK(vec.size() == 3);

        vec.push_back(4);
        CHECK(vec.capacity() > 3);
    }

    TEST_CASE("self-assignment")
    {
        demo::vector<int> vec{1, 2, 3};
        vec = vec;

        CHECK(vec.size() == 3);
        std::vector<int> expected{1, 2, 3};
        CHECK(vector_equals_std(vec, expected));
    }

    TEST_CASE("iterator invalidation after reallocation")
    {
        demo::vector<int> vec{1, 2, 3};
        auto it = vec.begin();
        int *ptr = vec.data();

        vec.reserve(100);

        CHECK(it != vec.begin());
        CHECK(vec.data() != ptr);
    }
}

// ============================================
// 测试套件: 字符串类型测试
// ============================================

TEST_SUITE("Vector String Type")
{
    TEST_CASE("string vector operations")
    {
        demo::vector<std::string> vec{"hello", "world", "test"};
        CHECK(vec.size() == 3);

        vec.push_back("end");
        vec.insert(vec.begin() + 1, "middle");

        std::vector<std::string> expected{"hello", "middle", "world", "test", "end"};
        CHECK(vector_equals_std(vec, expected));

        vec.erase(vec.begin() + 2);
        std::vector<std::string> expected_after_erase{"hello", "middle", "test", "end"};
        CHECK(vector_equals_std(vec, expected_after_erase));
    }
}

// ============================================
// 测试套件: 扩容测试
// ============================================

TEST_SUITE("Vector Capacity Growth")
{
    TEST_CASE("capacity grows exponentially")
    {
        demo::vector<int> vec;
        size_t prev_capacity = 0;

        for (size_t i = 0; i < 20; ++i)
        {
            vec.push_back(i);
            if (vec.capacity() > prev_capacity)
            {
                CHECK(vec.capacity() >= prev_capacity * 2);
                prev_capacity = vec.capacity();
            }
        }
    }

    TEST_CASE("reserve prevents reallocation")
    {
        demo::vector<int> vec;
        vec.reserve(10);
        size_t capacity = vec.capacity();

        for (size_t i = 0; i < 10; ++i)
        {
            vec.push_back(i);
        }

        CHECK(vec.capacity() == capacity);
    }
}