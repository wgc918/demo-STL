#define DOCTEST_CONFIG_NO_MULTITHREADING

#include <algorithm>
#include <chrono>
#include <iostream>
#include <random>
#include <string>
#include <vector>

#include "../set.h"
#include "doctest.h"

template <typename Key>
std::vector<Key> set_to_vector(const demo::set<Key>& st)
{
    std::vector<Key> result;
    for (const auto& elem : st)
    {
        result.push_back(elem);
    }
    return result;
}

template <typename Key, typename Compare = std::less<Key>>
bool set_equals_vector(const demo::set<Key, Compare>& st, const std::vector<Key>& vec)
{
    auto vec_it = vec.begin();
    for (const auto& elem : st)
    {
        if (vec_it == vec.end() || elem != *vec_it)
            return false;
        ++vec_it;
    }
    return vec_it == vec.end();
}

template <typename Key>
bool sets_equal(const demo::set<Key>& a, const demo::set<Key>& b)
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
void assert_set_sorted(const demo::set<Key>& st)
{
    auto prev = st.begin();
    if (prev == st.end())
        return;
    for (auto curr = ++st.begin(); curr != st.end(); ++curr)
    {
        CHECK(*curr > *prev);
        ++prev;
    }
}

TEST_SUITE("Set Constructors")
{
    TEST_CASE("Default constructor creates empty set")
    {
        demo::set<int> s;
        CHECK(s.empty());
        CHECK(s.size() == 0);
        CHECK(s.begin() == s.end());
    }

    TEST_CASE("Constructor with comparator")
    {
        auto                               comp = std::greater<int>();
        demo::set<int, std::greater<int>> s(comp);
        s.insert(1);
        s.insert(2);
        s.insert(3);

        std::vector<int> expected = {3, 2, 1};
        CHECK(set_equals_vector(s, expected));
    }

    TEST_CASE("Initializer list constructor")
    {
        demo::set<int> s = {3, 1, 2};
        CHECK(s.size() == 3);
        assert_set_sorted(s);
    }

    TEST_CASE("Iterator range constructor")
    {
        std::vector<int> vec = {3, 1, 2};
        demo::set<int> s(vec.begin(), vec.end());
        CHECK(s.size() == 3);
        assert_set_sorted(s);
    }

    TEST_CASE("Copy constructor")
    {
        demo::set<int> s1 = {1, 2, 3};
        demo::set<int> s2(s1);

        CHECK(s2.size() == 3);
        assert_set_sorted(s2);
        CHECK(sets_equal(s1, s2));
    }

    TEST_CASE("Move constructor")
    {
        demo::set<int> s1 = {1, 2, 3};
        auto size_before = s1.size();

        demo::set<int> s2(std::move(s1));

        CHECK(s2.size() == size_before);
        assert_set_sorted(s2);
        CHECK(s1.empty());
        CHECK(s1.size() == 0);
    }
}

TEST_SUITE("Set Assignment Operators")
{
    TEST_CASE("Copy assignment operator")
    {
        demo::set<int> s1 = {1, 2, 3};
        demo::set<int> s2;

        s2 = s1;

        CHECK(s2.size() == 3);
        CHECK(sets_equal(s1, s2));
    }

    TEST_CASE("Move assignment operator")
    {
        demo::set<int> s1 = {1, 2, 3};
        auto           size_before = s1.size();
        demo::set<int> s2;

        s2 = std::move(s1);

        CHECK(s2.size() == size_before);
        CHECK(s1.empty());
    }

    TEST_CASE("Initializer list assignment")
    {
        demo::set<int> s;
        s = {3, 1, 2};

        CHECK(s.size() == 3);
        assert_set_sorted(s);
    }
}

TEST_SUITE("Set Iterators")
{
    TEST_CASE("begin() and end() iterators")
    {
        demo::set<int> s = {1, 2, 3};

        auto it = s.begin();
        CHECK(*it == 1);

        ++it;
        CHECK(*it == 2);

        ++it;
        CHECK(*it == 3);

        ++it;
        CHECK(it == s.end());
    }

    TEST_CASE("const iterators cbegin() and cend()")
    {
        const demo::set<int> s = {1, 2, 3};

        auto it = s.cbegin();
        CHECK(*it == 1);
        ++it;
        CHECK(*it == 2);
        ++it;
        CHECK(*it == 3);
        ++it;
        CHECK(it == s.cend());
    }

    TEST_CASE("range-based for loop")
    {
        demo::set<int> s = {1, 2, 3};
        int            expected = 1;

        for (const auto& val : s)
        {
            CHECK(val == expected);
            ++expected;
        }
    }

    TEST_CASE("iterator decrement operator--")
    {
        demo::set<int> s = {1, 2, 3};
        auto           it = s.end();

        --it;
        CHECK(*it == 3);
        it--;
        CHECK(*it == 2);
        --it;
        CHECK(*it == 1);
    }

    TEST_CASE("reverse iterators rbegin() and rend()")
    {
        demo::set<int> s = {1, 2, 3};
        int            expected = 3;

        for (auto it = s.rbegin(); it != s.rend(); ++it)
        {
            CHECK(*it == expected);
            --expected;
        }
    }

    TEST_CASE("empty set iterators")
    {
        demo::set<int> s;

        CHECK(s.begin() == s.end());
        CHECK(s.cbegin() == s.cend());
        CHECK(s.rbegin() == s.rend());
        CHECK(s.crbegin() == s.crend());
    }
}

TEST_SUITE("Set Capacity")
{
    TEST_CASE("empty() returns true for empty set")
    {
        demo::set<int> s;
        CHECK(s.empty());
    }

    TEST_CASE("empty() returns false for non-empty set")
    {
        demo::set<int> s = {1};
        CHECK_FALSE(s.empty());
    }

    TEST_CASE("size() returns correct number of elements")
    {
        demo::set<int> s;

        CHECK(s.size() == 0);

        s.insert(1);
        CHECK(s.size() == 1);

        s.insert(2);
        CHECK(s.size() == 2);

        s.erase(1);
        CHECK(s.size() == 1);
    }
}

TEST_SUITE("Set Modifiers - Insert")
{
    TEST_CASE("insert() with const value")
    {
        demo::set<int> s;
        int            val = 1;

        s.insert(val);

        CHECK(s.size() == 1);
        CHECK(s.count(1) == 1);
    }

    TEST_CASE("insert() returns pair with iterator and bool")
    {
        demo::set<int> s;

        auto result = s.insert(1);
        CHECK(result.second == true);
        CHECK(*result.first == 1);

        result = s.insert(1);
        CHECK(result.second == false);
        CHECK(*result.first == 1);
    }

    TEST_CASE("insert() with iterator range")
    {
        std::vector<int> vec = {3, 1, 2};
        demo::set<int>   s;

        s.insert(vec.begin(), vec.end());

        CHECK(s.size() == 3);
        assert_set_sorted(s);
    }

    TEST_CASE("insert() with initializer list")
    {
        demo::set<int> s;

        s.insert({3, 1, 2});

        CHECK(s.size() == 3);
        assert_set_sorted(s);
    }

    TEST_CASE("insert duplicate key")
    {
        demo::set<int> s;

        s.insert(1);
        auto result = s.insert(1);

        CHECK(result.second == false);
        CHECK(s.size() == 1);
        CHECK(s.count(1) == 1);
    }

    TEST_CASE("emplace() inserts element")
    {
        demo::set<std::string> s;

        auto result = s.emplace("hello");
        CHECK(result.second == true);
        CHECK(s.count("hello") == 1);
    }

    TEST_CASE("emplace_hint() inserts element")
    {
        demo::set<int> s = {1, 3};

        auto hint = s.find(1);
        auto result = s.emplace_hint(hint, 2);
        CHECK(*result == 2);
        CHECK(s.size() == 3);
        assert_set_sorted(s);
    }
}

TEST_SUITE("Set Modifiers - Erase")
{
    TEST_CASE("erase() by iterator")
    {
        demo::set<int> s = {1, 2, 3};

        auto it = s.find(2);
        s.erase(it);

        CHECK(s.size() == 2);
        CHECK(s.count(2) == 0);
        assert_set_sorted(s);
    }

    TEST_CASE("erase() by key")
    {
        demo::set<int> s = {1, 2, 3};

        auto count = s.erase(2);

        CHECK(count == 1);
        CHECK(s.size() == 2);
        CHECK(s.count(2) == 0);
    }

    TEST_CASE("erase() returns iterator to next element")
    {
        demo::set<int> s = {1, 2, 3};

        auto it      = s.find(2);
        auto next_it = s.erase(it);

        CHECK(*next_it == 3);
    }

    TEST_CASE("erase() range")
    {
        demo::set<int> s = {1, 2, 3, 4, 5};

        auto first = s.find(2);
        auto last  = s.find(4);
        s.erase(first, last);

        CHECK(s.size() == 3);
        CHECK(s.count(2) == 0);
        CHECK(s.count(3) == 0);
        CHECK(s.count(1) == 1);
        CHECK(s.count(4) == 1);
        CHECK(s.count(5) == 1);
    }

    TEST_CASE("clear() removes all elements")
    {
        demo::set<int> s = {1, 2, 3};

        s.clear();

        CHECK(s.empty());
        CHECK(s.size() == 0);
    }

    TEST_CASE("erase non-existing key")
    {
        demo::set<int> s = {1};

        auto count = s.erase(2);

        CHECK(count == 0);
        CHECK(s.size() == 1);
    }

    TEST_CASE("erase root node")
    {
        demo::set<int> s = {2, 1, 3};

        s.erase(2);

        CHECK(s.size() == 2);
        CHECK(s.count(2) == 0);
        CHECK(s.count(1) == 1);
        CHECK(s.count(3) == 1);
        assert_set_sorted(s);
    }

    TEST_CASE("erase single element")
    {
        demo::set<int> s = {1};

        s.erase(1);

        CHECK(s.empty());
        CHECK(s.size() == 0);
    }
}

TEST_SUITE("Set Modifiers - Other")
{
    TEST_CASE("swap() exchanges contents")
    {
        demo::set<int> s1 = {1, 2};
        demo::set<int> s2 = {3, 4, 5};

        s1.swap(s2);

        CHECK(s1.size() == 3);
        CHECK(s1.count(3) == 1);
        CHECK(s1.count(4) == 1);
        CHECK(s1.count(5) == 1);

        CHECK(s2.size() == 2);
        CHECK(s2.count(1) == 1);
        CHECK(s2.count(2) == 1);
    }
}

TEST_SUITE("Set Lookup")
{
    TEST_CASE("find() returns iterator to existing element")
    {
        demo::set<int> s = {1, 2, 3};

        auto it = s.find(2);

        CHECK(it != s.end());
        CHECK(*it == 2);
    }

    TEST_CASE("find() returns end for non-existing element")
    {
        demo::set<int> s = {1, 2};

        auto it = s.find(3);

        CHECK(it == s.end());
    }

    TEST_CASE("count() returns 1 for existing key")
    {
        demo::set<int> s = {1, 2};

        CHECK(s.count(1) == 1);
        CHECK(s.count(2) == 1);
    }

    TEST_CASE("count() returns 0 for non-existing key")
    {
        demo::set<int> s = {1, 2};

        CHECK(s.count(3) == 0);
    }

    TEST_CASE("lower_bound() returns first not less than key")
    {
        demo::set<int> s = {1, 3, 5};

        auto it = s.lower_bound(2);
        CHECK(*it == 3);

        it = s.lower_bound(3);
        CHECK(*it == 3);

        it = s.lower_bound(6);
        CHECK(it == s.end());
    }

    TEST_CASE("upper_bound() returns first greater than key")
    {
        demo::set<int> s = {1, 3, 5};

        auto it = s.upper_bound(2);
        CHECK(*it == 3);

        it = s.upper_bound(3);
        CHECK(*it == 5);

        it = s.upper_bound(5);
        CHECK(it == s.end());
    }

    TEST_CASE("equal_range() returns pair of iterators")
    {
        demo::set<int> s = {1, 3, 5};

        auto range = s.equal_range(3);
        CHECK(*range.first == 3);
        CHECK(*range.second == 5);
    }
}

TEST_SUITE("Set Operations")
{
    TEST_CASE("merge() merges two sets")
    {
        demo::set<int> s1 = {1, 3};
        demo::set<int> s2 = {2, 4};

        s1.merge(s2);

        CHECK(s1.size() == 4);
        CHECK(s1.count(1) == 1);
        CHECK(s1.count(2) == 1);
        CHECK(s1.count(3) == 1);
        CHECK(s1.count(4) == 1);
        assert_set_sorted(s1);
    }

    TEST_CASE("merge() with overlapping keys")
    {
        demo::set<int> s1 = {1, 2};
        demo::set<int> s2 = {2, 3};

        s1.merge(s2);

        CHECK(s1.size() == 3);
        CHECK(s1.count(2) == 1);
        CHECK(s1.count(3) == 1);
        assert_set_sorted(s1);
    }

    TEST_CASE("key_comp() returns comparator")
    {
        demo::set<int> s;
        auto           comp = s.key_comp();

        CHECK(comp(1, 2));
        CHECK_FALSE(comp(2, 1));
    }
}

TEST_SUITE("Set Comparison Operators")
{
    TEST_CASE("operator== compares contents")
    {
        demo::set<int> s1 = {1, 2};
        demo::set<int> s2 = {1, 2};
        demo::set<int> s3 = {1, 3};

        CHECK(s1 == s2);
        CHECK_FALSE(s1 == s3);
    }

    TEST_CASE("operator!= compares contents")
    {
        demo::set<int> s1 = {1, 2};
        demo::set<int> s2 = {1, 2};
        demo::set<int> s3 = {1, 3};

        CHECK_FALSE(s1 != s2);
        CHECK(s1 != s3);
    }
}

TEST_SUITE("Set Boundary Cases")
{
    TEST_CASE("single element set operations")
    {
        demo::set<int> s = {1};

        CHECK(s.size() == 1);
        CHECK(!s.empty());
        CHECK(s.find(1) != s.end());
        CHECK(s.find(2) == s.end());

        s.erase(1);
        CHECK(s.empty());
    }

#ifndef NDEBUG
    TEST_CASE("validate_tree() in debug mode")
    {
        demo::set<int> s;
        s.insert(1);
        s.insert(2);
        s.insert(3);

        CHECK(s.validate_tree());
    }
#endif

    TEST_CASE("insert and erase alternating")
    {
        demo::set<int> s;

        for (int i = 0; i < 10; ++i)
        {
            s.insert(i);
        }

        for (int i = 0; i < 10; i += 2)
        {
            s.erase(i);
        }

        CHECK(s.size() == 5);
        for (int i = 1; i < 10; i += 2)
        {
            CHECK(s.count(i) == 1);
        }
    }
}

TEST_SUITE("Set String Type")
{
    TEST_CASE("string key set operations")
    {
        demo::set<std::string> s;

        s.insert("apple");
        s.insert("banana");
        s.insert("cherry");

        CHECK(s.size() == 3);
        CHECK(s.count("apple") == 1);
        CHECK(s.count("banana") == 1);
        CHECK(s.count("cherry") == 1);

        s.erase("banana");
        CHECK(s.size() == 2);
        CHECK(s.count("banana") == 0);
    }
}

TEST_SUITE("Set Stress Tests")
{
    TEST_CASE("insert and iterate 10000 elements")
    {
        demo::set<int> s;

        for (int i = 0; i < 10000; ++i)
        {
            s.insert(i);
        }

        CHECK(s.size() == 10000);

        int expected = 0;
        for (const auto& val : s)
        {
            CHECK(val == expected);
            ++expected;
        }
    }

    TEST_CASE("insert in random order")
    {
        demo::set<int> s;
        std::vector<int> keys;

        for (int i = 0; i < 5000; ++i)
        {
            keys.push_back(i);
        }

        std::random_device rd;
        std::mt19937       g(rd());
        std::shuffle(keys.begin(), keys.end(), g);

        for (int key : keys)
        {
            s.insert(key);
        }

        CHECK(s.size() == 5000);
        assert_set_sorted(s);
    }

    TEST_CASE("sequential insert and erase")
    {
        demo::set<int> s;

        for (int i = 0; i < 1000; ++i)
        {
            s.insert(i);
        }

        for (int i = 0; i < 1000; ++i)
        {
            s.erase(i);
        }

        CHECK(s.empty());
        CHECK(s.size() == 0);
    }
}