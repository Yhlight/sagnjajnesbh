# 查找测试框架
find_package(GTest QUIET)

if(NOT GTest_FOUND)
    # 如果没有找到GTest，使用简单的测试框架
    message(STATUS "GTest not found, using simple test framework")
    
    # CHTL核心测试
    add_executable(test_chtl_core
        CHTL/test_chtl_core.cpp
    )
    
    target_link_libraries(test_chtl_core
        chtl_core
        utils
    )
    
    add_test(NAME CHTLCoreTest COMMAND test_chtl_core)
    
    # CHTL AST测试
    add_executable(test_chtl_ast
        CHTL/test_chtl_ast.cpp
    )
    
    target_link_libraries(test_chtl_ast
        chtl_core
        utils
    )
    
    add_test(NAME CHTLASTTest COMMAND test_chtl_ast)
    
    # CHTL编译器完整测试
    add_executable(test_chtl_compiler
        CHTL/test_chtl_compiler.cpp
    )
    
    target_link_libraries(test_chtl_compiler
        chtl_core
        utils
    )
    
    add_test(NAME CHTLCompilerTest COMMAND test_chtl_compiler)
    
    # CHTL简单编译器测试
    add_executable(test_chtl_simple
        CHTL/test_chtl_simple.cpp
    )
    
    target_link_libraries(test_chtl_simple
        chtl_core
        utils
    )
    
    add_test(NAME CHTLSimpleTest COMMAND test_chtl_simple)
    
    # CHTL JS核心测试
    add_executable(test_chtljs_core
        CHTLJS/test_chtljs_core.cpp
    )
    
    target_link_libraries(test_chtljs_core
        chtljs_core
        utils
    )
    
    add_test(NAME CHTLJSCoreTest COMMAND test_chtljs_core)
    
    # 扫描器测试
    add_executable(test_scanner
        Scanner/test_scanner.cpp
    )
    
    target_link_libraries(test_scanner
        scanner
        utils
    )
    
    add_test(NAME ScannerTest COMMAND test_scanner)
    
    # 集成测试
    add_executable(test_integration
        Integration/test_integration.cpp
    )
    
    target_link_libraries(test_integration
        chtl_core
        chtljs_core
        scanner
        dispatcher
        css_compiler
        js_compiler
        utils
        ${ANTLR4_LIBRARIES}
    )
    
    add_test(NAME IntegrationTest COMMAND test_integration)
    
else()
    # 使用GTest
    enable_testing()
    
    # CHTL核心测试
    add_executable(test_chtl_core
        CHTL/test_chtl_core.cpp
    )
    
    target_link_libraries(test_chtl_core
        chtl_core
        utils
        GTest::gtest_main
    )
    
    gtest_discover_tests(test_chtl_core)
    
    # CHTL AST测试
    add_executable(test_chtl_ast
        CHTL/test_chtl_ast.cpp
    )
    
    target_link_libraries(test_chtl_ast
        chtl_core
        utils
        GTest::gtest_main
    )
    
    gtest_discover_tests(test_chtl_ast)
    
    # CHTL编译器完整测试
    add_executable(test_chtl_compiler
        CHTL/test_chtl_compiler.cpp
    )
    
    target_link_libraries(test_chtl_compiler
        chtl_core
        utils
        GTest::gtest_main
    )
    
    gtest_discover_tests(test_chtl_compiler)
    
    # CHTL JS核心测试
    add_executable(test_chtljs_core
        CHTLJS/test_chtljs_core.cpp
    )
    
    target_link_libraries(test_chtljs_core
        chtljs_core
        utils
        GTest::gtest_main
    )
    
    gtest_discover_tests(test_chtljs_core)
    
    # 扫描器测试
    add_executable(test_scanner
        Scanner/test_scanner.cpp
    )
    
    target_link_libraries(test_scanner
        scanner
        utils
        GTest::gtest_main
    )
    
    gtest_discover_tests(test_scanner)
    
    # 集成测试
    add_executable(test_integration
        Integration/test_integration.cpp
    )
    
    target_link_libraries(test_integration
        chtl_core
        chtljs_core
        scanner
        dispatcher
        css_compiler
        js_compiler
        utils
        GTest::gtest_main
        ${ANTLR4_LIBRARIES}
    )
    
    gtest_discover_tests(test_integration)
endif()