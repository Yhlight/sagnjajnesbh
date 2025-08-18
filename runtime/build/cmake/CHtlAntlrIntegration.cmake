# CHtlAntlrIntegration.cmake
# CHTL项目ANTLR集成辅助函数
#
# 提供便捷的函数来集成ANTLR到CHTL项目中
#

cmake_minimum_required(VERSION 3.16)

# 设置ANTLR集成的默认配置
function(chtl_setup_antlr_defaults)
    # 设置默认的ANTLR工具路径
    if(NOT ANTLR_EXECUTABLE)
        find_file(ANTLR_EXECUTABLE
            NAMES antlr-4.13.2-complete.jar antlr4.jar antlr.jar
            PATHS
                "${CMAKE_SOURCE_DIR}/external"
                "${CMAKE_SOURCE_DIR}/runtime/tools"
                "${CMAKE_SOURCE_DIR}/tools"
            DOC "ANTLR4 tool JAR file"
        )
    endif()
    
    # 设置Java路径
    if(NOT Java_JAVA_EXECUTABLE)
        find_package(Java REQUIRED COMPONENTS Runtime)
    endif()
    
    # 设置默认的语法文件路径
    if(NOT ANTLR_GRAMMARS_DIR)
        set(ANTLR_GRAMMARS_DIR "${CMAKE_SOURCE_DIR}/external/grammars" CACHE PATH
            "Directory containing ANTLR grammar files")
    endif()
    
    # 设置默认的生成输出目录
    if(NOT ANTLR_GENERATED_DIR)
        set(ANTLR_GENERATED_DIR "${CMAKE_BINARY_DIR}/generated/antlr" CACHE PATH
            "Directory for ANTLR generated files")
    endif()
    
    message(STATUS "ANTLR executable: ${ANTLR_EXECUTABLE}")
    message(STATUS "ANTLR grammars directory: ${ANTLR_GRAMMARS_DIR}")
    message(STATUS "ANTLR generated directory: ${ANTLR_GENERATED_DIR}")
endfunction()

# 生成ANTLR解析器
# chtl_add_antlr_parser(
#     TARGET target_name
#     GRAMMAR grammar_file.g4
#     [PACKAGE package_name]
#     [NAMESPACE namespace_name]
#     [LISTENER ON|OFF]
#     [VISITOR ON|OFF]
#     [OUTPUT_DIR output_directory]
#     [DEPENDS dependency_files...]
# )
function(chtl_add_antlr_parser)
    cmake_parse_arguments(ANTLR
        ""
        "TARGET;GRAMMAR;PACKAGE;NAMESPACE;LISTENER;VISITOR;OUTPUT_DIR"
        "DEPENDS"
        ${ARGN}
    )
    
    if(NOT ANTLR_TARGET)
        message(FATAL_ERROR "chtl_add_antlr_parser: TARGET is required")
    endif()
    
    if(NOT ANTLR_GRAMMAR)
        message(FATAL_ERROR "chtl_add_antlr_parser: GRAMMAR is required")
    endif()
    
    # 查找语法文件
    if(NOT IS_ABSOLUTE "${ANTLR_GRAMMAR}")
        set(ANTLR_GRAMMAR "${ANTLR_GRAMMARS_DIR}/${ANTLR_GRAMMAR}")
    endif()
    
    if(NOT EXISTS "${ANTLR_GRAMMAR}")
        message(FATAL_ERROR "Grammar file not found: ${ANTLR_GRAMMAR}")
    endif()
    
    # 设置默认值
    if(NOT DEFINED ANTLR_LISTENER)
        set(ANTLR_LISTENER ON)
    endif()
    
    if(NOT DEFINED ANTLR_VISITOR)
        set(ANTLR_VISITOR OFF)
    endif()
    
    if(NOT ANTLR_OUTPUT_DIR)
        get_filename_component(GRAMMAR_NAME "${ANTLR_GRAMMAR}" NAME_WE)
        set(ANTLR_OUTPUT_DIR "${ANTLR_GENERATED_DIR}/${GRAMMAR_NAME}")
    endif()
    
    # 构建ANTLR命令参数
    set(ANTLR_ARGS -Dlanguage=Cpp)
    
    if(ANTLR_PACKAGE)
        list(APPEND ANTLR_ARGS -package ${ANTLR_PACKAGE})
    endif()
    
    if(ANTLR_NAMESPACE)
        list(APPEND ANTLR_ARGS -DnamespaceStl=${ANTLR_NAMESPACE})
    endif()
    
    if(ANTLR_LISTENER)
        list(APPEND ANTLR_ARGS -listener)
    else()
        list(APPEND ANTLR_ARGS -no-listener)
    endif()
    
    if(ANTLR_VISITOR)
        list(APPEND ANTLR_ARGS -visitor)
    else()
        list(APPEND ANTLR_ARGS -no-visitor)
    endif()
    
    list(APPEND ANTLR_ARGS -o "${ANTLR_OUTPUT_DIR}")
    
    # 确定生成的文件
    get_filename_component(GRAMMAR_NAME "${ANTLR_GRAMMAR}" NAME_WE)
    
    set(GENERATED_FILES)
    
    # 词法分析器文件
    if("${GRAMMAR_NAME}" MATCHES "Lexer$" OR "${ANTLR_GRAMMAR}" MATCHES "Lexer\\.g4$")
        list(APPEND GENERATED_FILES
            "${ANTLR_OUTPUT_DIR}/${GRAMMAR_NAME}.h"
            "${ANTLR_OUTPUT_DIR}/${GRAMMAR_NAME}.cpp"
        )
    endif()
    
    # 语法分析器文件
    if("${GRAMMAR_NAME}" MATCHES "Parser$" OR "${ANTLR_GRAMMAR}" MATCHES "Parser\\.g4$")
        list(APPEND GENERATED_FILES
            "${ANTLR_OUTPUT_DIR}/${GRAMMAR_NAME}.h"
            "${ANTLR_OUTPUT_DIR}/${GRAMMAR_NAME}.cpp"
        )
        
        if(ANTLR_LISTENER)
            list(APPEND GENERATED_FILES
                "${ANTLR_OUTPUT_DIR}/${GRAMMAR_NAME}BaseListener.h"
                "${ANTLR_OUTPUT_DIR}/${GRAMMAR_NAME}BaseListener.cpp"
                "${ANTLR_OUTPUT_DIR}/${GRAMMAR_NAME}Listener.h"
                "${ANTLR_OUTPUT_DIR}/${GRAMMAR_NAME}Listener.cpp"
            )
        endif()
        
        if(ANTLR_VISITOR)
            list(APPEND GENERATED_FILES
                "${ANTLR_OUTPUT_DIR}/${GRAMMAR_NAME}BaseVisitor.h"
                "${ANTLR_OUTPUT_DIR}/${GRAMMAR_NAME}BaseVisitor.cpp"
                "${ANTLR_OUTPUT_DIR}/${GRAMMAR_NAME}Visitor.h"
                "${ANTLR_OUTPUT_DIR}/${GRAMMAR_NAME}Visitor.cpp"
            )
        endif()
    endif()
    
    # 通用语法文件（既不是纯Lexer也不是纯Parser）
    if(NOT "${GRAMMAR_NAME}" MATCHES "(Lexer|Parser)$")
        list(APPEND GENERATED_FILES
            "${ANTLR_OUTPUT_DIR}/${GRAMMAR_NAME}Lexer.h"
            "${ANTLR_OUTPUT_DIR}/${GRAMMAR_NAME}Lexer.cpp"
            "${ANTLR_OUTPUT_DIR}/${GRAMMAR_NAME}Parser.h"
            "${ANTLR_OUTPUT_DIR}/${GRAMMAR_NAME}Parser.cpp"
        )
        
        if(ANTLR_LISTENER)
            list(APPEND GENERATED_FILES
                "${ANTLR_OUTPUT_DIR}/${GRAMMAR_NAME}BaseListener.h"
                "${ANTLR_OUTPUT_DIR}/${GRAMMAR_NAME}BaseListener.cpp"
                "${ANTLR_OUTPUT_DIR}/${GRAMMAR_NAME}Listener.h"
                "${ANTLR_OUTPUT_DIR}/${GRAMMAR_NAME}Listener.cpp"
            )
        endif()
        
        if(ANTLR_VISITOR)
            list(APPEND GENERATED_FILES
                "${ANTLR_OUTPUT_DIR}/${GRAMMAR_NAME}BaseVisitor.h"
                "${ANTLR_OUTPUT_DIR}/${GRAMMAR_NAME}BaseVisitor.cpp"
                "${ANTLR_OUTPUT_DIR}/${GRAMMAR_NAME}Visitor.h"
                "${ANTLR_OUTPUT_DIR}/${GRAMMAR_NAME}Visitor.cpp"
            )
        endif()
    endif()
    
    # 创建生成命令
    add_custom_command(
        OUTPUT ${GENERATED_FILES}
        COMMAND ${CMAKE_COMMAND} -E make_directory "${ANTLR_OUTPUT_DIR}"
        COMMAND ${Java_JAVA_EXECUTABLE} -jar "${ANTLR_EXECUTABLE}" ${ANTLR_ARGS} "${ANTLR_GRAMMAR}"
        DEPENDS "${ANTLR_GRAMMAR}" ${ANTLR_DEPENDS}
        COMMENT "Generating ANTLR parser for ${GRAMMAR_NAME}"
        VERBATIM
    )
    
    # 创建目标库
    add_library(${ANTLR_TARGET} STATIC ${GENERATED_FILES})
    
    # 设置包含目录
    target_include_directories(${ANTLR_TARGET} PUBLIC "${ANTLR_OUTPUT_DIR}")
    
    # 链接ANTLR运行时
    if(TARGET antlr4-runtime::antlr4-runtime)
        target_link_libraries(${ANTLR_TARGET} PUBLIC antlr4-runtime::antlr4-runtime)
    else()
        message(WARNING "ANTLR runtime not found. Please call find_package(ANTLRRuntime) first.")
    endif()
    
    # 设置编译属性
    target_compile_features(${ANTLR_TARGET} PUBLIC cxx_std_17)
    
    # 禁用生成代码的警告
    if(MSVC)
        target_compile_options(${ANTLR_TARGET} PRIVATE /wd4267 /wd4244 /wd4996)
    else()
        target_compile_options(${ANTLR_TARGET} PRIVATE -Wno-unused-parameter -Wno-unused-variable)
    endif()
    
    message(STATUS "Added ANTLR parser target: ${ANTLR_TARGET}")
    message(STATUS "  Grammar: ${ANTLR_GRAMMAR}")
    message(STATUS "  Output: ${ANTLR_OUTPUT_DIR}")
endfunction()

# 设置CHTL项目的ANTLR编译器
# chtl_setup_antlr_compilers()
function(chtl_setup_antlr_compilers)
    # CSS编译器
    if(EXISTS "${ANTLR_GRAMMARS_DIR}/css3Lexer.g4" AND 
       EXISTS "${ANTLR_GRAMMARS_DIR}/css3Parser.g4")
        
        chtl_add_antlr_parser(
            TARGET chtl_css3_lexer
            GRAMMAR css3Lexer.g4
            NAMESPACE chtl::css
            LISTENER OFF
            VISITOR OFF
        )
        
        chtl_add_antlr_parser(
            TARGET chtl_css3_parser
            GRAMMAR css3Parser.g4
            NAMESPACE chtl::css
            LISTENER ON
            VISITOR OFF
            DEPENDS "${ANTLR_GRAMMARS_DIR}/css3Lexer.g4"
        )
        
        # 创建组合目标
        add_library(chtl_css_antlr INTERFACE)
        target_link_libraries(chtl_css_antlr INTERFACE chtl_css3_lexer chtl_css3_parser)
        
        message(STATUS "Created CSS ANTLR parser targets")
    endif()
    
    # JavaScript编译器
    if(EXISTS "${ANTLR_GRAMMARS_DIR}/JavaScriptLexer_cpp.g4" AND 
       EXISTS "${ANTLR_GRAMMARS_DIR}/JavaScriptParser_cpp.g4")
        
        chtl_add_antlr_parser(
            TARGET chtl_js_lexer
            GRAMMAR JavaScriptLexer_cpp.g4
            NAMESPACE chtl::javascript
            LISTENER OFF
            VISITOR OFF
        )
        
        chtl_add_antlr_parser(
            TARGET chtl_js_parser
            GRAMMAR JavaScriptParser_cpp.g4
            NAMESPACE chtl::javascript
            LISTENER ON
            VISITOR OFF
            DEPENDS "${ANTLR_GRAMMARS_DIR}/JavaScriptLexer_cpp.g4"
        )
        
        # 创建组合目标
        add_library(chtl_js_antlr INTERFACE)
        target_link_libraries(chtl_js_antlr INTERFACE chtl_js_lexer chtl_js_parser)
        
        message(STATUS "Created JavaScript ANTLR parser targets")
    endif()
endfunction()

# 简化的CHTL ANTLR设置函数
# chtl_enable_antlr()
function(chtl_enable_antlr)
    # 设置默认值
    chtl_setup_antlr_defaults()
    
    # 查找ANTLR运行时
    find_package(ANTLRRuntime REQUIRED)
    
    # 设置编译器
    chtl_setup_antlr_compilers()
    
    message(STATUS "CHTL ANTLR integration enabled")
endfunction()

# 验证ANTLR设置
function(chtl_verify_antlr_setup)
    set(ERRORS)
    
    # 检查ANTLR工具
    if(NOT ANTLR_EXECUTABLE OR NOT EXISTS "${ANTLR_EXECUTABLE}")
        list(APPEND ERRORS "ANTLR executable not found: ${ANTLR_EXECUTABLE}")
    endif()
    
    # 检查Java
    if(NOT Java_JAVA_EXECUTABLE)
        list(APPEND ERRORS "Java executable not found")
    endif()
    
    # 检查运行时
    if(NOT TARGET antlr4-runtime::antlr4-runtime)
        list(APPEND ERRORS "ANTLR runtime target not found")
    endif()
    
    # 检查语法文件目录
    if(NOT EXISTS "${ANTLR_GRAMMARS_DIR}")
        list(APPEND ERRORS "ANTLR grammars directory not found: ${ANTLR_GRAMMARS_DIR}")
    endif()
    
    if(ERRORS)
        message(STATUS "ANTLR setup verification failed:")
        foreach(ERROR ${ERRORS})
            message(STATUS "  - ${ERROR}")
        endforeach()
        return()
    endif()
    
    message(STATUS "ANTLR setup verification passed")
    message(STATUS "  ANTLR tool: ${ANTLR_EXECUTABLE}")
    message(STATUS "  Java: ${Java_JAVA_EXECUTABLE}")
    message(STATUS "  Runtime: Found")
    message(STATUS "  Grammars: ${ANTLR_GRAMMARS_DIR}")
endfunction()