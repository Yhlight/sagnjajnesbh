cmake_minimum_required(VERSION 3.16)
project(CHTL VERSION 1.0.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Compiler flags
if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W3 /permissive-")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /Od /Zi")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /O2 /DNDEBUG")
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wno-unused-parameter -Wno-unused-variable")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -O0")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -DNDEBUG")
endif()

# Include directories
include_directories(src)

# Use pre-built ANTLR4 from external/antlr4-cross-platform
set(ANTLR4_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/external/antlr4-cross-platform")

# Detect platform
if(WIN32)
    set(ANTLR4_PLATFORM "windows-x64")
elseif(APPLE)
    set(ANTLR4_PLATFORM "macos-x64")
else()
    set(ANTLR4_PLATFORM "linux-x64")
endif()

set(ANTLR4_PRECOMPILED_DIR "${ANTLR4_ROOT}/precompiled/${ANTLR4_PLATFORM}")

# Check for pre-built ANTLR4
if(EXISTS "${ANTLR4_PRECOMPILED_DIR}")
    message(STATUS "Found pre-built ANTLR4: ${ANTLR4_PRECOMPILED_DIR}")
    
    # Set ANTLR4 variables
    set(ANTLR4_INCLUDE_DIR "${ANTLR4_PRECOMPILED_DIR}/include")
    set(ANTLR4_LIBRARY_DIR "${ANTLR4_PRECOMPILED_DIR}/lib")
    
    # Find ANTLR4 library
    find_library(ANTLR4_LIBRARY
        NAMES antlr4-runtime libantlr4-runtime
        PATHS ${ANTLR4_LIBRARY_DIR}
        NO_DEFAULT_PATH
    )
    
    if(ANTLR4_LIBRARY AND EXISTS "${ANTLR4_INCLUDE_DIR}")
        set(ANTLR4_FOUND TRUE)
        add_definitions(-DCHTL_ENABLE_ANTLR)
        
        # Create imported target
        add_library(antlr4-runtime UNKNOWN IMPORTED)
        set_target_properties(antlr4-runtime PROPERTIES
            IMPORTED_LOCATION ${ANTLR4_LIBRARY}
            INTERFACE_INCLUDE_DIRECTORIES ${ANTLR4_INCLUDE_DIR}
        )
        
        message(STATUS "ANTLR4 library: ${ANTLR4_LIBRARY}")
    else()
        message(WARNING "Pre-built ANTLR4 library not found in ${ANTLR4_PRECOMPILED_DIR}")
        set(ANTLR4_FOUND FALSE)
    endif()
else()
    message(WARNING "Pre-built ANTLR4 not found for platform ${ANTLR4_PLATFORM}")
    set(ANTLR4_FOUND FALSE)
endif()

# ============================================================================
# Core Libraries
# ============================================================================

# Common/Shared Components Library
add_library(chtl_common STATIC
    src/Common/Core/ErrorHandler.cpp
    src/Common/Core/NamespaceAlgorithms.cpp
    src/Common/Utils/FileUtils.cpp
    src/Common/ModulePathManager.cpp
    src/Common/SimpleZip/SimpleZip.cpp
    src/Common/RAIIStateGuard.cpp
    src/Common/AST/CommentGenerator.cpp
    src/Common/AST/CommentNode.cpp
    src/Common/AST/OriginEmbedDetector.cpp
)

# Unified Scanner Library
add_library(chtl_scanner STATIC
    src/Scanner/CHTLUnifiedScanner.cpp
)
target_link_libraries(chtl_scanner chtl_common)

# CHTL Compiler Core Library
add_library(chtl_compiler_lib STATIC
    src/CHTLCompiler/CHTLCompilerCore.cpp
    src/CHTLCompiler/AST/CHTLNodes.cpp
    src/CHTLCompiler/Configuration/ConfigurationManager.cpp
    src/CHTLCompiler/Constraint/ConstraintManager.cpp
    src/CHTLCompiler/Constraint/ConstraintParser.cpp
    src/CHTLCompiler/Custom/CustomManager.cpp
    src/CHTLCompiler/Custom/CustomParser.cpp
    src/CHTLCompiler/Generator/CHTLGenerator.cpp
    src/CHTLCompiler/Import/ImportManager.cpp
    src/CHTLCompiler/Lexer/CHTLLexer.cpp
    src/CHTLCompiler/Lexer/CHTLToken.cpp
    src/CHTLCompiler/Module/CmodManager.cpp
    src/CHTLCompiler/Namespace/NamespaceManager.cpp
    src/CHTLCompiler/Origin/OriginManager.cpp
    src/CHTLCompiler/Origin/OriginParser.cpp
    src/CHTLCompiler/Parser/CHTLContext.cpp
    src/CHTLCompiler/Parser/CHTLParser.cpp
    src/CHTLCompiler/State/CHTLGlobalMap.cpp
    src/CHTLCompiler/State/CHTLState.cpp
    src/CHTLCompiler/Template/TemplateManager.cpp
    src/CHTLCompiler/Template/TemplateParser.cpp
)
target_link_libraries(chtl_compiler_lib chtl_common chtl_scanner)

# CHTL JS Compiler Library
add_library(chtljs_compiler STATIC
    src/CHTLJSCompiler/CHTLJSCompiler.cpp
    src/CHTLJSCompiler/Animation/AnimateSystemProcessor.cpp
    src/CHTLJSCompiler/AST/VirtualObjectManager.cpp
    src/CHTLJSCompiler/Core/AutoAdditionManager.cpp
    src/CHTLJSCompiler/Core/JSEventRegistry.cpp
    src/CHTLJSCompiler/Enhanced/UnorderedOptionalProcessor.cpp
    src/CHTLJSCompiler/Event/AllJSEventsSupport.cpp
    src/CHTLJSCompiler/Event/DelegateSystemProcessor.cpp
    src/CHTLJSCompiler/Event/ListenSystemProcessor.cpp
    src/CHTLJSCompiler/Lexer/CHTLJSLexer.cpp
    src/CHTLJSCompiler/Lexer/CHTLJSToken.cpp
    src/CHTLJSCompiler/Operator/ArrowOperatorProcessor.cpp
    src/CHTLJSCompiler/Parser/CHTLJSFunctionParser.cpp
    src/CHTLJSCompiler/Parser/SelectorDetector.cpp
    src/CHTLJSCompiler/Script/LocalScriptProcessor.cpp
    src/CHTLJSCompiler/Selector/EnhancedSelectorProcessor.cpp
    src/CHTLJSCompiler/Virtual/VirtualObjectProcessor.cpp
)
target_link_libraries(chtljs_compiler chtl_common chtl_scanner)

# CSS Compiler Library (with pre-built ANTLR support)
add_library(css_compiler STATIC
    src/CSSCompiler/CSSCompilerCore.cpp
    src/CSSCompiler/Parser/css3Lexer.cpp
    src/CSSCompiler/Parser/css3Parser.cpp
    src/CSSCompiler/Parser/css3ParserBaseListener.cpp
    src/CSSCompiler/Parser/css3ParserListener.cpp
)
target_link_libraries(css_compiler chtl_common)
if(ANTLR4_FOUND)
    target_link_libraries(css_compiler antlr4-runtime)
endif()

# JavaScript Compiler Library (with pre-built ANTLR support)
add_library(js_compiler STATIC
    src/JavaScriptCompiler/JavaScriptCompilerCore.cpp
    src/JavaScriptCompiler/Parser/JavaScriptLexerBase.cpp
    src/JavaScriptCompiler/Parser/JavaScriptLexer_cpp.cpp
    src/JavaScriptCompiler/Parser/JavaScriptParserBase.cpp
    src/JavaScriptCompiler/Parser/JavaScriptParser_cpp.cpp
    src/JavaScriptCompiler/Parser/JavaScriptParser_cppBaseListener.cpp
    src/JavaScriptCompiler/Parser/JavaScriptParser_cppListener.cpp
)
target_link_libraries(js_compiler chtl_common)
if(ANTLR4_FOUND)
    target_link_libraries(js_compiler antlr4-runtime)
endif()

# Compiler Dispatcher Library
add_library(chtl_dispatcher STATIC
    src/Dispatcher/CompilerDispatcher.cpp
)
target_link_libraries(chtl_dispatcher 
    chtl_compiler_lib
    chtljs_compiler 
    css_compiler 
    js_compiler 
    chtl_scanner 
    chtl_common
)

# ============================================================================
# Executables
# ============================================================================

# Main CHTL Compiler Executable
add_executable(chtl_main src/main.cpp)
target_link_libraries(chtl_main chtl_dispatcher)
set_target_properties(chtl_main PROPERTIES OUTPUT_NAME "chtl_compiler")

# Alternative executable
add_executable(chtl_alt src/main.cpp)
target_link_libraries(chtl_alt chtl_dispatcher)
set_target_properties(chtl_alt PROPERTIES OUTPUT_NAME "chtl")

# ============================================================================
# Tests (Optional - only if Test directory exists)
# ============================================================================

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/src/Test")
    # Test Framework
    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/src/Test/Framework/TestBase.cpp")
        add_library(chtl_test_framework STATIC
            src/Test/Framework/TestBase.cpp
            src/Test/Framework/TestRunner.cpp
        )
        target_link_libraries(chtl_test_framework chtl_common)
        
        # Test Main Runner
        if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/src/Test/TestMain.cpp")
            add_executable(chtl_test_main src/Test/TestMain.cpp)
            target_link_libraries(chtl_test_main chtl_test_framework chtl_dispatcher)
        endif()
        
        # Find and add other test files
        file(GLOB_RECURSE TEST_FILES "src/Test/*.cpp")
        foreach(TEST_FILE ${TEST_FILES})
            get_filename_component(TEST_NAME ${TEST_FILE} NAME_WE)
            if(NOT ${TEST_NAME} MATCHES "TestMain|TestBase|TestRunner")
                add_executable(${TEST_NAME} ${TEST_FILE})
                target_link_libraries(${TEST_NAME} chtl_test_framework chtl_dispatcher)
            endif()
        endforeach()
        
        # Enable testing
        enable_testing()
        
        # Add basic test
        if(TARGET chtl_test_main)
            add_test(NAME main_test COMMAND chtl_test_main)
        endif()
    endif()
endif()

# ============================================================================
# Output Directories
# ============================================================================

# Set output directories
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# For multi-config generators (Visual Studio)
foreach(CONFIG ${CMAKE_CONFIGURATION_TYPES})
    string(TOUPPER ${CONFIG} CONFIG_UPPER)
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${CONFIG_UPPER} ${CMAKE_BINARY_DIR}/lib/${CONFIG})
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_${CONFIG_UPPER} ${CMAKE_BINARY_DIR}/lib/${CONFIG})
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${CONFIG_UPPER} ${CMAKE_BINARY_DIR}/bin/${CONFIG})
endforeach()

# ============================================================================
# Installation
# ============================================================================

# Install executables
install(TARGETS chtl_main chtl_alt
    RUNTIME DESTINATION bin
)

# Install libraries
install(TARGETS 
    chtl_common 
    chtl_scanner 
    chtl_compiler_lib
    chtljs_compiler 
    css_compiler 
    js_compiler 
    chtl_dispatcher
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
)

# Install headers
install(DIRECTORY src/
    DESTINATION include/chtl
    FILES_MATCHING PATTERN "*.h"
    PATTERN "Test" EXCLUDE
    PATTERN "Module" EXCLUDE
)

# Install documentation
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/docs")
    install(DIRECTORY docs/
        DESTINATION share/doc/chtl
    )
endif()

# Install modules
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/module")
    install(DIRECTORY module/
        DESTINATION share/chtl/module
    )
endif()

# ============================================================================
# Package Configuration
# ============================================================================

# CPack configuration for creating packages
set(CPACK_PACKAGE_NAME "CHTL")
set(CPACK_PACKAGE_VENDOR "CHTL Development Team")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "CHTL - Chtholly Template Language Compiler")
set(CPACK_PACKAGE_VERSION_MAJOR 1)
set(CPACK_PACKAGE_VERSION_MINOR 0)
set(CPACK_PACKAGE_VERSION_PATCH 0)
set(CPACK_PACKAGE_INSTALL_DIRECTORY "CHTL")

# Platform-specific package settings
if(WIN32)
    set(CPACK_GENERATOR "ZIP")
    set(CPACK_PACKAGE_FILE_NAME "chtl-${CMAKE_PROJECT_VERSION}-windows-x64")
elseif(APPLE)
    set(CPACK_GENERATOR "TGZ")
    set(CPACK_PACKAGE_FILE_NAME "chtl-${CMAKE_PROJECT_VERSION}-macos-x64")
else()
    set(CPACK_GENERATOR "TGZ")
    set(CPACK_PACKAGE_FILE_NAME "chtl-${CMAKE_PROJECT_VERSION}-linux-x64")
endif()

include(CPack)

# ============================================================================
# Development Tools
# ============================================================================

# Add custom targets for development
add_custom_target(clean-all
    COMMAND ${CMAKE_BUILD_TOOL} clean
    COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_BINARY_DIR}/CMakeFiles
    COMMENT "Cleaning all build artifacts"
)

if(NOT WIN32)
    add_custom_target(format
        COMMAND find src -name "*.cpp" -o -name "*.h" | xargs clang-format -i
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        COMMENT "Formatting source code"
    )
endif()

# ============================================================================
# Summary
# ============================================================================

message(STATUS "")
message(STATUS "CHTL Build Configuration Summary")
message(STATUS "=================================")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "C++ standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "Compiler: ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}")
message(STATUS "Platform: ${ANTLR4_PLATFORM}")
message(STATUS "Install prefix: ${CMAKE_INSTALL_PREFIX}")
message(STATUS "Pre-built ANTLR4: ${ANTLR4_FOUND}")
if(ANTLR4_FOUND)
    message(STATUS "ANTLR4 location: ${ANTLR4_PRECOMPILED_DIR}")
endif()
message(STATUS "")
message(STATUS "Build targets:")
message(STATUS "  - chtl_main -> chtl_compiler (main executable)")
message(STATUS "  - chtl_alt -> chtl (alternative executable)")
message(STATUS "  - Libraries: chtl_common, chtl_scanner, chtl_compiler_lib, chtljs_compiler")
message(STATUS "  - ANTLR libraries: css_compiler, js_compiler")
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/src/Test")
    message(STATUS "  - Test framework: chtl_test_framework")
endif()
message(STATUS "")
message(STATUS "To build: cmake --build . --parallel")
message(STATUS "To install: cmake --install .")
message(STATUS "To test: ctest")
message(STATUS "To package: cpack")
message(STATUS "")