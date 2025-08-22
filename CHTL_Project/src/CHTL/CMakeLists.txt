set(CHTL_SOURCES
    CHTLToken.cpp
    CHTLGlobalMap.cpp
    CHTLState.cpp
    CHTLContext.cpp
    CHTLLexer.cpp
    # CHTLParser.cpp      # 暂时注释
    # CHTLGenerator.cpp   # 暂时注释
    # CHTLCompiler.cpp    # 暂时注释
)

set(CHTL_HEADERS
    ../../include/CHTL/CHTLToken.hpp
    ../../include/CHTL/CHTLGlobalMap.hpp
    ../../include/CHTL/CHTLState.hpp
    ../../include/CHTL/CHTLContext.hpp
    ../../include/CHTL/CHTLLexer.hpp
    # ../../include/CHTL/CHTLParser.hpp      # 暂时注释
    # ../../include/CHTL/CHTLGenerator.hpp   # 暂时注释
    # ../../include/CHTL/CHTLCompiler.hpp    # 暂时注释
)

add_library(CHTL_Compiler STATIC ${CHTL_SOURCES} ${CHTL_HEADERS})

target_include_directories(CHTL_Compiler
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../include>
        $<INSTALL_INTERFACE:include>
)

target_link_libraries(CHTL_Compiler
    PUBLIC
        CHTL_Core
)

target_compile_features(CHTL_Compiler PUBLIC cxx_std_17)

# 设置编译选项
if(MSVC)
    target_compile_options(CHTL_Compiler PRIVATE /W4 /utf-8)
else()
    target_compile_options(CHTL_Compiler PRIVATE -Wall -Wextra -pedantic)
endif()