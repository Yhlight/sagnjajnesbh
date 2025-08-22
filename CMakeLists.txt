cmake_minimum_required(VERSION 3.16)

project(CHTL LANGUAGES CXX)

if(NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(MSVC)
	add_compile_options(/utf-8)
else()
	add_compile_options(-Wall -Wextra -Wpedantic)
endif()

add_definitions(-DANTLR4CPP_STATIC)

# 路径
set(ANTLR4_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/ANTLR4/include)
set(JS_DIR ${CMAKE_SOURCE_DIR}/js)
set(CSS_DIR ${CMAKE_SOURCE_DIR}/css)

# antlr4 runtime 静态库（使用仓库内已提供源码）
file(GLOB_RECURSE ANTLR4_RUNTIME_SOURCES
	${ANTLR4_INCLUDE_DIR}/*.cpp
)
add_library(antlr4_runtime STATIC ${ANTLR4_RUNTIME_SOURCES})
target_include_directories(antlr4_runtime PUBLIC ${ANTLR4_INCLUDE_DIR})

# CSS 解析库
file(GLOB CSS_SOURCES
	${CSS_DIR}/*.cpp
)
add_library(cssparser STATIC ${CSS_SOURCES})
target_include_directories(cssparser PUBLIC ${ANTLR4_INCLUDE_DIR} ${CSS_DIR})
target_link_libraries(cssparser PUBLIC antlr4_runtime)

# JS 解析库
file(GLOB JS_SOURCES
	${JS_DIR}/*.cpp
)
add_library(jsparser STATIC ${JS_SOURCES})
target_include_directories(jsparser PUBLIC ${ANTLR4_INCLUDE_DIR} ${JS_DIR})
target_link_libraries(jsparser PUBLIC antlr4_runtime)

# 主可执行程序
set(SRC_DIR ${CMAKE_SOURCE_DIR}/src)
add_executable(chtlc
	${SRC_DIR}/main.cpp
	${SRC_DIR}/Core/CHTLUnifiedScanner.cpp
	${SRC_DIR}/Core/CompilerDispatcher.cpp
	${SRC_DIR}/Core/Merge/ResultMerger.cpp
	${SRC_DIR}/Utils/FileIO.cpp
	${SRC_DIR}/CSS/CssCompiler.cpp
	${SRC_DIR}/JS/JsCompiler.cpp
	${SRC_DIR}/CHTL/Lexer.cpp
	${SRC_DIR}/CHTL/AST.cpp
	${SRC_DIR}/CHTL/Parser.cpp
	${SRC_DIR}/CHTL/Generator.cpp
	${SRC_DIR}/CHTL/CHTLCompiler.cpp
	${SRC_DIR}/CHTLJS/Lexer.cpp
	${SRC_DIR}/CHTLJS/CHTLJSCompiler.cpp
)

target_include_directories(chtlc PRIVATE
	${SRC_DIR}
	${ANTLR4_INCLUDE_DIR}
	${JS_DIR}
	${CSS_DIR}
)

target_link_libraries(chtlc PRIVATE cssparser jsparser antlr4_runtime)

if(UNIX)
	target_link_libraries(chtlc PRIVATE pthread)
endif()