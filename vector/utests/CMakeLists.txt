cmake_minimum_required(VERSION 3.16)

# 设置项目名称
project(vector_tests LANGUAGES CXX)

# 指定 C++ 标准版本（C++17）
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# 设置编译选项
if(MSVC)
    # MSVC 编译器选项
    add_compile_options(
        /W4                     # 启用警告级别4
        #/WX                     # 将警告视为错误
        /EHsc                   # 启用标准异常处理
        /utf-8                  # 使用UTF-8编码
    )
else()
    # GCC/Clang 编译器选项
    add_compile_options(
        -Wall                   # 启用所有警告
        -Wextra                 # 启用额外警告
        #-Werror                 # 将警告视为错误
        -pedantic               # 启用严格标准检查
    )
endif()

# 包含头文件目录
include_directories(
    ${PROJECT_SOURCE_DIR}          # doctest.h 所在目录
    ${PROJECT_SOURCE_DIR}/..       # vector.h 所在目录
)

# 添加测试源文件
set(TEST_SOURCES
    test_vector.cpp
    main.cpp
)

# 添加测试目标（使用 doctest 的 IMPLEMENT_WITH_MAIN 模式）
add_executable(test ${TEST_SOURCES})

