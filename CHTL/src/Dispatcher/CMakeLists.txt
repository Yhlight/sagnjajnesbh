# Dispatcher库
add_library(CompilerDispatcher STATIC
    CompilerDispatcher.cpp
)

target_include_directories(CompilerDispatcher
    PUBLIC ${CMAKE_SOURCE_DIR}/include
)

target_link_libraries(CompilerDispatcher
    PUBLIC 
        CHTLScanner
        CHTLUtils
)