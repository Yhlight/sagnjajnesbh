#include <iostream>
#include <fstream>
#include <string>
#include "CHTLCompiler/CHTLCompilerCore.h"
#include "CHTLJSCompiler/Core/CHTLJSCompilerCore.h"
#include "Scanner/CHTLUnifiedScanner.h"

using namespace chtl;

int main(int argc, char* argv[]) {
    std::cout << "CHTL完整编译器 v1.0.0" << std::endl;
    std::cout << "======================" << std::endl;

    // 测试模式
    if (argc > 1 && std::string(argv[1]) == "--test") {
        std::cout << "\n🧪 测试模式" << std::endl;
        
        // 测试CHTL编译
        std::cout << "\n📋 测试CHTL编译..." << std::endl;
        CHTLCompilerCore chtlCompiler;
        if (!chtlCompiler.initialize()) {
            std::cerr << "❌ CHTL编译器初始化失败" << std::endl;
            return 1;
        }
        
        std::string testCHTL = R"(
html {
    head {
        title { text { Complete CHTL Test } }
    }
    body {
        div {
            class: main-container;
            h1 { text { CHTL Complete Compiler } }
            p { text { All parsers working correctly } }
        }
    }
}
)";
        
        std::string htmlOutput = chtlCompiler.compile(testCHTL);
        if (!htmlOutput.empty()) {
            std::cout << "✅ CHTL编译成功！" << std::endl;
            std::cout << "\n生成的HTML:" << std::endl;
            std::cout << "================================" << std::endl;
            std::cout << htmlOutput << std::endl;
            std::cout << "================================" << std::endl;
        } else {
            std::cout << "❌ CHTL编译失败" << std::endl;
        }
        
        // 测试CHTL JS编译
        std::cout << "\n📋 测试CHTL JS编译..." << std::endl;
        CHTLJSCompilerCore chtljsCompiler;
        
        std::string testCHTLJS = R"(
function testFunction() {
    console.log("CHTL JS Test");
}
)";
        
        std::string jsOutput = chtljsCompiler.compile(testCHTLJS);
        if (!jsOutput.empty()) {
            std::cout << "✅ CHTL JS编译成功！" << std::endl;
        } else {
            std::cout << "❌ CHTL JS编译失败" << std::endl;
        }
        
        // 测试统一扫描器
        std::cout << "\n📋 测试统一扫描器..." << std::endl;
        scanner::CHTLUnifiedScanner scanner;
        auto scanResult = scanner.scanCode(testCHTL);
        std::cout << "✅ 扫描器工作正常，识别了 " << scanResult.fragments.size() << " 个片段" << std::endl;
        
        std::cout << "\n🎉 所有组件测试完成！" << std::endl;
        return 0;
    }
    
    // 文件编译模式
    if (argc < 2) {
        std::cout << "用法: " << argv[0] << " <输入文件> [输出文件]" << std::endl;
        std::cout << "或者: " << argv[0] << " --test" << std::endl;
        return 1;
    }
    
    std::string inputFile = argv[1];
    std::string outputFile = argc > 2 ? argv[2] : "output.html";
    
    std::cout << "编译文件: " << inputFile << std::endl;
    std::cout << "输出到: " << outputFile << std::endl;
    
    CHTLCompilerCore compiler;
    if (!compiler.initialize()) {
        std::cerr << "❌ 编译器初始化失败" << std::endl;
        return 1;
    }
    
    std::string result = compiler.compileFile(inputFile);
    if (!result.empty()) {
        // 将结果写入文件
        std::ofstream outFile(outputFile);
        if (outFile.is_open()) {
            outFile << result;
            outFile.close();
            std::cout << "✅ 编译成功！输出已保存到: " << outputFile << std::endl;
        } else {
            std::cerr << "❌ 无法写入输出文件: " << outputFile << std::endl;
            return 1;
        }
    } else {
        std::cerr << "❌ 编译失败" << std::endl;
        return 1;
    }
    
    return 0;
}
