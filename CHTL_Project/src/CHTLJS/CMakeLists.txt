set(CHTLJS_SOURCES
    CHTLJSToken.cpp
    CHTLJSGlobalMap.cpp
    CHTLJSState.cpp
    CHTLJSContext.cpp
    CHTLJSLexer.cpp
    CHTLJSParser.cpp
    CHTLJSGenerator.cpp
    CHTLJSCompiler.cpp
)

set(CHTLJS_HEADERS
    ../../include/CHTLJS/CHTLJSToken.hpp
    ../../include/CHTLJS/CHTLJSGlobalMap.hpp
    ../../include/CHTLJS/CHTLJSState.hpp
    ../../include/CHTLJS/CHTLJSContext.hpp
    ../../include/CHTLJS/CHTLJSLexer.hpp
    ../../include/CHTLJS/CHTLJSParser.hpp
    ../../include/CHTLJS/CHTLJSGenerator.hpp
    ../../include/CHTLJS/CHTLJSCompiler.hpp
)

add_library(CHTLJS_Compiler STATIC ${CHTLJS_SOURCES} ${CHTLJS_HEADERS})

target_include_directories(CHTLJS_Compiler
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../include>
        $<INSTALL_INTERFACE:include>
)

target_link_libraries(CHTLJS_Compiler
    PUBLIC
        CHTL_Core
)

target_compile_features(CHTLJS_Compiler PUBLIC cxx_std_17)

# 设置编译选项
if(MSVC)
    target_compile_options(CHTLJS_Compiler PRIVATE /W4 /utf-8)
else()
    target_compile_options(CHTLJS_Compiler PRIVATE -Wall -Wextra -pedantic)
endif()