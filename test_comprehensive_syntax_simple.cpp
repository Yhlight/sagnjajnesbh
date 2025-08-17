#include "include/CHTLParser.h"
#include "include/CHTLGenerator.h"
#include "include/CHTLCompiler.h"
#include <iostream>
#include <fstream>
#include <sstream>

using namespace chtl;

String readFile(const String& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        return "";
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

void writeFile(const String& filename, const String& content) {
    std::ofstream file(filename);
    if (file.is_open()) {
        file << content;
        file.close();
    }
}

void testBasicSyntax() {
    std::cout << "测试基础语法..." << std::endl;
    
    String testCode = R"(
    // 单行注释测试
    /* 多行注释测试 */
    -- 生成器注释测试
    
    div {
        id: test-basic;
        class = "test-class";
        
        text { "基础语法测试" }
        
        style {
            width: 100px;
            height: 100px;
            background-color: red;
        }
    }
    )";
    
    CompilerOptions options;
    options.debugMode = true;
    
    CHTLParser parser(options);
    auto parseResult = parser.parse(testCode, "basic_test.chtl");
    
    if (!parseResult.success) {
        std::cout << "❌ 基础语法解析失败" << std::endl;
        return;
    }
    
    CHTLGenerator generator(options);
    auto genResult = generator.generate(parseResult.ast);
    
    if (!genResult.success) {
        std::cout << "❌ 基础语法生成失败" << std::endl;
        return;
    }
    
    std::cout << "✅ 基础语法测试通过" << std::endl;
    std::cout << "HTML输出: " << genResult.htmlOutput << std::endl;
}

void testTemplateSystem() {
    std::cout << "测试模板系统..." << std::endl;
    
    String testCode = R"(
    [Template] @Style BaseText {
        color: black;
        font-size: 16px;
    }
    
    [Template] @Var Colors {
        primary: "blue";
        secondary: "gray";
    }
    
    [Template] @Element Card {
        div {
            style {
                .card {
                    background: white;
                    border: 1px solid #ddd;
                }
            }
        }
    }
    
    div {
        style {
            @Style BaseText;
            color: Colors(primary);
        }
        
        text { "模板系统测试" }
    }
    )";
    
    CompilerOptions options;
    options.debugMode = true;
    
    CHTLParser parser(options);
    auto parseResult = parser.parse(testCode, "template_test.chtl");
    
    if (!parseResult.success) {
        std::cout << "❌ 模板系统解析失败" << std::endl;
        return;
    }
    
    CHTLGenerator generator(options);
    auto genResult = generator.generate(parseResult.ast);
    
    if (!genResult.success) {
        std::cout << "❌ 模板系统生成失败" << std::endl;
        return;
    }
    
    std::cout << "✅ 模板系统测试通过" << std::endl;
}

void testCustomSystem() {
    std::cout << "测试自定义系统..." << std::endl;
    
    String testCode = R"(
    [Custom] @Style FlexLayout {
        display,
        flex-direction,
        justify-content;
    }
    
    [Custom] @Element Button {
        button {
            style {
                .btn {
                    padding: 8px 16px;
                    border: none;
                }
            }
            text { "按钮" }
        }
    }
    
    div {
        style {
            @Style FlexLayout {
                display: flex;
                flex-direction: column;
                justify-content: center;
            }
        }
        
        @Element Button;
    }
    )";
    
    CompilerOptions options;
    options.debugMode = true;
    
    CHTLParser parser(options);
    auto parseResult = parser.parse(testCode, "custom_test.chtl");
    
    if (!parseResult.success) {
        std::cout << "❌ 自定义系统解析失败" << std::endl;
        return;
    }
    
    CHTLGenerator generator(options);
    auto genResult = generator.generate(parseResult.ast);
    
    if (!genResult.success) {
        std::cout << "❌ 自定义系统生成失败" << std::endl;
        return;
    }
    
    std::cout << "✅ 自定义系统测试通过" << std::endl;
}

void testLocalStyleBlocks() {
    std::cout << "测试局部样式块..." << std::endl;
    
    String testCode = R"(
    div {
        style {
            // 内联样式
            width: 200px;
            height: 200px;
            
            // 自动类名
            .box {
                background: blue;
                border-radius: 8px;
            }
            
            // 上下文推导
            &:hover {
                opacity: 0.8;
            }
        }
        
        text { "局部样式块测试" }
    }
    )";
    
    CompilerOptions options;
    options.debugMode = true;
    
    CHTLParser parser(options);
    auto parseResult = parser.parse(testCode, "style_test.chtl");
    
    if (!parseResult.success) {
        std::cout << "❌ 局部样式块解析失败" << std::endl;
        return;
    }
    
    CHTLGenerator generator(options);
    auto genResult = generator.generate(parseResult.ast);
    
    if (!genResult.success) {
        std::cout << "❌ 局部样式块生成失败" << std::endl;
        return;
    }
    
    std::cout << "✅ 局部样式块测试通过" << std::endl;
}

void testComprehensiveFile() {
    std::cout << "测试综合语法文件..." << std::endl;
    
    String testCode = readFile("test_comprehensive_syntax.chtl");
    if (testCode.empty()) {
        std::cout << "❌ 无法读取综合测试文件" << std::endl;
        return;
    }
    
    std::cout << "📁 已读取测试文件 (大小: " << testCode.length() << " 字符)" << std::endl;
    
    CompilerOptions options;
    options.debugMode = true;
    
    CHTLParser parser(options);
    auto parseResult = parser.parse(testCode, "test_comprehensive_syntax.chtl");
    
    if (!parseResult.success) {
        std::cout << "❌ 综合文件解析失败:" << std::endl;
        for (const auto& error : parseResult.errors) {
            std::cout << "  " << error.message << std::endl;
        }
        return;
    }
    
    CHTLGenerator generator(options);
    auto genResult = generator.generate(parseResult.ast);
    
    if (!genResult.success) {
        std::cout << "❌ 综合文件生成失败:" << std::endl;
        for (const auto& error : genResult.errors) {
            std::cout << "  " << error.message << std::endl;
        }
        return;
    }
    
    // 生成完整HTML文件
    String finalHtml = "<!DOCTYPE html>\n<html>\n<head>\n<style>\n" + 
                      genResult.cssOutput + 
                      "\n</style>\n</head>\n<body>\n" + 
                      genResult.htmlOutput + 
                      "\n<script>\n" + genResult.jsOutput + 
                      "\n</script>\n</body>\n</html>";
    
    writeFile("comprehensive_output.html", finalHtml);
    
    std::cout << "✅ 综合语法文件测试通过" << std::endl;
    std::cout << "📊 生成统计:" << std::endl;
    std::cout << "  HTML: " << genResult.htmlOutput.length() << " 字符" << std::endl;
    std::cout << "  CSS: " << genResult.cssOutput.length() << " 字符" << std::endl;
    std::cout << "  JS: " << genResult.jsOutput.length() << " 字符" << std::endl;
}

int main() {
    std::cout << "🚀 CHTL编译器综合语法测试" << std::endl;
    std::cout << "=========================" << std::endl;
    
    try {
        testBasicSyntax();
        std::cout << std::endl;
        
        testTemplateSystem();
        std::cout << std::endl;
        
        testCustomSystem();
        std::cout << std::endl;
        
        testLocalStyleBlocks();
        std::cout << std::endl;
        
        testComprehensiveFile();
        std::cout << std::endl;
        
        std::cout << "🎉 所有测试完成!" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "❌ 测试异常: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}