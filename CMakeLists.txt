cmake_minimum_required(VERSION 3.16)
project(CHTL_Compiler VERSION 1.0.0 LANGUAGES CXX)

# 设置C++标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 添加UTF-8支持
if(MSVC)
    add_compile_options(/utf-8)
else()
    add_compile_options(-finput-charset=UTF-8 -fexec-charset=UTF-8)
endif()

# 设置源文件编码为UTF-8
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DUNICODE -D_UNICODE")
set(CMAKE_CXX_EXTENSIONS OFF)

# 设置UTF-8编码支持
if(MSVC)
    add_compile_options(/utf-8)
    add_compile_definitions(_CRT_SECURE_NO_WARNINGS)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /bigobj")
else()
    add_compile_options(-finput-charset=UTF-8 -fexec-charset=UTF-8)
endif()

# 设置构建类型
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# 设置输出目录
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# 包含目录
include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(${CMAKE_SOURCE_DIR}/ANTLR4/include)

# 链接ANTLR4库
# 优先使用本地ANTLR4库
link_directories(${CMAKE_SOURCE_DIR}/ANTLR4/lib)
if(WIN32)
    set(ANTLR4_LIBRARIES antlr4-runtime-static)
else()
    # 在Linux上尝试使用静态库，如果不存在则尝试动态库
    if(EXISTS ${CMAKE_SOURCE_DIR}/ANTLR4/lib/libantlr4-runtime-static.a)
        set(ANTLR4_LIBRARIES antlr4-runtime-static)
    elseif(EXISTS ${CMAKE_SOURCE_DIR}/ANTLR4/lib/libantlr4-runtime.so)
        set(ANTLR4_LIBRARIES antlr4-runtime)
    else()
        # 如果本地库不存在，尝试系统库
        find_package(PkgConfig QUIET)
        if(PkgConfig_FOUND)
            pkg_check_modules(ANTLR4 QUIET antlr4-runtime)
            if(ANTLR4_FOUND)
                set(ANTLR4_LIBRARIES ${ANTLR4_LIBRARIES})
            else()
                message(WARNING "ANTLR4库未找到，某些功能可能无法正常工作")
                set(ANTLR4_LIBRARIES "")
            endif()
        else()
            message(WARNING "ANTLR4库未找到，某些功能可能无法正常工作")
            set(ANTLR4_LIBRARIES "")
        endif()
    endif()
endif()

# 添加子目录
add_subdirectory(src)

# 添加测试
enable_testing()
# add_subdirectory(tests)  # 临时禁用测试

# 安装配置
install(TARGETS chtl_compiler
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)

install(DIRECTORY include/ DESTINATION include)