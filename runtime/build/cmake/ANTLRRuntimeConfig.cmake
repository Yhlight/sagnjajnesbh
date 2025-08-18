# ANTLRRuntimeConfig.cmake
# CHTL ANTLR运行时配置文件
#
# 此文件为ANTLR运行时提供CMake配置支持
# 当ANTLR运行时作为已安装的包被查找时使用
#
# 使用方法:
#   find_package(ANTLRRuntime REQUIRED)
#   target_link_libraries(your_target antlr4-runtime::antlr4-runtime)

@PACKAGE_INIT@

# 版本信息
set(ANTLRRuntime_VERSION "@ANTLR_RUNTIME_VERSION@")
set(ANTLRRuntime_VERSION_MAJOR "@ANTLR_RUNTIME_VERSION_MAJOR@")
set(ANTLRRuntime_VERSION_MINOR "@ANTLR_RUNTIME_VERSION_MINOR@")
set(ANTLRRuntime_VERSION_PATCH "@ANTLR_RUNTIME_VERSION_PATCH@")

# 安装路径
set_and_check(ANTLRRuntime_INCLUDE_DIRS "@PACKAGE_CMAKE_INSTALL_INCLUDEDIR@")
set_and_check(ANTLRRuntime_LIBRARY_DIRS "@PACKAGE_CMAKE_INSTALL_LIBDIR@")

# 包含目标定义
include("${CMAKE_CURRENT_LIST_DIR}/ANTLRRuntimeTargets.cmake")

# 设置变量以保持兼容性
set(ANTLR_RUNTIME_FOUND TRUE)
set(ANTLR_RUNTIME_INCLUDE_DIRS ${ANTLRRuntime_INCLUDE_DIRS})
set(ANTLR_RUNTIME_LIBRARIES antlr4-runtime::antlr4-runtime)
set(ANTLR_RUNTIME_VERSION ${ANTLRRuntime_VERSION})

# 检查所有必需的文件是否存在
check_required_components(ANTLRRuntime)