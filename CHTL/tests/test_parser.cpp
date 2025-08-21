#include <iostream>
#include <memory>
#include "CHTL/Parser/Parser.h"
#include "CHTL/AST/ASTPrinter.h"

int main() {
    // 简单的CHTL测试代码
    std::string chtlCode = R"(
        html {
            head {
                title: "CHTL Parser Test";
            }
            
            body {
                div {
                    id: container;
                    class: main-content;
                    
                    style {
                        width: 100%;
                        height: 100vh;
                        
                        .box {
                            background-color: red;
                        }
                        
                        &:hover {
                            background-color: blue;
                        }
                    }
                    
                    text {
                        "Hello CHTL Parser!"
                    }
                }
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
    
    std::cout << "解析CHTL代码..." << std::endl;
    
    // 解析代码
    auto ast = parser.Parse(chtlCode, "test.chtl");
    
    if (parser.HasErrors()) {
        std::cerr << "解析过程中发生 " << parser.GetErrors().size() 
                  << " 个错误" << std::endl;
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