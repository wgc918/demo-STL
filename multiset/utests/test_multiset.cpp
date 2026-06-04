#define DOCTEST_CONFIG_NO_MULTITHREADING

#include <algorithm>
#include <chrono>
#include <functional>
#include <random>
#include <string>
#include <vector>

#include "../multiset.h"
#include "doctest.h"

// ============================================
// 测试辅助工具
// ============================================

template <typename Key>
std::vector<Key> multiset_to_vector(const demo::multiset<Key>& ms)
{
    std::vector<Key> result;
    for (const auto& elem : ms)
    {
        result.push_back(elem);
    }
    return result;
}

template <typename Key, typename Compare = std::less<Key>>
bool multiset_equals_vector(const demo::multiset<Key, Compare>& ms,
                            const std::vector<Key>&             vec)
{
    auto vec_it = vec.begin();
    for (const auto& elem : ms)
    {
        if (vec_it == vec.end() || elem != *vec_it)
            return false;
        ++vec_it;
    }
    return vec_it == vec.end();
}

template <typename Key>
bool multisets_equal(const demo::multiset<Key>& a,
                     const demo::multiset<Key>& b)
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

template <typename Key>
void assert_multiset_sorted(const demo::multiset<Key>& ms)
{
    auto prev = ms.begin();
    if (prev == ms.end())
        return;
    for (auto curr = ++ms.begin(); curr != ms.end(); ++curr)
    {
        CHECK(*curr >= *prev);
        ++prev;
    }
}

// ============================================
// 测试套件: 构造函数
// ============================================

TEST_SUITE("Multiset Constructors")
{
    TEST_CASE("Default constructor creates empty multiset")
    {
        demo::multiset<int> ms;
        CHECK(ms.empty());
        CHECK(ms.size() == 0);
        CHECK(ms.begin() == ms.end());
    }

    TEST_CASE("Constructor with comparator")
    {
        auto comp = std::greater<int>();
        demo::multiset<int, std::greater<int>> ms(comp);
        ms.insert(1);
        ms.insert(2);
        ms.insert(3);

        std::vector<int> expected = {3, 2, 1};
        CHECK(multiset_equals_vector(ms, expected));
    }

    TEST_CASE("Initializer list constructor")
    {
        demo::multiset<int> ms = {3, 1, 2};
        CHECK(ms.size() == 3);
        assert_multiset_sorted(ms);
    }

    TEST_CASE("Initializer list constructor with duplicate keys")
    {
        demo::multiset<int> ms = {1, 2, 1, 3, 2};
        CHECK(ms.size() == 5);
        assert_multiset_sorted(ms);
    }

    TEST_CASE("Iterator range constructor")
    {
        std::vector<int> vec = {3, 1, 2, 1, 3};
        demo::multiset<int> ms(vec.begin(), vec.end());
        CHECK(ms.size() == 5);
        assert_multiset_sorted(ms);
    }

    TEST_CASE("Copy constructor")
    {
        demo::multiset<int> ms1 = {1, 2, 3, 1, 2};
        demo::multiset<int> ms2(ms1);

        CHECK(ms2.size() == 5);
        CHECK(ms1.size() == 5);
        assert_multiset_sorted(ms2);
        CHECK(multisets_equal(ms1, ms2));
    }

    TEST_CASE("Move constructor")
    {
        demo::multiset<int> ms1 = {1, 2, 3, 1, 2};
        auto               size_before = ms1.size();

        demo::multiset<int> ms2(std::move(ms1));

        CHECK(ms2.size() == size_before);
        assert_multiset_sorted(ms2);
        CHECK(ms1.empty());
        CHECK(ms1.size() == 0);
    }

    TEST_CASE("Constructor from sorted vector")
    {
        std::vector<int> vec = {1, 1, 2, 3, 4, 4};
        demo::multiset<int> ms(
            vec, demo::multiset<int>::sorted_tag{});
        CHECK(ms.size() == 6);
        assert_multiset_sorted(ms);
    }

    TEST_CASE("Constructor from unsorted vector")
    {
        std::vector<int> vec = {3, 1, 4, 1, 2, 4};
        demo::multiset<int> ms(
            vec, demo::multiset<int>::unsorted_tag{});
        CHECK(ms.size() == 6);
        assert_multiset_sorted(ms);
    }
}

// ============================================
// 测试套件: 赋值操作符
// ============================================

TEST_SUITE("Multiset Assignment Operators")
{
    TEST_CASE("Copy assignment operator")
    {
        demo::multiset<int> ms1 = {1, 2, 3, 1, 2};
        demo::multiset<int> ms2;

        ms2 = ms1;

        CHECK(ms2.size() == 5);
        CHECK(ms1.size() == 5);
        assert_multiset_sorted(ms2);
        CHECK(multisets_equal(ms1, ms2));
    }

    TEST_CASE("Move assignment operator")
    {
        demo::multiset<int> ms1 = {1, 2, 3, 1, 2};
        auto               size_before = ms1.size();
        demo::multiset<int> ms2;

        ms2 = std::move(ms1);
        assert_multiset_sorted(ms2);
        CHECK(ms2.size() == size_before);
        CHECK(ms1.empty());
        CHECK(ms1.size() == 0);
    }

    TEST_CASE("Initializer list assignment")
    {
        demo::multiset<int> ms;
        ms = {3, 1, 2, 1};

        CHECK(ms.size() == 4);
        assert_multiset_sorted(ms);
    }

    TEST_CASE("Self assignment")
    {
        demo::multiset<int> ms = {1, 2, 1};
        ms = ms;
        CHECK(ms.size() == 3);
    }
}

// ============================================
// 测试套件: 迭代器
// ============================================

TEST_SUITE("Multiset Iterators")
{
    TEST_CASE("begin() and end() iterators")
    {
        demo::multiset<int> ms = {1, 2, 3};

        auto it = ms.begin();
        CHECK(*it == 1);

        ++it;
        CHECK(*it == 2);

        ++it;
        CHECK(*it == 3);

        ++it;
        CHECK(it == ms.end());
    }

    TEST_CASE("const iterators cbegin() and cend()")
    {
        const demo::multiset<int> ms = {1, 2, 3};

        auto it = ms.cbegin();
        CHECK(*it == 1);
        ++it;
        CHECK(*it == 2);
        ++it;
        CHECK(*it == 3);
        ++it;
        CHECK(it == ms.cend());
    }

    TEST_CASE("range-based for loop")
    {
        demo::multiset<int> ms = {1, 2, 3, 1, 2};
        int                count = 0;
        int                prev = -1;
        for (const auto& val : ms)
        {
            CHECK(val >= prev);
            prev = val;
            ++count;
        }
        CHECK(count == 5);
    }

    TEST_CASE("range-based for loop with duplicate keys")
    {
        demo::multiset<int> ms = {1, 1, 1, 2, 2, 3};

        int count = 0;
        std::vector<int> seen;
        for (const auto& val : ms)
        {
            seen.push_back(val);
            count++;
        }
        CHECK(count == 6);
        CHECK(seen.size() == 6);
        assert_multiset_sorted(ms);
    }

    TEST_CASE("iterator decrement operator--")
    {
        demo::multiset<int> ms = {1, 2, 3};
        auto               it  = ms.end();

        --it;
        CHECK(*it == 3);
        it--;
        CHECK(*it == 2);
        --it;
        CHECK(*it == 1);
    }

    TEST_CASE("reverse iterators rbegin() and rend()")
    {
        demo::multiset<int> ms = {1, 1, 2, 3};
        int                expected_key = 3;

        auto it = ms.rbegin();
        CHECK(*it == expected_key);
        ++it;
        CHECK(*it == 2);
        ++it;
        CHECK(*it == 1);
        ++it;
        CHECK(*it == 1);
    }

    TEST_CASE("const reverse iterators")
    {
        const demo::multiset<int> ms = {1, 2, 3};

        auto it = ms.crbegin();
        CHECK(*it == 3);
        ++it;
        CHECK(*it == 2);
        ++it;
        CHECK(*it == 1);
    }

    TEST_CASE("empty multiset iterators")
    {
        demo::multiset<int> ms;

        CHECK(ms.begin() == ms.end());
        CHECK(ms.cbegin() == ms.cend());
        CHECK(ms.rbegin() == ms.rend());
        CHECK(ms.crbegin() == ms.crend());
    }

    TEST_CASE("iterator post-increment and post-decrement")
    {
        demo::multiset<int> ms = {1, 2, 3};
        auto               it  = ms.begin();

        auto it_copy = it++;
        CHECK(*it_copy == 1);
        CHECK(*it == 2);

        it_copy = it--;
        CHECK(*it_copy == 2);
        CHECK(*it == 1);
    }

    TEST_CASE("iterator dereference and arrow operators")
    {
        demo::multiset<int> ms = {1};
        auto               it  = ms.begin();

        CHECK(*it == 1);
        CHECK(it.operator->() != nullptr);
        CHECK(*it.operator->() == 1);
    }

    TEST_CASE("iterator to const_iterator conversion")
    {
        demo::multiset<int> ms = {1};
        demo::multiset<int>::iterator       it       = ms.begin();
        demo::multiset<int>::const_iterator const_it = it;

        CHECK(*const_it == 1);
    }
}

// ============================================
// 测试套件: 容量操作
// ============================================

TEST_SUITE("Multiset Capacity")
{
    TEST_CASE("empty() returns true for empty multiset")
    {
        demo::multiset<int> ms;
        CHECK(ms.empty());
    }

    TEST_CASE("empty() returns false for non-empty multiset")
    {
        demo::multiset<int> ms = {1};
        CHECK_FALSE(ms.empty());
    }

    TEST_CASE("size() returns correct number of elements with duplicates")
    {
        demo::multiset<int> ms;

        CHECK(ms.size() == 0);

        ms.insert(1);
        CHECK(ms.size() == 1);

        ms.insert(1);
        CHECK(ms.size() == 2);

        ms.insert(2);
        CHECK(ms.size() == 3);

        ms.erase(1);
        CHECK(ms.size() == 1);
    }

    TEST_CASE("max_size() returns positive value")
    {
        demo::multiset<int> ms;
        CHECK(ms.max_size() > 0);
    }
}

// ============================================
// 测试套件: 修改器 - 插入操作
// ============================================

TEST_SUITE("Multiset Modifiers - Insert")
{
    TEST_CASE("insert() with const value")
    {
        demo::multiset<int> ms;
        int                 val = 1;

        auto it = ms.insert(val);

        CHECK(ms.size() == 1);
        CHECK(*it == 1);
        CHECK(ms.count(1) == 1);
    }

    TEST_CASE("insert() with rvalue")
    {
        demo::multiset<int> ms;

        auto it = ms.insert(1);

        CHECK(ms.size() == 1);
        CHECK(*it == 1);
    }

    TEST_CASE("insert() returns iterator to inserted element")
    {
        demo::multiset<int> ms;

        auto it = ms.insert(1);
        CHECK(*it == 1);
    }

    TEST_CASE("insert() allows duplicate keys")
    {
        demo::multiset<int> ms;

        auto it1 = ms.insert(1);
        auto it2 = ms.insert(1);
        auto it3 = ms.insert(1);

        CHECK(ms.size() == 3);
        CHECK(*it1 == 1);
        CHECK(*it2 == 1);
        CHECK(*it3 == 1);

        CHECK(it1 != it2);
        CHECK(it2 != it3);
        CHECK(it1 != it3);
    }

    TEST_CASE("insert() maintains sorted order with duplicates")
    {
        demo::multiset<int> ms;

        ms.insert(3);
        ms.insert(1);
        ms.insert(3);
        ms.insert(2);
        ms.insert(1);

        CHECK(ms.size() == 5);
        assert_multiset_sorted(ms);

        CHECK(ms.count(1) == 2);
        CHECK(ms.count(2) == 1);
        CHECK(ms.count(3) == 2);
    }

    TEST_CASE("insert() with hint position")
    {
        demo::multiset<int> ms = {1, 3};
        auto               hint   = ms.find(1);
        auto               result = ms.insert(hint, 2);

        CHECK(ms.size() == 3);
        CHECK(*result == 2);
        assert_multiset_sorted(ms);
    }

    TEST_CASE("insert() with hint and duplicate key")
    {
        demo::multiset<int> ms = {1, 3};
        auto               hint   = ms.find(1);
        auto               result = ms.insert(hint, 1);

        CHECK(ms.size() == 3);
        CHECK(*result == 1);
        assert_multiset_sorted(ms);
    }

    TEST_CASE("insert() with iterator range")
    {
        std::vector<int> vec = {1, 2, 1, 3};
        demo::multiset<int> ms;

        ms.insert(vec.begin(), vec.end());

        CHECK(ms.size() == 4);
        assert_multiset_sorted(ms);
    }

    TEST_CASE("insert() with initializer list")
    {
        demo::multiset<int> ms;

        ms.insert({1, 2, 1, 3});

        CHECK(ms.size() == 4);
        assert_multiset_sorted(ms);
    }

    TEST_CASE("emplace() inserts element")
    {
        demo::multiset<std::string> ms;

        auto it = ms.emplace("hello");
        CHECK(*it == "hello");
        CHECK(ms.count("hello") == 1);
    }

    TEST_CASE("emplace() allows duplicate elements")
    {
        demo::multiset<int> ms;

        auto it1 = ms.emplace(1);
        auto it2 = ms.emplace(1);

        CHECK(ms.size() == 2);
        CHECK(*it1 == 1);
        CHECK(*it2 == 1);
    }

    TEST_CASE("emplace_hint() inserts element successfully")
    {
        demo::multiset<int> ms = {1, 3};

        auto hint   = ms.find(1);
        auto result = ms.emplace_hint(hint, 0);
        CHECK(*result == 0);
        CHECK(ms.size() == 3);
        assert_multiset_sorted(ms);
    }

    TEST_CASE("emplace_hint() inserts duplicate element")
    {
        demo::multiset<int> ms = {1, 3};

        auto hint   = ms.find(1);
        auto result = ms.emplace_hint(hint, 2);
        CHECK(*result == 2);
        CHECK(ms.size() == 3);
        assert_multiset_sorted(ms);
    }
}

// ============================================
// 测试套件: 修改器 - 删除操作
// ============================================

TEST_SUITE("Multiset Modifiers - Erase")
{
    TEST_CASE("erase() by iterator removes single element")
    {
        demo::multiset<int> ms = {1, 1, 2, 3};

        auto it = ms.find(1);
        CHECK(it != ms.end());
        CHECK(*it == 1);

        ms.erase(it);

        CHECK(ms.size() == 3);
        CHECK(ms.count(1) == 1);
        CHECK(ms.count(2) == 1);
        CHECK(ms.count(3) == 1);
        assert_multiset_sorted(ms);
    }

    TEST_CASE("erase() returns iterator to next element")
    {
        demo::multiset<int> ms = {1, 2, 3};

        auto it      = ms.find(2);
        auto next_it = ms.erase(it);

        CHECK(*next_it == 3);
    }

    TEST_CASE("erase() by key removes all elements with that key")
    {
        demo::multiset<int> ms = {1, 1, 1, 2, 3};

        auto count = ms.erase(1);

        CHECK(count == 3);
        CHECK(ms.size() == 2);
        CHECK(ms.find(1) == ms.end());
        CHECK(ms.find(2) != ms.end());
        CHECK(ms.find(3) != ms.end());
        assert_multiset_sorted(ms);
    }

    TEST_CASE("erase() by key returns 0 for non-existing key")
    {
        demo::multiset<int> ms = {1};

        auto count = ms.erase(2);

        CHECK(count == 0);
        CHECK(ms.size() == 1);
    }

    TEST_CASE("erase() range")
    {
        demo::multiset<int> ms = {1, 2, 3, 4, 5};

        auto first = ms.find(2);
        auto last  = ms.find(4);
        ms.erase(first, last);

        CHECK(ms.size() == 3);
        CHECK(ms.find(2) == ms.end());
        CHECK(ms.find(3) == ms.end());
        CHECK(ms.find(1) != ms.end());
        CHECK(ms.find(4) != ms.end());
        CHECK(ms.find(5) != ms.end());
    }

    TEST_CASE("clear() removes all elements")
    {
        demo::multiset<int> ms = {1, 1, 2, 3};

        ms.clear();

        CHECK(ms.empty());
        CHECK(ms.size() == 0);
    }

    TEST_CASE("clear() on empty multiset")
    {
        demo::multiset<int> ms;

        CHECK_NOTHROW(ms.clear());
        CHECK(ms.empty());
    }

    TEST_CASE("erase single element by key")
    {
        demo::multiset<int> ms = {1};

        auto count = ms.erase(1);

        CHECK(count == 1);
        CHECK(ms.empty());
        CHECK(ms.size() == 0);
    }

    TEST_CASE("erase root node")
    {
        demo::multiset<int> ms = {2, 1, 3};

        auto it = ms.find(2);
        ms.erase(it);

        CHECK(ms.size() == 2);
        CHECK(ms.find(2) == ms.end());
        assert_multiset_sorted(ms);
    }

    TEST_CASE("erase non-existing key")
    {
        demo::multiset<int> ms = {1};

        auto count = ms.erase(2);

        CHECK(count == 0);
        CHECK(ms.size() == 1);
    }
}

// ============================================
// 测试套件: 修改器 - 其他操作
// ============================================

TEST_SUITE("Multiset Modifiers - Other")
{
    TEST_CASE("swap() exchanges contents")
    {
        demo::multiset<int> ms1 = {1, 2};
        demo::multiset<int> ms2 = {3, 4, 5};

        ms1.swap(ms2);

        CHECK(ms1.size() == 3);
        CHECK(ms1.count(3) == 1);
        CHECK(ms1.count(4) == 1);
        CHECK(ms1.count(5) == 1);

        CHECK(ms2.size() == 2);
        CHECK(ms2.count(1) == 1);
        CHECK(ms2.count(2) == 1);
    }

    TEST_CASE("swap() with empty multiset")
    {
        demo::multiset<int> ms1 = {1};
        demo::multiset<int> ms2;

        ms1.swap(ms2);

        CHECK(ms1.empty());
        CHECK(ms2.size() == 1);
    }
}

// ============================================
// 测试套件: 查找操作
// ============================================

TEST_SUITE("Multiset Lookup")
{
    TEST_CASE("find() returns iterator to first element with matching key")
    {
        demo::multiset<int> ms = {1, 1, 2, 3};

        auto it = ms.find(1);

        CHECK(it != ms.end());
        CHECK(*it == 1);
    }

    TEST_CASE("find() returns end for non-existing element")
    {
        demo::multiset<int> ms = {1, 2};

        auto it = ms.find(3);

        CHECK(it == ms.end());
    }

    TEST_CASE("const find() returns iterator to existing element")
    {
        const demo::multiset<int> ms = {1, 2};

        auto it = ms.find(1);

        CHECK(it != ms.end());
        CHECK(*it == 1);
    }

    TEST_CASE("count() returns correct count for duplicate keys")
    {
        demo::multiset<int> ms = {1, 1, 1, 2, 3};

        CHECK(ms.count(1) == 3);
        CHECK(ms.count(2) == 1);
        CHECK(ms.count(3) == 1);
    }

    TEST_CASE("count() returns 0 for non-existing key")
    {
        demo::multiset<int> ms = {1, 2};

        CHECK(ms.count(3) == 0);
    }

    TEST_CASE("count() returns 0 for empty multiset")
    {
        demo::multiset<int> ms;
        CHECK(ms.count(1) == 0);
    }

    TEST_CASE("lower_bound() returns first not less than key")
    {
        demo::multiset<int> ms = {1, 1, 3, 5};

        auto it = ms.lower_bound(2);
        CHECK(*it == 3);

        it = ms.lower_bound(1);
        CHECK(*it == 1);

        it = ms.lower_bound(6);
        CHECK(it == ms.end());
    }

    TEST_CASE("upper_bound() returns first greater than key")
    {
        demo::multiset<int> ms = {1, 1, 3, 5};

        auto it = ms.upper_bound(1);
        CHECK(*it == 3);

        it = ms.upper_bound(3);
        CHECK(*it == 5);

        it = ms.upper_bound(5);
        CHECK(it == ms.end());
    }

    TEST_CASE("equal_range() returns correct range for duplicate keys")
    {
        demo::multiset<int> ms = {1, 1, 1, 2, 3};

        auto range = ms.equal_range(1);
        auto count = std::distance(range.first, range.second);
        CHECK(count == 3);

        CHECK(*range.first == 1);
        ++range.first;
        CHECK(*range.first == 1);
        ++range.first;
        CHECK(*range.first == 1);
        ++range.first;
        CHECK(range.first == range.second);
    }

    TEST_CASE("equal_range() returns empty range for non-existing key")
    {
        demo::multiset<int> ms = {1};

        auto range = ms.equal_range(2);
        CHECK(range.first == range.second);
    }

    TEST_CASE("const equal_range()")
    {
        const demo::multiset<int> ms = {1, 1, 2};

        auto range = ms.equal_range(1);
        auto count = std::distance(range.first, range.second);
        CHECK(count == 2);
    }

    TEST_CASE("key_comp() returns comparator")
    {
        demo::multiset<int> ms;
        auto                comp = ms.key_comp();

        CHECK(comp(1, 2));
        CHECK_FALSE(comp(2, 1));
    }
}

// ============================================
// 测试套件: 全局比较运算符
// ============================================

TEST_SUITE("Multiset Comparison Operators")
{
    TEST_CASE("operator== compares contents")
    {
        demo::multiset<int> ms1 = {1, 1, 2};
        demo::multiset<int> ms2 = {1, 1, 2};
        demo::multiset<int> ms3 = {1, 2};

        CHECK(ms1 == ms2);
        CHECK_FALSE(ms1 == ms3);
    }

    TEST_CASE("operator!= compares contents")
    {
        demo::multiset<int> ms1 = {1, 2};
        demo::multiset<int> ms2 = {1, 2};
        demo::multiset<int> ms3 = {1, 3};

        CHECK_FALSE(ms1 != ms2);
        CHECK(ms1 != ms3);
    }

    TEST_CASE("operator== and != with different sizes")
    {
        demo::multiset<int> ms1 = {1};
        demo::multiset<int> ms2 = {1, 2};

        CHECK_FALSE(ms1 == ms2);
        CHECK(ms1 != ms2);
    }

    TEST_CASE("operator== with duplicate keys")
    {
        demo::multiset<int> ms1 = {1, 1, 2};
        demo::multiset<int> ms2 = {1, 1, 2};
        demo::multiset<int> ms3 = {1, 2, 2};

        CHECK(ms1 == ms2);
        CHECK_FALSE(ms1 == ms3);
    }
}

// ============================================
// 测试套件: 操作函数
// ============================================

TEST_SUITE("Multiset Operations")
{
    TEST_CASE("merge() merges two multisets")
    {
        demo::multiset<int> ms1 = {1, 3};
        demo::multiset<int> ms2 = {2, 4};

        ms1.merge(ms2);

        CHECK(ms1.size() == 4);
        assert_multiset_sorted(ms1);
    }

    TEST_CASE("merge() with overlapping keys")
    {
        demo::multiset<int> ms1 = {1, 2};
        demo::multiset<int> ms2 = {2, 3};

        ms1.merge(ms2);

        CHECK(ms1.size() == 4);
        assert_multiset_sorted(ms1);
        CHECK(ms1.count(2) == 2);
    }

    TEST_CASE("merge() with empty multiset")
    {
        demo::multiset<int> ms1 = {1};
        demo::multiset<int> ms2;

        ms1.merge(ms2);

        CHECK(ms1.size() == 1);
    }

    TEST_CASE("get_allocator() returns allocator")
    {
        demo::multiset<int> ms;
        auto                alloc = ms.get_allocator();

        CHECK(ms.size() == 0);
    }
}

// ============================================
// 测试套件: 边界情况和异常场景
// ============================================

TEST_SUITE("Multiset Boundary Cases")
{
    TEST_CASE("single element multiset operations")
    {
        demo::multiset<int> ms = {1};

        CHECK(ms.size() == 1);
        CHECK(!ms.empty());
        CHECK(ms.find(1) != ms.end());
        CHECK(ms.find(2) == ms.end());

        ms.erase(1);
        CHECK(ms.empty());
    }

    TEST_CASE("many duplicate keys")
    {
        demo::multiset<int> ms;

        for (int i = 0; i < 10; ++i)
        {
            ms.insert(1);
        }

        CHECK(ms.size() == 10);
        CHECK(ms.count(1) == 10);
        assert_multiset_sorted(ms);

        auto range = ms.equal_range(1);
        int  count = 0;
        for (auto it = range.first; it != range.second; ++it)
        {
            CHECK(*it == 1);
            count++;
        }
        CHECK(count == 10);
    }

    TEST_CASE("move semantics with temporary")
    {
        demo::multiset<std::string> ms;
        ms.insert(std::string("test"));

        CHECK(ms.size() == 1);
    }

    TEST_CASE("insert and erase alternating")
    {
        demo::multiset<int> ms;

        for (int i = 0; i < 10; ++i)
        {
            ms.insert(i);
        }

        for (int i = 0; i < 10; i += 2)
        {
            ms.erase(i);
        }

        CHECK(ms.size() == 5);
        for (int i = 1; i < 10; i += 2)
        {
            CHECK(ms.find(i) != ms.end());
        }
    }

    TEST_CASE("large key range")
    {
        demo::multiset<int> ms;

        ms.insert(-1000);
        ms.insert(0);
        ms.insert(1000);

        CHECK(ms.size() == 3);
        assert_multiset_sorted(ms);
    }

#ifndef NDEBUG
    TEST_CASE("validate_tree() in debug mode")
    {
        demo::multiset<int> ms;
        ms.insert(1);
        ms.insert(2);
        ms.insert(3);
        ms.insert(1);
        for (int i = 145; i < 500; ++i)
        {
            ms.insert(i + rand() % 300);
        }

        CHECK(ms.validate_tree());
    }
#endif
}

// ============================================
// 测试套件: 字符串类型测试
// ============================================

TEST_SUITE("Multiset String Type")
{
    TEST_CASE("string key multiset operations")
    {
        demo::multiset<std::string> ms;

        ms.insert("apple");
        ms.insert("banana");
        ms.insert("apple");
        ms.insert("cherry");

        CHECK(ms.size() == 4);
        CHECK(ms.count("apple") == 2);
        CHECK(ms.count("banana") == 1);
        CHECK(ms.count("cherry") == 1);

        ms.erase("apple");
        CHECK(ms.size() == 2);
        CHECK(ms.count("apple") == 0);
    }

    TEST_CASE("string key comparison")
    {
        demo::multiset<std::string> ms = {"a", "b", "c"};

        assert_multiset_sorted(ms);
    }
}

// ============================================
// 测试套件: 压力测试
// ============================================

TEST_SUITE("Multiset Stress Tests")
{
    TEST_CASE("insert and iterate 10000 elements")
    {
        demo::multiset<int> ms;

        for (int i = 0; i < 10000; ++i)
        {
            ms.insert(i);
        }

        CHECK(ms.size() == 10000);

        int expected = 0;
        for (const auto& val : ms)
        {
            CHECK(val == expected);
            ++expected;
        }
    }

    TEST_CASE("insert duplicate keys in large quantity")
    {
        demo::multiset<int> ms;

        for (int i = 0; i < 1000; ++i)
        {
            ms.insert(1);
        }
        for (int i = 0; i < 1000; ++i)
        {
            ms.insert(2);
        }

        CHECK(ms.size() == 2000);
        CHECK(ms.count(1) == 1000);
        CHECK(ms.count(2) == 1000);
        assert_multiset_sorted(ms);
    }

    TEST_CASE("insert in random order")
    {
        demo::multiset<int> ms;
        std::vector<int>    keys;

        for (int i = 0; i < 5000; ++i)
        {
            keys.push_back(i);
        }

        std::random_device rd;
        std::mt19937       g(rd());
        std::shuffle(keys.begin(), keys.end(), g);

        for (int key : keys)
        {
            ms.insert(key);
        }

        CHECK(ms.size() == 5000);
        assert_multiset_sorted(ms);
    }

    TEST_CASE("sequential insert and erase")
    {
        demo::multiset<int> ms;

        for (int i = 0; i < 1000; ++i)
        {
            ms.insert(i);
        }

        for (int i = 0; i < 1000; ++i)
        {
            ms.erase(i);
        }

        CHECK(ms.empty());
        CHECK(ms.size() == 0);
    }

    TEST_CASE("erase all duplicate keys at once")
    {
        demo::multiset<int> ms;

        for (int i = 0; i < 100; ++i)
        {
            ms.insert(1);
        }

        auto count = ms.erase(1);
        CHECK(count == 100);
        CHECK(ms.empty());
        CHECK(ms.count(1) == 0);
    }

    TEST_CASE("alternating insert and erase")
    {
        demo::multiset<int> ms;

        for (int i = 0; i < 1000; ++i)
        {
            ms.insert(i);
            if (i % 10 == 0)
            {
                ms.erase(i - 5);
            }
        }

        CHECK(ms.size() > 0);
        assert_multiset_sorted(ms);
    }

    TEST_CASE("performance test - insert 50000 elements")
    {
        demo::multiset<int> ms;

        auto start = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < 50000; ++i)
        {
            ms.insert(i);
        }
        auto end = std::chrono::high_resolution_clock::now();

        auto duration =
            std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

        CHECK(ms.size() == 50000);
        CHECK(duration.count() < 5000);
    }

    TEST_CASE("performance test - find speed")
    {
        demo::multiset<int> ms;
        for (int i = 0; i < 50000; ++i)
        {
            ms.insert(i);
        }

        auto start = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < 50000; ++i)
        {
            auto it = ms.find(i);
            CHECK(it != ms.end());
            CHECK(*it == i);
        }
        auto end = std::chrono::high_resolution_clock::now();

        auto duration =
            std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

        CHECK(duration.count() < 1000);
    }

    TEST_CASE("large data persistence")
    {
        demo::multiset<int> ms;

        for (int i = 0; i < 100000; ++i)
        {
            ms.insert(i);
        }

        CHECK(ms.size() == 100000);

        for (int i = 0; i < 100000; i += 1000)
        {
            CHECK(ms.find(i) != ms.end());
        }
    }
}

// ============================================
// 测试套件: 核心 multiset 特性验证
// ============================================

TEST_SUITE("Multiset Core Features")
{
    TEST_CASE("insert always succeeds with duplicate keys")
    {
        demo::multiset<int> ms;

        for (int i = 0; i < 20; ++i)
        {
            auto it = ms.insert(1);
            CHECK(it != ms.end());
        }

        CHECK(ms.size() == 20);
        CHECK(ms.count(1) == 20);
    }

    TEST_CASE("erase by key removes all matching elements")
    {
        demo::multiset<int> ms;

        ms.insert(1);
        ms.insert(1);
        ms.insert(1);
        ms.insert(2);

        auto count = ms.erase(1);
        CHECK(count == 3);
        CHECK(ms.size() == 1);
        CHECK(ms.count(1) == 0);
        CHECK(ms.find(2) != ms.end());
    }

    TEST_CASE("equal_range covers all duplicate keys")
    {
        demo::multiset<int> ms = {1, 2, 1, 3, 1, 2};

        auto range = ms.equal_range(1);
        int  count = 0;
        for (auto it = range.first; it != range.second; ++it)
        {
            CHECK(*it == 1);
            count++;
        }
        CHECK(count == 3);

        range = ms.equal_range(2);
        count = 0;
        for (auto it = range.first; it != range.second; ++it)
        {
            CHECK(*it == 2);
            count++;
        }
        CHECK(count == 2);
    }

    TEST_CASE("lower_bound and upper_bound with duplicate keys")
    {
        demo::multiset<int> ms = {1, 1, 1, 2, 3};

        auto lb = ms.lower_bound(1);
        CHECK(*lb == 1);

        auto ub = ms.upper_bound(1);
        CHECK(*ub == 2);

        auto count = std::distance(lb, ub);
        CHECK(count == 3);
    }

    TEST_CASE("count reflects actual number of elements")
    {
        demo::multiset<int> ms;

        CHECK(ms.count(1) == 0);

        ms.insert(1);
        CHECK(ms.count(1) == 1);

        ms.insert(1);
        CHECK(ms.count(1) == 2);

        ms.erase(ms.find(1));
        CHECK(ms.count(1) == 1);

        ms.erase(1);
        CHECK(ms.count(1) == 0);
    }
}

// ============================================
// 测试套件: 算法测试
// ============================================

TEST_SUITE("Multiset Algorithms")
{
    TEST_CASE("std::for_each on multiset")
    {
        demo::multiset<int> ms = {1, 1, 2, 3};
        int                 sum = 0;

        std::for_each(ms.begin(), ms.end(),
                      [&](const auto& val) { sum += val; });

        CHECK(sum == 7);
    }

    TEST_CASE("std::find_if on multiset")
    {
        demo::multiset<int> ms = {1, 2, 3};

        auto it =
            std::find_if(ms.begin(), ms.end(), [](int val) { return val == 2; });

        CHECK(it != ms.end());
        CHECK(*it == 2);
    }

    TEST_CASE("std::count_if on multiset")
    {
        demo::multiset<int> ms = {1, 2, 2, 3, 2, 4};

        auto count = std::count_if(ms.begin(), ms.end(),
                                   [](int val) { return val == 2; });

        CHECK(count == 3);
    }
}

// ============================================
// 测试套件: 特殊类型测试
// ============================================

TEST_SUITE("Multiset Special Types")
{
    TEST_CASE("multiset with custom struct as key")
    {
        struct MyStruct
        {
            int id;

            bool operator==(const MyStruct& other) const
            {
                return id == other.id;
            }
        };

        struct MyStructCompare
        {
            bool operator()(const MyStruct& a, const MyStruct& b) const
            {
                return a.id < b.id;
            }
        };

        demo::multiset<MyStruct, MyStructCompare> ms;
        ms.insert({1});
        ms.insert({1});
        ms.insert({2});

        CHECK(ms.size() == 3);
        CHECK(ms.count({1}) == 2);
        CHECK(ms.count({2}) == 1);
    }
}