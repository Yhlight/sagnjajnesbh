#include "../src/parser/Parser.h"
#include "../src/generator/HTMLGenerator.h"
#include "../src/common/Context.h"
#include <iostream>
#include <cassert>

using namespace chtl;
using namespace chtl::parser;
using namespace chtl::generator;
using namespace chtl::ast;

void testSimpleParsing() {
    std::cout << "=== 简单解析测试 ===" << std::endl;
    
    CompilerContext context;
    CHTLParser parser(context);
    
    // 非常简单的测试用例
    std::string input = R"(
        div {
            text {
                "Hello"
            }
        }
    )";
    
    parser.setInput(input);
    auto program = parser.parseProgram();
    
    assert(program != nullptr);
    assert(program->type == NodeType::PROGRAM);
    
    std::cout << "程序节点数量: " << program->getChildCount() << std::endl;
    
    if (program->getChildCount() > 0) {
        auto first_child = program->getChild(0);
        std::cout << "第一个子节点类型: " << first_child->getNodeTypeName() << std::endl;
        
        if (first_child->type == NodeType::ELEMENT) {
            auto element = dynamic_cast<ElementNode*>(first_child);
            std::cout << "元素标签: " << element->tag_name << std::endl;
            std::cout << "元素子节点数量: " << element->getChildCount() << std::endl;
        }
    }
    
    std::cout << "简单解析测试通过!" << std::endl;
}

void testSimpleGeneration() {
    std::cout << "=== 简单生成测试 ===" << std::endl;
    
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
    
    parser.setInput(input);
    auto program = parser.parseProgram();
    
    assert(program != nullptr);
    
    std::string html_output = generator.generate(program.get());
    
    std::cout << "生成的HTML长度: " << html_output.length() << std::endl;
    std::cout << "HTML包含div: " << (html_output.find("<div") != std::string::npos) << std::endl;
    
    std::cout << "=== 生成的HTML (前500字符) ===" << std::endl;
    std::cout << html_output.substr(0, 500) << std::endl;
    
    std::cout << "简单生成测试通过!" << std::endl;
}

int main() {
    try {
        std::cout << "开始简化的CHTL解析器测试..." << std::endl;
        
        testSimpleParsing();
        testSimpleGeneration();
        
        std::cout << std::endl << "=== 简化测试通过! ===" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "测试失败: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}