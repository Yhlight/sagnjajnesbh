/**
 * @file CJMOD_标准流程vs托管流程演示.cpp
 * @brief 演示标准CJMOD流程 vs createCHTLJSFunction托管流程
 * @author CHTL Team
 * @date 2024
 * 
 * 本文件展示两种方式：
 * 1. 完全手动的标准CJMOD流程
 * 2. 使用createCHTLJSFunction托管部分步骤的流程
 */

#include "CJMOD/ElegantCJMODApi.h"
#include <iostream>
#include <string>

using namespace CHTL::CJMOD;

// ==========================================
// 演示1：完全标准的CJMOD流程
// ==========================================

void demonstrateStandardCJMODFlow() {
    std::cout << "\n========================================" << std::endl;
    std::cout << "演示1：完全标准的CJMOD流程" << std::endl;
    std::cout << "========================================" << std::endl;
    
    std::cout << "\n【步骤1】syntaxAnalys - 手动编写语法模式" << std::endl;
    
    // 手动编写复杂的语法模式
    std::string pattern = R"(
        const $ = myImageProcessor({
            url: $,
            mode: $,
            width: $,
            height: $,
            scale: $
        });
    )";
    
    std::string ignoreChars = ",:{};()";
    auto keyword = syntaxAnalys(pattern, ignoreChars);
    
    std::cout << "✓ 手动编写语法模式完成" << std::endl;
    std::cout << "✓ 语法模式: " << pattern << std::endl;
    
    std::cout << "\n【步骤2】bind - 手动绑定每个参数处理器" << std::endl;
    
    // 手动为每个参数绑定处理器
    keyword->args.bind<std::string>("varName", [](const std::string& varName) -> std::string {
        std::cout << "  → 处理变量名: " << varName << std::endl;
        return varName;
    });
    
    keyword->args.bind<std::string>("url", [](const std::string& url) -> std::string {
        std::cout << "  → 处理URL: " << url << std::endl;
        // 手动处理URL验证
        if (url.find("http") == 0) {
            return "\"" + url + "\"";
        }
        return "\"./" + url + "\"";
    });
    
    keyword->args.bind<std::string>("mode", [](const std::string& mode) -> std::string {
        std::cout << "  → 处理模式: " << mode << std::endl;
        // 手动处理模式验证
        if (mode == "ascii" || mode == "pixel") {
            return "\"" + mode + "\"";
        }
        return "\"ascii\""; // 默认值
    });
    
    keyword->args.bind<std::string>("width", [](const std::string& width) -> std::string {
        std::cout << "  → 处理宽度: " << width << std::endl;
        return width.empty() ? "80" : width;
    });
    
    keyword->args.bind<std::string>("height", [](const std::string& height) -> std::string {
        std::cout << "  → 处理高度: " << height << std::endl;
        return height.empty() ? "40" : height;
    });
    
    keyword->args.bind<double>("scale", [](double scale) -> std::string {
        std::cout << "  → 处理缩放: " << scale << std::endl;
        return (scale > 0 && scale <= 5.0) ? std::to_string(scale) : "1.0";
    });
    
    std::cout << "✓ 手动绑定6个参数处理器完成" << std::endl;
    
    std::cout << "\n【步骤3】scanKeyword - 手动扫描CHTL代码" << std::endl;
    
    // 模拟CHTL代码
    std::string chtlCode = R"(
        const result = myImageProcessor({
            url: https://example.com/image.jpg,
            mode: ascii,
            width: 100,
            height: 50,
            scale: 1.5
        });
    )";
    
    auto& scanner = getCJMODScanner();
    std::string finalResult;
    
    scanner.scanKeyword("myImageProcessor", [&]() {
        std::cout << "✓ 检测到 myImageProcessor 函数调用" << std::endl;
        
        std::cout << "\n【步骤4】match - 手动匹配参数值" << std::endl;
        
        // 手动提取和匹配每个参数
        keyword->args.match("varName", "result");
        keyword->args.match("url", "https://example.com/image.jpg");
        keyword->args.match("mode", "ascii");
        keyword->args.match("width", "100");
        keyword->args.match("height", "50");
        keyword->args.match("scale", 1.5);
        
        std::cout << "✓ 手动匹配6个参数完成" << std::endl;
        
        std::cout << "\n【步骤5】generateCode - 手动生成最终JavaScript" << std::endl;
        
        finalResult = generateCode(*keyword);
        std::cout << "✓ JavaScript代码生成完成" << std::endl;
    });
    
    std::cout << "\n【最终结果】" << std::endl;
    std::cout << "生成的JavaScript代码:\n" << finalResult << std::endl;
    
    std::cout << "\n【标准流程总结】" << std::endl;
    std::cout << "✓ 需要手动编写复杂的语法模式" << std::endl;
    std::cout << "✓ 需要手动绑定每个参数处理器" << std::endl;
    std::cout << "✓ 需要手动扫描、匹配、生成代码" << std::endl;
    std::cout << "✓ 完全控制，但代码较多" << std::endl;
}

// ==========================================
// 演示2：使用createCHTLJSFunction托管部分步骤
// ==========================================

void demonstrateManagedCJMODFlow() {
    std::cout << "\n========================================" << std::endl;
    std::cout << "演示2：使用createCHTLJSFunction托管部分步骤" << std::endl;
    std::cout << "========================================" << std::endl;
    
    std::cout << "\n【托管步骤】createCHTLJSFunction - 自动处理语法创建" << std::endl;
    
    // 托管步骤：只需提供函数名和键名，自动生成语法模式
    auto myFunc = createCHTLJSFunction("myImageProcessor", {
        "url", "mode", "width", "height", "scale"
    });
    
    std::cout << "✓ 语法模式自动生成完成" << std::endl;
    std::cout << "✓ 返回标准Keyword对象，可继续标准流程" << std::endl;
    
    std::cout << "\n【托管步骤】bindKeyProcessor - 简化参数绑定" << std::endl;
    
    // 托管步骤：简化的参数绑定，无需手动调用keyword->args.bind
    myFunc->bindKeyProcessor("url", [](const std::string& url) -> std::string {
        std::cout << "  → 处理URL: " << url << std::endl;
        if (url.find("http") == 0) {
            return "\"" + url + "\"";
        }
        return "\"./" + url + "\"";
    });
    
    myFunc->bindKeyProcessor("mode", [](const std::string& mode) -> std::string {
        std::cout << "  → 处理模式: " << mode << std::endl;
        if (mode == "ascii" || mode == "pixel") {
            return "\"" + mode + "\"";
        }
        return "\"ascii\"";
    });
    
    // 托管步骤：设置默认值
    myFunc->setDefaultValues({
        {"width", "80"},
        {"height", "40"},
        {"scale", "1.0"}
    });
    
    std::cout << "✓ 简化参数绑定完成" << std::endl;
    std::cout << "✓ 默认值设置完成" << std::endl;
    
    std::cout << "\n【标准步骤】scanKeyword - 仍需手动执行" << std::endl;
    
    // 获取托管生成的标准Keyword对象
    auto& keyword = myFunc->getKeyword();
    
    // 标准步骤：手动扫描（与标准流程相同）
    std::string chtlCode = R"(
        const result = myImageProcessor({
            url: https://example.com/image.jpg,
            mode: ascii,
            width: 100,
            height: 50,
            scale: 1.5
        });
    )";
    
    auto& scanner = getCJMODScanner();
    std::string finalResult;
    
    scanner.scanKeyword("myImageProcessor", [&]() {
        std::cout << "✓ 检测到 myImageProcessor 函数调用" << std::endl;
        
        std::cout << "\n【标准步骤】match - 仍需手动执行" << std::endl;
        
        // 标准步骤：手动匹配（与标准流程相同）
        keyword->args.match("varName", "result");
        keyword->args.match("configObject", R"({
            url: https://example.com/image.jpg,
            mode: ascii,
            width: 100,
            height: 50,
            scale: 1.5
        })");
        
        std::cout << "✓ 参数匹配完成" << std::endl;
        
        std::cout << "\n【标准步骤】generateCode - 仍需手动执行" << std::endl;
        
        // 标准步骤：手动生成代码（与标准流程相同）
        finalResult = generateCode(*keyword);
        std::cout << "✓ JavaScript代码生成完成" << std::endl;
    });
    
    std::cout << "\n【最终结果】" << std::endl;
    std::cout << "生成的JavaScript代码:\n" << finalResult << std::endl;
    
    std::cout << "\n【托管流程总结】" << std::endl;
    std::cout << "✅ [已托管] 语法模式自动生成" << std::endl;
    std::cout << "✅ [已托管] 简化参数绑定" << std::endl;
    std::cout << "✅ [已托管] CHTL JS特性支持（无序、可选、无修饰字面量）" << std::endl;
    std::cout << "⚠️  [仍需手动] scanKeyword扫描" << std::endl;
    std::cout << "⚠️  [仍需手动] match匹配" << std::endl;
    std::cout << "⚠️  [仍需手动] generateCode生成" << std::endl;
    std::cout << "✓ 减少重复代码，但保持标准流程" << std::endl;
}

// ==========================================
// 演示3：对比分析
// ==========================================

void demonstrateComparison() {
    std::cout << "\n========================================" << std::endl;
    std::cout << "演示3：两种流程对比分析" << std::endl;
    std::cout << "========================================" << std::endl;
    
    std::cout << "\n【代码量对比】" << std::endl;
    std::cout << "标准流程：" << std::endl;
    std::cout << "  - 语法模式编写：~10行复杂字符串" << std::endl;
    std::cout << "  - 参数绑定：~30行重复代码" << std::endl;
    std::cout << "  - 扫描匹配：~15行标准代码" << std::endl;
    std::cout << "  - 总计：~55行代码" << std::endl;
    
    std::cout << "\n托管流程：" << std::endl;
    std::cout << "  - 函数创建：1行createCHTLJSFunction" << std::endl;
    std::cout << "  - 参数绑定：~15行简化代码" << std::endl;
    std::cout << "  - 扫描匹配：~15行标准代码（相同）" << std::endl;
    std::cout << "  - 总计：~31行代码" << std::endl;
    
    std::cout << "\n【减少代码量】约44%（24行 → 31行）" << std::endl;
    
    std::cout << "\n【功能对比】" << std::endl;
    std::cout << "标准流程：" << std::endl;
    std::cout << "  ✓ 完全控制所有步骤" << std::endl;
    std::cout << "  ✓ 可以自定义复杂语法" << std::endl;
    std::cout << "  ✗ 需要手动处理CHTL JS特性" << std::endl;
    std::cout << "  ✗ 重复代码较多" << std::endl;
    
    std::cout << "\n托管流程：" << std::endl;
    std::cout << "  ✓ 自动处理CHTL JS特性" << std::endl;
    std::cout << "  ✓ 减少重复代码" << std::endl;
    std::cout << "  ✓ 仍保持标准流程兼容" << std::endl;
    std::cout << "  ✗ 语法模式相对固定" << std::endl;
    
    std::cout << "\n【适用场景】" << std::endl;
    std::cout << "标准流程适用于：" << std::endl;
    std::cout << "  - 复杂的自定义语法" << std::endl;
    std::cout << "  - 需要完全控制的场景" << std::endl;
    std::cout << "  - 学习CJMOD内部机制" << std::endl;
    
    std::cout << "\n托管流程适用于：" << std::endl;
    std::cout << "  - 标准CHTL JS函数" << std::endl;
    std::cout << "  - 快速原型开发" << std::endl;
    std::cout << "  - 减少重复代码" << std::endl;
    
    std::cout << "\n【核心理念】" << std::endl;
    std::cout << "createCHTLJSFunction并不颠覆标准流程，而是：" << std::endl;
    std::cout << "  ✓ 托管语法模式生成" << std::endl;
    std::cout << "  ✓ 托管参数绑定简化" << std::endl;
    std::cout << "  ✓ 托管CHTL JS特性处理" << std::endl;
    std::cout << "  ✓ 返回标准Keyword对象" << std::endl;
    std::cout << "  ✓ 开发者仍需执行scanKeyword、match、generateCode" << std::endl;
}

// ==========================================
// 演示4：实际使用建议
// ==========================================

void demonstrateUsageRecommendations() {
    std::cout << "\n========================================" << std::endl;
    std::cout << "演示4：实际使用建议" << std::endl;
    std::cout << "========================================" << std::endl;
    
    std::cout << "\n【推荐使用托管流程的情况】" << std::endl;
    std::cout << "1. 创建标准CHTL JS函数（键值对形式）" << std::endl;
    std::cout << "2. 需要支持CHTL JS官方特性（无序、可选、无修饰字面量）" << std::endl;
    std::cout << "3. 快速原型开发和测试" << std::endl;
    std::cout << "4. 减少重复的语法模式编写" << std::endl;
    
    std::cout << "\n【推荐使用标准流程的情况】" << std::endl;
    std::cout << "1. 需要复杂的自定义语法" << std::endl;
    std::cout << "2. 非键值对形式的函数" << std::endl;
    std::cout << "3. 需要完全控制每个步骤" << std::endl;
    std::cout << "4. 学习CJMOD内部机制" << std::endl;
    
    std::cout << "\n【混合使用策略】" << std::endl;
    std::cout << "可以在同一个项目中混合使用两种方式：" << std::endl;
    std::cout << "  - 标准CHTL JS函数 → 使用托管流程" << std::endl;
    std::cout << "  - 复杂自定义语法 → 使用标准流程" << std::endl;
    std::cout << "  - 两者返回的都是标准Keyword对象，完全兼容" << std::endl;
    
    std::cout << "\n【迁移策略】" << std::endl;
    std::cout << "从标准流程迁移到托管流程：" << std::endl;
    std::cout << "1. 识别键值对形式的函数" << std::endl;
    std::cout << "2. 提取函数名和键名列表" << std::endl;
    std::cout << "3. 使用createCHTLJSFunction替换syntaxAnalys" << std::endl;
    std::cout << "4. 使用bindKeyProcessor替换多个bind调用" << std::endl;
    std::cout << "5. 保持scanKeyword、match、generateCode不变" << std::endl;
}

// ==========================================
// 主函数 - 完整演示
// ==========================================

int main() {
    std::cout << "========================================" << std::endl;
    std::cout << "    CJMOD标准流程 vs 托管流程演示" << std::endl;
    std::cout << "========================================" << std::endl;
    
    try {
        // 演示1：完全标准的CJMOD流程
        demonstrateStandardCJMODFlow();
        
        // 演示2：使用createCHTLJSFunction托管部分步骤
        demonstrateManagedCJMODFlow();
        
        // 演示3：对比分析
        demonstrateComparison();
        
        // 演示4：实际使用建议
        demonstrateUsageRecommendations();
        
        std::cout << "\n✅ 所有演示完成！" << std::endl;
        std::cout << "\n💡 核心要点：" << std::endl;
        std::cout << "   createCHTLJSFunction只是托管了部分步骤，" << std::endl;
        std::cout << "   并没有颠覆标准CJMOD流程，" << std::endl;
        std::cout << "   开发者仍需执行标准的scanKeyword、match、generateCode步骤。" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "❌ 演示过程中出现错误: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}