# CHTL编译器核心库
add_library(chtl_core STATIC
    CHTL/Core/CHTLToken.cpp
    CHTL/Core/CHTLGlobalMap.cpp
    CHTL/Core/CHTLState.cpp
    CHTL/Core/CHTLContext.cpp
    CHTL/Core/CHTLStateContext.cpp
    CHTL/Lexer/CHTLLexer.cpp
    CHTL/Parser/CHTLParser.cpp
    CHTL/AST/CHTLASTNodes.cpp
    CHTL/AST/CHTLASTVisitor.cpp
    CHTL/AST/CHTLASTBuilder.cpp
    CHTL/Generator/CHTLGenerator.cpp
    CHTL/Constraints/CHTLConstraintValidator.cpp
    CHTL/Constraints/ExceptConstraintParser.cpp
    CHTL/Selector/SelectorAutomation.cpp
    CHTL/Import/EnhancedImportSystem.cpp
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

# 编译器调度器库
add_library(dispatcher STATIC
    Dispatcher/CompilerDispatcher.cpp
)

target_include_directories(dispatcher PUBLIC
    ${CMAKE_SOURCE_DIR}/include/Dispatcher
)

# CSS编译器库（使用ANTLR4生成的代码）
add_library(css_compiler STATIC
    CSS/CSSCompiler.cpp
    ../css/css3Lexer.cpp
    ../css/css3Parser.cpp
    ../css/css3ParserBaseListener.cpp
    ../css/css3ParserListener.cpp
)

target_include_directories(css_compiler PUBLIC
    ${CMAKE_SOURCE_DIR}/include/CSS
    ${CMAKE_SOURCE_DIR}/css
)

target_link_libraries(css_compiler ${ANTLR4_LIBRARIES})

# JavaScript编译器库（使用ANTLR4生成的代码）
add_library(js_compiler STATIC
    JavaScript/JavaScriptCompiler.cpp
    ../js/JavaScriptLexer.cpp
    ../js/JavaScriptLexerBase.cpp
    ../js/JavaScriptParser.cpp
    ../js/JavaScriptParserBase.cpp
    ../js/JavaScriptParserBaseListener.cpp
    ../js/JavaScriptParserListener.cpp
)

target_include_directories(js_compiler PUBLIC
    ${CMAKE_SOURCE_DIR}/include/JavaScript
    ${CMAKE_SOURCE_DIR}/js
)

target_link_libraries(js_compiler ${ANTLR4_LIBRARIES})

# CMOD模块系统库
add_library(cmod STATIC
    CMOD/CMODManager.cpp
)

target_include_directories(cmod PUBLIC
    ${CMAKE_SOURCE_DIR}/include/CMOD
)

# CJMOD扩展系统库
add_library(cjmod STATIC
    CJMOD/CJMODApi.cpp
    CJMOD/Extensions/PrintMyloveExtension.cpp
    CJMOD/Extensions/INeverAwayExtension.cpp
)

target_include_directories(cjmod PUBLIC
    ${CMAKE_SOURCE_DIR}/include/CJMOD
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