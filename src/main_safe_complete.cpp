#include <iostream>
#include <fstream>
#include <string>
#include "CHTLCompiler/CHTLCompilerCore.h"
#include "Scanner/CHTLUnifiedScanner.h"

using namespace chtl;

int main(int argc, char* argv[]) {
    std::cout << "CHTL安全完整编译器 v1.0.0" << std::endl;
    std::cout << "==============================" << std::endl;

    // 测试模式
    if (argc > 1 && std::string(argv[1]) == "--test") {
        std::cout << "\n🧪 测试模式 - 安全版本" << std::endl;
        
        try {
            // 测试统一扫描器
            std::cout << "\n📋 测试统一扫描器..." << std::endl;
            scanner::CHTLUnifiedScanner scanner;
            
            std::string testCHTL = R"(
html {
    head {
        title { text { Safe Complete Test } }
    }
    body {
        div {
            class: main-container;
            h1 { text { CHTL Safe Compiler } }
            p { text { Testing without segfault } }
        }
    }
}
)";
            
            auto scanResult = scanner.scanCode(testCHTL);
            if (scanResult.hasErrors) {
                std::cout << "❌ 扫描器错误:" << std::endl;
                for (const auto& error : scanResult.errors) {
                    std::cout << "  " << error << std::endl;
                }
                return 1;
            }
            
            std::cout << "✅ 扫描成功，片段数: " << scanResult.fragments.size() << std::endl;
            
            // 测试CHTL编译器（直接使用，不通过调度器）
            std::cout << "\n📋 测试CHTL编译器..." << std::endl;
            CHTLCompilerCore chtlCompiler;
            
            if (!chtlCompiler.initialize()) {
                std::cerr << "❌ CHTL编译器初始化失败" << std::endl;
                return 1;
            }
            
            std::string htmlOutput = chtlCompiler.compile(testCHTL);
            if (!htmlOutput.empty()) {
                std::cout << "✅ CHTL编译成功！" << std::endl;
                std::cout << "\n生成的HTML:" << std::endl;
                std::cout << "================================" << std::endl;
                std::cout << htmlOutput << std::endl;
                std::cout << "================================" << std::endl;
                
                // 保存到文件
                std::ofstream outFile("safe_complete_output.html");
                if (outFile.is_open()) {
                    outFile << htmlOutput;
                    outFile.close();
                    std::cout << "📁 输出已保存到: safe_complete_output.html" << std::endl;
                }
            } else {
                std::cout << "❌ CHTL编译失败" << std::endl;
                auto errors = chtlCompiler.getErrors();
                for (const auto& error : errors) {
                    std::cout << "错误: " << error << std::endl;
                }
            }
            
        } catch (const std::exception& e) {
            std::cerr << "❌ 异常: " << e.what() << std::endl;
            return 1;
        }
        
        std::cout << "\n🎉 安全版本测试完成！" << std::endl;
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
    
    try {
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
            auto errors = compiler.getErrors();
            for (const auto& error : errors) {
                std::cerr << "错误: " << error << std::endl;
            }
            return 1;
        }
        
    } catch (const std::exception& e) {
        std::cerr << "❌ 编译异常: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}