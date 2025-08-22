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
    
    # CHTL复杂语法测试
    add_executable(test_complex_syntax
        CHTL/test_complex_syntax.cpp
    )
    
    target_link_libraries(test_complex_syntax
        chtl_core
        utils
    )
    
    add_test(NAME CHTLComplexSyntaxTest COMMAND test_complex_syntax)
    
    # CHTL简单继承测试
    add_executable(test_inheritance_simple
        CHTL/test_inheritance_simple.cpp
    )
    
    target_link_libraries(test_inheritance_simple
        chtl_core
        utils
    )
    
    add_test(NAME CHTLInheritanceSimpleTest COMMAND test_inheritance_simple)
    
    # CHTL高级特性测试
    add_executable(test_advanced_features
        CHTL/test_advanced_features.cpp
    )
    
    target_link_libraries(test_advanced_features
        chtl_core
        utils
    )
    
    add_test(NAME CHTLAdvancedFeaturesTest COMMAND test_advanced_features)
    
    # CMOD系统测试
    add_executable(test_cmod
        CMOD/test_cmod.cpp
    )
    
    target_link_libraries(test_cmod
        cmod
        utils
    )
    
    add_test(NAME CMODTest COMMAND test_cmod)
    
    # CMOD简化测试
    add_executable(test_cmod_simple
        CMOD/test_cmod_simple.cpp
    )
    
    target_link_libraries(test_cmod_simple
        cmod
        utils
    )
    
    add_test(NAME CMODSimpleTest COMMAND test_cmod_simple)
    
    # CMOD自动导出测试
    add_executable(test_auto_export
        CMOD/test_auto_export.cpp
    )
    
    target_link_libraries(test_auto_export
        cmod
        utils
    )
    
    add_test(NAME CMODAutoExportTest COMMAND test_auto_export)
    
    # TestModule自动导出测试
    add_executable(test_testmodule_auto
        CMOD/test_testmodule_auto.cpp
    )
    
    target_link_libraries(test_testmodule_auto
        cmod
        utils
    )
    
    add_test(NAME TestModuleAutoTest COMMAND test_testmodule_auto)
    
    # CJMOD API测试
    add_executable(test_cjmod_api
        CJMOD/test_cjmod_api.cpp
    )
    
    target_link_libraries(test_cjmod_api
        cjmod
        utils
    )
    
    add_test(NAME CJMODApiTest COMMAND test_cjmod_api)
    
    # 珂朵莉与由比滨结衣模块测试
    add_executable(test_chtholly_yuigahama
        Modules/test_chtholly_yuigahama.cpp
    )
    
    target_link_libraries(test_chtholly_yuigahama
        cjmod
        cmod
        utils
    )
    
    add_test(NAME ChthollyYuigahamaTest COMMAND test_chtholly_yuigahama)
    
    # CHTL语法约束验证器测试
    add_executable(test_constraint_validator
        CHTL/test_constraint_validator_simple.cpp
    )
    
    target_link_libraries(test_constraint_validator
        chtl_core
        utils
    )
    
    add_test(NAME ConstraintValidatorTest COMMAND test_constraint_validator)
    
    # CHTL JS核心测试
    add_executable(test_chtljs_core
        CHTLJS/test_chtljs_core.cpp
    )
    
    target_link_libraries(test_chtljs_core
        chtljs_core
        utils
    )
    
    add_test(NAME CHTLJSCoreTest COMMAND test_chtljs_core)
    
    # CHTL JS核心功能完整测试
    add_executable(test_chtljs_core_complete
        CHTLJS/test_chtljs_core_complete.cpp
    )
    
    target_link_libraries(test_chtljs_core_complete
        chtljs_core
        utils
    )
    
    add_test(NAME CHTLJSCoreCompleteTest COMMAND test_chtljs_core_complete)
    
    # CHTL JS Parser和Generator测试
    add_executable(test_chtljs_parser_generator
        CHTLJS/test_chtljs_parser_generator.cpp
    )
    
    target_link_libraries(test_chtljs_parser_generator
        chtljs_core
        utils
    )
    
    add_test(NAME CHTLJSParserGeneratorTest COMMAND test_chtljs_parser_generator)
    
    # CHTL JS简单Parser测试
    add_executable(test_chtljs_simple_parser
        CHTLJS/test_chtljs_simple_parser.cpp
    )
    
    target_link_libraries(test_chtljs_simple_parser
        chtljs_core
        utils
    )
    
    add_test(NAME CHTLJSSimpleParserTest COMMAND test_chtljs_simple_parser)
    
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