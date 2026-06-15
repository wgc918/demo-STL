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

#include "../unordered_map.h"
#include "doctest.h"

// ============================================
// 测试unordered_map的equal_range方法
// ============================================

template <typename K, typename T>
std::vector<std::pair<K, T>> unordered_map_to_vector(const demo::unordered_map<K, T>& mp)
{
    std::vector<std::pair<K, T>> result;
    for (const auto& elem : mp)
    {
        result.push_back(elem);
    }
    return result;
}

template <typename K, typename T>
bool unordered_maps_equal(const demo::unordered_map<K, T>& a, const demo::unordered_map<K, T>& b)
{
    if (a.size() != b.size())
        return false;

    for (const auto& elem : a)
    {
        auto it = b.find(elem.first);
        if (it == b.end() || it->second != elem.second)
            return false;
    }
    return true;
}

template <typename K, typename T>
size_t count_elements_in_buckets(const demo::unordered_map<K, T>& mp)
{
    size_t count = 0;
    for (size_t i = 0; i < mp.bucket_count(); ++i)
    {
        count += mp.bucket_size(i);
    }
    return count;
}

#define UM_INT_INT demo::unordered_map<int, int>
#define UM_STR_INT demo::unordered_map<std::string, int>

// ============================================
// 测试套件: Node结构测试
// ============================================

TEST_SUITE("Unordered_Map Node Structure")
{
    TEST_CASE("Node default constructor initializes correctly")
    {
        demo::unordered_map<int, int> m{};
        CHECK(m.empty());
        CHECK(m.size() == 0);
    }

    TEST_CASE("Node stores key-value pairs correctly")
    {
        demo::unordered_map<int, std::string> m{};
        m.insert({1, "one"});
        CHECK(m.size() == 1);
        CHECK(m[1] == "one");
    }

    TEST_CASE("Node maintains hash chain relationships")
    {
        demo::unordered_map<int, int> m{};
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
        demo::unordered_map<int, int> m{};
        m.insert({1, 10});
        m.insert({2, 20});
        m.insert({3, 30});
        m.erase(2);
        CHECK(m.size() == 2);
        CHECK(m.find(2) == m.end());
    }
}

// ============================================
// 测试套件: 构造函数
// ============================================

TEST_SUITE("Unordered_Map Constructors")
{
    TEST_CASE("Default constructor creates empty map")
    {
        demo::unordered_map<int, int> m{};
        CHECK(m.empty());
        CHECK(m.size() == 0);
        CHECK(m.begin() == m.end());
    }

    TEST_CASE("Constructor with bucket count")
    {
        demo::unordered_map<int, int> m(32);
        CHECK(m.bucket_count() == 32);
        CHECK(m.empty());

        demo::unordered_map<int, int> n(31);
        CHECK(n.bucket_count() >= 31);
        CHECK(n.empty());
    }

    TEST_CASE("Initializer list constructor")
    {
        demo::unordered_map<int, std::string> m = {
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
        demo::unordered_map<int, int> m(vec.begin(), vec.end());
        CHECK(m.size() == 3);
        CHECK(m[1] == 10);
        CHECK(m[2] == 20);
        CHECK(m[3] == 30);
    }

    TEST_CASE("Copy constructor")
    {
        demo::unordered_map<int, std::string> m1 = {
            {1, "one"},
            {2, "two"}
        };
        demo::unordered_map<int, std::string> m2(m1);

        CHECK(m2.size() == 2);
        CHECK(m2[1] == "one");
        CHECK(m2[2] == "two");
        CHECK(unordered_maps_equal(m1, m2));
        CHECK(m1.size() == 2);
    }

    TEST_CASE("Move constructor")
    {
        demo::unordered_map<int, std::string> m1 = {
            {1, "one"},
            {2, "two"}
        };
        auto size_before = m1.size();

        demo::unordered_map<int, std::string> m2(std::move(m1));

        CHECK(m2.size() == size_before);
        CHECK(m2[1] == "one");
        CHECK(m2[2] == "two");
        CHECK(m1.empty());
        CHECK(m1.size() == 0);
    }
}

// ============================================
// 测试套件: 赋值运算符
// ============================================

TEST_SUITE("Unordered_Map Assignment Operators")
{
    TEST_CASE("Copy assignment operator")
    {
        demo::unordered_map<int, std::string> m1 = {
            {1, "one"},
            {2, "two"}
        };
        demo::unordered_map<int, std::string> m2{};

        m2 = m1;

        CHECK(m2.size() == 2);
        CHECK(m2[1] == "one");
        CHECK(m2[2] == "two");
        CHECK(unordered_maps_equal(m1, m2));
    }

    TEST_CASE("Move assignment operator")
    {
        demo::unordered_map<int, std::string> m1 = {
            {1, "one"},
            {2, "two"}
        };
        auto                                  size_before = m1.size();
        demo::unordered_map<int, std::string> m2{};

        m2 = std::move(m1);

        CHECK(m2.size() == size_before);
        CHECK(m2[1] == "one");
        CHECK(m2[2] == "two");
        CHECK(m1.empty());
    }

    TEST_CASE("Initializer list assignment")
    {
        demo::unordered_map<int, std::string> m{};
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
        demo::unordered_map<int, std::string> m = {
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

TEST_SUITE("Unordered_Map Element Access")
{
    TEST_CASE("operator[] inserts and accesses element")
    {
        demo::unordered_map<int, std::string> m{};
        m[1] = "one";
        m[2] = "two";

        CHECK(m.size() == 2);
        CHECK(m[1] == "one");
        CHECK(m[2] == "two");
    }

    TEST_CASE("operator[] with rvalue key")
    {
        demo::unordered_map<std::string, int> m{};
        m[std::string("key1")] = 10;

        CHECK(m.size() == 1);
        CHECK(m["key1"] == 10);
    }

    TEST_CASE("at() returns existing element")
    {
        demo::unordered_map<int, std::string> m = {
            {1, "one"},
            {2, "two"}
        };

        CHECK(m.at(1) == "one");
        CHECK(m.at(2) == "two");
    }

    TEST_CASE("at() throws for non-existing key")
    {
        demo::unordered_map<int, std::string> m = {
            {1, "one"}
        };

        CHECK_THROWS_AS(m.at(2), std::out_of_range);
    }

    TEST_CASE("const at() returns existing element")
    {
        const demo::unordered_map<int, std::string> m = {
            {1, "one"},
            {2, "two"}
        };

        CHECK(m.at(1) == "one");
        CHECK(m.at(2) == "two");
    }

    TEST_CASE("operator[] modifies existing element")
    {
        demo::unordered_map<int, std::string> m = {
            {1, "one"  },
            {2, "two"  },
            {3, "three"}
        };
        m[1] = "modified";
        m[2] = "modified";
        m[3] = "modified";
        CHECK(m.size() == 3);
        m[0] = "modified";
        CHECK(m[1] == "modified");
        CHECK(m[2] == "modified");
        CHECK(m[3] == "modified");
        CHECK(m[0] == "modified");
        CHECK(m.size() == 4);
    }
}

// ============================================
// 测试套件: 迭代器
// ============================================

TEST_SUITE("Unordered_Map Iterators")
{
    TEST_CASE("begin() and end() iterators")
    {
        demo::unordered_map<int, int> m = {
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
        const demo::unordered_map<int, int> m = {
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
        demo::unordered_map<int, int> m = {
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
        demo::unordered_map<int, int> m = {
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

    TEST_CASE("iterator post-increment and post-decrement")
    {
        demo::unordered_map<int, int> m = {
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
        demo::unordered_map<int, int> m = {
            {1, 10}
        };
        auto it = m.begin();

        auto& pair = *it;
        CHECK(pair.first == 1);
        CHECK(pair.second == 10);
    }

    TEST_CASE("iterator arrow operator")
    {
        demo::unordered_map<int, int> m = {
            {1, 10}
        };
        auto it = m.begin();

        CHECK(it->first == 1);
        CHECK(it->second == 10);
    }

    TEST_CASE("empty map iterators")
    {
        demo::unordered_map<int, int> m{};

        CHECK(m.begin() == m.end());
        CHECK(m.cbegin() == m.cend());
    }

    TEST_CASE("iterator conversion to const_iterator")
    {
        demo::unordered_map<int, int> m = {
            {1, 10}
        };
        demo::unordered_map<int, int>::iterator       it       = m.begin();
        demo::unordered_map<int, int>::const_iterator const_it = it;

        CHECK(const_it->first == 1);
        CHECK(const_it->second == 10);
    }
}

// ============================================
// 测试套件: 桶迭代器
// ============================================

TEST_SUITE("Unordered_Map Bucket Iterators")
{
    TEST_CASE("local_iterator begin and end")
    {
        demo::unordered_map<int, int> m{};
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
        const demo::unordered_map<int, int> m = {
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
        demo::unordered_map<int, int> m(64);
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

TEST_SUITE("Unordered_Map Capacity")
{
    TEST_CASE("empty() returns true for empty map")
    {
        demo::unordered_map<int, int> m{};
        CHECK(m.empty());
    }

    TEST_CASE("empty() returns false for non-empty map")
    {
        demo::unordered_map<int, int> m = {
            {1, 10}
        };
        CHECK_FALSE(m.empty());
    }

    TEST_CASE("size() returns correct number of elements")
    {
        demo::unordered_map<int, int> m{};

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
        demo::unordered_map<int, int> m{};
        CHECK(m.max_size() > 0);
    }

    TEST_CASE("bucket_count() returns valid value")
    {
        demo::unordered_map<int, int> m{};
        CHECK(m.bucket_count() >= 16);
    }

    TEST_CASE("max_bucket_count() returns positive value")
    {
        demo::unordered_map<int, int> m{};
        CHECK(m.max_bucket_count() > 0);
    }
}

// ============================================
// 测试套件: 修改器 - 插入操作
// ============================================

TEST_SUITE("Unordered_Map Modifiers - Insert")
{
    TEST_CASE("insert() with const value")
    {
        demo::unordered_map<int, std::string> m{};
        std::pair<int, std::string>           val = {1, "one"};

        m.insert(val);

        CHECK(m.size() == 1);
        CHECK(m[1] == "one");
    }

    TEST_CASE("insert() with rvalue")
    {
        demo::unordered_map<int, std::string> m{};

        m.insert({1, "one"});

        CHECK(m.size() == 1);
        CHECK(m[1] == "one");
    }

    TEST_CASE("insert() returns pair with iterator and bool")
    {
        demo::unordered_map<int, std::string> m{};

        auto result = m.insert({1, "one"});
        CHECK(result.second == true);
        CHECK(result.first->first == 1);

        result = m.insert({1, "two"});
        CHECK(result.second == false);
        CHECK(result.first->second == "one");
    }

    TEST_CASE("insert() with hint position")
    {
        demo::unordered_map<int, int> m = {
            {1, 10},
            {3, 30}
        };
        CHECK(m.size() == 2);
        auto hint = m.find(1);
        CHECK(hint != m.end());
        auto result = m.insert(hint, {2, 20});
        CHECK(m.size() == 3);
        CHECK(m[2] == 20);
    }

    TEST_CASE("insert() with iterator range")
    {
        std::vector<std::pair<int, int>> vec = {
            {1, 10},
            {2, 20},
            {3, 30}
        };
        demo::unordered_map<int, int> m{};

        m.insert(vec.begin(), vec.end());

        CHECK(m.size() == 3);
        CHECK(m[1] == 10);
        CHECK(m[2] == 20);
        CHECK(m[3] == 30);
    }

    TEST_CASE("insert() with initializer list")
    {
        demo::unordered_map<int, int> m{};

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
        demo::unordered_map<int, std::string> m{};

        auto result = m.insert_or_assign(1, "one");
        CHECK(result.second == true);
        CHECK(m[1] == "one");
    }

    TEST_CASE("insert_or_assign() assigns to existing element")
    {
        demo::unordered_map<int, std::string> m = {
            {1, "one"}
        };

        auto result = m.insert_or_assign(1, "modified");
        CHECK(result.second == false);
        CHECK(m[1] == "modified");
        CHECK(m.size() == 1);
    }

    TEST_CASE("emplace() inserts new element")
    {
        demo::unordered_map<int, std::string> m{};

        auto result = m.emplace(1, "one");
        CHECK(result.second == true);
        CHECK(m[1] == "one");
    }

    TEST_CASE("emplace() does not insert for existing key")
    {
        demo::unordered_map<int, std::string> m = {
            {1, "original"}
        };

        auto result = m.emplace(1, "new_value");
        CHECK(result.second == false);
        CHECK(m[1] == "original");
        CHECK(m.size() == 1);
    }

    TEST_CASE("try_emplace() inserts new element")
    {
        demo::unordered_map<int, std::string> m{};

        auto result = m.try_emplace(1, "one");
        CHECK(result.second == true);
        CHECK(m[1] == "one");
    }

    TEST_CASE("try_emplace() does not modify for existing key")
    {
        demo::unordered_map<int, std::string> m = {
            {1, "original"}
        };

        auto result = m.try_emplace(1, "new_value");
        CHECK(result.second == false);
        CHECK(m[1] == "original");
        CHECK(m.size() == 1);
    }

    TEST_CASE("insert duplicate key")
    {
        demo::unordered_map<int, int> m{};

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

TEST_SUITE("Unordered_Map Modifiers - Erase")
{
    TEST_CASE("erase() by iterator")
    {
        demo::unordered_map<int, int> m = {
            {1, 10},
            {2, 20},
            {3, 30}
        };

        auto it = m.find(2);
        m.erase(it);

        CHECK(m.size() == 2);
        CHECK(m.find(2) == m.end());
    }

    TEST_CASE("erase() by key")
    {
        demo::unordered_map<int, int> m = {
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
        demo::unordered_map<int, int> m = {
            {1, 10},
            {2, 20},
            {3, 30}
        };

        auto it      = m.find(2);
        auto next_it = m.erase(it);

        CHECK(m.size() == 2);
        CHECK(m.find(2) == m.end());
    }

    TEST_CASE("erase() range")
    {
        demo::unordered_map<int, int> m = {
            {1, 10},
            {2, 20},
            {3, 30},
            {4, 40},
            {5, 50}
        };

        // unordered_map 是无序容器，元素的存储顺序由哈希函数和桶的分布决定。
        // 而本项目的默认哈希函数是 std::hash
        // 不同平台的标准库实现：
        // Windows(MSVC)：使用自己的标准库实现，哈希算法与 Linux 不同
        // Ubuntu(GCC / libstdc++)：使用 GNU 的标准库实现
        // 该测试用例想验证迭代器范围删除，所以只有分两种情况
#ifdef _WIN32
        for (auto it = m.begin(); it != m.end(); ++it)
        {
            std::cout << it->first << std::endl;
        }
        auto it = m.begin();  // 5
        it++;                 // 4
        auto first = it;      // 4
        it++;                 // 1
        it++;                 // 3
        auto last = it;       // 3
        // 会删除4,1这2个元素
        m.erase(first, last);

        std::cout << "====================" << std::endl;
        for (auto it = m.begin(); it != m.end(); ++it)
        {
            std::cout << it->first << std::endl;  // 输出 5 4 1 3 2
        }

        CHECK(m.size() == 3);
        CHECK(m.find(2) != m.end());
        CHECK(m.find(3) != m.end());
        CHECK(m.find(1) == m.end());
        CHECK(m.find(4) == m.end());
        CHECK(m.find(5) != m.end());

#else
        for (auto it = m.begin(); it != m.end(); ++it)
        {
            std::cout << it->first << std::endl;  // 输出 1 2 3 4 5
        }
        auto it = m.begin();  // 1
        it++;                 // 2
        auto first = it;      // 2
        it++;                 // 3
        it++;                 // 4
        auto last = it;       // 4
        // 会删除2，3这2个元素
        m.erase(first, last);

        std::cout << "====================" << std::endl;
        for (auto it = m.begin(); it != m.end(); ++it)
        {
            std::cout << it->first << std::endl;
        }

        CHECK(m.size() == 3);
        CHECK(m.find(2) == m.end());
        CHECK(m.find(3) == m.end());
        CHECK(m.find(1) != m.end());
        CHECK(m.find(4) != m.end());
        CHECK(m.find(5) != m.end());

#endif
    }

    TEST_CASE("clear() removes all elements")
    {
        demo::unordered_map<int, int> m = {
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
        std::cout << "clear() on empty map" << std::endl;
        demo::unordered_map<int, int> m{};

        CHECK_NOTHROW(m.clear());
        CHECK(m.empty());
    }

    TEST_CASE("erase non-existing key")
    {
        demo::unordered_map<int, int> m = {
            {1, 10}
        };

        auto count = m.erase(2);

        CHECK(count == 0);
        CHECK(m.size() == 1);
    }

    TEST_CASE("erase single element")
    {
        demo::unordered_map<int, int> m = {
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

TEST_SUITE("Unordered_Map Modifiers - Other")
{
    TEST_CASE("swap() exchanges contents")
    {
        demo::unordered_map<int, int> m1 = {
            {1, 10},
            {2, 20}
        };
        demo::unordered_map<int, int> m2 = {
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
        demo::unordered_map<int, int> m1 = {
            {1, 10}
        };
        demo::unordered_map<int, int> m2;

        m1.swap(m2);

        CHECK(m1.empty());
        CHECK(m2.size() == 1);
        CHECK(m2[1] == 10);
    }
}

// ============================================
// 测试套件: 查找操作
// ============================================

TEST_SUITE("Unordered_Map Lookup")
{
    TEST_CASE("find() returns iterator to existing element")
    {
        demo::unordered_map<int, int> m = {
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
        demo::unordered_map<int, int> m = {
            {1, 10},
            {2, 20}
        };

        auto it = m.find(3);

        CHECK(it == m.end());
    }

    TEST_CASE("const find() returns iterator to existing element")
    {
        const demo::unordered_map<int, int> m = {
            {1, 10},
            {2, 20}
        };

        auto it = m.find(1);

        CHECK(it != m.end());
        CHECK(it->first == 1);
    }

    TEST_CASE("count() returns 1 for existing key")
    {
        demo::unordered_map<int, int> m = {
            {1, 10},
            {2, 20}
        };

        CHECK(m.count(1) == 1);
        CHECK(m.count(2) == 1);
    }

    TEST_CASE("count() returns 0 for non-existing key")
    {
        demo::unordered_map<int, int> m = {
            {1, 10},
            {2, 20}
        };

        CHECK(m.count(3) == 0);
    }

    TEST_CASE("equal_range() returns pair of iterators")
    {
        demo::unordered_map<int, int> m = {
            {1, 10},
            {2, 20},
            {3, 30}
        };

        auto range = m.equal_range(2);
        CHECK(range.first->first == 2);
        CHECK(range.second == m.end());
    }

    TEST_CASE("equal_range() for non-existing key")
    {
        demo::unordered_map<int, int> m = {
            {1, 10},
            {2, 20}
        };

        auto range = m.equal_range(3);
        CHECK(range.first == m.end());
        CHECK(range.second == m.end());
    }
}

// ============================================
// 测试套件: 桶接口
// ============================================

TEST_SUITE("Unordered_Map Bucket Interface")
{
    TEST_CASE("bucket() returns valid bucket index")
    {
        demo::unordered_map<int, int> m{};
        m.insert({1, 10});
        m.insert({2, 20});

        size_t bucket1 = m.bucket(1);
        size_t bucket2 = m.bucket(2);

        CHECK(bucket1 < m.bucket_count());
        CHECK(bucket2 < m.bucket_count());
    }

    TEST_CASE("bucket_size() returns correct count")
    {
        demo::unordered_map<int, int> m(1);
        m.max_load_factor(10.0f);

        m.insert({1, 10});
        m.insert({2, 20});
        m.insert({3, 30});

        size_t total_count = count_elements_in_buckets(m);
        CHECK(total_count == m.size());
    }

    TEST_CASE("bucket_size() for empty bucket")
    {
        demo::unordered_map<int, int> m{};
        CHECK(m.bucket_size(0) == 0);
    }
}

// ============================================
// 测试套件: 哈希策略
// ============================================

TEST_SUITE("Unordered_Map Hash Policy")
{
    TEST_CASE("load_factor() returns correct value")
    {
        demo::unordered_map<int, int> m{};
        CHECK(m.load_factor() >= 0.0f);

        m.insert({1, 10});
        CHECK(m.load_factor() > 0.0f);
    }

    TEST_CASE("max_load_factor() returns default value")
    {
        demo::unordered_map<int, int> m{};
        CHECK(m.max_load_factor() == 1.0f);
    }

    TEST_CASE("max_load_factor() can be set")
    {
        demo::unordered_map<int, int> m{};
        m.max_load_factor(0.5f);
        CHECK(m.max_load_factor() == 0.5f);
    }

    TEST_CASE("rehash() changes bucket count")
    {
        demo::unordered_map<int, int> m{};

        m.rehash(64);
        CHECK(m.bucket_count() >= 64);
    }

    TEST_CASE("rehash() with small count preserves elements")
    {
        demo::unordered_map<int, int> m = {
            {1, 10},
            {2, 20},
            {3, 30}
        };

        m.rehash(1);
        // for (auto it = m.begin(); it != m.end(); ++it)
        // {
        //     std::cout << it->first << std::endl;
        // }
        CHECK(m.size() == 3);
        CHECK(m[1] == 10);
        CHECK(m[2] == 20);
        CHECK(m[3] == 30);
    }

    TEST_CASE("reserve() allocates space for elements")
    {
        demo::unordered_map<int, int> m{};
        m.reserve(100);

        CHECK(m.bucket_count() >= 100);
    }

    TEST_CASE("reserve() does not affect existing elements")
    {
        demo::unordered_map<int, int> m = {
            {1, 10},
            {2, 20}
        };
        auto size_before = m.size();

        m.reserve(100);

        CHECK(m.size() == size_before);
        CHECK(m[1] == 10);
        CHECK(m[2] == 20);
    }

    TEST_CASE("rehash occurs when exceeding default load factor")
    {
        demo::unordered_map<int, int> m(4);

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
        demo::unordered_map<int, int> m(4);
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

TEST_SUITE("Unordered_Map Observers")
{
    TEST_CASE("hash_function() returns hash object")
    {
        demo::unordered_map<int, int> m{};
        auto                          hash = m.hash_function();

        CHECK(hash(1) == std::hash<int>()(1));
    }

    TEST_CASE("key_eq() returns key equality object")
    {
        demo::unordered_map<int, int> m{};
        auto                          eq = m.key_eq();

        CHECK(eq(1, 1) == true);
        CHECK(eq(1, 2) == false);
    }

    TEST_CASE("get_allocator() returns allocator")
    {
        demo::unordered_map<int, int> m{};
        auto                          alloc = m.get_allocator();

        CHECK(m.size() == 0);
    }
}

// ============================================
// 测试套件: 操作函数
// ============================================

TEST_SUITE("Unordered_Map Operations")
{
    TEST_CASE("merge() merges two maps")
    {
        demo::unordered_map<int, int> m1 = {
            {1, 10},
            {3, 30}
        };
        demo::unordered_map<int, int> m2 = {
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
        demo::unordered_map<int, int> m1 = {
            {1, 10},
            {2, 20}
        };
        demo::unordered_map<int, int> m2 = {
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
        demo::unordered_map<int, int> m1 = {
            {1, 10}
        };
        demo::unordered_map<int, int> m2;

        m1.merge(m2);

        CHECK(m1.size() == 1);
        CHECK(m1[1] == 10);
    }

    TEST_CASE("merge() rvalue version")
    {
        demo::unordered_map<int, int> m1 = {
            {1, 10}
        };
        demo::unordered_map<int, int> m2 = {
            {2, 20}
        };

        m1.merge(std::move(m2));

        CHECK(m1.size() == 2);
        CHECK(m1[1] == 10);
        CHECK(m1[2] == 20);
    }
}

// ============================================
// 测试套件: 全局比较运算符
// ============================================

TEST_SUITE("Unordered_Map Comparison Operators")
{
    TEST_CASE("operator== compares contents")
    {
        demo::unordered_map<int, int> m1 = {
            {1, 10},
            {2, 20}
        };
        demo::unordered_map<int, int> m2 = {
            {1, 10},
            {2, 20}
        };
        demo::unordered_map<int, int> m3 = {
            {1, 10 },
            {2, 200}
        };

        CHECK(m1 == m2);
        CHECK_FALSE(m1 == m3);
    }

    TEST_CASE("operator!= compares contents")
    {
        demo::unordered_map<int, int> m1 = {
            {1, 10},
            {2, 20}
        };
        demo::unordered_map<int, int> m2 = {
            {1, 10},
            {2, 20}
        };
        demo::unordered_map<int, int> m3 = {
            {1, 10 },
            {2, 200}
        };

        CHECK_FALSE(m1 != m2);
        CHECK(m1 != m3);
    }

    TEST_CASE("operator== and != with different sizes")
    {
        demo::unordered_map<int, int> m1 = {
            {1, 10}
        };
        demo::unordered_map<int, int> m2 = {
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

TEST_SUITE("Unordered_Map Boundary Cases")
{
    TEST_CASE("single element map operations")
    {
        demo::unordered_map<int, int> m = {
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
        demo::unordered_map<int, std::string> m{};
        m.insert({1, std::string("test")});

        CHECK(m.size() == 1);
        CHECK(m[1] == "test");
    }

    TEST_CASE("insert and erase alternating")
    {
        demo::unordered_map<int, int> m{};

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
        demo::unordered_map<int, int> m{};

        m.insert({-1000, 1});
        m.insert({0, 2});
        m.insert({1000, 3});

        CHECK(m.size() == 3);
        CHECK(m[-1000] == 1);
        CHECK(m[0] == 2);
        CHECK(m[1000] == 3);
    }

    TEST_CASE("all elements accessible after operations")
    {
        demo::unordered_map<int, int> m{};
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
}

// ============================================
// 测试套件: 字符串类型测试
// ============================================

TEST_SUITE("Unordered_Map String Type")
{
    TEST_CASE("string key map operations")
    {
        demo::unordered_map<std::string, int> m{};

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

    TEST_CASE("string key hashing")
    {
        demo::unordered_map<std::string, int> m = {
            {"a", 1},
            {"b", 2},
            {"c", 3}
        };

        CHECK(m.size() == 3);
        CHECK(m.count("a") == 1);
        CHECK(m.count("d") == 0);
    }
}

// ============================================
// 测试套件: 压力测试
// ============================================

TEST_SUITE("Unordered_Map Stress Tests")
{
    TEST_CASE("insert and iterate 10000 elements")
    {
        demo::unordered_map<int, int> m{};

        for (int i = 0; i < 10000; ++i)
        {
            m.insert({i, i * 10});
        }

        CHECK(m.size() == 10000);

        int count = 0;
        for (const auto& pair : m)
        {
            CHECK(pair.second == pair.first * 10);
            ++count;
        }
        CHECK(count == 10000);
    }

    TEST_CASE("insert in random order")
    {
        demo::unordered_map<int, int> m{};
        std::vector<int>              keys;

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

    TEST_CASE("sequential insert and erase")
    {
        demo::unordered_map<int, int> m{};

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
        demo::unordered_map<int, int> m{};

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
        demo::unordered_map<int, int> m{};

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
        demo::unordered_map<int, int> m{};
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
        demo::unordered_map<int, int> m{};

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

    TEST_CASE("rehash performance")
    {
        demo::unordered_map<int, int> m{};

        for (int i = 0; i < 10000; ++i)
        {
            m.insert({i, i});
        }

        auto start = std::chrono::high_resolution_clock::now();
        m.rehash(1000);
        auto end = std::chrono::high_resolution_clock::now();

        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

        CHECK(m.size() == 10000);
        CHECK(duration.count() < 5000);
    }

    TEST_CASE("reserve performance")
    {
        demo::unordered_map<int, int> m{};

        auto start = std::chrono::high_resolution_clock::now();
        m.reserve(100000);
        for (int i = 0; i < 100000; ++i)
        {
            m.insert({i, i});
        }
        auto end = std::chrono::high_resolution_clock::now();

        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

        CHECK(m.size() == 100000);
    }
}

// ============================================
// 特殊类型测试
// ============================================

TEST_SUITE("Unordered_Map Special Types")
{
    TEST_CASE("unordered_map with custom struct as value")
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

        demo::unordered_map<int, MyStruct> m;
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

    TEST_CASE("unordered_map with pointer values")
    {
        demo::unordered_map<int, int*> m{};
        int*                           val1 = new int(10);
        int*                           val2 = new int(20);

        m.insert({1, val1});
        m.insert({2, val2});

        CHECK(m.size() == 2);
        CHECK(*m[1] == 10);
        CHECK(*m[2] == 20);

        delete val1;
        delete val2;
    }

    TEST_CASE("nested unordered_map")
    {
        demo::unordered_map<int, demo::unordered_map<int, std::string>> m{};

        demo::unordered_map<int, std::string> inner = {
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
// 算法测试用例
// ============================================

TEST_SUITE("Unordered_Map Algorithms")
{
    TEST_CASE("std::for_each on unordered_map")
    {
        demo::unordered_map<int, int> m = {
            {1, 10},
            {2, 20},
            {3, 30}
        };
        int sum = 0;

        std::for_each(m.begin(), m.end(), [&](const auto& pair) { sum += pair.second; });

        CHECK(sum == 60);
    }

    TEST_CASE("std::find_if on unordered_map")
    {
        demo::unordered_map<int, int> m = {
            {1, 10},
            {2, 20},
            {3, 30}
        };

        auto it =
            std::find_if(m.begin(), m.end(), [](const auto& pair) { return pair.second == 20; });

        CHECK(it != m.end());
        CHECK(it->first == 2);
    }

    TEST_CASE("std::count_if on unordered_map")
    {
        demo::unordered_map<int, int> m = {
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
// 自定义哈希函数测试
// ============================================

TEST_SUITE("Unordered_Map Custom Hash")
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

        demo::unordered_map<int, int, CustomHash> m{};
        m.insert({1, 10});
        m.insert({2, 20});

        CHECK(m.size() == 2);
        CHECK(m[1] == 10);
        CHECK(m[2] == 20);
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

        // 虽然key是相同的，但是hash_code不同，所以插入成功，size为2

        demo::unordered_map<int, int, std::hash<int>, CustomEqual> m;

        auto result = m.insert({1, 10});
        CHECK(result.second);
        CHECK(result.first->first == 1);
        CHECK(result.first->second == 10);
        result = m.insert({11, 110});
        CHECK(result.second);
        CHECK(result.first->first == 11);
        CHECK(result.first->second == 110);

        CHECK(m.size() == 2);
        CHECK(m[1] == 10);
        CHECK(m[11] == 110);
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

        // 自定义哈希：必须按个位数 hash！
        struct CustomHash
        {
            size_t operator()(int a) const
            {
                return std::hash<int>()(a % 10);
            }
        };

        // key 和 hash_code 都相同，，size为1

        demo::unordered_map<int, int, CustomHash, CustomEqual> m;

        auto result = m.insert({1, 10});
        CHECK(result.second);
        CHECK(result.first->first == 1);
        CHECK(result.first->second == 10);
        result = m.insert({11, 110});
        CHECK_FALSE(result.second);
        CHECK(result.first->first == 1);
        CHECK(result.first->second == 10);

        CHECK(m.size() == 1);
        CHECK(m[1] == 10);
    }
}

// ============================================
// 额外边界情况测试
// ============================================

TEST_SUITE("Unordered_Map Additional Edge Cases")
{
    TEST_CASE("erase all elements one by one")
    {
        demo::unordered_map<int, int> m{};
        for (int i = 0; i < 10; ++i)
        {
            m.insert({i, i * 10});
        }
        CHECK(m.size() == 10);

        for (int i = 0; i < 10; ++i)
        {
            m.erase(i);
        }
        CHECK(m.empty());
        CHECK(m.size() == 0);
    }

    TEST_CASE("erase elements in reverse order")
    {
        demo::unordered_map<int, int> m{};
        for (int i = 0; i < 5; ++i)
        {
            m.insert({i, i * 10});
        }

        for (int i = 4; i >= 0; --i)
        {
            m.erase(i);
            CHECK(m.find(i) == m.end());
        }
        CHECK(m.empty());
    }

    TEST_CASE("erase iterator from begin")
    {
        demo::unordered_map<int, int> m = {
            {1, 10},
            {2, 20},
            {3, 30}
        };

        auto it = m.begin();
        m.erase(it);

        CHECK(m.size() == 2);
    }

    TEST_CASE("erase iterator from end")
    {
        demo::unordered_map<int, int> m = {
            {1, 10},
            {2, 20},
            {3, 30}
        };

        auto it = m.find(3);
        REQUIRE(it != m.end());
        it = m.erase(it);

        CHECK(m.size() == 2);
        CHECK(m.find(3) == m.end());
    }

    TEST_CASE("swap preserves all elements")
    {
        demo::unordered_map<int, int> m1 = {
            {1, 10},
            {2, 20}
        };
        demo::unordered_map<int, int> m2 = {
            {3, 30},
            {4, 40}
        };

        m1.swap(m2);

        CHECK(m1.size() == 2);
        CHECK(m2.size() == 2);
        CHECK(m1[3] == 30);
        CHECK(m1[4] == 40);
        CHECK(m2[1] == 10);
        CHECK(m2[2] == 20);
    }

    TEST_CASE("swap multiple times")
    {
        demo::unordered_map<int, int> m1 = {
            {1, 10}
        };
        demo::unordered_map<int, int> m2 = {
            {2, 20}
        };
        demo::unordered_map<int, int> m3 = {
            {3, 30}
        };

        m1.swap(m2);
        CHECK(m1[2] == 20);
        CHECK(m2[1] == 10);

        m2.swap(m3);
        CHECK(m2[3] == 30);
        CHECK(m3[1] == 10);

        m1.swap(m3);
        CHECK(m1[1] == 10);
        CHECK(m3[2] == 20);
    }

    TEST_CASE("copy assignment replaces existing content")
    {
        demo::unordered_map<int, int> m1 = {
            {1, 10}
        };
        demo::unordered_map<int, int> m2 = {
            {2, 20},
            {3, 30},
            {4, 40}
        };

        m1 = m2;

        CHECK(m1.size() == 3);
        CHECK(m1[2] == 20);
        CHECK(m1[3] == 30);
        CHECK(m1[4] == 40);
        CHECK(m2.size() == 3);
    }

    TEST_CASE("copy assignment to empty map")
    {
        demo::unordered_map<int, int> m1{};
        demo::unordered_map<int, int> m2 = {
            {1, 10},
            {2, 20}
        };

        m1 = m2;

        CHECK(m1.size() == 2);
        CHECK(m1[1] == 10);
        CHECK(m1[2] == 20);
    }

    TEST_CASE("move assignment after use")
    {
        demo::unordered_map<int, std::string> m1 = {
            {1, "test1"}
        };
        demo::unordered_map<int, std::string> m2 = {
            {2, "test2"},
            {3, "test3"}
        };

        m1 = std::move(m2);

        CHECK(m1.size() == 2);
        CHECK(m1[2] == "test2");
        CHECK(m1[3] == "test3");
        CHECK(m2.empty());
    }

    TEST_CASE("rehash maintains all elements")
    {
        demo::unordered_map<int, int> m = {
            {1, 10},
            {2, 20},
            {3, 30}
        };

        size_t old_bucket_count = m.bucket_count();
        m.rehash(old_bucket_count * 4);

        CHECK(m.size() == 3);
        CHECK(m[1] == 10);
        CHECK(m[2] == 20);
        CHECK(m[3] == 30);
    }

    TEST_CASE("rehash to smaller size")
    {
        demo::unordered_map<int, int> m{};
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
            CHECK(m[i] == i);
        }
    }

    TEST_CASE("reserve and insert many elements")
    {
        demo::unordered_map<int, int> m{};
        m.reserve(1000);

        for (int i = 0; i < 1000; ++i)
        {
            m.insert({i, i * 2});
        }

        CHECK(m.size() == 1000);
        for (int i = 0; i < 1000; ++i)
        {
            CHECK(m[i] == i * 2);
        }
    }

    TEST_CASE("load factor behavior")
    {
        demo::unordered_map<int, int> m{};
        CHECK(m.load_factor() >= 0.0f);

        for (int i = 0; i < 100; ++i)
        {
            m.insert({i, i});
        }

        CHECK(m.load_factor() > 0.0f);
        CHECK(m.load_factor() <= m.max_load_factor() * 2);
    }

    TEST_CASE("clear then repopulate")
    {
        demo::unordered_map<int, int> m = {
            {1, 10},
            {2, 20}
        };

        m.clear();
        CHECK(m.empty());
        CHECK(m.size() == 0);

        m.insert({3, 30});
        CHECK(m.size() == 1);
        CHECK(m[3] == 30);
    }

    TEST_CASE("const map operations")
    {
        const demo::unordered_map<int, std::string> m = {
            {1, "one"  },
            {2, "two"  },
            {3, "three"}
        };

        CHECK(m.size() == 3);
        CHECK(m.at(1) == "one");
        CHECK(m.at(2) == "two");
        CHECK(m.at(3) == "three");
        CHECK(m.count(1) == 1);
        CHECK(m.count(4) == 0);
        CHECK(m.find(2) != m.end());
        CHECK(m.find(4) == m.end());
    }

    TEST_CASE("const find returns const iterator")
    {
        const demo::unordered_map<int, int> m = {
            {1, 10}
        };

        auto it = m.find(1);
        CHECK(it != m.end());
        CHECK(it->first == 1);
        CHECK(it->second == 10);
    }

    TEST_CASE("equal_range on existing key")
    {
        demo::unordered_map<int, int> m = {
            {1, 10},
            {2, 20}
        };

        auto range = m.equal_range(1);
        CHECK(range.first != m.end());
        CHECK(range.first->first == 1);
        CHECK(range.second == m.end());
    }

    TEST_CASE("equal_range on non-existing key")
    {
        demo::unordered_map<int, int> m = {
            {1, 10},
            {2, 20}
        };

        auto range = m.equal_range(99);
        CHECK(range.first == m.end());
        CHECK(range.second == m.end());
    }

    TEST_CASE("merge with empty source")
    {
        demo::unordered_map<int, int> m1 = {
            {1, 10}
        };
        demo::unordered_map<int, int> m2{};

        m1.merge(m2);

        CHECK(m1.size() == 1);
        CHECK(m1[1] == 10);
    }

    TEST_CASE("merge into empty target")
    {
        demo::unordered_map<int, int> m1{};
        demo::unordered_map<int, int> m2 = {
            {1, 10},
            {2, 20}
        };

        m1.merge(m2);

        CHECK(m1.size() == 2);
        CHECK(m1[1] == 10);
        CHECK(m1[2] == 20);
        CHECK(m2.empty());
    }

    TEST_CASE("operator== with empty maps")
    {
        demo::unordered_map<int, int> m1{};
        demo::unordered_map<int, int> m2{};

        CHECK(m1 == m2);
        CHECK_FALSE(m1 != m2);
    }

    TEST_CASE("operator== with different values")
    {
        demo::unordered_map<int, int> m1 = {
            {1, 10}
        };
        demo::unordered_map<int, int> m2 = {
            {1, 20}
        };

        CHECK_FALSE(m1 == m2);
        CHECK(m1 != m2);
    }

    TEST_CASE("operator!= with same maps")
    {
        demo::unordered_map<int, int> m1 = {
            {1, 10},
            {2, 20}
        };
        demo::unordered_map<int, int> m2 = {
            {1, 10},
            {2, 20}
        };

        CHECK_FALSE(m1 != m2);
    }

    TEST_CASE("insert returns correct iterator for existing key")
    {
        demo::unordered_map<int, int> m = {
            {1, 10}
        };

        auto result = m.insert({1, 100});

        CHECK(result.second == false);
        CHECK(result.first->second == 10);
        CHECK(m[1] == 10);
    }

    TEST_CASE("insert_or_assign updates existing")
    {
        demo::unordered_map<int, std::string> m = {
            {1, "original"}
        };

        auto result = m.insert_or_assign(1, "updated");

        CHECK(result.second == false);
        CHECK(m[1] == "updated");
        CHECK(m.size() == 1);
    }

    TEST_CASE("try_emplace does not overwrite existing")
    {
        demo::unordered_map<int, std::string> m = {
            {1, "original"}
        };

        auto result = m.try_emplace(1, "new_value");

        CHECK(result.second == false);
        CHECK(m[1] == "original");
        CHECK(m.size() == 1);
    }

    TEST_CASE("emplace returns correct result")
    {
        demo::unordered_map<int, int> m{};

        auto result1 = m.emplace(1, 10);
        CHECK(result1.second == true);
        CHECK(m[1] == 10);

        auto result2 = m.emplace(1, 20);
        CHECK(result2.second == false);
        CHECK(m[1] == 10);
    }

    TEST_CASE("bucket operations with rehashing")
    {
        demo::unordered_map<int, int> m(4);
        m.max_load_factor(0.5f);

        for (int i = 0; i < 10; ++i)
        {
            m.insert({i, i});
        }

        CHECK(m.size() == 10);
        for (int i = 0; i < 10; ++i)
        {
            CHECK(m.find(i) != m.end());
        }
    }

    TEST_CASE("local iterator traversal")
    {
        demo::unordered_map<int, int> m(32);
        m.max_load_factor(10.0f);

        for (int i = 0; i < 20; ++i)
        {
            m.insert({i, i * 10});
        }

        size_t total_elements = 0;
        for (size_t b = 0; b < m.bucket_count(); ++b)
        {
            for (auto it = m.begin(b); it != m.end(b); ++it)
            {
                CHECK(it->second == it->first * 10);
                ++total_elements;
            }
        }
        CHECK(total_elements == m.size());
    }

    TEST_CASE("copy constructor deep copies")
    {
        demo::unordered_map<int, std::vector<int>> m;
        m.insert({
            1, {1, 2, 3}
        });
        m.insert({
            2, {4, 5}
        });

        demo::unordered_map<int, std::vector<int>> m2(m);

        CHECK(m2.size() == 2);
        CHECK(m2[1].size() == 3);
        CHECK(m2[2].size() == 2);

        m[1].push_back(99);
        CHECK(m2[1].size() == 3);
    }

    TEST_CASE("move constructor moves data")
    {
        demo::unordered_map<int, std::vector<int>> m;
        m.insert({
            1, {1, 2, 3}
        });

        demo::unordered_map<int, std::vector<int>> m2(std::move(m));

        CHECK(m2.size() == 1);
        CHECK(m2[1].size() == 3);
        CHECK(m.empty());
    }
}
