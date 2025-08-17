cmake_minimum_required(VERSION 3.16)
project(CHTL_Compiler VERSION 1.0.0 LANGUAGES CXX)

# 设置C++标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# 设置编译选项
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# 包含目录
include_directories(src)

# 通用模块源文件
set(COMMON_SOURCES
    src/common/Token.cpp
    src/common/GlobalMap.cpp
    src/common/State.cpp
    src/common/Context.cpp
    src/common/Lexer.cpp
)

# AST模块源文件
set(AST_SOURCES
    src/ast/ASTNode.cpp
)

# 解析器模块源文件
set(PARSER_SOURCES
    src/parser/Parser.cpp
    src/parser/ParserExtensions.cpp
)

# 生成器模块源文件
set(GENERATOR_SOURCES
    src/generator/HTMLGenerator.cpp
)

# 创建库
add_library(chtl_common STATIC ${COMMON_SOURCES})
add_library(chtl_ast STATIC ${AST_SOURCES})
add_library(chtl_parser STATIC ${PARSER_SOURCES})
add_library(chtl_generator STATIC ${GENERATOR_SOURCES})

# 设置库的包含目录
target_include_directories(chtl_common PUBLIC src)
target_include_directories(chtl_ast PUBLIC src)
target_include_directories(chtl_parser PUBLIC src)
target_include_directories(chtl_generator PUBLIC src)

# 设置库依赖关系
target_link_libraries(chtl_parser chtl_common chtl_ast)
target_link_libraries(chtl_generator chtl_common chtl_ast)

# 测试程序
add_executable(test_basic test/test_basic.cpp)
target_link_libraries(test_basic chtl_common)

add_executable(test_ast test/test_ast.cpp)
target_link_libraries(test_ast chtl_common chtl_ast)

add_executable(test_parser test/test_parser.cpp)
target_link_libraries(test_parser chtl_common chtl_ast chtl_parser)

add_executable(test_parser_generator test/test_parser_generator.cpp)
target_link_libraries(test_parser_generator chtl_common chtl_ast chtl_parser chtl_generator)

add_executable(test_simple_parser test/test_simple_parser.cpp)
target_link_libraries(test_simple_parser chtl_common chtl_ast chtl_parser chtl_generator)

add_executable(test_final_parser test/test_final_parser.cpp)
target_link_libraries(test_final_parser chtl_common chtl_ast chtl_parser chtl_generator)

add_executable(test_debug_parser test/test_debug_parser.cpp)
target_link_libraries(test_debug_parser chtl_common chtl_ast chtl_parser chtl_generator)

add_executable(test_namespace_enhanced test/test_namespace_enhanced.cpp)
target_link_libraries(test_namespace_enhanced chtl_common)

# 设置输出目录
set_target_properties(chtl_common chtl_ast chtl_parser chtl_generator PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
)

set_target_properties(test_basic test_ast test_parser test_parser_generator test_simple_parser test_final_parser test_debug_parser test_namespace_enhanced PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

# 安装规则
install(TARGETS chtl_common chtl_ast chtl_parser chtl_generator
    ARCHIVE DESTINATION lib
)

install(TARGETS test_basic test_ast test_parser test_parser_generator test_simple_parser test_final_parser test_debug_parser test_namespace_enhanced
    RUNTIME DESTINATION bin
)

# 安装头文件
install(DIRECTORY src/
    DESTINATION include
    FILES_MATCHING PATTERN "*.h"
)