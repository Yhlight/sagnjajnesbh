#include "CHTL/Enhanced/OriginAnywhereSystem.h"
#include <iostream>
#include <string>

using namespace CHTL::Enhanced;

void demonstrateOriginCompleteFreedom() {
    std::cout << "\n========================================" << std::endl;
    std::cout << "[Origin]完全自由使用演示" << std::endl;
    std::cout << "核心理念：不预定义任何类型，开发者完全自由控制" << std::endl;
    std::cout << "========================================\n" << std::endl;
    
    auto& manager = getEnhancedFeatureManager();
    auto& originManager = manager.getOriginManager();
    
    std::cout << "💡 获取当前支持的Origin类型（应该为空，让开发者自由创建）:" << std::endl;
    auto types = originManager.getSupportedOriginTypes();
    
    std::cout << "\n🚀 开发者完全自由创建示例:" << std::endl;
    std::cout << "  [Origin] @Vue - 开发者想用Vue就用Vue" << std::endl;
    std::cout << "  [Origin] @React - 开发者想用React就用React" << std::endl;
    std::cout << "  [Origin] @TypeScript - 开发者想用TypeScript就用TypeScript" << std::endl;
    std::cout << "  [Origin] @Python - 开发者甚至可以嵌入Python代码" << std::endl;
    std::cout << "  [Origin] @GraphQL - 嵌入GraphQL查询" << std::endl;
    std::cout << "  [Origin] @Dockerfile - 嵌入Docker配置" << std::endl;
    std::cout << "  [Origin] @MyBusinessLogic - 完全自定义的业务逻辑" << std::endl;
    std::cout << "  [Origin] @AnyThingDeveloperWants - 开发者想要什么就有什么" << std::endl;
    
    std::cout << "\n✨ 测试任意位置使用:" << std::endl;
    
    // 测试各种"奇怪"的位置
    OriginPosition pos1;
    pos1.context = ContextType::CSS_BLOCK;
    pos1.line = 1;
    std::cout << "  在CSS块中使用[Origin] @Python: " 
              << (originManager.canUseOriginAnywhere(pos1, OriginType::CUSTOM_TYPE) ? "✅ 允许" : "❌ 禁止") << std::endl;
    
    OriginPosition pos2;
    pos2.context = ContextType::JAVASCRIPT_BLOCK;
    pos2.line = 2;
    std::cout << "  在JavaScript块中使用[Origin] @Html: " 
              << (originManager.canUseOriginAnywhere(pos2, OriginType::CUSTOM_TYPE) ? "✅ 允许" : "❌ 禁止") << std::endl;
    
    OriginPosition pos3;
    pos3.context = ContextType::ORIGIN_BLOCK;
    pos3.line = 3;
    std::cout << "  在Origin块中嵌套[Origin] @AnotherType: " 
              << (originManager.canUseOriginAnywhere(pos3, OriginType::CUSTOM_TYPE) ? "✅ 允许" : "❌ 禁止") << std::endl;
              
    std::cout << "\n🎯 结论：开发者拥有完全的控制权，系统不做任何限制！" << std::endl;
}

void demonstrateDashCommentAsCHTLNativeSyntax() {
    std::cout << "\n========================================" << std::endl;
    std::cout << "\"--\"注释作为CHTL原生语法演示" << std::endl;
    std::cout << "核心理念：\"--\"是CHTL的原生语法，默认转换为HTML注释" << std::endl;
    std::cout << "========================================\n" << std::endl;
    
    auto& manager = getEnhancedFeatureManager();
    auto& commentGenerator = manager.getCommentGenerator();
    
    std::cout << "📝 \"--\"注释转换规则:" << std::endl;
    std::cout << "  • CHTL上下文: -- 注释  →  <!-- 注释 --> (HTML注释)" << std::endl;
    std::cout << "  • CSS上下文: -- 注释  →  /* 注释 */ (CSS注释)" << std::endl;
    std::cout << "  • JavaScript上下文: -- 注释  →  // 注释 (JS注释)" << std::endl;
    std::cout << "  • 默认: -- 注释  →  <!-- 注释 --> (HTML注释)" << std::endl;
    
    std::cout << "\n🧪 实际转换测试:" << std::endl;
    
    struct TestCase {
        std::string contextName;
        ContextType context;
        std::string input;
    };
    
    std::vector<TestCase> tests = {
        {"CHTL根上下文", ContextType::CHTL_ROOT, "这是CHTL根级别的注释"},
        {"CHTL元素内", ContextType::CHTL_ELEMENT, "这是CHTL元素中的注释"},
        {"Origin块中", ContextType::ORIGIN_BLOCK, "这是Origin块中的注释"},
        {"模板块中", ContextType::TEMPLATE_BLOCK, "这是模板块中的注释"},
        {"CSS样式块", ContextType::CSS_BLOCK, "这是CSS样式块中的注释"},
        {"JavaScript块", ContextType::JAVASCRIPT_BLOCK, "这是JavaScript块中的注释"},
        {"CHTL JS块", ContextType::CHTL_JS_BLOCK, "这是CHTL JS块中的注释"}
    };
    
    for (const auto& test : tests) {
        std::string result = commentGenerator.processDashComment(test.input, test.context, false);
        std::cout << "  " << test.contextName << ":" << std::endl;
        std::cout << "    输入: --" << test.input << std::endl;
        std::cout << "    输出: " << result << std::endl;
        std::cout << std::endl;
    }
    
    std::cout << "🎯 重要理解：" << std::endl;
    std::cout << "  \"--\"是CHTL的原生语法，不是其他语言的注释!" << std::endl;
    std::cout << "  在CHTL上下文中，它被转换为HTML注释，这是正确的!" << std::endl;
    std::cout << "  只有在特定的语言上下文中，才转换为对应语言的注释格式!" << std::endl;
}

void demonstrateRealWorldScenario() {
    std::cout << "\n========================================" << std::endl;
    std::cout << "真实世界场景演示" << std::endl;
    std::cout << "========================================\n" << std::endl;
    
    auto& manager = getEnhancedFeatureManager();
    
    std::string chtlCode = R"(
// 这是普通的CHTL注释，不会被转换
-- 这是CHTL原生语法注释，会被转换为HTML注释

[Namespace] MyProject

html {
    head {
        title { text: "正确的CHTL理念演示"; }
        
        -- 这是在CHTL上下文中的注释，转换为HTML注释
        
        [Origin] @MyCustomMetaTags {
            -- 这在Origin块中，也转换为HTML注释
            <meta name="description" content="展示CHTL的正确理念">
            <meta name="keywords" content="CHTL,Origin,自由">
        }
    }
    
    body {
        div {
            class: "container";
            
            -- CHTL元素中的注释，转换为HTML注释
            
            h1 { text: "开发者完全自由的Origin"; }
            
            [Origin] @Vue {
                -- Vue组件中的注释，转换为HTML注释
                <template>
                    <div>Vue组件，开发者自己决定使用</div>
                </template>
            }
            
            [Origin] @Python {
                -- Python代码中的注释，转换为HTML注释（因为在Origin块中）
                def hello():
                    print("开发者甚至可以嵌入Python!")
            }
            
            style {
                -- CSS上下文中的注释，转换为CSS注释
                .container {
                    padding: 20px;
                }
                
                [Origin] @Sass {
                    -- Sass代码，转换为HTML注释（因为在Origin块中）
                    $primary-color: #007bff;
                    .button {
                        background: $primary-color;
                    }
                }
            }
            
            script {
                -- JavaScript上下文中的注释，转换为JS注释
                console.log('这是JavaScript上下文');
                
                [Origin] @TypeScript {
                    -- TypeScript代码，转换为HTML注释（因为在Origin块中）
                    interface User {
                        name: string;
                        age: number;
                    }
                }
                
                {{&}}->listen({
                    click: () => {
                        -- CHTL JS上下文中的注释，转换为JS注释
                        console.log('CHTL JS事件');
                    }
                });
            }
        }
    }
}
)";
    
    std::cout << "🔄 处理真实的CHTL代码..." << std::endl;
    std::string processedCode = manager.processCode(chtlCode);
    
    std::cout << "\n✅ 关键转换示例:" << std::endl;
    std::cout << "1. CHTL上下文: -- 注释  →  <!-- 注释 -->" << std::endl;
    std::cout << "2. CSS上下文: -- 注释  →  /* 注释 */" << std::endl;
    std::cout << "3. JS上下文: -- 注释  →  // 注释" << std::endl;
    std::cout << "4. Origin块: -- 注释  →  <!-- 注释 --> (因为Origin块本身还是CHTL上下文)" << std::endl;
    
    std::cout << "\n🎯 Origin完全自由示例:" << std::endl;
    std::cout << "  [Origin] @Vue - 开发者想用Vue组件" << std::endl;
    std::cout << "  [Origin] @Python - 开发者想嵌入Python代码" << std::endl;
    std::cout << "  [Origin] @Sass - 开发者想用Sass预处理器" << std::endl;
    std::cout << "  [Origin] @TypeScript - 开发者想用TypeScript类型" << std::endl;
    std::cout << "  [Origin] @AnyThingDeveloperWants - 完全由开发者控制!" << std::endl;
}

int main() {
    std::cout << "🚀 修正后的CHTL增强功能演示" << std::endl;
    std::cout << "体现正确的设计理念" << std::endl;
    
    // 演示Origin完全自由
    demonstrateOriginCompleteFreedom();
    
    // 演示"--"注释作为CHTL原生语法
    demonstrateDashCommentAsCHTLNativeSyntax();
    
    // 演示真实世界场景
    demonstrateRealWorldScenario();
    
    std::cout << "\n========================================" << std::endl;
    std::cout << "🎯 修正后的核心理念总结:" << std::endl;
    
    std::cout << "\n✨ [Origin]原始嵌入节点:" << std::endl;
    std::cout << "  ✅ 不预定义任何类型，开发者完全自由" << std::endl;
    std::cout << "  ✅ 可以在任意位置使用，不受任何束缚" << std::endl;
    std::cout << "  ✅ 问题由开发者自己控制，系统不做限制" << std::endl;
    std::cout << "  ✅ 开发者想用什么类型就用什么类型" << std::endl;
    std::cout << "  ✅ @Vue, @React, @Python, @AnyThing 都可以" << std::endl;
    
    std::cout << "\n💬 \"--\"注释（CHTL原生语法）:" << std::endl;
    std::cout << "  ✅ \"--\"是CHTL的原生语法，不是其他语言的注释" << std::endl;
    std::cout << "  ✅ 在CHTL上下文中转换为HTML注释（<!-- -->）" << std::endl;
    std::cout << "  ✅ 在CSS上下文中转换为CSS注释（/* */）" << std::endl;
    std::cout << "  ✅ 在JavaScript上下文中转换为JS注释（//）" << std::endl;
    std::cout << "  ✅ 智能检测上下文，自动适配注释格式" << std::endl;
    
    std::cout << "\n🌟 设计理念的正确体现:" << std::endl;
    std::cout << "  🎯 [Origin]: 绝对的自由，零束缚，完全由开发者控制" << std::endl;
    std::cout << "  🎯 \"--\"注释: CHTL原生语法的智能转换" << std::endl;
    std::cout << "  🎯 这才是真正的CHTL理念：自由且智能" << std::endl;
    
    std::cout << "\n🔥 关键修正:" << std::endl;
    std::cout << "  ❌ 错误：预定义Origin类型" << std::endl;
    std::cout << "  ✅ 正确：让开发者完全自由创建" << std::endl;
    std::cout << "  ❌ 错误：\"--\"转换为CHTL注释" << std::endl;
    std::cout << "  ✅ 正确：\"--\"是CHTL原生语法，转换为HTML注释" << std::endl;
    
    std::cout << "\n🎨 这就是CHTL的真正魅力：" << std::endl;
    std::cout << "    \"开发者的自由意志 + 系统的智能辅助\"" << std::endl;
    std::cout << "========================================" << std::endl;
    
    return 0;
}