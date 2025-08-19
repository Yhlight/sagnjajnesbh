# CMake generated Testfile for 
# Source directory: /workspace/chtl-compiler/tests
# Build directory: /workspace/chtl-compiler/build/tests
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test(scanner_test "/workspace/chtl-compiler/build/tests/test_scanner")
set_tests_properties(scanner_test PROPERTIES  _BACKTRACE_TRIPLES "/workspace/chtl-compiler/tests/CMakeLists.txt;25;add_test;/workspace/chtl-compiler/tests/CMakeLists.txt;0;")
add_test(scanner_v2_test "/workspace/chtl-compiler/build/tests/test_scanner_v2")
set_tests_properties(scanner_v2_test PROPERTIES  _BACKTRACE_TRIPLES "/workspace/chtl-compiler/tests/CMakeLists.txt;26;add_test;/workspace/chtl-compiler/tests/CMakeLists.txt;0;")
add_test(chtl_compiler_test "/workspace/chtl-compiler/build/tests/test_chtl_compiler")
set_tests_properties(chtl_compiler_test PROPERTIES  _BACKTRACE_TRIPLES "/workspace/chtl-compiler/tests/CMakeLists.txt;27;add_test;/workspace/chtl-compiler/tests/CMakeLists.txt;0;")
add_test(integration_test "/workspace/chtl-compiler/build/tests/test_integration")
set_tests_properties(integration_test PROPERTIES  _BACKTRACE_TRIPLES "/workspace/chtl-compiler/tests/CMakeLists.txt;28;add_test;/workspace/chtl-compiler/tests/CMakeLists.txt;0;")
