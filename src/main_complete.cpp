#include <iostream>
#include <fstream>
#include <string>
#include "Dispatcher/CompilerDispatcher.h"

using namespace chtl;

int main(int argc, char* argv[]) {
    std::cout << "CHTL完整编译器 v1.0.0 - 集成调度器" << std::endl;
    std::cout << "=====================================" << std::endl;

    // 初始化编译器调度器
    CompilerDispatcher dispatcher;
    if (!dispatcher.initialize()) {
        std::cerr << "❌ 编译器调度器初始化失败" << std::endl;
        return 1;
    }
    
    std::cout << "✅ 编译器调度器初始化成功" << std::endl;

    // 测试模式
    if (argc > 1 && std::string(argv[1]) == "--test") {
        std::cout << "\n🧪 测试模式 - 集成调度器测试" << std::endl;
        
        // 测试CHTL编译
        std::cout << "\n📋 测试CHTL编译..." << std::endl;
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
        
        auto chtlResult = dispatcher.compileCode(testCHTL);
        if (chtlResult.success) {
            std::cout << "✅ CHTL编译成功！" << std::endl;
            std::cout << "\n生成的HTML:" << std::endl;
            std::cout << "================================" << std::endl;
            std::cout << chtlResult.compiledCode << std::endl;
            std::cout << "================================" << std::endl;
        } else {
            std::cout << "❌ CHTL编译失败" << std::endl;
            for (const auto& error : chtlResult.errors) {
                std::cout << "  错误: " << error << std::endl;
            }
        }
        
        // 测试CHTL JS编译
        std::cout << "\n📋 测试CHTL JS编译..." << std::endl;
        std::string testCHTLJS = R"(
function testFunction() {
    console.log("CHTL JS Test");
    return "Hello from CHTL JS";
}
)";
        
        // 直接测试CHTL JS片段
        scanner::CHTLUnifiedScanner::CodeFragment jsFragment;
        jsFragment.content = testCHTLJS;
        jsFragment.context = scanner::CHTLUnifiedScanner::CodeContext::CHTL_SCRIPT;
        
        auto jsResult = dispatcher.compileFragment(jsFragment);
        if (jsResult.success) {
            std::cout << "✅ CHTL JS编译成功！" << std::endl;
        } else {
            std::cout << "❌ CHTL JS编译失败" << std::endl;
            for (const auto& error : jsResult.errors) {
                std::cout << "  错误: " << error << std::endl;
            }
        }
        
        // 测试CSS编译
        std::cout << "\n📋 测试CSS编译..." << std::endl;
        std::string testCSS = R"(
.container {
    width: 100%;
    margin: 0 auto;
}

h1 {
    color: #333;
    font-size: 24px;
}
)";
        
        scanner::CHTLUnifiedScanner::CodeFragment cssFragment;
        cssFragment.content = testCSS;
        cssFragment.context = scanner::CHTLUnifiedScanner::CodeContext::CSS_RAW;
        
        auto cssResult = dispatcher.compileFragment(cssFragment);
        if (cssResult.success) {
            std::cout << "✅ CSS编译成功！" << std::endl;
        } else {
            std::cout << "❌ CSS编译失败" << std::endl;
            for (const auto& error : cssResult.errors) {
                std::cout << "  错误: " << error << std::endl;
            }
        }
        
        std::cout << "\n🎉 所有编译器调度测试完成！" << std::endl;
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
    
    // 读取输入文件
    std::ifstream inFile(inputFile);
    if (!inFile.is_open()) {
        std::cerr << "❌ 无法打开输入文件: " << inputFile << std::endl;
        return 1;
    }
    
    std::string sourceCode((std::istreambuf_iterator<char>(inFile)),
                           std::istreambuf_iterator<char>());
    inFile.close();
    
    // 使用调度器编译
    auto result = dispatcher.compileCode(sourceCode);
    if (result.success) {
        // 将结果写入文件
        std::ofstream outFile(outputFile);
        if (outFile.is_open()) {
            outFile << result.compiledCode;
            outFile.close();
            std::cout << "✅ 编译成功！输出已保存到: " << outputFile << std::endl;
            
            if (!result.warnings.empty()) {
                std::cout << "\n⚠️ 警告:" << std::endl;
                for (const auto& warning : result.warnings) {
                    std::cout << "  " << warning << std::endl;
                }
            }
        } else {
            std::cerr << "❌ 无法写入输出文件: " << outputFile << std::endl;
            return 1;
        }
    } else {
        std::cerr << "❌ 编译失败" << std::endl;
        for (const auto& error : result.errors) {
            std::cerr << "  错误: " << error << std::endl;
        }
        return 1;
    }
    
    return 0;
}