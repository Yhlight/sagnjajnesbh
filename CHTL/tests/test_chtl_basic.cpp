#include <iostream>
#include <cassert>
#include "Scanner/UnifiedScanner.h"
#include "CHTL/Lexer/Lexer.h"
#include "CHTL/Compiler/Context.h"
#include "CHTL/Compiler/GlobalMap.h"
#include "CHTL/Compiler/State.h"

using namespace CHTL;

void TestScanner() {
    std::cout << "\n=== 测试统一扫描器 ===" << std::endl;
    
    Scanner::UnifiedScanner scanner;
    
    std::string testCode = R"(
[Template] @Style DefaultText
{
    color: black;
    line-height: 1.6;
}

div
{
    style
    {
        .box
        {
            width: 100px;
        }
    }
    
    script
    {
        {{box}}->addEventListener('click', () => {
            console.log('Clicked!');
        });
    }
}
)";
    
    auto fragments = scanner.Scan(testCode);
    
    std::cout << "扫描到 " << fragments.size() << " 个代码片段：" << std::endl;
    for (const auto& fragment : fragments) {
        std::string typeName;
        switch (fragment.type) {
            case Scanner::FragmentType::CHTL: typeName = "CHTL"; break;
            case Scanner::FragmentType::CHTLJS: typeName = "CHTL JS"; break;
            case Scanner::FragmentType::CSS: typeName = "CSS"; break;
            case Scanner::FragmentType::JavaScript: typeName = "JavaScript"; break;
            case Scanner::FragmentType::HTML: typeName = "HTML"; break;
            case Scanner::FragmentType::Text: typeName = "Text"; break;
        }
        
        std::cout << "\n[" << typeName << "] (" 
                  << fragment.startLine << ":" << fragment.startColumn << " - "
                  << fragment.endLine << ":" << fragment.endColumn << ")" << std::endl;
        std::cout << "内容: " << fragment.content.substr(0, 50) 
                  << (fragment.content.length() > 50 ? "..." : "") << std::endl;
    }
}

void TestLexer() {
    std::cout << "\n=== 测试CHTL词法分析器 ===" << std::endl;
    
    Compiler::Lexer lexer;
    
    std::string testCode = R"(
[Template] @Style ThemeColor
{
    color: rgba(255, 192, 203, 1);
    background-color: yellow;
}

[Custom] @Element Box
{
    div
    {
        id: mybox;
        class: container;
        
        text
        {
            "Hello CHTL!"
        }
    }
}
)";
    
    auto tokens = lexer.Tokenize(testCode);
    
    std::cout << "生成了 " << tokens.size() << " 个token" << std::endl;
    
    // 显示前20个token
    for (size_t i = 0; i < std::min(tokens.size(), size_t(20)); ++i) {
        std::cout << tokens[i].ToString() << std::endl;
    }
    
    if (tokens.size() > 20) {
        std::cout << "... (还有 " << (tokens.size() - 20) << " 个token)" << std::endl;
    }
}

void TestGlobalMap() {
    std::cout << "\n=== 测试全局映射表 ===" << std::endl;
    
    Compiler::GlobalMap globalMap;
    
    // 注册一些全局对象
    Compiler::GlobalObject obj1{
        Compiler::GlobalObjectType::TEMPLATE_STYLE,
        "DefaultText",
        "",
        nullptr,
        "test.chtl",
        10, 1
    };
    
    Compiler::GlobalObject obj2{
        Compiler::GlobalObjectType::CUSTOM_ELEMENT,
        "Box",
        "ui",
        nullptr,
        "test.chtl",
        20, 1
    };
    
    bool r1 = globalMap.Register(obj1);
    bool r2 = globalMap.Register(obj2);
    std::cout << "注册obj1结果: " << r1 << std::endl;
    std::cout << "注册obj2结果: " << r2 << std::endl;
    
    // 测试查找
    std::cout << "开始查找DefaultText..." << std::endl;
    auto found = globalMap.Find("DefaultText", Compiler::GlobalObjectType::TEMPLATE_STYLE);
    if (found) {
        std::cout << "找到对象: " << found->GetQualifiedName() << std::endl;
    } else {
        std::cout << "未找到DefaultText" << std::endl;
    }
    
    // 测试命名空间查找  
    std::cout << "开始查找ui.Box..." << std::endl;
    found = globalMap.Find("Box", Compiler::GlobalObjectType::CUSTOM_ELEMENT, "ui");
    if (found) {
        std::cout << "找到命名空间对象: " << found->GetQualifiedName() << std::endl;
    } else {
        std::cout << "未找到ui.Box" << std::endl;
    }
    
    std::cout << "调用Dump函数..." << std::endl;
    globalMap.Dump();
}

void TestStateManager() {
    std::cout << "\n=== 测试状态管理器 ===" << std::endl;
    
    Compiler::StateManager stateManager;
    
    // 初始状态应该是GLOBAL
    assert(stateManager.GetCurrentState() == Compiler::CompileState::GLOBAL);
    
    // 测试RAII状态管理
    {
        auto guard = stateManager.EnterState(Compiler::CompileState::IN_TEMPLATE, "Style");
        assert(stateManager.GetCurrentState() == Compiler::CompileState::IN_TEMPLATE);
        std::cout << "进入Template状态" << std::endl;
        
        {
            auto guard2 = stateManager.EnterState(Compiler::CompileState::IN_STYLE_PROPERTIES);
            assert(stateManager.GetCurrentState() == Compiler::CompileState::IN_STYLE_PROPERTIES);
            std::cout << "进入样式属性状态" << std::endl;
        }
        
        assert(stateManager.GetCurrentState() == Compiler::CompileState::IN_TEMPLATE);
        std::cout << "退出样式属性状态" << std::endl;
    }
    
    assert(stateManager.GetCurrentState() == Compiler::CompileState::GLOBAL);
    std::cout << "回到全局状态" << std::endl;
    
    stateManager.DumpStateStack();
}

void TestContext() {
    std::cout << "\n=== 测试编译上下文 ===" << std::endl;
    
    Compiler::CompileContext context;
    
    // 设置配置
    Compiler::ContextConfig config;
    config.debugMode = true;
    config.indexInitialCount = 1;
    context.SetConfig(config);
    
    // 测试作用域
    {
        Compiler::ScopeGuard scope(&context, "test_scope");
        
        // 注册局部变量
        context.RegisterLocalVariable("myVar", nullptr);
        assert(context.FindLocalVariable("myVar") != nullptr);
        
        // 生成自动类名
        auto className = context.GenerateAutoClass();
        std::cout << "生成的自动类名: " << className << std::endl;
        context.RegisterAutoClass(className);
    }
    
    // 离开作用域后应该找不到局部变量
    assert(context.FindLocalVariable("myVar") == nullptr);
    
    // 测试错误管理
    context.AddError("测试错误", 10, 5);
    context.AddWarning("测试警告", 15, 10);
    
    assert(context.HasErrors());
    
    context.DumpContext();
}

int main() {
    std::cout << "开始测试CHTL编译器基础组件..." << std::endl;
    
    try {
        TestScanner();
        TestLexer();
        TestGlobalMap();
        TestStateManager();
        TestContext();
        
        std::cout << "\n所有测试通过！" << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "测试失败: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}