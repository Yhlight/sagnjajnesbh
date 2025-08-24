#include "../include/CJMOD/FinalCJMODApi.h"
#include <iostream>
#include <vector>

using namespace CHTL::CJMOD;

/**
 * @brief 🚀 增强版CJMOD API演示
 * 
 * 展示：
 * 1. 简化流程 - 快速构建CHTL JS函数
 * 2. 虚函数支持系统
 * 3. 高级扫描机制
 * 4. 核心机制集成
 */

// 全局处理器
auto processor = createCJMODProcessor();

// ============================================================================
// 演示1: 简化流程 - 一行代码快速构建
// ============================================================================

void demonstrateQuickBuilder() {
    std::cout << "\n=== 🚀 简化流程 - 快速构建 ===\n";
    
    // 🔥 传统方式 vs 简化方式对比
    std::cout << "📊 传统方式 vs 简化方式对比：\n";
    
    // 传统方式（7步流程）
    std::cout << "\n🔧 传统方式（7步）：\n";
    auto keyword = syntaxAnalys("printMylove($, $, $)", ",:{};()");
    keyword->bind<std::string>("$", [](const std::string& s) { return "\"" + s + "\""; });
    keyword->bind<std::string>("$", [](const std::string& s) { return "\"" + s + "\""; });
    keyword->bind<std::string>("$", [](const std::string& s) { return s; });
    keyword->match("$", "photo.jpg");
    keyword->match("$", "ASCII");
    keyword->match("$", "2.5");
    std::string traditional = keyword->result();
    std::cout << "   结果: " << traditional << "\n";
    
    // 🔥 简化方式（1行）
    std::cout << "\n⚡ 简化方式（1行）：\n";
    auto quick = QuickBuilder::create("printMylove($, $, $)")
                              .values("photo.jpg", "ASCII", "2.5")
                              .build();
    std::cout << "   结果: " << quick << "\n";
    
    // 🔥 宏方式（更简单）
    std::cout << "\n✨ 宏方式（最简单）：\n";
    auto macro = QUICK_CJMOD("printMylove($, $, $)", "photo.jpg", "ASCII", "2.5");
    std::cout << "   结果: " << macro << "\n";
    
    std::cout << "💡 简化流程让复杂构建变成一行代码！\n";
}

// ============================================================================
// 演示2: 高级快速构建 - 带模板
// ============================================================================

void demonstrateAdvancedQuickBuilder() {
    std::cout << "\n=== 🎨 高级快速构建 - 带模板 ===\n";
    
    // 🔥 带JS模板的快速构建
    auto advanced = QuickBuilder::create("createUser($, $, $)")
                                .values("Chtholly", "18", "chtholly@example.com")
                                .templates({
                                    "setName('${value}')",
                                    "setAge(${value})",
                                    "setEmail('${value}')"
                                })
                                .build();
    
    std::cout << "高级快速构建结果: " << advanced << "\n";
    
    // 🔥 批量快速构建
    std::cout << "\n📦 批量处理演示：\n";
    std::vector<std::string> patterns = {
        "printMylove($, $)",
        "iNeverAway($, $)",
        "createUser($, $)"
    };
    
    std::vector<std::vector<std::string>> valuesList = {
        {"photo1.jpg", "ASCII"},
        {"Chtholly", "3.14"},
        {"Alice", "25"}
    };
    
    auto batchResults = processor->getCoreSystem().batchProcess(patterns, valuesList);
    
    for (size_t i = 0; i < batchResults.size(); ++i) {
        std::cout << "   批量结果[" << i << "]: " << batchResults[i] << "\n";
    }
    
    std::cout << "✅ 高级快速构建完成！\n";
}

// ============================================================================
// 演示3: 虚函数支持系统
// ============================================================================

void demonstrateVirtualFunctions() {
    std::cout << "\n=== 🎭 虚函数支持系统 ===\n";
    
    auto& virtualManager = processor->getCoreSystem().getVirtualFunctionManager();
    
    // 🔥 注册自定义虚函数
    std::cout << "📝 注册虚函数：\n";
    
    VirtualFunctionInfo callbackInfo;
    callbackInfo.name = "onClick";
    callbackInfo.type = VirtualFunctionType::CALLBACK;
    callbackInfo.parameters = {"event"};
    callbackInfo.jsTemplate = "function(${params}) { ${body} }";
    virtualManager.registerVirtualFunction("onClick", callbackInfo);
    
    VirtualFunctionInfo asyncInfo;
    asyncInfo.name = "fetchData";
    asyncInfo.type = VirtualFunctionType::ASYNC_FUNCTION;
    asyncInfo.parameters = {"url", "options"};
    asyncInfo.isAsync = true;
    asyncInfo.jsTemplate = "async function(${params}) { ${body} }";
    virtualManager.registerVirtualFunction("fetchData", asyncInfo);
    
    std::cout << "   ✓ 注册回调函数: onClick\n";
    std::cout << "   ✓ 注册异步函数: fetchData\n";
    
    // 🔥 创建虚函数实例
    std::cout << "\n🎯 创建虚函数实例：\n";
    
    auto callback = virtualManager.createVirtualFunction("onClick", {"event"});
    std::cout << "   回调函数: " << callback << "\n";
    
    auto asyncFunc = virtualManager.createVirtualFunction("fetchData", {"'/api/data'", "{method: 'GET'}"});
    std::cout << "   异步函数: " << asyncFunc << "\n";
    
    // 🔥 使用宏创建虚函数
    std::cout << "\n✨ 宏方式创建：\n";
    auto macroCallback = VIRTUAL_FUNC("onClick", VirtualFunctionType::CALLBACK, "event");
    std::cout << "   宏回调函数: " << macroCallback << "\n";
    
    // 🔥 检查虚函数
    std::cout << "\n🔍 虚函数检查：\n";
    std::cout << "   onClick是虚函数: " << (virtualManager.isVirtualFunction("onClick") ? "是" : "否") << "\n";
    std::cout << "   normalFunc是虚函数: " << (virtualManager.isVirtualFunction("normalFunc") ? "是" : "否") << "\n";
    
    std::cout << "✅ 虚函数系统演示完成！\n";
}

// ============================================================================
// 演示4: 高级扫描机制
// ============================================================================

void demonstrateAdvancedScanning() {
    std::cout << "\n=== 🔍 高级扫描机制 ===\n";
    
    auto& scanner = processor->getCoreSystem().getAdvancedScanner();
    
    // 测试代码
    std::string sourceCode = R"(
        printMylove({
            url: "photo.jpg",
            mode: "ASCII",
            callback: () => { console.log("done"); }
        });
        
        iNeverAway("Chtholly", 3.14);
        
        createUser({
            name: "Alice",
            age: 25,
            onClick: function(event) { alert("clicked"); }
        });
    )";
    
    // 🔥 多模式扫描
    std::cout << "🎯 多模式扫描：\n";
    std::vector<std::string> keywords = {"printMylove", "iNeverAway", "createUser"};
    auto results = scanner.multiModeScan(sourceCode, keywords);
    
    for (const auto& result : results) {
        if (result.found) {
            std::cout << "   找到关键字: " << result.keyword 
                      << " (位置: " << result.position << ")\n";
            std::cout << "     前文: " << result.beforeKeyword.substr(0, 20) << "...\n";
            std::cout << "     后文: " << result.afterKeyword.substr(0, 20) << "...\n";
        }
    }
    
    // 🔥 上下文感知扫描
    std::cout << "\n🧠 上下文感知扫描：\n";
    auto contextResult = scanner.contextAwareScan(sourceCode, "printMylove", "function_call");
    if (contextResult.found) {
        std::cout << "   上下文扫描成功，关键字: " << contextResult.keyword << "\n";
        std::cout << "   上下文信息: " << contextResult.context.size() << " 个元素\n";
    }
    
    // 🔥 嵌套结构扫描
    std::cout << "\n🏗️ 嵌套结构扫描：\n";
    auto nestedResults = scanner.nestedStructureScan(sourceCode, "{", "}");
    std::cout << "   发现 " << nestedResults.size() << " 个嵌套结构\n";
    
    for (size_t i = 0; i < nestedResults.size() && i < 3; ++i) {
        std::cout << "     结构[" << i << "]: 位置 " << nestedResults[i].position << "\n";
    }
    
    // 🔥 智能前置截取
    std::cout << "\n✂️ 智能前置截取：\n";
    auto extractResult = scanner.intelligentFrontExtract(sourceCode, "printMylove", 2);
    if (extractResult.success) {
        std::cout << "   截取成功！\n";
        std::cout << "   截取内容: " << extractResult.extracted.substr(0, 30) << "...\n";
    }
    
    // 🔥 宏方式扫描
    std::cout << "\n✨ 宏方式扫描：\n";
    auto macroResults = ADVANCED_SCAN(sourceCode, keywords);
    std::cout << "   宏扫描找到 " << macroResults.size() << " 个结果\n";
    
    std::cout << "✅ 高级扫描机制演示完成！\n";
}

// ============================================================================
// 演示5: 扫描器类型切换
// ============================================================================

void demonstrateScannerTypes() {
    std::cout << "\n=== ⚙️ 扫描器类型切换 ===\n";
    
    auto& scanner = processor->getCoreSystem().getAdvancedScanner();
    std::string testCode = "printMylove({ url: $, mode: $ });";
    
    // 🔥 测试不同扫描器类型
    std::vector<ScannerType> types = {
        ScannerType::DUAL_POINTER,
        ScannerType::REGEX_SCANNER,
        ScannerType::CONTEXT_SCANNER,
        ScannerType::HYBRID_SCANNER
    };
    
    std::vector<std::string> typeNames = {
        "双指针扫描器",
        "正则表达式扫描器", 
        "上下文扫描器",
        "混合扫描器"
    };
    
    for (size_t i = 0; i < types.size(); ++i) {
        std::cout << "🔧 切换到: " << typeNames[i] << "\n";
        scanner.setScannerType(types[i]);
        
        auto result = scanner.contextAwareScan(testCode, "printMylove");
        if (result.found) {
            std::cout << "   ✓ 扫描成功，位置: " << result.position << "\n";
        } else {
            std::cout << "   ✗ 扫描失败\n";
        }
    }
    
    std::cout << "✅ 扫描器类型切换演示完成！\n";
}

// ============================================================================
// 演示6: 智能功能
// ============================================================================

void demonstrateSmartFeatures() {
    std::cout << "\n=== 🧠 智能功能 ===\n";
    
    auto& coreSystem = processor->getCoreSystem();
    
    // 🔥 智能函数检测
    std::cout << "🔍 智能函数检测：\n";
    
    std::vector<std::string> testFunctions = {
        "printMylove({ url: 'test' })",
        "normalFunction()",
        "() => { return 'lambda'; }",
        "async function fetchData() { }"
    };
    
    for (const auto& func : testFunctions) {
        bool isSmart = coreSystem.isSmartFunction(func);
        std::cout << "   \"" << func.substr(0, 20) << "...\" 是智能函数: " 
                  << (isSmart ? "是" : "否") << "\n";
    }
    
    // 🔥 智能参数推导
    std::cout << "\n🎯 智能参数推导：\n";
    auto inferredParams = processor->smartParameterInference("createUser($, $, $)");
    std::cout << "   推导出 " << inferredParams.size() << " 个参数：\n";
    for (size_t i = 0; i < inferredParams.size(); ++i) {
        std::cout << "     参数[" << i << "]: " << inferredParams[i] << "\n";
    }
    
    // 🔥 自动代码优化
    std::cout << "\n⚡ 自动代码优化：\n";
    std::string rawJS = "function   test (  a,b  ) {  return a+b;  }";
    std::string optimized = processor->autoOptimize(rawJS);
    std::cout << "   原始代码: " << rawJS << "\n";
    std::cout << "   优化代码: " << optimized << "\n";
    
    // 🔥 自动扫描策略优化
    std::cout << "\n🎛️ 自动扫描策略优化：\n";
    std::string complexCode = R"(
        // 复杂嵌套代码
        printMylove({
            callback: () => {
                iNeverAway("nested", {
                    deep: function() { return "very deep"; }
                });
            }
        });
    )";
    
    coreSystem.optimizeScanningStrategy(complexCode);
    std::cout << "   ✓ 已自动优化扫描策略以处理复杂嵌套代码\n";
    
    std::cout << "✅ 智能功能演示完成！\n";
}

// ============================================================================
// 演示7: 完整工作流程 - 简化版 vs 标准版
// ============================================================================

void demonstrateCompleteWorkflows() {
    std::cout << "\n=== 🔄 完整工作流程对比 ===\n";
    
    // 🔥 场景：创建一个复杂的用户界面组件
    std::cout << "📋 场景：创建复杂的用户界面组件\n";
    
    // 标准7步流程
    std::cout << "\n🔧 标准流程（传统7步）：\n";
    auto start = std::chrono::high_resolution_clock::now();
    
    auto syntax = syntaxAnalys(R"(
        createComponent({
            name: $,
            props: { width: $, height: $ },
            events: { onClick: $, onHover: $ }
        })
    )", ",:{};()");
    
    syntax->bind<std::string>("$", [](const std::string& s) { return "\"" + s + "\""; });
    syntax->bind<std::string>("$", [](const std::string& s) { return s; });
    syntax->bind<std::string>("$", [](const std::string& s) { return s; });
    syntax->bind<std::string>("$", [](const std::string& s) { return s; });
    syntax->bind<std::string>("$", [](const std::string& s) { return s; });
    
    syntax->transform("name", "setComponentName(${value})");
    syntax->transform("width", "setWidth(${value})");
    syntax->transform("height", "setHeight(${value})");
    syntax->transform("onClick", "bindClickEvent(${value})");
    syntax->transform("onHover", "bindHoverEvent(${value})");
    
    processor->scanKeyword("createComponent", []() {
        std::cout << "     🎯 扫描器找到组件创建关键字\n";
    });
    
    syntax->match("$", "MyButton");
    syntax->match("$", "100");
    syntax->match("$", "50");
    syntax->match("$", "() => alert('clicked')");
    syntax->match("$", "() => console.log('hover')");
    
    std::string standardResult = syntax->result();
    std::string finalStandard = generateCode(*syntax);
    
    auto end = std::chrono::high_resolution_clock::now();
    auto standardTime = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
    
    std::cout << "   结果: " << finalStandard.substr(0, 50) << "...\n";
    std::cout << "   耗时: " << standardTime << " 微秒\n";
    
    // 🔥 简化流程
    std::cout << "\n⚡ 简化流程（1行）：\n";
    start = std::chrono::high_resolution_clock::now();
    
    auto quickResult = QUICK_CJMOD(
        "createComponent({ name: $, props: { width: $, height: $ }, events: { onClick: $, onHover: $ } })",
        "MyButton", "100", "50", "() => alert('clicked')", "() => console.log('hover')"
    );
    
    end = std::chrono::high_resolution_clock::now();
    auto quickTime = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
    
    std::cout << "   结果: " << quickResult.substr(0, 50) << "...\n";
    std::cout << "   耗时: " << quickTime << " 微秒\n";
    
    // 性能对比
    std::cout << "\n📊 性能对比：\n";
    std::cout << "   标准流程: " << standardTime << " 微秒\n";
    std::cout << "   简化流程: " << quickTime << " 微秒\n";
    std::cout << "   性能提升: " << (standardTime > quickTime ? 
                     std::to_string((standardTime - quickTime) * 100 / standardTime) + "%" : "相当") << "\n";
    
    std::cout << "✅ 工作流程对比完成！\n";
}

// ============================================================================
// 主函数
// ============================================================================

int main() {
    std::cout << "🚀 增强版CJMOD API - 完整功能演示\n";
    std::cout << "==========================================\n";
    std::cout << "🎯 新增功能：简化流程 + 虚函数支持 + 高级扫描机制\n";
    std::cout << "🏗️ 三个核心类：Arg + Syntax + CHTLJSFunction\n";
    std::cout << "⚡ 核心理念：保持简单，功能强大\n\n";
    
    // 演示所有新功能
    demonstrateQuickBuilder();          // 简化流程
    demonstrateAdvancedQuickBuilder();  // 高级快速构建
    demonstrateVirtualFunctions();      // 虚函数支持
    demonstrateAdvancedScanning();      // 高级扫描机制
    demonstrateScannerTypes();          // 扫描器类型
    demonstrateSmartFeatures();         // 智能功能
    demonstrateCompleteWorkflows();     // 完整工作流程对比
    
    std::cout << "\n==========================================\n";
    std::cout << "🎯 增强版CJMOD API总结：\n";
    std::cout << "   ⚡ 简化流程：1行代码 vs 传统7步\n";
    std::cout << "   🎭 虚函数支持：回调、异步、生成器等\n";
    std::cout << "   🔍 高级扫描：多模式、上下文感知、智能截取\n";
    std::cout << "   🧠 智能功能：参数推导、代码优化、策略自适应\n";
    std::cout << "   📦 批量处理：高效处理多个函数\n";
    std::cout << "   🎛️ 灵活配置：多种扫描器类型可切换\n";
    std::cout << "\n💡 核心优势：\n";
    std::cout << "   - 保持原有API的简单性和强大功能\n";
    std::cout << "   - 新增简化流程，大幅提升开发效率\n";
    std::cout << "   - 虚函数支持让CHTL JS更加灵活\n";
    std::cout << "   - 高级扫描机制处理复杂语法结构\n";
    std::cout << "   - 智能功能自动优化开发体验\n";
    std::cout << "\n🚀 这就是完整的CJMOD API生态系统！\n";
    
    return 0;
}

/**
 * @brief 🎯 增强版特点总结：
 * 
 * 【保持核心】
 * - 三个核心类：Arg + Syntax + CHTLJSFunction
 * - 原始API完全兼容：所有原有功能保持不变
 * - 7步标准流程：依然支持完整的控制
 * 
 * 【简化流程】
 * - QuickBuilder：链式调用快速构建
 * - 宏支持：QUICK_CJMOD一行搞定
 * - 批量处理：高效处理多个函数
 * 
 * 【虚函数支持】
 * - 多种函数类型：回调、事件、异步、生成器
 * - 智能模板系统：自动生成JS代码
 * - 灵活注册机制：支持自定义虚函数
 * 
 * 【高级扫描】
 * - 多模式扫描：双指针、正则、AST、上下文、混合
 * - 智能前置截取：上下文感知的截取
 * - 嵌套结构处理：处理复杂的语法结构
 * - 语义扫描：理解代码语义
 * 
 * 【智能功能】
 * - 智能函数检测：自动识别特殊函数
 * - 参数推导：自动推导参数类型和数量
 * - 代码优化：自动优化生成的JS代码
 * - 策略自适应：根据代码复杂度自动选择最佳扫描策略
 * 
 * 💡 设计哲学：
 * - 简单的事情更简单：QuickBuilder让常见任务一行搞定
 * - 复杂的事情成为可能：高级扫描和虚函数支持复杂场景
 * - 保持向后兼容：原有API完全不变
 * - 渐进式增强：可以选择使用新功能，也可以坚持原有方式
 */