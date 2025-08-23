#include <iostream>
#include <cassert>
#include <memory>
#include "CHTLJS/Core/CHTLJSToken.h"
#include "CHTLJS/Core/CHTLJSState.h"
#include "CHTLJS/Lexer/CHTLJSLexer.h"

using namespace CHTL::CHTLJS;

// 测试CHTL JS Token系统
void test_chtljs_token_system() {
    std::cout << "测试CHTL JS Token系统...\n";
    
    // 测试基础Token创建
    Core::TokenPosition pos(1, 1, 0);
    Core::CHTLJSToken token(Core::TokenType::VIR, "vir", pos);
    
    assert(token.GetType() == Core::TokenType::VIR);
    assert(token.GetValue() == "vir");
    assert(token.GetLine() == 1);
    assert(token.GetColumn() == 1);
    assert(token.IsCHTLJSKeyword());
    assert(token.IsVirtualObjectToken());
    
    // 测试关键字识别
    assert(Core::CHTLJSToken::IsKeywordString("vir"));
    assert(Core::CHTLJSToken::IsKeywordString("listen"));
    assert(Core::CHTLJSToken::IsKeywordString("delegate"));
    assert(Core::CHTLJSToken::IsKeywordString("animate"));
    assert(Core::CHTLJSToken::IsKeywordString("iNeverAway"));
    assert(!Core::CHTLJSToken::IsKeywordString("notAKeyword"));
    
    // 测试Token类型获取
    assert(Core::CHTLJSToken::GetKeywordTokenType("vir") == Core::TokenType::VIR);
    assert(Core::CHTLJSToken::GetKeywordTokenType("listen") == Core::TokenType::LISTEN);
    assert(Core::CHTLJSToken::GetKeywordTokenType("delegate") == Core::TokenType::DELEGATE);
    
    std::cout << "  Token系统测试通过\n";
}

// 测试CHTL JS TokenStream
void test_chtljs_token_stream() {
    std::cout << "测试CHTL JS TokenStream...\n";
    
    Core::CHTLJSTokenStream stream;
    
    // 添加一些Token
    stream.AddToken(Core::CHTLJSToken(Core::TokenType::VIR, "vir", Core::TokenPosition(1, 1, 0)));
    stream.AddToken(Core::CHTLJSToken(Core::TokenType::IDENTIFIER, "test", Core::TokenPosition(1, 5, 4)));
    stream.AddToken(Core::CHTLJSToken(Core::TokenType::EQUAL, "=", Core::TokenPosition(1, 10, 9)));
    stream.AddToken(Core::CHTLJSToken(Core::TokenType::LISTEN, "listen", Core::TokenPosition(1, 12, 11)));
    
    assert(stream.Size() == 4);
    assert(!stream.IsAtEnd());
    
    // 测试Token遍历
    assert(stream.Current().GetType() == Core::TokenType::VIR);
    stream.Advance();
    assert(stream.Current().GetType() == Core::TokenType::IDENTIFIER);
    assert(stream.Current().GetValue() == "test");
    
    // 测试Peek
    assert(stream.Peek().GetType() == Core::TokenType::EQUAL);
    
    // 测试重置
    stream.Reset();
    assert(stream.Current().GetType() == Core::TokenType::VIR);
    
    std::cout << "  TokenStream测试通过\n";
}

// 测试CHTL JS词法分析器
void test_chtljs_lexer() {
    std::cout << "测试CHTL JS词法分析器...\n";
    
    Lexer::CHTLJSLexer lexer;
    
    // 测试增强选择器
    std::string source1 = "{{.box}}->listen({click: () => {}});";
    auto tokens1 = lexer.Tokenize(source1, "test1.chtljs");
    
    assert(tokens1.Size() > 0);
    tokens1.Reset();
    
    // 验证增强选择器Token
    assert(tokens1.Current().GetType() == Core::TokenType::ENHANCED_SELECTOR);
    assert(tokens1.Current().GetValue() == "{{.box}}");
    
    tokens1.Advance();
    assert(tokens1.Current().GetType() == Core::TokenType::ARROW);
    assert(tokens1.Current().GetValue() == "->");
    
    tokens1.Advance();
    assert(tokens1.Current().GetType() == Core::TokenType::LISTEN);
    assert(tokens1.Current().GetValue() == "listen");
    
    // 测试虚对象语法
    std::string source2 = "vir test = iNeverAway({Void<A>: function() {}});";
    auto tokens2 = lexer.Tokenize(source2, "test2.chtljs");
    
    assert(tokens2.Size() > 0);
    tokens2.Reset();
    
    assert(tokens2.Current().GetType() == Core::TokenType::VIR);
    tokens2.Advance();
    assert(tokens2.Current().GetType() == Core::TokenType::IDENTIFIER);
    assert(tokens2.Current().GetValue() == "test");
    
    std::cout << "  词法分析器测试通过\n";
}

// 测试CHTL JS状态管理器
void test_chtljs_state_manager() {
    std::cout << "测试CHTL JS状态管理器...\n";
    
    Core::CHTLJSState stateManager;
    
    // 测试初始状态
    assert(stateManager.GetCurrentState() == Core::CompileState::INITIAL);
    
    // 测试状态转换
    bool success = stateManager.TransitionTo(Core::CompileState::PARSING_VIRTUAL_OBJECT);
    assert(success);
    assert(stateManager.GetCurrentState() == Core::CompileState::PARSING_VIRTUAL_OBJECT);
    
    // 测试状态栈
    stateManager.PushState(Core::CompileState::PARSING_LISTEN_BLOCK);
    assert(stateManager.GetCurrentState() == Core::CompileState::PARSING_LISTEN_BLOCK);
    
    Core::CompileState poppedState = stateManager.PopState();
    assert(poppedState == Core::CompileState::PARSING_LISTEN_BLOCK);
    assert(stateManager.GetCurrentState() == Core::CompileState::PARSING_VIRTUAL_OBJECT);
    
    // 测试虚对象上下文
    stateManager.EnterVirtualObject("testObject");
    assert(stateManager.GetContext().currentVirtualObject == "testObject");
    assert(stateManager.GetContextVariable("current_virtual_object") == "testObject");
    
    stateManager.ExitVirtualObject();
    assert(stateManager.GetContext().currentVirtualObject == "");
    
    // 测试选择器上下文
    stateManager.EnterSelector(".box");
    assert(stateManager.GetContext().currentSelector == ".box");
    
    stateManager.ExitSelector();
    assert(stateManager.GetContext().currentSelector == "");
    
    // 测试状态重置
    stateManager.Reset();
    assert(stateManager.GetCurrentState() == Core::CompileState::INITIAL);
    
    std::cout << "  状态管理器测试通过\n";
}

// 测试CHTL JS状态保护（RAII）
void test_chtljs_state_guard() {
    std::cout << "测试CHTL JS状态保护...\n";
    
    Core::CHTLJSState stateManager;
    assert(stateManager.GetCurrentState() == Core::CompileState::INITIAL);
    
    {
        // 使用状态保护进入虚对象解析状态
        Core::CHTLJSStateGuard guard(stateManager, Core::CompileState::PARSING_VIRTUAL_OBJECT);
        assert(guard.IsValid());
        assert(stateManager.GetCurrentState() == Core::CompileState::PARSING_VIRTUAL_OBJECT);
        
        // 在作用域结束时应该自动恢复状态
    }
    
    // 验证状态已自动恢复
    assert(stateManager.GetCurrentState() == Core::CompileState::INITIAL);
    
    std::cout << "  状态保护测试通过\n";
}

// 测试复杂CHTL JS语法的词法分析
void test_complex_chtljs_syntax() {
    std::cout << "测试复杂CHTL JS语法词法分析...\n";
    
    Lexer::CHTLJSLexer lexer;
    
    // 测试完整的CHTL JS代码
    std::string complexSource = R"(
vir eventHandler = listen({
    click: () => {
        {{.box}}->animate({
            duration: 1000,
            begin: {
                opacity: 0
            },
            end: {
                opacity: 1
            }
        });
    },
    mouseenter: function() {
        console.log('Mouse entered');
    }
});

{{document}}->delegate({
    target: {{.button}},
    click: eventHandler->click
});

const anim = animate({
    target: [{{.card}}, {{#main}}],
    duration: 500,
    when: [
        {
            at: 0.5,
            transform: 'scale(1.1)'
        }
    ]
});
)";
    
    auto tokens = lexer.Tokenize(complexSource, "complex.chtljs");
    
    assert(tokens.Size() > 0);
    
    // 验证关键Token的识别
    tokens.Reset();
    bool foundVir = false;
    bool foundListen = false;
    bool foundEnhancedSelector = false;
    bool foundArrow = false;
    bool foundAnimate = false;
    bool foundDelegate = false;
    
    while (!tokens.IsAtEnd()) {
        const auto& token = tokens.Current();
        
        if (token.GetType() == Core::TokenType::VIR) {
            foundVir = true;
        } else if (token.GetType() == Core::TokenType::LISTEN) {
            foundListen = true;
        } else if (token.GetType() == Core::TokenType::ENHANCED_SELECTOR) {
            foundEnhancedSelector = true;
        } else if (token.GetType() == Core::TokenType::ARROW) {
            foundArrow = true;
        } else if (token.GetType() == Core::TokenType::ANIMATE) {
            foundAnimate = true;
        } else if (token.GetType() == Core::TokenType::DELEGATE) {
            foundDelegate = true;
        }
        
        tokens.Advance();
    }
    
    assert(foundVir);
    assert(foundListen);
    assert(foundEnhancedSelector);
    assert(foundArrow);
    assert(foundAnimate);
    assert(foundDelegate);
    
    std::cout << "  复杂语法词法分析测试通过\n";
    std::cout << "    找到vir关键字: " << foundVir << "\n";
    std::cout << "    找到listen关键字: " << foundListen << "\n";
    std::cout << "    找到增强选择器: " << foundEnhancedSelector << "\n";
    std::cout << "    找到箭头操作符: " << foundArrow << "\n";
    std::cout << "    找到animate关键字: " << foundAnimate << "\n";
    std::cout << "    找到delegate关键字: " << foundDelegate << "\n";
}

int main() {
    std::cout << "运行CHTL JS核心功能完整测试...\n\n";
    
    try {
        test_chtljs_token_system();
        test_chtljs_token_stream();
        test_chtljs_lexer();
        test_chtljs_state_manager();
        test_chtljs_state_guard();
        test_complex_chtljs_syntax();
        
        std::cout << "\n所有CHTL JS核心功能测试通过!\n";
        std::cout << "✅ Token系统正常\n";
        std::cout << "✅ TokenStream正常\n";
        std::cout << "✅ 词法分析器正常\n";
        std::cout << "✅ 状态管理器正常\n";
        std::cout << "✅ 状态保护正常\n";
        std::cout << "✅ 复杂语法识别正常\n";
        
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "测试失败: " << e.what() << std::endl;
        return 1;
    }
}