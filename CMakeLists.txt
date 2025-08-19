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

# Find ANTLR4 (optional)
find_package(ANTLR4 QUIET)
if(ANTLR4_FOUND)
    message(STATUS "Found ANTLR4: ${ANTLR4_LIBRARIES}")
    add_definitions(-DCHTL_ENABLE_ANTLR)
else()
    message(WARNING "ANTLR4 not found - CSS/JS parsing will be limited")
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

# CHTL Compiler Core Library (renamed to avoid conflict)
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

# CSS Compiler Library (with ANTLR support)
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

# JavaScript Compiler Library (with ANTLR support)
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
# Tests
# ============================================================================

# Test Framework
add_library(chtl_test_framework STATIC
    src/Test/Framework/TestBase.cpp
    src/Test/Framework/TestRunner.cpp
)
target_link_libraries(chtl_test_framework chtl_common)

# Test Main Runner
add_executable(chtl_test_main src/Test/TestMain.cpp)
target_link_libraries(chtl_test_main chtl_test_framework chtl_dispatcher)

# Comprehensive Test Runner
add_executable(test_comprehensive src/Test/ComprehensiveTestRunner.cpp)
target_link_libraries(test_comprehensive chtl_test_framework chtl_dispatcher)

# Unit Tests
add_executable(test_namespace_algorithms src/Test/Unit/NamespaceAlgorithmsTest.cpp)
target_link_libraries(test_namespace_algorithms chtl_test_framework chtl_dispatcher)

add_executable(test_configuration_system src/Test/Unit/ConfigurationSystemTest.cpp)
target_link_libraries(test_configuration_system chtl_test_framework chtl_dispatcher)

add_executable(test_chtl_syntax src/Test/Unit/CHTLSyntaxComprehensiveTest.cpp)
target_link_libraries(test_chtl_syntax chtl_test_framework chtl_dispatcher)

add_executable(test_chtljs_advanced src/Test/Unit/CHTLJSAdvancedTest.cpp)
target_link_libraries(test_chtljs_advanced chtl_test_framework chtl_dispatcher)

# Integration Tests  
add_executable(test_module_system src/Test/Integration/ModuleSystemTest.cpp)
target_link_libraries(test_module_system chtl_test_framework chtl_dispatcher)

# Performance Tests
add_executable(test_performance src/Test/Performance/CompilerPerformanceTest.cpp)
target_link_libraries(test_performance chtl_test_framework chtl_dispatcher)

# Functional Tests
add_executable(test_syntax_implementation src/Test/Functional/ActualSyntaxImplementationTest.cpp)
target_link_libraries(test_syntax_implementation chtl_test_framework chtl_dispatcher)

# Compliance Tests
add_executable(test_syntax_compliance src/Test/Comprehensive/StrictSyntaxComplianceTest.cpp)
target_link_libraries(test_syntax_compliance chtl_test_framework chtl_dispatcher)

# Quick Tests
add_executable(test_quick_syntax src/Test/Simple/QuickSyntaxTest.cpp)
target_link_libraries(test_quick_syntax chtl_test_framework chtl_dispatcher)

# Enable testing
enable_testing()

# Add tests to CTest
add_test(NAME comprehensive_test COMMAND test_comprehensive)
add_test(NAME namespace_test COMMAND test_namespace_algorithms)
add_test(NAME configuration_test COMMAND test_configuration_system)
add_test(NAME chtl_syntax_test COMMAND test_chtl_syntax)
add_test(NAME chtljs_test COMMAND test_chtljs_advanced)
add_test(NAME module_test COMMAND test_module_system)
add_test(NAME performance_test COMMAND test_performance)
add_test(NAME syntax_implementation_test COMMAND test_syntax_implementation)
add_test(NAME compliance_test COMMAND test_syntax_compliance)
add_test(NAME quick_test COMMAND test_quick_syntax)

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
    set(CPACK_GENERATOR "NSIS;ZIP")
    set(CPACK_NSIS_DISPLAY_NAME "CHTL Compiler")
    set(CPACK_NSIS_PACKAGE_NAME "CHTL")
    set(CPACK_NSIS_CONTACT "dev@chtl-lang.org")
    set(CPACK_NSIS_URL_INFO_ABOUT "https://chtl-lang.org")
elseif(APPLE)
    set(CPACK_GENERATOR "DragNDrop;TGZ")
else()
    set(CPACK_GENERATOR "DEB;RPM;TGZ")
    set(CPACK_DEBIAN_PACKAGE_MAINTAINER "CHTL Development Team")
    set(CPACK_RPM_PACKAGE_GROUP "Development/Tools")
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
message(STATUS "Install prefix: ${CMAKE_INSTALL_PREFIX}")
message(STATUS "ANTLR4 support: ${ANTLR4_FOUND}")
message(STATUS "")
message(STATUS "Build targets:")
message(STATUS "  - chtl_main -> chtl_compiler (main executable)")
message(STATUS "  - chtl_alt -> chtl (alternative executable)")
message(STATUS "  - Libraries: chtl_common, chtl_scanner, chtl_compiler_lib, chtljs_compiler")
if(ANTLR4_FOUND)
    message(STATUS "  - ANTLR libraries: css_compiler, js_compiler")
endif()
message(STATUS "  - Test framework: chtl_test_framework")
message(STATUS "")
message(STATUS "To build: cmake --build . --parallel")
message(STATUS "To install: cmake --install .")
message(STATUS "To test: ctest")
message(STATUS "To package: cpack")
message(STATUS "")