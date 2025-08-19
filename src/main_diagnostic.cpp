#include <iostream>
#include <fstream>
#include <string>
#include "CHTLCompiler/CHTLCompilerCore.h"
#include "Scanner/CHTLUnifiedScanner.h"
#include "CHTLCompiler/Lexer/CHTLLexer.h"

using namespace chtl;

void printTokens(const std::vector<Token>& tokens) {
    std::cout << "\n📋 Token列表 (前20个):" << std::endl;
    for (size_t i = 0; i < std::min(tokens.size(), size_t(20)); i++) {
        std::cout << "  [" << i << "] " << static_cast<int>(tokens[i].type) 
                  << " : '" << tokens[i].value << "'" << std::endl;
    }
    if (tokens.size() > 20) {
        std::cout << "  ... 还有 " << (tokens.size() - 20) << " 个token" << std::endl;
    }
}

void printScanResult(const scanner::CHTLUnifiedScanner::ScanResult& result) {
    std::cout << "\n📋 扫描结果:" << std::endl;
    std::cout << "  片段数量: " << result.fragments.size() << std::endl;
    std::cout << "  是否有错误: " << (result.hasErrors ? "是" : "否") << std::endl;
    
    if (result.hasErrors) {
        std::cout << "  错误信息:" << std::endl;
        for (const auto& error : result.errors) {
            std::cout << "    - " << error << std::endl;
        }
    }
    
    for (size_t i = 0; i < result.fragments.size(); i++) {
        const auto& fragment = result.fragments[i];
        std::cout << "\n  片段 " << i << ":" << std::endl;
        std::cout << "    上下文: " << static_cast<int>(fragment.context) << std::endl;
        std::cout << "    内容长度: " << fragment.content.length() << std::endl;
        std::cout << "    起始位置: " << fragment.startPosition << std::endl;
        std::cout << "    结束位置: " << fragment.endPosition << std::endl;
        std::cout << "    嵌套层级: " << fragment.nestingLevel << std::endl;
        
        if (fragment.content.length() > 100) {
            std::cout << "    内容预览: " << fragment.content.substr(0, 100) << "..." << std::endl;
        } else {
            std::cout << "    完整内容: " << fragment.content << std::endl;
        }
    }
}

int main(int argc, char* argv[]) {
    std::cout << "🔍 CHTL编译器诊断工具" << std::endl;
    std::cout << "=====================" << std::endl;

    if (argc < 2) {
        std::cout << "用法: " << argv[0] << " <CHTL文件>" << std::endl;
        return 1;
    }
    
    std::string inputFile = argv[1];
    std::cout << "诊断文件: " << inputFile << std::endl;
    
    try {
        // 1. 读取文件
        std::cout << "\n🔍 步骤1: 读取文件..." << std::endl;
        std::ifstream file(inputFile);
        if (!file.is_open()) {
            std::cerr << "❌ 无法打开文件: " << inputFile << std::endl;
            return 1;
        }
        
        std::string sourceCode((std::istreambuf_iterator<char>(file)),
                               std::istreambuf_iterator<char>());
        file.close();
        
        std::cout << "✅ 文件读取成功，长度: " << sourceCode.length() << " 字符" << std::endl;
        
        // 2. 词法分析
        std::cout << "\n🔍 步骤2: 词法分析..." << std::endl;
        CHTLLexer lexer;
        lexer.setInput(sourceCode);
        auto tokens = lexer.tokenize();
        
        std::cout << "✅ 词法分析完成，生成 " << tokens.size() << " 个token" << std::endl;
        printTokens(tokens);
        
        // 3. 统一扫描器分析
        std::cout << "\n🔍 步骤3: 统一扫描器分析..." << std::endl;
        scanner::CHTLUnifiedScanner scanner;
        auto scanResult = scanner.scanCode(sourceCode);
        
        std::cout << "✅ 扫描完成" << std::endl;
        printScanResult(scanResult);
        
        // 4. 测试CHTL编译器（直接使用，避免段错误）
        std::cout << "\n🔍 步骤4: 测试CHTL编译器..." << std::endl;
        CHTLCompilerCore compiler;
        compiler.setDebugMode(true);
        
        std::cout << "  4.1 初始化编译器..." << std::endl;
        if (!compiler.initialize()) {
            std::cout << "❌ 编译器初始化失败" << std::endl;
            auto errors = compiler.getErrors();
            for (const auto& error : errors) {
                std::cout << "    错误: " << error << std::endl;
            }
            return 1;
        }
        std::cout << "  ✅ 编译器初始化成功" << std::endl;
        
        std::cout << "  4.2 开始编译..." << std::endl;
        std::string result = compiler.compile(sourceCode);
        
        if (!result.empty()) {
            std::cout << "✅ 编译成功！" << std::endl;
            std::cout << "生成HTML长度: " << result.length() << " 字符" << std::endl;
            
            // 保存结果
            std::string outputFile = "diagnostic_output.html";
            std::ofstream outFile(outputFile);
            if (outFile.is_open()) {
                outFile << result;
                outFile.close();
                std::cout << "📁 输出已保存到: " << outputFile << std::endl;
            }
            
            // 显示结果预览
            std::cout << "\n📋 HTML输出预览 (前500字符):" << std::endl;
            std::cout << "================================" << std::endl;
            if (result.length() > 500) {
                std::cout << result.substr(0, 500) << "..." << std::endl;
            } else {
                std::cout << result << std::endl;
            }
            std::cout << "================================" << std::endl;
            
        } else {
            std::cout << "❌ 编译失败" << std::endl;
            auto errors = compiler.getErrors();
            for (const auto& error : errors) {
                std::cout << "    错误: " << error << std::endl;
            }
        }
        
    } catch (const std::exception& e) {
        std::cerr << "❌ 异常: " << e.what() << std::endl;
        return 1;
    }
    
    std::cout << "\n🎉 诊断完成" << std::endl;
    return 0;
}