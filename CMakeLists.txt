cmake_minimum_required(VERSION 3.16)
project(CHTLANTLRTest VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wno-unused-parameter -Wno-unused-variable")

include_directories(src)
include_directories(external/antlr4-install/include/antlr4-runtime)

# 共享组件（最小化）
add_library(chtl_shared_min STATIC
    src/Shared/Utils/FileUtils.cpp
    src/Shared/Core/NamespaceAlgorithms.cpp
    src/Shared/Core/ErrorHandler.cpp
    src/Shared/Common/ModulePathManager.cpp
    src/CHTLCompiler/Import/ImportManager.cpp
    src/UnifiedCore/Configuration/ConfigurationManager.cpp
    src/UnifiedCore/Configuration/ConfigurationImporter.cpp
    src/UnifiedCore/Export/ExportGenerator.cpp
    src/CHTLCompiler/Template/TemplateManager.cpp
    src/CHTLCompiler/Template/TemplateParser.cpp
    src/CHTLCompiler/Custom/CustomManager.cpp
    src/CHTLCompiler/Custom/CustomParser.cpp
    src/CHTLCompiler/Origin/OriginManager.cpp
    src/CHTLCompiler/Origin/OriginParser.cpp
    src/CHTLCompiler/Constraint/ConstraintManager.cpp
    src/CHTLCompiler/Constraint/ConstraintParser.cpp
    src/CHTLJSCompiler/Selector/EnhancedSelectorProcessor.cpp
    src/CHTLJSCompiler/Event/ListenSystemProcessor.cpp
    src/CHTLJSCompiler/Event/DelegateSystemProcessor.cpp
    src/CHTLJSCompiler/Animation/AnimateSystemProcessor.cpp
    src/CHTLJSCompiler/Virtual/VirtualObjectProcessor.cpp
    src/CHTLJSCompiler/Operator/ArrowOperatorProcessor.cpp
    src/CHTLJSCompiler/Script/LocalScriptProcessor.cpp
    src/CHTLJSCompiler/CHTLJSCompiler.cpp
)

# CSS编译器（使用ANTLR）
add_library(chtl_css_compiler STATIC
    src/CSSCompiler/Parser/css3Lexer.cpp
    src/CSSCompiler/Parser/css3Parser.cpp
    src/CSSCompiler/Parser/css3ParserBaseListener.cpp
    src/CSSCompiler/Parser/css3ParserListener.cpp
    src/CSSCompiler/Core/CSSCompilerCore.cpp
)
target_link_libraries(chtl_css_compiler chtl_shared_min ${CMAKE_CURRENT_SOURCE_DIR}/external/antlr4-install/lib/libantlr4-runtime.a)

# JavaScript编译器（使用ANTLR）
add_library(chtl_js_compiler_antlr STATIC
    src/JavaScriptCompiler/Parser/JavaScriptLexer_cpp.cpp
    src/JavaScriptCompiler/Parser/JavaScriptParser_cpp.cpp
    src/JavaScriptCompiler/Parser/JavaScriptParser_cppBaseListener.cpp
    src/JavaScriptCompiler/Parser/JavaScriptParser_cppListener.cpp
    src/JavaScriptCompiler/Parser/JavaScriptLexerBase.cpp
    src/JavaScriptCompiler/Parser/JavaScriptParserBase.cpp
    src/JavaScriptCompiler/Core/JavaScriptCompilerCore.cpp
)
target_link_libraries(chtl_js_compiler_antlr chtl_shared_min ${CMAKE_CURRENT_SOURCE_DIR}/external/antlr4-install/lib/libantlr4-runtime.a)

# ANTLR编译器测试
add_executable(test_antlr_compilers test/test_antlr_compilers.cpp)
target_link_libraries(test_antlr_compilers chtl_css_compiler chtl_js_compiler_antlr)

# 基本ANTLR测试
add_executable(test_antlr_basic test/test_antlr_basic.cpp)
target_link_libraries(test_antlr_basic chtl_css_compiler chtl_js_compiler_antlr)

# 最小CSS测试
add_executable(test_minimal_css test/test_minimal_css.cpp)
target_link_libraries(test_minimal_css chtl_css_compiler)

# CSS验证测试
add_executable(test_css_validation_only test/test_css_validation_only.cpp)
target_link_libraries(test_css_validation_only chtl_css_compiler)

# 复杂语法错误测试
add_executable(test_complex_syntax_errors test/test_complex_syntax_errors.cpp)
target_link_libraries(test_complex_syntax_errors chtl_css_compiler chtl_js_compiler_antlr)

# CHTL测试框架
add_library(chtl_test_framework STATIC
    src/Test/Framework/TestBase.cpp
    src/Test/Framework/TestRunner.cpp
    src/Test/Utils/TestUtils.cpp
    src/Test/Mocks/MockFileSystem.cpp
)
target_link_libraries(chtl_test_framework chtl_shared_min)

# 简化测试运行器
add_executable(SimpleTestRunner src/Test/SimpleTestRunner.cpp)
target_link_libraries(SimpleTestRunner chtl_shared_min)

# 全面语法测试运行器
add_executable(ComprehensiveTestRunner 
    src/Test/ComprehensiveTestRunner.cpp
    src/Test/Utils/CHTLSyntaxValidator.cpp
)
target_link_libraries(ComprehensiveTestRunner chtl_shared_min)

# 设置输出目录
set_target_properties(
    chtl_shared_min chtl_css_compiler chtl_js_compiler_antlr chtl_test_framework
    PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
)

set_target_properties(
    test_antlr_compilers test_antlr_basic test_minimal_css test_css_validation_only test_complex_syntax_errors SimpleTestRunner ComprehensiveTestRunner
    PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)