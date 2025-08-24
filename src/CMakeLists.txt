cmake_minimum_required(VERSION 3.16)
project(CHTL)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 全局include路径
include_directories(${CMAKE_SOURCE_DIR}/../include)

# CHTL编译器核心库
add_library(chtl_core STATIC
    CHTL/Core/CHTLToken.cpp
    CHTL/Core/CHTLGlobalMap.cpp
    CHTL/Core/CHTLState.cpp
    CHTL/Core/CHTLContext.cpp
    CHTL/Core/CHTLStateContext.cpp
    CHTL/Core/ImportManager.cpp
    CHTL/Core/NamespaceMerger.cpp
    CHTL/Lexer/CHTLLexer.cpp
    CHTL/Parser/CHTLParser.cpp
    CHTL/AST/CHTLASTNodes.cpp
    CHTL/AST/CHTLASTVisitor.cpp
    CHTL/AST/CHTLASTBuilder.cpp
    CHTL/Generator/CHTLGenerator.cpp
    CHTL/Constraints/CHTLConstraintValidator.cpp
    CHTL/Constraints/ExceptConstraintParser.cpp
    CHTL/Selector/SelectorAutomation.cpp
    CHTL/Import/ImportSystem.cpp
    CHTL/Comments/ContextualCommentSystem.cpp
)

target_include_directories(chtl_core PUBLIC
    ${CMAKE_SOURCE_DIR}/include/CHTL
)

target_link_libraries(chtl_core cmod)

# CHTL JS编译器核心库
add_library(chtljs_core STATIC
    CHTLJS/Core/CHTLJSToken.cpp
    CHTLJS/Core/CHTLJSGlobalMap.cpp
    CHTLJS/Core/CHTLJSState.cpp
    CHTLJS/Core/CHTLJSContext.cpp
    CHTLJS/Lexer/CHTLJSLexer.cpp
    CHTLJS/Parser/CHTLJSParser.cpp
    CHTLJS/AST/CHTLJSASTNodes.cpp
    CHTLJS/Generator/CHTLJSGenerator.cpp
    CHTLJS/Events/JSEventSystem.cpp
    CHTLJS/Core/CHTLJSStateContext.cpp
    CHTLJS/Compiler/CHTLJSCompiler.cpp
)

target_include_directories(chtljs_core PUBLIC
    ${CMAKE_SOURCE_DIR}/include/CHTLJS
)

# 统一扫描器库
add_library(scanner STATIC
    Scanner/CHTLUnifiedScanner.cpp
)

target_include_directories(scanner PUBLIC
    ${CMAKE_SOURCE_DIR}/include/Scanner
)

# 分发器库
add_library(dispatcher STATIC
    Dispatcher/CompilerDispatcher.cpp
    Dispatcher/FragmentProcessors.cpp
)

target_include_directories(dispatcher PUBLIC
    ${CMAKE_SOURCE_DIR}/include/Dispatcher
)

target_link_libraries(dispatcher
    chtl_core
    chtljs_core
    scanner
    css_compiler
    js_compiler
)

# 工具库
add_library(utils STATIC
    Utils/FileUtils.cpp
    Utils/StringUtils.cpp
    Utils/ErrorHandler.cpp
)

target_include_directories(utils PUBLIC
    ${CMAKE_SOURCE_DIR}/include/Utils
)

# CSS编译器库
add_library(css_compiler STATIC
    CSS/CSSCompiler.cpp
)

target_include_directories(css_compiler PUBLIC
    ${CMAKE_SOURCE_DIR}/include/CSS
)

# JS编译器库
add_library(js_compiler STATIC
    JavaScript/JavaScriptCompiler.cpp
)

target_include_directories(js_compiler PUBLIC
    ${CMAKE_SOURCE_DIR}/include/JavaScript
)

# CMOD库 - 统一的CMOD系统
add_library(cmod STATIC
    CMOD/CMODSystem.cpp
)

target_include_directories(cmod PUBLIC
    ${CMAKE_SOURCE_DIR}/include/CMOD
)

target_link_libraries(cmod utils)

# CJMOD库 - 使用真正的CJMOD API
add_library(cjmod STATIC
    CJMOD/TrueCJMODApi.cpp
)

target_include_directories(cjmod PUBLIC
    ${CMAKE_SOURCE_DIR}/include/CJMOD
)

target_link_libraries(cjmod utils)

# 为CJMOD库添加-fPIC编译选项以支持动态库链接
set_target_properties(cjmod PROPERTIES
    POSITION_INDEPENDENT_CODE ON
)

# 模块构建工具
add_executable(module_builder
    Tools/module_builder.cpp
)

target_link_libraries(module_builder
    cmod
    cjmod
    utils
)

# 主程序
add_executable(chtl_compiler
    main.cpp
)

# CMOD导出生成工具
add_executable(cmod_export_generator
    Tools/cmod_export_generator.cpp
)

target_link_libraries(cmod_export_generator
    cmod
    utils
)

target_link_libraries(chtl_compiler
    chtl_core
    chtljs_core
    scanner
    dispatcher
    css_compiler
    js_compiler
    cmod
    utils
    ${ANTLR4_LIBRARIES}
)

if(WIN32)
    target_link_libraries(chtl_compiler ws2_32)
endif()

# ========== 模块自动构建系统 ==========
# 按照您的设计：自动构建src/Module中的模块，输出到与exe同级的module文件夹

# 查找所有模块源目录
file(GLOB MODULE_DIRS "${CMAKE_SOURCE_DIR}/src/Module/*")

# 为每个模块创建构建规则
foreach(MODULE_DIR ${MODULE_DIRS})
    get_filename_component(MODULE_NAME ${MODULE_DIR} NAME)
    
    # 检查模块结构类型
    if(EXISTS "${MODULE_DIR}/info")
        # 标准CMOD模块结构 (有info文件夹)
        set(MODULE_TYPE "CMOD")
        set(INFO_FILE "${MODULE_DIR}/info/${MODULE_NAME}.chtl")
        
        if(EXISTS "${MODULE_DIR}/src")
            set(SRC_FILES "${MODULE_DIR}/src/*.chtl")
        else()
            set(SRC_FILES "${MODULE_DIR}/*.chtl")
        endif()
        
        # 检查是否有CJMOD扩展
        if(EXISTS "${MODULE_DIR}/CJMOD")
            set(MODULE_TYPE "MIXED")  # CMOD + CJMOD混合模块
            file(GLOB_RECURSE CJMOD_FILES "${MODULE_DIR}/CJMOD/*.cpp")
        endif()
        
    else()
        # 简单模块结构 (无序结构，直接在根目录)
        set(MODULE_TYPE "SIMPLE")
        file(GLOB SRC_FILES "${MODULE_DIR}/*.cmod" "${MODULE_DIR}/*.chtl")
    endif()
    
    # 设置输出目录
    set(MODULE_OUTPUT_DIR "${CMAKE_BINARY_DIR}/bin/module/${MODULE_NAME}")
    
    # 创建模块构建目标
    add_custom_target(build_module_${MODULE_NAME}
        COMMENT "构建模块: ${MODULE_NAME} (类型: ${MODULE_TYPE})"
    )
    
    # 根据模块类型处理
    if(MODULE_TYPE STREQUAL "CMOD" OR MODULE_TYPE STREQUAL "MIXED")
        # 标准CMOD模块处理
        if(EXISTS ${INFO_FILE})
            add_custom_command(TARGET build_module_${MODULE_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E make_directory ${MODULE_OUTPUT_DIR}/info
                COMMAND ${CMAKE_COMMAND} -E copy ${INFO_FILE} ${MODULE_OUTPUT_DIR}/info/
                COMMENT "复制模块信息文件: ${MODULE_NAME}"
            )
        endif()
        
        # 复制源文件
        file(GLOB MODULE_SRC_FILES "${MODULE_DIR}/src/*.chtl")
        foreach(SRC_FILE ${MODULE_SRC_FILES})
            add_custom_command(TARGET build_module_${MODULE_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E make_directory ${MODULE_OUTPUT_DIR}/src
                COMMAND ${CMAKE_COMMAND} -E copy ${SRC_FILE} ${MODULE_OUTPUT_DIR}/src/
                COMMENT "复制模块源文件: ${SRC_FILE}"
            )
        endforeach()
        
        # 处理CJMOD扩展 (如果存在)
        if(MODULE_TYPE STREQUAL "MIXED")
            file(GLOB_RECURSE CJMOD_SRC_FILES "${MODULE_DIR}/CJMOD/*.cpp" "${MODULE_DIR}/CJMOD/*.h")
            foreach(CJMOD_FILE ${CJMOD_SRC_FILES})
                file(RELATIVE_PATH REL_PATH ${MODULE_DIR} ${CJMOD_FILE})
                get_filename_component(REL_DIR ${REL_PATH} DIRECTORY)
                
                add_custom_command(TARGET build_module_${MODULE_NAME} POST_BUILD
                    COMMAND ${CMAKE_COMMAND} -E make_directory ${MODULE_OUTPUT_DIR}/${REL_DIR}
                    COMMAND ${CMAKE_COMMAND} -E copy ${CJMOD_FILE} ${MODULE_OUTPUT_DIR}/${REL_DIR}/
                    COMMENT "复制CJMOD扩展文件: ${CJMOD_FILE}"
                )
            endforeach()
            
            # 编译CJMOD扩展为动态库
            file(GLOB_RECURSE CJMOD_CPP_FILES "${MODULE_DIR}/CJMOD/*.cpp")
            if(CJMOD_CPP_FILES)
                add_library(${MODULE_NAME}_cjmod SHARED ${CJMOD_CPP_FILES})
                target_include_directories(${MODULE_NAME}_cjmod PRIVATE
                    ${CMAKE_SOURCE_DIR}/include
                    ${CMAKE_SOURCE_DIR}/include/CJMOD
                )
                target_link_libraries(${MODULE_NAME}_cjmod cjmod utils)
                
                # 设置输出位置
                set_target_properties(${MODULE_NAME}_cjmod PROPERTIES
                    LIBRARY_OUTPUT_DIRECTORY ${MODULE_OUTPUT_DIR}/lib
                    RUNTIME_OUTPUT_DIRECTORY ${MODULE_OUTPUT_DIR}/lib
                )
                
                add_dependencies(build_module_${MODULE_NAME} ${MODULE_NAME}_cjmod)
            endif()
        endif()
        
    elseif(MODULE_TYPE STREQUAL "SIMPLE")
        # 简单模块处理 (无序结构)
        file(GLOB SIMPLE_MODULE_FILES "${MODULE_DIR}/*.cmod" "${MODULE_DIR}/*.chtl")
        foreach(SIMPLE_FILE ${SIMPLE_MODULE_FILES})
            add_custom_command(TARGET build_module_${MODULE_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E make_directory ${MODULE_OUTPUT_DIR}
                COMMAND ${CMAKE_COMMAND} -E copy ${SIMPLE_FILE} ${MODULE_OUTPUT_DIR}/
                COMMENT "复制简单模块文件: ${SIMPLE_FILE}"
            )
        endforeach()
    endif()
    
    # 添加到总的模块构建目标
    add_dependencies(chtl_compiler build_module_${MODULE_NAME})
endforeach()

# 创建模块构建汇总目标
add_custom_target(build_all_modules
    COMMENT "构建所有模块"
)

# 让所有模块构建目标依赖于汇总目标
foreach(MODULE_DIR ${MODULE_DIRS})
    get_filename_component(MODULE_NAME ${MODULE_DIR} NAME)
    add_dependencies(build_all_modules build_module_${MODULE_NAME})
endforeach()

# 确保主程序构建时自动构建所有模块
add_dependencies(chtl_compiler build_all_modules)

# 创建模块清理目标
add_custom_target(clean_modules
    COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_BINARY_DIR}/bin/module
    COMMENT "清理所有构建的模块"
)