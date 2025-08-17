cmake_minimum_required(VERSION 3.16)
project(CHTLCompiler VERSION 1.0.0 LANGUAGES CXX)

# 设置C++17标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 设置编译选项
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -O0")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -DNDEBUG")

# 包含目录
include_directories(src)

# 共享组件源文件
set(SHARED_SOURCES
    src/Shared/SimpleZip/SimpleZip.cpp
    src/Shared/Utils/FileUtils.cpp
)

# CHTL编译器源文件
set(CHTL_COMPILER_SOURCES
    src/CHTLCompiler/Lexer/CHTLLexer.cpp
    src/CHTLCompiler/Lexer/CHTLToken.cpp
    src/CHTLCompiler/Parser/CHTLContext.cpp
    src/CHTLCompiler/Parser/CHTLParser.cpp
    src/CHTLCompiler/AST/CHTLNodes.cpp
    src/CHTLCompiler/State/CHTLState.cpp
    src/CHTLCompiler/State/CHTLGlobalMap.cpp
    src/CHTLCompiler/Generator/CHTLGenerator.cpp
    src/CHTLCompiler/Module/CmodManager.cpp
    src/CHTLCompiler/Core/CHTLCompilerCore.cpp
)

# CHTL JS编译器源文件
set(CHTL_JS_COMPILER_SOURCES
    src/CHTLJSCompiler/Lexer/CHTLJSLexer.cpp
    src/CHTLJSCompiler/Lexer/CHTLJSToken.cpp
    src/CHTLJSCompiler/Parser/CHTLJSContext.cpp
    src/CHTLJSCompiler/Parser/CHTLJSParser.cpp
    src/CHTLJSCompiler/State/CHTLJSState.cpp
    src/CHTLJSCompiler/State/CHTLJSGlobalMap.cpp
    src/CHTLJSCompiler/Generator/CHTLJSGenerator.cpp
    src/CHTLJSCompiler/Module/CJmodManager.cpp
    src/CHTLJSCompiler/AST/VirtualObjectManager.cpp
    src/CHTLJSCompiler/Core/CHTLJSCompilerCore.cpp
)

# CSS编译器源文件
set(CSS_COMPILER_SOURCES
    src/CSSCompiler/Core/CSSCompilerCore.cpp
)

# JavaScript编译器源文件
set(JS_COMPILER_SOURCES
    src/JavaScriptCompiler/Core/JavaScriptCompilerCore.cpp
)

# 统一核心源文件
set(UNIFIED_CORE_SOURCES
    src/UnifiedCore/Scanner/CHTLUnifiedScanner.cpp
    src/UnifiedCore/Dispatcher/CompilerDispatcher.cpp
    src/UnifiedCore/Constraints/SyntaxConstrainer.cpp
    src/UnifiedCore/Import/ImportManager.cpp
)

# AST源文件（从旧结构迁移）
set(AST_SOURCES
    src/ast/ASTNode.cpp
    src/ast/ASTStateManager.cpp
)

# 其他源文件（暂时保留）
set(OTHER_SOURCES
    src/common/CompilerCore.cpp
    src/common/ContextManager.cpp
    src/parser/ParserExtensions.cpp
)

# 创建共享库
add_library(chtl_shared STATIC ${SHARED_SOURCES})

# 创建CHTL编译器库
add_library(chtl_compiler STATIC ${CHTL_COMPILER_SOURCES})
target_link_libraries(chtl_compiler chtl_shared)

# 创建CHTL JS编译器库
add_library(chtl_js_compiler STATIC ${CHTL_JS_COMPILER_SOURCES})
target_link_libraries(chtl_js_compiler chtl_shared)

# 创建CSS编译器库
add_library(css_compiler STATIC ${CSS_COMPILER_SOURCES})
target_link_libraries(css_compiler chtl_shared)

# 创建JavaScript编译器库
add_library(js_compiler STATIC ${JS_COMPILER_SOURCES})
target_link_libraries(js_compiler chtl_shared)

# 创建统一核心库
add_library(unified_core STATIC ${UNIFIED_CORE_SOURCES})
target_link_libraries(unified_core chtl_shared)

# 创建AST库（暂时保留）
add_library(chtl_ast STATIC ${AST_SOURCES})

# 创建其他组件库（暂时保留）
add_library(chtl_other STATIC ${OTHER_SOURCES})

# 主编译器可执行文件
add_executable(chtl_main src/main.cpp)
target_link_libraries(chtl_main 
    chtl_compiler 
    chtl_js_compiler 
    css_compiler 
    js_compiler 
    unified_core 
    chtl_ast 
    chtl_other
)

# 测试可执行文件
add_executable(test_basic test/test_basic.cpp)
target_link_libraries(test_basic chtl_compiler chtl_shared)

add_executable(test_chtl_compiler test/test_chtl_compiler.cpp)
target_link_libraries(test_chtl_compiler chtl_compiler)

add_executable(test_chtl_js_compiler test/test_chtl_js_compiler.cpp)
target_link_libraries(test_chtl_js_compiler chtl_js_compiler)

add_executable(test_unified_scanner test/test_unified_scanner.cpp)
target_link_libraries(test_unified_scanner unified_core)

add_executable(test_syntax_constraints test/test_syntax_constraints.cpp)
target_link_libraries(test_syntax_constraints unified_core chtl_ast)

add_executable(test_virtual_object test/test_virtual_object.cpp)
target_link_libraries(test_virtual_object chtl_js_compiler chtl_ast)

add_executable(test_cmod test/test_cmod.cpp)
target_link_libraries(test_cmod chtl_compiler)

add_executable(test_cjmod test/test_cjmod.cpp)
target_link_libraries(test_cjmod chtl_js_compiler)

add_executable(test_namespace_enhanced test/test_namespace_enhanced.cpp)
target_link_libraries(test_namespace_enhanced chtl_compiler)

# 设置输出目录
set_target_properties(
    chtl_shared chtl_compiler chtl_js_compiler css_compiler js_compiler 
    unified_core chtl_ast chtl_other
    PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
)

set_target_properties(
    chtl_main
    test_basic test_chtl_compiler test_chtl_js_compiler test_unified_scanner 
    test_syntax_constraints test_virtual_object test_cmod test_cjmod test_namespace_enhanced
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

# 安装规则
install(TARGETS 
    chtl_shared chtl_compiler chtl_js_compiler css_compiler js_compiler unified_core
    ARCHIVE DESTINATION lib
)

install(TARGETS chtl_main RUNTIME DESTINATION bin)

# 安装头文件
install(DIRECTORY src/CHTLCompiler/ DESTINATION include/CHTLCompiler FILES_MATCHING PATTERN "*.h")
install(DIRECTORY src/CHTLJSCompiler/ DESTINATION include/CHTLJSCompiler FILES_MATCHING PATTERN "*.h")
install(DIRECTORY src/CSSCompiler/ DESTINATION include/CSSCompiler FILES_MATCHING PATTERN "*.h")
install(DIRECTORY src/JavaScriptCompiler/ DESTINATION include/JavaScriptCompiler FILES_MATCHING PATTERN "*.h")
install(DIRECTORY src/UnifiedCore/ DESTINATION include/UnifiedCore FILES_MATCHING PATTERN "*.h")
install(DIRECTORY src/Shared/ DESTINATION include/Shared FILES_MATCHING PATTERN "*.h")

# 打包配置
set(CPACK_PACKAGE_NAME "CHTLCompiler")
set(CPACK_PACKAGE_VERSION "1.0.0")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "CHTL Programming Language Compiler")
set(CPACK_PACKAGE_VENDOR "CHTL Team")

include(CPack)