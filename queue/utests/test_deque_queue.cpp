#define DOCTEST_CONFIG_NO_MULTITHREADING

#include <chrono>
#include <string>
#include <vector>

#include "../queue.h"
#include "doctest.h"

template <typename T, typename Container>
std::vector<T> queue_to_vector(const demo::queue<T, Container>& q)
{
    std::vector<T>            result;
    demo::queue<T, Container> temp = q;
    while (!temp.empty())
    {
        result.push_back(temp.front());
        temp.pop();
    }
    return result;
}

template <typename T, typename Container>
bool queue_equals_vector(const demo::queue<T, Container>& q, const std::vector<T>& vec)
{
    if (q.size() != vec.size())
        return false;

    demo::queue<T, Container> temp = q;
    for (const auto& val : vec)
    {
        if (temp.front() != val)
            return false;
        temp.pop();
    }
    return true;
}

TEST_SUITE("Deque Queue Constructors")
{
    TEST_CASE("Default constructor creates empty queue")
    {
        demo::queue<int> q;
        CHECK(q.empty());
        CHECK(q.size() == 0);
    }

    TEST_CASE("Constructor with container")
    {
        demo::deque<int> dq{1, 2, 3, 4, 5};
        demo::queue<int> q(dq);
        CHECK(q.size() == 5);
        CHECK_FALSE(q.empty());
        CHECK(q.front() == 1);
        CHECK(q.back() == 5);
    }

    TEST_CASE("Copy constructor")
    {
        demo::queue<int> q1;
        q1.push(1);
        q1.push(2);
        q1.push(3);

        demo::queue<int> q2(q1);
        CHECK(q2.size() == 3);
        CHECK(q2.front() == 1);
        CHECK(q2.back() == 3);

        q2.pop();
        CHECK(q2.front() == 2);
        CHECK(q1.size() == 3);
        CHECK(q1.front() == 1);
    }

    TEST_CASE("Move constructor")
    {
        demo::queue<int> q1;
        q1.push(1);
        q1.push(2);
        q1.push(3);
        auto size_before = q1.size();

        demo::queue<int> q2(std::move(q1));
        CHECK(q2.size() == size_before);
        CHECK(q2.front() == 1);
        CHECK(q2.back() == 3);
        CHECK(q1.empty());
    }
}

TEST_SUITE("Deque Queue Assignment Operators")
{
    TEST_CASE("Copy assignment operator")
    {
        demo::queue<int> q1;
        q1.push(1);
        q1.push(2);
        demo::queue<int> q2;
        q2.push(10);

        q2 = q1;

        CHECK(q2.size() == 2);
        CHECK(q2.front() == 1);
        CHECK(q2.back() == 2);
        q2.pop();
        CHECK(q2.front() == 2);
        CHECK(q1.size() == 2);
        CHECK(q1.front() == 1);
    }

    TEST_CASE("Move assignment operator")
    {
        demo::queue<int> q1;
        q1.push(1);
        q1.push(2);
        auto             size_before = q1.size();
        demo::queue<int> q2;

        q2 = std::move(q1);

        CHECK(q2.size() == size_before);
        CHECK(q2.front() == 1);
        CHECK(q2.back() == 2);
        CHECK(q1.empty());
    }

    TEST_CASE("Self assignment")
    {
        demo::queue<int> q;
        q.push(1);
        q.push(2);
        q.push(3);

        q = q;

        CHECK(q.size() == 3);
        CHECK(q.front() == 1);
        CHECK(q.back() == 3);
    }
}

TEST_SUITE("Deque Queue Element Access")
{
    TEST_CASE("front() returns reference to front element")
    {
        demo::queue<int> q;
        q.push(1);
        q.push(2);
        q.push(3);

        int& ref = q.front();
        CHECK(ref == 1);

        ref = 42;
        CHECK(q.front() == 42);
    }

    TEST_CASE("const front() returns const reference")
    {
        demo::queue<int> q;
        q.push(1);
        q.push(2);

        const demo::queue<int>& const_q = q;
        const int&              ref     = const_q.front();
        CHECK(ref == 1);
    }

    TEST_CASE("back() returns reference to back element")
    {
        demo::queue<int> q;
        q.push(1);
        q.push(2);
        q.push(3);

        int& ref = q.back();
        CHECK(ref == 3);

        ref = 42;
        CHECK(q.back() == 42);
    }

    TEST_CASE("const back() returns const reference")
    {
        demo::queue<int> q;
        q.push(1);
        q.push(2);

        const demo::queue<int>& const_q = q;
        const int&              ref     = const_q.back();
        CHECK(ref == 2);
    }
}

TEST_SUITE("Deque Queue Capacity")
{
    TEST_CASE("empty() returns true for empty queue")
    {
        demo::queue<int> q;
        CHECK(q.empty());
    }

    TEST_CASE("empty() returns false for non-empty queue")
    {
        demo::queue<int> q;
        q.push(1);
        CHECK_FALSE(q.empty());
    }

    TEST_CASE("size() returns correct number of elements")
    {
        demo::queue<int> q;
        CHECK(q.size() == 0);

        q.push(1);
        CHECK(q.size() == 1);

        q.push(2);
        CHECK(q.size() == 2);

        q.pop();
        CHECK(q.size() == 1);
    }
}

TEST_SUITE("Deque Queue Modifiers")
{
    TEST_CASE("push() with const value")
    {
        demo::queue<int> q;
        int              val = 42;
        q.push(val);
        CHECK(q.size() == 1);
        CHECK(q.front() == 42);
        CHECK(q.back() == 42);
    }

    TEST_CASE("push() with rvalue")
    {
        demo::queue<int> q;
        q.push(42);
        CHECK(q.size() == 1);
        CHECK(q.front() == 42);
        CHECK(q.back() == 42);
    }

    TEST_CASE("emplace() constructs element in place")
    {
        demo::queue<std::string> q;
        q.emplace("hello");
        auto world = q.emplace("world");
        CHECK(world == q.back());

        CHECK(q.size() == 2);
        CHECK(q.front() == "hello");
        CHECK(q.back() == "world");
    }

    TEST_CASE("pop() removes front element")
    {
        demo::queue<int> q;
        q.push(1);
        q.push(2);
        q.push(3);

        CHECK(q.size() == 3);
        q.pop();
        CHECK(q.size() == 2);
        CHECK(q.front() == 2);
        q.pop();
        CHECK(q.size() == 1);
        CHECK(q.front() == 3);
        q.pop();
        CHECK(q.size() == 0);
        CHECK(q.empty());
    }

    TEST_CASE("swap() exchanges contents")
    {
        demo::queue<int> q1;
        q1.push(1);
        q1.push(2);

        demo::queue<int> q2;
        q2.push(10);
        q2.push(20);
        q2.push(30);

        q1.swap(q2);

        CHECK(q1.size() == 3);
        CHECK(q1.front() == 10);
        CHECK(q1.back() == 30);
        CHECK(q2.size() == 2);
        CHECK(q2.front() == 1);
        CHECK(q2.back() == 2);
    }
}

TEST_SUITE("Deque Queue Comparison Operators")
{
    TEST_CASE("operator== compares contents")
    {
        demo::queue<int> q1;
        q1.push(1);
        q1.push(2);

        demo::queue<int> q2;
        q2.push(1);
        q2.push(2);

        demo::queue<int> q3;
        q3.push(1);
        q3.push(3);

        CHECK(q1 == q2);
        CHECK_FALSE(q1 == q3);
    }

    TEST_CASE("operator!= compares contents")
    {
        demo::queue<int> q1;
        q1.push(1);
        q1.push(2);

        demo::queue<int> q2;
        q2.push(1);
        q2.push(2);

        demo::queue<int> q3;
        q3.push(1);
        q3.push(3);

        CHECK_FALSE(q1 != q2);
        CHECK(q1 != q3);
    }

    TEST_CASE("operator== and != with different sizes")
    {
        demo::queue<int> q1;
        q1.push(1);

        demo::queue<int> q2;
        q2.push(1);
        q2.push(2);

        CHECK_FALSE(q1 == q2);
        CHECK(q1 != q2);
    }
}

TEST_SUITE("Deque Queue Edge Cases")
{
    TEST_CASE("push and pop single element")
    {
        demo::queue<int> q;
        q.push(42);
        CHECK(q.size() == 1);
        CHECK(q.front() == 42);
        CHECK(q.back() == 42);
        q.pop();
        CHECK(q.empty());
    }

    TEST_CASE("push and pop many elements")
    {
        demo::queue<int> q;
        for (int i = 0; i < 100; ++i)
        {
            q.push(i);
        }
        CHECK(q.size() == 100);

        for (int i = 0; i < 100; ++i)
        {
            CHECK(q.front() == i);
            q.pop();
        }
        CHECK(q.empty());
    }

    TEST_CASE("queue of strings")
    {
        demo::queue<std::string> q;
        q.push("hello");
        q.push("world");
        q.push("!");

        CHECK(q.size() == 3);
        CHECK(q.front() == "hello");
        CHECK(q.back() == "!");

        q.pop();
        CHECK(q.front() == "world");
        CHECK(q.back() == "!");
    }
}

TEST_SUITE("Deque Queue Performance")
{
    TEST_CASE("push performance test")
    {
        demo::queue<int> q;
        auto             start = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < 100000; ++i)
        {
            q.push(i);
        }
        auto end      = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

        CHECK(q.size() == 100000);
        CHECK(duration.count() < 5000);
    }

    TEST_CASE("push and pop performance test")
    {
        demo::queue<int> q;
        auto             start = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < 100000; ++i)
        {
            q.push(i);
        }
        for (int i = 0; i < 100000; ++i)
        {
            q.pop();
        }
        auto end      = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

        CHECK(q.empty());
        CHECK(duration.count() < 10000);
    }
}