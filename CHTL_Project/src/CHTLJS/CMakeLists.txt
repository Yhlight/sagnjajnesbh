set(CHTLJS_SOURCES
    CHTLJSToken.cpp
    # CHTLJSGlobalMap.cpp   # 暂时注释
    # CHTLJSState.cpp       # 暂时注释
    # CHTLJSContext.cpp     # 暂时注释
    # CHTLJSLexer.cpp       # 暂时注释
    # CHTLJSParser.cpp      # 暂时注释
    # CHTLJSGenerator.cpp   # 暂时注释
    # CHTLJSCompiler.cpp    # 暂时注释
)

set(CHTLJS_HEADERS
    ../../include/CHTLJS/CHTLJSToken.hpp
    # ../../include/CHTLJS/CHTLJSGlobalMap.hpp   # 暂时注释
    # ../../include/CHTLJS/CHTLJSState.hpp       # 暂时注释
    # ../../include/CHTLJS/CHTLJSContext.hpp     # 暂时注释
    # ../../include/CHTLJS/CHTLJSLexer.hpp       # 暂时注释
    # ../../include/CHTLJS/CHTLJSParser.hpp      # 暂时注释
    # ../../include/CHTLJS/CHTLJSGenerator.hpp   # 暂时注释
    # ../../include/CHTLJS/CHTLJSCompiler.hpp    # 暂时注释
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