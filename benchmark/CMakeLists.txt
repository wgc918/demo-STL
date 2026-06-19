cmake_minimum_required(VERSION 3.16)

project(demo_stl_benchmark LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# 设置编译优化级别（根据构建类型）
if(MSVC)
    # MSVC 编译器设置
    add_compile_options(/EHsc /utf-8)
    
    # 根据构建类型设置优化选项
    set(CMAKE_CXX_FLAGS_RELEASE "/O2 /MD")
    set(CMAKE_CXX_FLAGS_DEBUG "/Od /MDd /RTC1")
else()
    add_compile_options(-O2)
endif()

# 包含头文件目录
include_directories(
    ${PROJECT_SOURCE_DIR}/..        # 各容器头文件所在目录
    ${PROJECT_SOURCE_DIR}           # 当前目录
)

# 收集所有 benchmark 源文件
set(BENCHMARK_SOURCES
    main.cpp
    benchmark_vector.cpp
    benchmark_forward_list.cpp
    benchmark_list.cpp
    benchmark_deque.cpp
    benchmark_map.cpp
    benchmark_multimap.cpp
    benchmark_set.cpp
    benchmark_multiset.cpp
    benchmark_unordered_map.cpp
    benchmark_unordered_multimap.cpp
    benchmark_unordered_set.cpp
    benchmark_unordered_multiset.cpp
    benchmark_stack.cpp
    benchmark_queue.cpp
    benchmark_priority_queue.cpp
)

# 添加可执行目标
add_executable(benchmark ${BENCHMARK_SOURCES})

# 创建结果输出目录
file(MAKE_DIRECTORY ${PROJECT_SOURCE_DIR}/results)