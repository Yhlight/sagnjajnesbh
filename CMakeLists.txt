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
# Step 1: Build Core Common Library (absolutely essential)
# ============================================================================

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

# ============================================================================
# Step 2: Build Scanner (depends on common)
# ============================================================================

add_library(chtl_scanner STATIC
    src/Scanner/CHTLUnifiedScanner.cpp
)
target_link_libraries(chtl_scanner chtl_common)

# ============================================================================
# Step 3: Add Core Managers (needed by tests)
# ============================================================================

add_library(chtl_managers STATIC
    src/CHTLCompiler/Template/TemplateManager.cpp
    src/CHTLCompiler/Custom/CustomManager.cpp
    src/CHTLCompiler/Origin/OriginManager.cpp
    src/CHTLCompiler/Constraint/ConstraintManager.cpp
)
target_link_libraries(chtl_managers chtl_common)

# ============================================================================
# Step 4: Add CHTL Compiler Core (basic components)
# ============================================================================

add_library(chtl_compiler_basic STATIC
    src/CHTLCompiler/CHTLCompilerCore.cpp
    src/CHTLCompiler/AST/CHTLNodes.cpp
    src/CHTLCompiler/Lexer/CHTLLexer.cpp
    src/CHTLCompiler/Lexer/CHTLToken.cpp
    src/CHTLCompiler/Parser/CHTLContext.cpp
    src/CHTLCompiler/Parser/CHTLParser.cpp
    src/CHTLCompiler/State/CHTLGlobalMap.cpp
    src/CHTLCompiler/State/CHTLState.cpp
    src/CHTLCompiler/Generator/CHTLGenerator.cpp
    src/CHTLCompiler/Import/ImportManager.cpp
    src/CHTLCompiler/Module/CmodManager.cpp
)
target_link_libraries(chtl_compiler_basic chtl_managers chtl_scanner chtl_common)

# ============================================================================
# Step 5: Test with basic compiler
# ============================================================================

# Main CHTL Compiler executable (simplified version)
add_executable(chtl_main src/main_simple.cpp)
target_link_libraries(chtl_main chtl_managers chtl_scanner chtl_common)
set_target_properties(chtl_main PROPERTIES OUTPUT_NAME "chtl_compiler")

# Test executable
add_executable(test_minimal
    src/Test/Simple/QuickSyntaxTest.cpp
)
target_link_libraries(test_minimal chtl_compiler_basic chtl_managers chtl_scanner chtl_common)

# ============================================================================
# Output Directories
# ============================================================================

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

foreach(CONFIG ${CMAKE_CONFIGURATION_TYPES})
    string(TOUPPER ${CONFIG} CONFIG_UPPER)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${CONFIG_UPPER} ${CMAKE_BINARY_DIR}/bin/${CONFIG})
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${CONFIG_UPPER} ${CMAKE_BINARY_DIR}/lib/${CONFIG})
endforeach()

# ============================================================================
# Summary
# ============================================================================

message(STATUS "")
message(STATUS "CHTL Working Build Configuration")
message(STATUS "=================================")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "Compiler: ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}")
if(ANTLR4_FOUND)
    message(STATUS "ANTLR4: Found (${ANTLR4_ROOT})")
else()
    message(STATUS "ANTLR4: Not found")
endif()
message(STATUS "")
message(STATUS "Build targets:")
message(STATUS "  - chtl_main -> chtl_compiler (main executable)")
message(STATUS "  - Libraries: chtl_common, chtl_scanner, chtl_managers")
message(STATUS "  - test_minimal (functionality test)")
message(STATUS "")
message(STATUS "This is a working build with core CHTL functionality")
message(STATUS "Some advanced features are temporarily disabled due to code issues")
message(STATUS "")
message(STATUS "To build: cmake --build . --parallel")
message(STATUS "To test: ./chtl_compiler")
message(STATUS "")