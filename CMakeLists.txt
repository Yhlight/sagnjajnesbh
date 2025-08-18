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
    src/ast/VirtualObjectManager.cpp
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
