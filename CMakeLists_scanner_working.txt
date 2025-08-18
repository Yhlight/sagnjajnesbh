cmake_minimum_required(VERSION 3.16)
project(CHTLScannerWorking VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wno-unused-parameter -Wno-unused-variable")

include_directories(src)

# 🔧 恢复工作状态：仅测试Scanner
add_library(chtl_scanner_working STATIC
    src/Scanner/CHTLUnifiedScanner.cpp
)

# Scanner工作状态测试
add_executable(test_scanner_working test/test_scanner_working.cpp)
target_link_libraries(test_scanner_working chtl_scanner_working)

# 设置输出目录
set_target_properties(chtl_scanner_working PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set_target_properties(test_scanner_working PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)