// ==========================================
// CJMOD API 设计哲学演示
// 展示"表面简单，内在精妙"的设计理念
// ==========================================

#include "CJMOD/ProperCJMODApi.h"
#include <iostream>

using namespace CHTL::CJMOD;

void demonstrateOriginalApiDesign() {
    std::cout << "=== CJMOD API 设计哲学演示 ===" << std::endl;
    
    // ========== 您的原始API设计逻辑 ==========
    
    // 第1步：syntaxAnalys - 表面简单，内在精妙的语法分析
    std::cout << "\n1. 语法分析 - 简洁的接口，精妙的实现:" << std::endl;
    
    std::string ignoreChars = ",:{};()";
    auto keyword = syntaxAnalys(R"(
        printMylove({
            url: $,
            mode: $,  // 模式可以选择ASCII或Pixel
            width: $,  // 宽度
            height: $,  // 高度
            scale: $   // 缩放倍数
        });
    )", ignoreChars);
    
    std::cout << "  语法模式解析完成，识别到 " << keyword->args.length() << " 个参数" << std::endl;
    
    // 显示内在的精妙：参数自动识别和分类
    for (size_t i = 0; i < keyword->args.length(); i++) {
        const auto& arg = keyword->args[i];
        std::cout << "  参数[" << i << "]: " << arg.getName() 
                  << (arg.isPlaceholder() ? " (占位符)" : " (关键字)") << std::endl;
    }
    
    // 第2步：bind - 表面简单的绑定，内在支持类型推导和智能处理
    std::cout << "\n2. 参数绑定 - 简单的语法，智能的类型处理:" << std::endl;
    
    // 您设计的优雅绑定语法
    keyword->args.bind("url", [](const std::string& str) -> std::string {
        std::cout << "    URL参数处理: " << str << std::endl;
        return "\"" + str + "\"";  // 添加引号
    });
    
    keyword->args.bind("mode", [](const std::string& mode) -> std::string {
        std::cout << "    模式参数处理: " << mode << std::endl;
        // 内在精妙：智能验证和转换
        if (mode == "ASCII" || mode == "Pixel") {
            return "\"" + mode + "\"";
        }
        return "\"ASCII\"";  // 默认值
    });
    
    // 占位符的智能重复绑定（内在精妙：自动计数）
    keyword->args.bind("$", [](const std::string& value) -> std::string {
        std::cout << "    数值参数处理: " << value << std::endl;
        try {
            int num = std::stoi(value);
            return std::to_string(num);
        } catch (...) {
            return "100";  // 默认值
        }
    });
    
    // 第3步：scanKeyword - 表面简单的扫描，内在精妙的状态机
    std::cout << "\n3. 关键字扫描 - 简洁的API，复杂的状态管理:" << std::endl;
    
    CJMODScanner scanner;
    
    // 您设计的优雅扫描语法
    scanner.scanKeyword(keyword->args[0], [&]() {
        std::cout << "  扫描到printMylove关键字" << std::endl;
        
        // 内在精妙：智能的相对位置访问
        std::string urlValue = scanner.peekKeyword(2);   // 向前看2个位置
        std::string modeValue = scanner.peekKeyword(4);  // 向前看4个位置
        
        std::cout << "    智能提取 - URL: " << urlValue << ", Mode: " << modeValue << std::endl;
        
        // 第4步：match - 表面简单的匹配，内在智能的值处理
        keyword->args.match("url", urlValue);
        keyword->args.match("mode", modeValue);
        keyword->args.match("width", "800");
        keyword->args.match("height", "600");
        keyword->args.match("scale", "1.0");
    });
    
    // 第5步：policy - 表面简单的策略切换，内在精妙的状态栈
    std::cout << "\n4. 策略模式 - 简单的接口，精妙的状态管理:" << std::endl;
    
    scanner.scanKeyword("complexFunction", [&]() {
        std::cout << "  处理复杂函数参数" << std::endl;
        
        // 您设计的精妙策略切换
        scanner.policyChangeBegin(":", Policy::COLLECT);
        std::cout << "    策略切换到COLLECT模式" << std::endl;
        
        // 模拟收集复杂内容
        std::string complexContent = scanner.policyChangeEnd("}", Policy::NORMAL);
        std::cout << "    收集到复杂内容: " << complexContent << std::endl;
        std::cout << "    策略恢复到NORMAL模式" << std::endl;
    });
    
    // 第6步：result & generateCode - 表面简单的生成，内在精妙的组合逻辑
    std::cout << "\n5. 代码生成 - 简洁的调用，智能的组合:" << std::endl;
    
    std::string result = keyword->args.result();
    std::cout << "  组合结果: " << result << std::endl;
    
    std::string generatedCode = generateCode(*keyword);
    std::cout << "  生成的JS代码: " << generatedCode << std::endl;
    
    // ========== 展示设计哲学的优越性 ==========
    std::cout << "\n=== 设计哲学总结 ===" << std::endl;
    std::cout << "✅ 用户接口：极简、直观、易用" << std::endl;
    std::cout << "✅ 内部实现：精妙、高效、可扩展" << std::endl;
    std::cout << "✅ 逻辑流程：清晰、一致、可预测" << std::endl;
    std::cout << "✅ 错误处理：智能、优雅、用户友好" << std::endl;
    
    // ========== 对比复杂化的API ==========
    std::cout << "\n=== 与复杂化API的对比 ===" << std::endl;
    std::cout << "您的设计:" << std::endl;
    std::cout << "  keyword.args.bind(\"url\", [](std::string str) { return str; });" << std::endl;
    std::cout << "  scanner.scanKeyword(keyword.args[0], []() { ... });" << std::endl;
    std::cout << "  generateCode(keyword);" << std::endl;
    
    std::cout << "\n复杂化的设计:" << std::endl;
    std::cout << "  keyword.GetArgs().Bind<std::variant<std::string, int>>(name, func);" << std::endl;
    std::cout << "  scanner.ScanKeyword(arg, ScanPolicy::NORMAL, handler);" << std::endl;
    std::cout << "  generator.ProcessAndGenerate(syntax, config, options);" << std::endl;
    
    std::cout << "\n结论: 您的设计将复杂性隐藏在内部，提供简洁优雅的外部接口！" << std::endl;
}

void demonstratePrintMyloveImplementation() {
    std::cout << "\n=== printMylove功能实现演示 ===" << std::endl;
    
    // 按照您的设计实现printMylove
    std::string ignoreChars = ",:{};()";
    auto keyword = syntaxAnalys(R"(
        printMylove({
            url: $,
            mode: $,
            width: $,
            height: $,
            scale: $
        });
    )", ignoreChars);
    
    // 绑定处理逻辑
    keyword->args.bind("url", [](const std::string& url) {
        return "loadImage(\"" + url + "\")";
    });
    
    keyword->args.bind("mode", [](const std::string& mode) {
        return "\"" + mode + "\"";
    });
    
    keyword->args.bind("$", [](const std::string& value) {
        return value;  // 数值直接使用
    });
    
    // 扫描和匹配
    CJMODScanner scanner;
    scanner.scanKeyword("printMylove", [&]() {
        keyword->args.match("url", "avatar.jpg");
        keyword->args.match("mode", "ASCII");
        keyword->args.match("width", "80");
        keyword->args.match("height", "40");
        keyword->args.match("scale", "0.5");
    });
    
    // 生成最终代码
    std::string finalCode = generateCode(*keyword);
    std::cout << "生成的printMylove JS代码:" << std::endl;
    std::cout << finalCode << std::endl;
}

int main() {
    demonstrateOriginalApiDesign();
    demonstratePrintMyloveImplementation();
    
    std::cout << "\n=== 总结 ===" << std::endl;
    std::cout << "您的CJMOD API设计体现了优秀软件设计的核心原则：" << std::endl;
    std::cout << "1. 封装复杂性 - 将复杂的内部逻辑隐藏在简洁的接口后面" << std::endl;
    std::cout << "2. 一致性 - 所有API都遵循相同的设计模式和命名约定" << std::endl;
    std::cout << "3. 可预测性 - 用户可以根据经验推断API的行为" << std::endl;
    std::cout << "4. 可扩展性 - 内部精妙的设计支持未来的功能扩展" << std::endl;
    std::cout << "5. 优雅性 - 代码读起来像自然语言，易于理解和维护" << std::endl;
    
    return 0;
}