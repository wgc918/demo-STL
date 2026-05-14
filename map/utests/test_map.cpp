#define DOCTEST_CONFIG_NO_MULTITHREADING

#include <algorithm>
#include <cassert>
#include <chrono>
#include <functional>
#include <iostream>
#include <random>
#include <stdexcept>
#include <string>
#include <vector>

#include "../map.h"
#include "doctest.h"

// ============================================
// 测试辅助工具
// ============================================

template <typename K, typename T>
std::vector<std::pair<K, T>> map_to_vector(const demo::map<K, T>& mp)
{
    std::vector<std::pair<K, T>> result;
    for (const auto& elem : mp)
    {
        result.push_back(elem);
    }
    return result;
}

template <typename K, typename T, typename Compare = std::less<K>>
bool map_equals_vector(const demo::map<K, T, Compare>&     mp,
                       const std::vector<std::pair<K, T>>& vec)
{
    auto vec_it = vec.begin();
    for (const auto& elem : mp)
    {
        if (vec_it == vec.end() || elem.first != vec_it->first ||
            elem.second != vec_it->second)
            return false;
        ++vec_it;
    }
    return vec_it == vec.end();
}

template <typename K, typename T>
bool maps_equal(const demo::map<K, T>& a, const demo::map<K, T>& b)
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

template <typename K, typename T>
void assert_map_sorted(const demo::map<K, T>& mp)
{
    auto prev = mp.begin();
    if (prev == mp.end())
        return;
    for (auto curr = ++mp.begin(); curr != mp.end(); ++curr)
    {
        CHECK(curr->first > prev->first);
        ++prev;
    }
}

// ============================================
// 测试套件: Node结构测试
// ============================================

TEST_SUITE("Map Node Structure")
{
    TEST_CASE("Node default constructor initializes to null")
    {
        demo::map<int, int> m;
        CHECK(m.empty());
        CHECK(m.size() == 0);
    }

    TEST_CASE("Node stores key-value pairs correctly")
    {
        demo::map<int, std::string> m;
        m.insert({1, "one"});
        CHECK(m.size() == 1);
        CHECK(m[1] == "one");
    }

    TEST_CASE("Node maintains parent-child relationships")
    {
        demo::map<int, int> m;
        m.insert({3, 30});
        m.insert({1, 10});
        m.insert({5, 50});
        m.insert({2, 20});
        m.insert({4, 40});

        CHECK(m.size() == 5);
        CHECK(m.find(1) != m.end());
        CHECK(m.find(2) != m.end());
        CHECK(m.find(3) != m.end());
        CHECK(m.find(4) != m.end());
        CHECK(m.find(5) != m.end());
    }

    TEST_CASE("Node color initialization")
    {
        demo::map<int, int> m;
        m.insert({1, 10});
        m.insert({2, 20});
        m.insert({3, 30});
        assert_map_sorted(m);
    }

    TEST_CASE("Node deletion removes all relationships")
    {
        demo::map<int, int> m;
        m.insert({1, 10});
        m.insert({2, 20});
        m.insert({3, 30});
        m.erase(2);
        CHECK(m.size() == 2);
        CHECK(m.find(2) == m.end());
        assert_map_sorted(m);
    }
}

// ============================================
// 测试套件: 构造函数
// ============================================

TEST_SUITE("Map Constructors")
{
    TEST_CASE("Default constructor creates empty map")
    {
        demo::map<int, int> m;
        CHECK(m.empty());
        CHECK(m.size() == 0);
        CHECK(m.begin() == m.end());
    }

    TEST_CASE("Constructor with comparator")
    {
        auto                                   comp = std::greater<int>();
        demo::map<int, int, std::greater<int>> m(comp);
        m.insert({1, 10});
        m.insert({2, 20});
        m.insert({3, 30});

        std::vector<std::pair<int, int>> expected = {
            {3, 30},
            {2, 20},
            {1, 10}
        };
        CHECK(map_equals_vector(m, expected));
    }

    TEST_CASE("Initializer list constructor")
    {
        demo::map<int, std::string> m = {
            {1, "one"  },
            {2, "two"  },
            {3, "three"}
        };
        CHECK(m.size() == 3);
        CHECK(m[1] == "one");
        CHECK(m[2] == "two");
        CHECK(m[3] == "three");
    }

    TEST_CASE("Iterator range constructor")
    {
        std::vector<std::pair<int, int>> vec = {
            {1, 10},
            {2, 20},
            {3, 30}
        };
        demo::map<int, int> m(vec.begin(), vec.end());
        CHECK(m.size() == 3);
        CHECK(m[1] == 10);
        CHECK(m[2] == 20);
        CHECK(m[3] == 30);
    }

    TEST_CASE("Copy constructor")
    {
        demo::map<int, std::string> m1 = {
            {1, "one"},
            {2, "two"}
        };
        demo::map<int, std::string> m2(m1);

        CHECK(m2.size() == 2);
        CHECK(m2[1] == "one");
        CHECK(m2[2] == "two");
        CHECK(maps_equal(m1, m2));
    }

    TEST_CASE("Move constructor")
    {
        demo::map<int, std::string> m1 = {
            {1, "one"},
            {2, "two"}
        };
        auto size_before = m1.size();

        demo::map<int, std::string> m2(std::move(m1));

        CHECK(m2.size() == size_before);
        CHECK(m2[1] == "one");
        CHECK(m2[2] == "two");
        CHECK(m1.empty());
    }

    TEST_CASE("Constructor from sorted vector")
    {
        std::vector<std::pair<int, int>> vec = {
            {1, 10},
            {2, 20},
            {3, 30},
            {4, 40},
            {5, 50}
        };
        std::vector<std::pair<const int, int>> vec_const(vec.begin(),
                                                         vec.end());
        demo::map<int, int> m(vec_const, demo::map<int, int>::sorted_tag{});
        CHECK(m.size() == 5);
        assert_map_sorted(m);
    }

    TEST_CASE("Constructor from unsorted vector")
    {
        std::vector<std::pair<const int, int>> vec = {
            {3, 30},
            {1, 10},
            {4, 40},
            {5, 50},
            {2, 20}
        };
        demo::map<int, int> m(vec, demo::map<int, int>::unsorted_tag{});
        CHECK(m.size() == 5);
        assert_map_sorted(m);
    }
}

// ============================================
// 测试套件: 赋值操作符
// ============================================

TEST_SUITE("Map Assignment Operators")
{
    TEST_CASE("Copy assignment operator")
    {
        demo::map<int, std::string> m1 = {
            {1, "one"},
            {2, "two"}
        };
        demo::map<int, std::string> m2;

        m2 = m1;

        CHECK(m2.size() == 2);
        CHECK(m2[1] == "one");
        CHECK(m2[2] == "two");
        CHECK(maps_equal(m1, m2));
    }

    TEST_CASE("Move assignment operator")
    {
        demo::map<int, std::string> m1 = {
            {1, "one"},
            {2, "two"}
        };
        auto                        size_before = m1.size();
        demo::map<int, std::string> m2;

        m2 = std::move(m1);

        CHECK(m2.size() == size_before);
        CHECK(m2[1] == "one");
        CHECK(m2[2] == "two");
        CHECK(m1.empty());
    }

    TEST_CASE("Initializer list assignment")
    {
        demo::map<int, std::string> m;
        m = {
            {1, "one"  },
            {2, "two"  },
            {3, "three"}
        };

        CHECK(m.size() == 3);
        CHECK(m[1] == "one");
        CHECK(m[2] == "two");
        CHECK(m[3] == "three");
    }

    TEST_CASE("Self assignment")
    {
        demo::map<int, std::string> m = {
            {1, "one"},
            {2, "two"}
        };
        m = m;
        CHECK(m.size() == 2);
        CHECK(m[1] == "one");
    }
}

// ============================================
// 测试套件: 元素访问
// ============================================

TEST_SUITE("Map Element Access")
{
    TEST_CASE("operator[] inserts and accesses element")
    {
        demo::map<int, std::string> m;
        m[1] = "one";
        m[2] = "two";

        CHECK(m.size() == 2);
        CHECK(m[1] == "one");
        CHECK(m[2] == "two");
    }

    TEST_CASE("operator[] with rvalue key")
    {
        demo::map<std::string, int> m;
        m[std::string("key1")] = 10;

        CHECK(m.size() == 1);
        CHECK(m["key1"] == 10);
    }

    TEST_CASE("at() returns existing element")
    {
        demo::map<int, std::string> m = {
            {1, "one"},
            {2, "two"}
        };

        CHECK(m.at(1) == "one");
        CHECK(m.at(2) == "two");
    }

    TEST_CASE("at() throws for non-existing key")
    {
        demo::map<int, std::string> m = {
            {1, "one"}
        };

        CHECK_THROWS_AS(m.at(2), std::out_of_range);
    }

    TEST_CASE("const at() returns existing element")
    {
        const demo::map<int, std::string> m = {
            {1, "one"},
            {2, "two"}
        };

        CHECK(m.at(1) == "one");
        CHECK(m.at(2) == "two");
    }

    TEST_CASE("operator[] modifies existing element")
    {
        demo::map<int, std::string> m = {
            {1, "one"}
        };
        m[1] = "modified";

        CHECK(m[1] == "modified");
        CHECK(m.size() == 1);
    }
}

// ============================================
// 测试套件: 迭代器
// ============================================

TEST_SUITE("Map Iterators")
{
    TEST_CASE("begin() and end() iterators")
    {
        demo::map<int, int> m = {
            {1, 10},
            {2, 20},
            {3, 30}
        };

        auto it = m.begin();
        CHECK(it->first == 1);
        CHECK(it->second == 10);

        ++it;
        CHECK(it->first == 2);
        CHECK(it->second == 20);

        ++it;
        CHECK(it->first == 3);
        CHECK(it->second == 30);

        ++it;
        CHECK(it == m.end());
    }

    TEST_CASE("const iterators cbegin() and cend()")
    {
        const demo::map<int, int> m = {
            {1, 10},
            {2, 20},
            {3, 30}
        };

        auto it = m.cbegin();
        CHECK(it->first == 1);
        ++it;
        CHECK(it->first == 2);
        ++it;
        CHECK(it->first == 3);
        ++it;
        CHECK(it == m.cend());
    }

    TEST_CASE("range-based for loop")
    {
        demo::map<int, int> m = {
            {1, 10},
            {2, 20},
            {3, 30}
        };
        int expected_key = 1;

        for (const auto& pair : m)
        {
            CHECK(pair.first == expected_key);
            CHECK(pair.second == expected_key * 10);
            ++expected_key;
        }
    }

    TEST_CASE("iterator increment operator++")
    {
        demo::map<int, int> m = {
            {1, 10},
            {2, 20},
            {3, 30}
        };
        auto it = m.begin();

        CHECK(it->first == 1);
        it++;
        CHECK(it->first == 2);
        ++it;
        CHECK(it->first == 3);
    }

    TEST_CASE("iterator decrement operator--")
    {
        demo::map<int, int> m = {
            {1, 10},
            {2, 20},
            {3, 30}
        };
        auto it = m.end();

        --it;
        CHECK(it->first == 3);
        it--;
        CHECK(it->first == 2);
        --it;
        CHECK(it->first == 1);
    }

    TEST_CASE("reverse iterators rbegin() and rend()")
    {
        demo::map<int, int> m = {
            {1, 10},
            {2, 20},
            {3, 30}
        };
        int expected_key = 3;

        for (auto it = m.rbegin(); it != m.rend(); ++it)
        {
            CHECK(it->first == expected_key);
            CHECK(it->second == expected_key * 10);
            --expected_key;
        }
    }

    TEST_CASE("const reverse iterators crbegin() and crend()")
    {
        const demo::map<int, int> m = {
            {1, 10},
            {2, 20},
            {3, 30}
        };
        int expected_key = 3;

        for (auto it = m.crbegin(); it != m.crend(); ++it)
        {
            CHECK(it->first == expected_key);
            --expected_key;
        }
    }

    TEST_CASE("iterator equality comparison")
    {
        demo::map<int, int> m = {
            {1, 10},
            {2, 20}
        };

        auto it1 = m.begin();
        auto it2 = m.begin();
        auto it3 = m.end();

        CHECK(it1 == it2);
        CHECK(it1 != it3);
        CHECK(it2 != it3);
    }

    TEST_CASE("iterator conversion to const_iterator")
    {
        demo::map<int, int> m = {
            {1, 10}
        };
        demo::map<int, int>::iterator       it       = m.begin();
        demo::map<int, int>::const_iterator const_it = it;

        CHECK(const_it->first == 1);
        CHECK(const_it->second == 10);
    }

    TEST_CASE("iterator post-increment and post-decrement")
    {
        demo::map<int, int> m = {
            {1, 10},
            {2, 20},
            {3, 30}
        };
        auto it = m.begin();

        auto it_copy = it++;
        CHECK(it_copy->first == 1);
        CHECK(it->first == 2);

        it_copy = it--;
        CHECK(it_copy->first == 2);
        CHECK(it->first == 1);
    }

    TEST_CASE("iterator dereference operator")
    {
        demo::map<int, int> m = {
            {1, 10}
        };
        auto it = m.begin();

        auto& pair = *it;
        CHECK(pair.first == 1);
        CHECK(pair.second == 10);
    }

    TEST_CASE("iterator arrow operator")
    {
        demo::map<int, int> m = {
            {1, 10}
        };
        auto it = m.begin();

        CHECK(it->first == 1);
        CHECK(it->second == 10);
    }

    TEST_CASE("empty map iterators")
    {
        demo::map<int, int> m;

        CHECK(m.begin() == m.end());
        CHECK(m.cbegin() == m.cend());
        CHECK(m.rbegin() == m.rend());
        CHECK(m.crbegin() == m.crend());
    }
}

// ============================================
// 测试套件: 容量操作
// ============================================

TEST_SUITE("Map Capacity")
{
    TEST_CASE("empty() returns true for empty map")
    {
        demo::map<int, int> m;
        CHECK(m.empty());
    }

    TEST_CASE("empty() returns false for non-empty map")
    {
        demo::map<int, int> m = {
            {1, 10}
        };
        CHECK_FALSE(m.empty());
    }

    TEST_CASE("size() returns correct number of elements")
    {
        demo::map<int, int> m;

        CHECK(m.size() == 0);

        m.insert({1, 10});
        CHECK(m.size() == 1);

        m.insert({2, 20});
        CHECK(m.size() == 2);

        m.erase(1);
        CHECK(m.size() == 1);
    }

    TEST_CASE("max_size() returns positive value")
    {
        demo::map<int, int> m;
        CHECK(m.max_size() > 0);
    }
}

// ============================================
// 测试套件: 修改器 - 插入操作
// ============================================

TEST_SUITE("Map Modifiers - Insert")
{
    TEST_CASE("insert() with const value")
    {
        demo::map<int, std::string> m;
        std::pair<int, std::string> val = {1, "one"};

        m.insert(val);

        CHECK(m.size() == 1);
        CHECK(m[1] == "one");
    }

    TEST_CASE("insert() with rvalue")
    {
        demo::map<int, std::string> m;

        m.insert({1, "one"});

        CHECK(m.size() == 1);
        CHECK(m[1] == "one");
    }

    TEST_CASE("insert() returns pair with iterator and bool")
    {
        demo::map<int, std::string> m;

        auto result = m.insert({1, "one"});
        CHECK(result.second == true);
        CHECK(result.first->first == 1);

        result = m.insert({1, "two"});
        CHECK(result.second == false);
        CHECK(result.first->second == "one");
    }

    TEST_CASE("insert() with hint position")
    {
        demo::map<int, int> m = {
            {1, 10},
            {3, 30}
        };

        auto hint = m.find(1);
        m.insert(hint, {2, 20});

        CHECK(m.size() == 3);
        CHECK(m[2] == 20);
        assert_map_sorted(m);
    }

    TEST_CASE("insert() with iterator range")
    {
        std::vector<std::pair<int, int>> vec = {
            {1, 10},
            {2, 20},
            {3, 30}
        };
        demo::map<int, int> m;

        m.insert(vec.begin(), vec.end());

        CHECK(m.size() == 3);
        CHECK(m[1] == 10);
        CHECK(m[2] == 20);
        CHECK(m[3] == 30);
    }

    TEST_CASE("insert() with initializer list")
    {
        demo::map<int, int> m;

        m.insert({
            {1, 10},
            {2, 20},
            {3, 30}
        });

        CHECK(m.size() == 3);
        CHECK(m[1] == 10);
        CHECK(m[2] == 20);
        CHECK(m[3] == 30);
    }

    TEST_CASE("insert_or_assign() inserts new element")
    {
        demo::map<int, std::string> m;

        auto result = m.insert_or_assign(1, "one");
        CHECK(result.second == true);
        CHECK(m[1] == "one");
    }

    TEST_CASE("insert_or_assign() assigns to existing element")
    {
        demo::map<int, std::string> m = {
            {1, "one"}
        };

        auto result = m.insert_or_assign(1, "modified");
        CHECK(result.second == false);
        CHECK(m[1] == "modified");
        CHECK(m.size() == 1);
    }

    TEST_CASE("try_emplace() inserts new element")
    {
        demo::map<int, std::string> m;

        auto result = m.try_emplace(1, "one");
        CHECK(result.second == true);
        CHECK(m[1] == "one");
    }

    TEST_CASE("try_emplace() does not modify for existing key")
    {
        demo::map<int, std::string> m = {
            {1, "original"}
        };

        auto result = m.try_emplace(1, "new_value");
        CHECK(result.second == false);
        CHECK(m[1] == "original");
        CHECK(m.size() == 1);
    }

    TEST_CASE("insert duplicate key")
    {
        demo::map<int, int> m;

        m.insert({1, 10});
        auto result = m.insert({1, 20});

        CHECK(result.second == false);
        CHECK(m.size() == 1);
        CHECK(m[1] == 10);
    }
}

// ============================================
// 测试套件: 修改器 - 删除操作
// ============================================

TEST_SUITE("Map Modifiers - Erase")
{
    TEST_CASE("erase() by iterator")
    {
        demo::map<int, int> m = {
            {1, 10},
            {2, 20},
            {3, 30}
        };

        auto it = m.find(2);
        m.erase(it);

        CHECK(m.size() == 2);
        CHECK(m.find(2) == m.end());
        assert_map_sorted(m);
    }

    TEST_CASE("erase() by key")
    {
        demo::map<int, int> m = {
            {1, 10},
            {2, 20},
            {3, 30}
        };

        m.erase(2);

        CHECK(m.size() == 2);
        CHECK(m.find(2) == m.end());
    }

    TEST_CASE("erase() returns iterator to next element")
    {
        demo::map<int, int> m = {
            {1, 10},
            {2, 20},
            {3, 30}
        };

        auto it      = m.find(2);
        auto next_it = m.erase(it);

        CHECK(next_it->first == 3);
    }

    TEST_CASE("erase() range")
    {
        demo::map<int, int> m = {
            {1, 10},
            {2, 20},
            {3, 30},
            {4, 40},
            {5, 50}
        };

        auto first = m.find(2);
        auto last  = m.find(4);
        m.erase(first, last);

        CHECK(m.size() == 3);
        CHECK(m.find(2) == m.end());
        CHECK(m.find(3) == m.end());
        CHECK(m.find(1) != m.end());
        CHECK(m.find(4) != m.end());
        CHECK(m.find(5) != m.end());
    }

    TEST_CASE("clear() removes all elements")
    {
        demo::map<int, int> m = {
            {1, 10},
            {2, 20},
            {3, 30}
        };

        m.clear();

        CHECK(m.empty());
        CHECK(m.size() == 0);
    }

    TEST_CASE("clear() on empty map")
    {
        demo::map<int, int> m;

        CHECK_NOTHROW(m.clear());
        CHECK(m.empty());
    }

    TEST_CASE("erase non-existing key")
    {
        demo::map<int, int> m = {
            {1, 10}
        };

        auto it = m.erase(2);

        CHECK(it == m.end());
        CHECK(m.size() == 1);
    }

    TEST_CASE("erase root node")
    {
        demo::map<int, int> m = {
            {2, 20},
            {1, 10},
            {3, 30}
        };

        m.erase(2);

        CHECK(m.size() == 2);
        CHECK(m.find(2) == m.end());
        assert_map_sorted(m);
    }

    TEST_CASE("erase single element")
    {
        demo::map<int, int> m = {
            {1, 10}
        };

        m.erase(1);

        CHECK(m.empty());
        CHECK(m.size() == 0);
    }
}

// ============================================
// 测试套件: 修改器 - 其他操作
// ============================================

TEST_SUITE("Map Modifiers - Other")
{
    TEST_CASE("swap() exchanges contents")
    {
        demo::map<int, int> m1 = {
            {1, 10},
            {2, 20}
        };
        demo::map<int, int> m2 = {
            {3, 30},
            {4, 40},
            {5, 50}
        };

        m1.swap(m2);

        CHECK(m1.size() == 3);
        CHECK(m1[3] == 30);
        CHECK(m1[4] == 40);
        CHECK(m1[5] == 50);

        CHECK(m2.size() == 2);
        CHECK(m2[1] == 10);
        CHECK(m2[2] == 20);
    }

    TEST_CASE("swap() with empty map")
    {
        demo::map<int, int> m1 = {
            {1, 10}
        };
        demo::map<int, int> m2;

        m1.swap(m2);

        CHECK(m1.empty());
        CHECK(m2.size() == 1);
        CHECK(m2[1] == 10);
    }
}

// ============================================
// 测试套件: 查找操作
// ============================================

TEST_SUITE("Map Lookup")
{
    TEST_CASE("find() returns iterator to existing element")
    {
        demo::map<int, int> m = {
            {1, 10},
            {2, 20},
            {3, 30}
        };

        auto it = m.find(2);

        CHECK(it != m.end());
        CHECK(it->first == 2);
        CHECK(it->second == 20);
    }

    TEST_CASE("find() returns end for non-existing element")
    {
        demo::map<int, int> m = {
            {1, 10},
            {2, 20}
        };

        auto it = m.find(3);

        CHECK(it == m.end());
    }

    TEST_CASE("const find() returns iterator to existing element")
    {
        const demo::map<int, int> m = {
            {1, 10},
            {2, 20}
        };

        auto it = m.find(1);

        CHECK(it != m.end());
        CHECK(it->first == 1);
    }

    TEST_CASE("count() returns 1 for existing key")
    {
        demo::map<int, int> m = {
            {1, 10},
            {2, 20}
        };

        CHECK(m.count(1) == 1);
        CHECK(m.count(2) == 1);
    }

    TEST_CASE("count() returns 0 for non-existing key")
    {
        demo::map<int, int> m = {
            {1, 10},
            {2, 20}
        };

        CHECK(m.count(3) == 0);
    }

    TEST_CASE("lower_bound() returns first not less than key")
    {
        demo::map<int, int> m = {
            {1, 10},
            {3, 30},
            {5, 50}
        };

        auto it = m.lower_bound(2);
        CHECK(it->first == 3);

        it = m.lower_bound(3);
        CHECK(it->first == 3);

        it = m.lower_bound(6);
        CHECK(it == m.end());
    }

    TEST_CASE("upper_bound() returns first greater than key")
    {
        demo::map<int, int> m = {
            {1, 10},
            {3, 30},
            {5, 50}
        };

        auto it = m.upper_bound(2);
        CHECK(it->first == 3);

        it = m.upper_bound(3);
        CHECK(it->first == 5);

        it = m.upper_bound(5);
        CHECK(it == m.end());
    }

    TEST_CASE("equal_range() returns pair of iterators")
    {
        demo::map<int, int> m = {
            {1, 10},
            {3, 30},
            {5, 50}
        };

        auto range = m.equal_range(3);
        CHECK(range.first->first == 3);
        CHECK(range.second->first == 5);
    }
}

// ============================================
// 测试套件: 操作函数
// ============================================

TEST_SUITE("Map Operations")
{
    TEST_CASE("merge() merges two maps")
    {
        demo::map<int, int> m1 = {
            {1, 10},
            {3, 30}
        };
        demo::map<int, int> m2 = {
            {2, 20},
            {4, 40}
        };

        m1.merge(m2);

        CHECK(m1.size() == 4);
        CHECK(m1[1] == 10);
        CHECK(m1[2] == 20);
        CHECK(m1[3] == 30);
        CHECK(m1[4] == 40);
    }

    TEST_CASE("merge() with overlapping keys")
    {
        demo::map<int, int> m1 = {
            {1, 10},
            {2, 20}
        };
        demo::map<int, int> m2 = {
            {2, 200},
            {3, 300}
        };

        m1.merge(m2);

        CHECK(m1.size() == 3);
        CHECK(m1[2] == 20);
        CHECK(m1[3] == 300);
    }

    TEST_CASE("merge() with empty map")
    {
        demo::map<int, int> m1 = {
            {1, 10}
        };
        demo::map<int, int> m2;

        m1.merge(m2);

        CHECK(m1.size() == 1);
        CHECK(m1[1] == 10);
    }

    TEST_CASE("key_comp() returns comparator")
    {
        demo::map<int, int> m;
        auto                comp = m.key_comp();

        CHECK(comp(1, 2));
        CHECK_FALSE(comp(2, 1));
    }
}

// ============================================
// 测试套件: 全局比较运算符
// ============================================

TEST_SUITE("Map Comparison Operators")
{
    TEST_CASE("operator== compares contents")
    {
        demo::map<int, int> m1 = {
            {1, 10},
            {2, 20}
        };
        demo::map<int, int> m2 = {
            {1, 10},
            {2, 20}
        };
        demo::map<int, int> m3 = {
            {1, 10 },
            {2, 200}
        };

        CHECK(m1 == m2);
        CHECK_FALSE(m1 == m3);
    }

    TEST_CASE("operator!= compares contents")
    {
        demo::map<int, int> m1 = {
            {1, 10},
            {2, 20}
        };
        demo::map<int, int> m2 = {
            {1, 10},
            {2, 20}
        };
        demo::map<int, int> m3 = {
            {1, 10 },
            {2, 200}
        };

        CHECK_FALSE(m1 != m2);
        CHECK(m1 != m3);
    }

    TEST_CASE("operator== and != with different sizes")
    {
        demo::map<int, int> m1 = {
            {1, 10}
        };
        demo::map<int, int> m2 = {
            {1, 10},
            {2, 20}
        };

        CHECK_FALSE(m1 == m2);
        CHECK(m1 != m2);
    }
}

// ============================================
// 测试套件: 边界情况和异常场景
// ============================================

TEST_SUITE("Map Boundary Cases")
{
    TEST_CASE("single element map operations")
    {
        demo::map<int, int> m = {
            {1, 10}
        };

        CHECK(m.size() == 1);
        CHECK(!m.empty());
        CHECK(m.find(1) != m.end());
        CHECK(m.find(2) == m.end());

        m.erase(1);
        CHECK(m.empty());
    }

    TEST_CASE("move semantics with temporary")
    {
        demo::map<int, std::string> m;
        m.insert({1, std::string("test")});

        CHECK(m.size() == 1);
        CHECK(m[1] == "test");
    }

    TEST_CASE("get_allocator() returns allocator")
    {
        demo::map<int, int> m;
        auto                alloc = m.get_allocator();

        CHECK(m.size() == 0);
    }

#ifndef NDEBUG
    TEST_CASE("validate_tree() in debug mode")
    {
        demo::map<int, int> m;
        m.insert({1, 10});
        m.insert({2, 20});
        m.insert({3, 30});

        CHECK(m.validate_tree());
    }
#endif

    TEST_CASE("insert and erase alternating")
    {
        demo::map<int, int> m;

        for (int i = 0; i < 10; ++i)
        {
            m.insert({i, i * 10});
        }

        for (int i = 0; i < 10; i += 2)
        {
            m.erase(i);
        }

        CHECK(m.size() == 5);
        for (int i = 1; i < 10; i += 2)
        {
            CHECK(m.find(i) != m.end());
        }
    }

    TEST_CASE("large key range")
    {
        demo::map<int, int> m;

        m.insert({-1000, 1});
        m.insert({0, 2});
        m.insert({1000, 3});

        CHECK(m.size() == 3);
        CHECK(m[-1000] == 1);
        CHECK(m[0] == 2);
        CHECK(m[1000] == 3);
    }
}

// ============================================
// 测试套件: 字符串类型测试
// ============================================

TEST_SUITE("Map String Type")
{
    TEST_CASE("string key map operations")
    {
        demo::map<std::string, int> m;

        m.insert({"apple", 1});
        m.insert({"banana", 2});
        m.insert({"cherry", 3});

        CHECK(m.size() == 3);
        CHECK(m["apple"] == 1);
        CHECK(m["banana"] == 2);
        CHECK(m["cherry"] == 3);

        m.erase("banana");
        CHECK(m.size() == 2);
        CHECK(m.find("banana") == m.end());
    }

    TEST_CASE("string key comparison")
    {
        demo::map<std::string, int> m = {
            {"a", 1},
            {"b", 2},
            {"c", 3}
        };

        assert_map_sorted(m);
    }
}

// ============================================
// 测试套件: 压力测试
// ============================================

TEST_SUITE("Map Stress Tests")
{
    TEST_CASE("insert and iterate 10000 elements")
    {
        demo::map<int, int> m;

        for (int i = 0; i < 10000; ++i)
        {
            m.insert({i, i * 10});
        }

        CHECK(m.size() == 10000);

        int expected = 0;
        for (const auto& pair : m)
        {
            CHECK(pair.first == expected);
            CHECK(pair.second == expected * 10);
            ++expected;
        }
    }

    TEST_CASE("insert in random order")
    {
        demo::map<int, int> m;
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
            m.insert({key, key * 10});
        }

        CHECK(m.size() == 5000);
        assert_map_sorted(m);
    }

    TEST_CASE("sequential insert and erase")
    {
        demo::map<int, int> m;

        for (int i = 0; i < 1000; ++i)
        {
            m.insert({i, i});
        }

        for (int i = 0; i < 1000; ++i)
        {
            m.erase(i);
        }

        CHECK(m.empty());
        CHECK(m.size() == 0);
    }

    TEST_CASE("alternating insert and erase")
    {
        demo::map<int, int> m;

        for (int i = 0; i < 1000; ++i)
        {
            m.insert({i, i});
            if (i % 10 == 0)
            {
                m.erase(i - 5);
            }
        }

        CHECK(m.size() > 0);
    }

    TEST_CASE("performance test - insert speed")
    {
        demo::map<int, int> m;

        auto start = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < 50000; ++i)
        {
            m.insert({i, i});
        }
        auto end = std::chrono::high_resolution_clock::now();

        auto duration =
            std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

        CHECK(m.size() == 50000);
        CHECK(duration.count() < 5000);
    }

    TEST_CASE("performance test - find speed")
    {
        demo::map<int, int> m;
        for (int i = 0; i < 50000; ++i)
        {
            m.insert({i, i});
        }

        auto start = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < 50000; ++i)
        {
            auto it = m.find(i);
            CHECK(it != m.end());
            CHECK(it->second == i);
        }
        auto end = std::chrono::high_resolution_clock::now();

        auto duration =
            std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

        CHECK(duration.count() < 1000);
    }

    TEST_CASE("large data persistence")
    {
        demo::map<int, int> m;

        for (int i = 0; i < 100000; ++i)
        {
            m.insert({i, i});
        }

        CHECK(m.size() == 100000);

        for (int i = 0; i < 100000; i += 1000)
        {
            CHECK(m.find(i) != m.end());
            CHECK(m[i] == i);
        }
    }
}

// ============================================
// 测试套件: 特殊类型测试
// ============================================

TEST_SUITE("Map Special Types")
{
    TEST_CASE("map with custom struct as value")
    {
        struct MyStruct
        {
            int         id;
            std::string name;

            bool operator==(const MyStruct& other) const
            {
                return id == other.id && name == other.name;
            }
        };

        demo::map<int, MyStruct> m;
        m.insert({
            1, {1, "first"}
        });
        m.insert({
            2, {2, "second"}
        });

        CHECK(m.size() == 2);
        CHECK(m[1].id == 1);
        CHECK(m[1].name == "first");
        CHECK(m[2].id == 2);
        CHECK(m[2].name == "second");
    }

    TEST_CASE("map with pointer values")
    {
        demo::map<int, int*> m;
        int*                 val1 = new int(10);
        int*                 val2 = new int(20);

        m.insert({1, val1});
        m.insert({2, val2});

        CHECK(m.size() == 2);
        CHECK(*m[1] == 10);
        CHECK(*m[2] == 20);

        delete val1;
        delete val2;
    }

    TEST_CASE("nested map")
    {
        demo::map<int, demo::map<int, std::string>> m;

        demo::map<int, std::string> inner = {
            {1, "one"},
            {2, "two"}
        };
        m.insert({1, inner});

        CHECK(m.size() == 1);
        CHECK(m[1].size() == 2);
        CHECK(m[1][1] == "one");
        CHECK(m[1][2] == "two");
    }
}

// ============================================
// 测试套件: 算法测试
// ============================================

TEST_SUITE("Map Algorithms")
{
    TEST_CASE("std::for_each on map")
    {
        demo::map<int, int> m = {
            {1, 10},
            {2, 20},
            {3, 30}
        };
        int sum = 0;

        std::for_each(m.begin(), m.end(),
                      [&](const auto& pair) { sum += pair.second; });

        CHECK(sum == 60);
    }

    TEST_CASE("std::find_if on map")
    {
        demo::map<int, int> m = {
            {1, 10},
            {2, 20},
            {3, 30}
        };

        auto it = std::find_if(m.begin(), m.end(), [](const auto& pair)
                               { return pair.second == 20; });

        CHECK(it != m.end());
        CHECK(it->first == 2);
    }

    TEST_CASE("std::count_if on map")
    {
        demo::map<int, int> m = {
            {1, 10},
            {2, 20},
            {3, 30},
            {4, 20}
        };

        auto count = std::count_if(m.begin(), m.end(), [](const auto& pair)
                                   { return pair.second == 20; });

        CHECK(count == 2);
    }
}