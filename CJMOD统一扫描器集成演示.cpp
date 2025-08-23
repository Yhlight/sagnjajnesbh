#include "CJMOD/ElegantCJMODApi.h"
#include <iostream>
#include <vector>
#include <string>

using namespace CHTL::CJMOD;

// 模拟统一扫描器的Token结构
struct MockToken {
    std::string value;
    std::string type;
    size_t position;
    
    MockToken(const std::string& val, const std::string& t, size_t pos) 
        : value(val), type(t), position(pos) {}
};

// 模拟统一扫描器
class MockUnifiedScanner {
private:
    std::vector<MockToken> tokens_;
    size_t currentPos_;
    
public:
    MockUnifiedScanner(const std::vector<MockToken>& tokens) 
        : tokens_(tokens), currentPos_(0) {}
    
    void scan() {
        std::cout << "\n🔍 统一扫描器开始扫描" << std::endl;
        std::cout << "总Token数: " << tokens_.size() << std::endl;
        
        while (currentPos_ < tokens_.size()) {
            const auto& token = tokens_[currentPos_];
            
            std::cout << "\n--- 扫描位置 " << currentPos_ << " ---" << std::endl;
            std::cout << "Token: '" << token.value << "' (类型: " << token.type << ")" << std::endl;
            
            // 检查是否为CJMOD关键字
            if (CJMODKeywordHandler::isCJMODKeyword(token.value)) {
                std::cout << "🎯 发现CJMOD关键字，调用CJMOD处理" << std::endl;
                
                // 调用CJMOD处理
                bool handled = CJMODScannerIntegration::processKeywordFromScanner(token.value, this);
                
                if (handled) {
                    std::cout << "✅ CJMOD处理成功，跳过统一扫描器处理" << std::endl;
                    handleCJMODProcessing(token);
                } else {
                    std::cout << "❌ CJMOD处理失败，继续统一扫描器处理" << std::endl;
                    handleNormalToken(token);
                }
            } else {
                std::cout << "📋 普通Token，统一扫描器正常处理" << std::endl;
                handleNormalToken(token);
            }
            
            currentPos_++;
        }
        
        std::cout << "\n✅ 统一扫描器扫描完成" << std::endl;
    }
    
private:
    void handleNormalToken(const MockToken& token) {
        std::cout << "  → 统一扫描器处理: " << token.value << std::endl;
    }
    
    void handleCJMODProcessing(const MockToken& token) {
        auto info = CJMODKeywordHandler::getKeywordInfo(token.value);
        
        std::cout << "  → CJMOD特殊处理开始" << std::endl;
        
        if (info.needsBacktrack) {
            performBacktrack(info.backtrackDistance);
        }
        
        if (info.needsForwardCollect) {
            performForwardCollect(info.forwardCollectDistance);
        }
        
        std::cout << "  → CJMOD特殊处理完成" << std::endl;
    }
    
    void performBacktrack(size_t distance) {
        std::cout << "    ⬅️ 执行回退操作，距离: " << distance << std::endl;
        
        for (size_t i = 1; i <= distance && currentPos_ >= i; ++i) {
            size_t backPos = currentPos_ - i;
            std::cout << "      回退到位置 " << backPos << ": '" 
                      << tokens_[backPos].value << "'" << std::endl;
        }
    }
    
    void performForwardCollect(size_t distance) {
        std::cout << "    ➡️ 执行向前收集，距离: " << distance << std::endl;
        
        for (size_t i = 1; i <= distance && currentPos_ + i < tokens_.size(); ++i) {
            size_t forwardPos = currentPos_ + i;
            std::cout << "      收集位置 " << forwardPos << ": '" 
                      << tokens_[forwardPos].value << "'" << std::endl;
        }
    }
};

// 演示场景1：处理幂运算符 **
void demonstratePowerOperator() {
    std::cout << "\n🌸 演示场景1：处理幂运算符 **\n" << std::endl;
    
    // 模拟代码：someFunction(arg ** arg2, param1);
    std::vector<MockToken> tokens = {
        {"someFunction", "IDENTIFIER", 0},
        {"(", "LPAREN", 1},
        {"arg", "IDENTIFIER", 2},
        {"**", "POWER_OP", 3},        // CJMOD关键字
        {"arg2", "IDENTIFIER", 4},
        {",", "COMMA", 5},
        {"param1", "IDENTIFIER", 6},
        {")", "RPAREN", 7}
    };
    
    std::cout << "源代码: someFunction(arg ** arg2, param1);" << std::endl;
    std::cout << "关键问题: ** 需要收集左操作数 'arg'（已在前面切片中）" << std::endl;
    
    MockUnifiedScanner scanner(tokens);
    scanner.scan();
}

// 演示场景2：处理iNeverAway函数
void demonstrateINeverAwayFunction() {
    std::cout << "\n🌸 演示场景2：处理iNeverAway函数\n" << std::endl;
    
    // 模拟代码：vir myVir = iNeverAway({MyPromise: function() {...}});
    std::vector<MockToken> tokens = {
        {"vir", "VIR", 0},
        {"myVir", "IDENTIFIER", 1},
        {"=", "ASSIGN", 2},
        {"iNeverAway", "IDENTIFIER", 3},  // CJMOD关键字
        {"(", "LPAREN", 4},
        {"{", "LBRACE", 5},
        {"MyPromise", "IDENTIFIER", 6},
        {":", "COLON", 7},
        {"function", "FUNCTION", 8},
        {"}", "RBRACE", 9},
        {")", "RPAREN", 10}
    };
    
    std::cout << "源代码: vir myVir = iNeverAway({MyPromise: function() {...}});" << std::endl;
    std::cout << "关键问题: iNeverAway 需要收集完整的函数参数" << std::endl;
    
    MockUnifiedScanner scanner(tokens);
    scanner.scan();
}

// 演示场景3：处理printMylove函数
void demonstratePrintMyloveFunction() {
    std::cout << "\n🌸 演示场景3：处理printMylove函数\n" << std::endl;
    
    // 模拟代码：printMylove({url: "image.jpg", mode: ascii});
    std::vector<MockToken> tokens = {
        {"printMylove", "IDENTIFIER", 0}, // CJMOD关键字
        {"(", "LPAREN", 1},
        {"{", "LBRACE", 2},
        {"url", "IDENTIFIER", 3},
        {":", "COLON", 4},
        {"\"image.jpg\"", "STRING", 5},
        {",", "COMMA", 6},
        {"mode", "IDENTIFIER", 7},
        {":", "COLON", 8},
        {"ascii", "IDENTIFIER", 9},      // 无修饰字面量
        {"}", "RBRACE", 10},
        {")", "RPAREN", 11}
    };
    
    std::cout << "源代码: printMylove({url: \"image.jpg\", mode: ascii});" << std::endl;
    std::cout << "关键问题: printMylove 需要处理CHTL JS官方特性（无修饰字面量等）" << std::endl;
    
    MockUnifiedScanner scanner(tokens);
    scanner.scan();
}

// 演示场景4：混合语法处理
void demonstrateMixedSyntax() {
    std::cout << "\n🌸 演示场景4：混合语法处理\n" << std::endl;
    
    // 模拟代码：const result = printMylove({url: base ** 2});
    std::vector<MockToken> tokens = {
        {"const", "CONST", 0},
        {"result", "IDENTIFIER", 1},
        {"=", "ASSIGN", 2},
        {"printMylove", "IDENTIFIER", 3}, // CJMOD关键字
        {"(", "LPAREN", 4},
        {"{", "LBRACE", 5},
        {"url", "IDENTIFIER", 6},
        {":", "COLON", 7},
        {"base", "IDENTIFIER", 8},
        {"**", "POWER_OP", 9},           // CJMOD关键字
        {"2", "NUMBER", 10},
        {"}", "RBRACE", 11},
        {")", "RPAREN", 12}
    };
    
    std::cout << "源代码: const result = printMylove({url: base ** 2});" << std::endl;
    std::cout << "关键问题: 混合了两个CJMOD关键字，需要正确处理嵌套" << std::endl;
    
    MockUnifiedScanner scanner(tokens);
    scanner.scan();
}

// 演示双指针扫描器概念
void demonstrateDualPointerConcept() {
    std::cout << "\n🌸 演示：双指针扫描器概念\n" << std::endl;
    
    std::cout << "=== 双指针扫描器状态演示 ===" << std::endl;
    
    std::vector<std::string> tokens = {"token0", "token1", "arg", "**", "arg2", "token5"};
    
    std::cout << "Token序列: ";
    for (size_t i = 0; i < tokens.size(); ++i) {
        std::cout << "[" << tokens[i] << "] ";
    }
    std::cout << std::endl;
    
    size_t frontPointer = 0;
    size_t backPointer = 0;
    
    std::cout << "\n1. 初始状态:" << std::endl;
    std::cout << "   frontPointer = " << frontPointer << ", backPointer = " << backPointer << std::endl;
    std::cout << "   两指针都指向位置0" << std::endl;
    
    // 正常扫描到位置3
    frontPointer = 3;
    backPointer = 3;
    std::cout << "\n2. 正常扫描到位置3:" << std::endl;
    std::cout << "   frontPointer = " << frontPointer << ", backPointer = " << backPointer << std::endl;
    std::cout << "   发现关键字 '**' 在位置3" << std::endl;
    
    // 进入收集模式
    backPointer = 2; // 回退到收集起点
    std::cout << "\n3. 进入CJMOD收集模式:" << std::endl;
    std::cout << "   frontPointer = " << frontPointer << " (关键字位置)" << std::endl;
    std::cout << "   backPointer = " << backPointer << " (收集起点)" << std::endl;
    std::cout << "   需要收集: [" << tokens[backPointer] << "] [" << tokens[frontPointer] << "] [" << tokens[frontPointer+1] << "]" << std::endl;
    
    // 收集完成
    frontPointer = 5;
    backPointer = 5;
    std::cout << "\n4. 收集完成，继续正常扫描:" << std::endl;
    std::cout << "   frontPointer = " << frontPointer << ", backPointer = " << backPointer << std::endl;
    std::cout << "   CJMOD片段已处理，继续扫描剩余token" << std::endl;
}

// 演示关键字注册和查询
void demonstrateKeywordRegistration() {
    std::cout << "\n🌸 演示：关键字注册和查询\n" << std::endl;
    
    // 注册自定义关键字
    std::cout << "=== 注册自定义CJMOD关键字 ===" << std::endl;
    
    CJMODKeywordInfo customOpInfo;
    customOpInfo.keyword = "@@";
    customOpInfo.needsBacktrack = true;
    customOpInfo.backtrackDistance = 2;
    customOpInfo.needsForwardCollect = true;
    customOpInfo.forwardCollectDistance = 2;
    customOpInfo.handler = []() {
        std::cout << "  → 处理自定义操作符 @@" << std::endl;
        std::cout << "  → 这是一个需要回退2单元，向前收集2单元的操作符" << std::endl;
    };
    
    CJMODKeywordHandler::registerKeyword("@@", customOpInfo);
    
    // 查询所有关键字
    std::cout << "\n=== 查询所有注册的关键字 ===" << std::endl;
    auto allKeywords = CJMODKeywordHandler::getAllKeywords();
    
    for (const auto& keyword : allKeywords) {
        auto info = CJMODKeywordHandler::getKeywordInfo(keyword);
        std::cout << "关键字: " << keyword << std::endl;
        std::cout << "  回退: " << (info.needsBacktrack ? "是" : "否");
        if (info.needsBacktrack) {
            std::cout << " (距离: " << info.backtrackDistance << ")";
        }
        std::cout << std::endl;
        std::cout << "  向前收集: " << (info.needsForwardCollect ? "是" : "否");
        if (info.needsForwardCollect) {
            std::cout << " (距离: " << info.forwardCollectDistance << ")";
        }
        std::cout << std::endl << std::endl;
    }
}

int main() {
    std::cout << "🌸 CJMOD与统一扫描器集成机制演示" << std::endl;
    std::cout << "解决CJMOD语法的拦截、推送、上下文处理问题" << std::endl;
    
    // 初始化CJMOD扫描器集成
    CJMODScannerIntegration::initialize();
    
    // 演示各种场景
    demonstratePowerOperator();
    demonstrateINeverAwayFunction();
    demonstratePrintMyloveFunction();
    demonstrateMixedSyntax();
    demonstrateDualPointerConcept();
    demonstrateKeywordRegistration();
    
    std::cout << "\n💝 珂朵莉的总结" << std::endl;
    std::cout << "✅ 方案1（回退重拼接）：适合处理中缀操作符如 **" << std::endl;
    std::cout << "✅ 方案2（双指针扫描器）：适合处理函数调用如 iNeverAway" << std::endl;
    std::cout << "✅ 混合方案：根据关键字类型选择最适合的处理策略" << std::endl;
    std::cout << "✅ 关键字注册系统：灵活配置不同关键字的处理方式" << std::endl;
    std::cout << "✅ 统一扫描器集成：无缝对接CJMOD和统一扫描器" << std::endl;
    std::cout << "\n珂朵莉永远在大家心中 ❀" << std::endl;
    
    return 0;
}