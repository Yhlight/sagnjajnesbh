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
    src/Shared/AST/CommentNode.cpp
    src/Shared/AST/CommentGenerator.cpp
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

# CHTL JS增强功能组件
add_library(chtl_js_enhanced STATIC
    src/CHTLJSCompiler/Core/JSEventRegistry.cpp
    src/CHTLJSCompiler/Parser/CHTLJSFunctionParser.cpp
)
target_link_libraries(chtl_js_enhanced chtl_shared)

# 自动添加功能组件
add_library(chtl_auto_addition STATIC
    src/CHTLJSCompiler/Parser/SelectorDetector.cpp
    src/CHTLJSCompiler/Core/AutoAdditionManager.cpp
)
target_link_libraries(chtl_auto_addition chtl_shared)

# Chtholly 珂朵莉模块
add_library(chtholly_cjmod STATIC
    src/Module/Chtholly/CJMOD/src/printMylove.cpp
    src/Module/Chtholly/CJMOD/src/iNeverAway.cpp
    src/Module/Chtholly/CJMOD/src/ChthollyInterface.cpp
)
target_link_libraries(chtholly_cjmod chtl_shared)

# 新的CJMOD框架
add_library(chtl_new_cjmod STATIC
    src/CHTLJSCompiler/Module/CJMODFramework.cpp
    src/CHTLJSCompiler/Module/CHTLJSIntegration.cpp
)
target_link_libraries(chtl_new_cjmod chtl_shared)

# 无限制CJMOD框架
add_library(chtl_unlimited_cjmod STATIC
    src/CHTLJSCompiler/Module/UnlimitedCJMOD.cpp
)
target_link_libraries(chtl_unlimited_cjmod chtl_shared)

# 测试程序
add_executable(test_chtl_compiler test/test_chtl_compiler.cpp)
target_link_libraries(test_chtl_compiler chtl_compiler)

add_executable(test_chtl_js_lexer test/test_chtl_js_lexer.cpp)
target_link_libraries(test_chtl_js_lexer chtl_js_compiler)

add_executable(test_chtl_compiler_simple test/test_chtl_compiler_simple.cpp)
target_link_libraries(test_chtl_compiler_simple chtl_compiler)

add_executable(test_undecorated_literals test/test_undecorated_literals.cpp)
target_link_libraries(test_undecorated_literals chtl_js_compiler)

add_executable(test_chtljs_enhanced_functions test/test_chtljs_enhanced_functions.cpp)
target_link_libraries(test_chtljs_enhanced_functions chtl_js_enhanced)

add_executable(test_auto_addition test/test_auto_addition.cpp)
target_link_libraries(test_auto_addition chtl_auto_addition)

add_executable(test_chtholly_module test/test_chtholly_module.cpp)
target_link_libraries(test_chtholly_module chtholly_cjmod)

add_executable(test_new_cjmod_framework test/test_new_cjmod_framework.cpp)
target_link_libraries(test_new_cjmod_framework chtl_new_cjmod)

add_executable(test_unlimited_cjmod_simple test/test_unlimited_cjmod_simple.cpp)
target_link_libraries(test_unlimited_cjmod_simple chtl_unlimited_cjmod)

add_executable(test_comment_system test/test_comment_system.cpp)
target_link_libraries(test_comment_system chtl_shared)

# 设置输出目录
set_target_properties(
    chtl_shared chtl_compiler chtl_js_compiler chtl_js_enhanced chtl_auto_addition 
    chtholly_cjmod chtl_new_cjmod chtl_unlimited_cjmod
    PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
)

set_target_properties(
    test_chtl_compiler test_chtl_js_lexer test_chtl_compiler_simple test_undecorated_literals
    test_chtljs_enhanced_functions test_auto_addition test_chtholly_module 
    test_new_cjmod_framework test_unlimited_cjmod_simple test_comment_system
    PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)