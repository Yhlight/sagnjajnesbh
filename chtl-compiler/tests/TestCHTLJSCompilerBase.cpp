#include <iostream>
#include <cassert>
#include "chtl_js/token/CHTLJSToken.h"
#include "chtl_js/lexer/CHTLJSLexer.h"
#include "chtl_js/state/CHTLJSState.h"
#include "chtl_js/context/CHTLJSContext.h"
#include "chtl_js/core/CHTLJSGlobalMap.h"
#include "utils/Logger.h"

using namespace chtl::chtljs;

void TestCHTLJSTokenTypes() {
    std::cout << "测试CHTL JS Token类型..." << std::endl;
    
    CHTLJSToken token1(CHTLJSTokenType::KEYWORD_VIR, "vir", CHTLJSTokenLocation(1, 1, 0, 3));
    assert(token1.GetType() == CHTLJSTokenType::KEYWORD_VIR);
    assert(token1.GetLexeme() == "vir");
    
    CHTLJSToken token2(CHTLJSTokenType::FUNCTION_LISTEN, "listen", CHTLJSTokenLocation(1, 1, 0, 6));
    assert(token2.GetType() == CHTLJSTokenType::FUNCTION_LISTEN);
    assert(token2.IsFunction());
    
    CHTLJSToken token3(CHTLJSTokenType::ENHANCED_SELECTOR_START, "{{", CHTLJSTokenLocation(1, 1, 0, 2));
    assert(token3.GetType() == CHTLJSTokenType::ENHANCED_SELECTOR_START);
    assert(token3.IsSelector());
    
    CHTLJSToken token4(CHTLJSTokenType::ARROW, "->", CHTLJSTokenLocation(1, 1, 0, 2));
    assert(token4.GetType() == CHTLJSTokenType::ARROW);
    assert(token4.IsOperator());
    
    std::cout << "CHTL JS Token类型测试通过!" << std::endl;
}

void TestCHTLJSLexer() {
    std::cout << "测试CHTL JS词法分析器..." << std::endl;
    
    CHTLJSLexer lexer;
    
    // 测试增强选择器
    std::string code1 = R"(
{{.box}}->addEventListener('click', () => {
    console.log('Clicked');
});

{{#main}}->style->color = "red";
{{button[0]}}->click();
)";
    
    lexer.Initialize(code1, "test1.chtljs");
    std::vector<CHTLJSToken> tokens = lexer.GetAllTokens();
    
    // 验证关键Token
    bool hasEnhancedSelector = false;
    bool hasArrow = false;
    bool hasClassSelector = false;
    bool hasIdSelector = false;
    bool hasIndexSelector = false;
    
    for (const auto& token : tokens) {
        if (token.GetType() == CHTLJSTokenType::ENHANCED_SELECTOR_START) {
            hasEnhancedSelector = true;
        }
        if (token.GetType() == CHTLJSTokenType::ARROW) {
            hasArrow = true;
        }
        if (token.GetType() == CHTLJSTokenType::SELECTOR_CLASS) {
            hasClassSelector = true;
        }
        if (token.GetType() == CHTLJSTokenType::SELECTOR_ID) {
            hasIdSelector = true;
        }
        if (token.GetType() == CHTLJSTokenType::SELECTOR_INDEX) {
            hasIndexSelector = true;
        }
    }
    
    assert(hasEnhancedSelector);
    assert(hasArrow);
    assert(hasClassSelector);
    assert(hasIdSelector);
    assert(hasIndexSelector);
    
    // 测试虚对象和CHTL JS函数
    std::string code2 = R"(
vir MyHandler = listen({
    click: () => {
        console.log("Clicked");
    },
    hover: () => {
        {{&}}->addClass('hovered');
    }
});

{{body}}->delegate({
    target: {{.dynamic-button}},
    click: MyHandler->click
});

const anim = animate({
    target: {{.box}},
    duration: 1000
});
)";
    
    lexer.Initialize(code2, "test2.chtljs");
    tokens = lexer.GetAllTokens();
    
    bool hasVir = false;
    bool hasListen = false;
    bool hasDelegate = false;
    bool hasAnimate = false;
    bool hasAmpersand = false;
    
    for (const auto& token : tokens) {
        if (token.GetType() == CHTLJSTokenType::KEYWORD_VIR) hasVir = true;
        if (token.GetType() == CHTLJSTokenType::FUNCTION_LISTEN) hasListen = true;
        if (token.GetType() == CHTLJSTokenType::FUNCTION_DELEGATE) hasDelegate = true;
        if (token.GetType() == CHTLJSTokenType::FUNCTION_ANIMATE) hasAnimate = true;
        if (token.GetType() == CHTLJSTokenType::SELECTOR_AMPERSAND) hasAmpersand = true;
    }
    
    assert(hasVir);
    assert(hasListen);
    assert(hasDelegate);
    assert(hasAnimate);
    assert(hasAmpersand);
    
    std::cout << "CHTL JS词法分析器测试通过!" << std::endl;
}

void TestCHTLJSStateMachine() {
    std::cout << "测试CHTL JS状态机..." << std::endl;
    
    CHTLJSStateMachine stateMachine;
    
    // 测试初始状态
    assert(stateMachine.GetCurrentState() == CHTLJSCompilerState::INITIAL);
    
    // 测试状态转换
    stateMachine.PushState(CHTLJSCompilerState::IN_SCRIPT_BLOCK);
    assert(stateMachine.GetCurrentState() == CHTLJSCompilerState::IN_SCRIPT_BLOCK);
    
    // 测试增强选择器状态
    stateMachine.PushState(CHTLJSCompilerState::IN_ENHANCED_SELECTOR);
    assert(stateMachine.IsInEnhancedSelector());
    assert(stateMachine.IsInSelectorContext());
    
    // 测试RAII状态管理
    {
        CHTLJSStateGuard guard(stateMachine, CHTLJSCompilerState::IN_VIRTUAL_OBJECT_DECLARATION, "MyHandler");
        assert(stateMachine.IsInVirtualObjectContext());
    }
    // 离开作用域后应该自动恢复
    assert(stateMachine.GetCurrentState() == CHTLJSCompilerState::IN_ENHANCED_SELECTOR);
    
    // 测试函数调用状态
    stateMachine.PopState(); // 退出增强选择器
    stateMachine.PushState(CHTLJSCompilerState::IN_FUNCTION_CALL);
    stateMachine.PushState(CHTLJSCompilerState::IN_LISTEN_CALL);
    assert(stateMachine.IsInCHTLJSFunctionCall());
    assert(stateMachine.IsInFunctionContext());
    
    std::cout << "CHTL JS状态机测试通过!" << std::endl;
}

void TestCHTLJSContext() {
    std::cout << "测试CHTL JS上下文..." << std::endl;
    
    CHTLJSContext context;
    
    // 测试作用域
    assert(context.GetCurrentScope() != nullptr);
    assert(context.GetCurrentScope()->type == CHTLJSScopeType::SCRIPT_BLOCK);
    
    // 测试虚对象定义
    context.DefineVirtualObject("MyHandler", "__chtljs_handler_1");
    assert(context.IsVirtualObjectDefined("MyHandler"));
    assert(context.GetVirtualObjectFunction("MyHandler") == "__chtljs_handler_1");
    
    // 测试选择器上下文
    context.EnterSelector("{{.box}}");
    assert(context.IsInSelector());
    assert(context.GetCurrentSelector() == "{{.box}}");
    context.ExitSelector();
    assert(!context.IsInSelector());
    
    // 测试调用链
    context.StartCallChain("{{.box}}");
    context.AddMethodToChain("style");
    context.AddMethodToChain("color");
    auto chain = context.GetCallChain();
    assert(chain.size() == 3);
    assert(chain[0] == "{{.box}}");
    assert(chain[1] == "style");
    assert(chain[2] == "color");
    
    // 测试函数上下文
    context.EnterFunction("listen");
    assert(context.IsInCHTLJSFunction());
    assert(context.GetCurrentFunction() == "listen");
    context.ExitFunction();
    
    // 测试对象字面量
    context.EnterObjectLiteral();
    assert(context.IsInObjectLiteral());
    context.SetCurrentObjectKey("click");
    assert(context.GetCurrentObjectKey() == "click");
    context.ExitObjectLiteral();
    
    // 测试事件处理器
    context.RegisterEventHandler("click", "__handler_click");
    assert(context.GetEventHandler("click") == "__handler_click");
    
    std::cout << "CHTL JS上下文测试通过!" << std::endl;
}

void TestCHTLJSGlobalMap() {
    std::cout << "测试CHTL JS全局映射表..." << std::endl;
    
    CHTLJSGlobalMap globalMap;
    
    // 测试内置函数
    assert(globalMap.IsCHTLJSFunction("listen"));
    assert(globalMap.IsCHTLJSFunction("delegate"));
    assert(globalMap.IsCHTLJSFunction("animate"));
    assert(globalMap.IsCHTLJSFunction("iNeverAway"));
    assert(globalMap.IsCHTLJSFunction("printMylove"));
    
    // 测试虚对象注册
    globalMap.RegisterVirtualObject("Test", nullptr);
    assert(globalMap.IsVirtualObject("Test"));
    
    // 测试选择器注册
    globalMap.RegisterSelector("{{.box}}", nullptr);
    auto selectors = globalMap.GetAllSelectors();
    assert(selectors.size() == 1);
    assert(selectors[0] == "{{.box}}");
    
    // 测试事件委托
    globalMap.RegisterEventDelegation("{{body}}", nullptr);
    assert(globalMap.HasEventDelegation("{{body}}"));
    
    // 测试调用链
    globalMap.StartCallChain("{{.element}}");
    globalMap.AddToCallChain("style");
    globalMap.AddToCallChain("display");
    auto chain = globalMap.GetCurrentCallChain();
    assert(chain.size() == 3);
    
    // 测试状态函数
    globalMap.RegisterStateFunction("Void", "A", "__void_a");
    assert(globalMap.ResolveStateFunction("Void", "A") == "__void_a");
    
    // 测试唯一函数名生成
    std::string name1 = globalMap.GenerateUniqueFunctionName("handler");
    std::string name2 = globalMap.GenerateUniqueFunctionName("handler");
    assert(name1 != name2);
    assert(name1.find("__chtljs_handler_") == 0);
    
    std::cout << "CHTL JS全局映射表测试通过!" << std::endl;
}

void TestCHTLJSIntegration() {
    std::cout << "测试CHTL JS集成..." << std::endl;
    
    // 创建一个CHTL JS代码片段
    std::string code = R"(
vir Button = listen({
    click: () => {
        {{.status}}->textContent = "Clicked!";
    },
    hover: () => {
        {{&}}->addClass('hover');
    }
});

{{.box}}->delegate({
    target: {{.btn}},
    click: Button->click
});

vir Test = iNeverAway({
    Void<A>: function(x, y) {
        return x + y;
    },
    Void<B>: function(x, y) {
        return x * y;
    }
});

Test->Void<A>(10, 20);
)";
    
    // 词法分析
    CHTLJSLexer lexer;
    lexer.Initialize(code, "integration.chtljs");
    
    // 状态机
    CHTLJSStateMachine stateMachine;
    stateMachine.PushState(CHTLJSCompilerState::IN_SCRIPT_BLOCK);
    
    // 上下文
    CHTLJSContext context;
    context.SetCurrentFile("integration.chtljs");
    
    // 全局映射表
    CHTLJSGlobalMap globalMap;
    
    // 处理Token流
    CHTLJSToken token = lexer.NextToken();
    int tokenCount = 0;
    bool hasVirButton = false;
    bool hasVirTest = false;
    bool hasINeverAway = false;
    
    while (token.GetType() != CHTLJSTokenType::END_OF_FILE) {
        tokenCount++;
        
        // 简单的Token识别
        if (token.GetType() == CHTLJSTokenType::KEYWORD_VIR) {
            CHTLJSToken next = lexer.PeekToken();
            if (next.GetType() == CHTLJSTokenType::IDENTIFIER) {
                if (next.GetLexeme() == "Button") hasVirButton = true;
                if (next.GetLexeme() == "Test") hasVirTest = true;
            }
        }
        
        if (token.GetType() == CHTLJSTokenType::FUNCTION_INEVERAWAY) {
            hasINeverAway = true;
        }
        
        token = lexer.NextToken();
    }
    
    assert(tokenCount > 0);
    assert(hasVirButton);
    assert(hasVirTest);
    assert(hasINeverAway);
    
    std::cout << "处理了 " << tokenCount << " 个CHTL JS Token" << std::endl;
    std::cout << "CHTL JS集成测试通过!" << std::endl;
}

int main() {
    // 设置日志级别
    chtl::utils::Logger::GetInstance().SetLevel(chtl::utils::LogLevel::INFO);
    
    std::cout << "运行CHTL JS编译器基础架构测试..." << std::endl;
    
    TestCHTLJSTokenTypes();
    TestCHTLJSLexer();
    TestCHTLJSStateMachine();
    TestCHTLJSContext();
    TestCHTLJSGlobalMap();
    TestCHTLJSIntegration();
    
    std::cout << "\n所有CHTL JS测试通过!" << std::endl;
    return 0;
}