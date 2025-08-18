// CHTL ANTLR运行时基础测试
// 测试ANTLR运行时的基本功能

#include "antlr4-runtime.h"
#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <cassert>

using namespace antlr4;

// 测试结果统计
struct TestResults {
    int total = 0;
    int passed = 0;
    int failed = 0;
    std::vector<std::string> failures;
    
    void addResult(const std::string& test_name, bool success, const std::string& error = "") {
        total++;
        if (success) {
            passed++;
            std::cout << "✅ " << test_name << " - PASSED" << std::endl;
        } else {
            failed++;
            failures.push_back(test_name + ": " + error);
            std::cout << "❌ " << test_name << " - FAILED: " << error << std::endl;
        }
    }
    
    void printSummary() {
        std::cout << "\n=== 测试总结 ===" << std::endl;
        std::cout << "总测试数: " << total << std::endl;
        std::cout << "通过: " << passed << std::endl;
        std::cout << "失败: " << failed << std::endl;
        
        if (failed > 0) {
            std::cout << "\n失败的测试:" << std::endl;
            for (const auto& failure : failures) {
                std::cout << "  - " << failure << std::endl;
            }
        }
        
        if (failed == 0) {
            std::cout << "\n🎉 所有测试通过！" << std::endl;
        } else {
            std::cout << "\n💥 " << failed << " 个测试失败" << std::endl;
        }
    }
    
    bool allPassed() const {
        return failed == 0;
    }
};

// 测试ANTLRInputStream
void testANTLRInputStream(TestResults& results) {
    try {
        std::string input = "hello world 123";
        ANTLRInputStream stream(input);
        
        // 测试基本属性
        if (stream.size() != input.length()) {
            results.addResult("ANTLRInputStream size", false, "Size mismatch");
            return;
        }
        
        // 测试字符读取
        if (stream.LA(1) != 'h') {
            results.addResult("ANTLRInputStream LA(1)", false, "First character mismatch");
            return;
        }
        
        // 测试toString
        std::string str = stream.toString();
        if (str != input) {
            results.addResult("ANTLRInputStream toString", false, "String conversion failed");
            return;
        }
        
        results.addResult("ANTLRInputStream", true);
        
    } catch (const std::exception& e) {
        results.addResult("ANTLRInputStream", false, e.what());
    }
}

// 测试CommonToken
void testCommonToken(TestResults& results) {
    try {
        // 创建token
        auto token = std::make_unique<CommonToken>(1, "hello");
        
        // 测试基本属性
        if (token->getType() != 1) {
            results.addResult("CommonToken type", false, "Type mismatch");
            return;
        }
        
        if (token->getText() != "hello") {
            results.addResult("CommonToken text", false, "Text mismatch");
            return;
        }
        
        // 测试设置属性
        token->setLine(10);
        token->setCharPositionInLine(5);
        
        if (token->getLine() != 10) {
            results.addResult("CommonToken line", false, "Line setting failed");
            return;
        }
        
        if (token->getCharPositionInLine() != 5) {
            results.addResult("CommonToken column", false, "Column setting failed");
            return;
        }
        
        results.addResult("CommonToken", true);
        
    } catch (const std::exception& e) {
        results.addResult("CommonToken", false, e.what());
    }
}

// 测试TokenFactory
void testTokenFactory(TestResults& results) {
    try {
        CommonTokenFactory factory;
        
        // 创建token
        auto token = factory.create(2, "world");
        
        if (!token) {
            results.addResult("TokenFactory create", false, "Token creation failed");
            return;
        }
        
        if (token->getType() != 2) {
            results.addResult("TokenFactory token type", false, "Type mismatch");
            return;
        }
        
        if (token->getText() != "world") {
            results.addResult("TokenFactory token text", false, "Text mismatch");
            return;
        }
        
        results.addResult("TokenFactory", true);
        
    } catch (const std::exception& e) {
        results.addResult("TokenFactory", false, e.what());
    }
}

// 简单的测试词法分析器
class TestLexer : public Lexer {
private:
    static const std::vector<std::string> ruleNames;
    static const std::vector<std::string> channelNames;
    static const std::vector<std::string> modeNames;
    static const std::vector<std::string> tokenNames;
    
public:
    enum {
        ID = 1,
        NUMBER = 2,
        WS = 3
    };
    
    TestLexer(CharStream* input) : Lexer(input) {
        _interp = new atn::LexerATNSimulator(this, _atn, _decisionToDFA, _sharedContextCache);
    }
    
    ~TestLexer() {
        delete _interp;
    }
    
    std::string getGrammarFileName() const override {
        return "Test.g4";
    }
    
    const std::vector<std::string>& getRuleNames() const override {
        return ruleNames;
    }
    
    const std::vector<std::string>& getChannelNames() const override {
        return channelNames;
    }
    
    const std::vector<std::string>& getModeNames() const override {
        return modeNames;
    }
    
    const std::vector<std::string>& getTokenNames() const override {
        return tokenNames;
    }
    
    dfa::Vocabulary& getVocabulary() const override {
        static dfa::VocabularyImpl vocab(tokenNames, {}, {});
        return vocab;
    }
    
    const std::string getSerializedATN() const override {
        return "";
    }
    
    const atn::ATN& getATN() const override {
        return _atn;
    }
    
private:
    static atn::ATN _atn;
};

// 静态成员定义
const std::vector<std::string> TestLexer::ruleNames = {"ID", "NUMBER", "WS"};
const std::vector<std::string> TestLexer::channelNames = {"DEFAULT_TOKEN_CHANNEL", "HIDDEN"};
const std::vector<std::string> TestLexer::modeNames = {"DEFAULT_MODE"};
const std::vector<std::string> TestLexer::tokenNames = {"<INVALID>", "ID", "NUMBER", "WS"};
atn::ATN TestLexer::_atn;

// 测试Lexer基础功能
void testLexerBasics(TestResults& results) {
    try {
        std::string input = "hello 123";
        ANTLRInputStream stream(input);
        TestLexer lexer(&stream);
        
        // 测试基本属性
        if (lexer.getGrammarFileName() != "Test.g4") {
            results.addResult("Lexer grammar name", false, "Grammar name mismatch");
            return;
        }
        
        const auto& ruleNames = lexer.getRuleNames();
        if (ruleNames.size() != 3) {
            results.addResult("Lexer rule names", false, "Rule names count mismatch");
            return;
        }
        
        results.addResult("Lexer basics", true);
        
    } catch (const std::exception& e) {
        results.addResult("Lexer basics", false, e.what());
    }
}

// 测试CommonTokenStream
void testCommonTokenStream(TestResults& results) {
    try {
        // 创建一些测试token
        std::vector<std::unique_ptr<Token>> tokens;
        tokens.push_back(std::make_unique<CommonToken>(1, "hello"));
        tokens.push_back(std::make_unique<CommonToken>(2, "123"));
        tokens.push_back(std::make_unique<CommonToken>(Token::EOF_TYPE, ""));
        
        // 注意：CommonTokenStream需要TokenSource，这里我们只测试基本构造
        std::string input = "hello 123";
        ANTLRInputStream stream(input);
        TestLexer lexer(&stream);
        CommonTokenStream tokenStream(&lexer);
        
        // 测试基本属性
        if (tokenStream.getTokenSource() != &lexer) {
            results.addResult("CommonTokenStream source", false, "Token source mismatch");
            return;
        }
        
        results.addResult("CommonTokenStream", true);
        
    } catch (const std::exception& e) {
        results.addResult("CommonTokenStream", false, e.what());
    }
}

// 测试异常处理
void testExceptionHandling(TestResults& results) {
    try {
        // 测试RecognitionException
        ANTLRInputStream stream("test");
        TestLexer lexer(&stream);
        
        // 创建一个RecognitionException
        auto token = std::make_unique<CommonToken>(1, "test");
        RecognitionException ex(&lexer, &stream, nullptr);
        
        // 测试异常属性
        if (ex.getRecognizer() != &lexer) {
            results.addResult("Exception recognizer", false, "Recognizer mismatch");
            return;
        }
        
        if (ex.getInputStream() != &stream) {
            results.addResult("Exception input stream", false, "Input stream mismatch");
            return;
        }
        
        results.addResult("Exception handling", true);
        
    } catch (const std::exception& e) {
        results.addResult("Exception handling", false, e.what());
    }
}

// 测试内存管理
void testMemoryManagement(TestResults& results) {
    try {
        // 测试大量对象创建和销毁
        const int iterations = 1000;
        
        for (int i = 0; i < iterations; i++) {
            std::string input = "test" + std::to_string(i);
            ANTLRInputStream stream(input);
            
            auto token = std::make_unique<CommonToken>(i, input);
            
            // 验证对象状态
            if (token->getText() != input) {
                results.addResult("Memory management", false, "Object state corrupted");
                return;
            }
        }
        
        results.addResult("Memory management", true);
        
    } catch (const std::exception& e) {
        results.addResult("Memory management", false, e.what());
    }
}

// 测试线程安全性（基础测试）
void testThreadSafety(TestResults& results) {
    try {
        // 简单的线程安全测试
        // 注意：这只是基础测试，真正的多线程测试需要更复杂的设置
        
        CommonTokenFactory factory1;
        CommonTokenFactory factory2;
        
        auto token1 = factory1.create(1, "thread1");
        auto token2 = factory2.create(2, "thread2");
        
        if (!token1 || !token2) {
            results.addResult("Thread safety", false, "Token creation failed");
            return;
        }
        
        if (token1->getText() != "thread1" || token2->getText() != "thread2") {
            results.addResult("Thread safety", false, "Token data corrupted");
            return;
        }
        
        results.addResult("Thread safety (basic)", true);
        
    } catch (const std::exception& e) {
        results.addResult("Thread safety (basic)", false, e.what());
    }
}

// 性能测试
void testPerformance(TestResults& results) {
    try {
        const int iterations = 10000;
        auto start = std::chrono::high_resolution_clock::now();
        
        for (int i = 0; i < iterations; i++) {
            std::string input = "performance test " + std::to_string(i);
            ANTLRInputStream stream(input);
            
            auto token = std::make_unique<CommonToken>(1, input);
        }
        
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        
        std::cout << "性能测试: " << iterations << " 次迭代耗时 " << duration.count() << " ms" << std::endl;
        
        // 简单的性能检查：不应该超过5秒
        if (duration.count() > 5000) {
            results.addResult("Performance", false, "Too slow: " + std::to_string(duration.count()) + " ms");
            return;
        }
        
        results.addResult("Performance", true);
        
    } catch (const std::exception& e) {
        results.addResult("Performance", false, e.what());
    }
}

// 主测试函数
int main(int argc, char* argv[]) {
    std::cout << "CHTL ANTLR运行时基础测试" << std::endl;
    std::cout << "=========================" << std::endl;
    
    TestResults results;
    
    // 运行所有测试
    std::cout << "\n运行基础功能测试..." << std::endl;
    testANTLRInputStream(results);
    testCommonToken(results);
    testTokenFactory(results);
    testLexerBasics(results);
    testCommonTokenStream(results);
    
    std::cout << "\n运行高级功能测试..." << std::endl;
    testExceptionHandling(results);
    testMemoryManagement(results);
    testThreadSafety(results);
    
    std::cout << "\n运行性能测试..." << std::endl;
    testPerformance(results);
    
    // 打印测试总结
    results.printSummary();
    
    return results.allPassed() ? 0 : 1;
}