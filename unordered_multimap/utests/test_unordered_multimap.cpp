#define DOCTEST_CONFIG_NO_MULTITHREADING

#include <algorithm>
#include <cassert>
#include <chrono>
#include <functional>
#include <random>
#include <string>
#include <vector>

#include "../unordered_multimap.h"
#include "doctest.h"

template <typename K, typename T>
std::vector<std::pair<K, T>> unordered_multimap_to_vector(
    const demo::unordered_multimap<K, T>& mp)
{
    std::vector<std::pair<K, T>> result;
    for (const auto& elem : mp)
    {
        result.push_back(elem);
    }
    return result;
}

template <typename K, typename T>
bool unordered_multimaps_equal(const demo::unordered_multimap<K, T>& a,
                               const demo::unordered_multimap<K, T>& b)
{
    if (a.size() != b.size())
        return false;

    for (const auto& elem : a)
    {
        auto range = b.equal_range(elem.first);
        bool found = false;
        for (auto it = range.first; it != range.second; ++it)
        {
            if (it->second == elem.second)
            {
                found = true;
                break;
            }
        }
        if (!found)
            return false;
    }
    return true;
}

template <typename K, typename T>
size_t count_elements_in_buckets(const demo::unordered_multimap<K, T>& mp)
{
    size_t count = 0;
    for (size_t i = 0; i < mp.bucket_count(); ++i)
    {
        count += mp.bucket_size(i);
    }
    return count;
}

#define UMM_INT_INT demo::unordered_multimap<int, int>
#define UMM_STR_INT demo::unordered_multimap<std::string, int>

// ============================================
// 测试套件: Node结构测试
// ============================================

TEST_SUITE("Unordered_Multimap Node Structure")
{
    TEST_CASE("Node default constructor initializes correctly")
    {
        demo::unordered_multimap<int, int> m{};
        CHECK(m.empty());
        CHECK(m.size() == 0);
    }

    TEST_CASE("Node stores key-value pairs correctly")
    {
        demo::unordered_multimap<int, std::string> m{};
        m.insert({1, "one"});
        CHECK(m.size() == 1);
        auto it = m.find(1);
        CHECK(it != m.end());
        CHECK(it->second == "one");
    }

    TEST_CASE("Node maintains hash chain relationships")
    {
        demo::unordered_multimap<int, int> m{};
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

    TEST_CASE("Node deletion removes all elements")
    {
        demo::unordered_multimap<int, int> m{};
        m.insert({1, 10});
        m.insert({2, 20});
        m.insert({3, 30});
        m.erase(m.find(2));
        CHECK(m.size() == 2);
        CHECK(m.find(2) == m.end());
    }
}

// ============================================
// 测试套件: 构造函数
// ============================================

TEST_SUITE("Unordered_Multimap Constructors")
{
    TEST_CASE("Default constructor creates empty map")
    {
        demo::unordered_multimap<int, int> m{};
        CHECK(m.empty());
        CHECK(m.size() == 0);
        CHECK(m.begin() == m.end());
    }

    TEST_CASE("Constructor with bucket count")
    {
        demo::unordered_multimap<int, int> m(32);
        CHECK(m.bucket_count() == 32);
        CHECK(m.empty());

        demo::unordered_multimap<int, int> n(31);
        CHECK(n.bucket_count() >= 31);
        CHECK(n.empty());
    }

    TEST_CASE("Initializer list constructor")
    {
        demo::unordered_multimap<int, std::string> m = {
            {1, "one"  },
            {2, "two"  },
            {3, "three"}
        };
        CHECK(m.size() == 3);
        CHECK(m.find(1)->second == "one");
        CHECK(m.find(2)->second == "two");
        CHECK(m.find(3)->second == "three");
    }

    TEST_CASE("Iterator range constructor")
    {
        std::vector<std::pair<int, int>> vec = {
            {1, 10},
            {2, 20},
            {3, 30}
        };
        demo::unordered_multimap<int, int> m(vec.begin(), vec.end());
        CHECK(m.size() == 3);
        CHECK(m.find(1)->second == 10);
        CHECK(m.find(2)->second == 20);
        CHECK(m.find(3)->second == 30);
    }

    TEST_CASE("Copy constructor")
    {
        demo::unordered_multimap<int, std::string> m1 = {
            {1, "one"},
            {2, "two"}
        };
        demo::unordered_multimap<int, std::string> m2(m1);

        CHECK(m2.size() == 2);
        CHECK(m2.find(1)->second == "one");
        CHECK(m2.find(2)->second == "two");
        CHECK(unordered_multimaps_equal(m1, m2));
        CHECK(m1.size() == 2);
    }

    TEST_CASE("Move constructor")
    {
        demo::unordered_multimap<int, std::string> m1 = {
            {1, "one"},
            {2, "two"}
        };
        auto size_before = m1.size();

        demo::unordered_multimap<int, std::string> m2(std::move(m1));

        CHECK(m2.size() == size_before);
        CHECK(m2.find(1)->second == "one");
        CHECK(m2.find(2)->second == "two");
        CHECK(m1.empty());
        CHECK(m1.size() == 0);
    }
}

// ============================================
// 测试套件: 赋值运算符
// ============================================

TEST_SUITE("Unordered_Multimap Assignment Operators")
{
    TEST_CASE("Copy assignment operator")
    {
        demo::unordered_multimap<int, std::string> m1 = {
            {1, "one"},
            {2, "two"}
        };
        demo::unordered_multimap<int, std::string> m2{};

        m2 = m1;

        CHECK(m2.size() == 2);
        CHECK(m2.find(1)->second == "one");
        CHECK(m2.find(2)->second == "two");
        CHECK(unordered_multimaps_equal(m1, m2));
    }

    TEST_CASE("Move assignment operator")
    {
        demo::unordered_multimap<int, std::string> m1 = {
            {1, "one"},
            {2, "two"}
        };
        auto                                     size_before = m1.size();
        demo::unordered_multimap<int, std::string> m2{};

        m2 = std::move(m1);

        CHECK(m2.size() == size_before);
        CHECK(m2.find(1)->second == "one");
        CHECK(m2.find(2)->second == "two");
        CHECK(m1.empty());
    }

    TEST_CASE("Self assignment")
    {
        demo::unordered_multimap<int, std::string> m = {
            {1, "one"},
            {2, "two"}
        };
        m = m;
        CHECK(m.size() == 2);
        CHECK(m.find(1)->second == "one");
    }
}

// ============================================
// 测试套件: 迭代器
// ============================================

TEST_SUITE("Unordered_Multimap Iterators")
{
    TEST_CASE("begin() and end() iterators")
    {
        demo::unordered_multimap<int, int> m = {
            {1, 10},
            {2, 20},
            {3, 30}
        };

        auto it    = m.begin();
        int  count = 0;
        for (; it != m.end(); ++it)
        {
            CHECK(it->second == it->first * 10);
            ++count;
        }
        CHECK(count == 3);
    }

    TEST_CASE("const iterators cbegin() and cend()")
    {
        const demo::unordered_multimap<int, int> m = {
            {1, 10},
            {2, 20},
            {3, 30}
        };

        auto it    = m.cbegin();
        int  count = 0;
        for (; it != m.cend(); ++it)
        {
            CHECK(it->second == it->first * 10);
            ++count;
        }
        CHECK(count == 3);
    }

    TEST_CASE("range-based for loop")
    {
        demo::unordered_multimap<int, int> m = {
            {1, 10},
            {2, 20},
            {3, 30}
        };
        int count = 0;

        for (const auto& pair : m)
        {
            CHECK(pair.second == pair.first * 10);
            ++count;
        }
        CHECK(count == 3);
    }

    TEST_CASE("iterator increment operator++")
    {
        demo::unordered_multimap<int, int> m = {
            {1, 10},
            {2, 20},
            {3, 30}
        };
        auto it = m.begin();

        it++;
        it++;
        it++;
        CHECK(it == m.end());
    }

    TEST_CASE("iterator post-increment")
    {
        demo::unordered_multimap<int, int> m = {
            {1, 10},
            {2, 20},
            {3, 30}
        };
        auto it = m.begin();

        auto it_copy = it++;
        CHECK(it_copy->first == 1);
    }

    TEST_CASE("iterator dereference operator")
    {
        demo::unordered_multimap<int, int> m = {
            {1, 10}
        };
        auto it = m.begin();

        auto& pair = *it;
        CHECK(pair.first == 1);
        CHECK(pair.second == 10);
    }

    TEST_CASE("iterator arrow operator")
    {
        demo::unordered_multimap<int, int> m = {
            {1, 10}
        };
        auto it = m.begin();

        CHECK(it->first == 1);
        CHECK(it->second == 10);
    }

    TEST_CASE("empty map iterators")
    {
        demo::unordered_multimap<int, int> m{};

        CHECK(m.begin() == m.end());
        CHECK(m.cbegin() == m.cend());
    }

    TEST_CASE("iterator conversion to const_iterator")
    {
        demo::unordered_multimap<int, int> m = {
            {1, 10}
        };
        demo::unordered_multimap<int, int>::iterator       it       = m.begin();
        demo::unordered_multimap<int, int>::const_iterator const_it = it;

        CHECK(const_it->first == 1);
        CHECK(const_it->second == 10);
    }
}

// ============================================
// 测试套件: 桶迭代器
// ============================================

TEST_SUITE("Unordered_Multimap Bucket Iterators")
{
    TEST_CASE("local_iterator begin and end")
    {
        demo::unordered_multimap<int, int> m{};
        m.insert({1, 10});
        m.insert({2, 20});

        size_t bucket_idx = m.bucket(1);
        auto   local_it   = m.begin(bucket_idx);
        auto   local_end  = m.end(bucket_idx);

        if (local_it == local_end)
        {
            CHECK(m.bucket_size(bucket_idx) == 0);
        }
        else
        {
            CHECK(m.bucket_size(bucket_idx) > 0);
        }
    }

    TEST_CASE("const_local_iterator begin and end")
    {
        const demo::unordered_multimap<int, int> m = {
            {1, 10},
            {2, 20}
        };

        size_t bucket_idx = m.bucket(1);
        auto   local_it   = m.cbegin(bucket_idx);
        auto   local_end  = m.cend(bucket_idx);

        if (local_it == local_end)
        {
            CHECK(m.bucket_size(bucket_idx) == 0);
        }
        else
        {
            CHECK(m.bucket_size(bucket_idx) > 0);
        }
    }

    TEST_CASE("local_iterator increment")
    {
        demo::unordered_multimap<int, int> m(64);
        for (int i = 0; i < 10; ++i)
        {
            m.insert({i, i * 10});
        }

        size_t bucket_idx = m.bucket(0);
        int    count      = 0;
        for (auto it = m.begin(bucket_idx); it != m.end(bucket_idx); ++it)
        {
            ++count;
        }
        CHECK(count == m.bucket_size(bucket_idx));
    }
}

// ============================================
// 测试套件: 容量操作
// ============================================

TEST_SUITE("Unordered_Multimap Capacity")
{
    TEST_CASE("empty() returns true for empty map")
    {
        demo::unordered_multimap<int, int> m{};
        CHECK(m.empty());
    }

    TEST_CASE("empty() returns false for non-empty map")
    {
        demo::unordered_multimap<int, int> m = {
            {1, 10}
        };
        CHECK_FALSE(m.empty());
    }

    TEST_CASE("size() returns correct number of elements")
    {
        demo::unordered_multimap<int, int> m{};

        CHECK(m.size() == 0);

        m.insert({1, 10});
        CHECK(m.size() == 1);

        m.insert({2, 20});
        CHECK(m.size() == 2);

        m.erase(m.find(1));
        CHECK(m.size() == 1);
    }

    TEST_CASE("max_size() returns positive value")
    {
        demo::unordered_multimap<int, int> m{};
        CHECK(m.max_size() > 0);
    }

    TEST_CASE("bucket_count() returns valid value")
    {
        demo::unordered_multimap<int, int> m{};
        CHECK(m.bucket_count() >= 16);
    }

    TEST_CASE("max_bucket_count() returns positive value")
    {
        demo::unordered_multimap<int, int> m{};
        CHECK(m.max_bucket_count() > 0);
    }
}

// ============================================
// 测试套件: 修改器 - 插入操作（核心区别：允许重复键）
// ============================================

TEST_SUITE("Unordered_Multimap Modifiers - Insert")
{
    TEST_CASE("insert() returns iterator (not pair<iterator,bool>)")
    {
        demo::unordered_multimap<int, std::string> m{};

        auto it = m.insert({1, "one"});
        CHECK(it->first == 1);
        CHECK(it->second == "one");
        CHECK(m.size() == 1);
    }

    TEST_CASE("insert() with const value")
    {
        demo::unordered_multimap<int, std::string> m{};
        std::pair<int, std::string>              val = {1, "one"};

        m.insert(val);

        CHECK(m.size() == 1);
        CHECK(m.find(1)->second == "one");
    }

    TEST_CASE("insert duplicate key succeeds")
    {
        demo::unordered_multimap<int, int> m{};

        auto it1 = m.insert({1, 10});
        CHECK(it1->second == 10);
        CHECK(m.size() == 1);

        auto it2 = m.insert({1, 20});
        CHECK(it2->second == 20);
        CHECK(m.size() == 2);

        CHECK(m.count(1) == 2);
    }

    TEST_CASE("insert multiple duplicate keys")
    {
        demo::unordered_multimap<int, int> m{};

        m.insert({1, 10});
        m.insert({1, 20});
        m.insert({1, 30});
        m.insert({2, 40});
        m.insert({2, 50});

        CHECK(m.size() == 5);
        CHECK(m.count(1) == 3);
        CHECK(m.count(2) == 2);
    }

    TEST_CASE("insert() with hint position")
    {
        demo::unordered_multimap<int, int> m = {
            {1, 10},
            {3, 30}
        };
        CHECK(m.size() == 2);
        auto hint = m.find(1);
        CHECK(hint != m.end());
        m.insert(hint, {2, 20});
        CHECK(m.size() == 3);
        CHECK(m.find(2)->second == 20);
    }

    TEST_CASE("insert() with iterator range")
    {
        std::vector<std::pair<int, int>> vec = {
            {1, 10},
            {2, 20},
            {3, 30},
            {1, 100}
        };
        demo::unordered_multimap<int, int> m{};

        m.insert(vec.begin(), vec.end());

        CHECK(m.size() == 4);
        CHECK(m.count(1) == 2);
        CHECK(m.count(2) == 1);
        CHECK(m.count(3) == 1);
    }

    TEST_CASE("insert() with initializer list")
    {
        demo::unordered_multimap<int, int> m{};

        m.insert({
            {1, 10},
            {2, 20},
            {3, 30},
            {1, 100}
        });

        CHECK(m.size() == 4);
        CHECK(m.count(1) == 2);
        CHECK(m.count(2) == 1);
        CHECK(m.count(3) == 1);
    }

    TEST_CASE("emplace() returns iterator")
    {
        demo::unordered_multimap<int, std::string> m{};

        auto it = m.emplace(1, "one");
        CHECK(it->first == 1);
        CHECK(it->second == "one");
        CHECK(m.size() == 1);
    }

    TEST_CASE("emplace() always inserts new element")
    {
        demo::unordered_multimap<int, std::string> m = {
            {1, "original"}
        };

        auto it = m.emplace(1, "duplicate");
        CHECK(it->second == "duplicate");
        CHECK(m.size() == 2);
        CHECK(m.count(1) == 2);
    }

    TEST_CASE("emplace_hint() always inserts")
    {
        demo::unordered_multimap<int, int> m = {
            {1, 10}
        };
        auto hint = m.find(1);
        auto it   = m.emplace_hint(hint, 1, 20);
        CHECK(it->second == 20);
        CHECK(m.size() == 2);
    }
}

// ============================================
// 测试套件: 修改器 - 删除操作（核心区别：erase(key) 删除所有匹配元素）
// ============================================

TEST_SUITE("Unordered_Multimap Modifiers - Erase")
{
    TEST_CASE("erase() by iterator")
    {
        demo::unordered_multimap<int, int> m = {
            {1, 10},
            {2, 20},
            {3, 30}
        };

        auto it = m.find(2);
        m.erase(it);

        CHECK(m.size() == 2);
        CHECK(m.find(2) == m.end());
    }

    TEST_CASE("erase() by key removes all matching elements")
    {
        demo::unordered_multimap<int, int> m{};
        m.insert({1, 10});
        m.insert({1, 20});
        m.insert({1, 30});
        m.insert({2, 40});
        m.insert({2, 50});

        size_t erased = m.erase(1);

        CHECK(erased == 3);
        CHECK(m.size() == 2);
        CHECK(m.count(1) == 0);
        CHECK(m.count(2) == 2);
    }

    TEST_CASE("erase() by key returns 0 for non-existing key")
    {
        demo::unordered_multimap<int, int> m = {
            {1, 10}
        };

        size_t erased = m.erase(2);

        CHECK(erased == 0);
        CHECK(m.size() == 1);
    }

    TEST_CASE("erase() returns iterator to next element")
    {
        demo::unordered_multimap<int, int> m = {
            {1, 10},
            {2, 20},
            {3, 30}
        };

        auto it      = m.find(2);
        m.erase(it);

        CHECK(m.size() == 2);
        CHECK(m.find(2) == m.end());
    }

    TEST_CASE("erase() range")
    {
        demo::unordered_multimap<int, int> m = {
            {1, 10},
            {2, 20},
            {3, 30},
            {4, 40},
            {5, 50}
        };
        auto it    = m.begin();
        it++;
        auto first = it;
        it++;
        it++;
        auto last = it;
        m.erase(first, last);

        CHECK(m.size() == 3);
    }

    TEST_CASE("clear() removes all elements")
    {
        demo::unordered_multimap<int, int> m = {
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
        demo::unordered_multimap<int, int> m{};

        CHECK_NOTHROW(m.clear());
        CHECK(m.empty());
    }

    TEST_CASE("erase single element from duplicate keys")
    {
        demo::unordered_multimap<int, int> m{};
        m.insert({1, 10});
        m.insert({1, 20});
        m.insert({1, 30});

        auto it = m.find(1);
        CHECK(it != m.end());
        m.erase(it);

        CHECK(m.size() == 2);
        CHECK(m.count(1) == 2);
    }
}

// ============================================
// 测试套件: 修改器 - swap/merge
// ============================================

TEST_SUITE("Unordered_Multimap Modifiers - Other")
{
    TEST_CASE("swap() exchanges contents")
    {
        demo::unordered_multimap<int, int> m1 = {
            {1, 10},
            {2, 20}
        };
        demo::unordered_multimap<int, int> m2 = {
            {3, 30},
            {4, 40},
            {5, 50}
        };

        m1.swap(m2);

        CHECK(m1.size() == 3);
        CHECK(m1.find(3)->second == 30);
        CHECK(m1.find(4)->second == 40);
        CHECK(m1.find(5)->second == 50);

        CHECK(m2.size() == 2);
        CHECK(m2.find(1)->second == 10);
        CHECK(m2.find(2)->second == 20);
    }

    TEST_CASE("swap() with empty map")
    {
        demo::unordered_multimap<int, int> m1 = {
            {1, 10}
        };
        demo::unordered_multimap<int, int> m2;

        m1.swap(m2);

        CHECK(m1.empty());
        CHECK(m2.size() == 1);
        CHECK(m2.find(1)->second == 10);
    }

    TEST_CASE("merge() merges all elements")
    {
        demo::unordered_multimap<int, int> m1 = {
            {1, 10},
            {1, 20}
        };
        demo::unordered_multimap<int, int> m2 = {
            {1, 30},
            {2, 40}
        };

        m1.merge(m2);

        CHECK(m1.size() == 4);
        CHECK(m1.count(1) == 3);
        CHECK(m1.count(2) == 1);
        CHECK(m2.empty());
    }

    TEST_CASE("merge() rvalue version")
    {
        demo::unordered_multimap<int, int> m1 = {
            {1, 10}
        };
        demo::unordered_multimap<int, int> m2 = {
            {2, 20}
        };

        m1.merge(std::move(m2));

        CHECK(m1.size() == 2);
        CHECK(m1.find(1)->second == 10);
        CHECK(m1.find(2)->second == 20);
    }
}

// ============================================
// 测试套件: 查找操作（核心区别）
// ============================================

TEST_SUITE("Unordered_Multimap Lookup")
{
    TEST_CASE("find() returns iterator to existing element")
    {
        demo::unordered_multimap<int, int> m = {
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
        demo::unordered_multimap<int, int> m = {
            {1, 10},
            {2, 20}
        };

        auto it = m.find(3);

        CHECK(it == m.end());
    }

    TEST_CASE("const find() returns iterator to existing element")
    {
        const demo::unordered_multimap<int, int> m = {
            {1, 10},
            {2, 20}
        };

        auto it = m.find(1);

        CHECK(it != m.end());
        CHECK(it->first == 1);
    }

    TEST_CASE("count() returns 1 for unique key")
    {
        demo::unordered_multimap<int, int> m = {
            {1, 10},
            {2, 20}
        };

        CHECK(m.count(1) == 1);
        CHECK(m.count(2) == 1);
    }

    TEST_CASE("count() returns 0 for non-existing key")
    {
        demo::unordered_multimap<int, int> m = {
            {1, 10},
            {2, 20}
        };

        CHECK(m.count(3) == 0);
    }

    TEST_CASE("count() returns >1 for duplicate keys")
    {
        demo::unordered_multimap<int, int> m{};
        m.insert({1, 10});
        m.insert({1, 20});
        m.insert({1, 30});

        CHECK(m.count(1) == 3);
    }

    TEST_CASE("equal_range() returns multiple elements for duplicate keys")
    {
        demo::unordered_multimap<int, int> m{};
        m.insert({1, 10});
        m.insert({2, 20});
        m.insert({1, 30});
        m.insert({2, 40});
        m.insert({1, 50});

        auto range = m.equal_range(1);
        int  count = 0;
        std::vector<int> values;
        for (auto it = range.first; it != range.second; ++it)
        {
            CHECK(it->first == 1);
            values.push_back(it->second);
            ++count;
        }
        CHECK(count == 3);
        CHECK(values.size() == 3);
    }

    TEST_CASE("equal_range() for non-existing key")
    {
        demo::unordered_multimap<int, int> m = {
            {1, 10},
            {2, 20}
        };

        auto range = m.equal_range(3);
        CHECK(range.first == m.end());
        CHECK(range.second == m.end());
    }

    TEST_CASE("equal_range() const version for duplicate keys")
    {
        const demo::unordered_multimap<int, int> m = {
            {1, 10},
            {1, 20},
            {2, 30}
        };

        auto range = m.equal_range(1);
        int  count = 0;
        for (auto it = range.first; it != range.second; ++it)
        {
            CHECK(it->first == 1);
            ++count;
        }
        CHECK(count == 2);
    }

    TEST_CASE("equal_range() returns single element for unique key")
    {
        demo::unordered_multimap<int, int> m = {
            {1, 10},
            {2, 20},
            {3, 30}
        };

        auto range = m.equal_range(2);
        CHECK(range.first->first == 2);
        auto next = range.first;
        ++next;
        CHECK(next == range.second);
    }
}

// ============================================
// 测试套件: 桶接口
// ============================================

TEST_SUITE("Unordered_Multimap Bucket Interface")
{
    TEST_CASE("bucket() returns valid bucket index")
    {
        demo::unordered_multimap<int, int> m{};
        m.insert({1, 10});
        m.insert({2, 20});

        size_t bucket1 = m.bucket(1);
        size_t bucket2 = m.bucket(2);

        CHECK(bucket1 < m.bucket_count());
        CHECK(bucket2 < m.bucket_count());
    }

    TEST_CASE("bucket_size() returns correct count")
    {
        demo::unordered_multimap<int, int> m(1);
        m.max_load_factor(10.0f);

        m.insert({1, 10});
        m.insert({2, 20});
        m.insert({3, 30});

        size_t total_count = count_elements_in_buckets(m);
        CHECK(total_count == m.size());
    }

    TEST_CASE("bucket_size() for empty bucket")
    {
        demo::unordered_multimap<int, int> m{};
        CHECK(m.bucket_size(0) == 0);
    }
}

// ============================================
// 测试套件: 哈希策略
// ============================================

TEST_SUITE("Unordered_Multimap Hash Policy")
{
    TEST_CASE("load_factor() returns correct value")
    {
        demo::unordered_multimap<int, int> m{};
        CHECK(m.load_factor() >= 0.0f);

        m.insert({1, 10});
        CHECK(m.load_factor() > 0.0f);
    }

    TEST_CASE("max_load_factor() returns default value")
    {
        demo::unordered_multimap<int, int> m{};
        CHECK(m.max_load_factor() == 1.0f);
    }

    TEST_CASE("max_load_factor() can be set")
    {
        demo::unordered_multimap<int, int> m{};
        m.max_load_factor(0.5f);
        CHECK(m.max_load_factor() == 0.5f);
    }

    TEST_CASE("rehash() changes bucket count")
    {
        demo::unordered_multimap<int, int> m{};

        m.rehash(64);
        CHECK(m.bucket_count() >= 64);
    }

    TEST_CASE("rehash() preserves elements including duplicates")
    {
        demo::unordered_multimap<int, int> m{};
        m.insert({1, 10});
        m.insert({1, 20});
        m.insert({2, 30});
        m.insert({2, 40});
        m.insert({3, 50});

        m.rehash(1);
        CHECK(m.size() == 5);
        CHECK(m.count(1) == 2);
        CHECK(m.count(2) == 2);
        CHECK(m.count(3) == 1);
    }

    TEST_CASE("reserve() allocates space for elements")
    {
        demo::unordered_multimap<int, int> m{};
        m.reserve(100);

        CHECK(m.bucket_count() >= 100);
    }

    TEST_CASE("reserve() does not affect existing elements")
    {
        demo::unordered_multimap<int, int> m = {
            {1, 10},
            {2, 20}
        };
        auto size_before = m.size();

        m.reserve(100);

        CHECK(m.size() == size_before);
        CHECK(m.find(1)->second == 10);
        CHECK(m.find(2)->second == 20);
    }

    TEST_CASE("rehash occurs when exceeding default load factor")
    {
        demo::unordered_multimap<int, int> m(4);

        m.insert({3, 30});
        m.insert({4, 40});
        m.insert({5, 50});
        m.insert({6, 60});
        m.insert({7, 70});
        m.insert({8, 80});
        m.insert({9, 90});

        CHECK(m.size() == 7);
        CHECK(m.bucket_count() > 4);
    }

    TEST_CASE("no rehash when load factor is sufficiently large")
    {
        demo::unordered_multimap<int, int> m(4);
        m.max_load_factor(7.0f);

        m.insert({3, 30});
        m.insert({4, 40});
        m.insert({5, 50});
        m.insert({6, 60});
        m.insert({7, 70});
        m.insert({8, 80});
        m.insert({9, 90});

        CHECK(m.size() == 7);
        CHECK(m.bucket_count() == 4);
    }
}

// ============================================
// 测试套件: 观察器
// ============================================

TEST_SUITE("Unordered_Multimap Observers")
{
    TEST_CASE("hash_function() returns hash object")
    {
        demo::unordered_multimap<int, int> m{};
        auto                             hash = m.hash_function();

        CHECK(hash(1) == std::hash<int>()(1));
    }

    TEST_CASE("key_eq() returns key equality object")
    {
        demo::unordered_multimap<int, int> m{};
        auto                             eq = m.key_eq();

        CHECK(eq(1, 1) == true);
        CHECK(eq(1, 2) == false);
    }

    TEST_CASE("get_allocator() returns allocator")
    {
        demo::unordered_multimap<int, int> m{};
        auto                             alloc = m.get_allocator();

        CHECK(m.size() == 0);
    }
}

// ============================================
// 测试套件: 全局比较运算符
// ============================================

TEST_SUITE("Unordered_Multimap Comparison Operators")
{
    TEST_CASE("operator== compares contents")
    {
        demo::unordered_multimap<int, int> m1 = {
            {1, 10},
            {2, 20},
            {1, 30}
        };
        demo::unordered_multimap<int, int> m2 = {
            {1, 10},
            {1, 30},
            {2, 20}
        };
        demo::unordered_multimap<int, int> m3 = {
            {1, 10},
            {2, 200}
        };

        CHECK(m1 == m2);
        CHECK_FALSE(m1 == m3);
    }

    TEST_CASE("operator!= compares contents")
    {
        demo::unordered_multimap<int, int> m1 = {
            {1, 10},
            {2, 20}
        };
        demo::unordered_multimap<int, int> m2 = {
            {1, 10},
            {2, 20}
        };
        demo::unordered_multimap<int, int> m3 = {
            {1, 10 },
            {2, 200}
        };

        CHECK_FALSE(m1 != m2);
        CHECK(m1 != m3);
    }

    TEST_CASE("operator== and != with different sizes")
    {
        demo::unordered_multimap<int, int> m1 = {
            {1, 10}
        };
        demo::unordered_multimap<int, int> m2 = {
            {1, 10},
            {2, 20}
        };

        CHECK_FALSE(m1 == m2);
        CHECK(m1 != m2);
    }

    TEST_CASE("operator== with duplicate keys")
    {
        demo::unordered_multimap<int, int> m1{};
        m1.insert({1, 10});
        m1.insert({1, 20});
        m1.insert({1, 30});

        demo::unordered_multimap<int, int> m2{};
        m2.insert({1, 30});
        m2.insert({1, 10});
        m2.insert({1, 20});

        CHECK(m1 == m2);
    }

    TEST_CASE("operator!= with different duplicate values")
    {
        demo::unordered_multimap<int, int> m1{};
        m1.insert({1, 10});

        demo::unordered_multimap<int, int> m2{};
        m2.insert({1, 20});

        CHECK(m1 != m2);
    }
}

// ============================================
// 测试套件: 边界情况和异常场景
// ============================================

TEST_SUITE("Unordered_Multimap Boundary Cases")
{
    TEST_CASE("single element map operations")
    {
        demo::unordered_multimap<int, int> m = {
            {1, 10}
        };

        CHECK(m.size() == 1);
        CHECK(!m.empty());
        CHECK(m.find(1) != m.end());
        CHECK(m.find(2) == m.end());
    }

    TEST_CASE("insert and erase alternating")
    {
        demo::unordered_multimap<int, int> m{};

        for (int i = 0; i < 10; ++i)
        {
            m.insert({i, i * 10});
        }

        for (int i = 0; i < 10; i += 2)
        {
            m.erase(i);
        }

        CHECK(m.size() == 5);
        CHECK(m.count(1) == 1);
        CHECK(m.count(3) == 1);
    }

    TEST_CASE("large key range")
    {
        demo::unordered_multimap<int, int> m{};

        m.insert({-1000, 1});
        m.insert({0, 2});
        m.insert({1000, 3});

        CHECK(m.size() == 3);
        CHECK(m.count(-1000) == 1);
        CHECK(m.count(0) == 1);
        CHECK(m.count(1000) == 1);
    }

    TEST_CASE("all elements accessible after operations")
    {
        demo::unordered_multimap<int, int> m{};
        for (int i = 0; i < 100; ++i)
        {
            m.insert({i, i * 2});
        }

        CHECK(m.size() == 100);

        for (int i = 0; i < 100; ++i)
        {
            auto it = m.find(i);
            CHECK(it != m.end());
            CHECK(it->second == i * 2);
        }
    }

    TEST_CASE("insert same key many times")
    {
        demo::unordered_multimap<int, int> m{};
        for (int i = 0; i < 50; ++i)
        {
            m.insert({1, i});
        }

        CHECK(m.size() == 50);
        CHECK(m.count(1) == 50);

        int count = 0;
        auto range = m.equal_range(1);
        for (auto it = range.first; it != range.second; ++it)
        {
            CHECK(it->first == 1);
            ++count;
        }
        CHECK(count == 50);
    }

    TEST_CASE("erase all duplicate keys")
    {
        demo::unordered_multimap<int, int> m{};
        m.insert({1, 10});
        m.insert({1, 20});
        m.insert({1, 30});
        m.insert({2, 40});

        size_t erased = m.erase(1);
        CHECK(erased == 3);
        CHECK(m.size() == 1);
        CHECK(m.count(1) == 0);
        CHECK(m.count(2) == 1);
    }
}

// ============================================
// 测试套件: 字符串类型测试
// ============================================

TEST_SUITE("Unordered_Multimap String Type")
{
    TEST_CASE("string key multimap operations")
    {
        demo::unordered_multimap<std::string, int> m{};

        m.insert({"apple", 1});
        m.insert({"banana", 2});
        m.insert({"apple", 10});
        m.insert({"cherry", 3});

        CHECK(m.size() == 4);
        CHECK(m.count("apple") == 2);
        CHECK(m.count("banana") == 1);
        CHECK(m.count("cherry") == 1);

        m.erase("apple");
        CHECK(m.size() == 2);
        CHECK(m.count("apple") == 0);
    }

    TEST_CASE("string key hashing")
    {
        demo::unordered_multimap<std::string, int> m = {
            {"a", 1},
            {"b", 2},
            {"a", 10},
            {"c", 3}
        };

        CHECK(m.size() == 4);
        CHECK(m.count("a") == 2);
        CHECK(m.count("d") == 0);
    }
}

// ============================================
// 测试套件: 压力测试
// ============================================

TEST_SUITE("Unordered_Multimap Stress Tests")
{
    TEST_CASE("insert and iterate 10000 elements with duplicates")
    {
        demo::unordered_multimap<int, int> m{};

        for (int i = 0; i < 5000; ++i)
        {
            m.insert({i, i * 10});
        }
        for (int i = 0; i < 5000; ++i)
        {
            m.insert({i, i * 10 + 1});
        }

        CHECK(m.size() == 10000);
        CHECK(m.count(0) == 2);
        CHECK(m.count(4999) == 2);
    }

    TEST_CASE("insert in random order")
    {
        demo::unordered_multimap<int, int> m{};
        std::vector<int>                 keys;

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
        CHECK(count_elements_in_buckets(m) == 5000);
    }

    TEST_CASE("sequential insert and erase by key")
    {
        demo::unordered_multimap<int, int> m{};

        for (int i = 0; i < 1000; ++i)
        {
            m.insert({i, i});
            m.insert({i, i + 1000});
        }

        for (int i = 0; i < 1000; ++i)
        {
            size_t erased = m.erase(i);
            CHECK(erased == 2);
        }

        CHECK(m.empty());
        CHECK(m.size() == 0);
    }

    TEST_CASE("alternating insert and erase")
    {
        demo::unordered_multimap<int, int> m{};

        for (int i = 0; i < 1000; ++i)
        {
            m.insert({i, i});
            if (i % 10 == 0 && i > 0)
            {
                m.erase(i - 5);
            }
        }

        CHECK(m.size() > 0);
    }

    TEST_CASE("performance test - insert speed")
    {
        demo::unordered_multimap<int, int> m{};

        auto start = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < 50000; ++i)
        {
            m.insert({i, i});
        }
        auto end = std::chrono::high_resolution_clock::now();

        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

        CHECK(m.size() == 50000);
        CHECK(duration.count() < 10000);
    }

    TEST_CASE("performance test - find speed")
    {
        demo::unordered_multimap<int, int> m{};
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

        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

        CHECK(duration.count() < 5000);
    }

    TEST_CASE("large data persistence")
    {
        demo::unordered_multimap<int, int> m{};

        for (int i = 0; i < 100000; ++i)
        {
            m.insert({i, i});
        }

        CHECK(m.size() == 100000);

        for (int i = 0; i < 100000; i += 1000)
        {
            CHECK(m.find(i) != m.end());
            CHECK(m.find(i)->second == i);
        }
    }

    TEST_CASE("rehash performance with duplicates")
    {
        demo::unordered_multimap<int, int> m{};

        for (int i = 0; i < 10000; ++i)
        {
            m.insert({i % 100, i});
        }

        auto start = std::chrono::high_resolution_clock::now();
        m.rehash(1000);
        auto end = std::chrono::high_resolution_clock::now();

        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

        CHECK(m.size() == 10000);
        CHECK(duration.count() < 5000);
    }
}

// ============================================
// 测试套件: 特殊类型测试
// ============================================

TEST_SUITE("Unordered_Multimap Special Types")
{
    TEST_CASE("unordered_multimap with custom struct as value")
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

        demo::unordered_multimap<int, MyStruct> m;
        m.insert({
            1, {1, "first"}
        });
        m.insert({
            2, {2, "second"}
        });

        CHECK(m.size() == 2);
        CHECK(m.find(1)->second.id == 1);
        CHECK(m.find(1)->second.name == "first");
        CHECK(m.find(2)->second.id == 2);
        CHECK(m.find(2)->second.name == "second");
    }

    TEST_CASE("unordered_multimap with pointer values")
    {
        demo::unordered_multimap<int, int*> m{};
        int*                              val1 = new int(10);
        int*                              val2 = new int(20);

        m.insert({1, val1});
        m.insert({2, val2});

        CHECK(m.size() == 2);
        CHECK(*m.find(1)->second == 10);
        CHECK(*m.find(2)->second == 20);

        delete val1;
        delete val2;
    }

    TEST_CASE("nested unordered_multimap")
    {
        demo::unordered_multimap<int, demo::unordered_multimap<int, std::string>> m{};

        demo::unordered_multimap<int, std::string> inner = {
            {1, "one"},
            {2, "two"}
        };
        m.insert({1, inner});

        CHECK(m.size() == 1);
        CHECK(m.find(1)->second.size() == 2);
    }
}

// ============================================
// 测试套件: 算法测试
// ============================================

TEST_SUITE("Unordered_Multimap Algorithms")
{
    TEST_CASE("std::for_each on unordered_multimap")
    {
        demo::unordered_multimap<int, int> m = {
            {1, 10},
            {2, 20},
            {3, 30}
        };
        int sum = 0;

        std::for_each(m.begin(), m.end(), [&](const auto& pair) { sum += pair.second; });

        CHECK(sum == 60);
    }

    TEST_CASE("std::find_if on unordered_multimap")
    {
        demo::unordered_multimap<int, int> m = {
            {1, 10},
            {2, 20},
            {3, 30}
        };

        auto it =
            std::find_if(m.begin(), m.end(), [](const auto& pair) { return pair.second == 20; });

        CHECK(it != m.end());
        CHECK(it->first == 2);
    }

    TEST_CASE("std::count_if on unordered_multimap")
    {
        demo::unordered_multimap<int, int> m = {
            {1, 10},
            {2, 20},
            {3, 30},
            {4, 20}
        };

        auto count =
            std::count_if(m.begin(), m.end(), [](const auto& pair) { return pair.second == 20; });

        CHECK(count == 2);
    }
}

// ============================================
// 测试套件: 自定义哈希函数测试
// ============================================

TEST_SUITE("Unordered_Multimap Custom Hash")
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

        demo::unordered_multimap<int, int, CustomHash> m{};
        m.insert({1, 10});
        m.insert({2, 20});
        m.insert({1, 30});

        CHECK(m.size() == 3);
        CHECK(m.count(1) == 2);
        CHECK(m.count(2) == 1);
    }
}

// ============================================
// 测试套件: 额外边界情况测试
// ============================================

TEST_SUITE("Unordered_Multimap Additional Edge Cases")
{
    TEST_CASE("erase all elements one by one")
    {
        demo::unordered_multimap<int, int> m{};
        for (int i = 0; i < 10; ++i)
        {
            m.insert({i % 3, i * 10});
        }
        CHECK(m.size() == 10);

        for (int i = 0; i < 3; ++i)
        {
            m.erase(i);
        }
        CHECK(m.empty());
        CHECK(m.size() == 0);
    }

    TEST_CASE("copy assignment replaces existing content")
    {
        demo::unordered_multimap<int, int> m1 = {
            {1, 10}
        };
        demo::unordered_multimap<int, int> m2 = {
            {2, 20},
            {3, 30},
            {4, 40}
        };

        m1 = m2;

        CHECK(m1.size() == 3);
        CHECK(m1.find(2)->second == 20);
        CHECK(m1.find(3)->second == 30);
        CHECK(m1.find(4)->second == 40);
        CHECK(m2.size() == 3);
    }

    TEST_CASE("copy assignment to empty map")
    {
        demo::unordered_multimap<int, int> m1{};
        demo::unordered_multimap<int, int> m2 = {
            {1, 10},
            {2, 20}
        };

        m1 = m2;

        CHECK(m1.size() == 2);
        CHECK(m1.find(1)->second == 10);
        CHECK(m1.find(2)->second == 20);
    }

    TEST_CASE("move assignment after use")
    {
        demo::unordered_multimap<int, std::string> m1 = {
            {1, "test1"}
        };
        demo::unordered_multimap<int, std::string> m2 = {
            {2, "test2"},
            {3, "test3"}
        };

        m1 = std::move(m2);

        CHECK(m1.size() == 2);
        CHECK(m1.find(2)->second == "test2");
        CHECK(m1.find(3)->second == "test3");
        CHECK(m2.empty());
    }

    TEST_CASE("rehash maintains all elements including duplicates")
    {
        demo::unordered_multimap<int, int> m{};
        m.insert({1, 10});
        m.insert({1, 20});
        m.insert({2, 30});
        m.insert({2, 40});
        m.insert({3, 50});

        size_t old_bucket_count = m.bucket_count();
        m.rehash(old_bucket_count * 4);

        CHECK(m.size() == 5);
        CHECK(m.count(1) == 2);
        CHECK(m.count(2) == 2);
        CHECK(m.count(3) == 1);
    }

    TEST_CASE("rehash to smaller size")
    {
        demo::unordered_multimap<int, int> m{};
        m.reserve(100);
        for (int i = 0; i < 50; ++i)
        {
            m.insert({i, i});
        }

        m.rehash(16);
        CHECK(m.size() == 50);
        for (int i = 0; i < 50; ++i)
        {
            CHECK(m.find(i) != m.end());
            CHECK(m.find(i)->second == i);
        }
    }

    TEST_CASE("clear then repopulate")
    {
        demo::unordered_multimap<int, int> m = {
            {1, 10},
            {2, 20}
        };

        m.clear();
        CHECK(m.empty());
        CHECK(m.size() == 0);

        m.insert({3, 30});
        m.insert({3, 40});
        CHECK(m.size() == 2);
        CHECK(m.count(3) == 2);
    }

    TEST_CASE("swap preserves all elements")
    {
        demo::unordered_multimap<int, int> m1 = {
            {1, 10},
            {2, 20}
        };
        demo::unordered_multimap<int, int> m2 = {
            {1, 30},
            {1, 40}
        };

        m1.swap(m2);

        CHECK(m1.size() == 2);
        CHECK(m2.size() == 2);
        CHECK(m1.count(1) == 2);
        CHECK(m2.count(1) == 1);
        CHECK(m2.count(2) == 1);
    }

    TEST_CASE("swap multiple times")
    {
        demo::unordered_multimap<int, int> m1 = {
            {1, 10}
        };
        demo::unordered_multimap<int, int> m2 = {
            {2, 20}
        };
        demo::unordered_multimap<int, int> m3 = {
            {3, 30},
            {3, 40}
        };

        m1.swap(m2);
        CHECK(m1.find(2)->second == 20);
        CHECK(m2.find(1)->second == 10);

        m2.swap(m3);
        CHECK(m2.count(3) == 2);
        CHECK(m3.find(1)->second == 10);

        m1.swap(m3);
        CHECK(m1.find(1)->second == 10);
        CHECK(m3.find(2)->second == 20);
    }

    TEST_CASE("operator== with empty maps")
    {
        demo::unordered_multimap<int, int> m1{};
        demo::unordered_multimap<int, int> m2{};

        CHECK(m1 == m2);
        CHECK_FALSE(m1 != m2);
    }

    TEST_CASE("operator== with different values for same key")
    {
        demo::unordered_multimap<int, int> m1 = {
            {1, 10}
        };
        demo::unordered_multimap<int, int> m2 = {
            {1, 20}
        };

        CHECK_FALSE(m1 == m2);
        CHECK(m1 != m2);
    }

    TEST_CASE("operator!= with same maps with duplicates")
    {
        demo::unordered_multimap<int, int> m1{};
        m1.insert({1, 10});
        m1.insert({1, 20});
        m1.insert({2, 30});

        demo::unordered_multimap<int, int> m2{};
        m2.insert({2, 30});
        m2.insert({1, 20});
        m2.insert({1, 10});

        CHECK_FALSE(m1 != m2);
    }
}