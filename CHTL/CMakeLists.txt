cmake_minimum_required(VERSION 3.14)
project(CHTL VERSION 1.0.0 LANGUAGES CXX)

# 设置C++标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# 设置编码为UTF-8
if(MSVC)
    add_compile_options(/utf-8)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
    add_compile_options(-finput-charset=UTF-8 -fexec-charset=UTF-8)
endif()

# 设置输出目录
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# 包含目录
include_directories(${CMAKE_SOURCE_DIR}/include)

# 查找线程库
find_package(Threads REQUIRED)

# 添加子目录
add_subdirectory(src/Core)
add_subdirectory(src/Scanner)
add_subdirectory(src/CHTL)
add_subdirectory(src/CHTLJS)
add_subdirectory(src/CSS)
add_subdirectory(src/JavaScript)
add_subdirectory(src/Dispatcher)
add_subdirectory(src/Module)
add_subdirectory(src/Utils)

# 主可执行文件
add_executable(chtl src/main.cpp)

# 链接库
target_link_libraries(chtl
    PRIVATE
        CHTLCore
        CHTLScanner
        CHTLCompiler
        CHTLJSCompiler
        CSSCompiler
        JavaScriptCompiler
        CompilerDispatcher
        ModuleSystem
        CHTLUtils
        Threads::Threads
)

# 安装规则
install(TARGETS chtl
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)

# 安装头文件
install(DIRECTORY include/
    DESTINATION include/CHTL
    FILES_MATCHING PATTERN "*.h" PATTERN "*.hpp"
)

# 创建模块目录
install(DIRECTORY DESTINATION share/CHTL/module)

# 测试
enable_testing()
add_subdirectory(tests)

# 打包配置
set(CPACK_PACKAGE_NAME "CHTL")
set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})
set(CPACK_PACKAGE_DESCRIPTION "CHTL - 基于C++的超文本语言编译器")
set(CPACK_PACKAGE_VENDOR "CHTL Team")
include(CPack)