cmake_minimum_required(VERSION 3.14)
project(chtl_example)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set CHTL root directory
set(CHTL_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/..)

# Include directories
include_directories(
    ${CHTL_ROOT}/src
    ${CHTL_ROOT}/third_party/antlr4-runtime/runtime/src
)

# Platform-specific settings
if(WIN32)
    # Windows settings
    add_compile_options(/EHsc)
    
    # Use different runtime for Debug/Release
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MDd")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MD")
    
    # Library directories
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        link_directories(${CHTL_ROOT}/prebuilt/windows/x64/Debug)
        set(CHTL_LIBS chtl_antlr_all_d.lib)
    else()
        link_directories(${CHTL_ROOT}/prebuilt/windows/x64/Release)
        set(CHTL_LIBS chtl_antlr_all.lib)
    endif()
else()
    # Linux/macOS settings
    link_directories(${CHTL_ROOT}/prebuilt/linux/x64)
    set(CHTL_LIBS chtl_antlr_all pthread)
endif()

# Example executable
add_executable(example_app main.cpp)
target_link_libraries(example_app ${CHTL_LIBS})

# Copy example files
configure_file(example.css ${CMAKE_CURRENT_BINARY_DIR}/example.css COPYONLY)
configure_file(example.js ${CMAKE_CURRENT_BINARY_DIR}/example.js COPYONLY)