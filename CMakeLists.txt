cmake_minimum_required(VERSION 3.16)
project(CHTL VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W3 /permissive-")
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wno-unused-parameter -Wno-unused-variable -Wno-tautological-constant-out-of-range-compare")
endif()

include_directories(src)

# ============================================================================
# Step 1: Core Libraries (Verified Working)
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

add_library(chtl_scanner STATIC
    src/Scanner/CHTLUnifiedScanner.cpp
)
target_link_libraries(chtl_scanner chtl_common)

add_library(chtl_managers STATIC
    src/CHTLCompiler/Template/TemplateManager.cpp
    src/CHTLCompiler/Custom/CustomManager.cpp
    src/CHTLCompiler/Origin/OriginManager.cpp
    src/CHTLCompiler/Constraint/ConstraintManager.cpp
)
target_link_libraries(chtl_managers chtl_common)

# ============================================================================
# Step 2: Core Compiler Components (Verified Working)
# ============================================================================

add_library(chtl_lexer STATIC
    src/CHTLCompiler/Lexer/CHTLLexer.cpp
    src/CHTLCompiler/Lexer/CHTLToken.cpp
)
target_link_libraries(chtl_lexer chtl_common)

add_library(chtl_ast STATIC
    src/CHTLCompiler/AST/CHTLNodes.cpp
)
target_link_libraries(chtl_ast chtl_common)

add_library(chtl_generator STATIC
    src/CHTLCompiler/Generator/CHTLGenerator.cpp
)
target_link_libraries(chtl_generator chtl_common chtl_ast)

# ============================================================================
# Step 3: Fixed Parsers (CustomParser ✅, OriginParser ✅)
# ============================================================================

add_library(chtl_parsers_fixed STATIC
    src/CHTLCompiler/Custom/CustomParser.cpp
    src/CHTLCompiler/Origin/OriginParser.cpp
    src/CHTLCompiler/Template/TemplateParser.cpp
)
target_link_libraries(chtl_parsers_fixed chtl_common chtl_ast)

# ============================================================================
# Step 4: Working Executables
# ============================================================================

# Main CHTL Compiler (with fixed parsers)
add_executable(chtl_main src/main_working.cpp)
target_link_libraries(chtl_main 
    chtl_parsers_fixed
    chtl_generator
    chtl_lexer
    chtl_managers
    chtl_scanner
    chtl_common
    chtl_ast
)
set_target_properties(chtl_main PROPERTIES OUTPUT_NAME "chtl_compiler")

# Test executable
add_executable(test_minimal src/Test/Simple/QuickSyntaxTest.cpp)
target_link_libraries(test_minimal chtl_managers chtl_scanner chtl_common)

# Test fixed parsers
add_executable(test_parsers_fixed test/test_fixed_parsers.cpp)
target_link_libraries(test_parsers_fixed 
    chtl_parsers_fixed
    chtl_lexer
    chtl_managers
    chtl_scanner
    chtl_common
    chtl_ast
)

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
message(STATUS "CHTL Progressive Build - Fixed Parsers")
message(STATUS "======================================")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "Compiler: ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}")
message(STATUS "")
message(STATUS "✅ FIXED Components:")
message(STATUS "  - CustomParser: Method signatures corrected")
message(STATUS "  - OriginParser: Method signatures corrected")
message(STATUS "  - UTF-8 Support: Added to CHTLLexer")
message(STATUS "  - AST Nodes: Added missing members")
message(STATUS "")
message(STATUS "⏳ Pending Components:")
message(STATUS "  - ConstraintParser: Needs method signature fixes")
message(STATUS "  - TemplateParser: Needs verification")
message(STATUS "  - CHTL JS Compiler: Needs namespace fixes")
message(STATUS "")
message(STATUS "Build targets:")
message(STATUS "  - chtl_main -> chtl_compiler (with fixed parsers)")
message(STATUS "  - test_minimal (manager functionality)")
message(STATUS "  - test_parsers_fixed (parser functionality)")
message(STATUS "")
message(STATUS "To build: cmake --build . --parallel")
message(STATUS "To test: ./chtl_compiler --test")
message(STATUS "")