# FindANTLRRuntime.cmake
# CHTL ANTLR运行时查找模块
#
# 此模块定义了以下变量:
#   ANTLR_RUNTIME_FOUND          - 如果找到ANTLR运行时则为true
#   ANTLR_RUNTIME_INCLUDE_DIRS   - ANTLR运行时头文件目录
#   ANTLR_RUNTIME_LIBRARIES      - ANTLR运行时库文件
#   ANTLR_RUNTIME_VERSION        - ANTLR运行时版本
#   ANTLR_RUNTIME_DEFINITIONS    - 编译定义
#
# 此模块定义了以下导入目标:
#   antlr4-runtime::antlr4-runtime-static   - 静态库目标
#   antlr4-runtime::antlr4-runtime-shared   - 动态库目标
#   antlr4-runtime::antlr4-runtime          - 首选库目标
#
# 用法示例:
#   find_package(ANTLRRuntime REQUIRED)
#   target_link_libraries(your_target antlr4-runtime::antlr4-runtime)
#
# 可以设置以下变量来控制查找行为:
#   ANTLR4_ROOT                  - ANTLR运行时根目录
#   ANTLR_RUNTIME_USE_STATIC     - 优先使用静态库
#   ANTLR_RUNTIME_USE_SHARED     - 优先使用动态库
#

cmake_minimum_required(VERSION 3.16)

# 检测平台
function(_antlr_detect_platform _platform_var)
    if(WIN32)
        if(CMAKE_SIZEOF_VOID_P EQUAL 8)
            set(${_platform_var} "windows-x64" PARENT_SCOPE)
        else()
            set(${_platform_var} "windows-x86" PARENT_SCOPE)
        endif()
    elseif(APPLE)
        if(CMAKE_OSX_ARCHITECTURES)
            if("arm64" IN_LIST CMAKE_OSX_ARCHITECTURES AND "x86_64" IN_LIST CMAKE_OSX_ARCHITECTURES)
                set(${_platform_var} "macos-universal" PARENT_SCOPE)
            elseif("arm64" IN_LIST CMAKE_OSX_ARCHITECTURES)
                set(${_platform_var} "macos-arm64" PARENT_SCOPE)
            else()
                set(${_platform_var} "macos-x64" PARENT_SCOPE)
            endif()
        else()
            if(CMAKE_SYSTEM_PROCESSOR MATCHES "arm64|aarch64")
                set(${_platform_var} "macos-arm64" PARENT_SCOPE)
            else()
                set(${_platform_var} "macos-x64" PARENT_SCOPE)
            endif()
        endif()
    elseif(UNIX)
        if(CMAKE_SYSTEM_PROCESSOR MATCHES "aarch64|arm64")
            set(${_platform_var} "linux-arm64" PARENT_SCOPE)
        else()
            set(${_platform_var} "linux-x64" PARENT_SCOPE)
        endif()
    else()
        set(${_platform_var} "unknown" PARENT_SCOPE)
    endif()
endfunction()

# 查找ANTLR运行时根目录
function(_antlr_find_root _root_var)
    set(_search_paths)
    
    # 1. 用户指定的路径
    if(ANTLR4_ROOT)
        list(APPEND _search_paths "${ANTLR4_ROOT}")
    endif()
    
    # 2. 环境变量
    if(DEFINED ENV{ANTLR4_ROOT})
        list(APPEND _search_paths "$ENV{ANTLR4_ROOT}")
    endif()
    
    # 3. 检测平台特定的预构建路径
    _antlr_detect_platform(_platform)
    
    # 查找runtime目录
    get_filename_component(_current_dir "${CMAKE_CURRENT_LIST_DIR}" DIRECTORY)
    get_filename_component(_runtime_dir "${_current_dir}" DIRECTORY)
    
    list(APPEND _search_paths 
        "${_runtime_dir}/prebuilt/${_platform}"
        "${CMAKE_SOURCE_DIR}/runtime/prebuilt/${_platform}"
        "${CMAKE_SOURCE_DIR}/external/antlr4-install"
        "${CMAKE_SOURCE_DIR}/prebuilt/antlr4"
    )
    
    # 4. 系统路径
    if(UNIX AND NOT APPLE)
        list(APPEND _search_paths
            "/usr/local"
            "/usr"
            "/opt/antlr4"
        )
    elseif(APPLE)
        list(APPEND _search_paths
            "/usr/local"
            "/opt/homebrew"
            "/opt/antlr4"
        )
    elseif(WIN32)
        list(APPEND _search_paths
            "C:/Program Files/antlr4"
            "C:/antlr4"
            "${CMAKE_PROGRAM_FILES}/antlr4"
        )
    endif()
    
    # 查找根目录
    foreach(_path ${_search_paths})
        if(EXISTS "${_path}/include/antlr4-runtime" AND 
           (EXISTS "${_path}/lib" OR EXISTS "${_path}/lib64"))
            set(${_root_var} "${_path}" PARENT_SCOPE)
            return()
        endif()
    endforeach()
    
    set(${_root_var} "" PARENT_SCOPE)
endfunction()

# 查找头文件
function(_antlr_find_includes _include_var _root)
    find_path(_antlr_include_dir
        NAMES "antlr4-runtime.h"
        PATHS "${_root}/include/antlr4-runtime"
        NO_DEFAULT_PATH
    )
    
    if(_antlr_include_dir)
        set(${_include_var} "${_antlr_include_dir}" PARENT_SCOPE)
    else()
        set(${_include_var} "" PARENT_SCOPE)
    endif()
endfunction()

# 查找库文件
function(_antlr_find_libraries _static_var _shared_var _root)
    set(_lib_paths "${_root}/lib" "${_root}/lib64")
    
    # 查找静态库
    find_library(_antlr_static_lib
        NAMES antlr4-runtime libantlr4-runtime
        PATHS ${_lib_paths}
        NO_DEFAULT_PATH
    )
    
    # 查找动态库
    if(WIN32)
        find_library(_antlr_shared_lib
            NAMES antlr4-runtime libantlr4-runtime
            PATHS ${_lib_paths}
            NO_DEFAULT_PATH
        )
        # 查找DLL
        find_file(_antlr_dll
            NAMES antlr4-runtime.dll libantlr4-runtime.dll
            PATHS "${_root}/bin" "${_root}/lib" ${_lib_paths}
            NO_DEFAULT_PATH
        )
    else()
        find_library(_antlr_shared_lib
            NAMES antlr4-runtime libantlr4-runtime
            PATHS ${_lib_paths}
            NO_DEFAULT_PATH
        )
    endif()
    
    set(${_static_var} "${_antlr_static_lib}" PARENT_SCOPE)
    set(${_shared_var} "${_antlr_shared_lib}" PARENT_SCOPE)
    if(WIN32 AND _antlr_dll)
        set(${_shared_var} "${_antlr_shared_lib};${_antlr_dll}" PARENT_SCOPE)
    endif()
endfunction()

# 获取版本信息
function(_antlr_get_version _version_var _root)
    set(_version_file "${_root}/include/antlr4-runtime/Version.h")
    if(EXISTS "${_version_file}")
        file(READ "${_version_file}" _version_content)
        
        string(REGEX MATCH "#define ANTLR_VERSION_MAJOR ([0-9]+)" _ "${_version_content}")
        set(_major "${CMAKE_MATCH_1}")
        
        string(REGEX MATCH "#define ANTLR_VERSION_MINOR ([0-9]+)" _ "${_version_content}")
        set(_minor "${CMAKE_MATCH_1}")
        
        string(REGEX MATCH "#define ANTLR_VERSION_PATCH ([0-9]+)" _ "${_version_content}")
        set(_patch "${CMAKE_MATCH_1}")
        
        if(_major AND _minor AND _patch)
            set(${_version_var} "${_major}.${_minor}.${_patch}" PARENT_SCOPE)
            return()
        endif()
    endif()
    
    # 备选方案：检查platform-info.txt
    set(_info_file "${_root}/platform-info.txt")
    if(EXISTS "${_info_file}")
        file(READ "${_info_file}" _info_content)
        string(REGEX MATCH "ANTLR Version: ([0-9]+\\.[0-9]+\\.[0-9]+)" _ "${_info_content}")
        if(CMAKE_MATCH_1)
            set(${_version_var} "${CMAKE_MATCH_1}" PARENT_SCOPE)
            return()
        endif()
    endif()
    
    # 默认版本
    set(${_version_var} "4.13.2" PARENT_SCOPE)
endfunction()

# 创建导入目标
function(_antlr_create_targets _root _include_dir _static_lib _shared_lib)
    # 静态库目标
    if(_static_lib)
        if(NOT TARGET antlr4-runtime::antlr4-runtime-static)
            add_library(antlr4-runtime::antlr4-runtime-static STATIC IMPORTED)
            set_target_properties(antlr4-runtime::antlr4-runtime-static PROPERTIES
                IMPORTED_LOCATION "${_static_lib}"
                INTERFACE_INCLUDE_DIRECTORIES "${_include_dir}"
            )
            
            # 添加必要的系统库
            if(UNIX AND NOT APPLE)
                set_property(TARGET antlr4-runtime::antlr4-runtime-static APPEND PROPERTY
                    INTERFACE_LINK_LIBRARIES pthread
                )
            elseif(WIN32)
                set_property(TARGET antlr4-runtime::antlr4-runtime-static APPEND PROPERTY
                    INTERFACE_LINK_LIBRARIES ws2_32
                )
            endif()
        endif()
    endif()
    
    # 动态库目标
    if(_shared_lib)
        if(NOT TARGET antlr4-runtime::antlr4-runtime-shared)
            add_library(antlr4-runtime::antlr4-runtime-shared SHARED IMPORTED)
            
            if(WIN32 AND ";" IN_LIST _shared_lib)
                # Windows: 分离.lib和.dll
                list(GET _shared_lib 0 _import_lib)
                list(GET _shared_lib 1 _dll_file)
                
                set_target_properties(antlr4-runtime::antlr4-runtime-shared PROPERTIES
                    IMPORTED_LOCATION "${_dll_file}"
                    IMPORTED_IMPLIB "${_import_lib}"
                    INTERFACE_INCLUDE_DIRECTORIES "${_include_dir}"
                )
            else()
                set_target_properties(antlr4-runtime::antlr4-runtime-shared PROPERTIES
                    IMPORTED_LOCATION "${_shared_lib}"
                    INTERFACE_INCLUDE_DIRECTORIES "${_include_dir}"
                )
            endif()
        endif()
    endif()
    
    # 首选目标（别名）
    if(NOT TARGET antlr4-runtime::antlr4-runtime)
        if(ANTLR_RUNTIME_USE_STATIC AND _static_lib)
            add_library(antlr4-runtime::antlr4-runtime ALIAS antlr4-runtime::antlr4-runtime-static)
        elseif(ANTLR_RUNTIME_USE_SHARED AND _shared_lib)
            add_library(antlr4-runtime::antlr4-runtime ALIAS antlr4-runtime::antlr4-runtime-shared)
        elseif(_shared_lib)
            # 默认优先使用动态库
            add_library(antlr4-runtime::antlr4-runtime ALIAS antlr4-runtime::antlr4-runtime-shared)
        elseif(_static_lib)
            add_library(antlr4-runtime::antlr4-runtime ALIAS antlr4-runtime::antlr4-runtime-static)
        endif()
    endif()
endfunction()

# 主查找逻辑
if(NOT ANTLR_RUNTIME_FOUND)
    # 查找根目录
    _antlr_find_root(_antlr_root)
    
    if(NOT _antlr_root)
        if(ANTLRRuntime_FIND_REQUIRED)
            message(FATAL_ERROR "Could not find ANTLR runtime. Please set ANTLR4_ROOT or install ANTLR runtime.")
        else()
            message(STATUS "ANTLR runtime not found")
            return()
        endif()
    endif()
    
    message(STATUS "Found ANTLR runtime root: ${_antlr_root}")
    
    # 查找头文件
    _antlr_find_includes(_antlr_include_dir "${_antlr_root}")
    
    if(NOT _antlr_include_dir)
        if(ANTLRRuntime_FIND_REQUIRED)
            message(FATAL_ERROR "Could not find ANTLR runtime headers in ${_antlr_root}")
        else()
            message(STATUS "ANTLR runtime headers not found")
            return()
        endif()
    endif()
    
    # 查找库文件
    _antlr_find_libraries(_antlr_static_lib _antlr_shared_lib "${_antlr_root}")
    
    if(NOT _antlr_static_lib AND NOT _antlr_shared_lib)
        if(ANTLRRuntime_FIND_REQUIRED)
            message(FATAL_ERROR "Could not find ANTLR runtime libraries in ${_antlr_root}")
        else()
            message(STATUS "ANTLR runtime libraries not found")
            return()
        endif()
    endif()
    
    # 获取版本
    _antlr_get_version(_antlr_version "${_antlr_root}")
    
    # 设置变量
    set(ANTLR_RUNTIME_FOUND TRUE)
    set(ANTLR_RUNTIME_ROOT "${_antlr_root}")
    set(ANTLR_RUNTIME_INCLUDE_DIRS "${_antlr_include_dir}")
    set(ANTLR_RUNTIME_VERSION "${_antlr_version}")
    set(ANTLR_RUNTIME_DEFINITIONS "")
    
    # 设置库文件变量
    set(ANTLR_RUNTIME_LIBRARIES "")
    if(_antlr_shared_lib)
        if(WIN32 AND ";" IN_LIST _antlr_shared_lib)
            list(GET _antlr_shared_lib 0 _import_lib)
            list(APPEND ANTLR_RUNTIME_LIBRARIES "${_import_lib}")
        else()
            list(APPEND ANTLR_RUNTIME_LIBRARIES "${_antlr_shared_lib}")
        endif()
    elseif(_antlr_static_lib)
        list(APPEND ANTLR_RUNTIME_LIBRARIES "${_antlr_static_lib}")
    endif()
    
    # 创建导入目标
    _antlr_create_targets("${_antlr_root}" "${_antlr_include_dir}" "${_antlr_static_lib}" "${_antlr_shared_lib}")
    
    # 输出信息
    message(STATUS "Found ANTLR runtime version: ${ANTLR_RUNTIME_VERSION}")
    message(STATUS "ANTLR runtime include dirs: ${ANTLR_RUNTIME_INCLUDE_DIRS}")
    message(STATUS "ANTLR runtime libraries: ${ANTLR_RUNTIME_LIBRARIES}")
    
    if(_antlr_static_lib)
        message(STATUS "ANTLR runtime static library: ${_antlr_static_lib}")
    endif()
    if(_antlr_shared_lib)
        message(STATUS "ANTLR runtime shared library: ${_antlr_shared_lib}")
    endif()
    
    # 缓存变量
    mark_as_advanced(
        ANTLR_RUNTIME_ROOT
        ANTLR_RUNTIME_INCLUDE_DIRS
        ANTLR_RUNTIME_LIBRARIES
        ANTLR_RUNTIME_VERSION
        ANTLR_RUNTIME_DEFINITIONS
    )
endif()

# 版本检查
if(ANTLRRuntime_FIND_VERSION)
    if(ANTLR_RUNTIME_VERSION VERSION_LESS ANTLRRuntime_FIND_VERSION)
        set(ANTLR_RUNTIME_FOUND FALSE)
        if(ANTLRRuntime_FIND_REQUIRED)
            message(FATAL_ERROR "ANTLR runtime version ${ANTLR_RUNTIME_VERSION} is less than required ${ANTLRRuntime_FIND_VERSION}")
        endif()
    endif()
endif()

# 标准FindPackage处理
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(ANTLRRuntime
    REQUIRED_VARS
        ANTLR_RUNTIME_INCLUDE_DIRS
        ANTLR_RUNTIME_LIBRARIES
    VERSION_VAR
        ANTLR_RUNTIME_VERSION
    HANDLE_COMPONENTS
)