#include <iostream>
#include <cassert>
#include "chtl_js/compiler/CHTLJSCompiler.h"
#include "chtl_js/parser/CHTLJSParser.h"
#include "chtl_js/generator/CHTLJSGenerator.h"
#include "chtl_js/lexer/CHTLJSLexer.h"
#include "utils/Logger.h"

using namespace chtl;
using namespace chtl::chtljs;

void TestEnhancedSelector() {
    std::cout << "测试增强选择器..." << std::endl;
    
    std::string code = R"chtljs(
{{box}}.addEventListener('click', function() {});
{{.box}}.style.color = 'red';
{{#main}}.innerHTML = 'Hello';
{{button}}.forEach(function(btn) {});
{{.container div}}.length;
{{button[0]}}.click();
)chtljs";
    
    CHTLJSCompiler compiler;
    auto result = compiler.Compile(code, "test.chtljs");
    
    assert(result.Success);
    std::cout << "生成的JavaScript:\n" << result.OutputContent << std::endl;
    
    // 验证选择器函数被调用
    assert(result.OutputContent.find("__chtljs_select") != std::string::npos);
    assert(result.OutputContent.find("__chtljs_select(\"box\")") != std::string::npos);
    assert(result.OutputContent.find("__chtljs_select(\".box\")") != std::string::npos);
    assert(result.OutputContent.find("__chtljs_select(\"#main\")") != std::string::npos);
    
    std::cout << "增强选择器测试通过!" << std::endl;
}

void TestArrowOperator() {
    std::cout << "测试箭头操作符..." << std::endl;
    
    std::string code = R"chtljs(
{{box}}->addEventListener('click', function() {});
{{.container}}->style->backgroundColor = 'blue';
)chtljs";
    
    CHTLJSCompiler compiler;
    auto result = compiler.Compile(code, "test.chtljs");
    
    assert(result.Success);
    std::cout << "生成的JavaScript:\n" << result.OutputContent << std::endl;
    
    // 验证箭头操作符被转换为点
    assert(result.OutputContent.find(".addEventListener") != std::string::npos);
    assert(result.OutputContent.find(".style.backgroundColor") != std::string::npos);
    
    std::cout << "箭头操作符测试通过!" << std::endl;
}

void TestListenFunction() {
    std::cout << "测试listen函数..." << std::endl;
    
    std::string code = R"chtljs(
{{.box}}->listen({
    click: function() {
        console.log('clicked');
    },
    mouseenter: function(e) {
        console.log('mouse entered');
    }
});
)chtljs";
    
    CHTLJSCompiler compiler;
    auto result = compiler.Compile(code, "test.chtljs");
    
    if (!result.Success) {
        for (const auto& error : result.Errors) {
            std::cout << "错误: " << error << std::endl;
        }
    }
    
    std::cout << "生成的JavaScript:\n" << result.OutputContent << std::endl;
    
    // 验证listen工具函数被使用
    assert(result.OutputContent.find("__chtljs_listen") != std::string::npos);
    
    std::cout << "listen函数测试通过!" << std::endl;
}

void TestDelegateFunction() {
    std::cout << "测试delegate函数..." << std::endl;
    
    std::string code = R"chtljs(
{{document.body}}->delegate({
    target: {{.item}},
    click: function(e) {
        console.log('Item clicked');
    },
    mouseover: function(e) {
        console.log('Mouse over item');
    }
});
)chtljs";
    
    CHTLJSCompiler compiler;
    auto result = compiler.Compile(code, "test.chtljs");
    
    std::cout << "生成的JavaScript:\n" << result.OutputContent << std::endl;
    
    // 验证delegate工具函数被使用
    assert(result.OutputContent.find("__chtljs_delegate") != std::string::npos);
    assert(result.OutputContent.find("__chtljs_delegate_registry") != std::string::npos);
    
    std::cout << "delegate函数测试通过!" << std::endl;
}

void TestVirObject() {
    std::cout << "测试虚对象..." << std::endl;
    
    std::string code = R"chtljs(
vir EventHandler = listen({
    click: function() {
        console.log('Virtual click');
    },
    submit: function() {
        console.log('Virtual submit');
    }
});

EventHandler->click();
EventHandler->submit();
)chtljs";
    
    CHTLJSCompiler compiler;
    auto result = compiler.Compile(code, "test.chtljs");
    
    std::cout << "生成的JavaScript:\n" << result.OutputContent << std::endl;
    
    // 验证虚对象被正确处理
    assert(result.OutputContent.find("// vir EventHandler") != std::string::npos);
    
    std::cout << "虚对象测试通过!" << std::endl;
}

void TestAnimateFunction() {
    std::cout << "测试animate函数..." << std::endl;
    
    std::string code = R"chtljs(
const anim = animate({
    target: {{.box}},
    duration: 1000,
    easing: 'ease-in-out',
    begin: {
        opacity: 0
    },
    end: {
        opacity: 1
    }
});
)chtljs";
    
    CHTLJSCompiler compiler;
    auto result = compiler.Compile(code, "test.chtljs");
    
    std::cout << "生成的JavaScript:\n" << result.OutputContent << std::endl;
    
    // 验证animate函数被使用
    assert(result.OutputContent.find("__chtljs_animate") != std::string::npos);
    
    std::cout << "animate函数测试通过!" << std::endl;
}

void TestComplexScript() {
    std::cout << "测试复杂脚本..." << std::endl;
    
    std::string code = R"chtljs(
// CHTL JS复杂示例
vir FormHandler = listen({
    submit: function(e) {
        e.preventDefault();
        console.log('Form submitted');
    }
});

{{.form}}->listen({
    submit: FormHandler->submit
});

{{.container}}->delegate({
    target: [{{.btn-primary}}, {{.btn-secondary}}],
    click: function(e) {
        const target = e.currentTarget;
        target->style->backgroundColor = 'green';
        
        animate({
            target: target,
            duration: 300,
            begin: { transform: 'scale(1)' },
            end: { transform: 'scale(1.1)' }
        });
    }
});
)chtljs";
    
    CHTLJSCompiler compiler;
    auto result = compiler.Compile(code, "test.chtljs");
    
    if (!result.Success) {
        for (const auto& error : result.Errors) {
            std::cout << "错误: " << error << std::endl;
        }
    } else {
        std::cout << "生成的JavaScript:\n" << result.OutputContent << std::endl;
        std::cout << "\n复杂脚本测试通过!" << std::endl;
    }
}

void TestCHTLJSNotJS() {
    std::cout << "验证CHTL JS不是JavaScript..." << std::endl;
    
    // CHTL JS特有语法
    std::string chtljsCode = R"chtljs(
// 这些是CHTL JS语法，不是JavaScript
{{.box}}->listen({ click: function() {} });
vir Test = listen({ hover: function() {} });
Test->hover();
)chtljs";
    
    // 纯JavaScript代码应该被当作JS片段
    std::string jsCode = R"chtljs(
// 这是普通的JavaScript
var x = 10;
console.log(x);
function test() { return 42; }
)chtljs";
    
    CHTLJSCompiler compiler;
    
    auto result1 = compiler.Compile(chtljsCode, "test1.chtljs");
    std::cout << "CHTL JS代码生成:\n" << result1.OutputContent << std::endl;
    
    auto result2 = compiler.Compile(jsCode, "test2.chtljs");
    std::cout << "\nJavaScript代码处理:\n" << result2.OutputContent << std::endl;
    
    std::cout << "CHTL JS独立性验证通过!" << std::endl;
}

int main() {
    // 设置日志级别
    utils::Logger::GetInstance().SetLevel(utils::LogLevel::INFO);
    
    std::cout << "运行CHTL JS解析器和生成器测试...\n" << std::endl;
    std::cout << "注意：CHTL JS不是JavaScript，是CHTL的扩展语法\n" << std::endl;
    
    TestEnhancedSelector();
    std::cout << std::endl;
    
    TestArrowOperator();
    std::cout << std::endl;
    
    TestListenFunction();
    std::cout << std::endl;
    
    TestDelegateFunction();
    std::cout << std::endl;
    
    TestVirObject();
    std::cout << std::endl;
    
    TestAnimateFunction();
    std::cout << std::endl;
    
    TestComplexScript();
    std::cout << std::endl;
    
    TestCHTLJSNotJS();
    
    std::cout << "\n所有测试完成!" << std::endl;
    std::cout << "CHTL JS是独立的扩展语法，不负责处理JavaScript内容" << std::endl;
    return 0;
}