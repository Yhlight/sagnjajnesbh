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
    Scanner/FragmentIndexManager.cpp
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

# CMOD导出生成工具 (暂时禁用，需要修复API)
# add_executable(cmod_export_generator
#     Tools/cmod_export_generator.cpp
# )
# 
# target_link_libraries(cmod_export_generator
#     cmod
#     utils
# )

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
# 按照您的设计：编译src/Module中的源码，输出到module文件夹
# 支持无序结构和分类结构（CMOD/、CJMOD/目录）

# 检测src/Module的结构类型
function(detect_module_structure MODULE_SOURCE_DIR STRUCTURE_TYPE)
    # 检查是否为分类结构（存在CMOD/或CJMOD/目录）
    set(IS_CLASSIFIED FALSE)
    
    # 检查各种可能的分类目录名
    foreach(CMOD_DIR "CMOD" "Cmod" "cmod")
        if(EXISTS "${MODULE_SOURCE_DIR}/${CMOD_DIR}")
            set(IS_CLASSIFIED TRUE)
            break()
        endif()
    endforeach()
    
    foreach(CJMOD_DIR "CJMOD" "CJmod" "cjmod")
        if(EXISTS "${MODULE_SOURCE_DIR}/${CJMOD_DIR}")
            set(IS_CLASSIFIED TRUE)
            break()
        endif()
    endforeach()
    
    if(IS_CLASSIFIED)
        set(${STRUCTURE_TYPE} "CLASSIFIED" PARENT_SCOPE)
    else()
        set(${STRUCTURE_TYPE} "MIXED" PARENT_SCOPE)
    endif()
endfunction()

# 主模块构建逻辑
set(MODULE_SOURCE_DIR "${CMAKE_SOURCE_DIR}/src/Module")
set(MODULE_OUTPUT_DIR "${CMAKE_BINARY_DIR}/bin/module")

if(EXISTS ${MODULE_SOURCE_DIR})
    detect_module_structure(${MODULE_SOURCE_DIR} MODULE_STRUCTURE)
    
    message(STATUS "检测到模块结构类型: ${MODULE_STRUCTURE}")
    
    if(MODULE_STRUCTURE STREQUAL "CLASSIFIED")
        # 分类结构：处理CMOD/和CJMOD/目录
        message(STATUS "使用分类结构构建模块")
        
        # 查找并构建CMOD组件
        foreach(CMOD_DIR "CMOD" "Cmod" "cmod")
            if(EXISTS "${MODULE_SOURCE_DIR}/${CMOD_DIR}")
                file(GLOB CMOD_COMPONENTS "${MODULE_SOURCE_DIR}/${CMOD_DIR}/*")
                foreach(COMPONENT_PATH ${CMOD_COMPONENTS})
                    if(IS_DIRECTORY ${COMPONENT_PATH})
                        get_filename_component(COMPONENT_NAME ${COMPONENT_PATH} NAME)
                        build_mixed_cmod_component(${COMPONENT_PATH} ${COMPONENT_NAME} "${MODULE_OUTPUT_DIR}/${CMOD_DIR}")
                    endif()
                endforeach()
                break()
            endif()
        endforeach()
        
        # 查找并构建CJMOD扩展
        foreach(CJMOD_DIR "CJMOD" "CJmod" "cjmod")
            if(EXISTS "${MODULE_SOURCE_DIR}/${CJMOD_DIR}")
                file(GLOB CJMOD_EXTENSIONS "${MODULE_SOURCE_DIR}/${CJMOD_DIR}/*")
                foreach(EXTENSION_PATH ${CJMOD_EXTENSIONS})
                    if(IS_DIRECTORY ${EXTENSION_PATH})
                        get_filename_component(EXTENSION_NAME ${EXTENSION_PATH} NAME)
                        build_mixed_cjmod_extension(${EXTENSION_PATH} ${EXTENSION_NAME} "${MODULE_OUTPUT_DIR}/${CJMOD_DIR}")
                    endif()
                endforeach()
                break()
            endif()
        endforeach()
        
    else()
        # 无序结构：所有模块都在src/Module根目录
        message(STATUS "使用无序结构构建模块")
        file(GLOB MODULE_DIRS "${MODULE_SOURCE_DIR}/*")
        foreach(MODULE_DIR ${MODULE_DIRS})
            if(IS_DIRECTORY ${MODULE_DIR})
                get_filename_component(MODULE_NAME ${MODULE_DIR} NAME)
                
                # 检测模块类型
                if(EXISTS "${MODULE_DIR}/CMOD" AND EXISTS "${MODULE_DIR}/CJMOD")
                    # 混合模块（结构3）
                    message(STATUS "检测到混合模块: ${MODULE_NAME}")
                    
                    # 构建CMOD组件
                    file(GLOB CMOD_COMPONENTS "${MODULE_DIR}/CMOD/*")
                    foreach(COMPONENT_PATH ${CMOD_COMPONENTS})
                        if(IS_DIRECTORY ${COMPONENT_PATH})
                            get_filename_component(COMPONENT_NAME ${COMPONENT_PATH} NAME)
                            build_mixed_cmod_component(${COMPONENT_PATH} ${COMPONENT_NAME} "${MODULE_OUTPUT_DIR}")
                        endif()
                    endforeach()
                    
                    # 构建CJMOD扩展
                    file(GLOB CJMOD_EXTENSIONS "${MODULE_DIR}/CJMOD/*")
                    foreach(EXTENSION_PATH ${CJMOD_EXTENSIONS})
                        if(IS_DIRECTORY ${EXTENSION_PATH})
                            get_filename_component(EXTENSION_NAME ${EXTENSION_PATH} NAME)
                            build_mixed_cjmod_extension(${EXTENSION_PATH} ${EXTENSION_NAME} "${MODULE_OUTPUT_DIR}")
                        endif()
                    endforeach()
                    
                elseif(EXISTS "${MODULE_DIR}/src" AND EXISTS "${MODULE_DIR}/info")
                    # 判断是CMOD还是CJMOD
                    file(GLOB CPP_FILES "${MODULE_DIR}/src/*.cpp")
                    file(GLOB CHTL_FILES "${MODULE_DIR}/src/*.chtl")
                    
                    if(CPP_FILES)
                        # CJMOD模块（结构2）
                        build_cjmod_module(${MODULE_DIR} ${MODULE_NAME} ${MODULE_OUTPUT_DIR})
                    elseif(CHTL_FILES)
                        # 简单CMOD模块（结构1）
                        build_simple_cmod_module(${MODULE_DIR} ${MODULE_NAME} ${MODULE_OUTPUT_DIR})
                    endif()
                endif()
            endif()
        endforeach()
    endif()
    
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

# ========== 模块构建函数定义 ==========

# 构建简单CMOD模块（结构1）
function(build_simple_cmod_module MODULE_PATH MODULE_NAME OUTPUT_DIR)
    message(STATUS "构建简单CMOD模块: ${MODULE_NAME}")
    
    add_custom_target(build_module_${MODULE_NAME}
        COMMENT "构建CMOD模块: ${MODULE_NAME}"
    )
    
    # 编译.chtl文件为.cmod
    file(GLOB CHTL_FILES "${MODULE_PATH}/src/*.chtl")
    foreach(CHTL_FILE ${CHTL_FILES})
        get_filename_component(FILENAME ${CHTL_FILE} NAME_WE)
        set(OUTPUT_FILE "${OUTPUT_DIR}/${FILENAME}.cmod")
        
        add_custom_command(TARGET build_module_${MODULE_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E make_directory ${OUTPUT_DIR}
            COMMAND ${CMAKE_BINARY_DIR}/bin/chtl_compiler ${CHTL_FILE} -o ${OUTPUT_FILE}
            COMMENT "编译CHTL文件: ${CHTL_FILE} -> ${OUTPUT_FILE}"
            DEPENDS chtl_compiler ${CHTL_FILE}
        )
    endforeach()
    
    # 复制模块信息文件
    if(EXISTS "${MODULE_PATH}/info/${MODULE_NAME}.chtl")
        add_custom_command(TARGET build_module_${MODULE_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy 
                "${MODULE_PATH}/info/${MODULE_NAME}.chtl"
                "${OUTPUT_DIR}/${MODULE_NAME}.info"
            COMMENT "复制模块信息文件"
        )
    endif()
    
    add_dependencies(build_all_modules build_module_${MODULE_NAME})
endfunction()

# 构建CJMOD模块（结构2）
function(build_cjmod_module MODULE_PATH MODULE_NAME OUTPUT_DIR)
    message(STATUS "构建CJMOD模块: ${MODULE_NAME}")
    
    # 编译C++源文件为动态库
    file(GLOB CPP_FILES "${MODULE_PATH}/src/*.cpp")
    if(CPP_FILES)
        add_library(${MODULE_NAME}_cjmod SHARED ${CPP_FILES})
        target_include_directories(${MODULE_NAME}_cjmod PRIVATE 
            ${CMAKE_SOURCE_DIR}/include
            ${MODULE_PATH}/src
        )
        
        # 设置输出名称为.cjmod
        set_target_properties(${MODULE_NAME}_cjmod PROPERTIES
            OUTPUT_NAME ${MODULE_NAME}
            SUFFIX ".cjmod"
            LIBRARY_OUTPUT_DIRECTORY ${OUTPUT_DIR}
        )
        
        target_link_libraries(${MODULE_NAME}_cjmod cjmod utils)
        
        # 复制模块信息文件
        if(EXISTS "${MODULE_PATH}/info/${MODULE_NAME}.chtl")
            add_custom_command(TARGET ${MODULE_NAME}_cjmod POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy 
                    "${MODULE_PATH}/info/${MODULE_NAME}.chtl"
                    "${OUTPUT_DIR}/${MODULE_NAME}.info"
                COMMENT "复制CJMOD模块信息文件"
            )
        endif()
        
        add_dependencies(build_all_modules ${MODULE_NAME}_cjmod)
    endif()
endfunction()

# 构建混合模块中的CMOD组件（结构3）
function(build_mixed_cmod_component COMPONENT_PATH COMPONENT_NAME OUTPUT_DIR)
    message(STATUS "构建混合CMOD组件: ${COMPONENT_NAME}")
    
    add_custom_target(build_cmod_${COMPONENT_NAME}
        COMMENT "构建混合CMOD组件: ${COMPONENT_NAME}"
    )
    
    # 编译组件的.chtl文件
    file(GLOB CHTL_FILES "${COMPONENT_PATH}/src/*.chtl")
    foreach(CHTL_FILE ${CHTL_FILES})
        get_filename_component(FILENAME ${CHTL_FILE} NAME_WE)
        set(OUTPUT_FILE "${OUTPUT_DIR}/${FILENAME}.cmod")
        
        add_custom_command(TARGET build_cmod_${COMPONENT_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E make_directory ${OUTPUT_DIR}
            COMMAND ${CMAKE_BINARY_DIR}/bin/chtl_compiler ${CHTL_FILE} -o ${OUTPUT_FILE}
            COMMENT "编译混合CMOD组件: ${CHTL_FILE} -> ${OUTPUT_FILE}"
            DEPENDS chtl_compiler ${CHTL_FILE}
        )
    endforeach()
    
    add_dependencies(build_all_modules build_cmod_${COMPONENT_NAME})
endfunction()

# 构建混合模块中的CJMOD扩展（结构3）
function(build_mixed_cjmod_extension EXTENSION_PATH EXTENSION_NAME OUTPUT_DIR)
    message(STATUS "构建混合CJMOD扩展: ${EXTENSION_NAME}")
    
    # 编译C++扩展
    file(GLOB CPP_FILES "${EXTENSION_PATH}/src/*.cpp")
    if(CPP_FILES)
        add_library(${EXTENSION_NAME}_cjmod SHARED ${CPP_FILES})
        target_include_directories(${EXTENSION_NAME}_cjmod PRIVATE 
            ${CMAKE_SOURCE_DIR}/include
            ${EXTENSION_PATH}/src
        )
        
        set_target_properties(${EXTENSION_NAME}_cjmod PROPERTIES
            OUTPUT_NAME ${EXTENSION_NAME}
            SUFFIX ".cjmod"
            LIBRARY_OUTPUT_DIRECTORY ${OUTPUT_DIR}
        )
        
        target_link_libraries(${EXTENSION_NAME}_cjmod cjmod utils)
        add_dependencies(build_all_modules ${EXTENSION_NAME}_cjmod)
    endif()
endfunction()

# 创建总的构建目标
add_custom_target(build_all_modules
    COMMENT "构建所有模块"
)

# 确保主程序构建时自动构建所有模块
add_dependencies(chtl_compiler build_all_modules)

# 创建模块清理目标
add_custom_target(clean_modules
    COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_BINARY_DIR}/bin/module
    COMMENT "清理所有构建的模块"
)