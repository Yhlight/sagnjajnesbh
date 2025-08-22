add_executable(test_basic test_basic.cpp)

target_link_libraries(test_basic
    CHTL_Core
    CHTL_Compiler
)

target_include_directories(test_basic PRIVATE
    ${CMAKE_SOURCE_DIR}/include
)

# 添加测试
add_test(NAME BasicCHTLTest COMMAND test_basic)