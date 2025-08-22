#include <iostream>
#include <cassert>
#include <memory>
#include "CHTLJS/Core/CHTLJSToken.h"
#include "CHTLJS/Core/CHTLJSState.h"
#include "CHTLJS/Lexer/CHTLJSLexer.h"
#include "CHTLJS/Parser/CHTLJSParser.h"
#include "CHTLJS/Generator/CHTLJSGenerator.h"

using namespace CHTL::CHTLJS;

// 测试增强选择器解析和生成
void test_enhanced_selector_parsing() {
    std::cout << "测试增强选择器解析和生成...\n";
    
    std::string source = "{{.box}}";
    
    // 词法分析
    Lexer::CHTLJSLexer lexer;
    auto tokens = lexer.Tokenize(source, "selector_test.chtljs");
    
    // 语法分析
    Core::CHTLJSState stateManager;
    Parser::CHTLJSParser parser(stateManager);
    auto ast = parser.Parse(tokens, "selector_test.chtljs");
    
    assert(ast != nullptr);
    assert(ast->GetChildCount() == 1);
    
    // 验证AST结构
    auto child = ast->GetChildren()[0];
    auto selectorNode = std::dynamic_pointer_cast<AST::EnhancedSelectorNode>(child);
    assert(selectorNode != nullptr);
    assert(selectorNode->GetSelector() == "{{.box}}");
    assert(selectorNode->GetSelectorType() == AST::EnhancedSelectorNode::SelectorType::CLASS);
    assert(selectorNode->GetParsedSelector() == "box");
    
    // 代码生成
    Generator::CHTLJSGenerator generator;
    std::string jsCode = generator.Generate(ast);
    
    assert(!jsCode.empty());
    assert(jsCode.find("document.querySelectorAll('.box')") != std::string::npos);
    
    std::cout << "  增强选择器解析和生成测试通过\n";
    std::cout << "  生成的JavaScript: " << jsCode << "\n";
}

// 测试虚对象解析和生成
void test_virtual_object_parsing() {
    std::cout << "测试虚对象解析和生成...\n";
    
    std::string source = R"(
vir eventHandler = listen({
    click: () => {
        console.log('clicked');
    }
});
)";
    
    // 词法分析
    Lexer::CHTLJSLexer lexer;
    auto tokens = lexer.Tokenize(source, "virtual_test.chtljs");
    
    // 语法分析
    Core::CHTLJSState stateManager;
    Parser::CHTLJSParser parser(stateManager);
    auto ast = parser.Parse(tokens, "virtual_test.chtljs");
    
    assert(ast != nullptr);
    
    // 代码生成
    Generator::CHTLJSGenerator generator;
    std::string jsCode = generator.Generate(ast);
    
    assert(!jsCode.empty());
    assert(jsCode.find("function CHTLJS_") != std::string::npos);
    
    std::cout << "  虚对象解析和生成测试通过\n";
    std::cout << "  生成的JavaScript长度: " << jsCode.length() << " 字符\n";
}

// 测试监听器解析和生成
void test_listen_block_parsing() {
    std::cout << "测试监听器块解析和生成...\n";
    
    std::string source = R"(
listen({
    click: () => {
        console.log('click event');
    },
    mouseenter: function() {
        console.log('mouse enter');
    }
});
)";
    
    // 词法分析
    Lexer::CHTLJSLexer lexer;
    auto tokens = lexer.Tokenize(source, "listen_test.chtljs");
    
    // 语法分析
    Core::CHTLJSState stateManager;
    Parser::CHTLJSParser parser(stateManager);
    auto ast = parser.Parse(tokens, "listen_test.chtljs");
    
    assert(ast != nullptr);
    
    // 验证AST结构
    auto child = ast->GetChildren()[0];
    auto listenNode = std::dynamic_pointer_cast<AST::ListenBlockNode>(child);
    assert(listenNode != nullptr);
    assert(listenNode->GetEventHandlers().size() == 2);
    
    // 代码生成
    Generator::CHTLJSGenerator generator;
    std::string jsCode = generator.Generate(ast);
    
    assert(!jsCode.empty());
    assert(jsCode.find("addEventListener") != std::string::npos);
    
    std::cout << "  监听器块解析和生成测试通过\n";
}

// 测试动画解析和生成
void test_animate_block_parsing() {
    std::cout << "测试动画块解析和生成...\n";
    
    std::string source = R"(
animate({
    target: {{.box}},
    duration: 1000,
    begin: {
        opacity: 0
    },
    end: {
        opacity: 1
    }
});
)";
    
    // 词法分析
    Lexer::CHTLJSLexer lexer;
    auto tokens = lexer.Tokenize(source, "animate_test.chtljs");
    
    // 语法分析
    Core::CHTLJSState stateManager;
    Parser::CHTLJSParser parser(stateManager);
    auto ast = parser.Parse(tokens, "animate_test.chtljs");
    
    assert(ast != nullptr);
    
    // 验证AST结构
    auto child = ast->GetChildren()[0];
    auto animateNode = std::dynamic_pointer_cast<AST::AnimateBlockNode>(child);
    assert(animateNode != nullptr);
    assert(animateNode->GetDuration() == 1000);
    
    // 代码生成
    Generator::CHTLJSGenerator generator;
    std::string jsCode = generator.Generate(ast);
    
    assert(!jsCode.empty());
    assert(jsCode.find("requestAnimationFrame") != std::string::npos);
    
    std::cout << "  动画块解析和生成测试通过\n";
}

// 测试复杂CHTL JS代码的完整编译
void test_complex_chtljs_compilation() {
    std::cout << "测试复杂CHTL JS代码完整编译...\n";
    
    std::string source = R"(
vir uiController = listen({
    handleClick: () => {
        {{.button}}->animate({
            duration: 300,
            begin: { transform: 'scale(1.0)' },
            end: { transform: 'scale(1.1)' }
        });
    }
});

{{document}}->delegate({
    target: {{.button}},
    click: uiController->handleClick
});

const fadeAnimation = animate({
    target: [{{.card}}, {{#main}}],
    duration: 500
});
)";
    
    try {
        // 词法分析
        Lexer::CHTLJSLexer lexer;
        auto tokens = lexer.Tokenize(source, "complex_test.chtljs");
        assert(tokens.Size() > 0);
        
        // 语法分析
        Core::CHTLJSState stateManager;
        Parser::CHTLJSParser parser(stateManager);
        auto ast = parser.Parse(tokens, "complex_test.chtljs");
        assert(ast != nullptr);
        
        // 代码生成
        Generator::CHTLJSGenerator generator;
        std::string jsCode = generator.Generate(ast);
        assert(!jsCode.empty());
        
        // 验证生成的JavaScript包含预期内容
        assert(jsCode.find("CHTL JS全局函数") != std::string::npos);
        assert(jsCode.find("document.querySelectorAll") != std::string::npos);
        assert(jsCode.find("addEventListener") != std::string::npos);
        assert(jsCode.find("requestAnimationFrame") != std::string::npos);
        
        std::cout << "  复杂CHTL JS编译测试通过\n";
        
        // 输出生成的JavaScript（如果不太长）
        if (jsCode.length() < 2000) {
            std::cout << "\n生成的JavaScript:\n" << jsCode << "\n";
        } else {
            std::cout << "  生成的JavaScript长度: " << jsCode.length() << " 字符\n";
        }
        
        // 输出统计信息
        std::cout << "\n" << generator.GetStatistics() << "\n";
        
    } catch (const std::exception& e) {
        std::cerr << "复杂编译测试异常: " << e.what() << std::endl;
        assert(false);
    }
}

// 测试方法调用链解析
void test_method_chain_parsing() {
    std::cout << "测试方法调用链解析...\n";
    
    std::string source = "{{.box}}->listen({click: () => {}});";
    
    // 词法分析
    Lexer::CHTLJSLexer lexer;
    auto tokens = lexer.Tokenize(source, "chain_test.chtljs");
    
    // 语法分析
    Core::CHTLJSState stateManager;
    Parser::CHTLJSParser parser(stateManager);
    auto ast = parser.Parse(tokens, "chain_test.chtljs");
    
    assert(ast != nullptr);
    
    // 代码生成
    Generator::CHTLJSGenerator generator;
    std::string jsCode = generator.Generate(ast);
    
    assert(!jsCode.empty());
    assert(jsCode.find("document.querySelectorAll('.box')") != std::string::npos);
    
    std::cout << "  方法调用链解析测试通过\n";
}

int main() {
    std::cout << "运行CHTL JS Parser和Generator测试...\n\n";
    
    try {
        test_enhanced_selector_parsing();
        test_virtual_object_parsing();
        test_listen_block_parsing();
        test_animate_block_parsing();
        test_method_chain_parsing();
        test_complex_chtljs_compilation();
        
        std::cout << "\n所有CHTL JS Parser和Generator测试通过!\n";
        std::cout << "✅ 增强选择器解析正常\n";
        std::cout << "✅ 虚对象解析正常\n";
        std::cout << "✅ 监听器块解析正常\n";
        std::cout << "✅ 动画块解析正常\n";
        std::cout << "✅ 方法调用链解析正常\n";
        std::cout << "✅ 复杂CHTL JS编译正常\n";
        
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "测试失败: " << e.what() << std::endl;
        return 1;
    }
}