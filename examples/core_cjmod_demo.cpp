#include "../include/CJMOD/CoreCJMODApi.h"
#include <iostream>
#include <vector>
#include <string>

using namespace CHTL::CJMOD;

/**
 * @brief 核心CJMOD API演示
 * 展示您的原始API设计 + 双指针扫描 + 前置截取架构
 */

// ============================================================================
// 演示1: 基础语法分析和参数绑定
// ============================================================================

void demonstrateBasicSyntaxAnalysis() {
    std::cout << "\n=== 演示1: 基础语法分析 ===\n";
    
    // 基础示例：arg ** arg2
    auto keyword = syntaxAnalys("arg ** arg2");
    
    std::cout << "语法分析结果:\n";
    for (size_t i = 0; i < keyword->args.size(); ++i) {
        std::cout << "  args[" << i << "] = \"" << keyword->args[i].getName() << "\"\n";
    }
    
    // 占位符示例：$ ** $
    auto keyword2 = syntaxAnalys("$ ** $");
    
    std::cout << "\n占位符分析结果:\n";
    for (size_t i = 0; i < keyword2->args.size(); ++i) {
        std::cout << "  args[" << i << "] = \"" << keyword2->args[i].getName() 
                  << "\" (占位符: " << (keyword2->args[i].isPlaceholder() ? "是" : "否") << ")\n";
    }
}

// ============================================================================
// 演示2: 复杂函数语法分析（printMylove）
// ============================================================================

void demonstratePrintMyloveAnalysis() {
    std::cout << "\n=== 演示2: printMylove 语法分析 ===\n";
    
    std::string ignoreChars = ",:{};()";
    auto keyword = syntaxAnalys(R"(
        printMylove({
            url: $,
            mode: $,
            width: $,
            height: $,
            scale: $,
        });
    )", ignoreChars);
    
    std::cout << "printMylove 函数分析结果:\n";
    for (size_t i = 0; i < keyword->args.size(); ++i) {
        std::cout << "  args[" << i << "] = \"" << keyword->args[i].getName() 
                  << "\" (占位符: " << (keyword->args[i].isPlaceholder() ? "是" : "否") << ")\n";
    }
    
    std::cout << "\n参数总数: " << keyword->length() << "\n";
}

// ============================================================================
// 演示3: 参数绑定和处理 - 您的核心API设计
// ============================================================================

void demonstrateParameterBinding() {
    std::cout << "\n=== 演示3: 参数绑定 - 核心API设计 ===\n";
    
    std::string ignoreChars = ",:{};()";
    auto keyword = syntaxAnalys(R"(
        printMylove({
            url: $,
            mode: $,
            width: $,
            height: $,
            scale: $,
        });
    )", ignoreChars);
    
    // 按照您的设计：bind方法绑定处理函数
    std::cout << "绑定参数处理函数...\n";
    
    keyword->bind<std::string>("url", [](const std::string& str) {
        std::cout << "  🔧 处理URL参数: " << str << "\n";
        return "\"" + str + "\"";  // 添加引号
    });
    
    keyword->bind<std::string>("mode", [](const std::string& str) {
        std::cout << "  🔧 处理模式参数: " << str << "\n";
        return str == "ASCII" ? "\"ASCII\"" : "\"Pixel\"";
    });
    
    keyword->bind<std::string>("width", [](const std::string& str) {
        std::cout << "  🔧 处理宽度参数: " << str << "\n";
        // 支持CSS单位、百分比、小数、纯数字
        if (str.find("px") != std::string::npos || 
            str.find("%") != std::string::npos ||
            str.find(".") != std::string::npos) {
            return "\"" + str + "\"";
        }
        return str;  // 纯数字
    });
    
    keyword->bind<std::string>("height", [](const std::string& str) {
        std::cout << "  🔧 处理高度参数: " << str << "\n";
        return str.find("px") != std::string::npos ? ("\"" + str + "\"") : str;
    });
    
    keyword->bind<double>("scale", [](double scale) {
        std::cout << "  🔧 处理缩放参数: " << scale << "\n";
        return std::to_string(scale);  // 等比缩放倍数
    });
    
    std::cout << "✅ 参数绑定完成\n";
}

// ============================================================================
// 演示4: 双指针扫描器架构
// ============================================================================

void demonstrateDualPointerScanning() {
    std::cout << "\n=== 演示4: 双指针扫描架构 ===\n";
    
    // 创建双指针扫描器
    DualPointerScanner scanner;
    
    // 设置扫描内容
    std::vector<std::string> tokens = {"arg", "**", "arg2", "more", "content"};
    scanner.setContent(tokens);
    
    std::cout << "📊 双指针扫描过程:\n";
    
    // 1. 扫描初始范围
    std::cout << "  1️⃣ 扫描初始范围（确定无语法片段）...\n";
    bool hasInitialSyntax = scanner.scanInitialRange(3);
    std::cout << "     初始范围检测结果: " << (hasInitialSyntax ? "存在语法片段" : "无语法片段") << "\n";
    
    // 2. 双指针同步扫描
    std::cout << "  2️⃣ 双指针同步扫描（寻找关键字）...\n";
    ScanPosition pos = scanner.synchronousScan("**");
    std::cout << "     前指针位置: " << pos.frontPointer << "\n";
    std::cout << "     后指针位置: " << pos.backPointer << "\n";
    std::cout << "     检测到关键字: " << (pos.hasKeyword ? "是" : "否") << "\n";
    std::cout << "     扫描内容: \"" << pos.content << "\"\n";
    
    // 3. peekKeyword 相对位置访问
    std::cout << "  3️⃣ peekKeyword 相对位置访问:\n";
    std::cout << "     peekKeyword(-1): \"" << scanner.peekKeyword(-1) << "\"\n";
    std::cout << "     peekKeyword(1): \"" << scanner.peekKeyword(1) << "\"\n";
    
    std::cout << "✅ 双指针扫描演示完成\n";
}

// ============================================================================
// 演示5: 前置截取机制
// ============================================================================

void demonstrateFrontExtraction() {
    std::cout << "\n=== 演示5: 前置截取机制 ===\n";
    
    DualPointerScanner scanner;
    
    // 设置包含前置参数的内容：arg ** arg2
    std::vector<std::string> tokens = {"previous", "arg", "**", "arg2", "next"};
    scanner.setContent(tokens);
    
    std::cout << "📋 前置截取场景: \"previous arg ** arg2 next\"\n";
    std::cout << "🎯 目标: 当统一扫描器扫描到 ** 时，截取前面的 arg\n";
    
    // 执行前置截取
    std::cout << "  🔍 执行前置截取...\n";
    ExtractResult result = scanner.frontExtract("**", -1);
    
    std::cout << "     截取成功: " << (result.success ? "是" : "否") << "\n";
    std::cout << "     截取内容: \"" << result.extracted << "\"\n";
    std::cout << "     剩余内容: \"" << result.remaining << "\"\n";
    
    std::cout << "💡 意义: 避免 'arg' 被发送给编译器，确保CJMOD正确处理\n";
    std::cout << "✅ 前置截取演示完成\n";
}

// ============================================================================
// 演示6: CJMODScanner完整流程
// ============================================================================

void demonstrateCompleteScanning() {
    std::cout << "\n=== 演示6: CJMODScanner 完整流程 ===\n";
    
    // 创建语法和扫描器
    auto keyword = syntaxAnalys("arg ** arg2");
    CJMODScanner scanner;
    
    // 设置扫描内容
    std::vector<std::string> tokens = {"value1", "arg", "**", "arg2", "value2"};
    scanner.setTokens(tokens);
    scanner.setPosition(1);  // 定位到 "arg"
    
    std::cout << "🎯 完整扫描流程演示:\n";
    
    // 1. 绑定参数处理函数
    std::cout << "  1️⃣ 绑定参数处理函数...\n";
    keyword->args[0].bind<std::string>([](const std::string& s) {
        std::cout << "     📥 左参数处理: " << s << " -> \"" << s << "\"\n";
        return "\"" + s + "\"";
    });
    
    keyword->args[2].bind<std::string>([](const std::string& s) {
        std::cout << "     📥 右参数处理: " << s << " -> \"" << s << "\"\n";
        return "\"" + s + "\"";
    });
    
    // 2. 关键字扫描
    std::cout << "  2️⃣ 关键字扫描处理...\n";
    scanner.scanKeyword(keyword->args[1].getName(), [&]() {
        std::cout << "     🔍 检测到关键字 '**'，执行处理逻辑\n";
        
        // 获取前后参数值
        std::string leftValue = scanner.peekKeyword(-1);
        std::string rightValue = scanner.peekKeyword(1);
        
        std::cout << "     📍 peekKeyword(-1): \"" << leftValue << "\"\n";
        std::cout << "     📍 peekKeyword(1): \"" << rightValue << "\"\n";
        
        // 匹配参数值
        keyword->args[0].match(leftValue);
        keyword->args[2].match(rightValue);
        
        std::cout << "     ✅ 参数匹配完成\n";
    });
    
    // 3. 转换为JS代码
    std::cout << "  3️⃣ 转换为JS代码...\n";
    keyword->args[0].transform("loadLeft(${value})");
    keyword->args[1].transform("**");
    keyword->args[2].transform("loadRight(${value})");
    
    // 4. 生成最终结果
    std::cout << "  4️⃣ 生成最终结果...\n";
    std::string result = keyword->result();
    std::cout << "     🎉 最终JS代码: " << result << "\n";
    
    std::cout << "✅ 完整流程演示完成\n";
}

// ============================================================================
// 演示7: Policy策略模式
// ============================================================================

void demonstratePolicyStrategies() {
    std::cout << "\n=== 演示7: Policy 策略模式 ===\n";
    
    CJMODScanner scanner;
    
    // 模拟复杂的函数/对象参数场景
    std::vector<std::string> tokens = {
        "printMylove", "(", "{", "width", ":", "()=>", "{", "return", "100", "}", ",",
        "height", ":", "{", "value", ":", "200", "}", "}", ")"
    };
    scanner.setTokens(tokens);
    
    std::cout << "🎭 Policy策略模式演示:\n";
    std::cout << "📋 场景: 处理函数和对象类型的参数值\n";
    
    // COLLECT策略演示
    std::cout << "  1️⃣ COLLECT策略 - 收集函数内容...\n";
    scanner.policyChangeBegin(":", Policy::COLLECT);
    std::cout << "     🔄 策略已切换为 COLLECT（遇到':'后开始收集）\n";
    
    std::string functionContent = scanner.policyChangeEnd("}", Policy::NORMAL);
    std::cout << "     📦 收集的函数内容: \"" << functionContent << "\"\n";
    std::cout << "     🔄 策略已恢复为 NORMAL（遇到'}'后停止收集）\n";
    
    // 对象检测
    std::cout << "  2️⃣ 辅助函数检测...\n";
    std::string testFunction = "()=>{return 100}";
    std::string testObject = "{value:200}";
    
    std::cout << "     🔍 isFunction(\"" << testFunction << "\"): " 
              << (scanner.isFunction(testFunction) ? "是" : "否") << "\n";
    std::cout << "     🔍 isObject(\"" << testObject << "\"): " 
              << (scanner.isObject(testObject) ? "是" : "否") << "\n";
    
    std::cout << "✅ Policy策略演示完成\n";
}

// ============================================================================
// 演示8: CHTLJSFunction高级封装
// ============================================================================

void demonstrateCHTLJSFunction() {
    std::cout << "\n=== 演示8: CHTLJSFunction 高级封装 ===\n";
    
    // 创建高级函数封装
    auto func = createCHTLJSFunction("printMylove", {"url", "mode", "width", "height", "scale"});
    
    std::cout << "🚀 CHTLJSFunction 高级封装演示:\n";
    
    // 链式参数配置
    std::cout << "  🔗 链式参数配置...\n";
    func->bindParam<std::string>("url", [](const std::string& s) {
        std::cout << "     🔧 绑定URL处理器\n";
        return "\"" + s + "\"";
    })->bindParam<std::string>("mode", [](const std::string& s) {
        std::cout << "     🔧 绑定模式处理器\n";
        return s == "ASCII" ? "\"ASCII\"" : "\"Pixel\"";
    })->bindParam<std::string>("width", [](const std::string& s) {
        std::cout << "     🔧 绑定宽度处理器\n";
        return "\"" + s + "\"";
    });
    
    // 匹配参数值
    std::cout << "  📝 匹配参数值...\n";
    func->matchParam("url", "photo.jpg")
        ->matchParam("mode", "ASCII")
        ->matchParam("width", "800px");
    
    // 转换参数
    std::cout << "  🔄 转换参数为JS...\n";
    func->transformParam("url", "loadImage('${value}')")
        ->transformParam("mode", "setMode('${value}')")
        ->transformParam("width", "setWidth('${value}')");
    
    // 生成代码
    std::cout << "  🎉 生成最终代码...\n";
    std::string result = func->generateCode();
    std::cout << "     结果: " << result << "\n";
    
    std::cout << "✅ CHTLJSFunction演示完成\n";
}

// ============================================================================
// 主函数
// ============================================================================

int main() {
    std::cout << "🚀 CHTL CJMOD 核心API架构演示\n";
    std::cout << "基于您的原始设计 + 双指针扫描 + 前置截取架构\n";
    std::cout << "========================================\n";
    
    // 演示各个核心功能
    demonstrateBasicSyntaxAnalysis();      // 基础语法分析
    demonstratePrintMyloveAnalysis();      // 复杂函数分析
    demonstrateParameterBinding();         // 参数绑定
    demonstrateDualPointerScanning();      // 双指针扫描
    demonstrateFrontExtraction();          // 前置截取
    demonstrateCompleteScanning();         // 完整流程
    demonstratePolicyStrategies();         // Policy策略
    demonstrateCHTLJSFunction();           // 高级封装
    
    std::cout << "\n========================================\n";
    std::cout << "🎯 核心架构总结:\n";
    std::cout << "   🏗️  三类核心: Arg + Syntax + CHTLJSFunction\n";
    std::cout << "   🎯  双指针: 前后指针同步扫描，关键字检测通知\n";
    std::cout << "   ✂️  前置截取: 处理复杂语法，避免编译器误解\n";
    std::cout << "   🎭  Policy策略: NORMAL/COLLECT/SKIP 模式切换\n";
    std::cout << "   🔧  完整流程: syntaxAnalys -> bind -> scan -> match -> transform -> result\n";
    std::cout << "   ✨  忠实于您的原始API设计理念！\n";
    
    return 0;
}

/**
 * @brief 编译说明：
 * 
 * g++ -std=c++17 -I../include core_cjmod_demo.cpp -o core_demo
 * ./core_demo
 * 
 * 🎯 这个演示展示了：
 * 1. 您的原始API设计的忠实实现
 * 2. 双指针扫描架构的完整工作流程
 * 3. 前置截取机制的实际应用
 * 4. Policy策略模式的使用场景
 * 5. 三类核心架构的协同工作
 * 
 * 💡 核心特点：
 * - operator() 重载支持函数绑定和值传递
 * - args向量访问：keyword.args[0]
 * - 占位符自动计数：bind("$", func)
 * - 双指针0开始，同步移动，关键字检测
 * - 前置截取避免arg被误发送给编译器
 * - Policy策略处理复杂参数（函数/对象）
 */