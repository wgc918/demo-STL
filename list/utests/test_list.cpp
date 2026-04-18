/**
 * @file test_list.cpp
 * @brief 基于doctest的工业级双向链表(list)测试套件
 *
 * 测试覆盖范围:
 * - 构造函数(默认、大小、迭代器范围、初始化列表、拷贝、移动)
 * - 元素访问(front, back)
 * - 迭代器(正向、反向、const迭代器)
 * - 容量操作(empty, size, max_size, resize)
 * - 修改器(clear, insert, emplace, erase, push/pop_front/back, swap, assign)
 * - 操作(merge, splice, remove, remove_if, reverse, unique, sort)
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
#include <list>
#include "../list.h"

// ============================================
// 测试辅助工具
// ============================================

/**
 * @brief 将demo::list转换为std::vector便于比较
 */
template <typename T>
std::vector<T> list_to_vector(const demo::list<T> &lst)
{
    std::vector<T> result;
    for (const auto &elem : lst)
    {
        result.push_back(elem);
    }
    return result;
}

/**
 * @brief 比较demo::list与std::vector内容是否一致
 */
template <typename T>
bool list_equals_vector(const demo::list<T> &lst, const std::vector<T> &vec)
{
    if (lst.size() != vec.size())
        return false;

    auto it = lst.begin();
    for (const auto &elem : vec)
    {
        if (it == lst.end() || *it != elem)
            return false;
        ++it;
    }
    return true;
}

/**
 * @brief 比较两个list内容是否一致
 */
template <typename T>
bool list_equals_list(const demo::list<T> &a, const demo::list<T> &b)
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

/**
 * @brief 生成指定大小的vector用于测试
 */
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

TEST_SUITE("List Constructors")
{

    TEST_CASE("Default constructor creates empty list")
    {
        demo::list<int> lst;
        CHECK(lst.empty());
        CHECK(lst.size() == 0);
    }

    TEST_CASE("Size constructor with default value")
    {
        demo::list<int> lst(5);
        CHECK(lst.size() == 5);
        CHECK_FALSE(lst.empty());

        for (const auto &elem : lst)
        {
            CHECK(elem == int()); // 默认构造值
        }
    }

    TEST_CASE("Size constructor with explicit value")
    {
        demo::list<std::string> lst(3, "hello");
        CHECK(lst.size() == 3);

        std::vector<std::string> expected{"hello", "hello", "hello"};
        CHECK(list_equals_vector(lst, expected));
    }

    TEST_CASE("Initializer list constructor")
    {
        demo::list<int> lst{1, 2, 3, 4, 5};
        CHECK(lst.size() == 5);

        std::vector<int> expected{1, 2, 3, 4, 5};
        CHECK(list_equals_vector(lst, expected));
    }

    TEST_CASE("Iterator range constructor")
    {
        std::vector<int> source{10, 20, 30, 40};
        demo::list<int> lst(source.begin(), source.end());

        CHECK(lst.size() == 4);
        CHECK(list_equals_vector(lst, source));
    }

    TEST_CASE("Copy constructor")
    {
        demo::list<int> original{1, 2, 3, 4};
        demo::list<int> copy(original);

        CHECK(copy.size() == original.size());
        CHECK(list_equals_list(copy, original));

        // 验证深拷贝 - 修改原列表不影响副本
        original.push_back(5);
        CHECK(copy.size() == 4);
        CHECK(original.size() == 5);
    }

    TEST_CASE("Move constructor")
    {
        demo::list<int> original{1, 2, 3, 4, 5};
        demo::list<int> moved(std::move(original));

        CHECK(moved.size() == 5);
        CHECK(original.size() == 0); // 移动后原列表应为空

        std::vector<int> expected{1, 2, 3, 4, 5};
        CHECK(list_equals_vector(moved, expected));
    }
}

// ============================================
// 测试套件: 赋值操作符
// ============================================

TEST_SUITE("List Assignment Operators")
{

    TEST_CASE("Copy assignment operator")
    {
        demo::list<std::string> original{"a", "b", "c"};
        demo::list<std::string> copy;

        copy = original;

        CHECK(copy.size() == 3);
        CHECK(list_equals_list(copy, original));

        // 验证自赋值安全
        copy = copy;
        CHECK(copy.size() == 3);
    }

    TEST_CASE("Move assignment operator")
    {
        demo::list<int> original{10, 20, 30};
        demo::list<int> moved;

        moved = std::move(original);

        CHECK(moved.size() == 3);
        CHECK_FALSE(moved.empty());
        CHECK(original.size() == 0);
        CHECK(original.empty());

        std::vector<int> expected{10, 20, 30};
        CHECK(list_equals_vector(moved, expected));
    }

    TEST_CASE("Initializer list assignment")
    {
        demo::list<int> lst;
        lst = {100, 200, 300};

        CHECK(lst.size() == 3);
        std::vector<int> expected{100, 200, 300};
        CHECK(list_equals_vector(lst, expected));
    }
}

// ============================================
// 测试套件: 元素访问
// ============================================

TEST_SUITE("List Element Access")
{

    TEST_CASE("front() returns first element")
    {
        demo::list<int> lst{1, 2, 3};
        CHECK(lst.front() == 1);

        lst.front() = 100; // 测试可修改性
        CHECK(lst.front() == 100);
    }

    TEST_CASE("back() returns last element")
    {
        demo::list<int> lst{1, 2, 3};
        CHECK(lst.back() == 3);

        lst.back() = 300;
        CHECK(lst.back() == 300);
    }

    TEST_CASE("const front() and back()")
    {
        const demo::list<int> lst{10, 20, 30};
        CHECK(lst.front() == 10);
        CHECK(lst.back() == 30);
    }
}

// ============================================
// 测试套件: 迭代器
// ============================================

TEST_SUITE("List Iterators")
{

    TEST_CASE("begin() and end() iterators")
    {
        demo::list<int> lst{1, 2, 3, 4, 5};

        int sum = 0;
        for (auto it = lst.begin(); it != lst.end(); ++it)
        {
            sum += *it;
        }
        CHECK(sum == 15);
    }

    TEST_CASE("const iterators cbegin() and cend()")
    {
        demo::list<int> lst{1, 2, 3};

        int sum = 0;
        for (auto it = lst.cbegin(); it != lst.cend(); ++it)
        {
            sum += *it;
        }
        CHECK(sum == 6);
    }

    TEST_CASE("range-based for loop")
    {
        demo::list<int> lst{10, 20, 30};

        int sum = 0;
        for (const auto &elem : lst)
        {
            sum += elem;
        }
        CHECK(sum == 60);
    }

    TEST_CASE("reverse iterators rbegin() and rend()")
    {
        demo::list<int> lst{1, 2, 3, 4};

        std::vector<int> reversed;
        for (auto it = lst.rbegin(); it != lst.rend(); ++it)
        {
            reversed.push_back(*it);
        }

        std::vector<int> expected{4, 3, 2, 1};
        CHECK(reversed == expected);
    }

    TEST_CASE("const reverse iterators crbegin() and crend()")
    {
        const demo::list<int> lst{1, 2, 3, 4};

        std::vector<int> reversed;
        for (auto it = lst.crbegin(); it != lst.crend(); ++it)
        {
            reversed.push_back(*it);
        }

        std::vector<int> expected{4, 3, 2, 1};
        CHECK(reversed == expected);
    }

    TEST_CASE("iterator bidirectional operations")
    {
        demo::list<int> lst{1, 2, 3, 4, 5};

        auto it = lst.begin();
        ++it;
        ++it; // 指向 3
        CHECK(*it == 3);

        --it; // 指向 2
        CHECK(*it == 2);

        auto old = it++;
        CHECK(*old == 2);
        CHECK(*it == 3);

        auto old2 = it--;
        CHECK(*old2 == 3);
        CHECK(*it == 2);
    }

    TEST_CASE("iterator equality comparison")
    {
        demo::list<int> lst{1, 2, 3};

        auto it1 = lst.begin();
        auto it2 = lst.begin();
        CHECK(it1 == it2);

        ++it1;
        CHECK(it1 != it2);

        auto it3 = lst.end();
        CHECK(it3 != it1);
    }

    TEST_CASE("iterator conversion to const_iterator")
    {
        demo::list<int> lst{1, 2, 3};

        demo::list<int>::iterator it = lst.begin();
        demo::list<int>::const_iterator cit = it;

        CHECK(cit == it);
        CHECK(*cit == *it);
    }
}

// ============================================
// 测试套件: 容量操作
// ============================================

TEST_SUITE("List Capacity")
{

    TEST_CASE("empty() returns true for empty list")
    {
        demo::list<int> lst;
        CHECK(lst.empty());

        lst.push_back(1);
        CHECK_FALSE(lst.empty());
    }

    TEST_CASE("size() returns correct element count")
    {
        demo::list<int> lst;
        CHECK(lst.size() == 0);

        for (int i = 0; i < 100; ++i)
        {
            lst.push_back(i);
        }
        CHECK(lst.size() == 100);
    }

    TEST_CASE("max_size() returns positive value")
    {
        demo::list<int> lst;
        CHECK(lst.max_size() > 0);
    }

    TEST_CASE("resize() to larger size")
    {
        demo::list<int> lst{1, 2, 3};
        lst.resize(5);

        CHECK(lst.size() == 5);
        CHECK(lst.back() == int()); // 新增元素为默认构造
        CHECK(list_equals_vector(lst, {1, 2, 3, int(), int()}));
    }

    TEST_CASE("resize() with explicit value")
    {
        demo::list<int> lst{1, 2};
        lst.resize(5, 99);

        std::vector<int> expected{1, 2, 99, 99, 99};
        CHECK(list_equals_vector(lst, expected));
    }

    TEST_CASE("resize() to smaller size")
    {
        demo::list<int> lst{1, 2, 3, 4, 5};
        lst.resize(2);

        CHECK(lst.size() == 2);
        std::vector<int> expected{1, 2};
        CHECK(list_equals_vector(lst, expected));
    }

    TEST_CASE("resize() to same size")
    {
        demo::list<int> lst{1, 2, 3};
        lst.resize(3);
        CHECK(lst.size() == 3);
        std::vector<int> expected{1, 2, 3};
        CHECK(list_equals_vector(lst, expected));
    }

    TEST_CASE("resize() to zero")
    {
        demo::list<int> lst{1, 2, 3};
        lst.resize(0);
        CHECK(lst.empty());
        CHECK(lst.size() == 0);
    }
}

// ============================================
// 测试套件: 修改器 - 插入操作
// ============================================

TEST_SUITE("List Modifiers - Insert")
{

    TEST_CASE("push_front() adds element to front")
    {
        demo::list<int> lst;
        lst.push_front(3);
        lst.push_front(2);
        lst.push_front(1);

        std::vector<int> expected{1, 2, 3};
        CHECK(list_equals_vector(lst, expected));
    }

    TEST_CASE("push_back() adds element to back")
    {
        demo::list<int> lst;
        lst.push_back(1);
        lst.push_back(2);
        lst.push_back(3);

        std::vector<int> expected{1, 2, 3};
        CHECK(list_equals_vector(lst, expected));
    }

    TEST_CASE("pop_front() removes first element")
    {
        demo::list<int> lst{1, 2, 3};
        lst.pop_front();

        CHECK(lst.front() == 2);
        CHECK(lst.size() == 2);
    }

    TEST_CASE("pop_back() removes last element")
    {
        demo::list<int> lst{1, 2, 3};
        lst.pop_back();

        CHECK(lst.back() == 2);
        CHECK(lst.size() == 2);
    }

    TEST_CASE("pop_front() on empty list throws exception")
    {
        demo::list<int> lst;
        CHECK_THROWS_AS(lst.pop_front(), std::out_of_range);
    }

    TEST_CASE("pop_back() on empty list throws exception")
    {
        demo::list<int> lst;
        CHECK_THROWS_AS(lst.pop_back(), std::out_of_range);
    }

    TEST_CASE("insert() single element at begin")
    {
        demo::list<int> lst{2, 3};
        auto result = lst.insert(lst.begin(), 1);

        CHECK(*result == 1);
        std::vector<int> expected{1, 2, 3};
        CHECK(list_equals_vector(lst, expected));
    }

    TEST_CASE("insert() single element in middle")
    {
        demo::list<int> lst{1, 3, 5};
        auto it = lst.begin();
        ++it; // 指向 3

        auto result = lst.insert(it, 2);
        CHECK(*result == 2);
        CHECK(lst.size() == 4);
        std::vector<int> expected{1, 2, 3, 5};
        CHECK(list_equals_vector(lst, expected));
    }

    TEST_CASE("insert() single element at end")
    {
        demo::list<int> lst{1, 2, 3};
        auto result = lst.insert(lst.end(), 4);

        CHECK(*result == 4);
        CHECK(lst.size() == 4);
        std::vector<int> expected{1, 2, 3, 4};
        CHECK(list_equals_vector(lst, expected));
    }

    TEST_CASE("insert() multiple copies")
    {
        demo::list<int> lst{1, 5};
        auto it = ++lst.begin(); // 指向 5

        lst.insert(it, 3, 3); // 插入3个3
        CHECK(lst.size() == 5);
        std::vector<int> expected{1, 3, 3, 3, 5};
        CHECK(list_equals_vector(lst, expected));
    }

    TEST_CASE("insert() iterator range")
    {
        demo::list<int> lst{1, 6};
        std::vector<int> to_insert{2, 3, 4, 5};

        auto it = ++lst.begin();
        lst.insert(it, to_insert.begin(), to_insert.end());
        CHECK(lst.size() == 6);
        std::vector<int> expected{1, 2, 3, 4, 5, 6};
        CHECK(list_equals_vector(lst, expected));
    }

    TEST_CASE("insert() initializer list")
    {
        demo::list<int> lst{1, 5};
        auto it = ++lst.begin();

        lst.insert(it, {2, 3, 4});

        std::vector<int> expected{1, 2, 3, 4, 5};
        CHECK(list_equals_vector(lst, expected));
    }

    TEST_CASE("emplace() constructs in-place")
    {
        demo::list<std::string> lst{"hello", "world"};
        auto it = ++lst.begin();

        auto result = lst.emplace(it, 5, 'x'); // 构造 "xxxxx"

        CHECK(*result == "xxxxx");
        CHECK(lst.size() == 3);
    }

    TEST_CASE("emplace_front() constructs at front")
    {
        demo::list<std::string> lst;
        lst.emplace_front(3, 'a'); // "aaa"

        CHECK(lst.front() == "aaa");
    }

    TEST_CASE("empalce_back() constructs at back")
    {
        demo::list<std::string> lst;
        lst.empalce_back(3, 'b'); // "bbb"

        CHECK(lst.back() == "bbb");
    }
}

// ============================================
// 测试套件: 修改器 - 删除操作
// ============================================

TEST_SUITE("List Modifiers - Erase")
{

    TEST_CASE("erase() single element in middle")
    {
        demo::list<int> lst{1, 2, 3, 4, 5};
        auto it = lst.begin();
        ++it;
        ++it; // 指向 3

        auto next = lst.erase(it);
        CHECK(*next == 4);

        std::vector<int> expected{1, 2, 4, 5};
        CHECK(list_equals_vector(lst, expected));
    }

    TEST_CASE("erase() single element at begin")
    {
        demo::list<int> lst{1, 2, 3};
        auto it = lst.begin();
        auto next = lst.erase(it);

        CHECK(*next == 2);
        std::vector<int> expected{2, 3};
        CHECK(list_equals_vector(lst, expected));
    }

    TEST_CASE("erase() single element at end")
    {
        demo::list<int> lst{1, 2, 3};
        auto it = lst.end();
        --it; // 指向 3
        auto next = lst.erase(it);

        CHECK(next == lst.end());
        std::vector<int> expected{1, 2};
        CHECK(list_equals_vector(lst, expected));
    }

    TEST_CASE("erase() range")
    {
        demo::list<int> lst{1, 2, 3, 4, 5, 6, 7};
        auto first = ++lst.begin(); // 指向 2
        auto last = lst.end();
        --last;
        --last; // 指向 6 (erase到last之前)

        lst.erase(first, last);

        std::vector<int> expected{1, 6, 7};
        CHECK(list_equals_vector(lst, expected));
    }

    TEST_CASE("erase() empty range")
    {
        demo::list<int> lst{1, 2, 3};
        auto it = lst.begin();
        auto result = lst.erase(it, it); // 空范围

        CHECK(result == it);
        std::vector<int> expected{1, 2, 3};
        CHECK(list_equals_vector(lst, expected));
    }

    TEST_CASE("clear() removes all elements")
    {
        demo::list<int> lst{1, 2, 3, 4, 5};
        lst.clear();

        CHECK(lst.empty());
        CHECK(lst.size() == 0);
    }

    TEST_CASE("clear() on empty list")
    {
        demo::list<int> lst;
        lst.clear(); // 不应抛出异常

        CHECK(lst.empty());
    }
}

// ============================================
// 测试套件: 修改器 - 其他操作
// ============================================

TEST_SUITE("List Modifiers - Other")
{

    TEST_CASE("swap() exchanges contents")
    {
        demo::list<int> lst1{1, 2, 3};
        demo::list<int> lst2{4, 5};

        lst1.swap(lst2);

        std::vector<int> expected1{4, 5};
        std::vector<int> expected2{1, 2, 3};
        CHECK(list_equals_vector(lst1, expected1));
        CHECK(list_equals_vector(lst2, expected2));
    }

    TEST_CASE("swap() with self")
    {
        demo::list<int> lst{1, 2, 3};
        lst.swap(lst); // 自交换应无效果

        std::vector<int> expected{1, 2, 3};
        CHECK(list_equals_vector(lst, expected));
    }

    TEST_CASE("assign() with count and value")
    {
        demo::list<int> lst{1, 2, 3, 4, 5};
        lst.assign(3, 99);

        CHECK(lst.size() == 3);
        std::vector<int> expected{99, 99, 99};
        CHECK(list_equals_vector(lst, expected));
    }

    TEST_CASE("assign() with iterator range")
    {
        demo::list<int> lst{1, 2, 3};
        std::vector<int> source{10, 20, 30, 40};

        lst.assign(source.begin(), source.end());

        CHECK(lst.size() == 4);
        CHECK(list_equals_vector(lst, source));
    }

    TEST_CASE("assign() with initializer list")
    {
        demo::list<int> lst{1, 2, 3};
        lst.assign({100, 200});

        CHECK(lst.size() == 2);
        std::vector<int> expected{100, 200};
        CHECK(list_equals_vector(lst, expected));
    }

    TEST_CASE("assign() to empty list")
    {
        demo::list<int> lst;
        lst.assign(5, 42);

        CHECK(lst.size() == 5);
        for (const auto &elem : lst)
        {
            CHECK(elem == 42);
        }
    }
}

// ============================================
// 测试套件: 列表操作
// ============================================

TEST_SUITE("List Operations")
{

    TEST_CASE("merge() two sorted lists")
    {
        demo::list<int> lst1{1, 3, 5, 7};
        demo::list<int> lst2{2, 4, 6, 8};

        lst1.merge(lst2);

        CHECK(lst2.empty()); // 合并后lst2应为空
        std::vector<int> expected{1, 2, 3, 4, 5, 6, 7, 8};
        CHECK(list_equals_vector(lst1, expected));
    }

    TEST_CASE("merge() with custom comparator")
    {
        demo::list<int> lst1{7, 5, 3, 1};
        demo::list<int> lst2{8, 6, 4, 2};

        lst1.merge(lst2, std::greater<int>());

        std::vector<int> expected{8, 7, 6, 5, 4, 3, 2, 1};
        CHECK(list_equals_vector(lst1, expected));
    }

    TEST_CASE("merge() into empty list")
    {
        demo::list<int> lst1;
        demo::list<int> lst2{1, 2, 3};

        lst1.merge(lst2);

        CHECK(lst2.empty());
        std::vector<int> expected{1, 2, 3};
        CHECK(list_equals_vector(lst1, expected));
    }

    TEST_CASE("merge() with empty source")
    {
        demo::list<int> lst1{1, 2, 3};
        demo::list<int> lst2;

        CHECK(lst2.empty());
        lst1.merge(lst2);

        std::vector<int> expected{1, 2, 3};
        CHECK(list_equals_vector(lst1, expected));
    }

    TEST_CASE("merge() self merge")
    {
        demo::list<int> lst{1, 2, 3};
        lst.merge(lst); // 不应崩溃

        std::vector<int> expected{1, 2, 3};
        CHECK(list_equals_vector(lst, expected));
    }

    TEST_CASE("splice() entire list")
    {
        demo::list<int> lst1{1, 4, 5};
        demo::list<int> lst2{2, 3};

        auto it = ++lst1.begin(); // 指向 4
        lst1.splice(it, lst2);

        CHECK(lst2.empty());
        std::vector<int> expected{1, 2, 3, 4, 5};
        CHECK(list_equals_vector(lst1, expected));
    }

    TEST_CASE("splice() single element")
    {
        demo::list<int> lst1{1, 3, 4};
        demo::list<int> lst2{2};

        auto it = ++lst1.begin(); // 指向 3
        auto it2 = lst2.begin();

        lst1.splice(it, lst2, it2);

        std::vector<int> expected{1, 2, 3, 4};
        CHECK(list_equals_vector(lst1, expected));
    }

    TEST_CASE("splice() range")
    {
        demo::list<int> lst1{1, 6};
        demo::list<int> lst2{2, 3, 4, 5};

        auto pos = ++lst1.begin();
        auto first = lst2.begin();
        auto last = lst2.end();
        --last; // 指向 5

        lst1.splice(pos, lst2, first, last);
        CHECK(lst1.size() == 5);

        // 手动遍历打印（最安全）
        MESSAGE("lst1 :");
        for (int v : lst1)
        {
            MESSAGE("  - ", v);
        }
        std::vector<int> expected{1, 2, 3, 4, 6};
        CHECK(list_equals_vector(lst1, expected));
    }

    TEST_CASE("splice() self splice entire list")
    {
        demo::list<int> lst{1, 2, 3};
        lst.splice(lst.begin(), lst); // 不应崩溃

        std::vector<int> expected{1, 2, 3};
        CHECK(list_equals_vector(lst, expected));
    }

    TEST_CASE("remove() specific value")
    {
        demo::list<int> lst{1, 2, 3, 2, 4, 2, 5};

        auto count = lst.remove(2);

        CHECK(count == 3);
        std::vector<int> expected{1, 3, 4, 5};
        CHECK(list_equals_vector(lst, expected));
    }

    TEST_CASE("remove() non-existent value")
    {
        demo::list<int> lst{1, 2, 3, 4, 5};

        auto count = lst.remove(10);

        CHECK(count == 0);
        std::vector<int> expected{1, 2, 3, 4, 5};
        CHECK(list_equals_vector(lst, expected));
    }

    TEST_CASE("remove() from empty list")
    {
        demo::list<int> lst;

        auto count = lst.remove(5);

        CHECK(count == 0);
        CHECK(lst.empty());
    }

    TEST_CASE("remove_if() with predicate")
    {
        demo::list<int> lst{1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

        auto count = lst.remove_if([](int n)
                                   { return n % 2 == 0; });

        CHECK(count == 5);
        std::vector<int> expected{1, 3, 5, 7, 9};
        CHECK(list_equals_vector(lst, expected));
    }

    TEST_CASE("remove_if() with always false predicate")
    {
        demo::list<int> lst{1, 2, 3, 4, 5};

        auto count = lst.remove_if([](int n)
                                   { (void)n;return false; });

        CHECK(count == 0);
        std::vector<int> expected{1, 2, 3, 4, 5};
        CHECK(list_equals_vector(lst, expected));
    }

    TEST_CASE("reverse() reverses list order")
    {
        demo::list<int> lst{1, 2, 3, 4, 5};
        lst.reverse();

        std::vector<int> expected{5, 4, 3, 2, 1};
        CHECK(list_equals_vector(lst, expected));
    }

    TEST_CASE("reverse() empty and single element list")
    {
        demo::list<int> empty_lst;
        empty_lst.reverse(); // 不应崩溃
        CHECK(empty_lst.empty());

        demo::list<int> single_lst{42};
        single_lst.reverse();
        CHECK(single_lst.front() == 42);
    }

    TEST_CASE("unique() removes consecutive duplicates")
    {
        demo::list<int> lst{1, 1, 2, 2, 2, 3, 3, 1};

        auto count = lst.unique();

        CHECK(count == 4);
        std::vector<int> expected{1, 2, 3, 1};
        CHECK(list_equals_vector(lst, expected));
    }

    TEST_CASE("unique() with custom predicate")
    {
        demo::list<std::string> lst{"A", "a", "B", "b", "C"};

        auto count = lst.unique([](const auto &a, const auto &b)
                                { return std::tolower(a[0]) == std::tolower(b[0]); });

        CHECK(count == 2);
        std::vector<std::string> expected{"A", "B", "C"};
        CHECK(list_equals_vector(lst, expected));
    }

    TEST_CASE("unique() on empty list")
    {
        demo::list<int> lst;
        auto count = lst.unique();
        CHECK(count == 0);
        CHECK(lst.empty());
    }

    TEST_CASE("unique() on single element list")
    {
        demo::list<int> lst{42};
        auto count = lst.unique();
        CHECK(count == 0);
        CHECK(lst.front() == 42);
    }

    TEST_CASE("sort() ascending order")
    {
        demo::list<int> lst{5, 2, 8, 1, 9, 3};
        lst.sort();

        std::vector<int> expected{1, 2, 3, 5, 8, 9};
        CHECK(list_equals_vector(lst, expected));
    }

    TEST_CASE("sort() with custom comparator")
    {
        demo::list<int> lst{5, 2, 8, 1, 9, 3};
        lst.sort(std::greater<int>());

        std::vector<int> expected{9, 8, 5, 3, 2, 1};
        CHECK(list_equals_vector(lst, expected));
    }

    TEST_CASE("sort() empty and single element list")
    {
        demo::list<int> empty_lst;
        empty_lst.sort();
        CHECK(empty_lst.empty());

        demo::list<int> single_lst{42};
        single_lst.sort();
        CHECK(single_lst.front() == 42);
    }

    TEST_CASE("sort() already sorted list")
    {
        demo::list<int> lst{1, 2, 3, 4, 5};
        lst.sort();

        std::vector<int> expected{1, 2, 3, 4, 5};
        CHECK(list_equals_vector(lst, expected));
    }

    TEST_CASE("sort() reverse sorted list")
    {
        demo::list<int> lst{5, 4, 3, 2, 1};
        lst.sort();

        std::vector<int> expected{1, 2, 3, 4, 5};
        CHECK(list_equals_vector(lst, expected));
    }

    TEST_CASE("sort() list with duplicates")
    {
        demo::list<int> lst{3, 1, 4, 1, 5, 9, 2, 6, 5, 3};
        lst.sort();

        std::vector<int> expected{1, 1, 2, 3, 3, 4, 5, 5, 6, 9};
        CHECK(list_equals_vector(lst, expected));
    }
}

// ============================================
// 测试套件: 比较运算符
// ============================================

TEST_SUITE("List Comparison Operators")
{

    TEST_CASE("operator== for equal lists")
    {
        demo::list<int> lst1{1, 2, 3};
        demo::list<int> lst2{1, 2, 3};

        CHECK(lst1 == lst2);
    }

    TEST_CASE("operator== for different size lists")
    {
        demo::list<int> lst1{1, 2, 3};
        demo::list<int> lst2{1, 2};

        CHECK_FALSE(lst1 == lst2);
    }

    TEST_CASE("operator== for different content lists")
    {
        demo::list<int> lst1{1, 2, 3};
        demo::list<int> lst2{1, 2, 4};

        CHECK_FALSE(lst1 == lst2);
    }

    TEST_CASE("operator!= for different lists")
    {
        demo::list<int> lst1{1, 2, 3};
        demo::list<int> lst2{1, 2, 4};

        CHECK(lst1 != lst2);
    }

    TEST_CASE("operator!= for equal lists")
    {
        demo::list<int> lst1{1, 2, 3};
        demo::list<int> lst2{1, 2, 3};

        CHECK_FALSE(lst1 != lst2);
    }
}

// ============================================
// 测试套件: 边界条件和异常
// ============================================

TEST_SUITE("List Edge Cases and Exceptions")
{

    TEST_CASE("Operations on empty list")
    {
        demo::list<int> lst;

        CHECK(lst.empty());
        CHECK(lst.size() == 0);

        // 迭代器应该相等
        CHECK(lst.begin() == lst.end());

        // 容量操作
        CHECK(lst.max_size() > 0);

        // 尝试访问元素应抛出异常
        CHECK_THROWS_AS(lst.front(), std::out_of_range);
        CHECK_THROWS_AS(lst.back(), std::out_of_range);
    }

    TEST_CASE("Single element list operations")
    {
        demo::list<int> lst{42};

        CHECK(lst.front() == 42);
        CHECK(lst.back() == 42);
        CHECK(lst.size() == 1);

        lst.pop_front();
        CHECK(lst.empty());
    }

    TEST_CASE("Large list operations")
    {
        demo::list<int> lst;
        const int N = 10000;

        for (int i = 0; i < N; ++i)
        {
            lst.push_back(i);
        }
        CHECK(lst.size() == N);

        lst.clear();
        CHECK(lst.empty());
    }

    TEST_CASE("Iterator stability after modifications")
    {
        demo::list<int> lst{1, 2, 3, 4, 5};
        auto it = ++lst.begin(); // 指向 2

        lst.push_back(6); // 不应使迭代器失效
        CHECK(*it == 2);

        lst.push_front(0);
        CHECK(*it == 2); // 仍然指向 2

        lst.insert(++lst.begin(), 10); // 在 0 和 1 之间插入
        CHECK(*it == 2);               // 仍然指向 2
    }

    TEST_CASE("Complex type operations")
    {
        demo::list<std::string> lst;

        lst.push_back("hello");
        lst.push_front("world");
        lst.emplace(++lst.begin(), "middle");

        CHECK(lst.size() == 3);
        CHECK(lst.front() == "world");
        CHECK(lst.back() == "hello");
    }

    TEST_CASE("Self assignment safety")
    {
        demo::list<int> lst{1, 2, 3};
        lst = lst;

        std::vector<int> expected{1, 2, 3};
        CHECK(list_equals_vector(lst, expected));
    }

    TEST_CASE("Multiple modifications sequence")
    {
        demo::list<int> lst;

        // 混合操作序列
        lst.push_back(1);
        lst.push_front(0);
        lst.insert(lst.end(), 2);
        lst.pop_front();
        lst.push_back(3);
        lst.erase(lst.begin());

        std::vector<int> expected{2, 3};
        CHECK(list_equals_vector(lst, expected));
    }

    TEST_CASE("emplace with multiple arguments")
    {
        demo::list<std::string> lst;

        // 测试emplace_front
        lst.emplace_front(5, 'x');
        CHECK(lst.front() == "xxxxx");

        // 测试emplace
        lst.emplace(lst.end(), 3, 'y');
        CHECK(lst.back() == "yyy");

        // 测试empalce_back
        lst.empalce_back(4, 'z');
        CHECK(lst.back() == "zzzz");
    }

    TEST_CASE("get_allocator")
    {
        demo::list<int> lst;
        std::allocator<int> alloc = lst.get_allocator();
        // 验证分配器可用
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

TEST_SUITE("List Performance Edge Cases")
{

    TEST_CASE("Stress test: many small insertions")
    {
        demo::list<int> lst;
        const int N = 10000;

        for (int i = 0; i < N; ++i)
        {
            lst.push_back(i);
        }

        CHECK(lst.size() == N);
        CHECK(lst.back() == N - 1);
    }

    TEST_CASE("Stress test: many small deletions")
    {
        demo::list<int> lst;
        const int N = 10000;

        for (int i = 0; i < N; ++i)
        {
            lst.push_back(i);
        }

        while (!lst.empty())
        {
            lst.pop_front();
        }

        CHECK(lst.empty());
    }

    TEST_CASE("Stress test: alternating insert and erase")
    {
        demo::list<int> lst;
        const int N = 1000;

        for (int i = 0; i < N; ++i)
        {
            lst.push_back(i);
            if (i > 0)
            {
                lst.pop_front();
            }
        }

        CHECK(lst.size() == 1);
        CHECK(lst.front() == N - 1);
    }
}
