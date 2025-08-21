# Utils库
add_library(CHTLUtils STATIC
    Logger.cpp
    FileUtils.cpp
)

target_include_directories(CHTLUtils
    PUBLIC ${CMAKE_SOURCE_DIR}/include
)

# Windows特定设置
if(WIN32)
    target_compile_definitions(CHTLUtils PRIVATE _SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING)
endif()