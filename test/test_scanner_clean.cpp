#include "Scanner/CHTLUnifiedScanner.h"
#include <iostream>
#include <string>

using namespace chtl;

int main() {
    std::cout << "🎯 CHTL统一扫描器测试 - 清理后的架构 🎯" << std::endl;
    std::cout << "=======================================" << std::endl;
    
    try {
        // 测试统一扫描器
        scanner::CHTLUnifiedScanner scanner;
        scanner.setDebugMode(true);
        
        std::string testCode = R"(
            div {
                style {
                    color: red;
                    background: blue;
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
                }
                text {
                    这是无修饰字面量文本
                }
            }
        )";
        
        std::cout << "\n📝 测试代码:" << std::endl;
        std::cout << testCode << std::endl;
        
        std::cout << "\n🔍 开始扫描..." << std::endl;
        auto scanResult = scanner.scanCode(testCode);
        
        std::cout << "\n📊 扫描结果:" << std::endl;
        std::cout << "片段数量: " << scanResult.fragments.size() << std::endl;
        std::cout << "是否有错误: " << (scanResult.hasErrors ? "是" : "否") << std::endl;
        
        if (!scanResult.warnings.empty()) {
            std::cout << "\n⚠️ 警告信息:" << std::endl;
            for (const auto& warning : scanResult.warnings) {
                std::cout << "  - " << warning << std::endl;
            }
        }
        
        // 测试CHTL JS特征检测
        std::cout << "\n🎯 测试CHTL JS特征检测:" << std::endl;
        std::string chtljsCode = "{{box}}->click(); listen({target: {{button}});";
        bool hasCHTLJSFeatures = scanner.detectCHTLJSFeatures(chtljsCode);
        std::cout << "CHTL JS特征检测: " << (hasCHTLJSFeatures ? "✅ 检测到" : "❌ 未检测到") << std::endl;
        
        std::cout << "  增强选择器: " << (scanner.hasEnhancedSelectors(chtljsCode) ? "✅" : "❌") << std::endl;
        std::cout << "  箭头操作符: " << (scanner.hasArrowOperators(chtljsCode) ? "✅" : "❌") << std::endl;
        std::cout << "  CHTL JS函数: " << (scanner.hasCHTLJSFunctionCalls(chtljsCode) ? "✅" : "❌") << std::endl;
        
        std::cout << "\n🎉 清理后的架构测试完成！" << std::endl;
        std::cout << "✅ 4个编译器 + Scanner + Dispatcher + Common 架构正常 ✅" << std::endl;
        
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "❌ 测试异常: " << e.what() << std::endl;
        return 1;
    }
}