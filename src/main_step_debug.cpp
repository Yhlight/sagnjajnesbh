#include <iostream>
#include <string>
#include "CHTLCompiler/CHTLCompilerCore.h"
#include "Scanner/CHTLUnifiedScanner.h"

using namespace chtl;

int main() {
    std::cout << "🔍 CHTL分步调试版本" << std::endl;
    std::cout << "===================" << std::endl;

    try {
        std::cout << "1. 测试CHTLCompilerCore..." << std::endl;
        CHTLCompilerCore chtlCompiler;
        std::cout << "✅ CHTLCompilerCore创建成功" << std::endl;
        
        std::cout << "2. 初始化CHTLCompilerCore..." << std::endl;
        if (!chtlCompiler.initialize()) {
            std::cerr << "❌ CHTLCompilerCore初始化失败" << std::endl;
            return 1;
        }
        std::cout << "✅ CHTLCompilerCore初始化成功" << std::endl;
        
        std::cout << "3. 测试统一扫描器..." << std::endl;
        scanner::CHTLUnifiedScanner scanner;
        std::cout << "✅ 统一扫描器创建成功" << std::endl;
        
        std::cout << "4. 测试简单扫描..." << std::endl;
        std::string testCode = "html { body { text { Hello } } }";
        auto scanResult = scanner.scanCode(testCode);
        std::cout << "✅ 扫描成功，片段数: " << scanResult.fragments.size() << std::endl;
        
        std::cout << "5. 测试CHTL编译..." << std::endl;
        std::string compiledHTML = chtlCompiler.compile(testCode);
        if (!compiledHTML.empty()) {
            std::cout << "✅ CHTL编译成功！" << std::endl;
            std::cout << "结果长度: " << compiledHTML.length() << " 字符" << std::endl;
        } else {
            std::cout << "❌ CHTL编译失败" << std::endl;
        }
        
    } catch (const std::exception& e) {
        std::cerr << "❌ 异常: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "❌ 未知异常" << std::endl;
        return 1;
    }
    
    std::cout << "🎉 分步调试完成" << std::endl;
    return 0;
}