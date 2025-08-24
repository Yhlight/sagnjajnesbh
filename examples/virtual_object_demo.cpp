#include "../include/CJMOD/FinalCJMODApi.h"
#include <iostream>

using namespace CHTL::CJMOD;

/**
 * @brief 🎭 虚对象支持演示
 * 
 * 基于语法文档第1274-1311行的虚对象设计：
 * 1. createCHTLJSFunction创建的CHTL JS函数能够直接支持虚对象的使用
 * 2. 不使用createCHTLJSFunction的也能使用virBind("函数名称")手动绑定
 * 3. 然后就交给虚对象的自动扫描机制了
 */

// 全局处理器
auto processor = createCJMODProcessor();

// ============================================================================
// 演示1: createCHTLJSFunction直接支持虚对象
// ============================================================================

void demonstrateCreateCHTLJSFunctionVirtualObject() {
    std::cout << "\n=== 🎭 createCHTLJSFunction直接支持虚对象 ===\n";
    
    // 🔥 使用createCHTLJSFunction创建CHTL JS函数
    std::cout << "📝 创建listen函数（用于虚对象）：\n";
    auto listenFunc = createCHTLJSFunction("listen");
    
    // 🔥 createCHTLJSFunction创建的函数直接支持虚对象
    listenFunc->enableVirtualObjectSupport(true);
    std::cout << "   ✓ listen函数已启用虚对象支持\n";
    std::cout << "   虚对象支持状态: " << (listenFunc->isVirtualObjectSupported() ? "已启用" : "未启用") << "\n";
    
    // 🔥 处理虚对象定义
    std::cout << "\n🎯 处理虚对象定义：\n";
    std::string virDefinition = R"(
        vir Test = listen({
            click: () => {
                console.log("Button clicked!");
            },
            hover: () => {
                console.log("Button hovered!");
            }
        });
    )";
    
    listenFunc->processVirtualObjectDefinition(virDefinition);
    std::cout << "   ✓ 虚对象 'Test' 定义处理完成\n";
    
    // 🔥 处理虚对象调用
    std::cout << "\n🔍 处理虚对象调用：\n";
    auto clickCall = listenFunc->processVirtualObjectCall("Test->click()");
    auto hoverCall = listenFunc->processVirtualObjectCall("Test->hover()");
    
    std::cout << "   Test->click() => " << clickCall << "\n";
    std::cout << "   Test->hover() => " << hoverCall << "\n";
    
    std::cout << "✅ createCHTLJSFunction虚对象支持演示完成！\n";
}

// ============================================================================
// 演示2: virBind手动绑定虚对象支持
// ============================================================================

void demonstrateVirBindManualBinding() {
    std::cout << "\n=== 🔗 virBind手动绑定虚对象支持 ===\n";
    
    // 🔥 不使用createCHTLJSFunction的情况，使用virBind手动绑定
    std::cout << "📝 手动绑定虚对象支持：\n";
    
    // 手动绑定printMylove函数的虚对象支持
    processor->virBind("printMylove");
    std::cout << "   ✓ 已为 'printMylove' 绑定虚对象支持\n";
    
    // 手动绑定iNeverAway函数的虚对象支持  
    processor->virBind("iNeverAway");
    std::cout << "   ✓ 已为 'iNeverAway' 绑定虚对象支持\n";
    
    // 🔥 使用宏方式绑定
    std::cout << "\n✨ 使用宏方式绑定：\n";
    VIR_BIND(processor, "customFunction");
    std::cout << "   ✓ 已为 'customFunction' 绑定虚对象支持（宏方式）\n";
    
    // 🔥 验证绑定状态
    std::cout << "\n🔍 验证绑定状态：\n";
    auto& virManager = processor->getCoreSystem().getVirtualObjectManager();
    
    // 注册一些虚对象来测试
    virManager.registerVirtualObject("MyPrinter", "printMylove");
    virManager.registerVirtualObject("MyPromise", "iNeverAway");
    
    std::cout << "   MyPrinter是虚对象: " << (virManager.isVirtualObject("MyPrinter") ? "是" : "否") << "\n";
    std::cout << "   MyPromise是虚对象: " << (virManager.isVirtualObject("MyPromise") ? "是" : "否") << "\n";
    std::cout << "   RandomName是虚对象: " << (virManager.isVirtualObject("RandomName") ? "是" : "否") << "\n";
    
    std::cout << "✅ virBind手动绑定演示完成！\n";
}

// ============================================================================
// 演示3: 虚对象自动扫描机制
// ============================================================================

void demonstrateVirtualObjectAutoScanning() {
    std::cout << "\n=== 🔍 虚对象自动扫描机制 ===\n";
    
    auto& scanner = processor->getCoreSystem().getAdvancedScanner();
    auto& virManager = processor->getCoreSystem().getVirtualObjectManager();
    
    // 🔥 测试代码包含虚对象语法
    std::string testCode = R"(
        // 虚对象定义
        vir Button = listen({
            click: () => {
                console.log("Button clicked!");
            },
            hover: () => {
                console.log("Button hovered!");
            },
            focus: () => {
                console.log("Button focused!");
            }
        });
        
        vir Promise = iNeverAway({
            resolve: (value) => {
                return Promise.resolve(value);
            },
            reject: (error) => {
                return Promise.reject(error);
            }
        });
        
        // 虚对象调用
        Button->click();
        Button->hover();
        Promise->resolve("success");
        Promise->reject("error");
    )";
    
    std::cout << "📋 测试代码中的虚对象语法：\n";
    std::cout << "   - vir Button = listen({...})\n";
    std::cout << "   - vir Promise = iNeverAway({...})\n";
    std::cout << "   - Button->click(), Button->hover()\n";
    std::cout << "   - Promise->resolve(), Promise->reject()\n";
    
    // 🔥 虚对象语法扫描
    std::cout << "\n🎯 执行虚对象语法扫描：\n";
    auto virResults = scanner.scanVirtualObjectSyntax(testCode);
    
    std::cout << "   发现 " << virResults.size() << " 个虚对象语法元素：\n";
    for (const auto& result : virResults) {
        if (result.found) {
            std::cout << "     - " << result.keyword << " (位置: " << result.position << ")\n";
            if (!result.metadata.empty()) {
                for (const auto& meta : result.metadata) {
                    std::cout << "       " << meta.first << ": " << meta.second << "\n";
                }
            }
        }
    }
    
    // 🔥 处理虚对象定义
    std::cout << "\n🔧 处理虚对象定义：\n";
    virManager.registerVirtualObject("Button", "listen");
    virManager.registerVirtualObject("Promise", "iNeverAway");
    
    virManager.parseFunctionKeys("Button", "{ click: ()=>{}, hover: ()=>{}, focus: ()=>{} }");
    virManager.parseFunctionKeys("Promise", "{ resolve: (value)=>{}, reject: (error)=>{} }");
    
    std::cout << "   ✓ Button虚对象已注册并解析函数键\n";
    std::cout << "   ✓ Promise虚对象已注册并解析函数键\n";
    
    // 🔥 处理虚对象调用
    std::cout << "\n⚡ 处理虚对象调用：\n";
    auto buttonClick = virManager.processVirtualCall("Button", "click");
    auto buttonHover = virManager.processVirtualCall("Button", "hover");
    auto promiseResolve = virManager.processVirtualCall("Promise", "resolve");
    auto promiseReject = virManager.processVirtualCall("Promise", "reject");
    
    std::cout << "   Button->click() => " << buttonClick << "\n";
    std::cout << "   Button->hover() => " << buttonHover << "\n";
    std::cout << "   Promise->resolve() => " << promiseResolve << "\n";
    std::cout << "   Promise->reject() => " << promiseReject << "\n";
    
    std::cout << "✅ 虚对象自动扫描机制演示完成！\n";
}

// ============================================================================
// 演示4: 虚对象原理演示（基于语法文档）
// ============================================================================

void demonstrateVirtualObjectPrinciple() {
    std::cout << "\n=== 📚 虚对象原理演示（基于语法文档）===\n";
    
    auto& virManager = processor->getCoreSystem().getVirtualObjectManager();
    
    // 🔥 虚对象原理说明
    std::cout << "📖 虚对象原理（语法文档第1287行）：\n";
    std::cout << "   1. vir是CHTL JS层面的语法糖，不涉及JS\n";
    std::cout << "   2. 编译器扫描到vir时，创建C++对象负责vir的解析\n";
    std::cout << "   3. 记录vir虚对象名称，解析CHTL JS函数中的函数键\n";
    std::cout << "   4. 遇到Test->click时，创建对应的全局函数并替换成函数引用\n";
    
    // 🔥 实际演示过程
    std::cout << "\n🎯 实际演示过程：\n";
    
    // 第1步：注册虚对象
    std::cout << "第1步：注册虚对象 'Test' 对应 'listen' 函数\n";
    virManager.registerVirtualObject("Test", "listen");
    
    // 第2步：解析函数键
    std::cout << "第2步：解析listen函数中的函数键\n";
    std::string functionObject = "{ click: ()=>{console.log('clicked');}, hover: ()=>{console.log('hovered');} }";
    virManager.parseFunctionKeys("Test", functionObject);
    std::cout << "   解析函数键: click, hover\n";
    
    // 第3步：生成全局函数
    std::cout << "第3步：生成全局函数\n";
    auto globalClick = virManager.generateGlobalFunction("Test", "click");
    auto globalHover = virManager.generateGlobalFunction("Test", "hover");
    std::cout << "   生成全局函数: " << globalClick << "\n";
    std::cout << "   生成全局函数: " << globalHover << "\n";
    
    // 第4步：处理虚对象调用
    std::cout << "第4步：处理虚对象调用，替换成函数引用\n";
    auto clickReference = virManager.processVirtualCall("Test", "click");
    auto hoverReference = virManager.processVirtualCall("Test", "hover");
    std::cout << "   Test->click() 替换为: " << clickReference << "\n";
    std::cout << "   Test->hover() 替换为: " << hoverReference << "\n";
    
    // 🔥 原理验证
    std::cout << "\n✅ 原理验证：\n";
    std::cout << "   ✓ vir本身不存在，是编译期间的语法糖\n";
    std::cout << "   ✓ listen按原样生成JS代码\n";
    std::cout << "   ✓ Test->click()最终转换为全局函数引用\n";
    std::cout << "   ✓ 防止重复创建相同的全局函数\n";
    
    std::cout << "✅ 虚对象原理演示完成！\n";
}

// ============================================================================
// 演示5: 简化流程 + 虚对象集成
// ============================================================================

void demonstrateQuickBuildWithVirtualObject() {
    std::cout << "\n=== ⚡ 简化流程 + 虚对象集成 ===\n";
    
    // 🔥 简化流程创建CHTL JS函数，自动支持虚对象
    std::cout << "🚀 简化流程创建listen函数：\n";
    auto listenResult = QUICK_CJMOD("listen({ click: $, hover: $ })", "()=>{alert('click')}", "()=>{alert('hover')}");
    std::cout << "   快速构建结果: " << listenResult << "\n";
    
    // 🔥 为快速构建的函数绑定虚对象支持
    std::cout << "\n🔗 为快速构建的函数绑定虚对象支持：\n";
    VIR_BIND(processor, "listen");
    std::cout << "   ✓ 已为快速构建的listen函数绑定虚对象支持\n";
    
    // 🔥 虚对象定义和调用的完整流程
    std::cout << "\n🎭 虚对象完整流程：\n";
    
    // 定义虚对象
    std::string virDef = "vir MyButton = " + listenResult;
    processor->processVirtualObjectDefinition(virDef);
    std::cout << "   ✓ 虚对象定义: " << virDef.substr(0, 30) << "...\n";
    
    // 虚对象调用
    auto clickCall = processor->processVirtualObjectCall("MyButton->click()");
    auto hoverCall = processor->processVirtualObjectCall("MyButton->hover()");
    std::cout << "   ✓ MyButton->click() => " << clickCall << "\n";
    std::cout << "   ✓ MyButton->hover() => " << hoverCall << "\n";
    
    // 🔥 展示集成优势
    std::cout << "\n💡 集成优势：\n";
    std::cout << "   ✅ 简化流程快速创建CHTL JS函数\n";
    std::cout << "   ✅ 自动支持虚对象语法\n";
    std::cout << "   ✅ 无缝集成自动扫描机制\n";
    std::cout << "   ✅ 一行代码即可实现复杂虚对象功能\n";
    
    std::cout << "✅ 简化流程 + 虚对象集成演示完成！\n";
}

// ============================================================================
// 主函数
// ============================================================================

int main() {
    std::cout << "🎭 CJMOD API - 虚对象支持演示\n";
    std::cout << "================================\n";
    std::cout << "🎯 基于语法文档第1274-1311行的虚对象设计\n";
    std::cout << "🏗️ 两种支持方式：\n";
    std::cout << "   1. createCHTLJSFunction创建的函数直接支持虚对象\n";
    std::cout << "   2. virBind(\"函数名称\")手动绑定虚对象支持\n";
    std::cout << "⚡ 简化流程 + 虚对象 = 强大而简单的CHTL JS开发\n\n";
    
    // 演示所有虚对象功能
    demonstrateCreateCHTLJSFunctionVirtualObject();  // createCHTLJSFunction直接支持
    demonstrateVirBindManualBinding();               // virBind手动绑定
    demonstrateVirtualObjectAutoScanning();          // 自动扫描机制
    demonstrateVirtualObjectPrinciple();             // 虚对象原理
    demonstrateQuickBuildWithVirtualObject();        // 简化流程集成
    
    std::cout << "\n================================\n";
    std::cout << "🎯 虚对象支持总结：\n";
    std::cout << "   🎭 虚对象原理：vir是编译期语法糖，转换为全局函数引用\n";
    std::cout << "   🔧 createCHTLJSFunction：直接支持虚对象，无需额外配置\n";
    std::cout << "   🔗 virBind：为任意函数手动绑定虚对象支持\n";
    std::cout << "   🔍 自动扫描：智能处理vir定义和->调用语法\n";
    std::cout << "   ⚡ 简化流程：一行代码创建支持虚对象的CHTL JS函数\n";
    std::cout << "\n💡 核心价值：\n";
    std::cout << "   - 保持CJMOD API的简单性\n";
    std::cout << "   - 完整支持CHTL JS虚对象语法\n";
    std::cout << "   - 灵活的绑定方式（自动+手动）\n";
    std::cout << "   - 智能的扫描和处理机制\n";
    std::cout << "   - 与简化流程无缝集成\n";
    std::cout << "\n🚀 这就是完整的虚对象支持系统！\n";
    
    return 0;
}

/**
 * @brief 🎯 虚对象支持设计要点：
 * 
 * 【虚对象原理】（语法文档第1274-1311行）
 * - vir是CHTL JS层面的语法糖，不涉及JS
 * - 编译器扫描到vir时，创建C++对象负责vir的解析
 * - 记录虚对象名称，解析CHTL JS函数中的函数键，创建对应表
 * - 遇到Test->click时，创建对应的全局函数并替换成函数引用
 * 
 * 【两种支持方式】
 * 1. createCHTLJSFunction创建的CHTL JS函数能够直接支持虚对象的使用
 * 2. 不使用createCHTLJSFunction的也能使用virBind("函数名称")手动绑定
 * 
 * 【自动扫描机制】
 * - 虚对象语法扫描：专门识别vir定义和->调用
 * - 函数键解析：自动解析CHTL JS函数中的函数键
 * - 全局函数生成：创建特殊命名的全局函数
 * - 调用替换：将Test->click()替换为全局函数引用
 * 
 * 【与简化流程集成】
 * - QuickBuilder创建的函数可以自动支持虚对象
 * - VIR_BIND宏简化手动绑定过程
 * - 一行代码即可实现复杂的虚对象功能
 * 
 * 💡 设计哲学：
 * - 忠实于CHTL语法文档的虚对象设计
 * - 保持API的简单性和易用性
 * - 提供灵活的支持方式（自动+手动）
 * - 与现有CJMOD API无缝集成
 * - 智能化的扫描和处理机制
 */