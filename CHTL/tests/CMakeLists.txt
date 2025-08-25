cmake_minimum_required(VERSION 3.16)

# 基础组件测试
add_executable(test_chtl_basic test_chtl_basic.cpp)
target_link_libraries(test_chtl_basic CHTLCore)

# CHTL JS基础组件测试
add_executable(test_chtljs_basic test_chtljs_basic.cpp)
target_link_libraries(test_chtljs_basic CHTLCore)

# 添加测试
add_test(NAME test_chtl_basic COMMAND test_chtl_basic)
add_test(NAME test_chtljs_basic COMMAND test_chtljs_basic)