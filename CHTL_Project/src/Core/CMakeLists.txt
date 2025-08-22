set(CORE_SOURCES
    CHTLTypes.cpp
    CHTLUnifiedScanner.cpp
    CompilerDispatcher.cpp
)

set(CORE_HEADERS
    ../../include/Core/CHTLTypes.hpp
    ../../include/Core/CHTLUnifiedScanner.hpp
    ../../include/Core/CompilerDispatcher.hpp
)

add_library(CHTL_Core STATIC ${CORE_SOURCES} ${CORE_HEADERS})

target_include_directories(CHTL_Core
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../include>
        $<INSTALL_INTERFACE:include>
)

target_compile_features(CHTL_Core PUBLIC cxx_std_17)

# 设置编译选项
if(MSVC)
    target_compile_options(CHTL_Core PRIVATE /W4 /utf-8)
else()
    target_compile_options(CHTL_Core PRIVATE -Wall -Wextra -pedantic)
endif()