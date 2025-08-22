# CMake generated Testfile for 
# Source directory: /workspace/tests
# Build directory: /workspace/build/tests
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test(CHTLCoreTest "/workspace/build/bin/test_chtl_core")
set_tests_properties(CHTLCoreTest PROPERTIES  _BACKTRACE_TRIPLES "/workspace/tests/CMakeLists.txt;18;add_test;/workspace/tests/CMakeLists.txt;0;")
add_test(CHTLJSCoreTest "/workspace/build/bin/test_chtljs_core")
set_tests_properties(CHTLJSCoreTest PROPERTIES  _BACKTRACE_TRIPLES "/workspace/tests/CMakeLists.txt;30;add_test;/workspace/tests/CMakeLists.txt;0;")
add_test(ScannerTest "/workspace/build/bin/test_scanner")
set_tests_properties(ScannerTest PROPERTIES  _BACKTRACE_TRIPLES "/workspace/tests/CMakeLists.txt;42;add_test;/workspace/tests/CMakeLists.txt;0;")
add_test(IntegrationTest "/workspace/build/bin/test_integration")
set_tests_properties(IntegrationTest PROPERTIES  _BACKTRACE_TRIPLES "/workspace/tests/CMakeLists.txt;60;add_test;/workspace/tests/CMakeLists.txt;0;")
