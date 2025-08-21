# CMake generated Testfile for 
# Source directory: /workspace/CHTL/tests
# Build directory: /workspace/CHTL/build/tests
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test(ParserTest "/workspace/CHTL/build/bin/test_parser")
set_tests_properties(ParserTest PROPERTIES  _BACKTRACE_TRIPLES "/workspace/CHTL/tests/CMakeLists.txt;29;add_test;/workspace/CHTL/tests/CMakeLists.txt;0;")
add_test(SimpleParserTest "/workspace/CHTL/build/bin/test_parser_simple")
set_tests_properties(SimpleParserTest PROPERTIES  _BACKTRACE_TRIPLES "/workspace/CHTL/tests/CMakeLists.txt;30;add_test;/workspace/CHTL/tests/CMakeLists.txt;0;")
add_test(LexerTest "/workspace/CHTL/build/bin/test_lexer")
set_tests_properties(LexerTest PROPERTIES  _BACKTRACE_TRIPLES "/workspace/CHTL/tests/CMakeLists.txt;31;add_test;/workspace/CHTL/tests/CMakeLists.txt;0;")
