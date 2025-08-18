#include "../src/CHTLCompiler/Parser/CHTLParser.h"
#include "../src/CHTLCompiler/Generator/CHTLGenerator.h"
#include "../src/CHTLCompiler/Parser/CHTLContext.h"
#include <iostream>

using namespace chtl;
using namespace chtl::parser;
using namespace chtl::generator;
using namespace chtl::ast;

int main() {
    try {
        std::cout << "=== 调试CHTL解析器和生成器 ===" << std::endl;
        
        CompilerContext context;
        CHTLParser parser(context);
        HTMLGenerator generator(context);
        
        std::string input = R"(
            div {
                id: "test";
                text {
                    "Hello World"
                }
            }
        )";
        
        std::cout << "输入的CHTL代码:" << std::endl;
        std::cout << input << std::endl;
        std::cout << "=========================" << std::endl;
        
        parser.setInput(input);
        auto program = parser.parseProgram();
        
        if (program) {
            std::cout << "解析成功! 程序节点数量: " << program->getChildCount() << std::endl;
            
            for (size_t i = 0; i < program->getChildCount(); ++i) {
                auto child = program->getChild(i);
                std::cout << "子节点 " << i << ": " << child->toString() << std::endl;
            }
        } else {
            std::cout << "解析失败!" << std::endl;
        }
        
        if (parser.hasError()) {
            std::cout << "解析错误:" << std::endl;
            for (const auto& error : parser.getErrors()) {
                std::cout << "  " << error.message << " @" 
                          << error.position.line << ":" << error.position.column << std::endl;
            }
        }
        
        if (context.hasErrors()) {
            std::cout << "上下文错误:" << std::endl;
            for (const auto& error : context.getErrors()) {
                std::cout << "  " << error.message << " @" 
                          << error.line << ":" << error.column << std::endl;
            }
        }
        
        std::cout << "=========================" << std::endl;
        
        std::string html_output = generator.generate(program.get());
        
        std::cout << "生成的HTML:" << std::endl;
        std::cout << html_output << std::endl;
        std::cout << "=========================" << std::endl;
        
        std::cout << "生成的CSS:" << std::endl;
        std::cout << generator.getGeneratedCSS() << std::endl;
        std::cout << "=========================" << std::endl;
        
        std::cout << "生成的JS:" << std::endl;
        std::cout << generator.getGeneratedJS() << std::endl;
        
        if (generator.hasError()) {
            std::cout << "生成错误:" << std::endl;
            for (const auto& error : generator.getErrors()) {
                std::cout << "  " << error << std::endl;
            }
        }
        
    } catch (const std::exception& e) {
        std::cerr << "异常: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}