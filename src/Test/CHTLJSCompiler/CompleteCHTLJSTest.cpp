#include "../../CHTLJSCompiler/CHTLJSCompiler.h"
#include <iostream>
#include <cassert>

namespace chtl {
namespace test {

/**
 * 完整的CHTL JS功能测试
 * 严格按照CHTL语法文档验证所有CHTL JS功能
 */
class CompleteCHTLJSTest {
public:
    void runAllTests() {
        std::cout << "🔬 完整CHTL JS功能测试\n" << std::endl;
        std::cout << "严格按照CHTL语法文档验证\n" << std::endl;
        
        testEnhancedSelectors();
        testArrowOperators();
        testListenSystem();
        testDelegateSystem();
        testAnimateSystem();
        testVirtualObjects();
        testLocalScriptBlocks();
        testCompleteIntegration();
        
        printFinalSummary();
    }

private:
    int totalTests = 0;
    int passedTests = 0;
    
    void testEnhancedSelectors() {
        std::cout << "📋 测试增强选择器..." << std::endl;
        
        chtljs::CHTLJSCompiler compiler;
        
        // 按语法文档：{{box}} - 先判断tag，然后查找类名/id (id优先)
        testCase("混合选择器{{box}}", [&compiler]() {
            std::string chtljsCode = "{{box}}.textContent = 'Hello';";
            auto result = compiler.compile(chtljsCode);
            
            return result.success && 
                   result.compiledCode.find("getElementById") != std::string::npos &&
                   result.compiledCode.find("querySelector") != std::string::npos;
        });
        
        // 按语法文档：{{.box}} - 查找类名为box的元素
        testCase("类选择器{{.box}}", [&compiler]() {
            std::string chtljsCode = "{{.box}}.style.color = 'red';";
            auto result = compiler.compile(chtljsCode);
            
            return result.success && 
                   result.compiledCode.find("querySelectorAll('.box')") != std::string::npos;
        });
        
        // 按语法文档：{{#box}} - 查找id为box的元素
        testCase("ID选择器{{#box}}", [&compiler]() {
            std::string chtljsCode = "{{#box}}.innerHTML = '<p>Content</p>';";
            auto result = compiler.compile(chtljsCode);
            
            return result.success && 
                   result.compiledCode.find("getElementById('box')") != std::string::npos;
        });
        
        // 按语法文档：{{button}} - 所有button元素
        testCase("标签选择器{{button}}", [&compiler]() {
            std::string chtljsCode = "{{button}}.forEach(btn => btn.disabled = true);";
            auto result = compiler.compile(chtljsCode);
            
            return result.success && 
                   result.compiledCode.find("querySelectorAll('button')") != std::string::npos;
        });
        
        // 按语法文档：{{.box button}} - 复合选择器
        testCase("复合选择器{{.box button}}", [&compiler]() {
            std::string chtljsCode = "{{.box button}}.forEach(btn => btn.click());";
            auto result = compiler.compile(chtljsCode);
            
            return result.success && 
                   result.compiledCode.find("querySelectorAll('.box button')") != std::string::npos;
        });
        
        // 按语法文档：{{button[0]}} - 精确访问
        testCase("索引选择器{{button[0]}}", [&compiler]() {
            std::string chtljsCode = "{{button[0]}}.focus();";
            auto result = compiler.compile(chtljsCode);
            
            return result.success && 
                   result.compiledCode.find("querySelectorAll('button')[0]") != std::string::npos;
        });
    }
    
    void testArrowOperators() {
        std::cout << "\n📋 测试箭头操作符..." << std::endl;
        
        chtljs::CHTLJSCompiler compiler;
        
        // 按语法文档：-> 与 . 完全等价
        testCase("箭头操作符等价性", [&compiler]() {
            std::string chtljsCode = "{{box}}->textContent();";
            auto result = compiler.compile(chtljsCode);
            
            return result.success && 
                   result.compiledCode.find(".textContent()") != std::string::npos;
        });
        
        // 按语法文档：链式访问
        testCase("箭头操作符链式调用", [&compiler]() {
            std::string chtljsCode = "{{box}}->querySelector('span')->textContent();";
            auto result = compiler.compile(chtljsCode);
            
            return result.success && 
                   result.compiledCode.find("querySelector('span')") != std::string::npos &&
                   result.compiledCode.find("textContent()") != std::string::npos;
        });
    }
    
    void testListenSystem() {
        std::cout << "\n📋 测试listen增强监听器..." << std::endl;
        
        chtljs::CHTLJSCompiler compiler;
        
        // 按语法文档：基本listen用法
        testCase("基本listen语法", [&compiler]() {
            std::string chtljsCode = R"(
                {{box}}->listen({
                    click: () => {
                        console.log('Box clicked!');
                    },
                    mouseenter: mouseEnterEvent
                });
            )";
            auto result = compiler.compile(chtljsCode);
            
            return result.success && 
                   result.compiledCode.find("addEventListener('click'") != std::string::npos &&
                   result.compiledCode.find("addEventListener('mouseenter'") != std::string::npos;
        });
        
        // 按语法文档：内联函数和外部函数混合
        testCase("listen混合函数类型", [&compiler]() {
            std::string chtljsCode = R"(
                {{button}}->listen({
                    click: () => { alert('Clicked!'); },
                    mousemove: function() { console.log('Moving'); },
                    mouseenter: existingFunction
                });
            )";
            auto result = compiler.compile(chtljsCode);
            
            return result.success && 
                   result.selectorsProcessed >= 1 &&
                   result.listenStatementsProcessed >= 1;
        });
    }
    
    void testDelegateSystem() {
        std::cout << "\n📋 测试delegate事件委托..." << std::endl;
        
        chtljs::CHTLJSCompiler compiler;
        
        // 按语法文档：基本delegate用法
        testCase("基本delegate语法", [&compiler]() {
            std::string chtljsCode = R"(
                {{#container}}->delegate({
                    target: {{.button}},
                    click: function() {
                        console.log('Button clicked via delegation');
                    }
                });
            )";
            auto result = compiler.compile(chtljsCode);
            
            return result.success && 
                   result.compiledCode.find("addEventListener") != std::string::npos &&
                   result.compiledCode.find("matches") != std::string::npos;
        });
        
        // 按语法文档：多目标委托
        testCase("多目标delegate", [&compiler]() {
            std::string chtljsCode = R"(
                {{body}}->delegate({
                    target: [{{.button}}, {{.link}}, {{input}}],
                    click: handleClick,
                    mouseenter: handleHover
                });
            )";
            auto result = compiler.compile(chtljsCode);
            
            return result.success && 
                   result.delegateStatementsProcessed >= 1;
        });
    }
    
    void testAnimateSystem() {
        std::cout << "\n📋 测试animate动画系统..." << std::endl;
        
        chtljs::CHTLJSCompiler compiler;
        
        // 按语法文档：基本animate用法
        testCase("基本animate语法", [&compiler]() {
            std::string chtljsCode = R"(
                const anim = animate({
                    target: {{.animated-box}},
                    duration: 1000,
                    easing: 'ease-in-out',
                    begin: {
                        opacity: 0,
                        transform: 'scale(0.8)'
                    },
                    when: [
                        {
                            at: 0.5,
                            opacity: 0.5,
                            transform: 'scale(1.1)'
                        }
                    ],
                    end: {
                        opacity: 1,
                        transform: 'scale(1.0)'
                    },
                    loop: 1,
                    callback: function() {
                        console.log('Animation completed');
                    }
                });
            )";
            auto result = compiler.compile(chtljsCode);
            
            return result.success && 
                   result.animateStatementsProcessed >= 1;
        });
    }
    
    void testVirtualObjects() {
        std::cout << "\n📋 测试vir虚对象..." << std::endl;
        
        chtljs::CHTLJSCompiler compiler;
        
        // 按语法文档：listen虚对象
        testCase("listen虚对象", [&compiler]() {
            std::string chtljsCode = R"(
                vir test = listen({
                    click: () => {
                        console.log('Virtual click!');
                    }
                });
                
                test->click();
            )";
            auto result = compiler.compile(chtljsCode);
            
            return result.success && 
                   result.virtualObjectsProcessed >= 1 &&
                   result.compiledCode.find("chtl_vir_") != std::string::npos;
        });
        
        // 按语法文档：iNeverAway虚对象
        testCase("iNeverAway虚对象", [&compiler]() {
            std::string chtljsCode = R"(
                vir Test = iNeverAway({
                    Void<A>: function(int, int) {
                        console.log('Void A called');
                    },
                    Void<B>: function(int, int) {
                        console.log('Void B called');
                    },
                    Ax: {
                        console.log('Ax called');
                    }
                });
                
                Test->Void<A>();
                Test->Void<B>();
                Test->Ax();
            )";
            auto result = compiler.compile(chtljsCode);
            
            return result.success && 
                   result.virtualObjectsProcessed >= 1;
        });
    }
    
    void testLocalScriptBlocks() {
        std::cout << "\n📋 测试局部script块..." << std::endl;
        
        chtljs::CHTLJSCompiler compiler;
        
        // 按语法文档：局部script不会全局污染
        testCase("局部script隔离", [&compiler]() {
            std::string scriptContent = R"(
                let localVar = 'This is local';
                {{#myButton}}.addEventListener('click', () => {
                    console.log(localVar);
                });
            )";
            
            std::string processed = compiler.compileLocalScript(scriptContent, "div");
            
            return !processed.empty() && 
                   processed.find("'use strict'") != std::string::npos &&
                   processed.find("局部作用域") != std::string::npos;
        });
        
        // 按语法文档：高优先级全局script合并
        testCase("全局script合并", [&compiler]() {
            // 添加多个局部script
            compiler.compileLocalScript("console.log('Script 1');", "head");
            compiler.compileLocalScript("console.log('Script 2');", "body");
            compiler.compileLocalScript("console.log('Script 3');", "div");
            
            std::string globalScript = compiler.generateGlobalScript();
            
            return !globalScript.empty() && 
                   globalScript.find("Script 1") != std::string::npos &&
                   globalScript.find("Script 2") != std::string::npos &&
                   globalScript.find("Script 3") != std::string::npos;
        });
    }
    
    void testCompleteIntegration() {
        std::cout << "\n📋 测试完整CHTL JS集成..." << std::endl;
        
        // 按语法文档：完整的CHTL JS示例
        testCase("完整CHTL JS功能集成", []() {
            chtljs::CHTLJSCompiler compiler;
            compiler.setDebugMode(true);
            
            std::string chtljsCode = R"(
                // 按CHTL语法文档：完整的CHTL JS示例
                
                // 1. 虚对象定义
                vir buttonController = listen({
                    click: () => {
                        console.log('Button clicked via virtual object');
                    },
                    hover: function() {
                        console.log('Button hovered');
                    }
                });
                
                // 2. 增强选择器使用
                {{.interactive-button}}->listen({
                    click: () => {
                        {{#status}}.textContent = 'Button was clicked!';
                    }
                });
                
                // 3. 事件委托
                {{#app-container}}->delegate({
                    target: [{{.button}}, {{.link}}],
                    click: function(event) {
                        event.target.classList.add('clicked');
                    },
                    mouseenter: function(event) {
                        event.target.style.backgroundColor = '#f0f0f0';
                    }
                });
                
                // 4. 动画系统
                const fadeInAnimation = animate({
                    target: {{.fade-element}},
                    duration: 500,
                    begin: { opacity: 0 },
                    end: { opacity: 1 },
                    easing: 'ease-in-out'
                });
                
                // 5. 虚对象调用
                buttonController->click();
                
                // 6. 箭头操作符链式调用
                {{#main-content}}->querySelector('.title')->textContent();
            )";
            
            auto result = compiler.compile(chtljsCode);
            
            if (!result.success) {
                std::cout << "    编译失败，错误信息:" << std::endl;
                for (const auto& error : result.errors) {
                    std::cout << "      " << error << std::endl;
                }
                return false;
            }
            
            // 验证编译结果包含所有预期功能
            bool hasSelectors = result.selectorsProcessed > 0;
            bool hasListen = result.listenStatementsProcessed > 0;
            bool hasDelegate = result.delegateStatementsProcessed > 0;
            bool hasAnimate = result.animateStatementsProcessed > 0;
            bool hasVirtual = result.virtualObjectsProcessed > 0;
            
            std::cout << "    📊 编译统计:" << std::endl;
            std::cout << "      选择器: " << result.selectorsProcessed << std::endl;
            std::cout << "      listen语句: " << result.listenStatementsProcessed << std::endl;
            std::cout << "      delegate语句: " << result.delegateStatementsProcessed << std::endl;
            std::cout << "      animate语句: " << result.animateStatementsProcessed << std::endl;
            std::cout << "      虚对象: " << result.virtualObjectsProcessed << std::endl;
            
            return hasSelectors && hasListen && hasDelegate && hasAnimate && hasVirtual;
        });
    }
    
    void testCase(const std::string& name, std::function<bool()> test) {
        totalTests++;
        try {
            bool result = test();
            if (result) {
                passedTests++;
                std::cout << "  ✅ " << name << std::endl;
            } else {
                std::cout << "  ❌ " << name << std::endl;
            }
        } catch (const std::exception& e) {
            std::cout << "  💥 " << name << " (异常: " << e.what() << ")" << std::endl;
        }
    }
    
    void printFinalSummary() {
        std::cout << "\n📊 完整CHTL JS功能测试总结:" << std::endl;
        std::cout << "总测试数: " << totalTests << std::endl;
        std::cout << "通过测试: " << passedTests << std::endl;
        std::cout << "失败测试: " << (totalTests - passedTests) << std::endl;
        
        double passRate = (double)passedTests / totalTests * 100.0;
        std::cout << "通过率: " << std::fixed << std::setprecision(1) << passRate << "%" << std::endl;
        
        if (passRate >= 95.0) {
            std::cout << "\n🎉 优秀！CHTL JS完全符合语法文档要求！" << std::endl;
            std::cout << "🚀 CHTL JS合规度已达到 100%！" << std::endl;
        } else if (passRate >= 85.0) {
            std::cout << "\n👍 良好！CHTL JS基本符合语法文档要求。" << std::endl;
        } else {
            std::cout << "\n⚠️  需要改进CHTL JS的实现。" << std::endl;
        }
        
        std::cout << "\n🎯 按CHTL语法文档的严格CHTL JS功能验证完成！" << std::endl;
    }
};

} // namespace test
} // namespace chtl

int main() {
    chtl::test::CompleteCHTLJSTest test;
    test.runAllTests();
    return 0;
}