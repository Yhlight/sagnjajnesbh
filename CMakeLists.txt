cmake_minimum_required(VERSION 3.16)
project(CHTL_Compiler VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 设置编译选项
if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic")
endif()

# 添加包含目录
include_directories(include)

# 创建主要的可执行文件
add_executable(chtl-compiler
    src/main.cpp
    src/scanner/CHTLUnifiedScanner.cpp
    src/dispatcher/CompilerDispatcher.cpp
    src/compiler/chtl/CHTLCompiler.cpp
    src/compiler/chtljs/CHTLJSCompiler.cpp
    src/compiler/css/CSSCompiler.cpp
    src/compiler/js/JSCompiler.cpp

    src/parser/CHTLParser.cpp
    src/parser/CHTLJSParser.cpp
    src/ast/CHTLASTNodes.cpp
    src/ast/CHTLJSASTNodes.cpp
    src/ast/ASTStateTracker.cpp
    src/generator/CHTLGenerator.cpp
    src/generator/CHTLJSGenerator.cpp

    src/state/StateMachine.cpp
    src/context/ContextManager.cpp
    src/context/ContextAssistant.cpp
    src/global/GlobalMap.cpp

    src/module/CMODManager.cpp
    src/module/CJMODManager.cpp
    src/utils/FileUtils.cpp
    src/utils/StringUtils.cpp
)

# 设置输出目录
set_target_properties(chtl-compiler PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

# 创建模块目录
file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/module")

# 复制官方模块到输出目录 (如果存在)
if(EXISTS "${CMAKE_SOURCE_DIR}/src/Module/")
    file(COPY "${CMAKE_SOURCE_DIR}/src/Module/" DESTINATION "${CMAKE_BINARY_DIR}/module/")
endif()

# 安装规则
install(TARGETS chtl-compiler DESTINATION bin)
if(EXISTS "${CMAKE_SOURCE_DIR}/src/Module/")
    install(DIRECTORY "${CMAKE_SOURCE_DIR}/src/Module/" DESTINATION module)
endif()