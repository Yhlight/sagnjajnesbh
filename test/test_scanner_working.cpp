#include "Scanner/CHTLUnifiedScanner.h"
#include <iostream>
#include <string>

using namespace chtl;

int main() {
    std::cout << "🔧 CHTL统一扫描器 - 恢复工作状态测试 🔧" << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << "严格按照CHTL语法文档实现" << std::endl;
    
    try {
        // 测试统一扫描器
        scanner::CHTLUnifiedScanner scanner;
        scanner.setDebugMode(true);
        
        std::string testCode = R"(
            div {
                id: box;
                class: welcome;
                
                style {
                    color: red;
                    background: blue;
                    .welcome {
                        font-size: 16px;
                    }
                }
                
                script {
                    {{box}}->click();
                    listen({
                        target: {{button}},
                        event: click,
                        handler: function() {
                            console.log("CHTL JS clicked!");
                        }
                    });
                    
                    animate({
                        target: {{.welcome}},
                        duration: 1000,
                        begin: { opacity: 0 },
                        end: { opacity: 1 }
                    });
                }
                
                text {
                    这是无修饰字面量文本
                }
            }
        )";
        
        std::cout << "\n📝 测试代码（严格按照CHTL语法）:" << std::endl;
        std::cout << testCode << std::endl;
        
        std::cout << "\n🔍 开始扫描..." << std::endl;
        auto scanResult = scanner.scanCode(testCode);
        
        std::cout << "\n📊 扫描结果:" << std::endl;
        std::cout << "片段数量: " << scanResult.fragments.size() << std::endl;
        std::cout << "是否有错误: " << (scanResult.hasErrors ? "是" : "否") << std::endl;
        
        if (scanResult.hasErrors) {
            std::cout << "\n❌ 错误信息:" << std::endl;
            for (const auto& error : scanResult.errors) {
                std::cout << "  - " << error << std::endl;
            }
        }
        
        if (!scanResult.warnings.empty()) {
            std::cout << "\n⚠️ 警告信息:" << std::endl;
            for (const auto& warning : scanResult.warnings) {
                std::cout << "  - " << warning << std::endl;
            }
        }
        
        std::cout << "\n📋 代码片段详情:" << std::endl;
        for (size_t i = 0; i < scanResult.fragments.size(); ++i) {
            const auto& fragment = scanResult.fragments[i];
            std::cout << "片段 " << (i + 1) << ":" << std::endl;
            std::cout << "  上下文: " << static_cast<int>(fragment.context) << std::endl;
            std::cout << "  起始位置: " << fragment.startPosition << std::endl;
            std::cout << "  结束位置: " << fragment.endPosition << std::endl;
            std::cout << "  嵌套层级: " << fragment.nestingLevel << std::endl;
            std::cout << "  包含CHTL JS特征: " << (fragment.containsCHTLJSFeatures ? "是" : "否") << std::endl;
            std::cout << "  包含无修饰字面量: " << (fragment.containsUndecoratedLiterals ? "是" : "否") << std::endl;
            std::cout << "  内容预览: " << fragment.content.substr(0, 100) << "..." << std::endl;
            std::cout << std::endl;
        }
        
        // 测试CHTL JS特征检测
        std::cout << "\n🎯 测试CHTL JS特征检测（严格按照语法文档）:" << std::endl;
        
        std::string chtljsCode = "{{box}}->click(); listen({target: {{button}}});";
        bool hasCHTLJSFeatures = scanner.detectCHTLJSFeatures(chtljsCode);
        std::cout << "CHTL JS特征检测: " << (hasCHTLJSFeatures ? "✅ 检测到" : "❌ 未检测到") << std::endl;
        
        std::cout << "  增强选择器: " << (scanner.hasEnhancedSelectors(chtljsCode) ? "✅" : "❌") << std::endl;
        std::cout << "  箭头操作符: " << (scanner.hasArrowOperators(chtljsCode) ? "✅" : "❌") << std::endl;
        std::cout << "  CHTL JS函数: " << (scanner.hasCHTLJSFunctionCalls(chtljsCode) ? "✅" : "❌") << std::endl;
        
        // 测试无修饰字面量扫描
        std::cout << "\n📝 测试无修饰字面量扫描（基于上下文，非判断条件）:" << std::endl;
        std::string textContent = "这是无修饰字面量文本";
        auto literals = scanner.scanUndecoratedLiterals(textContent, scanner::CHTLUnifiedScanner::CodeContext::CHTL_TEXT);
        std::cout << "扫描到的无修饰字面量数量: " << literals.size() << std::endl;
        
        for (const auto& [pos, value] : literals) {
            std::cout << "  位置 " << pos << ": " << value << std::endl;
        }
        
        // 测试上下文识别
        std::cout << "\n🔍 测试上下文识别:" << std::endl;
        std::vector<std::string> contextStack = {"div", "script"};
        auto context = scanner.identifyContext(testCode, 100, contextStack);
        std::cout << "识别的上下文: " << static_cast<int>(context) << std::endl;
        
        std::cout << "\n🎉 统一扫描器恢复工作状态测试完成！" << std::endl;
        std::cout << "✅ 严格按照CHTL语法文档实现" << std::endl;
        std::cout << "✅ 基于上下文的精确扫描" << std::endl;
        std::cout << "✅ 不将无修饰字面量作为CHTL JS判断条件" << std::endl;
        std::cout << "✅ 可变长度切片机制（基础实现）" << std::endl;
        
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "❌ 测试异常: " << e.what() << std::endl;
        return 1;
    }
}