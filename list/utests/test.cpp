#include <iostream>
#include <string>
#include <stdexcept>
#include <functional>
#include <forward_list> // 用于生成预期结果
#include <sstream>      // 用于forward_list_to_string
#include "forward_list.h"

// 工具1：对比两个forward_list的所有元素（逐元素校验）
template <typename T>
bool forward_list_equals(const demo::forward_list<T> &actual, const std::forward_list<T> &expected)
{
    auto it_actual = actual.begin();
    auto it_expected = expected.begin();

    for (; it_actual != actual.end() && it_expected != expected.end(); ++it_actual, ++it_expected)
    {
        if (*it_actual != *it_expected)
            return false;
    }

    // 检查是否都到达末尾
    return (it_actual == actual.end() && it_expected == expected.end());
}

// 工具2：打印forward_list的完整内容（调试用）
template <typename T>
std::string forward_list_to_string(const demo::forward_list<T> &flist)
{
    std::ostringstream oss;
    oss << "[";
    bool first = true;
    for (const auto &elem : flist)
    {
        if (!first)
            oss << ", ";
        oss << "\"" << elem << "\"";
        first = false;
    }
    oss << "]";
    return oss.str();
}

// 工具3：断言两个forward_list内容完全相等（带详细错误提示）
template <typename T>
void assert_forward_list_equal(const demo::forward_list<T> &actual, const std::forward_list<T> &expected, const std::string &test_name)
{
    if (forward_list_equals(actual, expected))
    {
        std::cout << " 测试通过: " << test_name << " | 内容: " << forward_list_to_string(actual) << std::endl;
    }
    else
    {
        std::cerr << " 测试失败: " << test_name << std::endl;
        std::cerr << "    实际内容: " << forward_list_to_string(actual) << std::endl;
        std::cerr << "    预期内容: [";
        bool first = true;
        for (const auto &elem : expected)
        {
            if (!first)
                std::cerr << ", ";
            std::cerr << "\"" << elem << "\"";
            first = false;
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

// 计算forward_list的元素个数（辅助测试）
template <typename T>
size_t forward_list_size(const demo::forward_list<T> &flist)
{
    size_t count = 0;
    for ([[maybe_unused]] const auto &elem : flist)
    {
        count++;
    }
    return count;
}

// 测试demo::forward_list<std::string>的所有核心功能
void test_demo_forward_list_string()
{
    std::cout << "========== 开始测试 demo::forward_list<std::string> ==========\n"
              << std::endl;

    // 1. 构造函数测试
    std::cout << "--- 1. 构造函数测试 ---" << std::endl;
    // 空构造
    demo::forward_list<std::string> fl1;
    assert_true(fl1.empty(), "空构造 - empty");
    assert_forward_list_equal(fl1, std::forward_list<std::string>{}, "空构造 - 全量元素校验");

    // 大小构造
    demo::forward_list<std::string> fl2(3);
    assert_equal(forward_list_size(fl2), 3ULL, "大小构造 - size");
    std::forward_list<std::string> expected_fl2(3, "");
    assert_forward_list_equal(fl2, expected_fl2, "大小构造 - 全量元素校验");

    // 大小+值构造
    demo::forward_list<std::string> fl3(2, "hello");
    assert_equal(forward_list_size(fl3), 2ULL, "大小+值构造 - size");
    std::forward_list<std::string> expected_fl3(2, "hello");
    assert_forward_list_equal(fl3, expected_fl3, "大小+值构造 - 全量元素校验");

    // 迭代器范围构造
    std::string arr[] = {"a", "b", "c"};
    demo::forward_list<std::string> fl4(std::begin(arr), std::end(arr));
    std::forward_list<std::string> expected_fl4{"a", "b", "c"};
    assert_forward_list_equal(fl4, expected_fl4, "迭代器构造 - 全量元素校验");

    // 初始化列表构造
    demo::forward_list<std::string> fl5{"apple", "banana", "cherry"};
    std::forward_list<std::string> expected_fl5{"apple", "banana", "cherry"};
    assert_forward_list_equal(fl5, expected_fl5, "初始化列表构造 - 全量元素校验");

    // 拷贝构造
    demo::forward_list<std::string> fl6(fl5);
    assert_forward_list_equal(fl6, expected_fl5, "拷贝构造 - 全量元素校验");

    // 移动构造
    demo::forward_list<std::string> fl7(std::move(fl5));
    assert_forward_list_equal(fl7, expected_fl5, "移动构造 - 新对象全量元素");
    assert_forward_list_equal(fl5, std::forward_list<std::string>{}, "移动构造 - 原对象全量元素");

    std::cout << std::endl;

    // 2. 元素访问与迭代器测试
    std::cout << "--- 2. 元素访问与迭代器测试 ---" << std::endl;
    demo::forward_list<std::string> fl_access{"one", "two", "three"};

    // 普通迭代器遍历
    std::string concat;
    for (demo::forward_list<std::string>::iterator it = fl_access.begin(); it != fl_access.end(); ++it)
    {
        concat += *it + " ";
    }
    assert_equal(concat, std::string("one two three "), "普通迭代器遍历 - 拼接结果");

    // const迭代器遍历
    std::string const_concat;
    for (demo::forward_list<std::string>::const_iterator it = fl_access.cbegin(); it != fl_access.cend(); ++it)
    {
        const_concat += *it + " ";
    }
    assert_equal(const_concat, std::string("one two three "), "const迭代器遍历 - 拼接结果");

    // before_begin迭代器
    auto before_begin_it = fl_access.before_begin();
    assert_true(++before_begin_it == fl_access.begin(), "before_begin迭代器 - 自增后指向第一个元素");

    // 首元素访问（通过迭代器）
    assert_equal(*fl_access.begin(), std::string("one"), "begin() - 访问第一个元素");

    std::cout << std::endl;

    // 3. 容量操作测试
    std::cout << "--- 3. 容量操作测试 ---" << std::endl;
    demo::forward_list<std::string> fl_cap;
    assert_true(fl_cap.empty(), "空列表 - empty");
    assert_true(fl_cap.max_size() > 0, "max_size - 有效最大值");

    fl_cap.push_front("test");
    assert_true(!fl_cap.empty(), "push_front后 - not empty");
    assert_equal(forward_list_size(fl_cap), 1ULL, "push_front后 - size=1");

    // resize测试
    fl_cap.resize(3, "resize_val");
    assert_equal(forward_list_size(fl_cap), 3ULL, "resize扩容 - size=3");
    std::forward_list<std::string> expected_resize1{"test", "resize_val", "resize_val"};
    assert_forward_list_equal(fl_cap, expected_resize1, "resize扩容 - 全量元素校验");

    fl_cap.resize(2);
    assert_equal(forward_list_size(fl_cap), 2ULL, "resize缩容 - size=2");
    std::forward_list<std::string> expected_resize2{"test", "resize_val"};
    assert_forward_list_equal(fl_cap, expected_resize2, "resize缩容 - 全量元素校验");

    std::cout << std::endl;

    // 4. 修改操作测试
    std::cout << "--- 4. 修改操作测试 ---" << std::endl;
    demo::forward_list<std::string> fl_mod{"cat", "dog"};
    assert_forward_list_equal(fl_mod, std::forward_list<std::string>{"cat", "dog"}, "初始状态 - 全量元素");

    // push_front
    fl_mod.push_front("fish");
    assert_forward_list_equal(fl_mod, std::forward_list<std::string>{"fish", "cat", "dog"}, "push_front - 新增单个元素");
    fl_mod.push_front(std::string("bird"));
    assert_forward_list_equal(fl_mod, std::forward_list<std::string>{"bird", "fish", "cat", "dog"}, "push_front - 移动语义新增");

    // emplace_front
    fl_mod.emplace_front("tiger", 3); // 构造"tig"
    assert_forward_list_equal(fl_mod, std::forward_list<std::string>{"tig", "bird", "fish", "cat", "dog"}, "emplace_front - 原地构造");

    // pop_front
    fl_mod.pop_front();
    assert_forward_list_equal(fl_mod, std::forward_list<std::string>{"bird", "fish", "cat", "dog"}, "pop_front - 删除第一个元素");
    assert_throws("pop_front - 空forward_list", [&]()
                  { demo::forward_list<std::string>().pop_front(); });

    // insert_after
    auto pos = fl_mod.before_begin(); // 在第一个元素前插入
    fl_mod.insert_after(pos, "rabbit");
    assert_forward_list_equal(fl_mod, std::forward_list<std::string>{"rabbit", "bird", "fish", "cat", "dog"}, "insert_after - 单个元素插入");

    // insert_after 多个元素
    pos = fl_mod.end();
    pos = fl_mod.insert_after(fl_mod.before_begin(), 2, "snake");
    assert_forward_list_equal(fl_mod, std::forward_list<std::string>{"snake", "snake", "rabbit", "bird", "fish", "cat", "dog"}, "insert_after - 多个元素插入");

    // erase_after
    pos = fl_mod.before_begin();
    pos = fl_mod.erase_after(pos); // 删除第一个元素
    assert_forward_list_equal(fl_mod, std::forward_list<std::string>{"snake", "rabbit", "bird", "fish", "cat", "dog"}, "erase_after - 单个元素删除");

    // erase_after 范围
    auto first = fl_mod.begin();
    auto last = fl_mod.end();
    ++first;
    ++first;
    ++first;                                            // 跳过前3个元素
    fl_mod.erase_after(++fl_mod.before_begin(), first); // 删除第2-3个元素
    assert_forward_list_equal(fl_mod, std::forward_list<std::string>{"snake", "fish", "cat", "dog"}, "erase_after - 范围元素删除");

    // clear
    fl_mod.clear();
    assert_forward_list_equal(fl_mod, std::forward_list<std::string>{}, "clear - 清空所有元素");

    // swap
    demo::forward_list<std::string> fl_swap1{"x", "y"}, fl_swap2{"1", "2", "3"};
    fl_swap1.swap(fl_swap2);
    assert_forward_list_equal(fl_swap1, std::forward_list<std::string>{"1", "2", "3"}, "swap - 第一个forward_list全量元素");
    assert_forward_list_equal(fl_swap2, std::forward_list<std::string>{"x", "y"}, "swap - 第二个forward_list全量元素");

    // assign 大小+值
    fl_mod.assign(2, "new_val");
    assert_forward_list_equal(fl_mod, std::forward_list<std::string>{"new_val", "new_val"}, "assign - 大小+值全量元素");

    // assign 初始化列表
    fl_mod.assign({"a", "b", "c"});
    assert_forward_list_equal(fl_mod, std::forward_list<std::string>{"a", "b", "c"}, "assign - 初始化列表全量元素");

    std::cout << std::endl;

    // 5. 特殊操作测试（merge/splice/sort/remove/unique/reverse）
    std::cout << "--- 5. 特殊操作测试 ---" << std::endl;

    // merge测试
    demo::forward_list<std::string> fl_merge1{"a", "c", "e"}, fl_merge2{"b", "d", "f"};
    fl_merge1.merge(fl_merge2);
    std::forward_list<std::string> expected_merge{"a", "b", "c", "d", "e", "f"};
    assert_forward_list_equal(fl_merge1, expected_merge, "merge - 合并有序列表");

    // splice_after测试
    demo::forward_list<std::string> fl_splice1{"1", "2", "3"}, fl_splice2{"x", "y"};
    fl_splice1.splice_after(fl_splice1.before_begin(), fl_splice2);
    std::forward_list<std::string> expected_splice{"x", "y", "1", "2", "3"};
    assert_forward_list_equal(fl_splice1, expected_splice, "splice_after - 拼接整个列表");

    // sort测试
    demo::forward_list<std::string> fl_sort{"banana", "apple", "cherry"};
    fl_sort.sort();
    std::forward_list<std::string> expected_sort{"apple", "banana", "cherry"};
    assert_forward_list_equal(fl_sort, expected_sort, "sort - 升序排序");

    // remove测试
    demo::forward_list<std::string> fl_remove{"a", "b", "a", "c", "a"};
    size_t remove_count = fl_remove.remove("a");
    assert_equal(remove_count, 3ULL, "remove - 删除元素个数");
    assert_forward_list_equal(fl_remove, std::forward_list<std::string>{"b", "c"}, "remove - 剩余元素校验");

    // unique测试
    demo::forward_list<std::string> fl_unique{"a", "a", "b", "b", "b", "c"};
    size_t unique_count = fl_unique.unique();
    assert_equal(unique_count, 3ULL, "unique - 去重元素个数");
    assert_forward_list_equal(fl_unique, std::forward_list<std::string>{"a", "b", "c"}, "unique - 去重后元素校验");

    // reverse测试
    demo::forward_list<std::string> fl_reverse{"1", "2", "3", "4"};
    fl_reverse.reverse();
    std::forward_list<std::string> expected_reverse{"4", "3", "2", "1"};
    assert_forward_list_equal(fl_reverse, expected_reverse, "reverse - 反转列表");

    std::cout << std::endl;

    // 6. 运算符测试
    std::cout << "--- 6. 运算符测试 ---" << std::endl;
    demo::forward_list<std::string> fl_op1{"a", "b"}, fl_op2{"a", "b"}, fl_op3{"a", "c"};
    assert_true(fl_op1 == fl_op2, "== 运算符 - 相等");
    assert_true(fl_op1 != fl_op3, "!= 运算符 - 不相等");

    // 拷贝赋值
    demo::forward_list<std::string> fl_assign;
    fl_assign = fl_op1;
    assert_forward_list_equal(fl_assign, std::forward_list<std::string>{"a", "b"}, "拷贝赋值 - 全量元素");

    // 初始化列表赋值
    fl_assign = {"x", "y", "z"};
    assert_forward_list_equal(fl_assign, std::forward_list<std::string>{"x", "y", "z"}, "初始化列表赋值 - 全量元素");

    // 移动赋值
    demo::forward_list<std::string> fl_move_assign;
    fl_move_assign = std::move(fl_assign);
    assert_forward_list_equal(fl_move_assign, std::forward_list<std::string>{"x", "y", "z"}, "移动赋值 - 新对象全量元素");
    assert_forward_list_equal(fl_assign, std::forward_list<std::string>{}, "移动赋值 - 原对象全量元素");

    // 7. 边界场景测试
    std::cout << "\n--- 7. 边界场景测试 ---" << std::endl;
    // 测试insert_after到末尾
    demo::forward_list<std::string> fl_edge{"a", "b"};
    auto last_pos = fl_edge.before_begin();
    for (auto it = fl_edge.begin(); it != fl_edge.end(); ++it)
        ++last_pos;
    fl_edge.insert_after(last_pos, "c");
    assert_forward_list_equal(fl_edge, std::forward_list<std::string>{"a", "b", "c"}, "insert_after - 插入到末尾");

    // 测试连续push_front
    demo::forward_list<std::string> fl_expand;
    for (int i = 0; i < 5; ++i)
    {
        fl_expand.push_front(std::to_string(i));
    }
    std::forward_list<std::string> expected_expand{"4", "3", "2", "1", "0"};
    assert_forward_list_equal(fl_expand, expected_expand, "连续push_front - 全量元素校验");

    std::cout << "\n========== 所有测试完成 ==========" << std::endl;
}

int main()
{
    try
    {
        test_demo_forward_list_string();
    }
    catch (const std::exception &e)
    {
        std::cerr << "\n  测试过程中发生未捕获的异常: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}