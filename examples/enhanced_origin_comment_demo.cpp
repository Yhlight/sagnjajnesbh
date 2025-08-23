#include "CHTL/Enhanced/OriginAnywhereSystem.h"
#include <iostream>
#include <string>

using namespace CHTL::Enhanced;

void demonstrateOriginAnywhere() {
    std::cout << "\n========================================" << std::endl;
    std::cout << "[Origin]原始嵌入节点任意位置使用演示" << std::endl;
    std::cout << "核心理念：不应该被束缚，问题由开发者自己控制" << std::endl;
    std::cout << "========================================\n" << std::endl;
    
    auto& manager = getEnhancedFeatureManager();
    auto& originManager = manager.getOriginManager();
    
    // 演示1：在CHTL根级别使用Origin
    {
        std::cout << "📍 演示1：CHTL根级别使用Origin" << std::endl;
        
        OriginPosition position;
        position.line = 1;
        position.column = 1;
        position.context = ContextType::CHTL_ROOT;
        position.surroundingCode = "[Origin] @Style globalStyles;";
        position.nestingLevel = 0;
        
        bool canUse = originManager.canUseOriginAnywhere(position, OriginType::STYLE);
        std::string advice = originManager.getOriginUsageAdvice(position, OriginType::STYLE);
        
        std::cout << "✅ 可以使用: " << (canUse ? "是" : "否") << std::endl;
        std::cout << "💡 使用建议:\n" << advice << std::endl;
        std::cout << std::endl;
    }
    
    // 演示2：在Origin块内部再次使用Origin
    {
        std::cout << "📍 演示2：Origin块内部嵌套使用Origin" << std::endl;
        
        OriginPosition position;
        position.line = 5;
        position.column = 10;
        position.context = ContextType::ORIGIN_BLOCK;
        position.surroundingCode = "[Origin] @Html { [Origin] @JavaScript nestedScript; }";
        position.nestingLevel = 2;
        
        bool canUse = originManager.canUseOriginAnywhere(position, OriginType::JAVASCRIPT);
        std::string advice = originManager.getOriginUsageAdvice(position, OriginType::JAVASCRIPT);
        
        std::cout << "✅ 可以使用: " << (canUse ? "是" : "否") << std::endl;
        std::cout << "💡 使用建议:\n" << advice << std::endl;
        std::cout << std::endl;
    }
    
    // 演示3：在CSS块中使用HTML Origin（语义不常见但允许）
    {
        std::cout << "📍 演示3：CSS块中使用HTML Origin（语义提醒）" << std::endl;
        
        OriginPosition position;
        position.line = 10;
        position.column = 15;
        position.context = ContextType::CSS_BLOCK;
        position.surroundingCode = "style { [Origin] @Html htmlInCSS; }";
        position.nestingLevel = 1;
        
        bool canUse = originManager.canUseOriginAnywhere(position, OriginType::HTML);
        auto [isReasonable, semanticAdvice] = originManager.validateOriginSemantics(position, OriginType::HTML);
        
        std::cout << "✅ 可以使用: " << (canUse ? "是" : "否") << std::endl;
        std::cout << "🧠 语义合理: " << (isReasonable ? "是" : "否") << std::endl;
        std::cout << "⚠️  语义建议: " << semanticAdvice << std::endl;
        std::cout << std::endl;
    }
    
    // 演示4：自定义Origin类型注册和使用
    {
        std::cout << "📍 演示4：自定义Origin类型注册和使用" << std::endl;
        
        // 注册自定义类型
        originManager.registerCustomOriginType("@TypeScript", "TypeScript代码嵌入", 
            {ContextType::CHTL_SCRIPT, ContextType::JAVASCRIPT_BLOCK});
        
        originManager.registerCustomOriginType("@Sass", "Sass/SCSS样式嵌入",
            {ContextType::CHTL_STYLE, ContextType::CSS_BLOCK});
        
        // 获取所有支持的类型
        auto supportedTypes = originManager.getSupportedOriginTypes();
        std::cout << "🎯 支持的Origin类型:" << std::endl;
        for (const auto& [type, description] : supportedTypes) {
            std::cout << "  " << type << ": " << description << std::endl;
        }
        std::cout << std::endl;
        
        // 使用自定义类型
        OriginPosition position;
        position.line = 20;
        position.column = 5;
        position.context = ContextType::CHTL_SCRIPT;
        position.surroundingCode = "script { [Origin] @TypeScript tsCode; }";
        
        bool canUse = originManager.canUseOriginAnywhere(position, OriginType::CUSTOM_TYPE);
        std::cout << "✅ 自定义类型可以使用: " << (canUse ? "是" : "否") << std::endl;
        std::cout << std::endl;
    }
    
    // 演示5：获取最佳实践建议
    {
        std::cout << "📍 演示5：各种上下文的最佳实践建议" << std::endl;
        
        std::vector<ContextType> contexts = {
            ContextType::CHTL_ROOT,
            ContextType::CHTL_ELEMENT,
            ContextType::CHTL_STYLE,
            ContextType::CHTL_SCRIPT
        };
        
        for (auto context : contexts) {
            auto practices = originManager.getBestPractices(context);
            std::cout << "🎯 " << (context == ContextType::CHTL_ROOT ? "CHTL根上下文" :
                                 context == ContextType::CHTL_ELEMENT ? "CHTL元素内部" :
                                 context == ContextType::CHTL_STYLE ? "CHTL样式块" : "CHTL脚本块")
                      << " 最佳实践:" << std::endl;
            for (const auto& practice : practices) {
                std::cout << "  • " << practice << std::endl;
            }
            std::cout << std::endl;
        }
    }
}

void demonstrateContextualComments() {
    std::cout << "\n========================================" << std::endl;
    std::cout << "\"--\"注释上下文感知生成演示" << std::endl;
    std::cout << "核心理念：根据上下文生成不同编程语言的注释" << std::endl;
    std::cout << "========================================\n" << std::endl;
    
    auto& manager = getEnhancedFeatureManager();
    auto& commentGenerator = manager.getCommentGenerator();
    
    // 演示1：在不同上下文中的"--"注释转换
    {
        std::cout << "📍 演示1：不同上下文中的\"--\"注释转换" << std::endl;
        
        struct TestCase {
            std::string name;
            ContextType context;
            std::string commentContent;
        };
        
        std::vector<TestCase> testCases = {
            {"CHTL根上下文", ContextType::CHTL_ROOT, "这是CHTL根级别的注释"},
            {"HTML元素", ContextType::HTML_ELEMENT, "这是HTML元素中的注释"},
            {"CSS样式块", ContextType::CSS_BLOCK, "这是CSS样式块中的注释"},
            {"JavaScript代码块", ContextType::JAVASCRIPT_BLOCK, "这是JavaScript代码块中的注释"},
            {"CHTL JS块", ContextType::CHTL_JS_BLOCK, "这是CHTL JS块中的注释"},
            {"Origin块", ContextType::ORIGIN_BLOCK, "这是Origin块中的注释"}
        };
        
        for (const auto& testCase : testCases) {
            std::string result = commentGenerator.processDashComment(
                testCase.commentContent, testCase.context, false
            );
            std::cout << "🎯 " << testCase.name << ":" << std::endl;
            std::cout << "  输入: --" << testCase.commentContent << std::endl;
            std::cout << "  输出: " << result << std::endl;
            std::cout << std::endl;
        }
    }
    
    // 演示2：智能上下文检测
    {
        std::cout << "📍 演示2：智能上下文检测和注释转换" << std::endl;
        
        struct ContextDetectionTest {
            std::string name;
            std::string surroundingCode;
            size_t cursorPosition;
            std::string commentContent;
        };
        
        std::vector<ContextDetectionTest> tests = {
            {
                "在样式块中",
                "div { style { color: red; -- 这里是样式注释 } }",
                25, // 光标在"--"位置
                "这里是样式注释"
            },
            {
                "在Origin HTML块中",
                "[Origin] @Html { <div>content</div> -- HTML注释 }",
                35, // 光标在"--"位置
                "HTML注释"
            },
            {
                "在脚本块中",
                "div { script { console.log('test'); -- JS注释 } }",
                35, // 光标在"--"位置
                "JS注释"
            },
            {
                "在CHTL元素中",
                "div { class: 'test'; -- 元素注释 }",
                25, // 光标在"--"位置
                "元素注释"
            }
        };
        
        for (const auto& test : tests) {
            ContextType detectedContext = commentGenerator.detectCurrentContext(
                test.surroundingCode, test.cursorPosition
            );
            
            std::string transformedComment = commentGenerator.smartProcessDashComment(
                test.commentContent, test.surroundingCode, test.cursorPosition
            );
            
            std::cout << "🎯 " << test.name << ":" << std::endl;
            std::cout << "  代码: " << test.surroundingCode << std::endl;
            std::cout << "  检测上下文: " << static_cast<int>(detectedContext) << std::endl;
            std::cout << "  转换结果: " << transformedComment << std::endl;
            std::cout << std::endl;
        }
    }
    
    // 演示3：多行注释处理
    {
        std::cout << "📍 演示3：多行注释处理" << std::endl;
        
        std::string multiLineComment = "这是一个\n多行注释\n用于详细说明";
        
        std::vector<ContextType> contexts = {
            ContextType::CHTL_ROOT,
            ContextType::CSS_BLOCK,
            ContextType::JAVASCRIPT_BLOCK
        };
        
        for (auto context : contexts) {
            std::string result = commentGenerator.processDashComment(
                multiLineComment, context, true
            );
            std::cout << "🎯 多行注释在不同上下文中的转换:" << std::endl;
            std::cout << "  上下文: " << static_cast<int>(context) << std::endl;
            std::cout << "  结果: " << result << std::endl;
            std::cout << std::endl;
        }
    }
    
    // 演示4：获取支持的注释格式
    {
        std::cout << "📍 演示4：支持的注释格式总览" << std::endl;
        
        auto formats = commentGenerator.getSupportedCommentFormats();
        std::cout << "🎯 支持的注释格式:" << std::endl;
        for (const auto& [language, formatPair] : formats) {
            std::cout << "  " << language << ":" << std::endl;
            std::cout << "    单行: " << formatPair.first << std::endl;
            std::cout << "    多行: " << formatPair.second << std::endl;
        }
        std::cout << std::endl;
    }
    
    // 演示5：注释示例获取
    {
        std::cout << "📍 演示5：各上下文注释示例" << std::endl;
        
        std::vector<ContextType> contexts = {
            ContextType::HTML_ELEMENT,
            ContextType::CSS_BLOCK,
            ContextType::JAVASCRIPT_BLOCK,
            ContextType::CHTL_JS_BLOCK,
            ContextType::CHTL_ROOT
        };
        
        for (auto context : contexts) {
            std::string example = commentGenerator.getCommentExample(context);
            std::cout << "🎯 上下文 " << static_cast<int>(context) << " 注释示例: " << example << std::endl;
        }
        std::cout << std::endl;
    }
}

void demonstrateRealWorldUsage() {
    std::cout << "\n========================================" << std::endl;
    std::cout << "真实世界应用场景演示" << std::endl;
    std::cout << "========================================\n" << std::endl;
    
    auto& manager = getEnhancedFeatureManager();
    
    // 模拟一个复杂的CHTL文件
    std::string chtlCode = R"(
[Namespace] MyApp

// 这是CHTL根级别注释
[Origin] @Style globalStyles {
    -- 这是CSS上下文中的注释
    * {
        box-sizing: border-box;
    }
}

html {
    head {
        title { text: "Enhanced CHTL Demo"; }
        
        [Origin] @Html metaTags {
            -- 这是HTML上下文中的注释
            <meta charset="UTF-8">
            <meta name="viewport" content="width=device-width, initial-scale=1.0">
        }
    }
    
    body {
        div {
            class: "container";
            
            -- 这是CHTL元素中的注释
            
            h1 { text: "欢迎使用增强的CHTL"; }
            
            div {
                class: "demo-section";
                
                [Origin] @Html complexHTML {
                    -- 在Origin HTML块中的注释
                    <div class="card">
                        <h2>Origin任意位置使用</h2>
                        <p>不应该被束缚，问题由开发者自己控制</p>
                    </div>
                }
                
                style {
                    -- 样式块中的注释
                    .card {
                        padding: 20px;
                        border: 1px solid #ddd;
                        border-radius: 8px;
                    }
                    
                    [Origin] @Style advancedCSS {
                        -- Origin样式块中的注释
                        .card:hover {
                            transform: translateY(-2px);
                            box-shadow: 0 4px 8px rgba(0,0,0,0.1);
                        }
                    }
                }
                
                script {
                    -- 脚本块中的注释
                    console.log('CHTL增强功能演示');
                    
                    [Origin] @JavaScript enhancedJS {
                        -- Origin JavaScript块中的注释
                        class CHTLDemo {
                            constructor() {
                                this.initDemo();
                            }
                            
                            initDemo() {
                                console.log('增强功能已启用');
                            }
                        }
                        
                        new CHTLDemo();
                    }
                    
                    // 使用CHTL JS功能
                    {{&}}->listen({
                        click: () => {
                            -- CHTL JS上下文中的注释
                            console.log('CHTL JS事件处理');
                        }
                    });
                }
            }
        }
        
        [Origin] @Vue vueComponent {
            -- Vue组件中的注释
            <template>
                <div class="vue-demo">
                    <h3>Vue组件集成</h3>
                </div>
            </template>
        }
        
        [Origin] @React reactComponent {
            -- React组件中的注释
            function ReactDemo() {
                return (
                    <div className="react-demo">
                        <h3>React组件集成</h3>
                    </div>
                );
            }
        }
    }
}
)";
    
    std::cout << "🔄 处理复杂的CHTL代码..." << std::endl;
    std::cout << "原始代码长度: " << chtlCode.length() << " 字符" << std::endl;
    
    // 处理代码
    std::string processedCode = manager.processCode(chtlCode);
    
    std::cout << "\n✅ 代码处理完成！" << std::endl;
    std::cout << "处理后代码长度: " << processedCode.length() << " 字符" << std::endl;
    
    // 显示转换前后的部分对比
    std::cout << "\n📋 \"--\"注释转换示例对比:" << std::endl;
    std::cout << "转换前: -- 这是CSS上下文中的注释" << std::endl;
    std::cout << "转换后: /* 这是CSS上下文中的注释 */" << std::endl;
    std::cout << std::endl;
    
    std::cout << "转换前: -- 脚本块中的注释" << std::endl;
    std::cout << "转换后: // 脚本块中的注释" << std::endl;
    std::cout << std::endl;
    
    std::cout << "转换前: -- 在Origin HTML块中的注释" << std::endl;
    std::cout << "转换后: <!-- 在Origin HTML块中的注释 -->" << std::endl;
    std::cout << std::endl;
    
    // 验证Origin使用
    auto validationReport = manager.validateOriginUsage(chtlCode);
    if (!validationReport.empty()) {
        std::cout << "📋 Origin使用验证报告（前3条）:" << std::endl;
        for (size_t i = 0; i < std::min(size_t(3), validationReport.size()); ++i) {
            std::cout << "  " << (i+1) << ". " << validationReport[i].substr(0, 100) << "..." << std::endl;
        }
        std::cout << "  ... 共 " << validationReport.size() << " 条报告" << std::endl;
    }
}

int main() {
    std::cout << "🚀 CHTL增强功能演示程序" << std::endl;
    std::cout << "展示[Origin]任意位置使用和\"--\"注释上下文感知" << std::endl;
    
    // 演示Origin任意位置使用
    demonstrateOriginAnywhere();
    
    // 演示上下文注释生成
    demonstrateContextualComments();
    
    // 演示真实世界应用
    demonstrateRealWorldUsage();
    
    std::cout << "\n========================================" << std::endl;
    std::cout << "🎯 演示完成！核心特性总结:" << std::endl;
    std::cout << "\n✨ [Origin]原始嵌入节点特性:" << std::endl;
    std::cout << "  ✅ 可以在任意位置使用，不受束缚" << std::endl;
    std::cout << "  ✅ 问题由开发者自己控制" << std::endl;
    std::cout << "  ✅ 支持语义合理性提醒（不阻止使用）" << std::endl;
    std::cout << "  ✅ 支持自定义Origin类型注册" << std::endl;
    std::cout << "  ✅ 提供最佳实践建议" << std::endl;
    std::cout << "  ✅ 支持嵌套使用" << std::endl;
    
    std::cout << "\n💬 \"--\"注释上下文感知特性:" << std::endl;
    std::cout << "  ✅ 根据上下文生成不同编程语言的注释" << std::endl;
    std::cout << "  ✅ 智能检测当前上下文类型" << std::endl;
    std::cout << "  ✅ 支持HTML、CSS、JavaScript、CHTL等多种注释格式" << std::endl;
    std::cout << "  ✅ 支持单行和多行注释转换" << std::endl;
    std::cout << "  ✅ 支持Origin块内部上下文检测" << std::endl;
    std::cout << "  ✅ 提供智能化的注释转换" << std::endl;
    
    std::cout << "\n🎨 设计理念体现:" << std::endl;
    std::cout << "  🎯 [Origin]: 最大化灵活性，开发者完全控制" << std::endl;
    std::cout << "  🎯 \"--\"注释: 智能化上下文感知，自动适配" << std::endl;
    std::cout << "  🎯 两者结合: 既有灵活性，又有智能性" << std::endl;
    
    std::cout << "\n🌟 这两个功能完美体现了CHTL的设计理念：" << std::endl;
    std::cout << "    \"不束缚开发者，但提供智能化辅助\"" << std::endl;
    std::cout << "========================================" << std::endl;
    
    return 0;
}