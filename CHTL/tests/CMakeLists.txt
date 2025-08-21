# 测试配置
enable_testing()

# Parser测试
add_executable(test_parser test_parser.cpp)
target_link_libraries(test_parser
    PRIVATE
        CHTLCompiler
        CHTLUtils
)

# 简单Parser测试
add_executable(test_parser_simple test_parser_simple.cpp)
target_link_libraries(test_parser_simple
    PRIVATE
        CHTLCompiler
        CHTLUtils
)

# Lexer测试
add_executable(test_lexer test_lexer.cpp)
target_link_libraries(test_lexer
    PRIVATE
        CHTLCompiler
        CHTLUtils
)

# 添加测试
add_test(NAME ParserTest COMMAND test_parser)
add_test(NAME SimpleParserTest COMMAND test_parser_simple)
add_test(NAME LexerTest COMMAND test_lexer)

# 添加测试可执行文件
# TODO: 添加具体的测试