#include "../src/common/Token.h"
#include "../src/common/GlobalMap.h"
#include "../src/common/State.h"
#include "../src/common/Context.h"
#include "../src/common/Lexer.h"
#include <iostream>
#include <cassert>

using namespace chtl;

void testToken() {
    std::cout << "=== 测试Token模块 ===" << std::endl;
    
    // 测试Token创建
    Token token1(TokenType::IDENTIFIER, "test", TokenPosition(1, 1, 0));
    assert(token1.type == TokenType::IDENTIFIER);
    assert(token1.value == "test");
    
    // 测试TokenFactory
    TokenFactory::initKeywordMap();
    Token token2 = TokenFactory::createToken("text", TokenPosition(1, 1, 0));
    assert(token2.type == TokenType::TEXT);
    
    Token token3 = TokenFactory::createToken("@Style", TokenPosition(1, 1, 0));
    assert(token3.type == TokenType::AT_STYLE);
    
    std::cout << "Token模块测试通过!" << std::endl;
}

void testGlobalMap() {
    std::cout << "=== 测试GlobalMap模块 ===" << std::endl;
    
    GlobalMap globalMap;
    
    // 测试符号添加
    bool result = globalMap.addSymbol("testSymbol", SymbolType::TEMPLATE_STYLE, "test_value");
    assert(result == true);
    
    // 测试符号查找
    auto symbol = globalMap.findSymbol("testSymbol");
    assert(symbol != nullptr);
    assert(symbol->name == "testSymbol");
    assert(symbol->type == SymbolType::TEMPLATE_STYLE);
    
    // 测试作用域管理
    globalMap.enterScope("testScope");
    globalMap.addSymbol("localSymbol", SymbolType::CUSTOM_ELEMENT, "local_value");
    globalMap.exitScope();
    
    // 测试命名空间
    globalMap.enterNamespace("testNamespace");
    std::string currentNs = globalMap.getCurrentNamespace();
    assert(currentNs == "testNamespace");
    globalMap.addSymbol("nsSymbol1", SymbolType::CUSTOM_ELEMENT, "ns_value1");
    globalMap.exitNamespace();
    
    // 测试命名空间合并
    bool merged = globalMap.mergeNamespace("testNamespace");
    assert(merged == true); // 应该合并现有命名空间
    globalMap.addSymbol("nsSymbol2", SymbolType::CUSTOM_STYLE, "ns_value2");
    
    // 验证两个符号都在同一命名空间中
    auto nsSymbol1 = globalMap.findSymbol("nsSymbol1");
    auto nsSymbol2 = globalMap.findSymbol("nsSymbol2");
    assert(nsSymbol1 != nullptr);
    assert(nsSymbol2 != nullptr);
    assert(nsSymbol1->namespace_path == "testNamespace");
    assert(nsSymbol2->namespace_path == "testNamespace");
    
    globalMap.exitNamespace();
    
    std::cout << "GlobalMap模块测试通过!" << std::endl;
}

void testState() {
    std::cout << "=== 测试State模块 ===" << std::endl;
    
    StateManager stateManager;
    
    // 测试初始状态
    assert(stateManager.getCurrentState() == CompilerState::IDLE);
    assert(stateManager.getCurrentContext() == SyntaxContext::GLOBAL);
    
    // 测试状态转换
    bool result = stateManager.transition(CompilerState::PARSING_CHTL, SyntaxContext::GLOBAL);
    assert(result == true);
    assert(stateManager.getCurrentState() == CompilerState::PARSING_CHTL);
    
    // 测试状态栈
    result = stateManager.pushState(CompilerState::PARSING_STYLE_BLOCK, SyntaxContext::LOCAL_STYLE);
    assert(result == true);
    assert(stateManager.getCurrentState() == CompilerState::PARSING_STYLE_BLOCK);
    
    result = stateManager.popState();
    assert(result == true);
    assert(stateManager.getCurrentState() == CompilerState::PARSING_CHTL);
    
    std::cout << "State模块测试通过!" << std::endl;
}

void testContext() {
    std::cout << "=== 测试Context模块 ===" << std::endl;
    
    CompilerContext context;
    
    // 测试文件和位置管理
    context.setCurrentFile("test.chtl");
    assert(context.getCurrentFile() == "test.chtl");
    
    context.setCurrentPosition(10, 20);
    auto pos = context.getCurrentPosition();
    assert(pos.first == 10);
    assert(pos.second == 20);
    
    // 测试作用域管理
    context.enterScope("testScope");
    std::string scope = context.getCurrentScope();
    assert(scope.find("testScope") != std::string::npos);
    context.exitScope();
    
    // 测试模板状态
    context.enterTemplate("TestTemplate");
    assert(context.isInTemplate() == true);
    assert(context.getCurrentTemplate() == "TestTemplate");
    context.exitTemplate();
    assert(context.isInTemplate() == false);
    
    // 测试错误报告
    context.reportError("测试错误");
    assert(context.hasErrors() == true);
    auto errors = context.getErrors();
    assert(errors.size() == 1);
    assert(errors[0].message == "测试错误");
    
    std::cout << "Context模块测试通过!" << std::endl;
}

void testLexer() {
    std::cout << "=== 测试Lexer模块 ===" << std::endl;
    
    CompilerContext context;
    Lexer lexer(context);
    
    // 测试基本词法分析
    std::string input = "text { \"Hello World\" }";
    lexer.setInput(input);
    
    Token token1 = lexer.nextToken();
    assert(token1.type == TokenType::TEXT);
    assert(token1.value == "text");
    
    Token token2 = lexer.nextToken();
    assert(token2.type == TokenType::LBRACE);
    
    Token token3 = lexer.nextToken();
    assert(token3.type == TokenType::STRING_LITERAL);
    assert(token3.value == "\"Hello World\"");
    
    Token token4 = lexer.nextToken();
    assert(token4.type == TokenType::RBRACE);
    
    Token token5 = lexer.nextToken();
    assert(token5.type == TokenType::EOF_TOKEN);
    
    // 测试CHTL特殊语法
    lexer.setInput("@Style {{ box }}");
    
    Token at_token = lexer.nextToken();
    assert(at_token.type == TokenType::AT_STYLE);
    
    Token double_brace = lexer.nextToken();
    assert(double_brace.type == TokenType::DOUBLE_LBRACE);
    
    std::cout << "Lexer模块测试通过!" << std::endl;
}

void testRAII() {
    std::cout << "=== 测试RAII管理 ===" << std::endl;
    
    CompilerContext context;
    
    // 测试ContextGuard
    {
        CHTL_CONTEXT_GUARD(context, "testScope");
        std::string scope = context.getCurrentScope();
        assert(scope.find("testScope") != std::string::npos);
    } // guard自动析构，应该退出作用域
    
    // 测试StateGuard
    {
        CHTL_STATE_GUARD(context, CompilerState::PARSING_CHTL, SyntaxContext::GLOBAL);
        assert(context.getStateManager().getCurrentState() == CompilerState::PARSING_CHTL);
    } // guard自动析构，应该恢复状态
    
    std::cout << "RAII管理测试通过!" << std::endl;
}

int main() {
    try {
        std::cout << "开始CHTL编译器基础模块测试..." << std::endl;
        
        testToken();
        testGlobalMap();
        testState();
        testContext();
        testLexer();
        testRAII();
        
        std::cout << std::endl << "=== 所有测试通过! ===" << std::endl;
        std::cout << "第一个Todo (完善Token、GlobalMap、State、Context及Lexer模块) 已完成!" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "测试失败: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}