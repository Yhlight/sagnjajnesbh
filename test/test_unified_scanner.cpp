#include "UnifiedCore/Scanner/CHTLUnifiedScanner.h"
#include "UnifiedCore/Dispatcher/CompilerDispatcher.h"
#include <iostream>
#include <string>

using namespace chtl;

int main() {
    std::cout << "🔍 CHTL统一扫描器测试 🔍" << std::endl;
    std::cout << "=====================================" << std::endl;
    
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
            std::cout << "  内容预览: " << fragment.content.substr(0, 50) << "..." << std::endl;
            std::cout << std::endl;
        }
        
        // 测试编译器调度器
        std::cout << "\n🚀 测试编译器调度器..." << std::endl;
        CompilerDispatcher dispatcher;
        if (dispatcher.initialize()) {
            std::cout << "✅ 编译器调度器初始化成功" << std::endl;
            
            auto compileResult = dispatcher.compileCode(testCode);
            std::cout << "编译成功: " << (compileResult.success ? "是" : "否") << std::endl;
            
            if (!compileResult.errors.empty()) {
                std::cout << "编译错误:" << std::endl;
                for (const auto& error : compileResult.errors) {
                    std::cout << "  - " << error << std::endl;
                }
            }
        } else {
            std::cout << "❌ 编译器调度器初始化失败" << std::endl;
        }
        
        std::cout << "\n🎉 统一扫描器测试完成！" << std::endl;
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "❌ 测试异常: " << e.what() << std::endl;
        return 1;
    }
}