#include <iostream>
#include <string>
#include <stdexcept>
#include "vector.h"
#include <functional>
#include <vector>  // 用于生成预期结果
#include <sstream> // 用于vector_to_string

// 工具1：对比两个vector的所有元素（逐元素校验）
template <typename T>
bool vector_equals(const demo::vector<T> &actual, const std::vector<T> &expected)
{
    if (actual.size() != expected.size())
        return false;
    for (size_t i = 0; i < actual.size(); ++i)
    {
        if (actual[i] != expected[i])
            return false;
    }
    return true;
}

// 工具2：打印vector的完整内容（调试用）
template <typename T>
std::string vector_to_string(const demo::vector<T> &vec)
{
    std::ostringstream oss;
    oss << "[";
    for (size_t i = 0; i < vec.size(); ++i)
    {
        oss << "\"" << vec[i] << "\"";
        if (i != vec.size() - 1)
            oss << ", ";
    }
    oss << "]";
    return oss.str();
}

// 工具3：断言两个vector内容完全相等（带详细错误提示）
template <typename T>
void assert_vector_equal(const demo::vector<T> &actual, const std::vector<T> &expected, const std::string &test_name)
{
    if (vector_equals(actual, expected))
    {
        std::cout << " 测试通过: " << test_name << " | 内容: " << vector_to_string(actual) << std::endl;
    }
    else
    {
        std::cerr << " 测试失败: " << test_name << std::endl;
        std::cerr << "    实际内容: " << vector_to_string(actual) << std::endl;
        std::cerr << "    预期内容: [";
        for (size_t i = 0; i < expected.size(); ++i)
        {
            std::cerr << "\"" << expected[i] << "\"";
            if (i != expected.size() - 1)
                std::cerr << ", ";
        }
        std::cerr << "]" << std::endl;
    }
}

// 原有断言函数保留（补充size_t显式类型）
template <typename T>
void assert_equal(const T &actual, const T &expected, const std::string &test_name)
{
    if (actual != expected)
    {
        std::cerr << " 测试失败: " << test_name
                  << " | 实际值: " << actual
                  << " | 期望值: " << expected << std::endl;
    }
    else
    {
        std::cout << " 测试通过: " << test_name << std::endl;
    }
}

void assert_true(bool condition, const std::string &test_name)
{
    if (!condition)
    {
        std::cerr << " 测试失败: " << test_name << std::endl;
    }
    else
    {
        std::cout << " 测试通过: " << test_name << std::endl;
    }
}

void assert_throws(const std::string &test_name, std::function<void()> func)
{
    try
    {
        func();
        std::cerr << " 测试失败: " << test_name << " (未抛出预期异常)" << std::endl;
    }
    catch (...)
    {
        std::cout << " 测试通过: " << test_name << std::endl;
    }
}

// 测试demo::vector<std::string>的所有核心功能（细化版）
void test_demo_vector_string()
{
    std::cout << "========== 开始测试 demo::vector<std::string> ==========\n"
              << std::endl;

    // 1. 构造函数测试（新增全量元素校验）
    std::cout << "--- 1. 构造函数测试 ---" << std::endl;
    // 空构造
    demo::vector<std::string> v1;
    assert_equal<std::size_t>(v1.size(), 0ULL, "空构造 - size");
    assert_equal(v1.capacity(), 0ULL, "空构造 - capacity");
    assert_true(v1.empty(), "空构造 - empty");
    assert_vector_equal(v1, std::vector<std::string>{}, "空构造 - 全量元素校验");

    // 大小构造
    demo::vector<std::string> v2(3);
    assert_equal(v2.size(), 3ULL, "大小构造 - size");
    assert_equal(v2.capacity(), 3ULL, "大小构造 - capacity");
    assert_vector_equal(v2, std::vector<std::string>{"", "", ""}, "大小构造 - 全量元素校验");

    // 大小+值构造
    demo::vector<std::string> v3(2, "hello");
    assert_equal(v3.size(), 2ULL, "大小+值构造 - size");
    assert_vector_equal(v3, std::vector<std::string>{"hello", "hello"}, "大小+值构造 - 全量元素校验");

    // 迭代器范围构造
    std::string arr[] = {"a", "b", "c"};
    demo::vector<std::string> v4(std::begin(arr), std::end(arr));
    assert_vector_equal(v4, std::vector<std::string>{"a", "b", "c"}, "迭代器构造 - 全量元素校验");

    // 初始化列表构造
    demo::vector<std::string> v5{"apple", "banana", "cherry"};
    assert_vector_equal(v5, std::vector<std::string>{"apple", "banana", "cherry"}, "初始化列表构造 - 全量元素校验");

    // 拷贝构造
    demo::vector<std::string> v6(v5);
    assert_vector_equal(v6, std::vector<std::string>{"apple", "banana", "cherry"}, "拷贝构造 - 全量元素校验");

    // 移动构造
    demo::vector<std::string> v7(std::move(v5));
    assert_vector_equal(v7, std::vector<std::string>{"apple", "banana", "cherry"}, "移动构造 - 新对象全量元素");
    assert_vector_equal(v5, std::vector<std::string>{}, "移动构造 - 原对象全量元素");

    std::cout << std::endl;

    // 2. 元素访问测试（无新增，原有已覆盖）
    std::cout << "--- 2. 元素访问测试 ---" << std::endl;
    demo::vector<std::string> v_access{"one", "two", "three"};
    assert_equal(v_access[0], std::string("one"), "operator[] - 访问第一个元素");
    assert_equal(v_access[2], std::string("three"), "operator[] - 访问最后一个元素");
    assert_equal(v_access.at(1), std::string("two"), "at() - 合法索引");
    assert_throws("at() - 越界索引", [&]()
                  { v_access.at(5); });
    assert_equal(v_access.front(), std::string("one"), "front()");
    assert_equal(v_access.back(), std::string("three"), "back()");
    assert_throws("front() - 空vector", [&]()
                  { demo::vector<std::string>().front(); });
    assert_equal(*v_access.data(), std::string("one"), "data() - 访问首元素");

    std::cout << std::endl;

    // 3. 容量操作测试（新增resize后全量元素校验）
    std::cout << "--- 3. 容量操作测试 ---" << std::endl;
    demo::vector<std::string> v_cap;
    v_cap.reserve(10);
    assert_equal(v_cap.capacity(), 10ULL, "reserve - 扩容容量");
    assert_equal(v_cap.size(), 0ULL, "reserve - 不改变size");

    // resize扩容（带初始值）
    v_cap.resize(5, "test");
    assert_vector_equal(v_cap, std::vector<std::string>{"test", "test", "test", "test", "test"}, "resize - 扩容全量元素");

    // resize缩容
    v_cap.resize(3);
    assert_vector_equal(v_cap, std::vector<std::string>{"test", "test", "test"}, "resize - 缩容全量元素");
    assert_throws("resize缩容后访问越界", [&]()
                  { v_cap.at(4); });

    // shrink_to_fit
    v_cap.reserve(20);
    v_cap.shrink_to_fit();
    assert_equal(v_cap.capacity(), 3ULL, "shrink_to_fit - 收缩容量");
    assert_true(v_cap.max_size() > 0, "max_size - 有效最大值");

    std::cout << std::endl;

    // 4. 修改操作测试（重点新增全量元素校验）
    std::cout << "--- 4. 修改操作测试 ---" << std::endl;
    demo::vector<std::string> v_mod{"cat", "dog"};
    assert_vector_equal(v_mod, std::vector<std::string>{"cat", "dog"}, "初始状态 - 全量元素");

    // push_back
    v_mod.push_back("fish");
    assert_vector_equal(v_mod, std::vector<std::string>{"cat", "dog", "fish"}, "push_back - 新增单个元素");
    v_mod.push_back(std::string("bird"));
    assert_vector_equal(v_mod, std::vector<std::string>{"cat", "dog", "fish", "bird"}, "push_back - 移动语义新增");

    // emplace_back
    auto &ref = v_mod.emplace_back("tiger", 3);
    assert_vector_equal(v_mod, std::vector<std::string>{"cat", "dog", "fish", "bird", "tig"}, "emplace_back - 原地构造");

    // pop_back
    v_mod.pop_back();
    assert_vector_equal(v_mod, std::vector<std::string>{"cat", "dog", "fish", "bird"}, "pop_back - 删除最后一个元素");
    assert_throws("pop_back - 空vector", [&]()
                  { demo::vector<std::string>().pop_back(); });

    // insert 单个元素
    auto it = v_mod.insert(v_mod.begin() + 1, "rabbit");
    assert_vector_equal(v_mod, std::vector<std::string>{"cat", "rabbit", "dog", "fish", "bird"}, "insert - 单个元素插入");

    // insert 多个元素
    v_mod.insert(v_mod.end(), 2, "snake");
    assert_vector_equal(v_mod, std::vector<std::string>{"cat", "rabbit", "dog", "fish", "bird", "snake", "snake"}, "insert - 多个元素插入");

    // erase 单个元素
    it = v_mod.erase(v_mod.begin() + 1);
    assert_vector_equal(v_mod, std::vector<std::string>{"cat", "dog", "fish", "bird", "snake", "snake"}, "erase - 单个元素删除");

    // erase 范围元素
    it = v_mod.erase(v_mod.begin() + 4, v_mod.end());
    assert_vector_equal(v_mod, std::vector<std::string>{"cat", "dog", "fish", "bird"}, "erase - 范围元素删除");

    // clear
    v_mod.clear();
    assert_vector_equal(v_mod, std::vector<std::string>{}, "clear - 清空所有元素");

    // swap
    demo::vector<std::string> v_swap1{"x", "y"}, v_swap2{"1", "2", "3"};
    v_swap1.swap(v_swap2);
    assert_vector_equal(v_swap1, std::vector<std::string>{"1", "2", "3"}, "swap - 第一个vector全量元素");
    assert_vector_equal(v_swap2, std::vector<std::string>{"x", "y"}, "swap - 第二个vector全量元素");

    // assign 大小+值
    v_mod.assign(2, "new_val");
    assert_vector_equal(v_mod, std::vector<std::string>{"new_val", "new_val"}, "assign - 大小+值全量元素");

    // assign 初始化列表
    v_mod.assign({"a", "b", "c"});
    assert_vector_equal(v_mod, std::vector<std::string>{"a", "b", "c"}, "assign - 初始化列表全量元素");

    std::cout << std::endl;

    // 5. 迭代器测试（新增遍历后全量元素校验）
    std::cout << "--- 5. 迭代器测试 ---" << std::endl;
    demo::vector<std::string> v_iter{"alpha", "beta", "gamma"};

    // 普通迭代器遍历（校验拼接结果）
    std::string concat;
    for (demo::vector<std::string>::iterator it = v_iter.begin(); it != v_iter.end(); ++it)
    {
        concat += *it + " ";
    }
    assert_equal(concat, std::string("alpha beta gamma "), "普通迭代器遍历 - 拼接结果");

    // const迭代器遍历
    std::string const_concat;
    for (demo::vector<std::string>::const_iterator it = v_iter.cbegin(); it != v_iter.cend(); ++it)
    {
        const_concat += *it + " ";
    }
    assert_equal(const_concat, std::string("alpha beta gamma "), "const迭代器遍历 - 拼接结果");

    // 反向迭代器遍历
    std::string reverse_concat;
    for (demo::vector<std::string>::reverse_iterator it = v_iter.rbegin(); it != v_iter.rend(); ++it)
    {
        reverse_concat += *it + " ";
    }
    assert_equal(reverse_concat, std::string("gamma beta alpha "), "反向迭代器遍历 - 拼接结果");

    // 迭代器运算
    auto it_begin = v_iter.begin();
    assert_equal(*(it_begin + 2), std::string("gamma"), "迭代器加法");
    assert_equal(*(v_iter.end() - 1), std::string("gamma"), "迭代器减法");
    assert_true(it_begin < v_iter.end(), "迭代器比较");

    std::cout << std::endl;

    // 6. 运算符测试（新增赋值后全量元素校验）
    std::cout << "--- 6. 运算符测试 ---" << std::endl;
    demo::vector<std::string> v_op1{"a", "b"}, v_op2{"a", "b"}, v_op3{"a", "c"};
    assert_true(v_op1 == v_op2, "== 运算符 - 相等");
    assert_true(v_op1 != v_op3, "!= 运算符 - 不相等");

    // 拷贝赋值
    demo::vector<std::string> v_assign;
    v_assign = v_op1;
    assert_vector_equal(v_assign, std::vector<std::string>{"a", "b"}, "拷贝赋值 - 全量元素");

    // 初始化列表赋值
    v_assign = {"x", "y", "z"};
    assert_vector_equal(v_assign, std::vector<std::string>{"x", "y", "z"}, "初始化列表赋值 - 全量元素");

    // 移动赋值
    demo::vector<std::string> v_move_assign;
    v_move_assign = std::move(v_assign);
    assert_vector_equal(v_move_assign, std::vector<std::string>{"x", "y", "z"}, "移动赋值 - 新对象全量元素");
    assert_vector_equal(v_assign, std::vector<std::string>{}, "移动赋值 - 原对象全量元素");

    // 7. 新增边界场景测试
    std::cout << "\n--- 7. 边界场景测试 ---" << std::endl;
    // 测试insert到开头
    demo::vector<std::string> v_edge{"b", "c"};
    v_edge.insert(v_edge.begin(), "a");
    assert_vector_equal(v_edge, std::vector<std::string>{"a", "b", "c"}, "insert - 插入到开头");

    // 测试erase最后一个元素
    v_edge.erase(v_edge.end() - 1);
    assert_vector_equal(v_edge, std::vector<std::string>{"a", "b"}, "erase - 删除最后一个元素");

    // 测试连续push_back触发扩容
    demo::vector<std::string> v_expand;
    for (int i = 0; i < 10; ++i)
    {
        v_expand.push_back(std::to_string(i));
    }
    std::vector<std::string> expected_expand;
    for (int i = 0; i < 10; ++i)
    {
        expected_expand.push_back(std::to_string(i));
    }
    assert_vector_equal(v_expand, expected_expand, "push_back - 扩容后全量元素");

    std::cout << "\n========== 所有测试完成 ==========" << std::endl;
}

int main()
{
    try
    {
        test_demo_vector_string();
    }
    catch (const std::exception &e)
    {
        std::cerr << "\n  测试过程中发生未捕获的异常: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}