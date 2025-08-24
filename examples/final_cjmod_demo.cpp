#include "../include/CJMOD/FinalCJMODApi.h"
#include <iostream>

using namespace CHTL::CJMOD;

/**
 * @brief 🎯 最终版本CJMOD API演示
 * 基于您的原始API文档，三个核心类：Arg + Syntax + CHTLJSFunction
 */

// 全局CJMOD处理器实例
auto processor = createCJMODProcessor();

// ============================================================================
// 演示1: 基础语法分析 - 忠实原始API
// ============================================================================

void demonstrateBasicSyntaxAnalysis() {
    std::cout << "\n=== 📝 基础语法分析 - 原始API ===\n";
    
    // 🔥 syntaxAnalys函数用于分析CHTL JS语法，并分解成多个arg
    const auto keyword = syntaxAnalys("arg ** arg2");
    
    std::cout << "语法分析结果：\n";
    std::cout << keyword->args[0] << std::endl;  // arg
    std::cout << keyword->args[1] << std::endl;  // **
    std::cout << keyword->args[2] << std::endl;  // arg2
    
    std::cout << "✅ 完全按照原始API设计！\n";
}

// ============================================================================
// 演示2: 占位符支持 - 原始API
// ============================================================================

void demonstratePlaceholders() {
    std::cout << "\n=== 💲 占位符支持 - 原始API ===\n";
    
    // 🔥 你可以使用$表示占位符，使用占位符能够更好地处理参数问题
    const auto keyword2 = syntaxAnalys("$ ** $");
    
    std::cout << "占位符分析结果：\n";
    std::cout << keyword2->args[0] << std::endl;  // $
    std::cout << keyword2->args[1] << std::endl;  // **
    std::cout << keyword2->args[2] << std::endl;  // $
    
    std::cout << "✅ 占位符让语法更通用！\n";
}

// ============================================================================
// 演示3: 复杂语法分析 - printMylove原始示例
// ============================================================================

void demonstratePrintMylove() {
    std::cout << "\n=== 🖼️ printMylove - 复杂语法分析 ===\n";
    
    // 🔥 ignoreChars: 这个字符数组/string包含的字符将不会被视为arg
    std::string str = ",:{};()";   // 需要忽略的符号
    const auto keyword = syntaxAnalys(R"(
        printMylove({
            url: $,
            mode: $,  // 模式可以选择ASCII或Pixel
            width: $,  // 宽度，支持的单位有CSS单位以及百分比，小数，纯数字(像素)
            height: $,  // 高度
            scale:  $,  // 缩放倍数，限定为等比缩放策略
        });
    )", str);
    
    std::cout << "printMylove语法分析完成，参数数量: " << keyword->length() << "\n";
    
    for (size_t i = 0; i < keyword->args.size(); ++i) {
        std::cout << "args[" << i << "] = " << keyword->args[i] << "\n";
    }
    
    std::cout << "✅ 复杂语法自动分解完成！\n";
}

// ============================================================================
// 演示4: bind函数 - 原始API
// ============================================================================

void demonstrateBind() {
    std::cout << "\n=== 🔗 bind函数 - 参数绑定 ===\n";
    
    std::string str = ",:{};()";
    const auto keyword = syntaxAnalys(R"(
        printMylove({
            url: $,
            mode: $,
            width: $,
            height: $,
            scale: $
        });
    )", str);
    
    // 🔥 bind函数会自动搜索相关的参数并绑定函数
    // 对于占位符，你需要多次重复绑定，bind函数会自动计数占位符
    std::cout << "绑定占位符处理函数：\n";
    
    keyword->bind<std::string>("$", [](const std::string& str) {
        std::cout << "  处理URL: " << str << "\n";
        return "\"" + str + "\"";  // 自动加引号
    });
    
    keyword->bind<std::string>("$", [](const std::string& str) {
        std::cout << "  处理模式: " << str << "\n";
        return str == "ASCII" ? "\"ASCII\"" : "\"Pixel\"";
    });
    
    keyword->bind<std::string>("$", [](const std::string& str) {
        std::cout << "  处理宽度: " << str << "\n";
        return str.find("px") != std::string::npos ? ("\"" + str + "\"") : str;
    });
    
    keyword->bind<std::string>("$", [](const std::string& str) {
        std::cout << "  处理高度: " << str << "\n";
        return str.find("px") != std::string::npos ? ("\"" + str + "\"") : str;
    });
    
    keyword->bind<std::string>("$", [](const std::string& str) {
        std::cout << "  处理缩放: " << str << "\n";
        return str;
    });
    
    // 🔥 也可以通过索引去绑定
    // keyword.args[1].bind([]() { return x; });
    
    std::cout << "✅ 参数绑定函数设置完成！\n";
}

// ============================================================================
// 演示5: transform函数 - 转换为JS代码
// ============================================================================

void demonstrateTransform() {
    std::cout << "\n=== 🔄 transform函数 - JS代码转换 ===\n";
    
    std::string str = ",:{};()";
    const auto keyword = syntaxAnalys("printMylove({ url: $, mode: $ });", str);
    
    // 🔥 transform函数用于将arg转变为JS代码
    std::cout << "设置JS代码转换模板：\n";
    
    keyword->transform("url", "loadImage('${value}')");
    keyword->transform("mode", "setRenderMode('${value}')");
    
    // 🔥 也可以通过索引转换
    // keyword.args[0].transform("URL url = new URL()");
    
    std::cout << "  ✓ URL参数 -> loadImage('${value}')\n";
    std::cout << "  ✓ 模式参数 -> setRenderMode('${value}')\n";
    std::cout << "✅ JS代码转换模板设置完成！\n";
}

// ============================================================================
// 演示6: scanKeyword - 扫描器核心接口
// ============================================================================

void demonstrateScanKeyword() {
    std::cout << "\n=== 🔍 scanKeyword - 扫描器核心 ===\n";
    
    const auto keyword = syntaxAnalys("arg ** arg2");
    
    // 🔥 scanKeyword函数是扫描器暴露的接口，是扩展CHTL JS的核心
    std::cout << "设置扫描关键字和处理逻辑：\n";
    
    processor->scanKeyword(keyword->args[1], []() {
        std::cout << "  🎯 检测到关键字 '**'！\n";
        std::cout << "  📍 左参数: " << processor->peekKeyword(-1) << "\n";
        std::cout << "  📍 右参数: " << processor->peekKeyword(1) << "\n";
        
        // 🔥 keyword.arg[0](peekKeyword(-1)); // 把内容传递给参数
        // 🔥 keyword.arg[2](peekKeyword(1));
    });
    
    std::cout << "✅ 扫描器配置完成！这是CHTL JS语法得到支持的关键一步\n";
}

// ============================================================================
// 演示7: match函数 - 参数值匹配
// ============================================================================

void demonstrateMatch() {
    std::cout << "\n=== 🎯 match函数 - 参数值匹配 ===\n";
    
    std::string str = ",:{};()";
    const auto keyword = syntaxAnalys("printMylove({ url: $, mode: $ });", str);
    
    // 先绑定处理函数
    keyword->bind<std::string>("$", [](const std::string& s) { return "\"" + s + "\""; });
    keyword->bind<std::string>("$", [](const std::string& s) { return "\"" + s + "\""; });
    
    // 🔥 match函数用于匹配相应的arg，传递值并调用函数
    std::cout << "匹配参数值（模拟从peekKeyword获得）：\n";
    
    keyword->match("url", "photo.jpg");     // 模拟 peekKeyword(-1)
    keyword->match("mode", "ASCII");        // 模拟 peekKeyword(1)
    
    // 🔥 也可以直接使用match
    // keyword.args[0].match(peekKeyword(-1));
    
    std::cout << "  ✓ URL匹配: photo.jpg\n";
    std::cout << "  ✓ 模式匹配: ASCII\n";
    std::cout << "✅ 参数值匹配完成！\n";
}

// ============================================================================
// 演示8: policy策略 - 处理复杂参数
// ============================================================================

void demonstratePolicy() {
    std::cout << "\n=== 🎭 policy策略 - 处理复杂参数 ===\n";
    
    // 🔥 policy系列函数用于改变扫描政策，在处理复杂的参数值时会很有用
    std::cout << "策略模式演示：\n";
    std::cout << "  NORMAL - 常规\n";
    std::cout << "  COLLECT - 收集\n";
    std::cout << "  SKIP - 跳过\n";
    
    // 模拟复杂参数处理场景
    std::cout << "\n模拟处理函数/对象参数：\n";
    
    processor->scanKeyword("printMylove", []() {
        std::cout << "  🔄 策略切换演示：\n";
        
        // 🔥 遇到":"后，策略更改，收集所有的内容
        processor->policyChangeBegin(":", Policy::COLLECT);
        std::cout << "     策略切换为 COLLECT（收集模式）\n";
        
        // 🔥 遇到"}"后，函数结束，停止收集，并返回string
        std::string result = processor->policyChangeEnd("}", Policy::NORMAL);
        std::cout << "     策略恢复为 NORMAL，收集结果: " << result << "\n";
    });
    
    std::cout << "✅ Policy策略系统配置完成！\n";
}

// ============================================================================
// 演示9: result函数 - 组合JS代码
// ============================================================================

void demonstrateResult() {
    std::cout << "\n=== 🎉 result函数 - 组合JS代码 ===\n";
    
    std::string str = ",:{};()";
    const auto keyword = syntaxAnalys("printMylove({ url: $, mode: $ });", str);
    
    // 设置处理和转换
    keyword->bind<std::string>("$", [](const std::string& s) { return "\"" + s + "\""; });
    keyword->bind<std::string>("$", [](const std::string& s) { return "\"" + s + "\""; });
    
    keyword->transform("url", "loadImage(${value})");
    keyword->transform("mode", "setMode(${value})");
    
    // 匹配参数值
    keyword->match("url", "photo.jpg");
    keyword->match("mode", "ASCII");
    
    // 🔥 result函数用于将转变为JS代码后的arg组合起来
    std::string jsCode = keyword->result();
    
    std::cout << "组合后的JS代码: " << jsCode << "\n";
    std::cout << "✅ JS代码组合完成！\n";
}

// ============================================================================
// 演示10: generateCode - 生成标准JS
// ============================================================================

void demonstrateGenerateCode() {
    std::cout << "\n=== ✨ generateCode - 生成标准JS ===\n";
    
    std::string str = ",:{};()";
    const auto keyword = syntaxAnalys("printMylove({ url: $, mode: $ });", str);
    
    // 完整设置
    keyword->bind<std::string>("$", [](const std::string& s) { return "\"" + s + "\""; });
    keyword->bind<std::string>("$", [](const std::string& s) { return "\"" + s + "\""; });
    keyword->transform("url", "loadImage(${value})");
    keyword->transform("mode", "setRenderMode(${value})");
    keyword->match("url", "photo.jpg");
    keyword->match("mode", "ASCII");
    
    // 🔥 generateCode函数用于生成JS代码
    std::string finalCode = generateCode(*keyword);
    
    std::cout << "最终标准JS代码: " << finalCode << "\n";
    std::cout << "✅ 标准JS代码生成完成！\n";
}

// ============================================================================
// 演示11: 辅助函数
// ============================================================================

void demonstrateHelperFunctions() {
    std::cout << "\n=== 🛠️ 辅助函数 ===\n";
    
    // 🔥 isObject - 辅助函数，判断是不是对象
    std::string objTest = "{name: 'test', value: 123}";
    bool isObj = processor->isObject(objTest);
    std::cout << "isObject(\"" << objTest << "\"): " << (isObj ? "是" : "否") << "\n";
    
    // 🔥 isFunction - 辅助函数，判断是不是函数
    std::string funcTest = "() => { return 'hello'; }";
    bool isFunc = processor->isFunction(funcTest);
    std::cout << "isFunction(\"" << funcTest << "\"): " << (isFunc ? "是" : "否") << "\n";
    
    // 🔥 slice - 辅助函数，切割字符串（处理函数与对象）
    std::string sliceTest = "Hello, World!";
    std::string sliced = processor->slice(sliceTest, 7, 12);
    std::cout << "slice(\"" << sliceTest << "\", 7, 12): \"" << sliced << "\"\n";
    
    std::cout << "✅ 辅助函数演示完成！\n";
}

// ============================================================================
// 演示12: 完整流程 - 按照原始API流程
// ============================================================================

void demonstrateCompleteWorkflow() {
    std::cout << "\n=== 🚀 完整流程 - 原始API流程 ===\n";
    
    std::cout << "🔧 执行完整的7步流程：\n";
    
    // 1. syntaxAnalys(拆分成多个arg)
    std::cout << "1️⃣ syntaxAnalys - 拆分成多个arg\n";
    std::string str = ",:{};()";
    const auto keyword = syntaxAnalys("printMylove({ url: $, mode: $ });", str);
    
    // 2. bind(绑定获取值的函数)
    std::cout << "2️⃣ bind - 绑定获取值的函数\n";
    keyword->bind<std::string>("$", [](const std::string& s) { return "\"" + s + "\""; });
    keyword->bind<std::string>("$", [](const std::string& s) { return "\"" + s + "\""; });
    
    // 3. transform(最终的JS代码)
    std::cout << "3️⃣ transform - 最终的JS代码\n";
    keyword->transform("url", "loadImage(${value})");
    keyword->transform("mode", "setRenderMode(${value})");
    
    // 4. scanKeyword(什么关键字作为统一扫描器的搜索)
    std::cout << "4️⃣ scanKeyword - 设置统一扫描器的搜索关键字\n";
    processor->scanKeyword("printMylove", []() {
        std::cout << "     🎯 统一扫描器找到关键字！\n";
    });
    
    // 5. match(捕获参数值，要拿到代码片段才能使用peekKeyword)
    std::cout << "5️⃣ match - 捕获参数值（来自代码片段peekKeyword）\n";
    keyword->match("url", "photo.jpg");  // 模拟 peekKeyword(-1)
    keyword->match("mode", "ASCII");     // 模拟 peekKeyword(1)
    
    // 6. result(组合所有参数的最终JS代码)
    std::cout << "6️⃣ result - 组合所有参数的最终JS代码\n";
    std::string jsCode = keyword->result();
    
    // 7. generateCode(生成器输出标准JS)
    std::cout << "7️⃣ generateCode - 生成器输出标准JS\n";
    std::string finalCode = generateCode(*keyword);
    
    std::cout << "\n🎉 完整流程执行结果:\n";
    std::cout << "   JS代码: " << jsCode << "\n";
    std::cout << "   标准JS: " << finalCode << "\n";
    std::cout << "✅ 完整的7步流程执行完成！\n";
}

// ============================================================================
// 主函数
// ============================================================================

int main() {
    std::cout << "🎯 CJMOD API - 最终版本演示\n";
    std::cout << "基于原始API文档的三个核心类：Arg + Syntax + CHTLJSFunction\n";
    std::cout << "===============================================\n";
    
    // 按照原始API文档的顺序演示
    demonstrateBasicSyntaxAnalysis();   // 基础语法分析
    demonstratePlaceholders();          // 占位符
    demonstratePrintMylove();          // 复杂语法
    demonstrateBind();                 // bind函数
    demonstrateTransform();            // transform函数
    demonstrateScanKeyword();          // scanKeyword
    demonstrateMatch();                // match函数
    demonstratePolicy();               // policy策略
    demonstrateResult();               // result函数
    demonstrateGenerateCode();         // generateCode
    demonstrateHelperFunctions();      // 辅助函数
    demonstrateCompleteWorkflow();     // 完整流程
    
    std::cout << "\n===============================================\n";
    std::cout << "🎯 三个核心类总结：\n";
    std::cout << "   🏗️ Arg类 - 参数处理核心\n";
    std::cout << "      └── operator(), bind(), match(), transform()\n";
    std::cout << "   📦 Syntax类 - 语法容器和主控制器\n";
    std::cout << "      └── args[], bind(), match(), transform(), result(), length()\n";
    std::cout << "   🚀 CHTLJSFunction类 - 完整处理器\n";
    std::cout << "      └── syntaxAnalys(), scanKeyword(), peekKeyword(), policy系列, generateCode(), 辅助函数\n";
    std::cout << "\n💡 API使用起来简单，自动的语法分析，\n";
    std::cout << "   通过手动match将代码片段的value填入需要的位置，\n";
    std::cout << "   简单但是又功能强大！\n";
    
    return 0;
}

/**
 * @brief 🎯 最终版本特点：
 * 
 * 【三个核心类】
 * 1. Arg - 参数处理核心
 * 2. Syntax - 语法容器和主控制器  
 * 3. CHTLJSFunction - 完整的CJMOD处理器
 * 
 * 【完全忠实原始API】
 * - 所有函数签名和用法完全按照您的文档
 * - 支持所有原始API的特性和语法
 * - 保持简单易用的特点
 * 
 * 【7步完整流程】
 * syntaxAnalys -> bind -> transform -> scanKeyword -> match -> result -> generateCode
 * 
 * 【核心优势】
 * - 简单：API使用起来简单
 * - 自动：自动的语法分析
 * - 强大：功能强大的处理能力
 * - 精准：手动match精确填值
 */