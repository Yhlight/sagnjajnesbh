// CHTL ANTLR运行时基础使用示例
// 展示ANTLR运行时的基本功能

#include "antlr4-runtime.h"
#include <iostream>
#include <string>
#include <memory>

using namespace antlr4;

int main() {
    std::cout << "CHTL ANTLR运行时基础示例" << std::endl;
    std::cout << "========================" << std::endl;
    
    try {
        // 1. 创建输入流
        std::cout << "\n1. 创建输入流..." << std::endl;
        std::string input = "Hello, ANTLR World! 这是一个测试字符串。";
        ANTLRInputStream stream(input);
        
        std::cout << "输入内容: \"" << input << "\"" << std::endl;
        std::cout << "流大小: " << stream.size() << " 字符" << std::endl;
        std::cout << "第一个字符: '" << static_cast<char>(stream.LA(1)) << "'" << std::endl;
        
        // 2. 创建和操作Token
        std::cout << "\n2. 创建和操作Token..." << std::endl;
        
        // 创建token工厂
        CommonTokenFactory factory;
        
        // 创建一些测试token
        auto token1 = factory.create(1, "Hello");
        auto token2 = factory.create(2, "World");
        auto token3 = factory.create(3, "!");
        
        // 设置token位置信息
        token1->setLine(1);
        token1->setCharPositionInLine(0);
        
        token2->setLine(1);
        token2->setCharPositionInLine(6);
        
        token3->setLine(1);
        token3->setCharPositionInLine(11);
        
        // 显示token信息
        std::cout << "Token 1: 类型=" << token1->getType() 
                  << ", 文本=\"" << token1->getText() 
                  << "\", 行=" << token1->getLine()
                  << ", 列=" << token1->getCharPositionInLine() << std::endl;
                  
        std::cout << "Token 2: 类型=" << token2->getType() 
                  << ", 文本=\"" << token2->getText() 
                  << "\", 行=" << token2->getLine()
                  << ", 列=" << token2->getCharPositionInLine() << std::endl;
                  
        std::cout << "Token 3: 类型=" << token3->getType() 
                  << ", 文本=\"" << token3->getText() 
                  << "\", 行=" << token3->getLine()
                  << ", 列=" << token3->getCharPositionInLine() << std::endl;
        
        // 3. 字符流操作
        std::cout << "\n3. 字符流操作..." << std::endl;
        
        // 重新创建流用于演示
        ANTLRInputStream demo_stream("ANTLR");
        
        std::cout << "流内容: \"" << demo_stream.toString() << "\"" << std::endl;
        std::cout << "逐字符读取: ";
        
        for (size_t i = 1; i <= demo_stream.size(); i++) {
            char ch = static_cast<char>(demo_stream.LA(i));
            std::cout << "'" << ch << "' ";
        }
        std::cout << std::endl;
        
        // 4. 内存管理演示
        std::cout << "\n4. 内存管理演示..." << std::endl;
        
        {
            // 在作用域内创建多个对象
            std::vector<std::unique_ptr<Token>> tokens;
            
            for (int i = 0; i < 10; i++) {
                tokens.push_back(factory.create(i + 1, "token_" + std::to_string(i)));
            }
            
            std::cout << "创建了 " << tokens.size() << " 个token" << std::endl;
            
            // 验证所有token
            bool all_valid = true;
            for (size_t i = 0; i < tokens.size(); i++) {
                if (!tokens[i] || tokens[i]->getType() != static_cast<int>(i + 1)) {
                    all_valid = false;
                    break;
                }
            }
            
            std::cout << "所有token验证: " << (all_valid ? "通过" : "失败") << std::endl;
            
            // tokens将在作用域结束时自动清理
        }
        
        std::cout << "作用域结束，内存已自动清理" << std::endl;
        
        // 5. 异常处理演示
        std::cout << "\n5. 异常处理演示..." << std::endl;
        
        try {
            // 创建一个可能抛出异常的场景
            ANTLRInputStream test_stream("test");
            
            // 模拟一个识别异常
            RecognitionException ex(nullptr, &test_stream, nullptr);
            
            std::cout << "创建识别异常成功" << std::endl;
            
            // 检查异常属性
            if (ex.getInputStream() == &test_stream) {
                std::cout << "异常输入流匹配正确" << std::endl;
            }
            
        } catch (const std::exception& e) {
            std::cout << "捕获异常: " << e.what() << std::endl;
        }
        
        // 6. 性能测试
        std::cout << "\n6. 简单性能测试..." << std::endl;
        
        auto start = std::chrono::high_resolution_clock::now();
        
        const int iterations = 1000;
        for (int i = 0; i < iterations; i++) {
            std::string test_input = "performance_test_" + std::to_string(i);
            ANTLRInputStream perf_stream(test_input);
            auto perf_token = factory.create(i, test_input);
        }
        
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        
        std::cout << "创建 " << iterations << " 个流和token耗时: " 
                  << duration.count() << " 微秒" << std::endl;
        std::cout << "平均每次操作: " << (duration.count() / iterations) << " 微秒" << std::endl;
        
        std::cout << "\n✅ 所有基础功能演示完成！" << std::endl;
        std::cout << "\n🎉 ANTLR运行时工作正常！" << std::endl;
        
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "\n❌ 发生异常: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "\n❌ 发生未知异常" << std::endl;
        return 1;
    }
}