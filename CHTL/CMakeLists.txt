cmake_minimum_required(VERSION 3.16)
project(CHTL VERSION 1.0.0 LANGUAGES CXX)

# 设置C++17标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# 设置UTF-8编码支持
if(MSVC)
    # Visual Studio UTF-8支持
    add_compile_options(/utf-8)
    add_compile_options(/source-charset:utf-8)
    add_compile_options(/execution-charset:utf-8)
else()
    # GCC/Clang UTF-8支持
    add_compile_options(-finput-charset=UTF-8)
    add_compile_options(-fexec-charset=UTF-8)
endif()

# 设置构建类型
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# 编译选项
if(MSVC)
    add_compile_options(/W4 /WX-)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        add_compile_options(-g -O0)
    else()
        add_compile_options(-O3)
    endif()
endif()

# 包含目录
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

# ANTLR4 运行时库路径
set(ANTLR4_RUNTIME_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../ANTLR4)
if(EXISTS ${ANTLR4_RUNTIME_DIR})
    include_directories(${ANTLR4_RUNTIME_DIR}/runtime/src)
    link_directories(${ANTLR4_RUNTIME_DIR}/dist)
endif()

# 输出目录
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# 收集源文件
file(GLOB_RECURSE CHTL_SOURCES 
    ${CMAKE_CURRENT_SOURCE_DIR}/src/CHTL/*.cpp
)

file(GLOB_RECURSE CHTLJS_SOURCES 
    ${CMAKE_CURRENT_SOURCE_DIR}/src/CHTLJS/*.cpp
)

file(GLOB_RECURSE CSS_SOURCES 
    ${CMAKE_CURRENT_SOURCE_DIR}/src/CSS/*.cpp
)

file(GLOB_RECURSE JS_SOURCES 
    ${CMAKE_CURRENT_SOURCE_DIR}/src/JS/*.cpp
)

file(GLOB_RECURSE SCANNER_SOURCES 
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Scanner/*.cpp
)

file(GLOB_RECURSE DISPATCHER_SOURCES 
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Dispatcher/*.cpp
)

file(GLOB_RECURSE COMMON_SOURCES 
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Common/*.cpp
)

file(GLOB_RECURSE MERGER_SOURCES 
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Merger/*.cpp
)

# 创建静态库
add_library(CHTLCore STATIC
    ${CHTL_SOURCES}
    ${CHTLJS_SOURCES}
    ${SCANNER_SOURCES}
    ${DISPATCHER_SOURCES}
    ${COMMON_SOURCES}
    ${MERGER_SOURCES}
)

add_library(CSSCompiler STATIC ${CSS_SOURCES})
add_library(JSCompiler STATIC ${JS_SOURCES})

# 主程序
add_executable(chtl src/main.cpp)

# 链接库
target_link_libraries(chtl 
    CHTLCore
    CSSCompiler
    JSCompiler
)

# 如果找到ANTLR4运行时，链接它
# TODO: 暂时注释掉，等ANTLR4集成后再启用
# if(EXISTS ${ANTLR4_RUNTIME_DIR})
#     target_link_libraries(CSSCompiler antlr4-runtime)
#     target_link_libraries(JSCompiler antlr4-runtime)
# endif()

# 安装规则
install(TARGETS chtl DESTINATION bin)
install(DIRECTORY modules/official/ DESTINATION modules)

# 测试
enable_testing()
add_subdirectory(tests)