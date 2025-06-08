cmake_minimum_required(VERSION 3.10)
project(GraphAnalyzerTests)

enable_testing()

# 添加 GTest 源码子目录
add_subdirectory(external/googletest)

# 检查是否启用代码覆盖率
option(BUILD_CODE_COVERAGE "Enable code coverage instrumentation" ON)

# 添加你的测试可执行文件
add_executable(GraphAnalyzerTests
    test_main.cpp
    graph_analyzer.cpp
    graph_analyzer.h
)

# 链接 GTest
target_link_libraries(GraphAnalyzerTests PRIVATE gtest gtest_main)

if (BUILD_CODE_COVERAGE)
    # 使用 target_compile_options 为编译阶段添加 GCOV 标志和调试信息
    # -fprofile-arcs 和 -ftest-coverage 用于生成 .gcno 文件 (编译阶段)
    # -g 用于生成调试信息，GCOV 需要它 (编译阶段)
    target_compile_options(GraphAnalyzerTests PRIVATE -fprofile-arcs -ftest-coverage -g)

    # 使用 target_link_options 为链接阶段添加 GCOV 标志
    # -fprofile-arcs 和 -ftest-coverage 也需要在链接阶段使用
    target_link_options(GraphAnalyzerTests PRIVATE -fprofile-arcs -ftest-coverage)
endif()

# 启用测试发现
include(GoogleTest)
gtest_discover_tests(GraphAnalyzerTests)
