# CHTL编译器源代码

# 收集所有源文件
set(CHTL_SOURCES
    main.cpp
)

# Scanner模块
file(GLOB SCANNER_SOURCES scanner/*.cpp)
list(APPEND CHTL_SOURCES ${SCANNER_SOURCES})

# Compiler模块
file(GLOB CHTL_COMPILER_SOURCES compiler/chtl/*.cpp)
file(GLOB CHTLJS_COMPILER_SOURCES compiler/chtljs/*.cpp)
file(GLOB CSS_COMPILER_SOURCES compiler/css/*.cpp)
file(GLOB JS_COMPILER_SOURCES compiler/js/*.cpp)

list(APPEND CHTL_SOURCES 
    ${CHTL_COMPILER_SOURCES}
    ${CHTLJS_COMPILER_SOURCES}
    ${CSS_COMPILER_SOURCES}
    ${JS_COMPILER_SOURCES}
)

# Dispatcher模块
file(GLOB DISPATCHER_SOURCES dispatcher/*.cpp)
list(APPEND CHTL_SOURCES ${DISPATCHER_SOURCES})

# Merger模块
file(GLOB MERGER_SOURCES merger/*.cpp)
list(APPEND CHTL_SOURCES ${MERGER_SOURCES})

# Common模块
file(GLOB COMMON_SOURCES common/*.cpp)
list(APPEND CHTL_SOURCES ${COMMON_SOURCES})

# Modules模块
file(GLOB CMOD_SOURCES modules/cmod/*.cpp)
file(GLOB CJMOD_SOURCES modules/cjmod/*.cpp)
list(APPEND CHTL_SOURCES ${CMOD_SOURCES} ${CJMOD_SOURCES})

# Utils模块
file(GLOB UTILS_SOURCES utils/*.cpp)
list(APPEND CHTL_SOURCES ${UTILS_SOURCES})

# 创建可执行文件
add_executable(chtl ${CHTL_SOURCES})

# 链接库
target_link_libraries(chtl PRIVATE Threads::Threads)

# 如果找到ANTLR4，链接ANTLR4运行时
# find_package(antlr4-runtime QUIET)
# if(antlr4-runtime_FOUND)
#     target_link_libraries(chtl PRIVATE antlr4-runtime)
# endif()

# 安装
install(TARGETS chtl DESTINATION bin)