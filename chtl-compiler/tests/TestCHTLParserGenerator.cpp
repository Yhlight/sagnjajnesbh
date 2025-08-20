#include <iostream>
#include <cassert>
#include "chtl/compiler/CHTLCompiler.h"
#include "chtl/parser/CHTLParser.h"
#include "chtl/generator/CHTLGenerator.h"
#include "chtl/lexer/CHTLLexer.h"
#include "utils/Logger.h"

using namespace chtl;
using namespace chtl::compiler;
using namespace chtl::parser;
using namespace chtl::generator;

void TestBasicElement() {
    std::cout << "测试基本元素解析和生成..." << std::endl;
    
    std::string code = R"chtl(
html {
    head {
        title {
            text { "Hello CHTL" }
        }
    }
    body {
        div {
            id: "main";
            class: "container";
            
            text { "Welcome to CHTL!" }
        }
    }
}
)chtl";
    
    CHTLCompiler compiler;
    auto result = compiler.Compile(code, "test.chtl");
    
    assert(result.Success);
    std::cout << "生成的HTML:\n" << result.OutputContent << std::endl;
    
    // 验证输出包含期望的内容
    assert(result.OutputContent.find("<!DOCTYPE html>") != std::string::npos);
    assert(result.OutputContent.find("<title>Hello CHTL</title>") != std::string::npos);
    assert(result.OutputContent.find("id=\"main\"") != std::string::npos);
    assert(result.OutputContent.find("class=\"container\"") != std::string::npos);
    
    std::cout << "基本元素测试通过!" << std::endl;
}

void TestLocalStyleBlock() {
    std::cout << "测试局部样式块..." << std::endl;
    
    std::string code = R"chtl(
div {
    style {
        width: 100px;
        height: 200px;
        
        .box {
            background-color: red;
            padding: 10px;
        }
        
        &:hover {
            background-color: blue;
        }
    }
    
    text { "Styled Box" }
}
)chtl";
    
    CHTLCompiler compiler;
    auto result = compiler.Compile(code, "test.chtl");
    
    assert(result.Success);
    std::cout << "生成的HTML:\n" << result.OutputContent << std::endl;
    
    // 验证自动添加的类名
    assert(result.OutputContent.find("class=\"box\"") != std::string::npos);
    
    // 验证CSS被提取
    assert(result.OutputContent.find("<style>") != std::string::npos);
    assert(result.OutputContent.find(".box {") != std::string::npos);
    assert(result.OutputContent.find(".box:hover {") != std::string::npos);
    
    std::cout << "局部样式块测试通过!" << std::endl;
}

void TestTemplate() {
    std::cout << "测试模板功能..." << std::endl;
    
    std::string code = R"chtl(
[Template] @Style DefaultText {
    color: black;
    font-size: 16px;
    line-height: 1.6;
}

[Template] @Element Card {
    div {
        style {
            .card {
                border: 1px solid #ddd;
                padding: 20px;
            }
        }
        
        h3 {
            text { "Card Title" }
        }
        
        p {
            style {
                @Style DefaultText;
            }
            text { "Card content" }
        }
    }
}

body {
    @Element Card;
}
)chtl";
    
    CHTLCompiler compiler;
    auto result = compiler.Compile(code, "test.chtl");
    
    if (!result.Success) {
        for (const auto& error : result.Errors) {
            std::cout << "错误: " << error << std::endl;
        }
    }
    
    std::cout << "生成的HTML:\n" << result.OutputContent << std::endl;
    
    std::cout << "模板测试完成!" << std::endl;
}

void TestComments() {
    std::cout << "测试注释处理..." << std::endl;
    
    std::string code = R"chtl(
// 这是单行注释，不会出现在HTML中
/* 这是多行注释
   也不会出现在HTML中 */
-- 这是生成器注释，会出现在HTML中

div {
    // 元素内的注释
    text { "Content" }
}
)chtl";
    
    CHTLCompiler compiler;
    auto result = compiler.Compile(code, "test.chtl");
    
    assert(result.Success);
    std::cout << "生成的HTML:\n" << result.OutputContent << std::endl;
    
    // 验证只有生成器注释出现在输出中
    assert(result.OutputContent.find("这是生成器注释") != std::string::npos);
    assert(result.OutputContent.find("这是单行注释") == std::string::npos);
    assert(result.OutputContent.find("这是多行注释") == std::string::npos);
    
    std::cout << "注释处理测试通过!" << std::endl;
}

void TestCEEquivalence() {
    std::cout << "测试CE对等式（: 和 = 等价）..." << std::endl;
    
    std::string code = R"chtl(
div {
    id: "test1";
    class = "test2";
    
    style {
        width: 100px;
        height = 200px;
    }
}
)chtl";
    
    CHTLCompiler compiler;
    auto result = compiler.Compile(code, "test.chtl");
    
    assert(result.Success);
    std::cout << "生成的HTML:\n" << result.OutputContent << std::endl;
    
    // 验证两种写法都能正确处理
    assert(result.OutputContent.find("id=\"test1\"") != std::string::npos);
    assert(result.OutputContent.find("class=\"test2\"") != std::string::npos);
    
    std::cout << "CE对等式测试通过!" << std::endl;
}

void TestComplexStructure() {
    std::cout << "测试复杂结构..." << std::endl;
    
    std::string code = R"chtl(
[Template] @Style Theme {
    color: #333;
    font-family: Arial, sans-serif;
}

html {
    head {
        title { text { "CHTL Complex Test" } }
        
        style {
            body {
                margin: 0;
                padding: 0;
            }
        }
    }
    
    body {
        header {
            style {
                .header {
                    background: #f0f0f0;
                    padding: 20px;
                }
            }
            
            h1 {
                text { "Welcome to CHTL" }
            }
        }
        
        main {
            style {
                .content {
                    max-width: 1200px;
                    margin: 0 auto;
                    padding: 20px;
                }
            }
            
            div {
                style {
                    @Style Theme;
                }
                
                text { "This is the main content area." }
            }
        }
        
        footer {
            style {
                .footer {
                    background: #333;
                    color: white;
                    padding: 10px;
                    text-align: center;
                }
            }
            
            text { "© 2024 CHTL" }
        }
    }
}
)chtl";
    
    CHTLCompiler compiler;
    auto result = compiler.Compile(code, "test.chtl");
    
    if (!result.Success) {
        for (const auto& error : result.Errors) {
            std::cout << "错误: " << error << std::endl;
        }
    } else {
        std::cout << "生成的HTML:\n" << result.OutputContent << std::endl;
        std::cout << "\n复杂结构测试通过!" << std::endl;
    }
}

int main() {
    // 设置日志级别
    utils::Logger::GetInstance().SetLevel(utils::LogLevel::INFO);
    
    std::cout << "运行CHTL解析器和生成器测试...\n" << std::endl;
    
    TestBasicElement();
    std::cout << std::endl;
    
    TestLocalStyleBlock();
    std::cout << std::endl;
    
    TestTemplate();
    std::cout << std::endl;
    
    TestComments();
    std::cout << std::endl;
    
    TestCEEquivalence();
    std::cout << std::endl;
    
    TestComplexStructure();
    
    std::cout << "\n所有测试完成!" << std::endl;
    return 0;
}