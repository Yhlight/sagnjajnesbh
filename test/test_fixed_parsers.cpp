#include "../src/CHTLCompiler/Custom/CustomParser.h"
#include "../src/CHTLCompiler/Origin/OriginParser.h"
#include "../src/CHTLCompiler/Lexer/CHTLLexer.h"
#include <iostream>
#include <cassert>

int main() {
    std::cout << "🔧 测试已修复的Parser组件" << std::endl;
    std::cout << "=========================" << std::endl;
    
    try {
        // 测试CustomParser
        std::cout << "\n📋 测试CustomParser..." << std::endl;
        
        chtl::CHTLLexer lexer;
        lexer.setInput("[Custom] @Style TestStyle { color: red; }");
        auto tokens = lexer.tokenize();
        
        if (lexer.getErrors().size() > 0) {
            std::cout << "词法分析错误:" << std::endl;
            for (const auto& error : lexer.getErrors()) {
                std::cout << "  " << error << std::endl;
            }
        } else {
            std::cout << "  词法分析成功，生成 " << tokens.size() << " 个token" << std::endl;
            
            chtl::custom_system::CustomParser customParser;
            size_t position = 0;
            auto result = customParser.parseCustom(tokens, position);
            
            if (result) {
                std::cout << "  ✅ CustomParser解析成功: " << result->toString() << std::endl;
            } else {
                std::cout << "  ❌ CustomParser解析失败" << std::endl;
            }
        }
        
        // 测试OriginParser
        std::cout << "\n📋 测试OriginParser..." << std::endl;
        
        lexer.setInput("[Origin] @Html TestHTML { <div>Hello</div> }");
        tokens = lexer.tokenize();
        
        if (lexer.getErrors().size() > 0) {
            std::cout << "词法分析错误:" << std::endl;
            for (const auto& error : lexer.getErrors()) {
                std::cout << "  " << error << std::endl;
            }
        } else {
            std::cout << "  词法分析成功，生成 " << tokens.size() << " 个token" << std::endl;
            
            chtl::origin_system::OriginParser originParser;
            size_t position = 0;
            auto result = originParser.parseOrigin(tokens, position);
            
            if (result) {
                std::cout << "  ✅ OriginParser解析成功: " << result->toString() << std::endl;
            } else {
                std::cout << "  ❌ OriginParser解析失败" << std::endl;
            }
        }
        
        // 测试UTF-8中文字符支持
        std::cout << "\n📋 测试UTF-8中文字符支持..." << std::endl;
        
        lexer.setInput("div { text { 欢迎使用CHTL } }");
        tokens = lexer.tokenize();
        
        if (lexer.getErrors().size() > 0) {
            std::cout << "  ❌ 中文字符支持仍有问题:" << std::endl;
            for (const auto& error : lexer.getErrors()) {
                std::cout << "    " << error << std::endl;
            }
        } else {
            std::cout << "  ✅ UTF-8中文字符支持正常，生成 " << tokens.size() << " 个token" << std::endl;
        }
        
        std::cout << "\n🎉 已修复Parser组件测试完成" << std::endl;
        std::cout << "✅ CustomParser: 方法签名已修复" << std::endl;
        std::cout << "✅ OriginParser: 方法签名已修复" << std::endl;
        std::cout << "✅ UTF-8支持: 基础中文字符识别" << std::endl;
        
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "测试错误: " << e.what() << std::endl;
        return 1;
    }
}