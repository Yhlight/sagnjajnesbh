#include <iostream>
#include <cassert>
#include "CHTL/Core/CHTLToken.h"
#include "CHTL/Core/CHTLGlobalMap.h"
#include "CHTL/Core/CHTLState.h"

using namespace CHTL::Core;

// 测试Token系统
void test_chtl_token_system() {
    std::cout << "测试CHTL Token系统...\n";
    
    // 测试Token创建
    CHTLToken token(TokenType::TEMPLATE, "[Template]", 1, 1, "test.chtl");
    assert(token.GetType() == TokenType::TEMPLATE);
    assert(token.GetValue() == "[Template]");
    assert(token.GetLine() == 1);
    assert(token.GetColumn() == 1);
    assert(token.GetFileName() == "test.chtl");
    
    // 测试Token类型检查
    assert(token.IsMarkerKeyword());
    assert(!token.IsLiteral());
    assert(!token.IsOperator());
    
    // 测试Token类型名称
    assert(CHTLToken::GetTokenTypeName(TokenType::TEMPLATE) == "TEMPLATE");
    
    // 测试从字符串获取Token类型
    assert(CHTLToken::GetTokenTypeFromString("[Template]") == TokenType::TEMPLATE);
    assert(CHTLToken::GetTokenTypeFromString("div") == TokenType::HTML_TAG);
    assert(CHTLToken::GetTokenTypeFromString(".box") == TokenType::CLASS_SELECTOR);
    assert(CHTLToken::GetTokenTypeFromString("#main") == TokenType::ID_SELECTOR);
    
    // 测试关键字检查
    assert(CHTLToken::IsKeywordString("text"));
    assert(CHTLToken::IsKeywordString("style"));
    assert(CHTLToken::IsKeywordString("[Template]"));
    assert(!CHTLToken::IsKeywordString("randomword"));
    
    // 测试HTML标签检查
    assert(CHTLToken::IsHTMLTagString("div"));
    assert(CHTLToken::IsHTMLTagString("span"));
    assert(!CHTLToken::IsHTMLTagString("notanelement"));
    
    std::cout << "  Token系统测试通过\n";
}

// 测试TokenStream
void test_token_stream() {
    std::cout << "测试Token流...\n";
    
    TokenStream stream;
    
    // 添加Token
    stream.AddToken(CHTLToken(TokenType::TEMPLATE, "[Template]"));
    stream.AddToken(CHTLToken(TokenType::AT_STYLE, "@Style"));
    stream.AddToken(CHTLToken(TokenType::IDENTIFIER, "TestStyle"));
    stream.AddToken(CHTLToken(TokenType::LEFT_BRACE, "{"));
    
    assert(stream.Size() == 4);
    assert(!stream.IsAtEnd());
    
    // 测试流操作
    assert(stream.Current().GetType() == TokenType::TEMPLATE);
    
    stream.Advance();
    assert(stream.Current().GetType() == TokenType::AT_STYLE);
    
    assert(stream.Peek().GetType() == TokenType::IDENTIFIER);
    assert(stream.Peek(2).GetType() == TokenType::LEFT_BRACE);
    
    stream.Retreat();
    assert(stream.Current().GetType() == TokenType::TEMPLATE);
    
    std::cout << "  Token流测试通过\n";
}

// 测试全局映射表
void test_global_map() {
    std::cout << "测试CHTL全局映射表...\n";
    
    CHTLGlobalMap globalMap;
    
    // 测试添加符号
    SymbolInfo templateStyle(SymbolType::TEMPLATE_STYLE, "DefaultButton");
    templateStyle.fileName = "test.chtl";
    templateStyle.line = 10;
    templateStyle.properties["background-color"] = "#007bff";
    templateStyle.properties["color"] = "white";
    
    assert(globalMap.AddSymbol(templateStyle));
    
    // 测试查找符号
    const SymbolInfo* found = globalMap.FindSymbol("DefaultButton");
    assert(found != nullptr);
    assert(found->type == SymbolType::TEMPLATE_STYLE);
    assert(found->name == "DefaultButton");
    assert(found->properties.at("background-color") == "#007bff");
    
    // 测试按类型查找符号
    const SymbolInfo* foundByType = globalMap.FindSymbolByType("DefaultButton", SymbolType::TEMPLATE_STYLE);
    assert(foundByType != nullptr);
    assert(foundByType->name == "DefaultButton");
    
    // 测试符号冲突检查
    SymbolInfo conflictSymbol(SymbolType::CUSTOM_ELEMENT, "DefaultButton");
    assert(globalMap.HasConflict("DefaultButton", SymbolType::CUSTOM_ELEMENT));
    
    // 测试命名空间
    NamespaceInfo ns("TestNamespace");
    ns.fileName = "test.chtl";
    ns.line = 1;
    
    assert(globalMap.AddNamespace(ns));
    
    const NamespaceInfo* foundNs = globalMap.FindNamespace("TestNamespace");
    assert(foundNs != nullptr);
    assert(foundNs->name == "TestNamespace");
    
    // 测试命名空间操作
    globalMap.SetCurrentNamespace("TestNamespace");
    assert(globalMap.GetCurrentNamespace() == "TestNamespace");
    
    // 测试配置
    ConfigurationInfo config("TestConfig");
    config.settings["DEBUG_MODE"] = "true";
    config.settings["INDEX_INITIAL_COUNT"] = "0";
    
    assert(globalMap.AddConfiguration(config));
    
    const ConfigurationInfo* foundConfig = globalMap.FindConfiguration("TestConfig");
    assert(foundConfig != nullptr);
    assert(foundConfig->settings.at("DEBUG_MODE") == "true");
    
    // 测试统计信息
    std::string stats = globalMap.GetStatistics();
    assert(!stats.empty());
    
    std::cout << "  全局映射表测试通过\n";
}

// 测试状态管理器
void test_state_manager() {
    std::cout << "测试CHTL状态管理器...\n";
    
    CHTLState stateManager;
    
    // 测试初始状态
    assert(stateManager.GetCurrentState() == CompileState::INITIAL);
    assert(CHTLState::GetStateName(CompileState::INITIAL) == "INITIAL");
    
    // 测试状态转换
    CHTLToken templateToken(TokenType::TEMPLATE, "[Template]");
    assert(stateManager.CanTransitionTo(CompileState::PARSING_TEMPLATE, templateToken));
    assert(stateManager.TransitionTo(CompileState::PARSING_TEMPLATE, templateToken));
    assert(stateManager.GetCurrentState() == CompileState::PARSING_TEMPLATE);
    
    // 测试状态上下文
    StateContext& context = stateManager.GetCurrentContext();
    context.templateName = "TestTemplate";
    context.nestingLevel = 1;
    
    assert(stateManager.GetCurrentContext().templateName == "TestTemplate");
    assert(stateManager.GetCurrentContext().nestingLevel == 1);
    
    // 测试状态栈操作
    StateContext newContext;
    newContext.elementType = "div";
    stateManager.PushState(CompileState::PARSING_HTML_ELEMENT, newContext);
    
    assert(stateManager.GetCurrentState() == CompileState::PARSING_HTML_ELEMENT);
    assert(stateManager.GetStackDepth() == 1);
    
    CompileState poppedState = stateManager.PopState();
    assert(poppedState == CompileState::PARSING_HTML_ELEMENT);
    assert(stateManager.GetCurrentState() == CompileState::PARSING_TEMPLATE);
    
    // 测试状态类型检查
    assert(CHTLState::IsParsingState(CompileState::PARSING_TEMPLATE));
    assert(!CHTLState::IsErrorState(CompileState::PARSING_TEMPLATE));
    assert(!CHTLState::IsFinalState(CompileState::PARSING_TEMPLATE));
    
    assert(CHTLState::IsErrorState(CompileState::ERROR_STATE));
    assert(CHTLState::IsFinalState(CompileState::FINISHED));
    
    // 测试RAII状态保护
    {
        StateGuard guard(stateManager, CompileState::PARSING_STYLE_BLOCK);
        assert(guard.IsValid());
        assert(stateManager.GetCurrentState() == CompileState::PARSING_STYLE_BLOCK);
        assert(stateManager.GetCurrentContext().inLocalStyleBlock);
    }
    // guard析构后应该自动恢复状态
    assert(stateManager.GetCurrentState() == CompileState::PARSING_TEMPLATE);
    
    // 测试状态历史
    assert(stateManager.GetStateHistory().size() > 0);
    
    // 测试统计信息
    std::string stats = stateManager.GetStatistics();
    assert(!stats.empty());
    
    std::cout << "  状态管理器测试通过\n";
}

// 测试全限定名称解析
void test_full_name_parsing() {
    std::cout << "测试全限定名称解析...\n";
    
    std::string namespaceName, symbolName;
    
    // 测试简单名称
    assert(CHTLGlobalMap::ParseFullName("TestSymbol", namespaceName, symbolName));
    assert(namespaceName.empty());
    assert(symbolName == "TestSymbol");
    
    // 测试全限定名称
    assert(CHTLGlobalMap::ParseFullName("MyNamespace.TestSymbol", namespaceName, symbolName));
    assert(namespaceName == "MyNamespace");
    assert(symbolName == "TestSymbol");
    
    // 测试嵌套命名空间
    assert(CHTLGlobalMap::ParseFullName("Level1.Level2.TestSymbol", namespaceName, symbolName));
    assert(namespaceName == "Level1.Level2");
    assert(symbolName == "TestSymbol");
    
    // 测试构建全限定名称
    assert(CHTLGlobalMap::BuildFullName("", "TestSymbol") == "TestSymbol");
    assert(CHTLGlobalMap::BuildFullName("MyNamespace", "TestSymbol") == "MyNamespace.TestSymbol");
    
    std::cout << "  全限定名称解析测试通过\n";
}

// 测试符号类型兼容性
void test_symbol_type_compatibility() {
    std::cout << "测试符号类型兼容性...\n";
    
    // 相同类型应该兼容
    assert(CHTLGlobalMap::IsCompatibleType(SymbolType::TEMPLATE_STYLE, SymbolType::TEMPLATE_STYLE));
    
    // 模板和自定义类型应该兼容
    assert(CHTLGlobalMap::IsCompatibleType(SymbolType::TEMPLATE_STYLE, SymbolType::CUSTOM_STYLE));
    assert(CHTLGlobalMap::IsCompatibleType(SymbolType::CUSTOM_STYLE, SymbolType::TEMPLATE_STYLE));
    assert(CHTLGlobalMap::IsCompatibleType(SymbolType::TEMPLATE_ELEMENT, SymbolType::CUSTOM_ELEMENT));
    assert(CHTLGlobalMap::IsCompatibleType(SymbolType::TEMPLATE_VAR, SymbolType::CUSTOM_VAR));
    
    // 不兼容的类型
    assert(!CHTLGlobalMap::IsCompatibleType(SymbolType::TEMPLATE_STYLE, SymbolType::TEMPLATE_ELEMENT));
    assert(!CHTLGlobalMap::IsCompatibleType(SymbolType::CUSTOM_VAR, SymbolType::ORIGIN_HTML));
    
    std::cout << "  符号类型兼容性测试通过\n";
}

int main() {
    std::cout << "运行CHTL核心功能测试...\n\n";
    
    try {
        test_chtl_token_system();
        test_token_stream();
        test_global_map();
        test_state_manager();
        test_full_name_parsing();
        test_symbol_type_compatibility();
        
        std::cout << "\n所有CHTL核心功能测试通过!\n";
        std::cout << "✅ Token系统运行正常\n";
        std::cout << "✅ 全局映射表运行正常\n";
        std::cout << "✅ 状态管理器运行正常\n";
        std::cout << "✅ RAII状态保护运行正常\n";
        
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "测试失败: " << e.what() << std::endl;
        return 1;
    }
}