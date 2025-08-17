#include "include/CHTLParser.h"
#include "include/CHTLGenerator.h"
#include "include/CHTLCompiler.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <cassert>

using namespace chtl;

String readFile(const String& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "无法打开文件: " << filename << std::endl;
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
        std::cout << "已写入文件: " << filename << std::endl;
    } else {
        std::cerr << "无法写入文件: " << filename << std::endl;
    }
}

void testComprehensiveSyntax() {
    std::cout << "=== CHTL 综合语法测试 ===" << std::endl;
    
    // 读取综合语法测试文件
    String testCode = readFile("test_comprehensive_syntax.chtl");
    if (testCode.empty()) {
        std::cout << "❌ 无法读取测试文件" << std::endl;
        return;
    }
    
    std::cout << "📁 已读取测试文件 (大小: " << testCode.length() << " 字符)" << std::endl;
    
    // 配置编译选项
    CompilerOptions options;
    options.debugMode = true;
    options.outputPath = "comprehensive_test_output.html";
    
    std::cout << "\n1. 开始解析CHTL代码..." << std::endl;
    
    // 解析CHTL代码
    CHTLParser parser(options);
    auto parseResult = parser.parse(testCode, "test_comprehensive_syntax.chtl");
    
    if (!parseResult.success) {
        std::cout << "❌ 解析失败:" << std::endl;
        for (const auto& error : parseResult.errors) {
            std::cout << "  错误: " << error.message << std::endl;
        }
        return;
    }
    
    std::cout << "✅ 解析成功 - 生成了 " << parseResult.ast.size() << " 个AST节点" << std::endl;
    
    std::cout << "\n2. 开始生成HTML/CSS/JS代码..." << std::endl;
    
    // 生成HTML/CSS/JS代码
    CHTLGenerator generator(options);
    auto genResult = generator.generate(parseResult.ast);
    
    if (!genResult.success) {
        std::cout << "❌ 生成失败:" << std::endl;
        for (const auto& error : genResult.errors) {
            std::cout << "  错误: " << error.message << std::endl;
        }
        return;
    }
    
    std::cout << "✅ 代码生成成功" << std::endl;
    
    // 显示生成结果统计
    std::cout << "\n📊 生成结果统计:" << std::endl;
    std::cout << "  HTML代码长度: " << genResult.htmlOutput.length() << " 字符" << std::endl;
    std::cout << "  CSS代码长度: " << genResult.cssOutput.length() << " 字符" << std::endl;
    std::cout << "  JS代码长度: " << genResult.jsOutput.length() << " 字符" << std::endl;
    
    // 生成完整的HTML文件
    String finalHtml = R"(<!DOCTYPE html>
<html lang="zh-CN">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>CHTL综合语法测试</title>
    <style>
)" + genResult.cssOutput + R"(
    </style>
</head>
<body>
)" + genResult.htmlOutput + R"(
    <script>
)" + genResult.jsOutput + R"(
    </script>
</body>
</html>)";
    
    // 写入输出文件
    writeFile("comprehensive_test_output.html", finalHtml);
    
    // 显示部分输出内容
    std::cout << "\n📝 HTML输出预览 (前500字符):" << std::endl;
    std::cout << "----------------------------------------" << std::endl;
    std::string htmlPreview = genResult.htmlOutput.substr(0, 500);
    std::cout << htmlPreview;
    if (genResult.htmlOutput.length() > 500) {
        std::cout << "...";
    }
    std::cout << std::endl;
    std::cout << "----------------------------------------" << std::endl;
    
    if (!genResult.cssOutput.empty()) {
        std::cout << "\n🎨 CSS输出预览 (前300字符):" << std::endl;
        std::cout << "----------------------------------------" << std::endl;
        std::string cssPreview = genResult.cssOutput.substr(0, 300);
        std::cout << cssPreview;
        if (genResult.cssOutput.length() > 300) {
            std::cout << "...";
        }
        std::cout << std::endl;
        std::cout << "----------------------------------------" << std::endl;
    }
    
    if (!genResult.jsOutput.empty()) {
        std::cout << "\n⚡ JavaScript输出预览 (前300字符):" << std::endl;
        std::cout << "----------------------------------------" << std::endl;
        std::string jsPreview = genResult.jsOutput.substr(0, 300);
        std::cout << jsPreview;
        if (genResult.jsOutput.length() > 300) {
            std::cout << "...";
        }
        std::cout << std::endl;
        std::cout << "----------------------------------------" << std::endl;
    }
    
    std::cout << "\n✅ 综合语法测试完成!" << std::endl;
}

void testSyntaxFeatures() {
    std::cout << "\n=== 语法特性覆盖测试 ===" << std::endl;
    
    struct SyntaxTest {
        String name;
        String code;
        String description;
    };
    
    std::vector<SyntaxTest> tests = {
        {
            "注释测试",
            R"(
            // 单行注释
            /* 多行注释 */
            -- 生成器注释
            div { text { 测试注释 } }
            )",
            "测试三种注释类型"
        },
        {
            "文本节点测试", 
            R"(
            text { "带引号的文本" }
            text { 无修饰字面量文本 }
            )",
            "测试文本节点的不同写法"
        },
        {
            "CE对等式测试",
            R"(
            div {
                id: test1;
                class = test2;
                data-value = "测试";
            }
            )",
            "测试冒号和等号的等价性"
        },
        {
            "局部样式块测试",
            R"(
            div {
                style {
                    width: 100px;
                    .auto-class {
                        color: red;
                    }
                    &:hover {
                        opacity: 0.8;
                    }
                }
            }
            )",
            "测试内联样式、自动类名、上下文推导"
        }
    };
    
    int passedTests = 0;
    CompilerOptions options;
    options.debugMode = false;
    
    for (const auto& test : tests) {
        std::cout << "\n🧪 " << test.name << ": " << test.description << std::endl;
        
        CHTLParser parser(options);
        auto parseResult = parser.parse(test.code, "feature_test.chtl");
        
        if (parseResult.success) {
            CHTLGenerator generator(options);
            auto genResult = generator.generate(parseResult.ast);
            
            if (genResult.success) {
                std::cout << "  ✅ 通过 - 生成了 " << genResult.htmlOutput.length() << " 字符HTML";
                if (!genResult.cssOutput.empty()) {
                    std::cout << ", " << genResult.cssOutput.length() << " 字符CSS";
                }
                std::cout << std::endl;
                passedTests++;
            } else {
                std::cout << "  ❌ 生成失败: " << genResult.errors[0].message << std::endl;
            }
        } else {
            std::cout << "  ❌ 解析失败: " << parseResult.errors[0].message << std::endl;
        }
    }
    
    std::cout << "\n📊 语法特性测试结果: " << passedTests << "/" << tests.size() << " 通过" << std::endl;
}

int main() {
    std::cout << "🚀 CHTL编译器综合语法解析与生成测试" << std::endl;
    std::cout << "========================================" << std::endl;
    
    try {
        // 测试综合语法文件
        testComprehensiveSyntax();
        
        // 测试各个语法特性
        testSyntaxFeatures();
        
        std::cout << "\n🎉 所有测试完成!" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "❌ 测试过程中发生异常: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}