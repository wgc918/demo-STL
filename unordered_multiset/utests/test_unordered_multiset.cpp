#define DOCTEST_CONFIG_NO_MULTITHREADING

#include <algorithm>
#include <chrono>
#include <functional>
#include <iostream>
#include <random>
#include <string>
#include <vector>

#include "../unordered_multiset.h"
#include "doctest.h"

template <typename K>
std::vector<K> unordered_multiset_to_vector(const demo::unordered_multiset<K>& st)
{
    std::vector<K> result;
    for (const auto& elem : st)
    {
        result.push_back(elem);
    }
    return result;
}

template <typename K>
bool unordered_multisets_equal(const demo::unordered_multiset<K>& a,
                                const demo::unordered_multiset<K>& b)
{
    if (a.size() != b.size())
        return false;

    for (const auto& elem : a)
    {
        if (a.count(elem) != b.count(elem))
            return false;
    }
    return true;
}

template <typename K>
size_t count_elements_in_buckets(const demo::unordered_multiset<K>& st)
{
    size_t count = 0;
    for (size_t i = 0; i < st.bucket_count(); ++i)
    {
        count += st.bucket_size(i);
    }
    return count;
}

#define UMS_INT demo::unordered_multiset<int>
#define UMS_STR demo::unordered_multiset<std::string>

TEST_SUITE("Unordered_MultiSet Node Structure")
{
    TEST_CASE("Node default constructor initializes correctly")
    {
        demo::unordered_multiset<int> s{};
        CHECK(s.empty());
        CHECK(s.size() == 0);
    }

    TEST_CASE("Node stores keys correctly")
    {
        demo::unordered_multiset<std::string> s{};
        s.insert("one");
        CHECK(s.size() == 1);
        CHECK(s.find("one") != s.end());
    }

    TEST_CASE("Node maintains hash chain relationships")
    {
        demo::unordered_multiset<int> s{};
        s.insert(3);
        s.insert(1);
        s.insert(5);
        s.insert(2);
        s.insert(4);

        CHECK(s.size() == 5);
        CHECK(s.find(1) != s.end());
        CHECK(s.find(2) != s.end());
        CHECK(s.find(3) != s.end());
        CHECK(s.find(4) != s.end());
        CHECK(s.find(5) != s.end());
    }

    TEST_CASE("Node deletion removes all elements")
    {
        demo::unordered_multiset<int> s{};
        s.insert(1);
        s.insert(2);
        s.insert(3);
        s.erase(2);
        CHECK(s.size() == 2);
        CHECK(s.find(2) == s.end());
    }
}

TEST_SUITE("Unordered_MultiSet Constructors")
{
    TEST_CASE("Default constructor creates empty multiset")
    {
        demo::unordered_multiset<int> s{};
        CHECK(s.empty());
        CHECK(s.size() == 0);
        CHECK(s.begin() == s.end());
    }

    TEST_CASE("Constructor with bucket count")
    {
        demo::unordered_multiset<int> s(32);
        CHECK(s.bucket_count() == 32);
        CHECK(s.empty());

        demo::unordered_multiset<int> n(31);
        CHECK(n.bucket_count() >= 31);
        CHECK(n.empty());
    }

    TEST_CASE("Initializer list constructor")
    {
        demo::unordered_multiset<std::string> s = {"one", "two", "three"};
        CHECK(s.size() == 3);
        CHECK(s.find("one") != s.end());
        CHECK(s.find("two") != s.end());
        CHECK(s.find("three") != s.end());
    }

    TEST_CASE("Initializer list with duplicates")
    {
        demo::unordered_multiset<int> s = {1, 1, 2, 2, 2, 3};
        CHECK(s.size() == 6);
        CHECK(s.count(1) == 2);
        CHECK(s.count(2) == 3);
        CHECK(s.count(3) == 1);
    }

    TEST_CASE("Iterator range constructor")
    {
        std::vector<int> vec = {1, 2, 3};
        demo::unordered_multiset<int> s(vec.begin(), vec.end());
        CHECK(s.size() == 3);
        CHECK(s.find(1) != s.end());
        CHECK(s.find(2) != s.end());
        CHECK(s.find(3) != s.end());
    }

    TEST_CASE("Iterator range constructor with duplicates")
    {
        std::vector<int> vec = {1, 1, 2, 2, 2};
        demo::unordered_multiset<int> s(vec.begin(), vec.end());
        CHECK(s.size() == 5);
        CHECK(s.count(1) == 2);
        CHECK(s.count(2) == 3);
    }

    TEST_CASE("Copy constructor")
    {
        demo::unordered_multiset<std::string> s1 = {"one", "two"};
        demo::unordered_multiset<std::string> s2(s1);

        CHECK(s2.size() == 2);
        CHECK(s2.find("one") != s2.end());
        CHECK(s2.find("two") != s2.end());
        CHECK(unordered_multisets_equal(s1, s2));
        CHECK(s1.size() == 2);
    }

    TEST_CASE("Copy constructor with duplicates")
    {
        demo::unordered_multiset<int> s1 = {1, 1, 2};
        demo::unordered_multiset<int> s2(s1);

        CHECK(s2.size() == 3);
        CHECK(s2.count(1) == 2);
        CHECK(s2.count(2) == 1);
    }

    TEST_CASE("Move constructor")
    {
        demo::unordered_multiset<std::string> s1 = {"one", "two"};
        auto size_before = s1.size();

        demo::unordered_multiset<std::string> s2(std::move(s1));

        CHECK(s2.size() == size_before);
        CHECK(s2.find("one") != s2.end());
        CHECK(s2.find("two") != s2.end());
        CHECK(s1.empty());
        CHECK(s1.size() == 0);
    }
}

TEST_SUITE("Unordered_MultiSet Assignment Operators")
{
    TEST_CASE("Copy assignment operator")
    {
        demo::unordered_multiset<std::string> s1 = {"one", "two"};
        demo::unordered_multiset<std::string> s2{};

        s2 = s1;

        CHECK(s2.size() == 2);
        CHECK(s2.find("one") != s2.end());
        CHECK(s2.find("two") != s2.end());
        CHECK(unordered_multisets_equal(s1, s2));
    }

    TEST_CASE("Move assignment operator")
    {
        demo::unordered_multiset<std::string> s1 = {"one", "two"};
        auto size_before = s1.size();
        demo::unordered_multiset<std::string> s2{};

        s2 = std::move(s1);

        CHECK(s2.size() == size_before);
        CHECK(s2.find("one") != s2.end());
        CHECK(s2.find("two") != s2.end());
        CHECK(s1.empty());
    }

    TEST_CASE("Initializer list assignment")
    {
        demo::unordered_multiset<std::string> s{};
        s = {"one", "two", "three"};

        CHECK(s.size() == 3);
        CHECK(s.find("one") != s.end());
        CHECK(s.find("two") != s.end());
        CHECK(s.find("three") != s.end());
    }

    TEST_CASE("Self assignment")
    {
        demo::unordered_multiset<std::string> s = {"one", "two"};
        s = s;
        CHECK(s.size() == 2);
        CHECK(s.find("one") != s.end());
    }
}

TEST_SUITE("Unordered_MultiSet Iterators")
{
    TEST_CASE("begin() and end() iterators")
    {
        demo::unordered_multiset<int> s = {1, 2, 3};

        auto it = s.begin();
        int count = 0;
        for (; it != s.end(); ++it)
        {
            bool is_valid = (*it == 1 || *it == 2 || *it == 3);
            CHECK(is_valid);
            ++count;
        }
        CHECK(count == 3);
    }

    TEST_CASE("const iterators cbegin() and cend()")
    {
        const demo::unordered_multiset<int> s = {1, 2, 3};

        auto it = s.cbegin();
        int count = 0;
        for (; it != s.cend(); ++it)
        {
            bool is_valid = (*it == 1 || *it == 2 || *it == 3);
            CHECK(is_valid);
            ++count;
        }
        CHECK(count == 3);
    }

    TEST_CASE("range-based for loop")
    {
        demo::unordered_multiset<int> s = {1, 2, 3};
        int count = 0;

        for (const auto& val : s)
        {
            bool is_valid = (val == 1 || val == 2 || val == 3);
            CHECK(is_valid);
            ++count;
        }
        CHECK(count == 3);
    }

    TEST_CASE("range-based for loop with duplicates")
    {
        demo::unordered_multiset<int> s = {1, 1, 2, 2, 2};
        int count = 0;

        for (const auto& val : s)
        {
            ++count;
        }
        CHECK(count == 5);
    }

    TEST_CASE("iterator increment operator++")
    {
        demo::unordered_multiset<int> s = {1, 2, 3};
        auto it = s.begin();

        it++;
        it++;
        it++;
        CHECK(it == s.end());
    }

    TEST_CASE("iterator post-increment")
    {
        demo::unordered_multiset<int> s = {1, 2, 3};
        auto it = s.begin();

        auto it_copy = it++;
        CHECK(*it_copy == 1);
    }

    TEST_CASE("iterator dereference operator")
    {
        demo::unordered_multiset<int> s = {1};
        auto it = s.begin();

        auto& val = *it;
        CHECK(val == 1);
    }

    TEST_CASE("iterator arrow operator")
    {
        demo::unordered_multiset<std::string> s = {"test"};
        auto it = s.begin();

        CHECK(*it == "test");
    }

    TEST_CASE("empty multiset iterators")
    {
        demo::unordered_multiset<int> s{};

        CHECK(s.begin() == s.end());
        CHECK(s.cbegin() == s.cend());
    }

    TEST_CASE("iterator conversion to const_iterator")
    {
        demo::unordered_multiset<int> s = {1};
        demo::unordered_multiset<int>::iterator it = s.begin();
        demo::unordered_multiset<int>::const_iterator const_it = it;

        CHECK(*const_it == 1);
    }
}

TEST_SUITE("Unordered_MultiSet Bucket Iterators")
{
    TEST_CASE("local_iterator begin and end")
    {
        demo::unordered_multiset<int> s{};
        s.insert(1);
        s.insert(2);

        size_t bucket_idx = s.bucket(1);
        auto local_it = s.begin(bucket_idx);
        auto local_end = s.end(bucket_idx);

        if (local_it == local_end)
        {
            CHECK(s.bucket_size(bucket_idx) == 0);
        }
        else
        {
            CHECK(s.bucket_size(bucket_idx) > 0);
        }
    }

    TEST_CASE("const_local_iterator begin and end")
    {
        const demo::unordered_multiset<int> s = {1, 2};

        size_t bucket_idx = s.bucket(1);
        auto local_it = s.cbegin(bucket_idx);
        auto local_end = s.cend(bucket_idx);

        if (local_it == local_end)
        {
            CHECK(s.bucket_size(bucket_idx) == 0);
        }
        else
        {
            CHECK(s.bucket_size(bucket_idx) > 0);
        }
    }

    TEST_CASE("local_iterator increment")
    {
        demo::unordered_multiset<int> s(64);
        for (int i = 0; i < 10; ++i)
        {
            s.insert(i);
        }

        size_t bucket_idx = s.bucket(0);
        int count = 0;
        for (auto it = s.begin(bucket_idx); it != s.end(bucket_idx); ++it)
        {
            ++count;
        }
        CHECK(count == s.bucket_size(bucket_idx));
    }
}

TEST_SUITE("Unordered_MultiSet Capacity")
{
    TEST_CASE("empty() returns true for empty multiset")
    {
        demo::unordered_multiset<int> s{};
        CHECK(s.empty());
    }

    TEST_CASE("empty() returns false for non-empty multiset")
    {
        demo::unordered_multiset<int> s = {1};
        CHECK_FALSE(s.empty());
    }

    TEST_CASE("size() returns correct number of elements")
    {
        demo::unordered_multiset<int> s{};

        CHECK(s.size() == 0);

        s.insert(1);
        CHECK(s.size() == 1);

        s.insert(2);
        CHECK(s.size() == 2);

        s.erase(1);
        CHECK(s.size() == 1);
    }

    TEST_CASE("size() with duplicates")
    {
        demo::unordered_multiset<int> s{};

        s.insert(1);
        s.insert(1);
        s.insert(1);
        CHECK(s.size() == 3);

        s.erase(1);
        CHECK(s.size() == 0);
    }

    TEST_CASE("max_size() returns positive value")
    {
        demo::unordered_multiset<int> s{};
        CHECK(s.max_size() > 0);
    }

    TEST_CASE("bucket_count() returns valid value")
    {
        demo::unordered_multiset<int> s{};
        CHECK(s.bucket_count() >= 16);
    }

    TEST_CASE("max_bucket_count() returns positive value")
    {
        demo::unordered_multiset<int> s{};
        CHECK(s.max_bucket_count() > 0);
    }
}

TEST_SUITE("Unordered_MultiSet Modifiers - Insert")
{
    TEST_CASE("insert() with const value")
    {
        demo::unordered_multiset<std::string> s{};
        std::string val = "one";

        auto it = s.insert(val);

        CHECK(s.size() == 1);
        CHECK(*it == "one");
        CHECK(s.find("one") != s.end());
    }

    TEST_CASE("insert() with rvalue")
    {
        demo::unordered_multiset<std::string> s{};

        auto it = s.insert("one");

        CHECK(s.size() == 1);
        CHECK(*it == "one");
        CHECK(s.find("one") != s.end());
    }

    TEST_CASE("insert() returns iterator to inserted element")
    {
        demo::unordered_multiset<std::string> s{};

        auto it = s.insert("one");
        CHECK(*it == "one");

        it = s.insert("one");
        CHECK(*it == "one");
        CHECK(s.size() == 2);
    }

    TEST_CASE("insert() allows duplicates")
    {
        demo::unordered_multiset<int> s{};

        s.insert(1);
        s.insert(1);
        s.insert(1);

        CHECK(s.size() == 3);
        CHECK(s.count(1) == 3);
    }

    TEST_CASE("insert() with hint position")
    {
        demo::unordered_multiset<int> s = {1, 3};
        CHECK(s.size() == 2);
        auto hint = s.find(1);
        CHECK(hint != s.end());
        auto result = s.insert(hint, 2);
        CHECK(*result == 2);
        CHECK(s.size() == 3);
        CHECK(s.find(2) != s.end());
    }

    TEST_CASE("insert() with hint allows duplicates")
    {
        demo::unordered_multiset<int> s = {1, 2};
        auto hint = s.find(2);
        auto result = s.insert(hint, 2);
        CHECK(*result == 2);
        CHECK(s.size() == 3);
        CHECK(s.count(2) == 2);
    }

    TEST_CASE("insert() with iterator range")
    {
        std::vector<int> vec = {1, 2, 3};
        demo::unordered_multiset<int> s{};

        s.insert(vec.begin(), vec.end());

        CHECK(s.size() == 3);
        CHECK(s.find(1) != s.end());
        CHECK(s.find(2) != s.end());
        CHECK(s.find(3) != s.end());
    }

    TEST_CASE("insert() with iterator range including duplicates")
    {
        std::vector<int> vec = {1, 1, 2, 2, 2};
        demo::unordered_multiset<int> s{};

        s.insert(vec.begin(), vec.end());

        CHECK(s.size() == 5);
        CHECK(s.count(1) == 2);
        CHECK(s.count(2) == 3);
    }

    TEST_CASE("insert() with initializer list")
    {
        demo::unordered_multiset<int> s{};

        s.insert({1, 2, 3});

        CHECK(s.size() == 3);
        CHECK(s.find(1) != s.end());
        CHECK(s.find(2) != s.end());
        CHECK(s.find(3) != s.end());
    }

    TEST_CASE("emplace() inserts new element")
    {
        demo::unordered_multiset<std::string> s{};

        auto it = s.emplace("one");
        CHECK(*it == "one");
        CHECK(s.find("one") != s.end());
    }

    TEST_CASE("emplace() allows duplicates")
    {
        demo::unordered_multiset<std::string> s = {"original"};

        auto it = s.emplace("original");
        CHECK(*it == "original");
        CHECK(s.count("original") == 2);
        CHECK(s.size() == 2);
    }

    TEST_CASE("insert duplicate key")
    {
        demo::unordered_multiset<int> s{};

        s.insert(1);
        auto it = s.insert(1);

        CHECK(*it == 1);
        CHECK(s.size() == 2);
        CHECK(s.count(1) == 2);
    }

    TEST_CASE("insert many duplicates")
    {
        demo::unordered_multiset<int> s{};

        for (int i = 0; i < 100; ++i)
        {
            s.insert(42);
        }

        CHECK(s.size() == 100);
        CHECK(s.count(42) == 100);
    }
}

TEST_SUITE("Unordered_MultiSet Modifiers - Erase")
{
    TEST_CASE("erase() by iterator")
    {
        demo::unordered_multiset<int> s = {1, 2, 3};

        auto it = s.find(2);
        s.erase(it);

        CHECK(s.size() == 2);
        CHECK(s.find(2) == s.end());
    }

    TEST_CASE("erase() by key removes one element")
    {
        demo::unordered_multiset<int> s = {1, 2, 3};

        s.erase(2);

        CHECK(s.size() == 2);
        CHECK(s.find(2) == s.end());
    }

    TEST_CASE("erase() by key with duplicates removes all")
    {
        demo::unordered_multiset<int> s = {1, 1, 1, 2, 2};

        auto erased = s.erase(1);
        CHECK(erased == 3);
        CHECK(s.size() == 2);
        CHECK(s.count(1) == 0);
        CHECK(s.count(2) == 2);
    }

    TEST_CASE("erase() returns count of removed elements")
    {
        demo::unordered_multiset<int> s = {1, 1, 1, 2};

        CHECK(s.erase(1) == 3);
        CHECK(s.erase(1) == 0);
        CHECK(s.erase(2) == 1);
        CHECK(s.erase(3) == 0);
    }

    TEST_CASE("erase() returns iterator to next element")
    {
        demo::unordered_multiset<int> s = {1, 2, 3};

        auto it = s.find(2);
        auto next_it = s.erase(it);

        CHECK(s.size() == 2);
        CHECK(s.find(2) == s.end());
    }

    TEST_CASE("erase() range")
    {
        demo::unordered_multiset<int> s;
        for (int i = 0; i < 5; ++i)
        {
            s.insert(i);
        }
        CHECK(s.size() == 5);

        auto it = s.begin();
        auto first = it;
        ++it;
        ++it;
        auto last = it;

        s.erase(first, last);

        CHECK(s.size() == 3);
        int count = 0;
        for (const auto& val : s)
        {
            (void)val;
            ++count;
        }
        CHECK(count == 3);
    }

    TEST_CASE("erase() range with duplicates")
    {
        demo::unordered_multiset<int> s = {1, 1, 2, 2, 2, 3};
        CHECK(s.size() == 6);

        auto erased = s.erase(2);
        CHECK(erased == 3);
        CHECK(s.size() == 3);
        CHECK(s.count(1) == 2);
        CHECK(s.count(3) == 1);
    }

    TEST_CASE("clear() removes all elements")
    {
        demo::unordered_multiset<int> s = {1, 2, 3};

        s.clear();

        CHECK(s.empty());
        CHECK(s.size() == 0);
    }

    TEST_CASE("clear() on empty multiset")
    {
        demo::unordered_multiset<int> s{};

        CHECK_NOTHROW(s.clear());
        CHECK(s.empty());
    }

    TEST_CASE("erase non-existing key")
    {
        demo::unordered_multiset<int> s = {1};

        auto count = s.erase(2);

        CHECK(count == 0);
        CHECK(s.size() == 1);
    }

    TEST_CASE("erase single element")
    {
        demo::unordered_multiset<int> s = {1};

        s.erase(1);

        CHECK(s.empty());
        CHECK(s.size() == 0);
    }
}

TEST_SUITE("Unordered_MultiSet Modifiers - Other")
{
    TEST_CASE("swap() exchanges contents")
    {
        demo::unordered_multiset<int> s1 = {1, 2};
        demo::unordered_multiset<int> s2 = {3, 4, 5};

        s1.swap(s2);

        CHECK(s1.size() == 3);
        CHECK(s1.find(3) != s1.end());
        CHECK(s1.find(4) != s1.end());
        CHECK(s1.find(5) != s1.end());

        CHECK(s2.size() == 2);
        CHECK(s2.find(1) != s2.end());
        CHECK(s2.find(2) != s2.end());
    }

    TEST_CASE("swap() with duplicates")
    {
        demo::unordered_multiset<int> s1 = {1, 1, 2};
        demo::unordered_multiset<int> s2 = {3, 3, 3, 4};

        s1.swap(s2);

        CHECK(s1.size() == 4);
        CHECK(s1.count(3) == 3);
        CHECK(s1.count(4) == 1);

        CHECK(s2.size() == 3);
        CHECK(s2.count(1) == 2);
        CHECK(s2.count(2) == 1);
    }

    TEST_CASE("swap() with empty multiset")
    {
        demo::unordered_multiset<int> s1 = {1};
        demo::unordered_multiset<int> s2;

        s1.swap(s2);

        CHECK(s1.empty());
        CHECK(s2.size() == 1);
        CHECK(s2.find(1) != s2.end());
    }
}

TEST_SUITE("Unordered_MultiSet Lookup")
{
    TEST_CASE("find() returns iterator to existing element")
    {
        demo::unordered_multiset<int> s = {1, 2, 3};

        auto it = s.find(2);

        CHECK(it != s.end());
        CHECK(*it == 2);
    }

    TEST_CASE("find() returns end for non-existing element")
    {
        demo::unordered_multiset<int> s = {1, 2};

        auto it = s.find(3);

        CHECK(it == s.end());
    }

    TEST_CASE("find() with duplicates returns first match")
    {
        demo::unordered_multiset<int> s = {1, 1, 1};

        auto it = s.find(1);

        CHECK(it != s.end());
        CHECK(*it == 1);
    }

    TEST_CASE("const find() returns iterator to existing element")
    {
        const demo::unordered_multiset<int> s = {1, 2};

        auto it = s.find(1);

        CHECK(it != s.end());
        CHECK(*it == 1);
    }

    TEST_CASE("count() returns 1 for single existing key")
    {
        demo::unordered_multiset<int> s = {1, 2};

        CHECK(s.count(1) == 1);
        CHECK(s.count(2) == 1);
    }

    TEST_CASE("count() returns 0 for non-existing key")
    {
        demo::unordered_multiset<int> s = {1, 2};

        CHECK(s.count(3) == 0);
    }

    TEST_CASE("count() returns correct count for duplicate keys")
    {
        demo::unordered_multiset<int> s = {1, 1, 1, 2, 2};

        CHECK(s.count(1) == 3);
        CHECK(s.count(2) == 2);
        CHECK(s.count(3) == 0);
    }

    TEST_CASE("equal_range() returns pair of iterators")
    {
        demo::unordered_multiset<int> s = {1, 2, 3};

        auto range = s.equal_range(2);
        CHECK(*range.first == 2);
    }

    TEST_CASE("equal_range() for non-existing key")
    {
        demo::unordered_multiset<int> s = {1, 2};

        auto range = s.equal_range(3);
        CHECK(range.first == s.end());
        CHECK(range.second == s.end());
    }

    TEST_CASE("equal_range() with duplicates returns full range")
    {
        demo::unordered_multiset<int> s = {1, 1, 1, 2};

        auto range = s.equal_range(1);
        int count = 0;
        for (auto it = range.first; it != range.second; ++it)
        {
            CHECK(*it == 1);
            ++count;
        }
        CHECK(count == 3);
    }

    TEST_CASE("equal_range() const with duplicates")
    {
        const demo::unordered_multiset<int> s = {1, 1, 2};

        auto range = s.equal_range(1);
        int count = 0;
        for (auto it = range.first; it != range.second; ++it)
        {
            CHECK(*it == 1);
            ++count;
        }
        CHECK(count == 2);
    }
}

TEST_SUITE("Unordered_MultiSet Bucket Interface")
{
    TEST_CASE("bucket() returns valid bucket index")
    {
        demo::unordered_multiset<int> s{};
        s.insert(1);
        s.insert(2);

        size_t bucket1 = s.bucket(1);
        size_t bucket2 = s.bucket(2);

        CHECK(bucket1 < s.bucket_count());
        CHECK(bucket2 < s.bucket_count());
    }

    TEST_CASE("bucket_size() returns correct count")
    {
        demo::unordered_multiset<int> s(1);
        s.max_load_factor(10.0f);

        s.insert(1);
        s.insert(2);
        s.insert(3);

        size_t total_count = count_elements_in_buckets(s);
        CHECK(total_count == s.size());
    }

    TEST_CASE("bucket_size() for empty bucket")
    {
        demo::unordered_multiset<int> s{};
        CHECK(s.bucket_size(0) == 0);
    }

    TEST_CASE("bucket_size() with duplicates")
    {
        demo::unordered_multiset<int> s(1);
        s.max_load_factor(10.0f);

        s.insert(1);
        s.insert(1);
        s.insert(1);

        size_t total_count = count_elements_in_buckets(s);
        CHECK(total_count == 3);
    }
}

TEST_SUITE("Unordered_MultiSet Hash Policy")
{
    TEST_CASE("load_factor() returns correct value")
    {
        demo::unordered_multiset<int> s{};
        CHECK(s.load_factor() >= 0.0f);

        s.insert(1);
        CHECK(s.load_factor() > 0.0f);
    }

    TEST_CASE("max_load_factor() returns default value")
    {
        demo::unordered_multiset<int> s{};
        CHECK(s.max_load_factor() == 1.0f);
    }

    TEST_CASE("max_load_factor() can be set")
    {
        demo::unordered_multiset<int> s{};
        s.max_load_factor(0.5f);
        CHECK(s.max_load_factor() == 0.5f);
    }

    TEST_CASE("rehash() changes bucket count")
    {
        demo::unordered_multiset<int> s{};

        s.rehash(64);
        CHECK(s.bucket_count() >= 64);
    }

    TEST_CASE("rehash() with small count preserves elements")
    {
        demo::unordered_multiset<int> s = {1, 2, 3};

        s.rehash(1);
        CHECK(s.size() == 3);
        CHECK(s.find(1) != s.end());
        CHECK(s.find(2) != s.end());
        CHECK(s.find(3) != s.end());
    }

    TEST_CASE("rehash() preserves duplicates")
    {
        demo::unordered_multiset<int> s = {1, 1, 1, 2, 2};

        s.rehash(64);
        CHECK(s.size() == 5);
        CHECK(s.count(1) == 3);
        CHECK(s.count(2) == 2);
    }

    TEST_CASE("reserve() allocates space for elements")
    {
        demo::unordered_multiset<int> s{};
        s.reserve(100);

        CHECK(s.bucket_count() >= 100);
    }

    TEST_CASE("reserve() does not affect existing elements")
    {
        demo::unordered_multiset<int> s = {1, 2};
        auto size_before = s.size();

        s.reserve(100);

        CHECK(s.size() == size_before);
        CHECK(s.find(1) != s.end());
        CHECK(s.find(2) != s.end());
    }

    TEST_CASE("rehash occurs when exceeding default load factor")
    {
        demo::unordered_multiset<int> s(4);

        s.insert(3);
        s.insert(4);
        s.insert(5);
        s.insert(6);
        s.insert(7);
        s.insert(8);
        s.insert(9);

        CHECK(s.size() == 7);
        CHECK(s.bucket_count() > 4);
    }

    TEST_CASE("no rehash when load factor is sufficiently large")
    {
        demo::unordered_multiset<int> s(4);
        s.max_load_factor(7.0f);

        s.insert(3);
        s.insert(4);
        s.insert(5);
        s.insert(6);
        s.insert(7);
        s.insert(8);
        s.insert(9);

        CHECK(s.size() == 7);
        CHECK(s.bucket_count() == 4);
    }
}

TEST_SUITE("Unordered_MultiSet Observers")
{
    TEST_CASE("hash_function() returns hash object")
    {
        demo::unordered_multiset<int> s{};
        auto hash = s.hash_function();

        CHECK(hash(1) == std::hash<int>()(1));
    }

    TEST_CASE("key_eq() returns key equality object")
    {
        demo::unordered_multiset<int> s{};
        auto eq = s.key_eq();

        CHECK(eq(1, 1) == true);
        CHECK(eq(1, 2) == false);
    }

    TEST_CASE("get_allocator() returns allocator")
    {
        demo::unordered_multiset<int> s{};
        auto alloc = s.get_allocator();

        CHECK(s.size() == 0);
    }
}

TEST_SUITE("Unordered_MultiSet Operations")
{
    TEST_CASE("merge() merges two multisets")
    {
        demo::unordered_multiset<int> s1 = {1, 3};
        demo::unordered_multiset<int> s2 = {2, 4};

        s1.merge(s2);

        CHECK(s1.size() == 4);
        CHECK(s1.find(1) != s1.end());
        CHECK(s1.find(2) != s1.end());
        CHECK(s1.find(3) != s1.end());
        CHECK(s1.find(4) != s1.end());
    }

    TEST_CASE("merge() with overlapping keys merges all")
    {
        demo::unordered_multiset<int> s1 = {1, 2};
        demo::unordered_multiset<int> s2 = {2, 3};

        s1.merge(s2);

        CHECK(s1.size() == 4);
        CHECK(s1.count(1) == 1);
        CHECK(s1.count(2) == 2);
        CHECK(s1.count(3) == 1);
        CHECK(s2.empty());
    }

    TEST_CASE("merge() with duplicates")
    {
        demo::unordered_multiset<int> s1 = {1, 1};
        demo::unordered_multiset<int> s2 = {1, 1, 2};

        s1.merge(s2);

        CHECK(s1.size() == 5);
        CHECK(s1.count(1) == 4);
        CHECK(s1.count(2) == 1);
        CHECK(s2.empty());
    }

    TEST_CASE("merge() with empty source")
    {
        demo::unordered_multiset<int> s1 = {1};
        demo::unordered_multiset<int> s2;

        s1.merge(s2);

        CHECK(s1.size() == 1);
        CHECK(s1.find(1) != s1.end());
    }

    TEST_CASE("merge() rvalue version")
    {
        demo::unordered_multiset<int> s1 = {1};
        demo::unordered_multiset<int> s2 = {2};

        s1.merge(std::move(s2));

        CHECK(s1.size() == 2);
        CHECK(s1.find(1) != s1.end());
        CHECK(s1.find(2) != s1.end());
    }
}

TEST_SUITE("Unordered_MultiSet Comparison Operators")
{
    TEST_CASE("operator== compares contents")
    {
        demo::unordered_multiset<int> s1 = {1, 2};
        demo::unordered_multiset<int> s2 = {1, 2};
        demo::unordered_multiset<int> s3 = {1, 3};

        CHECK(s1 == s2);
        CHECK_FALSE(s1 == s3);
    }

    TEST_CASE("operator== with duplicates")
    {
        demo::unordered_multiset<int> s1 = {1, 1, 2};
        demo::unordered_multiset<int> s2 = {1, 1, 2};
        demo::unordered_multiset<int> s3 = {1, 2, 2};

        CHECK(s1 == s2);
        CHECK_FALSE(s1 == s3);
    }

    TEST_CASE("operator== same count different order")
    {
        demo::unordered_multiset<int> s1 = {1, 1, 2, 2};
        demo::unordered_multiset<int> s2 = {1, 2, 1, 2};

        CHECK(s1 == s2);
    }

    TEST_CASE("operator!= compares contents")
    {
        demo::unordered_multiset<int> s1 = {1, 2};
        demo::unordered_multiset<int> s2 = {1, 2};
        demo::unordered_multiset<int> s3 = {1, 3};

        CHECK_FALSE(s1 != s2);
        CHECK(s1 != s3);
    }

    TEST_CASE("operator== and != with different sizes")
    {
        demo::unordered_multiset<int> s1 = {1};
        demo::unordered_multiset<int> s2 = {1, 2};

        CHECK_FALSE(s1 == s2);
        CHECK(s1 != s2);
    }

    TEST_CASE("operator== with different counts of same key")
    {
        demo::unordered_multiset<int> s1 = {1, 1};
        demo::unordered_multiset<int> s2 = {1, 1, 1};

        CHECK_FALSE(s1 == s2);
        CHECK(s1 != s2);
    }
}

TEST_SUITE("Unordered_MultiSet Boundary Cases")
{
    TEST_CASE("single element multiset operations")
    {
        demo::unordered_multiset<int> s = {1};

        CHECK(s.size() == 1);
        CHECK(!s.empty());
        CHECK(s.find(1) != s.end());
        CHECK(s.find(2) == s.end());

        s.erase(1);
        CHECK(s.empty());
    }

    TEST_CASE("move semantics with temporary")
    {
        demo::unordered_multiset<std::string> s{};
        s.insert(std::string("test"));

        CHECK(s.size() == 1);
        CHECK(s.find("test") != s.end());
    }

    TEST_CASE("insert and erase alternating")
    {
        demo::unordered_multiset<int> s{};

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
            CHECK(s.find(i) != s.end());
        }
    }

    TEST_CASE("large key range")
    {
        demo::unordered_multiset<int> s{};

        s.insert(-1000);
        s.insert(0);
        s.insert(1000);

        CHECK(s.size() == 3);
        CHECK(s.find(-1000) != s.end());
        CHECK(s.find(0) != s.end());
        CHECK(s.find(1000) != s.end());
    }

    TEST_CASE("all elements accessible after operations")
    {
        demo::unordered_multiset<int> s{};
        for (int i = 0; i < 100; ++i)
        {
            s.insert(i);
        }

        CHECK(s.size() == 100);

        for (int i = 0; i < 100; ++i)
        {
            auto it = s.find(i);
            CHECK(it != s.end());
            CHECK(*it == i);
        }
    }

    TEST_CASE("all duplicates accessible after operations")
    {
        demo::unordered_multiset<int> s{};
        for (int i = 0; i < 10; ++i)
        {
            s.insert(42);
        }

        CHECK(s.size() == 10);
        CHECK(s.count(42) == 10);

        auto range = s.equal_range(42);
        int count = 0;
        for (auto it = range.first; it != range.second; ++it)
        {
            CHECK(*it == 42);
            ++count;
        }
        CHECK(count == 10);
    }
}

TEST_SUITE("Unordered_MultiSet String Type")
{
    TEST_CASE("string key multiset operations")
    {
        demo::unordered_multiset<std::string> s{};

        s.insert("apple");
        s.insert("banana");
        s.insert("cherry");

        CHECK(s.size() == 3);
        CHECK(s.find("apple") != s.end());
        CHECK(s.find("banana") != s.end());
        CHECK(s.find("cherry") != s.end());

        s.erase("banana");
        CHECK(s.size() == 2);
        CHECK(s.find("banana") == s.end());
    }

    TEST_CASE("string key duplicates")
    {
        demo::unordered_multiset<std::string> s = {"a", "a", "b", "b", "b"};

        CHECK(s.size() == 5);
        CHECK(s.count("a") == 2);
        CHECK(s.count("b") == 3);
        CHECK(s.count("c") == 0);
    }

    TEST_CASE("string key hashing")
    {
        demo::unordered_multiset<std::string> s = {"a", "b", "c"};

        CHECK(s.size() == 3);
        CHECK(s.count("a") == 1);
        CHECK(s.count("d") == 0);
    }
}

TEST_SUITE("Unordered_MultiSet Stress Tests")
{
    TEST_CASE("insert and iterate 10000 elements")
    {
        demo::unordered_multiset<int> s{};

        for (int i = 0; i < 10000; ++i)
        {
            s.insert(i);
        }

        CHECK(s.size() == 10000);

        int count = 0;
        for (const auto& val : s)
        {
            bool is_valid = (val >= 0 && val < 10000);
            CHECK(is_valid);
            ++count;
        }
        CHECK(count == 10000);
    }

    TEST_CASE("insert duplicates in random order")
    {
        demo::unordered_multiset<int> s{};
        std::vector<int> keys;

        for (int i = 0; i < 1000; ++i)
        {
            keys.push_back(i % 100);
        }

        std::random_device rd;
        std::mt19937 g(rd());
        std::shuffle(keys.begin(), keys.end(), g);

        for (int key : keys)
        {
            s.insert(key);
        }

        CHECK(s.size() == 1000);
        CHECK(count_elements_in_buckets(s) == 1000);
    }

    TEST_CASE("sequential insert and erase")
    {
        demo::unordered_multiset<int> s{};

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

    TEST_CASE("sequential insert duplicates and erase all")
    {
        demo::unordered_multiset<int> s{};

        for (int i = 0; i < 100; ++i)
        {
            s.insert(42);
        }

        CHECK(s.size() == 100);

        auto erased = s.erase(42);
        CHECK(erased == 100);
        CHECK(s.empty());
    }

    TEST_CASE("alternating insert and erase")
    {
        demo::unordered_multiset<int> s{};

        for (int i = 0; i < 1000; ++i)
        {
            s.insert(i);
            if (i % 10 == 0 && i > 0)
            {
                s.erase(i - 5);
            }
        }

        CHECK(s.size() > 0);
    }

    TEST_CASE("performance test - insert speed")
    {
        demo::unordered_multiset<int> s{};

        auto start = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < 50000; ++i)
        {
            s.insert(i);
        }
        auto end = std::chrono::high_resolution_clock::now();

        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

        CHECK(s.size() == 50000);
        CHECK(duration.count() < 10000);
    }

    TEST_CASE("performance test - find speed")
    {
        demo::unordered_multiset<int> s{};
        for (int i = 0; i < 50000; ++i)
        {
            s.insert(i);
        }

        auto start = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < 50000; ++i)
        {
            auto it = s.find(i);
            CHECK(it != s.end());
            CHECK(*it == i);
        }
        auto end = std::chrono::high_resolution_clock::now();

        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

        CHECK(duration.count() < 5000);
    }

    TEST_CASE("large data persistence")
    {
        demo::unordered_multiset<int> s{};

        for (int i = 0; i < 100000; ++i)
        {
            s.insert(i);
        }

        CHECK(s.size() == 100000);

        for (int i = 0; i < 100000; i += 1000)
        {
            CHECK(s.find(i) != s.end());
            CHECK(*s.find(i) == i);
        }
    }

    TEST_CASE("rehash performance")
    {
        demo::unordered_multiset<int> s{};

        for (int i = 0; i < 10000; ++i)
        {
            s.insert(i);
        }

        auto start = std::chrono::high_resolution_clock::now();
        s.rehash(1000);
        auto end = std::chrono::high_resolution_clock::now();

        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

        CHECK(s.size() == 10000);
        CHECK(duration.count() < 5000);
    }

    TEST_CASE("reserve performance")
    {
        demo::unordered_multiset<int> s{};

        auto start = std::chrono::high_resolution_clock::now();
        s.reserve(100000);
        for (int i = 0; i < 100000; ++i)
        {
            s.insert(i);
        }
        auto end = std::chrono::high_resolution_clock::now();

        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

        CHECK(s.size() == 100000);
    }
}

TEST_SUITE("Unordered_MultiSet Special Types")
{
    TEST_CASE("unordered_multiset with custom struct as key")
    {
        struct MyStruct
        {
            int id;
            std::string name;

            bool operator==(const MyStruct& other) const
            {
                return id == other.id && name == other.name;
            }
        };

        struct MyHash
        {
            size_t operator()(const MyStruct& s) const
            {
                return std::hash<int>()(s.id) ^ std::hash<std::string>()(s.name);
            }
        };

        demo::unordered_multiset<MyStruct, MyHash> s;
        s.insert({1, "first"});
        s.insert({2, "second"});

        CHECK(s.size() == 2);
        CHECK(s.find({1, "first"}) != s.end());
        CHECK(s.find({2, "second"}) != s.end());
    }

    TEST_CASE("unordered_multiset with pointer keys")
    {
        demo::unordered_multiset<int*> s{};
        int* val1 = new int(10);
        int* val2 = new int(20);

        s.insert(val1);
        s.insert(val2);

        CHECK(s.size() == 2);
        CHECK(s.find(val1) != s.end());
        CHECK(s.find(val2) != s.end());

        delete val1;
        delete val2;
    }

    TEST_CASE("unordered_multiset with mixed const and non-const operations")
    {
        demo::unordered_multiset<int> s = {1, 2, 3};
        const demo::unordered_multiset<int>& cs = s;

        CHECK(cs.size() == 3);
        CHECK(cs.count(1) == 1);
        CHECK(cs.find(2) != cs.end());
        CHECK(s.size() == cs.size());
    }
}

TEST_SUITE("Unordered_MultiSet Algorithms")
{
    TEST_CASE("std::for_each on unordered_multiset")
    {
        demo::unordered_multiset<int> s = {1, 2, 3};
        int sum = 0;

        std::for_each(s.begin(), s.end(), [&](const auto& val) { sum += val; });

        CHECK(sum == 6);
    }

    TEST_CASE("std::find_if on unordered_multiset")
    {
        demo::unordered_multiset<int> s = {1, 2, 3};

        auto it = std::find_if(s.begin(), s.end(), [](const auto& val) { return val == 2; });

        CHECK(it != s.end());
        CHECK(*it == 2);
    }

    TEST_CASE("std::count_if on unordered_multiset")
    {
        demo::unordered_multiset<int> s = {1, 2, 3, 4};

        auto count = std::count_if(s.begin(), s.end(), [](const auto& val) { return val % 2 == 0; });

        CHECK(count == 2);
    }

    TEST_CASE("std::count_if on multiset with duplicates")
    {
        demo::unordered_multiset<int> s = {1, 1, 2, 2, 2, 3};

        auto count = std::count_if(s.begin(), s.end(), [](const auto& val) { return val == 2; });

        CHECK(count == 3);
    }
}

TEST_SUITE("Unordered_MultiSet Custom Hash")
{
    TEST_CASE("custom hash function")
    {
        struct CustomHash
        {
            size_t operator()(int val) const
            {
                return static_cast<size_t>(val) * 31;
            }
        };

        demo::unordered_multiset<int, CustomHash> s{};
        s.insert(1);
        s.insert(2);

        CHECK(s.size() == 2);
        CHECK(s.find(1) != s.end());
        CHECK(s.find(2) != s.end());
    }

    TEST_CASE("custom key equality")
    {
        struct CustomEqual
        {
            bool operator()(int a, int b) const
            {
                return (a % 10) == (b % 10);
            }
        };

        demo::unordered_multiset<int, std::hash<int>, CustomEqual> s;

        auto it = s.insert(1);
        CHECK(*it == 1);
        it = s.insert(11);
        CHECK(*it == 11);

        CHECK(s.size() == 2);
        CHECK(s.find(1) != s.end());
        CHECK(s.find(11) != s.end());
    }

    TEST_CASE("custom hash and equality")
    {
        struct CustomEqual
        {
            bool operator()(int a, int b) const
            {
                return (a % 10) == (b % 10);
            }
        };

        struct CustomHash
        {
            size_t operator()(int a) const
            {
                return std::hash<int>()(a % 10);
            }
        };

        demo::unordered_multiset<int, CustomHash, CustomEqual> s;

        auto it = s.insert(1);
        CHECK(*it == 1);
        it = s.insert(11);
        CHECK(*it == 11);

        CHECK(s.size() == 2);
        CHECK(s.count(1) == 2);
        CHECK(s.count(11) == 2);
    }
}

TEST_SUITE("Unordered_MultiSet Additional Edge Cases")
{
    TEST_CASE("erase all elements one by one")
    {
        demo::unordered_multiset<int> s{};
        for (int i = 0; i < 10; ++i)
        {
            s.insert(i);
        }
        CHECK(s.size() == 10);

        for (int i = 0; i < 10; ++i)
        {
            s.erase(i);
        }
        CHECK(s.empty());
        CHECK(s.size() == 0);
    }

    TEST_CASE("erase elements in reverse order")
    {
        demo::unordered_multiset<int> s{};
        for (int i = 0; i < 5; ++i)
        {
            s.insert(i);
        }

        for (int i = 4; i >= 0; --i)
        {
            s.erase(i);
            CHECK(s.find(i) == s.end());
        }
        CHECK(s.empty());
    }

    TEST_CASE("erase iterator from begin")
    {
        demo::unordered_multiset<int> s = {1, 2, 3};

        auto it = s.begin();
        s.erase(it);

        CHECK(s.size() == 2);
    }

    TEST_CASE("erase iterator from end")
    {
        demo::unordered_multiset<int> s = {1, 2, 3};

        auto it = s.find(3);
        REQUIRE(it != s.end());
        it = s.erase(it);

        CHECK(s.size() == 2);
        CHECK(s.find(3) == s.end());
    }

    TEST_CASE("swap preserves all elements")
    {
        demo::unordered_multiset<int> s1 = {1, 2};
        demo::unordered_multiset<int> s2 = {3, 4};

        s1.swap(s2);

        CHECK(s1.size() == 2);
        CHECK(s2.size() == 2);
        CHECK(s1.find(3) != s1.end());
        CHECK(s1.find(4) != s1.end());
        CHECK(s2.find(1) != s2.end());
        CHECK(s2.find(2) != s2.end());
    }

    TEST_CASE("swap multiple times")
    {
        demo::unordered_multiset<int> s1 = {1};
        demo::unordered_multiset<int> s2 = {2};
        demo::unordered_multiset<int> s3 = {3};

        s1.swap(s2);
        CHECK(s1.find(2) != s1.end());
        CHECK(s2.find(1) != s2.end());

        s2.swap(s3);
        CHECK(s2.find(3) != s2.end());
        CHECK(s3.find(1) != s3.end());

        s1.swap(s3);
        CHECK(s1.find(1) != s1.end());
        CHECK(s3.find(2) != s3.end());
    }

    TEST_CASE("copy assignment replaces existing content")
    {
        demo::unordered_multiset<int> s1 = {1};
        demo::unordered_multiset<int> s2 = {2, 3, 4};

        s1 = s2;

        CHECK(s1.size() == 3);
        CHECK(s1.find(2) != s1.end());
        CHECK(s1.find(3) != s1.end());
        CHECK(s1.find(4) != s1.end());
        CHECK(s2.size() == 3);
    }

    TEST_CASE("copy assignment to empty multiset")
    {
        demo::unordered_multiset<int> s1{};
        demo::unordered_multiset<int> s2 = {1, 2};

        s1 = s2;

        CHECK(s1.size() == 2);
        CHECK(s1.find(1) != s1.end());
        CHECK(s1.find(2) != s1.end());
    }

    TEST_CASE("move assignment after use")
    {
        demo::unordered_multiset<std::string> s1 = {"test1"};
        demo::unordered_multiset<std::string> s2 = {"test2", "test3"};

        s1 = std::move(s2);

        CHECK(s1.size() == 2);
        CHECK(s1.find("test2") != s1.end());
        CHECK(s1.find("test3") != s1.end());
        CHECK(s2.empty());
    }

    TEST_CASE("rehash maintains all elements")
    {
        demo::unordered_multiset<int> s = {1, 2, 3};

        size_t old_bucket_count = s.bucket_count();
        s.rehash(old_bucket_count * 4);

        CHECK(s.size() == 3);
        CHECK(s.find(1) != s.end());
        CHECK(s.find(2) != s.end());
        CHECK(s.find(3) != s.end());
    }

    TEST_CASE("rehash to smaller size")
    {
        demo::unordered_multiset<int> s{};
        s.reserve(100);
        for (int i = 0; i < 50; ++i)
        {
            s.insert(i);
        }

        s.rehash(16);
        CHECK(s.size() == 50);
        for (int i = 0; i < 50; ++i)
        {
            CHECK(s.find(i) != s.end());
            CHECK(*s.find(i) == i);
        }
    }

    TEST_CASE("reserve and insert many elements")
    {
        demo::unordered_multiset<int> s{};
        s.reserve(1000);

        for (int i = 0; i < 1000; ++i)
        {
            s.insert(i);
        }

        CHECK(s.size() == 1000);
        for (int i = 0; i < 1000; ++i)
        {
            CHECK(*s.find(i) == i);
        }
    }

    TEST_CASE("load factor behavior")
    {
        demo::unordered_multiset<int> s{};
        CHECK(s.load_factor() >= 0.0f);

        for (int i = 0; i < 100; ++i)
        {
            s.insert(i);
        }

        CHECK(s.load_factor() > 0.0f);
        CHECK(s.load_factor() <= s.max_load_factor() * 2);
    }

    TEST_CASE("clear then repopulate")
    {
        demo::unordered_multiset<int> s = {1, 2};

        s.clear();
        CHECK(s.empty());
        CHECK(s.size() == 0);

        s.insert(3);
        CHECK(s.size() == 1);
        CHECK(s.find(3) != s.end());
    }

    TEST_CASE("const multiset operations")
    {
        const demo::unordered_multiset<std::string> s = {"one", "two", "three"};

        CHECK(s.size() == 3);
        CHECK(s.count("one") == 1);
        CHECK(s.count("four") == 0);
        CHECK(s.find("two") != s.end());
        CHECK(s.find("four") == s.end());
    }

    TEST_CASE("const find returns const iterator")
    {
        const demo::unordered_multiset<int> s = {1};

        auto it = s.find(1);
        CHECK(it != s.end());
        CHECK(*it == 1);
    }

    TEST_CASE("equal_range on existing key")
    {
        demo::unordered_multiset<int> s = {1, 2};

        auto range = s.equal_range(1);
        CHECK(range.first != s.end());
        CHECK(*range.first == 1);
    }

    TEST_CASE("equal_range on non-existing key")
    {
        demo::unordered_multiset<int> s = {1, 2};

        auto range = s.equal_range(99);
        CHECK(range.first == s.end());
        CHECK(range.second == s.end());
    }

    TEST_CASE("merge with empty source")
    {
        demo::unordered_multiset<int> s1 = {1};
        demo::unordered_multiset<int> s2{};

        s1.merge(s2);

        CHECK(s1.size() == 1);
        CHECK(s1.find(1) != s1.end());
    }

    TEST_CASE("merge into empty target")
    {
        demo::unordered_multiset<int> s1{};
        demo::unordered_multiset<int> s2 = {1, 2};

        s1.merge(s2);

        CHECK(s1.size() == 2);
        CHECK(s1.find(1) != s1.end());
        CHECK(s1.find(2) != s1.end());
        CHECK(s2.empty());
    }

    TEST_CASE("operator== with empty multisets")
    {
        demo::unordered_multiset<int> s1{};
        demo::unordered_multiset<int> s2{};

        CHECK(s1 == s2);
        CHECK_FALSE(s1 != s2);
    }

    TEST_CASE("operator== with different values")
    {
        demo::unordered_multiset<int> s1 = {1};
        demo::unordered_multiset<int> s2 = {2};

        CHECK_FALSE(s1 == s2);
        CHECK(s1 != s2);
    }

    TEST_CASE("operator!= with same multisets")
    {
        demo::unordered_multiset<int> s1 = {1, 2};
        demo::unordered_multiset<int> s2 = {1, 2};

        CHECK_FALSE(s1 != s2);
    }

    TEST_CASE("emplace_hint with duplicates")
    {
        demo::unordered_multiset<int> s = {1, 2};
        auto hint = s.find(1);

        auto it = s.emplace_hint(hint, 1);
        CHECK(*it == 1);
        CHECK(s.size() == 3);
        CHECK(s.count(1) == 2);
    }
}