# CHTL ANTLR运行时使用示例
# 展示如何在项目中集成ANTLR跨平台运行时

cmake_minimum_required(VERSION 3.16)
project(CHTL_ANTLR_Example VERSION 1.0.0 LANGUAGES CXX)

# 设置C++标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 添加编译选项
if(MSVC)
    add_compile_options(/W4 /wd4267 /wd4244 /wd4996)
else()
    add_compile_options(-Wall -Wextra -Wno-unused-parameter -Wno-unused-variable)
endif()

# 设置输出目录
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# 选项
option(BUILD_TESTS "Build test programs" ON)
option(BUILD_EXAMPLES "Build example programs" ON)

# 查找ANTLR运行时
message(STATUS "Looking for ANTLR runtime...")

# 方法1: 使用我们的FindANTLRRuntime模块
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/../build/cmake")
find_package(ANTLRRuntime REQUIRED)

# 或者方法2: 手动设置路径
# set(ANTLR4_ROOT "${CMAKE_SOURCE_DIR}/../prebuilt/linux-x64")
# find_package(ANTLRRuntime REQUIRED)

# 显示找到的信息
message(STATUS "ANTLR runtime found:")
message(STATUS "  Version: ${ANTLR_RUNTIME_VERSION}")
message(STATUS "  Include dirs: ${ANTLR_RUNTIME_INCLUDE_DIRS}")
message(STATUS "  Libraries: ${ANTLR_RUNTIME_LIBRARIES}")

# 基础示例程序
if(BUILD_EXAMPLES)
    # 基础功能示例
    add_executable(basic_example basic_example.cpp)
    target_link_libraries(basic_example antlr4-runtime::antlr4-runtime)
    
    # 词法分析器示例
    add_executable(lexer_example lexer_example.cpp)
    target_link_libraries(lexer_example antlr4-runtime::antlr4-runtime)
    
    # 语法分析器示例
    add_executable(parser_example parser_example.cpp)
    target_link_libraries(parser_example antlr4-runtime::antlr4-runtime)
    
    # 错误处理示例
    add_executable(error_handling_example error_handling_example.cpp)
    target_link_libraries(error_handling_example antlr4-runtime::antlr4-runtime)
    
    message(STATUS "Example programs will be built")
endif()

# 测试程序
if(BUILD_TESTS)
    # 基础测试
    add_executable(basic_test ../test/basic-test.cpp)
    target_link_libraries(basic_test antlr4-runtime::antlr4-runtime)
    
    # 平台兼容性测试
    add_executable(platform_test ../test/platform-test.cpp)
    target_link_libraries(platform_test antlr4-runtime::antlr4-runtime)
    
    # 性能测试
    add_executable(performance_test performance_test.cpp)
    target_link_libraries(performance_test antlr4-runtime::antlr4-runtime)
    
    # 启用测试
    enable_testing()
    add_test(NAME BasicTest COMMAND basic_test)
    add_test(NAME PlatformTest COMMAND platform_test)
    add_test(NAME PerformanceTest COMMAND performance_test)
    
    message(STATUS "Test programs will be built")
endif()

# 如果找到了CHTL的ANTLR集成模块，使用它
if(EXISTS "${CMAKE_SOURCE_DIR}/../build/cmake/CHtlAntlrIntegration.cmake")
    include("${CMAKE_SOURCE_DIR}/../build/cmake/CHtlAntlrIntegration.cmake")
    
    # 验证ANTLR设置
    chtl_verify_antlr_setup()
    
    # 如果有语法文件，可以使用集成功能
    if(EXISTS "${CMAKE_SOURCE_DIR}/../../external/grammars")
        # 设置语法文件目录
        set(ANTLR_GRAMMARS_DIR "${CMAKE_SOURCE_DIR}/../../external/grammars")
        
        # 启用ANTLR集成
        chtl_enable_antlr()
        
        message(STATUS "CHTL ANTLR integration enabled")
    endif()
endif()

# 安装配置
if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    set(CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}/install" CACHE PATH "Installation prefix" FORCE)
endif()

# 安装示例程序
if(BUILD_EXAMPLES)
    install(TARGETS basic_example lexer_example parser_example error_handling_example
            RUNTIME DESTINATION bin
            COMPONENT examples)
endif()

# 安装测试程序
if(BUILD_TESTS)
    install(TARGETS basic_test platform_test performance_test
            RUNTIME DESTINATION bin
            COMPONENT tests)
endif()

# 创建包配置
include(CMakePackageConfigHelpers)

# 显示构建信息
message(STATUS "")
message(STATUS "CHTL ANTLR Runtime Example Configuration:")
message(STATUS "  Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "  Install prefix: ${CMAKE_INSTALL_PREFIX}")
message(STATUS "  Build examples: ${BUILD_EXAMPLES}")
message(STATUS "  Build tests: ${BUILD_TESTS}")
message(STATUS "  C++ standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "")

# 添加自定义目标
add_custom_target(run_tests
    COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure
    DEPENDS basic_test platform_test performance_test
    COMMENT "Running all tests"
)

add_custom_target(run_examples
    COMMAND echo "Running basic example..." && $<TARGET_FILE:basic_example>
    COMMAND echo "Running lexer example..." && $<TARGET_FILE:lexer_example>  
    COMMAND echo "Running parser example..." && $<TARGET_FILE:parser_example>
    DEPENDS basic_example lexer_example parser_example
    COMMENT "Running all examples"
)