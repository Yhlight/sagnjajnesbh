enable_testing()

# 添加测试可执行文件
add_executable(test_scanner test_scanner.cpp)
target_link_libraries(test_scanner chtl_scanner)

add_executable(test_chtl_compiler test_chtl_compiler.cpp)
target_link_libraries(test_chtl_compiler chtl_compiler_lib)

add_executable(test_integration test_integration.cpp)
target_link_libraries(test_integration 
    chtl_scanner
    chtl_dispatcher
    chtl_compiler_lib
    chtl_js_compiler_lib
    css_compiler_lib
    javascript_compiler_lib
    chtl_merger
)

# 添加测试
add_test(NAME scanner_test COMMAND test_scanner)
add_test(NAME chtl_compiler_test COMMAND test_chtl_compiler)
add_test(NAME integration_test COMMAND test_integration)