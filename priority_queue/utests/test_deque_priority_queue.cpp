/**
 * @file test_deque_priority_queue.cpp
 * @brief 基于 doctest 的工业级优先队列测试套件 - 使用 deque 作为底层容器
 *
 * 测试覆盖范围:
 * - 构造函数(默认、容器、比较函数、拷贝、移动、迭代器范围)
 * - 赋值操作符(拷贝赋值、移动赋值)
 * - 元素访问(top)
 * - 容量操作(empty, size)
 * - 修改器(push, emplace, pop, swap)
 * - 边界条件和异常安全
 * - 性能测试
 */

#define DOCTEST_CONFIG_NO_MULTITHREADING
#include <chrono>
#include <random>
#include <string>
#include <vector>

#include "../priority_queue.h"
#include "doctest.h"

// ============================================
// 测试辅助工具
// ============================================

template <typename T, typename Container, typename Compare>
std::vector<T> priority_queue_to_vector(const demo::priority_queue<T, Container, Compare>& pq)
{
    std::vector<T>                              result;
    demo::priority_queue<T, Container, Compare> temp = pq;
    while (!temp.empty())
    {
        result.push_back(temp.top());
        temp.pop();
    }
    return result;
}

// ============================================
// 测试套件: 构造函数
// ============================================

TEST_SUITE("Deque Priority Queue Constructors")
{
    TEST_CASE("Default constructor creates empty priority queue")
    {
        demo::priority_queue<int, demo::deque<int>> pq;
        CHECK(pq.empty());
        CHECK(pq.size() == 0);
    }

    TEST_CASE("Constructor with container")
    {
        demo::deque<int>                            dq{3, 1, 4, 1, 5, 9, 2, 6};
        demo::priority_queue<int, demo::deque<int>> pq(dq);

        CHECK(pq.size() == 8);
        CHECK_FALSE(pq.empty());

        std::vector<int> result = priority_queue_to_vector(pq);
        std::vector<int> expected{9, 6, 5, 4, 3, 2, 1, 1};
        CHECK(result == expected);
    }

    TEST_CASE("Constructor with compare function")
    {
        auto                                                           comp = std::greater<int>();
        demo::priority_queue<int, demo::deque<int>, std::greater<int>> pq(comp);
        pq.push(1);
        pq.push(3);
        pq.push(4);

        CHECK(pq.size() == 3);
        CHECK(pq.top() == 1);
    }

    TEST_CASE("Constructor with container and compare function")
    {
        demo::deque<int>                                               dq{3, 1, 4, 1, 5};
        demo::priority_queue<int, demo::deque<int>, std::greater<int>> pq(dq, std::greater<int>());

        CHECK(pq.size() == 5);
        CHECK(pq.top() == 1);

        std::vector<int> result = priority_queue_to_vector(pq);
        std::vector<int> expected{1, 1, 3, 4, 5};
        CHECK(result == expected);
    }

    TEST_CASE("Move constructor with container")
    {
        demo::deque<int>                            dq{3, 1, 4, 1, 5};
        demo::priority_queue<int, demo::deque<int>> pq(std::move(dq));

        CHECK(pq.size() == 5);
        CHECK(pq.top() == 5);
    }

    TEST_CASE("Iterator range constructor")
    {
        demo::deque<int>                            dq{9, 2, 8, 3, 7};
        demo::priority_queue<int, demo::deque<int>> pq(dq.begin(), dq.end());

        CHECK(pq.size() == 5);
        CHECK(pq.top() == 9);

        std::vector<int> result = priority_queue_to_vector(pq);
        std::vector<int> expected{9, 8, 7, 3, 2};
        CHECK(result == expected);
    }

    TEST_CASE("Copy constructor")
    {
        demo::priority_queue<int, demo::deque<int>> pq1;
        pq1.push(10);
        pq1.push(20);
        pq1.push(30);

        demo::priority_queue<int, demo::deque<int>> pq2(pq1);
        CHECK(pq2.size() == 3);
        CHECK(pq2.top() == 30);

        pq2.pop();
        CHECK(pq2.top() == 20);
        CHECK(pq1.size() == 3);
        CHECK(pq1.top() == 30);
    }

    TEST_CASE("Move constructor")
    {
        demo::priority_queue<int, demo::deque<int>> pq1;
        pq1.push(10);
        pq1.push(20);
        pq1.push(30);
        auto size_before = pq1.size();

        demo::priority_queue<int, demo::deque<int>> pq2(std::move(pq1));
        CHECK(pq2.size() == size_before);
        CHECK(pq2.top() == 30);
        CHECK(pq1.empty());
    }
}

// ============================================
// 测试套件: 赋值操作符
// ============================================

TEST_SUITE("Deque Priority Queue Assignment Operators")
{
    TEST_CASE("Copy assignment operator")
    {
        demo::priority_queue<int, demo::deque<int>> pq1;
        pq1.push(10);
        pq1.push(20);
        demo::priority_queue<int, demo::deque<int>> pq2;
        pq2.push(100);

        pq2 = pq1;

        CHECK(pq2.size() == 2);
        CHECK(pq2.top() == 20);
        pq2.pop();
        CHECK(pq2.top() == 10);
        CHECK(pq1.size() == 2);
        CHECK(pq1.top() == 20);
    }

    TEST_CASE("Move assignment operator")
    {
        demo::priority_queue<int, demo::deque<int>> pq1;
        pq1.push(10);
        pq1.push(20);
        auto                                        size_before = pq1.size();
        demo::priority_queue<int, demo::deque<int>> pq2;

        pq2 = std::move(pq1);

        CHECK(pq2.size() == size_before);
        CHECK(pq2.top() == 20);
        CHECK(pq1.empty());
    }

    TEST_CASE("Self assignment")
    {
        demo::priority_queue<int, demo::deque<int>> pq;
        pq.push(10);
        pq.push(20);
        pq.push(30);

        pq = pq;

        CHECK(pq.size() == 3);
        CHECK(pq.top() == 30);
    }
}

// ============================================
// 测试套件: 元素访问
// ============================================

TEST_SUITE("Deque Priority Queue Element Access")
{
    TEST_CASE("top() returns const reference to top element")
    {
        demo::priority_queue<int, demo::deque<int>> pq;
        pq.push(10);
        pq.push(20);
        pq.push(30);

        const demo::priority_queue<int, demo::deque<int>>& const_pq = pq;
        const int&                                         ref      = const_pq.top();
        CHECK(ref == 30);
    }
}

// ============================================
// 测试套件: 容量操作
// ============================================

TEST_SUITE("Deque Priority Queue Capacity")
{
    TEST_CASE("empty() returns true for empty priority queue")
    {
        demo::priority_queue<int, demo::deque<int>> pq;
        CHECK(pq.empty());
    }

    TEST_CASE("empty() returns false for non-empty priority queue")
    {
        demo::priority_queue<int, demo::deque<int>> pq;
        pq.push(10);
        CHECK_FALSE(pq.empty());
    }

    TEST_CASE("size() returns correct number of elements")
    {
        demo::priority_queue<int, demo::deque<int>> pq;
        CHECK(pq.size() == 0);

        pq.push(10);
        CHECK(pq.size() == 1);

        pq.push(20);
        CHECK(pq.size() == 2);

        pq.pop();
        CHECK(pq.size() == 1);
    }
}

// ============================================
// 测试套件: 修改器
// ============================================

TEST_SUITE("Deque Priority Queue Modifiers")
{
    TEST_CASE("push() with const value")
    {
        demo::priority_queue<int, demo::deque<int>> pq;
        int                                         val = 42;
        pq.push(val);
        CHECK(pq.size() == 1);
        CHECK(pq.top() == 42);

        pq.push(50);
        CHECK(pq.top() == 50);

        pq.push(30);
        CHECK(pq.top() == 50);
    }

    TEST_CASE("push() with rvalue")
    {
        demo::priority_queue<int, demo::deque<int>> pq;
        pq.push(42);
        CHECK(pq.size() == 1);
        CHECK(pq.top() == 42);
    }

    TEST_CASE("emplace() constructs element in place")
    {
        demo::priority_queue<std::string, demo::deque<std::string>> pq;
        pq.emplace("hello");
        pq.emplace("world");
        pq.emplace("priority");

        CHECK(pq.size() == 3);
        CHECK(pq.top() == "world");
    }

    TEST_CASE("pop() removes top element")
    {
        demo::priority_queue<int, demo::deque<int>> pq;
        pq.push(10);
        pq.push(30);
        pq.push(20);

        CHECK(pq.size() == 3);
        CHECK(pq.top() == 30);

        pq.pop();
        CHECK(pq.size() == 2);
        CHECK(pq.top() == 20);

        pq.pop();
        CHECK(pq.size() == 1);
        CHECK(pq.top() == 10);

        pq.pop();
        CHECK(pq.size() == 0);
        CHECK(pq.empty());
    }

    TEST_CASE("swap() exchanges contents")
    {
        demo::priority_queue<int, demo::deque<int>> pq1;
        pq1.push(10);
        pq1.push(20);

        demo::priority_queue<int, demo::deque<int>> pq2;
        pq2.push(100);
        pq2.push(200);
        pq2.push(300);

        pq1.swap(pq2);

        CHECK(pq1.size() == 3);
        CHECK(pq1.top() == 300);
        CHECK(pq2.size() == 2);
        CHECK(pq2.top() == 20);
    }
}

// ============================================
// 测试套件: 边界条件
// ============================================

TEST_SUITE("Deque Priority Queue Edge Cases")
{
    TEST_CASE("Push and pop single element")
    {
        demo::priority_queue<int, demo::deque<int>> pq;
        pq.push(42);
        CHECK(pq.size() == 1);
        CHECK(pq.top() == 42);
        pq.pop();
        CHECK(pq.empty());
    }

    TEST_CASE("Push and pop many elements")
    {
        demo::priority_queue<int, demo::deque<int>> pq;
        for (int i = 0; i < 100; i++)
        {
            pq.push(i);
        }
        CHECK(pq.size() == 100);

        for (int i = 99; i >= 0; i--)
        {
            CHECK(pq.top() == i);
            pq.pop();
        }
        CHECK(pq.empty());
    }

    TEST_CASE("Priority queue of strings")
    {
        demo::priority_queue<std::string, demo::deque<std::string>> pq;
        pq.push("apple");
        pq.push("banana");
        pq.push("cherry");
        pq.push("date");

        CHECK(pq.size() == 4);
        CHECK(pq.top() == "date");

        pq.pop();
        CHECK(pq.top() == "cherry");
    }

    TEST_CASE("Priority queue with duplicates")
    {
        demo::priority_queue<int, demo::deque<int>> pq;
        pq.push(5);
        pq.push(3);
        pq.push(5);
        pq.push(1);
        pq.push(5);

        CHECK(pq.size() == 5);
        CHECK(pq.top() == 5);

        pq.pop();
        CHECK(pq.top() == 5);

        pq.pop();
        CHECK(pq.top() == 5);

        pq.pop();
        CHECK(pq.top() == 3);
    }

    TEST_CASE("Min priority queue using greater")
    {
        demo::priority_queue<int, demo::deque<int>, std::greater<int>> pq;
        pq.push(5);
        pq.push(3);
        pq.push(8);
        pq.push(1);
        pq.push(4);

        CHECK(pq.size() == 5);
        CHECK(pq.top() == 1);

        pq.pop();
        CHECK(pq.top() == 3);

        pq.pop();
        CHECK(pq.top() == 4);
    }
}

// ============================================
// 测试套件: 性能测试
// ============================================

TEST_SUITE("Deque Priority Queue Performance")
{
    TEST_CASE("Push performance test")
    {
        demo::priority_queue<int, demo::deque<int>> pq;
        auto start = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < 100000; i++)
        {
            pq.push(i);
        }
        auto end      = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

        CHECK(pq.size() == 100000);
        CHECK(duration.count() < 10000);  // 应该在10秒内完成
    }

    TEST_CASE("Push and pop performance test")
    {
        demo::priority_queue<int, demo::deque<int>> pq;
        auto start = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < 100000; i++)
        {
            pq.push(i);
        }
        for (int i = 0; i < 100000; i++)
        {
            pq.pop();
        }
        auto end      = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

        CHECK(pq.empty());
        CHECK(duration.count() < 15000);  // 应该在15秒内完成
    }

    TEST_CASE("Random push and pop performance test")
    {
        demo::priority_queue<int, demo::deque<int>> pq;
        std::mt19937                                rng(42);
        std::uniform_int_distribution<int>          dist(1, 1000000);

        auto start = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < 50000; i++)
        {
            pq.push(dist(rng));
        }
        for (int i = 0; i < 25000; i++)
        {
            pq.pop();
        }
        for (int i = 0; i < 50000; i++)
        {
            pq.push(dist(rng));
        }
        auto end      = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

        CHECK(pq.size() == 75000);
        CHECK(duration.count() < 15000);  // 应该在15秒内完成
    }
}
