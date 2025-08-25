#include <iostream>
#include <cassert>
#include "CHTLJS/Lexer/Lexer.h"
#include "CHTLJS/Compiler/Context.h"
#include "CHTLJS/Compiler/GlobalMap.h"
#include "CHTLJS/Compiler/State.h"

using namespace CHTL;

void TestCHTLJSLexer() {
    std::cout << "\n=== 测试CHTL JS词法分析器 ===" << std::endl;
    
    JSCompiler::Lexer lexer;
    
    std::string testCode = R"(
vir Test = listen({
    click: () => {
        console.log("Clicked!");
    }
});

{{.box}}->addEventListener('click', () => {
    console.log('Box clicked!');
});

{{#myId}}->delegate({
    target: {{.item}},
    click: function(e) {
        console.log('Item clicked');
    }
});

const anim = animate({
    target: {{button[0]}},
    duration: 1000,
    easing: "ease-in-out"
});

Test->click();
)";
    
    auto tokens = lexer.Tokenize(testCode);
    
    std::cout << "生成了 " << tokens.size() << " 个token" << std::endl;
    
    // 显示前30个token
    for (size_t i = 0; i < std::min(tokens.size(), size_t(30)); ++i) {
        std::cout << tokens[i].ToString() << std::endl;
    }
    
    if (tokens.size() > 30) {
        std::cout << "... (还有 " << (tokens.size() - 30) << " 个token)" << std::endl;
    }
}

void TestCHTLJSGlobalMap() {
    std::cout << "\n=== 测试CHTL JS全局映射表 ===" << std::endl;
    
    JSCompiler::GlobalMap globalMap;
    
    // 注册虚对象
    JSCompiler::GlobalObject virObj{
        JSCompiler::GlobalObjectType::VIR_OBJECT,
        "Test",
        "",
        nullptr,
        "test.js",
        10, 1
    };
    
    bool r1 = globalMap.RegisterVirObject("Test", virObj);
    std::cout << "注册虚对象Test结果: " << r1 << std::endl;
    
    // 注册函数映射
    std::string funcName = globalMap.GenerateGlobalFunctionName();
    bool r2 = globalMap.RegisterFunctionMapping("Test", "click", funcName);
    std::cout << "注册函数映射Test.click -> " << funcName << " 结果: " << r2 << std::endl;
    
    // 注册事件委托
    bool r3 = globalMap.RegisterDelegate("body", ".item", "click");
    std::cout << "注册事件委托结果: " << r3 << std::endl;
    
    // 查找测试
    auto found = globalMap.FindVirObject("Test");
    if (found) {
        std::cout << "找到虚对象: " << found->name << std::endl;
    }
    
    std::string mapping = globalMap.FindFunctionMapping("Test", "click");
    std::cout << "找到函数映射: Test.click -> " << mapping << std::endl;
    
    globalMap.Dump();
}

void TestCHTLJSStateManager() {
    std::cout << "\n=== 测试CHTL JS状态管理器 ===" << std::endl;
    
    JSCompiler::StateManager stateManager;
    
    // 初始状态应该是GLOBAL
    assert(stateManager.GetCurrentState() == JSCompiler::CompileState::GLOBAL);
    
    // 测试状态转换
    {
        auto guard = stateManager.EnterState(JSCompiler::CompileState::IN_VIR_DECLARATION, "Test");
        stateManager.SetCurrentVirObject("Test");
        std::cout << "进入VIR声明状态，虚对象: " << stateManager.GetCurrentVirObject() << std::endl;
        
        {
            auto guard2 = stateManager.EnterState(JSCompiler::CompileState::IN_LISTEN_CALL);
            std::cout << "进入listen调用状态" << std::endl;
            
            {
                auto guard3 = stateManager.EnterState(JSCompiler::CompileState::IN_OBJECT_LITERAL);
                std::cout << "进入对象字面量状态" << std::endl;
            }
        }
    }
    
    assert(stateManager.GetCurrentState() == JSCompiler::CompileState::GLOBAL);
    std::cout << "回到全局状态" << std::endl;
    
    stateManager.DumpStateStack();
}

void TestCHTLJSContext() {
    std::cout << "\n=== 测试CHTL JS编译上下文 ===" << std::endl;
    
    JSCompiler::CompileContext context;
    
    // 设置配置
    JSCompiler::ContextConfig config;
    config.enableVirObjects = true;
    config.enableEventDelegation = true;
    config.debugMode = true;
    context.SetConfig(config);
    
    // 测试选择器解析
    std::cout << "\n选择器解析测试:" << std::endl;
    
    auto testSelector = [&](const std::string& sel) {
        auto info = context.ParseSelector(sel);
        std::cout << "选择器: " << sel << " -> ";
        std::cout << "类型: " << info.type << ", 值: " << info.value;
        if (!info.index.empty()) {
            std::cout << ", 索引: " << info.index;
        }
        std::cout << std::endl;
        std::cout << "查询代码: " << context.GenerateQuerySelector(info) << std::endl;
    };
    
    testSelector(".box");
    testSelector("#myId");
    testSelector("button[0]");
    testSelector(".parent .child");
    
    // 测试虚对象注册
    context.RegisterCurrentVirObject("MyVir");
    std::cout << "\n当前虚对象: " << context.GetCurrentVirObject() << std::endl;
    
    // 测试函数名生成
    std::cout << "生成的函数名: " << context.GenerateUniqueFunctionName() << std::endl;
    std::cout << "生成的函数名: " << context.GenerateUniqueFunctionName("custom") << std::endl;
    
    // 测试事件委托
    context.RegisterEventDelegate("body", ".item", "click");
    context.RegisterEventDelegate("body", ".item", "hover");
    
    std::cout << "\n生成的委托代码:" << std::endl;
    std::cout << context.GenerateDelegateCode() << std::endl;
    
    context.DumpContext();
}

int main() {
    std::cout << "开始测试CHTL JS编译器基础组件..." << std::endl;
    
    try {
        TestCHTLJSLexer();
        TestCHTLJSGlobalMap();
        TestCHTLJSStateManager();
        TestCHTLJSContext();
        
        std::cout << "\n所有测试通过！" << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "测试失败: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}