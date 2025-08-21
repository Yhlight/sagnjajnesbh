# Scanner库
add_library(CHTLScanner STATIC
    UnifiedScanner.cpp
)

target_include_directories(CHTLScanner
    PUBLIC ${CMAKE_SOURCE_DIR}/include
)

target_link_libraries(CHTLScanner
    PUBLIC CHTLUtils
)