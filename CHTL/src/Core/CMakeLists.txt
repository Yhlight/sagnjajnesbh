# Core库
add_library(CHTLCore STATIC
    # 暂时创建一个空文件
    dummy.cpp
)

target_include_directories(CHTLCore
    PUBLIC ${CMAKE_SOURCE_DIR}/include
)

# 创建dummy.cpp
file(WRITE ${CMAKE_CURRENT_SOURCE_DIR}/dummy.cpp "// Placeholder for CHTLCore\n")