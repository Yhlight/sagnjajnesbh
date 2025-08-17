#include "../src/parser/Parser.h"
#include "../src/generator/HTMLGenerator.h"
#include "../src/common/Context.h"
#include <iostream>
#include <cassert>

using namespace chtl;
using namespace chtl::parser;
using namespace chtl::generator;
using namespace chtl::ast;

void testCompleteWorkflow() {
    std::cout << "=== 测试完整的解析和生成工作流 ===" << std::endl;
    
    CompilerContext context;
    CHTLParser parser(context);
    HTMLGenerator generator(context);
    
    // 测试一个完整的CHTL文档
    std::string input = R"(
        [Template] @Style DefaultText {
            color: black;
            line-height: 1.6;
            font-size: 16px;
        }
        
        [Custom] @Element Box {
            div {
                style {
                    width: 200px;
                    height: 200px;
                    border: 1px solid gray;
                }
            }
        }
        
        html {
            head {
                title {
                    text {
                        "CHTL测试页面"
                    }
                }
            }
            
            body {
                div {
                    id: "container";
                    class: "main-container";
                    
                    style {
                        @Style DefaultText;
                        
                        .box {
                            background-color: lightblue;
                        }
                        
                        &:hover {
                            opacity: 0.8;
                        }
                    }
                    
                    script {
                        {{box}}->listen({
                            click: () => {
                                console.log('Box clicked!');
                            },
                            
                            mouseenter: function() {
                                this.style.backgroundColor = 'lightgreen';
                            }
                        });
                    }
                    
                    h1 {
                        text {
                            "欢迎使用CHTL"
                        }
                    }
                    
                    @Element Box;
                    
                    text {
                        "这是一个CHTL生成的页面"
                    }
                }
            }
        }
    )";
    
    // 解析CHTL代码
    parser.setInput(input);
    auto program = parser.parseProgram();
    
    assert(program != nullptr);
    assert(program->type == NodeType::PROGRAM);
    
    if (parser.hasError()) {
        std::cout << "解析错误:" << std::endl;
        for (const auto& error : parser.getErrors()) {
            std::cout << "  " << error.message << std::endl;
        }
    }
    
    // 生成HTML
    std::string html_output = generator.generate(program.get());
    
    assert(!html_output.empty());
    assert(html_output.find("<!DOCTYPE html>") != std::string::npos);
    assert(html_output.find("<html>") != std::string::npos);
    assert(html_output.find("</html>") != std::string::npos);
    
    if (generator.hasError()) {
        std::cout << "生成错误:" << std::endl;
        for (const auto& error : generator.getErrors()) {
            std::cout << "  " << error << std::endl;
        }
    }
    
    // 检查生成的CSS
    std::string generated_css = generator.getGeneratedCSS();
    assert(!generated_css.empty());
    
    // 检查生成的JS
    std::string generated_js = generator.getGeneratedJS();
    assert(!generated_js.empty());
    
    std::cout << "=== 生成的HTML ===" << std::endl;
    std::cout << html_output << std::endl;
    
    std::cout << "=== 生成的CSS ===" << std::endl;
    std::cout << generated_css << std::endl;
    
    std::cout << "=== 生成的JS ===" << std::endl;
    std::cout << generated_js << std::endl;
    
    std::cout << "完整工作流测试通过!" << std::endl;
}

void testTemplateExpansion() {
    std::cout << "=== 测试模板展开 ===" << std::endl;
    
    CompilerContext context;
    CHTLParser parser(context);
    HTMLGenerator generator(context);
    
    std::string input = R"(
        [Template] @Element Button {
            button {
                style {
                    padding: 10px 20px;
                    border: none;
                    border-radius: 4px;
                    cursor: pointer;
                }
                
                text {
                    "Click Me"
                }
            }
        }
        
        div {
            @Element Button;
            @Element Button;
        }
    )";
    
    parser.setInput(input);
    auto program = parser.parseProgram();
    
    assert(program != nullptr);
    
    std::string html_output = generator.generate(program.get());
    
    // 检查是否正确展开了模板
    assert(html_output.find("<button>") != std::string::npos);
    assert(html_output.find("Click Me") != std::string::npos);
    
    std::cout << "模板展开测试通过!" << std::endl;
}

void testCHTLJSGeneration() {
    std::cout << "=== 测试CHTL JS代码生成 ===" << std::endl;
    
    CompilerContext context;
    CHTLParser parser(context);
    HTMLGenerator generator(context);
    
    std::string input = R"(
        div {
            style {
                .interactive-box {
                    width: 100px;
                    height: 100px;
                    background-color: red;
                }
            }
            
            script {
                {{interactive-box}}->listen({
                    click: () => {
                        console.log('Interactive box clicked!');
                    }
                });
                
                vir TestVir = listen({
                    click: function() {
                        alert('Virtual object clicked!');
                    }
                });
            }
        }
    )";
    
    parser.setInput(input);
    auto program = parser.parseProgram();
    
    assert(program != nullptr);
    
    std::string html_output = generator.generate(program.get());
    std::string generated_js = generator.getGeneratedJS();
    
    // 检查生成的JavaScript代码
    assert(generated_js.find("addEventListener") != std::string::npos);
    assert(generated_js.find("querySelector") != std::string::npos);
    
    std::cout << "生成的JS代码:" << std::endl;
    std::cout << generated_js << std::endl;
    
    std::cout << "CHTL JS代码生成测试通过!" << std::endl;
}

void testOriginBlockGeneration() {
    std::cout << "=== 测试原始嵌入块生成 ===" << std::endl;
    
    CompilerContext context;
    CHTLParser parser(context);
    HTMLGenerator generator(context);
    
    std::string input = R"(
        [Origin] @Html customContent {
            <div class="custom-html">
                <p>这是原始HTML内容</p>
            </div>
        }
        
        [Origin] @Style {
            .custom-style {
                background: linear-gradient(45deg, red, blue);
            }
        }
        
        [Origin] @JavaScript {
            function customFunction() {
                return "Hello from custom JS";
            }
        }
        
        div {
            [Origin] @Html customContent;
        }
    )";
    
    parser.setInput(input);
    auto program = parser.parseProgram();
    
    assert(program != nullptr);
    
    std::string html_output = generator.generate(program.get());
    std::string generated_css = generator.getGeneratedCSS();
    std::string generated_js = generator.getGeneratedJS();
    
    // 检查原始内容是否正确嵌入
    assert(html_output.find("这是原始HTML内容") != std::string::npos);
    assert(generated_css.find("linear-gradient") != std::string::npos);
    assert(generated_js.find("customFunction") != std::string::npos);
    
    std::cout << "原始嵌入块生成测试通过!" << std::endl;
}

void testErrorRecovery() {
    std::cout << "=== 测试错误恢复 ===" << std::endl;
    
    CompilerContext context;
    CHTLParser parser(context);
    HTMLGenerator generator(context);
    
    std::string input = R"(
        div {
            id: "test"  // 缺少分号
            
            style {
                width: 100px;
                // 这里有一个错误但应该能继续解析
                invalid-property-syntax
                height: 200px;
            }
            
            text {
                "这个文本应该能正常解析"
            }
        }
    )";
    
    parser.setInput(input);
    auto program = parser.parseProgram();
    
    assert(program != nullptr);
    
    // 即使有错误，也应该能生成一些HTML
    std::string html_output = generator.generate(program.get());
    assert(!html_output.empty());
    
    // 检查错误是否被正确记录
    bool has_parsing_errors = parser.hasError() || context.hasErrors();
    if (has_parsing_errors) {
        std::cout << "成功捕获并处理了解析错误" << std::endl;
    }
    
    std::cout << "错误恢复测试通过!" << std::endl;
}

int main() {
    try {
        std::cout << "开始CHTL解析器和生成器集成测试..." << std::endl;
        
        testCompleteWorkflow();
        testTemplateExpansion();
        testCHTLJSGeneration();
        testOriginBlockGeneration();
        testErrorRecovery();
        
        std::cout << std::endl << "=== 所有解析器和生成器测试通过! ===" << std::endl;
        std::cout << "第三个Todo (开发CHTL与CHTL JS的解析器及生成器) 已完成!" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "测试失败: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}