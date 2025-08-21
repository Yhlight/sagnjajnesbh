cmake_minimum_required(VERSION 3.16)
project(CHTL_Compiler VERSION 1.0.0 LANGUAGES CXX)

# 设置C++17标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# UTF-8支持
if(MSVC)
    add_compile_options(/utf-8)
    add_compile_definitions(_CRT_SECURE_NO_WARNINGS)
endif()

# 设置输出目录
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# 包含目录
include_directories(include)
include_directories(third_party/ANTLR4/include)

# 链接目录
link_directories(third_party/ANTLR4/lib)

# CHTL核心库
add_subdirectory(src/Core)

# CHTL编译器库
add_subdirectory(src/CHTL)

# CHTL JS编译器库
add_subdirectory(src/CHTLJS)

# CSS编译器库（基于ANTLR4）
add_subdirectory(src/CSS)

# JavaScript编译器库（基于ANTLR4）
add_subdirectory(src/JavaScript)

# 模块系统
add_subdirectory(src/Module)

# 工具库
add_subdirectory(src/Utils)

# 主编译器可执行文件
add_executable(chtl_compiler
    src/main.cpp
)

target_link_libraries(chtl_compiler
    CHTL_Core
    CHTL_Compiler
    CHTLJS_Compiler
    CSS_Compiler
    JavaScript_Compiler
    Module_System
    Utils
)

# 如果是Windows平台，链接ANTLR4库
if(WIN32)
    target_link_libraries(chtl_compiler antlr4-runtime)
else()
    target_link_libraries(chtl_compiler antlr4-runtime)
endif()

# 测试
enable_testing()
add_subdirectory(tests)