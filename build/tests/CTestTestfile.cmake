# CMake generated Testfile for 
# Source directory: /workspace/tests
# Build directory: /workspace/build/tests
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test(CHTLCoreTest "/workspace/build/bin/test_chtl_core")
set_tests_properties(CHTLCoreTest PROPERTIES  _BACKTRACE_TRIPLES "/workspace/tests/CMakeLists.txt;18;add_test;/workspace/tests/CMakeLists.txt;0;")
add_test(CHTLASTTest "/workspace/build/bin/test_chtl_ast")
set_tests_properties(CHTLASTTest PROPERTIES  _BACKTRACE_TRIPLES "/workspace/tests/CMakeLists.txt;30;add_test;/workspace/tests/CMakeLists.txt;0;")
add_test(CHTLCompilerTest "/workspace/build/bin/test_chtl_compiler")
set_tests_properties(CHTLCompilerTest PROPERTIES  _BACKTRACE_TRIPLES "/workspace/tests/CMakeLists.txt;42;add_test;/workspace/tests/CMakeLists.txt;0;")
add_test(CHTLSimpleTest "/workspace/build/bin/test_chtl_simple")
set_tests_properties(CHTLSimpleTest PROPERTIES  _BACKTRACE_TRIPLES "/workspace/tests/CMakeLists.txt;54;add_test;/workspace/tests/CMakeLists.txt;0;")
add_test(CHTLJSCoreTest "/workspace/build/bin/test_chtljs_core")
set_tests_properties(CHTLJSCoreTest PROPERTIES  _BACKTRACE_TRIPLES "/workspace/tests/CMakeLists.txt;66;add_test;/workspace/tests/CMakeLists.txt;0;")
add_test(ScannerTest "/workspace/build/bin/test_scanner")
set_tests_properties(ScannerTest PROPERTIES  _BACKTRACE_TRIPLES "/workspace/tests/CMakeLists.txt;78;add_test;/workspace/tests/CMakeLists.txt;0;")
add_test(IntegrationTest "/workspace/build/bin/test_integration")
set_tests_properties(IntegrationTest PROPERTIES  _BACKTRACE_TRIPLES "/workspace/tests/CMakeLists.txt;96;add_test;/workspace/tests/CMakeLists.txt;0;")
