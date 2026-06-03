#define DOCTEST_CONFIG_NO_MULTITHREADING

#include <algorithm>
#include <cassert>
#include <chrono>
#include <functional>
#include <random>
#include <string>
#include <vector>

#include "../multimap.h"
#include "doctest.h"

// ============================================
// 测试辅助工具
// ============================================

template <typename K, typename T>
std::vector<std::pair<K, T>> multimap_to_vector(
    const demo::multimap<K, T>& mm)
{
    std::vector<std::pair<K, T>> result;
    for (const auto& elem : mm)
    {
        result.push_back(elem);
    }
    return result;
}

template <typename K, typename T, typename Compare = std::less<K>>
bool multimap_equals_vector(
    const demo::multimap<K, T, Compare>&     mm,
    const std::vector<std::pair<K, T>>& vec)
{
    auto vec_it = vec.begin();
    for (const auto& elem : mm)
    {
        if (vec_it == vec.end() || elem.first != vec_it->first ||
            elem.second != vec_it->second)
            return false;
        ++vec_it;
    }
    return vec_it == vec.end();
}

template <typename K, typename T>
bool multimaps_equal(const demo::multimap<K, T>& a,
                     const demo::multimap<K, T>& b)
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
void assert_multimap_sorted(const demo::multimap<K, T>& mm)
{
    auto prev = mm.begin();
    if (prev == mm.end())
        return;
    for (auto curr = ++mm.begin(); curr != mm.end(); ++curr)
    {
        CHECK(curr->first >= prev->first);
        ++prev;
    }
}

// ============================================
// 测试套件: 构造函数
// ============================================

TEST_SUITE("Multimap Constructors")
{
    TEST_CASE("Default constructor creates empty multimap")
    {
        demo::multimap<int, int> mm;
        CHECK(mm.empty());
        CHECK(mm.size() == 0);
        CHECK(mm.begin() == mm.end());
    }

    TEST_CASE("Constructor with comparator")
    {
        auto comp = std::greater<int>();
        demo::multimap<int, int, std::greater<int>> mm(comp);
        mm.insert({1, 10});
        mm.insert({2, 20});
        mm.insert({3, 30});

        std::vector<std::pair<int, int>> expected = {
            {3, 30},
            {2, 20},
            {1, 10}
        };
        CHECK(multimap_equals_vector(mm, expected));
    }

    TEST_CASE("Initializer list constructor")
    {
        demo::multimap<int, std::string> mm = {
            {1, "one"  },
            {2, "two"  },
            {3, "three"}
        };
        CHECK(mm.size() == 3);
        assert_multimap_sorted(mm);
    }

    TEST_CASE("Initializer list constructor with duplicate keys")
    {
        demo::multimap<int, std::string> mm = {
            {1, "one"  },
            {2, "two"  },
            {1, "first"}
        };
        CHECK(mm.size() == 3);
        assert_multimap_sorted(mm);
    }

    TEST_CASE("Iterator range constructor")
    {
        std::vector<std::pair<int, int>> vec = {
            {1, 10},
            {2, 20},
            {3, 30}
        };
        demo::multimap<int, int> mm(vec.begin(), vec.end());
        CHECK(mm.size() == 3);
        assert_multimap_sorted(mm);
    }

    TEST_CASE("Copy constructor")
    {
        demo::multimap<int, std::string> mm1 = {
            {1, "one"},
            {2, "two"},
            {1, "uno"}
        };
        demo::multimap<int, std::string> mm2(mm1);

        CHECK(mm2.size() == 3);
        CHECK(multimaps_equal(mm1, mm2));
        assert_multimap_sorted(mm2);
        CHECK(mm1.size() == 3);
    }

    TEST_CASE("Move constructor")
    {
        demo::multimap<int, std::string> mm1 = {
            {1, "one"},
            {2, "two"},
            {1, "uno"}
        };
        auto size_before = mm1.size();

        demo::multimap<int, std::string> mm2(std::move(mm1));

        CHECK(mm2.size() == size_before);
        assert_multimap_sorted(mm2);
        CHECK(mm1.empty());
        CHECK(mm1.size() == 0);
    }

    TEST_CASE("Constructor from sorted vector")
    {
        std::vector<std::pair<const int, int>> vec = {
            {1, 10},
            {1, 11},
            {2, 20},
            {3, 30},
            {4, 40}
        };
        demo::multimap<int, int> mm(vec,
                                    demo::multimap<int, int>::sorted_tag{});
        CHECK(mm.size() == 5);
        assert_multimap_sorted(mm);
    }

    TEST_CASE("Constructor from unsorted vector")
    {
        std::vector<std::pair<const int, int>> vec = {
            {3, 30},
            {1, 10},
            {4, 40},
            {1, 11},
            {2, 20}
        };
        demo::multimap<int, int> mm(vec,
                                    demo::multimap<int, int>::unsorted_tag{});
        CHECK(mm.size() == 5);
        assert_multimap_sorted(mm);
    }
}

// ============================================
// 测试套件: 赋值操作符
// ============================================

TEST_SUITE("Multimap Assignment Operators")
{
    TEST_CASE("Copy assignment operator")
    {
        demo::multimap<int, std::string> mm1 = {
            {1, "one"},
            {2, "two"},
            {1, "uno"}
        };
        demo::multimap<int, std::string> mm2;

        mm2 = mm1;

        CHECK(mm2.size() == 3);
        CHECK(multimaps_equal(mm1, mm2));
    }

    TEST_CASE("Move assignment operator")
    {
        demo::multimap<int, std::string> mm1 = {
            {1, "one"},
            {2, "two"},
            {1, "uno"}
        };
        auto size_before = mm1.size();
        demo::multimap<int, std::string> mm2;

        mm2 = std::move(mm1);

        CHECK(mm2.size() == size_before);
        CHECK(mm1.empty());
    }

    TEST_CASE("Initializer list assignment")
    {
        demo::multimap<int, std::string> mm;
        mm = {
            {1, "one"  },
            {2, "two"  },
            {3, "three"}
        };

        CHECK(mm.size() == 3);
        assert_multimap_sorted(mm);
    }

    TEST_CASE("Self assignment")
    {
        demo::multimap<int, std::string> mm = {
            {1, "one"},
            {2, "two"},
            {1, "uno"}
        };
        mm = mm;
        CHECK(mm.size() == 3);
    }
}

// ============================================
// 测试套件: 迭代器
// ============================================

TEST_SUITE("Multimap Iterators")
{
    TEST_CASE("begin() and end() iterators")
    {
        demo::multimap<int, int> mm = {
            {1, 10},
            {2, 20},
            {3, 30}
        };

        auto it = mm.begin();
        CHECK(it->first == 1);
        CHECK(it->second == 10);

        ++it;
        CHECK(it->first == 2);
        CHECK(it->second == 20);

        ++it;
        CHECK(it->first == 3);
        CHECK(it->second == 30);

        ++it;
        CHECK(it == mm.end());
    }

    TEST_CASE("const iterators cbegin() and cend()")
    {
        const demo::multimap<int, int> mm = {
            {1, 10},
            {2, 20},
            {3, 30}
        };

        auto it = mm.cbegin();
        CHECK(it->first == 1);
        ++it;
        CHECK(it->first == 2);
        ++it;
        CHECK(it->first == 3);
        ++it;
        CHECK(it == mm.cend());
    }

    TEST_CASE("range-based for loop with duplicate keys")
    {
        demo::multimap<int, int> mm = {
            {1, 10},
            {1, 11},
            {2, 20},
            {2, 21},
            {3, 30}
        };

        int count = 0;
        std::vector<std::pair<int, int>> seen;
        for (const auto& pair : mm)
        {
            seen.push_back(pair);
            count++;
        }
        CHECK(count == 5);
        CHECK(seen.size() == 5);
        assert_multimap_sorted(mm);
    }

    TEST_CASE("iterator increment operator++")
    {
        demo::multimap<int, int> mm = {
            {1, 10},
            {2, 20},
            {3, 30}
        };
        auto it = mm.begin();

        CHECK(it->first == 1);
        it++;
        CHECK(it->first == 2);
        ++it;
        CHECK(it->first == 3);
        ++it;
        CHECK(it == mm.end());
    }

    TEST_CASE("iterator decrement operator--")
    {
        demo::multimap<int, int> mm = {
            {1, 10},
            {2, 20},
            {3, 30}
        };
        auto it = mm.end();

        --it;
        CHECK(it->first == 3);
        it--;
        CHECK(it->first == 2);
        --it;
        CHECK(it->first == 1);
    }

    TEST_CASE("reverse iterators rbegin() and rend()")
    {
        demo::multimap<int, int> mm = {
            {1, 10},
            {1, 11},
            {2, 20},
            {3, 30}
        };
        int expected_key = 3;

        auto it = mm.rbegin();
        CHECK(it->first == expected_key);
        ++it;
        CHECK(it->first == 2);
        ++it;
        CHECK(it->first == 1);
        CHECK(it->second == 11);
        ++it;
        CHECK(it->first == 1);
        CHECK(it->second == 10);
    }

    TEST_CASE("const reverse iterators")
    {
        const demo::multimap<int, int> mm = {
            {1, 10},
            {2, 20},
            {3, 30}
        };

        auto it = mm.crbegin();
        CHECK(it->first == 3);
        ++it;
        CHECK(it->first == 2);
    }

    TEST_CASE("iterator equality comparison")
    {
        demo::multimap<int, int> mm = {
            {1, 10},
            {2, 20}
        };

        auto it1 = mm.begin();
        auto it2 = mm.begin();
        auto it3 = mm.end();

        CHECK(it1 == it2);
        CHECK(it1 != it3);
        CHECK(it2 != it3);
    }

    TEST_CASE("iterator to const_iterator conversion")
    {
        demo::multimap<int, int> mm = {
            {1, 10}
        };
        demo::multimap<int, int>::iterator       it       = mm.begin();
        demo::multimap<int, int>::const_iterator const_it = it;

        CHECK(const_it->first == 1);
        CHECK(const_it->second == 10);
    }

    TEST_CASE("iterator post-increment and post-decrement")
    {
        demo::multimap<int, int> mm = {
            {1, 10},
            {2, 20},
            {3, 30}
        };
        auto it = mm.begin();

        auto it_copy = it++;
        CHECK(it_copy->first == 1);
        CHECK(it->first == 2);

        it_copy = it--;
        CHECK(it_copy->first == 2);
        CHECK(it->first == 1);
    }

    TEST_CASE("iterator dereference and arrow operators")
    {
        demo::multimap<int, int> mm = {
            {1, 10}
        };
        auto it = mm.begin();

        auto& pair = *it;
        CHECK(pair.first == 1);
        CHECK(pair.second == 10);

        CHECK(it->first == 1);
        CHECK(it->second == 10);
    }

    TEST_CASE("empty multimap iterators")
    {
        demo::multimap<int, int> mm;

        CHECK(mm.begin() == mm.end());
        CHECK(mm.cbegin() == mm.cend());
        CHECK(mm.rbegin() == mm.rend());
        CHECK(mm.crbegin() == mm.crend());
    }
}

// ============================================
// 测试套件: 容量操作
// ============================================

TEST_SUITE("Multimap Capacity")
{
    TEST_CASE("empty() returns true for empty multimap")
    {
        demo::multimap<int, int> mm;
        CHECK(mm.empty());
    }

    TEST_CASE("empty() returns false for non-empty multimap")
    {
        demo::multimap<int, int> mm = {
            {1, 10}
        };
        CHECK_FALSE(mm.empty());
    }

    TEST_CASE("size() returns correct number of elements with duplicates")
    {
        demo::multimap<int, int> mm;

        CHECK(mm.size() == 0);

        mm.insert({1, 10});
        CHECK(mm.size() == 1);

        mm.insert({1, 11});
        CHECK(mm.size() == 2);

        mm.insert({2, 20});
        CHECK(mm.size() == 3);

        mm.erase(1);
        CHECK(mm.size() == 1);
    }

    TEST_CASE("max_size() returns positive value")
    {
        demo::multimap<int, int> mm;
        CHECK(mm.max_size() > 0);
    }
}

// ============================================
// 测试套件: 修改器 - 插入操作
// ============================================

TEST_SUITE("Multimap Modifiers - Insert")
{
    TEST_CASE("insert() with const value")
    {
        demo::multimap<int, std::string> mm;
        std::pair<int, std::string>      val = {1, "one"};

        auto it = mm.insert(val);

        CHECK(mm.size() == 1);
        CHECK(it->first == 1);
        CHECK(it->second == "one");
    }

    TEST_CASE("insert() with rvalue")
    {
        demo::multimap<int, std::string> mm;

        auto it = mm.insert({1, "one"});

        CHECK(mm.size() == 1);
        CHECK(it->first == 1);
        CHECK(it->second == "one");
    }

    TEST_CASE("insert() returns iterator to inserted element")
    {
        demo::multimap<int, std::string> mm;

        auto it = mm.insert({1, "one"});
        CHECK(it->first == 1);
        CHECK(it->second == "one");
    }

    TEST_CASE("insert() allows duplicate keys")
    {
        demo::multimap<int, int> mm;

        auto it1 = mm.insert({1, 10});
        auto it2 = mm.insert({1, 20});
        auto it3 = mm.insert({1, 30});

        CHECK(mm.size() == 3);
        CHECK(it1->second == 10);
        CHECK(it2->second == 20);
        CHECK(it3->second == 30);

        // 所有插入都应该成功
        CHECK(it1 != it2);
        CHECK(it2 != it3);
        CHECK(it1 != it3);
    }

    TEST_CASE("insert() maintains sorted order with duplicates")
    {
        demo::multimap<int, int> mm;

        mm.insert({3, 31});
        mm.insert({1, 11});
        mm.insert({3, 30});
        mm.insert({2, 20});
        mm.insert({1, 10});

        CHECK(mm.size() == 5);
        assert_multimap_sorted(mm);

        // 验证重复键的元素数量
        CHECK(mm.count(1) == 2);
        CHECK(mm.count(2) == 1);
        CHECK(mm.count(3) == 2);
    }

    TEST_CASE("insert() with hint position")
    {
        demo::multimap<int, int> mm = {
            {1, 10},
            {3, 30}
        };
        auto hint   = mm.find(1);
        auto result = mm.insert(hint, {2, 20});

        CHECK(mm.size() == 3);
        CHECK(result->second == 20);
        assert_multimap_sorted(mm);
    }

    TEST_CASE("insert() with hint and duplicate key")
    {
        demo::multimap<int, int> mm = {
            {1, 10},
            {3, 30}
        };
        auto hint   = mm.find(1);
        auto result = mm.insert(hint, {1, 20});

        CHECK(mm.size() == 3);
        CHECK(result->second == 20);
        assert_multimap_sorted(mm);
    }

    TEST_CASE("insert() with iterator range")
    {
        std::vector<std::pair<int, int>> vec = {
            {1, 10},
            {2, 20},
            {1, 11},
            {3, 30}
        };
        demo::multimap<int, int> mm;

        mm.insert(vec.begin(), vec.end());

        CHECK(mm.size() == 4);
        assert_multimap_sorted(mm);
    }

    TEST_CASE("insert() with initializer list")
    {
        demo::multimap<int, int> mm;

        mm.insert({
            {1, 10},
            {2, 20},
            {1, 11},
            {3, 30}
        });

        CHECK(mm.size() == 4);
        assert_multimap_sorted(mm);
    }
}

// ============================================
// 测试套件: 修改器 - 删除操作
// ============================================

TEST_SUITE("Multimap Modifiers - Erase")
{
    TEST_CASE("erase() by iterator removes single element")
    {
        demo::multimap<int, int> mm = {
            {1, 10},
            {1, 11},
            {2, 20},
            {3, 30}
        };

        auto it = mm.find(1);
        CHECK(it != mm.end());
        CHECK(it->first == 1);

        mm.erase(it);

        CHECK(mm.size() == 3);
        // 验证还有两个键为 1 的元素
        CHECK(mm.count(1) == 1);
        CHECK(mm.count(2) == 1);
        CHECK(mm.count(3) == 1);
        assert_multimap_sorted(mm);
    }

    TEST_CASE("erase() returns iterator to next element")
    {
        demo::multimap<int, int> mm = {
            {1, 10},
            {2, 20},
            {3, 30}
        };

        auto it      = mm.find(2);
        auto next_it = mm.erase(it);

        CHECK(next_it->first == 3);
    }

    TEST_CASE("erase() by key removes all elements with that key")
    {
        demo::multimap<int, int> mm = {
            {1, 10},
            {1, 11},
            {1, 12},
            {2, 20},
            {3, 30}
        };

        auto count = mm.erase(1);

        CHECK(count == 3);
        CHECK(mm.size() == 2);
        CHECK(mm.find(1) == mm.end());
        CHECK(mm.find(2) != mm.end());
        CHECK(mm.find(3) != mm.end());
        assert_multimap_sorted(mm);
    }

    TEST_CASE("erase() by key returns 0 for non-existing key")
    {
        demo::multimap<int, int> mm = {
            {1, 10}
        };

        auto count = mm.erase(2);

        CHECK(count == 0);
        CHECK(mm.size() == 1);
    }

    TEST_CASE("erase() range")
    {
        demo::multimap<int, int> mm = {
            {1, 10},
            {2, 20},
            {3, 30},
            {4, 40},
            {5, 50}
        };

        auto first = mm.find(2);
        auto last  = mm.find(4);
        mm.erase(first, last);

        CHECK(mm.size() == 3);
        CHECK(mm.find(2) == mm.end());
        CHECK(mm.find(3) == mm.end());
        CHECK(mm.find(1) != mm.end());
        CHECK(mm.find(4) != mm.end());
        CHECK(mm.find(5) != mm.end());
    }

    TEST_CASE("clear() removes all elements")
    {
        demo::multimap<int, int> mm = {
            {1, 10},
            {1, 11},
            {2, 20},
            {3, 30}
        };

        mm.clear();

        CHECK(mm.empty());
        CHECK(mm.size() == 0);
    }

    TEST_CASE("clear() on empty multimap")
    {
        demo::multimap<int, int> mm;

        CHECK_NOTHROW(mm.clear());
        CHECK(mm.empty());
    }

    TEST_CASE("erase single element by key")
    {
        demo::multimap<int, int> mm = {
            {1, 10}
        };

        auto count = mm.erase(1);

        CHECK(count == 1);
        CHECK(mm.empty());
        CHECK(mm.size() == 0);
    }

    TEST_CASE("erase root node")
    {
        demo::multimap<int, int> mm = {
            {2, 20},
            {1, 10},
            {3, 30}
        };

        auto it = mm.find(2);
        mm.erase(it);

        CHECK(mm.size() == 2);
        CHECK(mm.find(2) == mm.end());
        assert_multimap_sorted(mm);
    }
}

// ============================================
// 测试套件: 修改器 - 其他操作
// ============================================

TEST_SUITE("Multimap Modifiers - Other")
{
    TEST_CASE("swap() exchanges contents")
    {
        demo::multimap<int, int> mm1 = {
            {1, 10},
            {2, 20}
        };
        demo::multimap<int, int> mm2 = {
            {3, 30},
            {4, 40},
            {5, 50}
        };

        mm1.swap(mm2);

        CHECK(mm1.size() == 3);
        CHECK(mm2.size() == 2);
    }

    TEST_CASE("swap() with empty multimap")
    {
        demo::multimap<int, int> mm1 = {
            {1, 10}
        };
        demo::multimap<int, int> mm2;

        mm1.swap(mm2);

        CHECK(mm1.empty());
        CHECK(mm2.size() == 1);
    }
}

// ============================================
// 测试套件: 查找操作
// ============================================

TEST_SUITE("Multimap Lookup")
{
    TEST_CASE("find() returns iterator to first element with matching key")
    {
        demo::multimap<int, int> mm = {
            {1, 10},
            {1, 11},
            {2, 20},
            {3, 30}
        };

        auto it = mm.find(1);

        CHECK(it != mm.end());
        CHECK(it->first == 1);
        // multimap 中 find 返回第一个匹配的元素，具体是哪个实现定义
        CHECK((it->second == 10 || it->second == 11));
    }

    TEST_CASE("find() returns end for non-existing element")
    {
        demo::multimap<int, int> mm = {
            {1, 10},
            {2, 20}
        };

        auto it = mm.find(3);

        CHECK(it == mm.end());
    }

    TEST_CASE("const find() returns iterator to existing element")
    {
        const demo::multimap<int, int> mm = {
            {1, 10},
            {2, 20}
        };

        auto it = mm.find(1);

        CHECK(it != mm.end());
        CHECK(it->first == 1);
    }

    TEST_CASE("count() returns correct count for duplicate keys")
    {
        demo::multimap<int, int> mm = {
            {1, 10},
            {1, 11},
            {1, 12},
            {2, 20},
            {3, 30}
        };

        CHECK(mm.count(1) == 3);
        CHECK(mm.count(2) == 1);
        CHECK(mm.count(3) == 1);
    }

    TEST_CASE("count() returns 0 for non-existing key")
    {
        demo::multimap<int, int> mm = {
            {1, 10},
            {2, 20}
        };

        CHECK(mm.count(3) == 0);
    }

    TEST_CASE("count() returns 0 for empty multimap")
    {
        demo::multimap<int, int> mm;
        CHECK(mm.count(1) == 0);
    }

    TEST_CASE("lower_bound() returns first not less than key")
    {
        demo::multimap<int, int> mm = {
            {1, 10},
            {1, 11},
            {3, 30},
            {5, 50}
        };

        auto it = mm.lower_bound(2);
        CHECK(it->first == 3);

        it = mm.lower_bound(1);
        CHECK(it->first == 1);
        CHECK(it->second == 10);

        it = mm.lower_bound(6);
        CHECK(it == mm.end());
    }

    TEST_CASE("upper_bound() returns first greater than key")
    {
        demo::multimap<int, int> mm = {
            {1, 10},
            {1, 11},
            {3, 30},
            {5, 50}
        };

        auto it = mm.upper_bound(1);
        CHECK(it->first == 3);

        it = mm.upper_bound(3);
        CHECK(it->first == 5);

        it = mm.upper_bound(5);
        CHECK(it == mm.end());
    }

    TEST_CASE("equal_range() returns correct range for duplicate keys")
    {
        demo::multimap<int, int> mm = {
            {1, 10},
            {1, 11},
            {1, 12},
            {2, 20},
            {3, 30}
        };

        auto range = mm.equal_range(1);
        auto count = std::distance(range.first, range.second);
        CHECK(count == 3);

        CHECK(range.first->first == 1);
        CHECK(range.first->second == 10);
        ++range.first;
        CHECK(range.first->second == 11);
        ++range.first;
        CHECK(range.first->second == 12);
        ++range.first;
        CHECK(range.first == range.second);
    }

    TEST_CASE("equal_range() returns empty range for non-existing key")
    {
        demo::multimap<int, int> mm = {
            {1, 10}
        };

        auto range = mm.equal_range(2);
        CHECK(range.first == range.second);
    }

    TEST_CASE("const equal_range()")
    {
        const demo::multimap<int, int> mm = {
            {1, 10},
            {1, 11},
            {2, 20}
        };

        auto range = mm.equal_range(1);
        auto count = std::distance(range.first, range.second);
        CHECK(count == 2);
    }

    TEST_CASE("key_comp() returns comparator")
    {
        demo::multimap<int, int> mm;
        auto                     comp = mm.key_comp();

        CHECK(comp(1, 2));
        CHECK_FALSE(comp(2, 1));
    }
}

// ============================================
// 测试套件: 全局比较运算符
// ============================================

TEST_SUITE("Multimap Comparison Operators")
{
    TEST_CASE("operator== compares contents")
    {
        demo::multimap<int, int> mm1 = {
            {1, 10},
            {1, 11},
            {2, 20}
        };
        demo::multimap<int, int> mm2 = {
            {1, 10},
            {1, 11},
            {2, 20}
        };
        demo::multimap<int, int> mm3 = {
            {1, 10},
            {2, 20}
        };

        CHECK(mm1 == mm2);
        CHECK_FALSE(mm1 == mm3);
    }

    TEST_CASE("operator!= compares contents")
    {
        demo::multimap<int, int> mm1 = {
            {1, 10},
            {2, 20}
        };
        demo::multimap<int, int> mm2 = {
            {1, 10},
            {2, 20}
        };
        demo::multimap<int, int> mm3 = {
            {1, 10 },
            {2, 200}
        };

        CHECK_FALSE(mm1 != mm2);
        CHECK(mm1 != mm3);
    }

    TEST_CASE("operator== and != with different sizes")
    {
        demo::multimap<int, int> mm1 = {
            {1, 10}
        };
        demo::multimap<int, int> mm2 = {
            {1, 10},
            {2, 20}
        };

        CHECK_FALSE(mm1 == mm2);
        CHECK(mm1 != mm2);
    }
}

// ============================================
// 测试套件: 操作函数
// ============================================

TEST_SUITE("Multimap Operations")
{
    TEST_CASE("merge() merges two multimaps")
    {
        demo::multimap<int, int> mm1 = {
            {1, 10},
            {3, 30}
        };
        demo::multimap<int, int> mm2 = {
            {2, 20},
            {4, 40}
        };

        mm1.merge(mm2);

        CHECK(mm1.size() == 4);
        assert_multimap_sorted(mm1);
    }

    TEST_CASE("merge() with overlapping keys")
    {
        demo::multimap<int, int> mm1 = {
            {1, 10},
            {2, 20}
        };
        demo::multimap<int, int> mm2 = {
            {2, 200},
            {3, 300}
        };

        mm1.merge(mm2);

        CHECK(mm1.size() == 4);
        assert_multimap_sorted(mm1);
    }

    TEST_CASE("merge() with empty multimap")
    {
        demo::multimap<int, int> mm1 = {
            {1, 10}
        };
        demo::multimap<int, int> mm2;

        mm1.merge(mm2);

        CHECK(mm1.size() == 1);
    }

    TEST_CASE("get_allocator() returns allocator")
    {
        demo::multimap<int, int> mm;
        auto                     alloc = mm.get_allocator();

        CHECK(mm.size() == 0);
    }
}

// ============================================
// 测试套件: 边界情况和异常场景
// ============================================

TEST_SUITE("Multimap Boundary Cases")
{
    TEST_CASE("single element multimap operations")
    {
        demo::multimap<int, int> mm = {
            {1, 10}
        };

        CHECK(mm.size() == 1);
        CHECK(!mm.empty());
        CHECK(mm.find(1) != mm.end());
        CHECK(mm.find(2) == mm.end());

        mm.erase(1);
        CHECK(mm.empty());
    }

    TEST_CASE("many duplicate keys")
    {
        demo::multimap<int, int> mm;

        for (int i = 0; i < 10; ++i)
        {
            mm.insert({1, i});
        }

        CHECK(mm.size() == 10);
        CHECK(mm.count(1) == 10);
        assert_multimap_sorted(mm);

        // 验证所有值都存在
        auto range = mm.equal_range(1);
        int  count = 0;
        for (auto it = range.first; it != range.second; ++it)
        {
            CHECK(it->first == 1);
            count++;
        }
        CHECK(count == 10);
    }

    TEST_CASE("move semantics with temporary")
    {
        demo::multimap<int, std::string> mm;
        mm.insert({1, std::string("test")});

        CHECK(mm.size() == 1);
    }

    TEST_CASE("insert and erase alternating")
    {
        demo::multimap<int, int> mm;

        for (int i = 0; i < 10; ++i)
        {
            mm.insert({i, i * 10});
        }

        for (int i = 0; i < 10; i += 2)
        {
            mm.erase(i);
        }

        CHECK(mm.size() == 5);
        for (int i = 1; i < 10; i += 2)
        {
            CHECK(mm.find(i) != mm.end());
        }
    }

    TEST_CASE("large key range")
    {
        demo::multimap<int, int> mm;

        mm.insert({-1000, 1});
        mm.insert({0, 2});
        mm.insert({1000, 3});

        CHECK(mm.size() == 3);
        assert_multimap_sorted(mm);
    }

#ifndef NDEBUG
    TEST_CASE("validate_tree() in debug mode")
    {
        demo::multimap<int, int> mm;
        mm.insert({1, 10});
        mm.insert({2, 20});
        mm.insert({3, 30});
        mm.insert({1, 11});

        CHECK(mm.validate_tree());
    }
#endif
}

// ============================================
// 测试套件: 字符串类型测试
// ============================================

TEST_SUITE("Multimap String Type")
{
    TEST_CASE("string key multimap operations")
    {
        demo::multimap<std::string, int> mm;

        mm.insert({"apple", 1});
        mm.insert({"banana", 2});
        mm.insert({"apple", 3});
        mm.insert({"cherry", 4});

        CHECK(mm.size() == 4);
        CHECK(mm.count("apple") == 2);
        CHECK(mm.count("banana") == 1);
        CHECK(mm.count("cherry") == 1);

        mm.erase("apple");
        CHECK(mm.size() == 2);
        CHECK(mm.count("apple") == 0);
    }

    TEST_CASE("string key comparison")
    {
        demo::multimap<std::string, int> mm = {
            {"a", 1},
            {"b", 2},
            {"c", 3}
        };

        assert_multimap_sorted(mm);
    }
}

// ============================================
// 测试套件: 压力测试
// ============================================

TEST_SUITE("Multimap Stress Tests")
{
    TEST_CASE("insert and iterate 10000 elements")
    {
        demo::multimap<int, int> mm;

        for (int i = 0; i < 10000; ++i)
        {
            mm.insert({i, i * 10});
        }

        CHECK(mm.size() == 10000);

        int expected = 0;
        for (const auto& pair : mm)
        {
            CHECK(pair.first == expected);
            CHECK(pair.second == expected * 10);
            ++expected;
        }
    }

    TEST_CASE("insert duplicate keys in large quantity")
    {
        demo::multimap<int, int> mm;

        for (int i = 0; i < 1000; ++i)
        {
            mm.insert({1, i});
        }
        for (int i = 0; i < 1000; ++i)
        {
            mm.insert({2, i});
        }

        CHECK(mm.size() == 2000);
        CHECK(mm.count(1) == 1000);
        CHECK(mm.count(2) == 1000);
        assert_multimap_sorted(mm);
    }

    TEST_CASE("insert in random order")
    {
        demo::multimap<int, int> mm;
        std::vector<int>         keys;

        for (int i = 0; i < 5000; ++i)
        {
            keys.push_back(i);
        }

        std::random_device rd;
        std::mt19937       g(rd());
        std::shuffle(keys.begin(), keys.end(), g);

        for (int key : keys)
        {
            mm.insert({key, key * 10});
        }

        CHECK(mm.size() == 5000);
        assert_multimap_sorted(mm);
    }

    TEST_CASE("sequential insert and erase")
    {
        demo::multimap<int, int> mm;

        for (int i = 0; i < 1000; ++i)
        {
            mm.insert({i, i});
        }

        for (int i = 0; i < 1000; ++i)
        {
            mm.erase(i);
        }

        CHECK(mm.empty());
        CHECK(mm.size() == 0);
    }

    TEST_CASE("erase all duplicate keys at once")
    {
        demo::multimap<int, int> mm;

        for (int i = 0; i < 100; ++i)
        {
            mm.insert({1, i});
        }

        auto count = mm.erase(1);
        CHECK(count == 100);
        CHECK(mm.empty());
        CHECK(mm.count(1) == 0);
    }

    TEST_CASE("alternating insert and erase")
    {
        demo::multimap<int, int> mm;

        for (int i = 0; i < 1000; ++i)
        {
            mm.insert({i, i});
            if (i % 10 == 0)
            {
                mm.erase(i - 5);
            }
        }

        CHECK(mm.size() > 0);
        assert_multimap_sorted(mm);
    }

    TEST_CASE("performance test - insert 50000 elements")
    {
        demo::multimap<int, int> mm;

        auto start = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < 50000; ++i)
        {
            mm.insert({i, i});
        }
        auto end = std::chrono::high_resolution_clock::now();

        auto duration =
            std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

        CHECK(mm.size() == 50000);
        CHECK(duration.count() < 5000);
    }

    TEST_CASE("performance test - find speed")
    {
        demo::multimap<int, int> mm;
        for (int i = 0; i < 50000; ++i)
        {
            mm.insert({i, i});
        }

        auto start = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < 50000; ++i)
        {
            auto it = mm.find(i);
            CHECK(it != mm.end());
            CHECK(it->second == i);
        }
        auto end = std::chrono::high_resolution_clock::now();

        auto duration =
            std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

        CHECK(duration.count() < 1000);
    }

    TEST_CASE("large data persistence")
    {
        demo::multimap<int, int> mm;

        for (int i = 0; i < 100000; ++i)
        {
            mm.insert({i, i});
        }

        CHECK(mm.size() == 100000);

        for (int i = 0; i < 100000; i += 1000)
        {
            CHECK(mm.find(i) != mm.end());
        }
    }
}

// ============================================
// 测试套件: 特殊类型测试
// ============================================

TEST_SUITE("Multimap Special Types")
{
    TEST_CASE("multimap with custom struct as value")
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

        demo::multimap<int, MyStruct> mm;
        mm.insert({
            1, {1, "first"}
        });
        mm.insert({
            1, {2, "second"}
        });
        mm.insert({
            2, {3, "third"}
        });

        CHECK(mm.size() == 3);
        CHECK(mm.count(1) == 2);
        CHECK(mm.count(2) == 1);
    }

    TEST_CASE("multimap with pointer values")
    {
        demo::multimap<int, int*> mm;
        int*                      val1 = new int(10);
        int*                      val2 = new int(20);

        mm.insert({1, val1});
        mm.insert({1, val2});

        CHECK(mm.size() == 2);

        auto it = mm.find(1);
        CHECK(*it->second == 10);
        ++it;
        CHECK(*it->second == 20);

        delete val1;
        delete val2;
    }

    TEST_CASE("nested multimap")
    {
        demo::multimap<int, demo::multimap<int, std::string>> mm;

        demo::multimap<int, std::string> inner = {
            {1, "one"},
            {2, "two"}
        };
        mm.insert({1, inner});

        CHECK(mm.size() == 1);
        CHECK(mm.find(1)->second.size() == 2);
    }
}

// ============================================
// 测试套件: 算法测试
// ============================================

TEST_SUITE("Multimap Algorithms")
{
    TEST_CASE("std::for_each on multimap")
    {
        demo::multimap<int, int> mm = {
            {1, 10},
            {1, 11},
            {2, 20},
            {3, 30}
        };
        int sum = 0;

        std::for_each(mm.begin(), mm.end(),
                      [&](const auto& pair) { sum += pair.second; });

        CHECK(sum == 71);
    }

    TEST_CASE("std::find_if on multimap")
    {
        demo::multimap<int, int> mm = {
            {1, 10},
            {2, 20},
            {3, 30}
        };

        auto it = std::find_if(mm.begin(), mm.end(), [](const auto& pair)
                               { return pair.second == 20; });

        CHECK(it != mm.end());
        CHECK(it->first == 2);
    }

    TEST_CASE("std::count_if on multimap")
    {
        demo::multimap<int, int> mm = {
            {1, 10},
            {1, 20},
            {2, 20},
            {3, 30},
            {4, 20}
        };

        auto count = std::count_if(mm.begin(), mm.end(), [](const auto& pair)
                                   { return pair.second == 20; });

        CHECK(count == 3);
    }
}

// ============================================
// 测试套件: 核心 multimap 特性验证
// ============================================

TEST_SUITE("Multimap Core Features")
{
    TEST_CASE("insert always succeeds with duplicate keys")
    {
        demo::multimap<int, std::string> mm;

        for (int i = 0; i < 20; ++i)
        {
            auto it = mm.insert({1, "value"});
            CHECK(it != mm.end());
        }

        CHECK(mm.size() == 20);
        CHECK(mm.count(1) == 20);
    }

    TEST_CASE("erase by key removes all matching elements")
    {
        demo::multimap<int, int> mm;

        mm.insert({1, 10});
        mm.insert({1, 20});
        mm.insert({1, 30});
        mm.insert({2, 40});

        auto count = mm.erase(1);
        CHECK(count == 3);
        CHECK(mm.size() == 1);
        CHECK(mm.count(1) == 0);
        CHECK(mm.find(2) != mm.end());
    }

    TEST_CASE("equal_range covers all duplicate keys")
    {
        demo::multimap<int, std::string> mm = {
            {1, "a"},
            {2, "b"},
            {1, "c"},
            {3, "d"},
            {1, "e"},
            {2, "f"}
        };

        auto range = mm.equal_range(1);
        int  count = 0;
        for (auto it = range.first; it != range.second; ++it)
        {
            CHECK(it->first == 1);
            count++;
        }
        CHECK(count == 3);

        range = mm.equal_range(2);
        count = 0;
        for (auto it = range.first; it != range.second; ++it)
        {
            CHECK(it->first == 2);
            count++;
        }
        CHECK(count == 2);
    }

    TEST_CASE("lower_bound and upper_bound with duplicate keys")
    {
        demo::multimap<int, int> mm = {
            {1, 10},
            {1, 11},
            {1, 12},
            {2, 20},
            {3, 30}
        };

        auto lb = mm.lower_bound(1);
        CHECK(lb->first == 1);
        CHECK(lb->second == 10);

        auto ub = mm.upper_bound(1);
        CHECK(ub->first == 2);

        auto count = std::distance(lb, ub);
        CHECK(count == 3);
    }

    TEST_CASE("count reflects actual number of elements")
    {
        demo::multimap<int, int> mm;

        CHECK(mm.count(1) == 0);

        mm.insert({1, 10});
        CHECK(mm.count(1) == 1);

        mm.insert({1, 20});
        CHECK(mm.count(1) == 2);

        mm.erase(mm.find(1));
        CHECK(mm.count(1) == 1);

        mm.erase(1);
        CHECK(mm.count(1) == 0);
    }
}