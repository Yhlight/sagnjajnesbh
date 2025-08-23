#include "../include/CJMOD/LayeredCJMODApi.h"
#include <iostream>
#include <vector>
#include <string>

/**
 * @brief 三层CJMOD API使用示例
 * 演示如何在同一个项目中选择不同的API模式
 */

// ============================================================================
// Layer 1: Original API 示例 - 您的原始设计理念
// ============================================================================

void demonstrateOriginalAPI() {
    std::cout << "\n=== Layer 1: Original API Demo ===\n";
    
    CJMOD_USE_ORIGINAL()  // 使用原始API
    
    // 设计printMylove函数
    auto keyword = syntaxAnalys("printMylove({ url: $, mode: $ });");
    
    // 按照您的原始设计：功能职责分离
    // bind() - 专门处理值转换
    keyword->args.bind<std::string>("$", [](const std::string& s) {
        std::cout << "  🔧 [bind] 处理字符串参数: " << s << " -> \"" << s << "\"\n";
        return "\"" + s + "\"";
    });
    
    keyword->args.bind<int>("$", [](int i) {
        std::cout << "  🔧 [bind] 处理数字参数: " << i << " -> " << i << "\n";
        return std::to_string(i);
    });
    
    // match() - 值匹配
    std::cout << "  📝 [match] 匹配参数值...\n";
    keyword->args.match("$", "photo.jpg");
    keyword->args.match("$", "300");
    
    // transform() - JS代码转换
    std::cout << "  🔄 [transform] 转换为JS代码...\n";
    keyword->args.transform("$", "loadImage(${value})");
    keyword->args.transform("$", "setMode(${value})");
    
    // 生成最终代码
    std::string result = keyword->result();
    std::cout << "  ✅ [result] 生成的JS代码: " << result << "\n";
    
    std::cout << "  💡 Original API特点: 职责分离、性能最佳、设计最纯粹\n";
}

// ============================================================================
// Layer 2: Enhanced API 示例 - 在Original基础上添加链式调用
// ============================================================================

void demonstrateEnhancedAPI() {
    std::cout << "\n=== Layer 2: Enhanced API Demo ===\n";
    
    CJMOD_USE_ENHANCED()  // 使用增强API
    
    // 同样的printMylove函数，但使用链式语法
    auto keyword = syntaxAnalys("printMylove({ url: $, mode: $ });");
    
    std::cout << "  🔗 使用流畅的链式调用...\n";
    
    // 链式调用 - 在Original基础上添加的语法糖
    keyword->chainableArgs
        .bind<std::string>("$", [](const std::string& s) {
            std::cout << "    🔧 [链式bind] 字符串: " << s << "\n";
            return "\"" + s + "\"";
        })
        .bind<int>("$", [](int i) {
            std::cout << "    🔧 [链式bind] 数字: " << i << "\n";
            return std::to_string(i);
        })
        .match("$", "photo.jpg")
        .match("$", "300")
        .transform("$", "loadImage(${value})")
        .transform("$", "setMode(${value})");
    
    std::string result = keyword->result();
    std::cout << "  ✅ [result] 生成的JS代码: " << result << "\n";
    
    // 演示批量操作
    std::cout << "  📦 演示批量操作...\n";
    auto keyword2 = syntaxAnalys("processImages($, $, $);");
    keyword2->chainableArgs
        .bindSequence<std::string, int, std::string>(
            [](const std::string& s) { return "\"" + s + "\""; },
            [](int i) { return std::to_string(i); },
            [](const std::string& s) { return "\"" + s + "\""; }
        )
        .matchSequence({"image1.jpg", "100", "filter"});
    
    std::cout << "  ✅ [批量操作] 结果: " << keyword2->result() << "\n";
    std::cout << "  💡 Enhanced API特点: 链式调用、批量操作、现代语法\n";
}

// ============================================================================
// Layer 3: AutoFill API 示例 - 在Enhanced基础上添加智能填充
// ============================================================================

void demonstrateAutoFillAPI() {
    std::cout << "\n=== Layer 3: AutoFill API Demo ===\n";
    
    CJMOD_USE_AUTOFILL()  // 使用智能API
    
    // 同样的printMylove函数，但使用智能填充
    auto keyword = syntaxAnalys("printMylove({ url: $, size: $ });");
    
    std::cout << "  🧠 启用智能自动填充...\n";
    
    // 模拟源码token（实际中会从扫描器获取）
    std::vector<std::string> sourceTokens = {
        "https://example.com/photo.jpg", "800", "px", "auto", "crop"
    };
    
    // 智能填充 - 自动识别类型并处理
    keyword->smartArgs
        .enableAutoFill({ParamType::URL, ParamType::SIZE})
        .smartFill(sourceTokens);
    
    std::string result = keyword->result();
    std::cout << "  ✅ [智能填充] 生成的JS代码: " << result << "\n";
    
    // 演示智能类型绑定
    std::cout << "  🎯 演示智能类型绑定...\n";
    auto keyword2 = syntaxAnalys("setStyle({ color: $, background: $ });");
    keyword2->smartArgs
        .smartBindSequence({ParamType::COLOR, ParamType::COLOR})
        .matchSequence({"ff0000", "00ff00"});
    
    std::cout << "  ✅ [智能类型] 结果: " << keyword2->result() << "\n";
    std::cout << "  💡 AutoFill API特点: 智能识别、自动填充、减少配置\n";
}

// ============================================================================
// 运行时模式切换示例
// ============================================================================

void demonstrateRuntimeModeSwitch() {
    std::cout << "\n=== 运行时模式切换 Demo ===\n";
    
    // 模拟不同的使用场景
    struct UserConfig {
        bool preferChaining = false;
        bool useAutoFill = false;
        std::string userLevel = "beginner";  // beginner, intermediate, advanced
    };
    
    UserConfig configs[] = {
        {false, false, "beginner"},      // 新手：使用Original
        {true, false, "intermediate"},   // 中级：使用Enhanced  
        {true, true, "advanced"}         // 高级：使用AutoFill
    };
    
    for (const auto& config : configs) {
        std::cout << "\n  👤 用户级别: " << config.userLevel << "\n";
        
        if (config.useAutoFill) {
            std::cout << "    🚀 切换到AutoFill模式\n";
            CJMOD_SET_MODE(AUTOFILL);
            
            // 使用默认模式创建（会自动选择AutoFill）
            auto syntax = CHTL::CJMOD::ModeSelector::createSyntax<CHTL::CJMOD::APIMode::AUTOFILL>(
                "loadImage($);"
            );
            std::cout << "    ✅ AutoFill模式语法解析完成\n";
            
        } else if (config.preferChaining) {
            std::cout << "    ⛓️ 切换到Enhanced模式\n";
            CJMOD_SET_MODE(ENHANCED);
            
            auto syntax = CHTL::CJMOD::ModeSelector::createSyntax<CHTL::CJMOD::APIMode::ENHANCED>(
                "loadImage($);"
            );
            std::cout << "    ✅ Enhanced模式语法解析完成\n";
            
        } else {
            std::cout << "    🎯 使用Original模式（默认）\n";
            CJMOD_SET_MODE(ORIGINAL);
            
            auto syntax = CHTL::CJMOD::ModeSelector::createSyntax<CHTL::CJMOD::APIMode::ORIGINAL>(
                "loadImage($);"
            );
            std::cout << "    ✅ Original模式语法解析完成\n";
        }
    }
}

// ============================================================================
// 性能对比示例
// ============================================================================

void demonstratePerformanceComparison() {
    std::cout << "\n=== 性能特点对比 ===\n";
    
    std::cout << "  📊 Layer 1 - Original API:\n";
    std::cout << "     ⚡ 性能: 最高（零开销抽象）\n";
    std::cout << "     📝 代码: 最直接（逐步操作）\n";
    std::cout << "     🎯 适用: 生产环境、性能关键场景\n";
    
    std::cout << "\n  📊 Layer 2 - Enhanced API:\n";
    std::cout << "     ⚡ 性能: 高（轻微方法调用开销）\n";
    std::cout << "     📝 代码: 流畅（链式调用）\n";
    std::cout << "     🎯 适用: 开发效率优先、代码可读性重要\n";
    
    std::cout << "\n  📊 Layer 3 - AutoFill API:\n";
    std::cout << "     ⚡ 性能: 中等（智能分析开销）\n";
    std::cout << "     📝 代码: 最简（自动处理）\n";
    std::cout << "     🎯 适用: 快速原型、复杂场景自动化\n";
    
    std::cout << "\n  💡 建议：\n";
    std::cout << "     🏭 生产环境 -> Original API\n";
    std::cout << "     🛠️ 开发环境 -> Enhanced API\n";
    std::cout << "     🧪 原型环境 -> AutoFill API\n";
}

// ============================================================================
// 主函数
// ============================================================================

int main() {
    std::cout << "🚀 CHTL CJMOD 三层API架构演示\n";
    std::cout << "========================================\n";
    
    // 演示三种API模式
    demonstrateOriginalAPI();
    demonstrateEnhancedAPI();
    demonstrateAutoFillAPI();
    
    // 演示运行时切换
    demonstrateRuntimeModeSwitch();
    
    // 性能对比
    demonstratePerformanceComparison();
    
    std::cout << "\n========================================\n";
    std::cout << "🎯 总结：三层架构让您可以根据场景选择最适合的API模式\n";
    std::cout << "   Layer 1 (Original) <- Layer 2 (Enhanced) <- Layer 3 (AutoFill)\n";
    std::cout << "   默认使用 Original API，确保与您的原始设计完全一致！\n";
    
    return 0;
}

/**
 * @brief 编译和运行说明：
 * 
 * g++ -std=c++17 -I../include layered_cjmod_demo.cpp -o layered_demo
 * ./layered_demo
 * 
 * 预期输出：
 * - 展示三种API模式的具体用法
 * - 演示运行时模式切换
 * - 性能特点对比分析
 * - 使用场景建议
 */