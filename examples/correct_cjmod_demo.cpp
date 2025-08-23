#include "../include/CJMOD/FinalCJMODApi.h"
#include <iostream>

using namespace CHTL::CJMOD;

/**
 * @brief 🎯 正确的CJMOD API演示
 * 
 * 严格根据CHTL语法文档：
 * 1. CJMOD允许包含头文件和全局变量作为注册表（第1425行）
 * 2. 简化流程只是createCHTLJSFunction自动集成原始API流程
 * 3. vir功能委托给CHTL JS，不重新实现（第1296-1310行）
 */

// ============================================================================
// 全局注册表实现 - 根据语法文档第1425行：允许编写全局变量管理状态
// ============================================================================

// 全局虚对象注册表
std::unordered_map<std::string, std::string> g_virtualObjectRegistry;

// 全局函数注册表 - 防止重复创建（语法文档第1310行）
std::unordered_map<std::string, std::string> g_globalFunctionRegistry;

// 事件委托注册表 - 管理所有事件委托的父元素（语法文档第1230行）
std::unordered_map<std::string, std::vector<std::string>> g_eventDelegateRegistry;

// ============================================================================
// 演示1: 原始API完整流程
// ============================================================================

void demonstrateOriginalAPIFlow() {
    std::cout << "\n=== 🔧 原始API完整流程 ===\n";
    std::cout << "流程：syntaxAnalys -> bind -> transform -> scanKeyword -> match -> result -> generateCode\n\n";
    
    // 创建CJMOD处理器
    auto processor = createCJMODProcessor();
    
    // 1. syntaxAnalys - 语法分析
    std::cout << "1️⃣ syntaxAnalys - 语法分析：\n";
    std::string ignoreChars = ",:{};()";
    auto syntax = processor->syntaxAnalys(R"(
        printMylove({
            url: $,
            mode: $,
            width: $,
            height: $,
            scale: $
        });
    )", ignoreChars);
    
    std::cout << "   解析出 " << syntax->args.size() << " 个参数\n";
    for (size_t i = 0; i < syntax->args.size(); ++i) {
        std::cout << "   args[" << i << "]: " << syntax->args[i].getName() << "\n";
    }
    
    // 2. bind - 绑定处理函数
    std::cout << "\n2️⃣ bind - 绑定处理函数：\n";
    syntax->bind("url", [](const std::string& str) -> std::string {
        return "\"" + str + "\"";  // 添加引号
    });
    
    syntax->bind("mode", [](const std::string& str) -> std::string {
        return "\"" + str + "\"";
    });
    
    std::cout << "   ✓ 已绑定url和mode参数的处理函数\n";
    
    // 3. transform - 转换为JS代码
    std::cout << "\n3️⃣ transform - 转换为JS代码：\n";
    syntax->transform("url", "const imageUrl = $;");
    syntax->transform("mode", "const renderMode = $;");
    
    std::cout << "   ✓ 已设置url和mode的JS模板\n";
    
    // 4. scanKeyword - 设置扫描关键字
    std::cout << "\n4️⃣ scanKeyword - 设置扫描关键字：\n";
    processor->scanKeyword("printMylove", [&]() {
        std::cout << "   🔍 扫描到printMylove关键字\n";
        
        // 5. match - 匹配参数值（在scanKeyword回调中）
        std::cout << "\n5️⃣ match - 匹配参数值：\n";
        syntax->match("url", processor->peekKeyword(1));  // 获取后续内容
        syntax->match("mode", "ASCII");
        syntax->match("width", "800");
        syntax->match("height", "600");
        syntax->match("scale", "1.0");
        
        std::cout << "   ✓ 已匹配所有参数值\n";
    });
    
    // 6. result - 组合结果
    std::cout << "\n6️⃣ result - 组合结果：\n";
    std::string result = syntax->result();
    std::cout << "   组合结果: " << result << "\n";
    
    // 7. generateCode - 生成最终JS代码
    std::cout << "\n7️⃣ generateCode - 生成最终JS代码：\n";
    std::string jsCode = processor->generateCode(*syntax);
    std::cout << "   最终JS代码:\n" << jsCode << "\n";
    
    std::cout << "✅ 原始API完整流程演示完成！\n";
}

// ============================================================================
// 演示2: 简化流程 - createCHTLJSFunction自动集成原始API流程
// ============================================================================

void demonstrateSimplifiedFlow() {
    std::cout << "\n=== ⚡ 简化流程 - createCHTLJSFunction自动集成原始API流程 ===\n";
    std::cout << "目标：让原始API更容易使用，不重新实现复杂功能\n\n";
    
    // 创建简化的CHTL JS函数处理器
    std::cout << "📝 创建简化的CHTL JS函数处理器：\n";
    auto simplifiedFunc = createCHTLJSFunction("printMylove");
    std::cout << "   ✓ 创建了 '" << simplifiedFunc->getFunctionName() << "' 处理器\n";
    
    // 自动化的原始API流程
    std::cout << "\n🚀 自动化的原始API流程：\n";
    std::cout << "   内部自动执行：syntaxAnalys -> bind -> transform -> scanKeyword -> match -> result -> generateCode\n";
    
    std::string pattern = "printMylove({ url: $, mode: $, width: $, height: $ });";
    std::vector<std::string> values = {"photo.jpg", "ASCII", "800", "600"};
    
    std::string result = simplifiedFunc->autoProcess(pattern, values);
    
    std::cout << "   输入模式: " << pattern << "\n";
    std::cout << "   输入参数: [";
    for (size_t i = 0; i < values.size(); ++i) {
        std::cout << "\"" << values[i] << "\"";
        if (i < values.size() - 1) std::cout << ", ";
    }
    std::cout << "]\n";
    std::cout << "   输出结果: " << result << "\n";
    
    // 对比原始API的复杂性
    std::cout << "\n💡 简化流程的价值：\n";
    std::cout << "   ✅ 原始API需要7个步骤，现在只需要1行代码\n";
    std::cout << "   ✅ 自动处理语法分析、绑定、转换、扫描等复杂步骤\n";
    std::cout << "   ✅ 保持原始API的所有功能，只是更容易使用\n";
    std::cout << "   ✅ 不重新实现，而是自动化现有流程\n";
    
    std::cout << "✅ 简化流程演示完成！\n";
}

// ============================================================================
// 演示3: 虚对象委托 - 委托给CHTL JS，不重新实现
// ============================================================================

void demonstrateVirtualObjectDelegate() {
    std::cout << "\n=== 🎭 虚对象委托 - 委托给CHTL JS，不重新实现 ===\n";
    std::cout << "根据语法文档第1296-1310行：vir是CHTL JS层面的语法糖，编译器处理\n\n";
    
    // 使用全局注册表管理虚对象状态
    std::cout << "📋 使用全局注册表管理虚对象状态：\n";
    
    // 注册虚对象到全局注册表
    VirtualObjectDelegate::registerVirtualObject("Test", "listen");
    VirtualObjectDelegate::registerVirtualObject("MyButton", "listen");
    VirtualObjectDelegate::registerVirtualObject("Promise", "iNeverAway");
    
    std::cout << "   ✓ 已注册虚对象到全局注册表\n";
    std::cout << "   全局虚对象注册表大小: " << g_virtualObjectRegistry.size() << "\n";
    
    // 检查虚对象注册状态
    std::cout << "\n🔍 检查虚对象注册状态：\n";
    std::cout << "   Test是否已注册: " << (VirtualObjectDelegate::isVirtualObjectRegistered("Test") ? "是" : "否") << "\n";
    std::cout << "   MyButton是否已注册: " << (VirtualObjectDelegate::isVirtualObjectRegistered("MyButton") ? "是" : "否") << "\n";
    std::cout << "   UnknownVir是否已注册: " << (VirtualObjectDelegate::isVirtualObjectRegistered("UnknownVir") ? "是" : "否") << "\n";
    
    // 委托虚对象调用给CHTL JS编译器
    std::cout << "\n🔗 委托虚对象调用给CHTL JS编译器：\n";
    std::cout << "   根据语法文档第1310行：遇到Test->click时，创建对应的全局函数并加入注册表\n";
    
    auto clickRef = VirtualObjectDelegate::delegateVirtualCall("Test", "click");
    auto hoverRef = VirtualObjectDelegate::delegateVirtualCall("MyButton", "hover");
    auto resolveRef = VirtualObjectDelegate::delegateVirtualCall("Promise", "resolve");
    
    std::cout << "   Test->click() 委托结果: " << clickRef << "\n";
    std::cout << "   MyButton->hover() 委托结果: " << hoverRef << "\n";
    std::cout << "   Promise->resolve() 委托结果: " << resolveRef << "\n";
    
    // 展示全局函数注册表的防重复机制
    std::cout << "\n🛡️ 全局函数注册表的防重复机制：\n";
    std::cout << "   全局函数注册表大小: " << g_globalFunctionRegistry.size() << "\n";
    
    // 重复调用，应该返回相同结果
    auto clickRef2 = VirtualObjectDelegate::delegateVirtualCall("Test", "click");
    std::cout << "   重复调用Test->click(): " << clickRef2 << "\n";
    std::cout << "   结果相同: " << (clickRef == clickRef2 ? "是" : "否") << " (防止重复创建)\n";
    
    // 虚对象委托的核心原理
    std::cout << "\n📚 虚对象委托的核心原理：\n";
    std::cout << "   ✅ vir是CHTL JS层面的语法糖，不涉及JS\n";
    std::cout << "   ✅ 编译器扫描到vir时，创建C++对象负责vir的解析\n";
    std::cout << "   ✅ 记录vir虚对象名称，委托给CHTL JS编译器处理\n";
    std::cout << "   ✅ 遇到Test->click时，委托创建全局函数并替换成函数引用\n";
    std::cout << "   ✅ 使用全局注册表防止重复创建相同的全局函数\n";
    
    std::cout << "✅ 虚对象委托演示完成！\n";
}

// ============================================================================
// 演示4: 全局注册表管理 - CJMOD允许全局变量管理状态
// ============================================================================

void demonstrateGlobalRegistryManagement() {
    std::cout << "\n=== 📊 全局注册表管理 - CJMOD允许全局变量管理状态 ===\n";
    std::cout << "根据语法文档第1425行：CJMOD可以自由包含头文件，编写全局变量管理状态\n\n";
    
    // 展示三个全局注册表
    std::cout << "📋 三个全局注册表的状态：\n";
    
    // 1. 虚对象注册表
    std::cout << "\n1️⃣ 虚对象注册表 (g_virtualObjectRegistry)：\n";
    std::cout << "   大小: " << g_virtualObjectRegistry.size() << "\n";
    for (const auto& pair : g_virtualObjectRegistry) {
        std::cout << "   " << pair.first << " -> " << pair.second << "\n";
    }
    
    // 2. 全局函数注册表
    std::cout << "\n2️⃣ 全局函数注册表 (g_globalFunctionRegistry)：\n";
    std::cout << "   大小: " << g_globalFunctionRegistry.size() << "\n";
    for (const auto& pair : g_globalFunctionRegistry) {
        std::cout << "   " << pair.first << " -> " << pair.second << "\n";
    }
    
    // 3. 事件委托注册表
    std::cout << "\n3️⃣ 事件委托注册表 (g_eventDelegateRegistry)：\n";
    std::cout << "   根据语法文档第1230行：管理所有事件委托的父元素\n";
    
    // 模拟添加事件委托
    g_eventDelegateRegistry["body"].push_back(".button");
    g_eventDelegateRegistry["body"].push_back(".link");
    g_eventDelegateRegistry[".container"].push_back(".item");
    
    std::cout << "   大小: " << g_eventDelegateRegistry.size() << "\n";
    for (const auto& pair : g_eventDelegateRegistry) {
        std::cout << "   " << pair.first << " -> [";
        for (size_t i = 0; i < pair.second.size(); ++i) {
            std::cout << pair.second[i];
            if (i < pair.second.size() - 1) std::cout << ", ";
        }
        std::cout << "]\n";
    }
    
    // 全局注册表的管理价值
    std::cout << "\n💡 全局注册表的管理价值：\n";
    std::cout << "   ✅ 虚对象注册表：记录vir虚对象名称和对应的CHTL JS函数\n";
    std::cout << "   ✅ 全局函数注册表：防止重复创建相同的全局函数\n";
    std::cout << "   ✅ 事件委托注册表：管理事件委托的父元素，避免重复绑定\n";
    std::cout << "   ✅ 状态持久化：全局变量确保状态在整个CJMOD生命周期中保持\n";
    std::cout << "   ✅ 性能优化：避免重复创建和重复绑定，提高执行效率\n";
    
    std::cout << "✅ 全局注册表管理演示完成！\n";
}

// ============================================================================
// 演示5: CJMOD接口扩展 - 通过接口扩展，不是任意C++代码
// ============================================================================

void demonstrateCJMODInterfaceExtension() {
    std::cout << "\n=== 🔌 CJMOD接口扩展 - 通过接口扩展，不是任意C++代码 ===\n";
    std::cout << "CJMOD目前只能通过接口，不能任意C++代码进行编写功能\n\n";
    
    // 展示CJMOD的接口限制
    std::cout << "🔒 CJMOD的接口限制：\n";
    std::cout << "   ✅ 允许：包含头文件 (#include)\n";
    std::cout << "   ✅ 允许：定义全局变量作为注册表\n";
    std::cout << "   ✅ 允许：使用CHTL JS编译器提供的接口\n";
    std::cout << "   ❌ 限制：不能任意编写C++功能代码\n";
    std::cout << "   ❌ 限制：必须通过规定的接口进行扩展\n";
    
    // 展示正确的CJMOD扩展方式
    std::cout << "\n✅ 正确的CJMOD扩展方式：\n";
    
    // 1. 通过三个核心类扩展
    std::cout << "\n1️⃣ 通过三个核心类扩展：\n";
    auto processor = createCJMODProcessor();
    std::cout << "   ✓ 使用Arg类处理参数\n";
    std::cout << "   ✓ 使用Syntax类管理语法\n";
    std::cout << "   ✓ 使用CHTLJSFunction类处理完整流程\n";
    
    // 2. 通过全局注册表管理状态
    std::cout << "\n2️⃣ 通过全局注册表管理状态：\n";
    std::cout << "   ✓ g_virtualObjectRegistry管理虚对象\n";
    std::cout << "   ✓ g_globalFunctionRegistry管理全局函数\n";
    std::cout << "   ✓ g_eventDelegateRegistry管理事件委托\n";
    
    // 3. 通过委托接口扩展功能
    std::cout << "\n3️⃣ 通过委托接口扩展功能：\n";
    std::cout << "   ✓ VirtualObjectDelegate委托虚对象处理\n";
    std::cout << "   ✓ createCHTLJSFunction简化API使用\n";
    std::cout << "   ✓ 所有扩展都通过规定的接口进行\n";
    
    // CJMOD设计哲学
    std::cout << "\n🎯 CJMOD设计哲学：\n";
    std::cout << "   💡 不用C++开发者眼光过度设计\n";
    std::cout << "   💡 严格遵循CHTL语法文档的定义\n";
    std::cout << "   💡 通过接口扩展，保持系统稳定性\n";
    std::cout << "   💡 委托给CHTL JS，不重新实现已有功能\n";
    std::cout << "   💡 简化流程 = 自动化原始API，不是重新发明\n";
    
    std::cout << "✅ CJMOD接口扩展演示完成！\n";
}

// ============================================================================
// 主函数
// ============================================================================

int main() {
    std::cout << "🎯 正确的CJMOD API演示\n";
    std::cout << "================================\n";
    std::cout << "📚 严格根据CHTL语法文档实现：\n";
    std::cout << "   - CJMOD允许包含头文件和全局变量作为注册表（第1425行）\n";
    std::cout << "   - 简化流程只是createCHTLJSFunction自动集成原始API流程\n";
    std::cout << "   - vir功能委托给CHTL JS，不重新实现（第1296-1310行）\n";
    std::cout << "   - 通过接口扩展，不是任意C++代码\n\n";
    
    // 演示所有正确的CJMOD功能
    demonstrateOriginalAPIFlow();           // 原始API完整流程
    demonstrateSimplifiedFlow();            // 简化流程
    demonstrateVirtualObjectDelegate();     // 虚对象委托
    demonstrateGlobalRegistryManagement();  // 全局注册表管理
    demonstrateCJMODInterfaceExtension();   // CJMOD接口扩展
    
    std::cout << "\n================================\n";
    std::cout << "🎯 正确的CJMOD API总结：\n";
    std::cout << "\n【遵循CHTL语法文档】\n";
    std::cout << "   ✅ CJMOD允许包含头文件和全局变量作为注册表\n";
    std::cout << "   ✅ 全局注册表管理虚对象、全局函数、事件委托\n";
    std::cout << "   ✅ vir是CHTL JS层面的语法糖，委托给编译器处理\n";
    std::cout << "\n【三个核心类保持不变】\n";
    std::cout << "   ✅ Arg：参数处理核心，支持operator()重载\n";
    std::cout << "   ✅ Syntax：语法容器，维护args向量\n";
    std::cout << "   ✅ CHTLJSFunction：完整的CJMOD处理器\n";
    std::cout << "\n【简化流程的正确理解】\n";
    std::cout << "   ✅ createCHTLJSFunction：自动集成原始API流程\n";
    std::cout << "   ✅ SimplifiedCHTLJSFunction：继承CHTLJSFunction，提供autoProcess\n";
    std::cout << "   ✅ 不重新实现复杂功能，只是让原始API更容易使用\n";
    std::cout << "\n【虚对象的正确处理】\n";
    std::cout << "   ✅ VirtualObjectDelegate：委托接口，不重新实现vir功能\n";
    std::cout << "   ✅ 使用全局注册表管理虚对象状态\n";
    std::cout << "   ✅ 委托给CHTL JS编译器处理vir语法糖\n";
    std::cout << "\n💡 设计哲学：\n";
    std::cout << "   - 严格遵循CHTL语法文档，不过度设计\n";
    std::cout << "   - 委托给CHTL JS，不重新实现已有功能\n";
    std::cout << "   - 简化流程 = 自动化原始API，不是重新发明\n";
    std::cout << "   - 通过接口扩展，保持系统的稳定性和一致性\n";
    std::cout << "   - vir功能委托给CHTL JS，不重新实现\n";
    std::cout << "\n🚀 这就是正确的CJMOD API设计！\n";
    
    return 0;
}

/**
 * @brief 🎯 正确的CJMOD API设计要点：
 * 
 * 【严格遵循CHTL语法文档】
 * - 第1425行：CJMOD可以自由包含头文件，编写全局变量管理状态
 * - 第1230行：需要创建一个全局注册表，管理所有事件委托的父元素
 * - 第1310行：创建对应的全局函数并加入注册表之中防止重复创建
 * - 第1296-1310行：vir是CHTL JS层面的语法糖，编译器处理
 * 
 * 【三个核心类】
 * - Arg：参数处理核心，支持operator()重载和bind函数
 * - Syntax：语法容器，维护args向量，提供bind/match/transform/result方法
 * - CHTLJSFunction：完整的CJMOD处理器，实现7步原始API流程
 * 
 * 【简化流程的正确理解】
 * - createCHTLJSFunction：创建SimplifiedCHTLJSFunction实例
 * - SimplifiedCHTLJSFunction：继承CHTLJSFunction，提供autoProcess方法
 * - autoProcess：内部自动执行原始API的7个步骤，用户只需提供模式和参数
 * - 不重新实现复杂功能，只是让原始API更容易使用
 * 
 * 【虚对象的正确处理】
 * - VirtualObjectDelegate：委托接口命名空间，不重新实现vir功能
 * - 使用全局注册表（g_virtualObjectRegistry, g_globalFunctionRegistry）管理状态
 * - 委托给CHTL JS编译器处理vir语法糖的解析和转换
 * - 防止重复创建相同的全局函数
 * 
 * 【CJMOD接口扩展】
 * - 允许包含头文件和定义全局变量
 * - 通过规定的接口进行扩展，不是任意C++代码
 * - 保持系统稳定性和一致性
 * 
 * 💡 设计哲学：
 * - 严格遵循CHTL语法文档，不过度设计
 * - 委托给CHTL JS，不重新实现已有功能
 * - 简化流程 = 自动化原始API，不是重新发明
 * - 通过接口扩展，保持系统的稳定性和一致性
 */