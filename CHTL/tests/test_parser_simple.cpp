#include <iostream>
#include <memory>
#include "CHTL/Parser/Parser.h"
#include "CHTL/AST/ASTPrinter.h"

int main() {
    // 非常简单的CHTL测试代码
    std::string chtlCode = R"(
div {
    id: container;
    
    text {
        "Hello World"
    }
}
    )";
    
    // 创建Parser
    CHTL::ParserConfig config;
    config.debugMode = true;
    CHTL::Parser parser(config);
    
    // 设置错误回调
    parser.SetErrorCallback([](const CHTL::ParseError& error) {
        std::cerr << "Parse Error at " << error.line << ":" << error.column 
                  << " - " << error.message << std::endl;
    });
    
    std::cout << "解析简单CHTL代码..." << std::endl;
    
    // 解析代码
    auto ast = parser.Parse(chtlCode, "simple_test.chtl");
    
    if (parser.HasErrors()) {
        std::cerr << "解析过程中发生 " << parser.GetErrors().size() 
                  << " 个错误" << std::endl;
        
        // 打印所有错误
        for (const auto& error : parser.GetErrors()) {
            std::cerr << "  错误: " << error.message 
                      << " at " << error.line << ":" << error.column << std::endl;
        }
        return 1;
    }
    
    if (ast) {
        std::cout << "解析成功！生成的AST结构：" << std::endl;
        std::cout << "================================" << std::endl;
        
        // 打印AST
        CHTL::AST::ASTPrinter printer(std::cout);
        ast->Accept(&printer);
        
        std::cout << "================================" << std::endl;
        std::cout << "AST节点总数：" << ast->GetChildCount() << std::endl;
    } else {
        std::cerr << "解析失败！" << std::endl;
        return 1;
    }
    
    return 0;
}