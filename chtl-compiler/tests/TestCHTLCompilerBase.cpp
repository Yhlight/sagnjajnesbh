#include <iostream>
#include <cassert>
#include "chtl/token/CHTLToken.h"
#include "chtl/lexer/CHTLLexer.h"
#include "chtl/state/CHTLState.h"
#include "chtl/context/CHTLContext.h"
#include "chtl/core/CHTLGlobalMap.h"
#include "utils/Logger.h"

using namespace chtl::compiler;

void TestTokenTypes() {
    std::cout << "测试Token类型..." << std::endl;
    
    CHTLToken token1(CHTLTokenType::IDENTIFIER, "div", TokenLocation(1, 1, 0, 3));
    assert(token1.GetType() == CHTLTokenType::IDENTIFIER);
    assert(token1.GetLexeme() == "div");
    assert(!token1.IsKeyword());
    
    CHTLToken token2(CHTLTokenType::KEYWORD_TEMPLATE, "[Template]", TokenLocation(1, 1, 0, 10));
    assert(token2.GetType() == CHTLTokenType::KEYWORD_TEMPLATE);
    assert(token2.IsKeyword());
    assert(token2.IsCHTLStructure());
    
    CHTLToken token3(CHTLTokenType::STRING_LITERAL, "\"hello\"", std::string("hello"), TokenLocation(1, 1, 0, 7));
    assert(token3.GetType() == CHTLTokenType::STRING_LITERAL);
    assert(token3.IsLiteral());
    
    std::cout << "Token类型测试通过!" << std::endl;
}

void TestLexer() {
    std::cout << "测试词法分析器..." << std::endl;
    
    CHTLLexer lexer;
    
    // 测试基本元素
    std::string code1 = R"(
div {
    id: "main";
    class: container;
    
    text {
        "Hello CHTL"
    }
}
)";
    
    lexer.Initialize(code1, "test1.chtl");
    std::vector<CHTLToken> tokens = lexer.GetAllTokens();
    
    // 验证一些关键Token
    bool hasDiv = false;
    bool hasId = false;
    bool hasText = false;
    bool hasString = false;
    
    for (const auto& token : tokens) {
        if (token.GetType() == CHTLTokenType::IDENTIFIER && token.GetLexeme() == "div") {
            hasDiv = true;
        }
        if (token.GetType() == CHTLTokenType::IDENTIFIER && token.GetLexeme() == "id") {
            hasId = true;
        }
        if (token.GetType() == CHTLTokenType::KEYWORD_TEXT) {
            hasText = true;
        }
        if (token.GetType() == CHTLTokenType::STRING_LITERAL) {
            hasString = true;
        }
    }
    
    assert(hasDiv);
    assert(hasId);
    assert(hasText);
    assert(hasString);
    
    // 测试关键字
    std::string code2 = R"(
[Template] @Style DefaultText {
    color: black;
}

[Custom] @Element Box {
    div { }
}

[Import] @Chtl from "module.chtl"
)";
    
    lexer.Initialize(code2, "test2.chtl");
    tokens = lexer.GetAllTokens();
    
    bool hasTemplate = false;
    bool hasCustom = false;
    bool hasImport = false;
    bool hasAtStyle = false;
    bool hasAtElement = false;
    bool hasAtChtl = false;
    
    for (const auto& token : tokens) {
        if (token.GetType() == CHTLTokenType::KEYWORD_TEMPLATE) hasTemplate = true;
        if (token.GetType() == CHTLTokenType::KEYWORD_CUSTOM) hasCustom = true;
        if (token.GetType() == CHTLTokenType::KEYWORD_IMPORT) hasImport = true;
        if (token.GetType() == CHTLTokenType::AT_STYLE) hasAtStyle = true;
        if (token.GetType() == CHTLTokenType::AT_ELEMENT) hasAtElement = true;
        if (token.GetType() == CHTLTokenType::AT_CHTL) hasAtChtl = true;
    }
    
    assert(hasTemplate);
    assert(hasCustom);
    assert(hasImport);
    assert(hasAtStyle);
    assert(hasAtElement);
    assert(hasAtChtl);
    
    std::cout << "词法分析器测试通过!" << std::endl;
}

void TestStateMachine() {
    std::cout << "测试状态机..." << std::endl;
    
    CHTLStateMachine stateMachine;
    
    // 测试初始状态
    assert(stateMachine.GetCurrentState() == CHTLCompilerState::INITIAL);
    
    // 测试状态转换
    stateMachine.PushState(CHTLCompilerState::TOP_LEVEL);
    assert(stateMachine.GetCurrentState() == CHTLCompilerState::TOP_LEVEL);
    
    // 测试RAII状态管理
    {
        CHTLStateGuard guard(stateMachine, CHTLCompilerState::IN_ELEMENT, "div");
        assert(stateMachine.GetCurrentState() == CHTLCompilerState::IN_ELEMENT);
        assert(stateMachine.IsInElementContext());
    }
    // 离开作用域后应该自动恢复
    assert(stateMachine.GetCurrentState() == CHTLCompilerState::TOP_LEVEL);
    
    // 测试状态查询
    stateMachine.PushState(CHTLCompilerState::IN_TEMPLATE_DECLARATION);
    stateMachine.PushState(CHTLCompilerState::IN_TEMPLATE_STYLE);
    assert(stateMachine.IsInTemplateContext());
    
    std::cout << "状态机测试通过!" << std::endl;
}

void TestContext() {
    std::cout << "测试上下文..." << std::endl;
    
    CHTLContext context;
    
    // 测试作用域
    assert(context.GetCurrentScope() != nullptr);
    assert(context.GetCurrentScope()->type == CHTLScopeType::GLOBAL);
    
    // 测试符号定义
    context.DefineSymbol("myVar", "value1");
    assert(context.IsSymbolDefined("myVar"));
    assert(context.GetSymbolValue("myVar") == "value1");
    
    // 测试RAII作用域管理
    {
        CHTLScopeGuard guard(context, CHTLScopeType::ELEMENT, "div");
        context.DefineSymbol("localVar", "value2");
        assert(context.IsSymbolDefined("localVar"));
    }
    // 离开作用域后局部符号应该不可见
    assert(!context.IsSymbolDefined("localVar"));
    assert(context.IsSymbolDefined("myVar")); // 全局符号仍然可见
    
    // 测试元素上下文
    context.SetCurrentElement("button");
    assert(context.GetCurrentElement() == "button");
    
    // 测试属性
    context.AddAttribute("id", "btn1");
    context.AddAttribute("class", "primary");
    auto attrs = context.GetCurrentAttributes();
    assert(attrs.size() == 2);
    assert(attrs[0].first == "id" && attrs[0].second == "btn1");
    
    // 测试样式上下文
    context.EnterStyleBlock(true); // 局部样式
    assert(context.IsInLocalStyleBlock());
    assert(!context.IsInGlobalStyleBlock());
    context.ExitStyleBlock();
    assert(!context.IsInLocalStyleBlock());
    
    std::cout << "上下文测试通过!" << std::endl;
}

void TestGlobalMap() {
    std::cout << "测试全局映射表..." << std::endl;
    
    CHTLGlobalMap globalMap;
    
    // 测试命名空间
    assert(globalMap.GetCurrentNamespace() == "");
    globalMap.EnterNamespace("myapp");
    assert(globalMap.GetCurrentNamespace() == "myapp");
    globalMap.EnterNamespace("components");
    assert(globalMap.GetCurrentNamespace() == "myapp.components");
    globalMap.ExitNamespace();
    assert(globalMap.GetCurrentNamespace() == "myapp");
    
    // 测试导入
    globalMap.RegisterImport("styles.css", "@Style");
    assert(globalMap.IsImported("styles.css"));
    
    // 测试别名
    globalMap.AddImportAlias("MyStyles", "styles.css");
    auto resolved = globalMap.ResolveAlias("MyStyles");
    assert(resolved.has_value());
    assert(resolved.value() == "styles.css");
    
    // 测试官方模块
    assert(globalMap.IsOfficialModule("chtl::core"));
    assert(!globalMap.IsOfficialModule("mymodule"));
    
    std::cout << "全局映射表测试通过!" << std::endl;
}

void TestIntegration() {
    std::cout << "测试集成..." << std::endl;
    
    // 创建一个简单的CHTL代码片段
    std::string code = R"(
[Template] @Style Theme {
    color: black;
    font-size: 16px;
}

div {
    style {
        @Style Theme;
        background: white;
    }
    
    text {
        "Welcome to CHTL"
    }
}
)";
    
    // 词法分析
    CHTLLexer lexer;
    lexer.Initialize(code, "integration.chtl");
    
    // 状态机
    CHTLStateMachine stateMachine;
    stateMachine.PushState(CHTLCompilerState::TOP_LEVEL);
    
    // 上下文
    CHTLContext context;
    context.SetCurrentFile("integration.chtl");
    
    // 全局映射表
    CHTLGlobalMap globalMap;
    
    // 处理Token流
    CHTLToken token = lexer.NextToken();
    int tokenCount = 0;
    
    while (token.GetType() != CHTLTokenType::END_OF_FILE) {
        tokenCount++;
        
        // 根据Token类型更新状态
        if (token.GetType() == CHTLTokenType::KEYWORD_TEMPLATE) {
            stateMachine.PushState(CHTLCompilerState::IN_TEMPLATE_DECLARATION);
        } else if (token.GetType() == CHTLTokenType::IDENTIFIER && token.GetLexeme() == "div") {
            stateMachine.PushState(CHTLCompilerState::IN_ELEMENT, "div");
            context.SetCurrentElement("div");
        }
        
        token = lexer.NextToken();
    }
    
    assert(tokenCount > 0);
    std::cout << "处理了 " << tokenCount << " 个Token" << std::endl;
    
    std::cout << "集成测试通过!" << std::endl;
}

int main() {
    // 设置日志级别
    chtl::utils::Logger::GetInstance().SetLevel(chtl::utils::LogLevel::INFO);
    
    std::cout << "运行CHTL编译器基础架构测试..." << std::endl;
    
    TestTokenTypes();
    TestLexer();
    TestStateMachine();
    TestContext();
    TestGlobalMap();
    TestIntegration();
    
    std::cout << "\n所有测试通过!" << std::endl;
    return 0;
}