cmake_minimum_required(VERSION 3.14)
project(CHTLCompiler VERSION 1.0.0 LANGUAGES CXX)

# 设置C++17标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# 设置编译选项
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    add_compile_options(-Wall -Wextra -Wpedantic)
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        add_compile_options(-g -O0)
    else()
        add_compile_options(-O2)
    endif()
elseif(MSVC)
    add_compile_options(/W4)
endif()

# 添加include目录
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

# 查找ANTLR4运行时库（用于CSS和JavaScript编译器）
find_package(antlr4-runtime CONFIG)
if(NOT antlr4-runtime_FOUND)
    message(WARNING "ANTLR4 runtime not found. CSS and JavaScript compilers will use stub implementations.")
    set(USE_ANTLR OFF)
else()
    set(USE_ANTLR ON)
    add_definitions(-DUSE_ANTLR)
endif()

# 源文件
set(SCANNER_SOURCES
    src/scanner/chtl_unified_scanner.cpp
    src/scanner/token.cpp
    src/scanner/scanner_context.cpp
)

set(DISPATCHER_SOURCES
    src/dispatcher/compiler_dispatcher.cpp
    src/dispatcher/code_fragment.cpp
)

set(CHTL_COMPILER_SOURCES
    src/compilers/chtl/chtl_compiler.cpp
    src/compilers/chtl/chtl_parser.cpp
    src/compilers/chtl/chtl_ast.cpp
    src/compilers/chtl/chtl_code_generator.cpp
)

set(CHTL_JS_COMPILER_SOURCES
    src/compilers/chtl_js/chtl_js_compiler.cpp
    src/compilers/chtl_js/chtl_js_parser.cpp
    src/compilers/chtl_js/chtl_js_ast.cpp
    src/compilers/chtl_js/chtl_js_code_generator.cpp
)

set(CSS_COMPILER_SOURCES
    src/compilers/css/css_compiler.cpp
)

set(JAVASCRIPT_COMPILER_SOURCES
    src/compilers/javascript/javascript_compiler.cpp
)

set(MERGER_SOURCES
    src/merger/result_merger.cpp
    src/merger/html_generator.cpp
)

# 创建静态库
add_library(chtl_scanner STATIC ${SCANNER_SOURCES})
add_library(chtl_dispatcher STATIC ${DISPATCHER_SOURCES})
add_library(chtl_compiler_lib STATIC ${CHTL_COMPILER_SOURCES})
add_library(chtl_js_compiler_lib STATIC ${CHTL_JS_COMPILER_SOURCES})
add_library(css_compiler_lib STATIC ${CSS_COMPILER_SOURCES})
add_library(javascript_compiler_lib STATIC ${JAVASCRIPT_COMPILER_SOURCES})
add_library(chtl_merger STATIC ${MERGER_SOURCES})

# 主程序
add_executable(chtlc src/main.cpp)

# 链接库
target_link_libraries(chtlc
    chtl_scanner
    chtl_dispatcher
    chtl_compiler_lib
    chtl_js_compiler_lib
    css_compiler_lib
    javascript_compiler_lib
    chtl_merger
)

# 如果找到ANTLR，链接ANTLR运行时
if(USE_ANTLR)
    target_link_libraries(css_compiler_lib antlr4_static)
    target_link_libraries(javascript_compiler_lib antlr4_static)
endif()

# 测试
enable_testing()
add_subdirectory(tests)

# 安装
install(TARGETS chtlc DESTINATION bin)
install(DIRECTORY include/ DESTINATION include/chtl)