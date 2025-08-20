enable_testing()

# 查找测试文件
file(GLOB_RECURSE TEST_SOURCES *.cpp)

# 为每个测试文件创建可执行文件
foreach(test_source ${TEST_SOURCES})
    get_filename_component(test_name ${test_source} NAME_WE)
    add_executable(${test_name} ${test_source})
    
    # 链接必要的库
    target_link_libraries(${test_name}
        chtl_utils
        chtl_scanner
        chtl_core
        chtl_js_core
    )
    
    # 添加测试
    add_test(NAME ${test_name} COMMAND ${test_name})
endforeach()