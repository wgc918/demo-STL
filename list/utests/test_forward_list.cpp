/**
 * @file test_forward_list.cpp
 * @brief 基于doctest的工业级单向链表(forward_list)测试套件
 *
 * 测试覆盖范围:
 * - 构造函数(默认、大小、迭代器范围、初始化列表、拷贝、移动)
 * - 元素访问(front)
 * - 迭代器(正向、const迭代器、before_begin)
 * - 容量操作(empty, max_size, resize)
 * - 修改器(clear, insert_after, emplace_after, erase_after,
 *           push/pop_front, swap, assign)
 * - 操作(merge, splice_after, remove, remove_if, reverse, unique, sort)
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
#include "../forward_list.h"

// ============================================
// 测试辅助工具
// ============================================

/**
 * @brief 将demo::forward_list转换为std::vector便于比较
 */
template <typename T>
std::vector<T> list_to_vector(const demo::forward_list<T> &lst)
{
    std::vector<T> result;
    for (const auto &elem : lst)
    {
        result.push_back(elem);
    }
    return result;
}

/**
 * @brief 比较demo::forward_list与std::vector内容是否一致
 */
template <typename T>
bool list_equals_vector(const demo::forward_list<T> &lst, const std::vector<T> &vec)
{
    auto vec_it = vec.begin();
    for (const auto &elem : lst)
    {
        if (vec_it == vec.end() || elem != *vec_it)
            return false;
        ++vec_it;
    }
    return vec_it == vec.end();
}

/**
 * @brief 比较两个forward_list内容是否一致
 */
template <typename T>
bool list_equals_list(const demo::forward_list<T> &a, const demo::forward_list<T> &b)
{
    auto it_a = a.begin();
    auto it_b = b.begin();
    for (; it_a != a.end() && it_b != b.end(); ++it_a, ++it_b)
    {
        if (*it_a != *it_b)
            return false;
    }
    return it_a == a.end() && it_b == b.end();
}

/**
 * @brief 获取forward_list的元素数量
 */
template <typename T>
size_t get_list_size(const demo::forward_list<T> &lst)
{
    size_t size = 0;
    for (const auto &elem : lst)
    {
        (void)elem;
        ++size;
    }
    return size;
}

// ============================================
// 测试套件: 构造函数
// ============================================

TEST_SUITE("ForwardList Constructors")
{
    TEST_CASE("Default constructor creates empty list")
    {
        demo::forward_list<int> lst;
        CHECK(lst.empty());
        CHECK(get_list_size(lst) == 0);
    }

    TEST_CASE("Size constructor with default value")
    {
        demo::forward_list<int> lst(5);
        CHECK_FALSE(lst.empty());
        CHECK(get_list_size(lst) == 5);

        for (const auto &elem : lst)
        {
            CHECK(elem == int());
        }
    }

    TEST_CASE("Size constructor with explicit value")
    {
        demo::forward_list<std::string> lst(3, "hello");
        CHECK(get_list_size(lst) == 3);

        std::vector<std::string> expected{"hello", "hello", "hello"};
        CHECK(list_equals_vector(lst, expected));
    }

    TEST_CASE("Initializer list constructor")
    {
        demo::forward_list<int> lst{1, 2, 3, 4, 5};
        CHECK(get_list_size(lst) == 5);

        std::vector<int> expected{1, 2, 3, 4, 5};
        CHECK(list_equals_vector(lst, expected));
    }

    TEST_CASE("Iterator range constructor")
    {
        std::vector<int> source{10, 20, 30, 40};
        demo::forward_list<int> lst(source.begin(), source.end());

        CHECK(get_list_size(lst) == 4);
        CHECK(list_equals_vector(lst, source));
    }

    TEST_CASE("Copy constructor")
    {
        demo::forward_list<int> original{1, 2, 3, 4};
        demo::forward_list<int> copy(original);

        CHECK(get_list_size(copy) == get_list_size(original));
        CHECK(list_equals_list(copy, original));

        // 验证深拷贝 - 修改原列表不影响副本
        original.push_front(5);
        CHECK(get_list_size(copy) == 4);
        CHECK(get_list_size(original) == 5);
    }

    TEST_CASE("Move constructor")
    {
        demo::forward_list<int> original{1, 2, 3, 4, 5};
        demo::forward_list<int> moved(std::move(original));

        CHECK(get_list_size(moved) == 5);
        CHECK(original.empty());

        std::vector<int> expected{1, 2, 3, 4, 5};
        CHECK(list_equals_vector(moved, expected));
    }
}

// ============================================
// 测试套件: 赋值操作符
// ============================================

TEST_SUITE("ForwardList Assignment Operators")
{
    TEST_CASE("Copy assignment operator")
    {
        demo::forward_list<std::string> original{"a", "b", "c"};
        demo::forward_list<std::string> copy;

        copy = original;

        CHECK(get_list_size(copy) == 3);
        CHECK(list_equals_list(copy, original));

        // 验证自赋值安全
        copy = copy;
        CHECK(get_list_size(copy) == 3);
    }

    TEST_CASE("Move assignment operator")
    {
        demo::forward_list<int> original{10, 20, 30};
        demo::forward_list<int> moved;

        moved = std::move(original);

        CHECK(get_list_size(moved) == 3);
        CHECK_FALSE(moved.empty());
        CHECK(original.empty());

        std::vector<int> expected{10, 20, 30};
        CHECK(list_equals_vector(moved, expected));
    }

    TEST_CASE("Initializer list assignment")
    {
        demo::forward_list<int> lst;
        lst = {100, 200, 300};

        CHECK(get_list_size(lst) == 3);
        std::vector<int> expected{100, 200, 300};
        CHECK(list_equals_vector(lst, expected));
    }
}

// ============================================
// 测试套件: 元素访问
// ============================================

TEST_SUITE("ForwardList Element Access")
{
    TEST_CASE("front() returns first element")
    {
        demo::forward_list<int> lst{1, 2, 3};
        CHECK(lst.front() == 1);

        lst.front() = 100;
        CHECK(lst.front() == 100);
    }

    TEST_CASE("const front()")
    {
        const demo::forward_list<int> lst{10, 20, 30};
        CHECK(lst.front() == 10);
    }
}

// ============================================
// 测试套件: 迭代器
// ============================================

TEST_SUITE("ForwardList Iterators")
{
    TEST_CASE("begin() and end() iterators")
    {
        demo::forward_list<int> lst{1, 2, 3, 4, 5};

        int sum = 0;
        for (auto it = lst.begin(); it != lst.end(); ++it)
        {
            sum += *it;
        }
        CHECK(sum == 15);
    }

    TEST_CASE("const iterators cbegin() and cend()")
    {
        demo::forward_list<int> lst{1, 2, 3};

        int sum = 0;
        for (auto it = lst.cbegin(); it != lst.cend(); ++it)
        {
            sum += *it;
        }
        CHECK(sum == 6);
    }

    TEST_CASE("range-based for loop")
    {
        demo::forward_list<int> lst{10, 20, 30};

        int sum = 0;
        for (const auto &elem : lst)
        {
            sum += elem;
        }
        CHECK(sum == 60);
    }

    TEST_CASE("before_begin() iterator")
    {
        demo::forward_list<int> lst{1, 2, 3};

        auto it = lst.before_begin();
        ++it; // 移动到第一个元素
        CHECK(*it == 1);
    }

    TEST_CASE("cbefore_begin() const iterator")
    {
        const demo::forward_list<int> lst{1, 2, 3};

        auto it = lst.cbefore_begin();
        ++it;
        CHECK(*it == 1);
    }

    TEST_CASE("iterator forward operations")
    {
        demo::forward_list<int> lst{1, 2, 3, 4, 5};

        auto it = lst.begin();
        CHECK(*it == 1);

        ++it;
        CHECK(*it == 2);

        auto old = it++;
        CHECK(*old == 2);
        CHECK(*it == 3);
    }

    TEST_CASE("iterator equality comparison")
    {
        demo::forward_list<int> lst{1, 2, 3};

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
        demo::forward_list<int> lst{1, 2, 3};

        demo::forward_list<int>::iterator it = lst.begin();
        demo::forward_list<int>::const_iterator cit = it;

        CHECK(cit == it);
        CHECK(*cit == *it);
    }
}

// ============================================
// 测试套件: 容量操作
// ============================================

TEST_SUITE("ForwardList Capacity")
{
    TEST_CASE("empty() returns true for empty list")
    {
        demo::forward_list<int> lst;
        CHECK(lst.empty());

        lst.push_front(1);
        CHECK_FALSE(lst.empty());
    }

    TEST_CASE("max_size() returns positive value")
    {
        demo::forward_list<int> lst;
        CHECK(lst.max_size() > 0);
    }

    TEST_CASE("resize() to larger size")
    {
        demo::forward_list<int> lst{1, 2, 3};
        lst.resize(5);

        CHECK(get_list_size(lst) == 5);
        std::vector<int> expected{1, 2, 3, 0, 0};
        CHECK(list_equals_vector(lst, expected));
    }

    TEST_CASE("resize() with explicit value")
    {
        demo::forward_list<int> lst{1, 2};
        lst.resize(5, 99);

        CHECK(get_list_size(lst) == 5);
        std::vector<int> expected{1, 2, 99, 99, 99};
        CHECK(list_equals_vector(lst, expected));
    }

    TEST_CASE("resize() to smaller size")
    {
        demo::forward_list<int> lst{1, 2, 3, 4, 5};
        lst.resize(2);

        CHECK(get_list_size(lst) == 2);
        std::vector<int> expected{1, 2};
        CHECK(list_equals_vector(lst, expected));
    }

    TEST_CASE("resize() to same size")
    {
        demo::forward_list<int> lst{1, 2, 3};
        lst.resize(3);
        CHECK(get_list_size(lst) == 3);
        std::vector<int> expected{1, 2, 3};
        CHECK(list_equals_vector(lst, expected));
    }

    TEST_CASE("resize() to zero")
    {
        demo::forward_list<int> lst{1, 2, 3};
        lst.resize(0);
        CHECK(lst.empty());
        CHECK(get_list_size(lst) == 0);
    }
}

// ============================================
// 测试套件: 修改器 - 插入操作
// ============================================

TEST_SUITE("ForwardList Modifiers - Insert")
{
    TEST_CASE("push_front() adds element to front")
    {
        demo::forward_list<int> lst;
        lst.push_front(3);
        lst.push_front(2);
        lst.push_front(1);

        std::vector<int> expected{1, 2, 3};
        CHECK(list_equals_vector(lst, expected));
    }

    TEST_CASE("pop_front() removes first element")
    {
        demo::forward_list<int> lst{1, 2, 3};
        lst.pop_front();

        CHECK(lst.front() == 2);
        CHECK(get_list_size(lst) == 2);
    }

    TEST_CASE("pop_front() on empty list throws exception")
    {
        demo::forward_list<int> lst;
        CHECK_THROWS_AS(lst.pop_front(), std::out_of_range);
    }

    TEST_CASE("insert_after() single element")
    {
        demo::forward_list<int> lst{1, 3, 4};
        auto it = lst.begin();
        auto result = lst.insert_after(it, 2);

        CHECK(*result == 2);
        std::vector<int> expected{1, 2, 3, 4};
        CHECK(list_equals_vector(lst, expected));
    }

    TEST_CASE("insert_after() at before_begin")
    {
        demo::forward_list<int> lst{2, 3};
        lst.insert_after(lst.before_begin(), 1);

        std::vector<int> expected{1, 2, 3};
        CHECK(list_equals_vector(lst, expected));
    }

    TEST_CASE("insert_after() multiple copies")
    {
        demo::forward_list<int> lst{1, 5};
        auto it = lst.begin();

        lst.insert_after(it, 3, 2);
        CHECK(get_list_size(lst) == 5);
        std::vector<int> expected{1, 2, 2, 2, 5};
        CHECK(list_equals_vector(lst, expected));
    }

    TEST_CASE("insert_after() iterator range")
    {
        demo::forward_list<int> lst{1, 6};
        std::vector<int> to_insert{2, 3, 4, 5};

        auto it = lst.begin();
        lst.insert_after(it, to_insert.begin(), to_insert.end());
        CHECK(get_list_size(lst) == 6);
        std::vector<int> expected{1, 2, 3, 4, 5, 6};
        CHECK(list_equals_vector(lst, expected));
    }

    TEST_CASE("insert_after() initializer list")
    {
        demo::forward_list<int> lst{1, 6};
        auto it = lst.begin();

        lst.insert_after(it, {2, 3, 4, 5});

        std::vector<int> expected{1, 2, 3, 4, 5, 6};
        CHECK(list_equals_vector(lst, expected));
    }

    TEST_CASE("emplace_after() constructs in-place")
    {
        demo::forward_list<std::string> lst{"hello", "world"};
        auto it = lst.begin();

        auto result = lst.emplace_after(it, 5, 'x');

        CHECK(*result == "xxxxx");
        CHECK(get_list_size(lst) == 3);
    }

    TEST_CASE("emplace_front() constructs at front")
    {
        demo::forward_list<std::string> lst;
        lst.emplace_front(3, 'a');

        CHECK(lst.front() == "aaa");
    }
}

// ============================================
// 测试套件: 修改器 - 删除操作
// ============================================

TEST_SUITE("ForwardList Modifiers - Erase")
{
    TEST_CASE("erase_after() single element")
    {
        demo::forward_list<int> lst{1, 2, 3, 4};
        auto it = lst.begin();
        auto result = lst.erase_after(it);

        CHECK(*result == 3);
        std::vector<int> expected{1, 3, 4};
        CHECK(list_equals_vector(lst, expected));
    }

    TEST_CASE("erase_after() range")
    {
        demo::forward_list<int> lst{1, 2, 3, 4, 5};
        auto first = lst.begin();
        auto last = lst.begin();
        ++last;
        ++last;
        ++last; // last 指向 4

        lst.erase_after(first, last);

        std::vector<int> expected{1, 4, 5};
        CHECK(list_equals_vector(lst, expected));
    }

    TEST_CASE("erase_after() on empty list")
    {
        demo::forward_list<int> lst;
        auto result = lst.erase_after(lst.before_begin());
        CHECK(result == lst.end());
        CHECK(lst.empty());
    }

    TEST_CASE("clear() removes all elements")
    {
        demo::forward_list<int> lst{1, 2, 3, 4, 5};
        lst.clear();

        CHECK(lst.empty());
        CHECK(get_list_size(lst) == 0);
    }

    TEST_CASE("clear() on empty list")
    {
        demo::forward_list<int> lst;
        lst.clear(); // 不应抛出异常
        CHECK(lst.empty());
    }
}

// ============================================
// 测试套件: 修改器 - 其他操作
// ============================================

TEST_SUITE("ForwardList Modifiers - Other")
{
    TEST_CASE("assign() with count and value")
    {
        demo::forward_list<int> lst{1, 2, 3};
        lst.assign(4, 5);

        std::vector<int> expected{5, 5, 5, 5};
        CHECK(list_equals_vector(lst, expected));
    }

    TEST_CASE("assign() with iterator range")
    {
        demo::forward_list<int> lst{1, 2, 3};
        std::vector<int> src{4, 5, 6, 7};
        lst.assign(src.begin(), src.end());

        CHECK(list_equals_vector(lst, src));
    }

    TEST_CASE("assign() with initializer list")
    {
        demo::forward_list<int> lst{1, 2, 3};
        lst.assign({8, 9, 10});

        std::vector<int> expected{8, 9, 10};
        CHECK(list_equals_vector(lst, expected));
    }

    TEST_CASE("swap() exchanges contents")
    {
        demo::forward_list<int> lst1{1, 2, 3};
        demo::forward_list<int> lst2{4, 5, 6, 7};

        lst1.swap(lst2);

        std::vector<int> expected1{4, 5, 6, 7};
        std::vector<int> expected2{1, 2, 3};
        CHECK(list_equals_vector(lst1, expected1));
        CHECK(list_equals_vector(lst2, expected2));
    }
}

// ============================================
// 测试套件: 操作函数
// ============================================

TEST_SUITE("ForwardList Operations")
{
    TEST_CASE("reverse() reverses the list")
    {
        demo::forward_list<int> lst{1, 2, 3, 4, 5};
        lst.reverse();

        std::vector<int> expected{5, 4, 3, 2, 1};
        CHECK(list_equals_vector(lst, expected));
    }

    TEST_CASE("reverse() on empty list")
    {
        demo::forward_list<int> lst;
        lst.reverse();
        CHECK(lst.empty());
    }

    TEST_CASE("reverse() on single element")
    {
        demo::forward_list<int> lst{42};
        lst.reverse();
        CHECK(lst.front() == 42);
        CHECK(get_list_size(lst) == 1);
    }

    TEST_CASE("sort() with default comparator")
    {
        demo::forward_list<int> lst{3, 1, 4, 1, 5, 9, 2, 6};
        lst.sort();

        std::vector<int> expected{1, 1, 2, 3, 4, 5, 6, 9};
        CHECK(list_equals_vector(lst, expected));
    }

    TEST_CASE("sort() with custom comparator")
    {
        demo::forward_list<int> lst{3, 1, 4, 1, 5, 9, 2, 6};
        lst.sort(std::greater<int>());

        std::vector<int> expected{9, 6, 5, 4, 3, 2, 1, 1};
        CHECK(list_equals_vector(lst, expected));
    }

    TEST_CASE("sort() on empty list")
    {
        demo::forward_list<int> lst;
        lst.sort();
        CHECK(lst.empty());
    }

    TEST_CASE("sort() on single element")
    {
        demo::forward_list<int> lst{42};
        lst.sort();
        CHECK(lst.front() == 42);
    }

    TEST_CASE("merge() merges two sorted lists")
    {
        demo::forward_list<int> lst1{1, 3, 5};
        demo::forward_list<int> lst2{2, 4, 6};
        lst1.merge(lst2);

        std::vector<int> expected{1, 2, 3, 4, 5, 6};
        CHECK(list_equals_vector(lst1, expected));
        CHECK(lst2.empty());
    }

    TEST_CASE("merge() with custom comparator")
    {
        demo::forward_list<int> lst1{5, 3, 1};
        demo::forward_list<int> lst2{6, 4, 2};
        lst1.merge(lst2, std::greater<int>());

        std::vector<int> expected{6, 5, 4, 3, 2, 1};
        CHECK(list_equals_vector(lst1, expected));
    }

    TEST_CASE("merge() with empty list")
    {
        demo::forward_list<int> lst1{1, 2, 3};
        demo::forward_list<int> lst2;
        lst1.merge(lst2);

        std::vector<int> expected{1, 2, 3};
        CHECK(list_equals_vector(lst1, expected));
    }

    TEST_CASE("merge() into empty list")
    {
        demo::forward_list<int> lst1;
        demo::forward_list<int> lst2{1, 2, 3};
        lst1.merge(lst2);

        std::vector<int> expected{1, 2, 3};
        CHECK(list_equals_vector(lst1, expected));
        CHECK(lst2.empty());
    }

    TEST_CASE("splice_after() splices entire list")
    {
        demo::forward_list<int> lst1{1, 2, 3};
        demo::forward_list<int> lst2{4, 5, 6};
        lst1.splice_after(lst1.before_begin(), lst2);

        std::vector<int> expected{4, 5, 6, 1, 2, 3};
        CHECK(list_equals_vector(lst1, expected));
        CHECK(lst2.empty());
    }

    TEST_CASE("splice_after() splices single element")
    {
        demo::forward_list<int> lst1{1, 3, 4};
        demo::forward_list<int> lst2{2};
        auto it = lst1.begin();
        lst1.splice_after(it, lst2, lst2.before_begin());

        std::vector<int> expected{1, 2, 3, 4};
        CHECK(list_equals_vector(lst1, expected));
    }

    TEST_CASE("splice_after() splices range")
    {
        demo::forward_list<int> lst1{1, 5, 6};
        demo::forward_list<int> lst2{2, 3, 4};
        lst1.splice_after(lst1.begin(), lst2, lst2.before_begin(), lst2.end());

        std::vector<int> expected{1, 2, 3, 4, 5, 6};
        CHECK(list_equals_vector(lst1, expected));
        CHECK(lst2.empty());
    }

    TEST_CASE("remove() removes matching elements")
    {
        demo::forward_list<int> lst{1, 2, 3, 2, 4, 2};
        size_t removed = lst.remove(2);

        std::vector<int> expected{1, 3, 4};
        CHECK(list_equals_vector(lst, expected));
        CHECK(removed == 3);
    }

    TEST_CASE("remove() no matching elements")
    {
        demo::forward_list<int> lst{1, 2, 3};
        size_t removed = lst.remove(99);

        CHECK(removed == 0);
        std::vector<int> expected{1, 2, 3};
        CHECK(list_equals_vector(lst, expected));
    }

    TEST_CASE("remove_if() removes elements satisfying predicate")
    {
        demo::forward_list<int> lst{1, 2, 3, 4, 5, 6};
        size_t removed = lst.remove_if([](int val)
                                       { return val % 2 == 0; });

        std::vector<int> expected{1, 3, 5};
        CHECK(list_equals_vector(lst, expected));
        CHECK(removed == 3);
    }

    TEST_CASE("unique() removes consecutive duplicates")
    {
        demo::forward_list<int> lst{1, 1, 2, 2, 2, 3, 3, 4};
        size_t removed = lst.unique();

        std::vector<int> expected{1, 2, 3, 4};
        CHECK(list_equals_vector(lst, expected));
        CHECK(removed == 4);
    }

    TEST_CASE("unique() with custom predicate")
    {
        demo::forward_list<int> lst{1, 3, 5, 2, 4, 6};
        size_t removed = lst.unique([](int a, int b)
                                    { return a % 2 == b % 2; });

        std::vector<int> expected{1, 2};
        CHECK(list_equals_vector(lst, expected));
        CHECK(removed == 4);
    }

    TEST_CASE("unique() on empty list")
    {
        demo::forward_list<int> lst;
        size_t removed = lst.unique();
        CHECK(removed == 0);
        CHECK(lst.empty());
    }
}

// ============================================
// 测试套件: 全局比较运算符
// ============================================

TEST_SUITE("ForwardList Comparison Operators")
{
    TEST_CASE("operator== compares contents")
    {
        demo::forward_list<int> lst1{1, 2, 3};
        demo::forward_list<int> lst2{1, 2, 3};
        demo::forward_list<int> lst3{1, 2, 3, 4};

        CHECK(lst1 == lst2);
        CHECK_FALSE(lst1 == lst3);
    }

    TEST_CASE("operator!= compares contents")
    {
        demo::forward_list<int> lst1{1, 2, 3};
        demo::forward_list<int> lst2{1, 2, 3};
        demo::forward_list<int> lst3{1, 2, 4};

        CHECK_FALSE(lst1 != lst2);
        CHECK(lst1 != lst3);
    }
}

// ============================================
// 测试套件: 边界情况和异常场景
// ============================================

TEST_SUITE("ForwardList Boundary Cases")
{
    TEST_CASE("single element list operations")
    {
        demo::forward_list<int> lst{42};

        CHECK(get_list_size(lst) == 1);
        CHECK(lst.front() == 42);

        lst.pop_front();
        CHECK(lst.empty());
    }

    TEST_CASE("insert duplicate elements")
    {
        demo::forward_list<int> lst{1, 2, 3};
        lst.push_front(2);
        lst.insert_after(lst.begin(), 2);

        CHECK(get_list_size(lst) == 5);
        std::vector<int> expected{2, 2, 1, 2, 3};
        CHECK(list_equals_vector(lst, expected));
    }

    TEST_CASE("self merge")
    {
        demo::forward_list<int> lst{1, 2, 3};
        lst.merge(lst);

        std::vector<int> expected{1, 2, 3};
        CHECK(list_equals_vector(lst, expected));
    }

    TEST_CASE("self splice_after")
    {
        demo::forward_list<int> lst{1, 2, 3};
        lst.splice_after(lst.before_begin(), lst);

        CHECK_FALSE(lst.empty());
        CHECK(list_equals_vector(lst, {1, 2, 3}));
    }

    TEST_CASE("move semantics with temporary")
    {
        demo::forward_list<int> lst = std::move(demo::forward_list<int>{1, 2, 3});

        std::vector<int> expected{1, 2, 3};
        CHECK(list_equals_vector(lst, expected));
    }

    TEST_CASE("get_allocator() returns allocator")
    {
        demo::forward_list<int> lst;
        auto alloc = lst.get_allocator();
        (void)alloc; // 测试编译通过
    }
}

// ============================================
// 测试套件: 字符串类型测试
// ============================================

TEST_SUITE("ForwardList String Type")
{
    TEST_CASE("string list operations")
    {
        demo::forward_list<std::string> lst{"hello", "world", "test"};
        CHECK(get_list_size(lst) == 3);

        lst.push_front("first");
        lst.insert_after(lst.begin(), "second");

        std::vector<std::string> expected{"first", "second", "hello", "world", "test"};
        CHECK(list_equals_vector(lst, expected));

        lst.remove("hello");
        std::vector<std::string> expected_after_remove{"first", "second", "world", "test"};
        CHECK(list_equals_vector(lst, expected_after_remove));
    }
}