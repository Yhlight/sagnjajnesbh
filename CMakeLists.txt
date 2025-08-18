cmake_minimum_required(VERSION 3.16)
project(CHTLCompiler VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wno-unused-parameter -Wno-unused-variable")

include_directories(src)

# 🎯 基于已有功能的完整编译配置

# 统一扫描器
add_library(chtl_scanner STATIC
    src/Scanner/CHTLUnifiedScanner.cpp
)

# 共同组件（包含已有的功能）
add_library(chtl_common STATIC
    src/Common/Core/NamespaceAlgorithms.cpp
    src/Common/Core/ErrorHandler.cpp
    src/Common/Utils/FileUtils.cpp
    src/Common/ModulePathManager.cpp
    src/Common/SimpleZip/SimpleZip.cpp
    src/Common/RAIIStateGuard.cpp
)

# CHTL编译器（包含已有的完整功能）
add_library(chtl_compiler STATIC
    src/CHTLCompiler/CHTLCompilerCore.cpp
    src/CHTLCompiler/Import/ImportManager.cpp
    src/CHTLCompiler/Namespace/NamespaceManager.cpp
    src/CHTLCompiler/Module/CmodManager.cpp
    src/CHTLCompiler/State/CHTLState.cpp
    src/CHTLCompiler/State/CHTLGlobalMap.cpp
)
target_link_libraries(chtl_compiler chtl_common)

# CHTL JS编译器（包含已有的完整功能）
add_library(chtljs_compiler STATIC
    src/CHTLJSCompiler/CHTLJSCompiler.cpp
    src/CHTLJSCompiler/Core/AutoAdditionManager.cpp
    src/CHTLJSCompiler/Core/JSEventRegistry.cpp
    src/CHTLJSCompiler/Enhanced/UnorderedOptionalProcessor.cpp
    src/CHTLJSCompiler/Parser/SelectorDetector.cpp
)
target_link_libraries(chtljs_compiler chtl_common)

# 测试现有功能
add_executable(test_existing_features test/test_existing_features.cpp)

# 完整CHTL合规性验证
add_executable(test_complete_chtl_compliance test/test_complete_chtl_compliance.cpp)

# 可变长度切片机制测试
add_executable(test_variable_length_slicing test/test_variable_length_slicing.cpp)
target_link_libraries(test_existing_features 
    chtl_scanner 
    chtl_common 
    chtl_compiler 
    chtljs_compiler
)

target_link_libraries(test_complete_chtl_compliance
    chtl_scanner 
    chtl_common 
    chtl_compiler 
    chtljs_compiler
)

target_link_libraries(test_variable_length_slicing
    chtl_scanner
)

# 设置输出目录
set_target_properties(
    chtl_scanner chtl_common chtl_compiler chtljs_compiler
    PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
)

set_target_properties(
    test_existing_features test_complete_chtl_compliance test_variable_length_slicing
    PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)