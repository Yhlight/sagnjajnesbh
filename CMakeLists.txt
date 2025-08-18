cmake_minimum_required(VERSION 3.16)
project(CHTLCompiler VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wno-unused-parameter -Wno-unused-variable")

include_directories(src)

# 共享组件
add_library(chtl_shared STATIC
    src/Shared/SimpleZip/SimpleZip.cpp
    src/Shared/Utils/FileUtils.cpp
)

# AST组件（需要支持语法约束器）
add_library(chtl_ast STATIC
    src/ast/ASTNode.cpp
    src/CHTLJSCompiler/AST/VirtualObjectManager.cpp
)

# CHTL编译器（完整可用）
add_library(chtl_compiler STATIC
    src/CHTLCompiler/Lexer/CHTLLexer.cpp
    src/CHTLCompiler/Lexer/CHTLToken.cpp
    src/CHTLCompiler/Parser/CHTLContext.cpp
    src/CHTLCompiler/Parser/CHTLParser.cpp
    src/CHTLCompiler/AST/CHTLNodes.cpp
    src/CHTLCompiler/Generator/CHTLGenerator.cpp
    src/CHTLCompiler/Module/CmodManager.cpp
    src/CHTLCompiler/Core/CHTLCompilerCore.cpp
)
target_link_libraries(chtl_compiler chtl_shared)

# CHTL JS编译器（完整可用）
add_library(chtl_js_compiler STATIC
    src/CHTLJSCompiler/Lexer/CHTLJSLexer.cpp
    src/CHTLJSCompiler/Lexer/CHTLJSToken.cpp
    src/CHTLJSCompiler/AST/VirtualObjectManager.cpp
    src/CHTLJSCompiler/Module/CJmodManager.cpp
)
target_link_libraries(chtl_js_compiler chtl_shared)

# 统一核心（包含扫描器和约束器）
add_library(unified_core STATIC
    src/UnifiedCore/Scanner/CHTLUnifiedScanner.cpp
    src/UnifiedCore/Constraints/SyntaxConstrainer.cpp
)
target_link_libraries(unified_core chtl_shared chtl_ast)

# 测试程序
add_executable(test_chtl_compiler test/test_chtl_compiler.cpp)
target_link_libraries(test_chtl_compiler chtl_compiler)

add_executable(test_chtl_js_lexer test/test_chtl_js_lexer.cpp)
target_link_libraries(test_chtl_js_lexer chtl_js_compiler)

add_executable(test_unified_scanner test/test_unified_scanner.cpp)
target_link_libraries(test_unified_scanner unified_core)

add_executable(test_syntax_constraints test/test_syntax_constraints.cpp)
target_link_libraries(test_syntax_constraints unified_core)

# 设置输出目录
set_target_properties(
    chtl_shared chtl_ast chtl_compiler chtl_js_compiler unified_core
    PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
)

set_target_properties(
    test_chtl_compiler test_chtl_js_lexer test_unified_scanner test_syntax_constraints
    PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

# 简化的CHTL编译器测试
add_executable(test_chtl_compiler_simple test/test_chtl_compiler_simple.cpp)
target_link_libraries(test_chtl_compiler_simple chtl_compiler)

# 更新输出目录设置
set_target_properties(
    test_chtl_compiler_simple
    PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

# 无修饰字面量测试
add_executable(test_undecorated_literals test/test_undecorated_literals.cpp)
target_link_libraries(test_undecorated_literals chtl_js_compiler unified_core)

# 更新输出目录设置
set_target_properties(
    test_undecorated_literals
    PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

# 添加CHTL JS增强功能组件到CHTL JS编译器
set(CHTL_JS_ENHANCED_SOURCES
    src/CHTLJSCompiler/Core/JSEventRegistry.cpp
    src/CHTLJSCompiler/Parser/CHTLJSFunctionParser.cpp
)

# 更新CHTL JS编译器库
add_library(chtl_js_enhanced STATIC ${CHTL_JS_ENHANCED_SOURCES})
target_link_libraries(chtl_js_enhanced chtl_shared)

# CHTL JS增强功能测试
add_executable(test_chtljs_enhanced_functions test/test_chtljs_enhanced_functions.cpp)
target_link_libraries(test_chtljs_enhanced_functions chtl_js_enhanced)

# 更新输出目录设置
set_target_properties(
    chtl_js_enhanced test_chtljs_enhanced_functions
    PROPERTIES 
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

# 自动添加功能组件
set(AUTO_ADDITION_SOURCES
    src/CHTLJSCompiler/Parser/SelectorDetector.cpp
    src/CHTLJSCompiler/Core/AutoAdditionManager.cpp
)

# 自动添加功能库
add_library(chtl_auto_addition STATIC ${AUTO_ADDITION_SOURCES})
target_link_libraries(chtl_auto_addition chtl_shared)

# 自动添加功能测试
add_executable(test_auto_addition test/test_auto_addition.cpp)
target_link_libraries(test_auto_addition chtl_auto_addition)

# 更新输出目录设置
set_target_properties(
    chtl_auto_addition test_auto_addition
    PROPERTIES 
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

# Chtholly 珂朵莉模块
set(CHTHOLLY_CJMOD_SOURCES
    src/Module/Chtholly/CJMOD/src/printMylove.cpp
    src/Module/Chtholly/CJMOD/src/iNeverAway.cpp
    src/Module/Chtholly/CJMOD/src/ChthollyInterface.cpp
)

# Chtholly CJMOD库
add_library(chtholly_cjmod STATIC ${CHTHOLLY_CJMOD_SOURCES})
target_link_libraries(chtholly_cjmod chtl_shared)

# Chtholly模块测试
add_executable(test_chtholly_module test/test_chtholly_module.cpp)
target_link_libraries(test_chtholly_module chtholly_cjmod)

# 更新输出目录设置
set_target_properties(
    chtholly_cjmod test_chtholly_module
    PROPERTIES 
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

# 新的CJMOD框架
set(NEW_CJMOD_SOURCES
    src/CHTLJSCompiler/Module/CJMODFramework.cpp
    src/CHTLJSCompiler/Module/CHTLJSIntegration.cpp
)

# 新CJMOD框架库
add_library(chtl_new_cjmod STATIC ${NEW_CJMOD_SOURCES})
target_link_libraries(chtl_new_cjmod chtl_shared)

# 新CJMOD框架测试
add_executable(test_new_cjmod_framework test/test_new_cjmod_framework.cpp)
target_link_libraries(test_new_cjmod_framework chtl_new_cjmod)

# 更新输出目录设置
set_target_properties(
    chtl_new_cjmod test_new_cjmod_framework
    PROPERTIES 
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

# 无限制CJMOD框架
set(UNLIMITED_CJMOD_SOURCES
    src/CHTLJSCompiler/Module/UnlimitedCJMOD.cpp
)

# 无限制CJMOD库
add_library(chtl_unlimited_cjmod STATIC ${UNLIMITED_CJMOD_SOURCES})
target_link_libraries(chtl_unlimited_cjmod chtl_shared)

# 无限制CJMOD核心测试
add_executable(test_unlimited_cjmod_core test/test_unlimited_cjmod_core.cpp)
target_link_libraries(test_unlimited_cjmod_core chtl_unlimited_cjmod)

# 更新输出目录设置
set_target_properties(
    chtl_unlimited_cjmod test_unlimited_cjmod_core
    PROPERTIES 
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

# 无限制CJMOD简单测试
add_executable(test_unlimited_cjmod_simple test/test_unlimited_cjmod_simple.cpp)
target_link_libraries(test_unlimited_cjmod_simple chtl_unlimited_cjmod)

set_target_properties(
    test_unlimited_cjmod_simple
    PROPERTIES 
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

# ANTLR 4集成
set(ANTLR4_ROOT /workspace/external/antlr4-install)
set(ANTLR4_INCLUDE_DIRS ${ANTLR4_ROOT}/include/antlr4-runtime)
set(ANTLR4_LIBRARIES ${ANTLR4_ROOT}/lib/libantlr4-runtime.a)

# CSS编译器 (ANTLR官方语法)
set(CSS_COMPILER_SOURCES
    src/CSSCompiler/Parser/css3Lexer.cpp
    src/CSSCompiler/Parser/css3Parser.cpp
    src/CSSCompiler/Parser/css3ParserBaseListener.cpp
    src/CSSCompiler/Parser/css3ParserListener.cpp
    src/CSSCompiler/Core/CSSCompilerCore.cpp
)

# JavaScript编译器 (ANTLR官方语法)
set(JS_COMPILER_SOURCES
    src/JavaScriptCompiler/Parser/JavaScriptLexer_cpp.cpp
    src/JavaScriptCompiler/Parser/JavaScriptLexerBase.cpp
    src/JavaScriptCompiler/Parser/JavaScriptParser_cpp.cpp
    src/JavaScriptCompiler/Parser/JavaScriptParserBase.cpp
    src/JavaScriptCompiler/Parser/JavaScriptParser_cppBaseListener.cpp
    src/JavaScriptCompiler/Parser/JavaScriptParser_cppListener.cpp
    src/JavaScriptCompiler/Core/JavaScriptCompilerCore.cpp
)

# CSS编译器库
add_library(chtl_css_antlr STATIC ${CSS_COMPILER_SOURCES})
target_include_directories(chtl_css_antlr PRIVATE ${ANTLR4_INCLUDE_DIRS})
target_link_libraries(chtl_css_antlr ${ANTLR4_LIBRARIES} chtl_shared)

# JavaScript编译器库
add_library(chtl_js_antlr STATIC ${JS_COMPILER_SOURCES})
target_include_directories(chtl_js_antlr PRIVATE ${ANTLR4_INCLUDE_DIRS})
target_link_libraries(chtl_js_antlr ${ANTLR4_LIBRARIES} chtl_shared)

# ANTLR集成测试
add_executable(test_antlr_integration test/test_antlr_integration.cpp)
target_include_directories(test_antlr_integration PRIVATE ${ANTLR4_INCLUDE_DIRS})
target_link_libraries(test_antlr_integration chtl_css_antlr chtl_js_antlr)

# 更新输出目录设置
set_target_properties(
    chtl_css_antlr chtl_js_antlr test_antlr_integration
    PROPERTIES 
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

# 简单ANTLR测试 (只测试解析器)
set(ANTLR_PARSER_SOURCES
    src/CSSCompiler/Parser/css3Lexer.cpp
    src/CSSCompiler/Parser/css3Parser.cpp
    src/CSSCompiler/Parser/css3ParserBaseListener.cpp
    src/JavaScriptCompiler/Parser/JavaScriptLexer_cpp.cpp
    src/JavaScriptCompiler/Parser/JavaScriptLexerBase.cpp
    src/JavaScriptCompiler/Parser/JavaScriptParser_cpp.cpp
    src/JavaScriptCompiler/Parser/JavaScriptParserBase.cpp
    src/JavaScriptCompiler/Parser/JavaScriptParser_cppBaseListener.cpp
)

add_library(chtl_antlr_parsers STATIC ${ANTLR_PARSER_SOURCES})
target_include_directories(chtl_antlr_parsers PRIVATE ${ANTLR4_INCLUDE_DIRS})
target_link_libraries(chtl_antlr_parsers ${ANTLR4_LIBRARIES})

# 简单测试可执行文件
add_executable(test_antlr_basic 
    test/test_antlr_basic.cpp
)
target_include_directories(test_antlr_basic PRIVATE ${ANTLR4_INCLUDE_DIRS})
target_link_libraries(test_antlr_basic chtl_antlr_parsers)

set_target_properties(
    chtl_antlr_parsers test_antlr_basic
    PROPERTIES 
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)
