cmake_minimum_required(VERSION 3.15)
project(CHTL VERSION 1.0.0 LANGUAGES CXX)

# 设置C++17标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# 编译选项
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# 添加编译选项
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    add_compile_options(-Wall -Wextra -Wpedantic)
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        add_compile_options(-g -O0)
    else()
        add_compile_options(-O3)
    endif()
elseif(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    add_compile_options(/W4)
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        add_compile_options(/Od)
    else()
        add_compile_options(/O2)
    endif()
endif()

# 包含目录
include_directories(${CMAKE_SOURCE_DIR}/include)

# 查找所有源文件
file(GLOB_RECURSE SCANNER_SOURCES src/scanner/*.cpp)
file(GLOB_RECURSE CHTL_SOURCES src/chtl/*.cpp)
file(GLOB_RECURSE CHTL_JS_SOURCES src/chtl_js/*.cpp)
file(GLOB_RECURSE CSS_SOURCES src/css/*.cpp)
file(GLOB_RECURSE JS_SOURCES src/js/*.cpp)
file(GLOB_RECURSE DISPATCHER_SOURCES src/dispatcher/*.cpp)
file(GLOB_RECURSE MERGER_SOURCES src/merger/*.cpp)
file(GLOB_RECURSE UTILS_SOURCES src/utils/*.cpp)
file(GLOB_RECURSE MODULE_SOURCES src/module/*.cpp)

# 创建静态库
add_library(chtl_scanner STATIC ${SCANNER_SOURCES})
add_library(chtl_core STATIC ${CHTL_SOURCES})
add_library(chtl_js_core STATIC ${CHTL_JS_SOURCES})
add_library(chtl_css STATIC ${CSS_SOURCES})
add_library(chtl_javascript STATIC ${JS_SOURCES})
add_library(chtl_dispatcher STATIC ${DISPATCHER_SOURCES})
add_library(chtl_merger STATIC ${MERGER_SOURCES})
add_library(chtl_utils STATIC ${UTILS_SOURCES})
add_library(chtl_module STATIC ${MODULE_SOURCES})

# 主执行文件
add_executable(chtl src/main.cpp)

# 链接库
target_link_libraries(chtl
    chtl_dispatcher
    chtl_scanner
    chtl_core
    chtl_js_core
    chtl_css
    chtl_javascript
    chtl_merger
    chtl_module
    chtl_utils
)

# 测试
enable_testing()
add_subdirectory(tests)

# 安装
install(TARGETS chtl DESTINATION bin)
install(DIRECTORY include/ DESTINATION include/chtl)
install(DIRECTORY module/ DESTINATION share/chtl/module)