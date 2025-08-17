cmake_minimum_required(VERSION 3.16)
project(CHTLCompiler VERSION 1.0.0 LANGUAGES CXX)

# 设置C++17标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 设置编译选项
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")

# 包含目录
include_directories(src)

# 共享组件源文件
set(SHARED_SOURCES
    src/Shared/SimpleZip/SimpleZip.cpp
    src/Shared/Utils/FileUtils.cpp
)

# CHTL编译器源文件（核心组件）
set(CHTL_COMPILER_SOURCES
    src/CHTLCompiler/Lexer/CHTLLexer.cpp
    src/CHTLCompiler/Lexer/CHTLToken.cpp
    src/CHTLCompiler/Parser/CHTLContext.cpp
    src/CHTLCompiler/Parser/CHTLParser.cpp
    src/CHTLCompiler/AST/CHTLNodes.cpp
    src/CHTLCompiler/Generator/CHTLGenerator.cpp
    src/CHTLCompiler/Module/CmodManager.cpp
    src/CHTLCompiler/Core/CHTLCompilerCore.cpp
)

# CHTL JS编译器源文件（最小可用版本）
set(CHTL_JS_COMPILER_SOURCES
    src/CHTLJSCompiler/Lexer/CHTLJSLexer.cpp
    src/CHTLJSCompiler/Lexer/CHTLJSToken.cpp
    src/CHTLJSCompiler/AST/VirtualObjectManager.cpp
    src/CHTLJSCompiler/Module/CJmodManager.cpp
)

# 统一核心源文件
set(UNIFIED_CORE_SOURCES
    src/UnifiedCore/Scanner/CHTLUnifiedScanner.cpp
    src/UnifiedCore/Dispatcher/CompilerDispatcher.cpp
    src/UnifiedCore/Constraints/SyntaxConstrainer.cpp
    src/UnifiedCore/Import/ImportManager.cpp
)

# 创建库
add_library(chtl_shared STATIC ${SHARED_SOURCES})
add_library(chtl_compiler STATIC ${CHTL_COMPILER_SOURCES})
add_library(chtl_js_compiler STATIC ${CHTL_JS_COMPILER_SOURCES})
add_library(unified_core STATIC ${UNIFIED_CORE_SOURCES})

# 设置依赖关系
target_link_libraries(chtl_compiler chtl_shared)
target_link_libraries(chtl_js_compiler chtl_shared)
target_link_libraries(unified_core chtl_shared)

# 测试程序
add_executable(test_chtl_compiler test/test_chtl_compiler.cpp)
target_link_libraries(test_chtl_compiler chtl_compiler)

add_executable(test_chtl_js_lexer test/test_chtl_js_lexer.cpp)
target_link_libraries(test_chtl_js_lexer chtl_js_compiler)

add_executable(test_unified_scanner test/test_unified_scanner.cpp)
target_link_libraries(test_unified_scanner unified_core)

# 设置输出目录
set_target_properties(
    chtl_shared chtl_compiler chtl_js_compiler unified_core
    PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
)

set_target_properties(
    test_chtl_compiler test_chtl_js_lexer test_unified_scanner
    PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)