cmake_minimum_required(VERSION 3.16)
project(CHTL VERSION 1.0.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Compiler flags
if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W3 /permissive-")
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wno-unused-parameter -Wno-unused-variable -Wno-tautological-constant-out-of-range-compare")
endif()

# Include directories
include_directories(src)

# ============================================================================
# Minimal Working Build (Only verified compiling core files)
# ============================================================================

# Core common library (absolutely essential files only)
add_library(chtl_common STATIC
    src/Common/Core/ErrorHandler.cpp
    src/Common/Core/NamespaceAlgorithms.cpp
    src/Common/Utils/FileUtils.cpp
    src/Common/ModulePathManager.cpp
    src/Common/SimpleZip/SimpleZip.cpp
    src/Common/AST/CommentGenerator.cpp
    src/Common/AST/CommentNode.cpp
    src/Common/AST/OriginEmbedDetector.cpp
)

# Scanner (core functionality)
add_library(chtl_scanner STATIC
    src/Scanner/CHTLUnifiedScanner.cpp
)
target_link_libraries(chtl_scanner chtl_common)

# Main executable (minimal version)
add_executable(chtl_main src/main.cpp)
target_link_libraries(chtl_main chtl_scanner chtl_common)
set_target_properties(chtl_main PROPERTIES OUTPUT_NAME "chtl_compiler")

# Alternative executable name
add_executable(chtl_alt src/main.cpp)
target_link_libraries(chtl_alt chtl_scanner chtl_common)
set_target_properties(chtl_alt PROPERTIES OUTPUT_NAME "chtl")

# ============================================================================
# Output Directories
# ============================================================================

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# For Visual Studio
foreach(CONFIG ${CMAKE_CONFIGURATION_TYPES})
    string(TOUPPER ${CONFIG} CONFIG_UPPER)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${CONFIG_UPPER} ${CMAKE_BINARY_DIR}/bin/${CONFIG})
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${CONFIG_UPPER} ${CMAKE_BINARY_DIR}/lib/${CONFIG})
endforeach()

# ============================================================================
# Installation
# ============================================================================

install(TARGETS chtl_main chtl_alt RUNTIME DESTINATION bin)
install(TARGETS chtl_common chtl_scanner ARCHIVE DESTINATION lib)

# ============================================================================
# Summary
# ============================================================================

message(STATUS "")
message(STATUS "CHTL Minimal Working Build")
message(STATUS "===========================")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "Compiler: ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}")
message(STATUS "")
message(STATUS "This is an ultra-minimal build for development")
message(STATUS "Only includes core files that compile successfully")
message(STATUS "Perfect for VSCode + CMake workflow")
message(STATUS "")
message(STATUS "Build targets:")
message(STATUS "  - chtl_main -> chtl_compiler")
message(STATUS "  - chtl_alt -> chtl")
message(STATUS "  - Libraries: chtl_common, chtl_scanner")
message(STATUS "")
message(STATUS "To build: cmake --build . --parallel")
message(STATUS "")