# CHTL编译器库
add_library(CHTLCompiler STATIC
    Token.cpp
    GlobalMap.cpp
    State.cpp
    Context.cpp
    Lexer.cpp
)

target_include_directories(CHTLCompiler
    PUBLIC ${CMAKE_SOURCE_DIR}/include
)

target_link_libraries(CHTLCompiler
    PUBLIC CHTLUtils
)