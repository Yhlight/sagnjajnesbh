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

# 创建通用库
add_library(chtl_common STATIC ${COMMON_SOURCES})

# 创建AST库
add_library(chtl_ast STATIC ${AST_SOURCES})

# 设置库的包含目录
target_include_directories(chtl_common PUBLIC src)
target_include_directories(chtl_ast PUBLIC src)

# 测试程序
add_executable(test_basic test/test_basic.cpp)
target_link_libraries(test_basic chtl_common)

add_executable(test_ast test/test_ast.cpp)
target_link_libraries(test_ast chtl_common chtl_ast)

# 设置输出目录
set_target_properties(chtl_common chtl_ast PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
)

set_target_properties(test_basic test_ast PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

# 安装规则
install(TARGETS chtl_common chtl_ast
    ARCHIVE DESTINATION lib
)

install(TARGETS test_basic test_ast
    RUNTIME DESTINATION bin
)

# 安装头文件
install(DIRECTORY src/
    DESTINATION include
    FILES_MATCHING PATTERN "*.h"
)