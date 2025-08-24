#include "CJMOD/ProperCJMODApi.h"
#include "Scanner/CHTLUnifiedScanner.h"
#include <iostream>
#include <memory>

using namespace CHTL;
using namespace CHTL::CJMOD;

/**
 * @brief 演示正确的CJMOD架构 - 与统一扫描器完美协作
 * 
 * 核心理念：
 * 1. 统一扫描器负责代码片段切割（不做词法分析）
 * 2. CJMOD API接收CodeFragment，使用peekKeyword提取参数
 * 3. 专注于参数处理和JS代码生成
 */

void demonstrateProperArchitecture() {
    std::cout << "\n=== 🏗️ 正确的CJMOD架构演示 ===\n";
    std::cout << "展示CHTL优秀架构设计的威力！\n\n";
    
    // ============ 第1步：创建统一扫描器 ============
    std::cout << "🔍 第1步：创建统一扫描器\n";
    Scanner::CHTLUnifiedScanner scanner;
    
    // ============ 第2步：注册CJMOD扩展关键字 ============
    std::cout << "📝 第2步：注册CJMOD扩展关键字\n";
    std::vector<std::string> cjmodKeywords = {"printMylove", "iNeverAway", "listen"};
    registerCJMODExtensions(scanner, cjmodKeywords);
    std::cout << "   ✅ 已注册关键字：printMylove, iNeverAway, listen\n";
    
    // ============ 第3步：准备CHTL源代码 ============
    std::cout << "\n📄 第3步：准备CHTL源代码\n";
    std::string chtlSource = R"(
<!DOCTYPE html>
<html>
<head>
    <title>CJMOD架构演示</title>
</head>
<body>
    <h1>正确的CJMOD架构</h1>
    <script>
    {
        // 这里是CHTL JS代码，会被统一扫描器识别
        const result = printMylove({
            url: "https://example.com/image.jpg",
            mode: "ASCII",
            width: "100px",
            height: "100px",
            scale: 0.5
        });
        
        console.log("普通JS代码");
        
        vir test = listen({
            click: function() {
                console.log("点击事件");
            }
        });
        
        test->click();
    }
    </script>
</body>
</html>
    )";
    
    std::cout << "   📋 源代码包含：printMylove调用和虚对象语法\n";
    
    // ============ 第4步：统一扫描器扫描源代码 ============
    std::cout << "\n🔍 第4步：统一扫描器扫描源代码\n";
    auto fragments = scanner.ScanSource(chtlSource, "demo.chtl");
    
    std::cout << "   🎯 扫描结果：发现 " << fragments.size() << " 个代码片段\n";
    
    for (size_t i = 0; i < fragments.size(); ++i) {
        const auto& fragment = fragments[i];
        std::cout << "   片段 " << (i+1) << ": ";
        
        switch (fragment.type) {
            case Scanner::FragmentType::CHTL:
                std::cout << "CHTL片段";
                break;
            case Scanner::FragmentType::CHTL_JS:
                std::cout << "CHTL JS片段 ⭐";
                break;
            case Scanner::FragmentType::CSS:
                std::cout << "CSS片段";
                break;
            case Scanner::FragmentType::JS:
                std::cout << "JS片段";
                break;
            default:
                std::cout << "未知片段";
                break;
        }
        
        std::cout << " (长度: " << fragment.content.length() << ")\n";
        
        // 如果是CHTL JS片段，展示内容预览
        if (fragment.type == Scanner::FragmentType::CHTL_JS) {
            std::string preview = fragment.content.substr(0, 50);
            if (fragment.content.length() > 50) preview += "...";
            std::cout << "      内容预览: " << preview << "\n";
        }
    }
}

void demonstrateStandardFlow() {
    std::cout << "\n=== 📋 标准流程演示 - 7步原始API ===\n";
    
    // 模拟统一扫描器返回的CodeFragment
    Scanner::CodeFragment fragment;
    fragment.type = Scanner::FragmentType::CHTL_JS;
    fragment.content = R"(printMylove({
        url: "https://example.com/love.jpg",
        mode: "ASCII", 
        width: "200px",
        height: "150px",
        scale: 0.8
    }))";
    fragment.startPos = 0;
    fragment.endPos = fragment.content.length();
    
    std::cout << "🎯 处理来自统一扫描器的CodeFragment:\n";
    std::cout << "   " << fragment.content.substr(0, 40) << "...\n\n";
    
    // 创建CJMOD处理器
    auto processor = createCJMODProcessor("printMylove");
    
    std::cout << "🔄 执行7步原始API流程:\n";
    
    // 1. syntaxAnalys
    std::cout << "1️⃣ syntaxAnalys - 语法分析\n";
    auto syntax = processor->syntaxAnalys("printMylove({url, mode, width, height, scale})");
    
    // 2. bind - 参数绑定
    std::cout << "2️⃣ bind - 参数绑定\n";
    processor->bind("url", [](const std::string& value) {
        return "'" + value + "'";  // 确保URL被引号包围
    });
    processor->bind("mode", [](const std::string& value) {
        return value == "ASCII" ? "'ASCII'" : "'Pixel'";
    });
    
    // 3. transform - 代码转换（准备模板）
    std::cout << "3️⃣ transform - 准备代码转换\n";
    
    // 4. scanKeyword - 设置扫描关键字
    std::cout << "4️⃣ scanKeyword - 设置扫描关键字\n";
    processor->scanKeyword("printMylove");
    
    // 5. match - 匹配参数值（从CodeFragment中提取）
    std::cout << "5️⃣ match - 从CodeFragment匹配参数值\n";
    processor->match(fragment);
    
    // 6. result - 获取处理结果
    std::cout << "6️⃣ result - 获取处理结果\n";
    
    // 7. generateCode - 生成最终JS代码
    std::cout << "7️⃣ generateCode - 生成最终JS代码\n";
    std::string jsCode = processor->generateCode();
    
    std::cout << "\n✅ 生成的标准JS代码:\n";
    std::cout << "```javascript\n" << jsCode << "```\n";
}

void demonstrateSimplifiedFlow() {
    std::cout << "\n=== ⚡ 简化流程演示 - 自动集成原始API ===\n";
    
    // 模拟统一扫描器返回的CodeFragment
    Scanner::CodeFragment fragment;
    fragment.type = Scanner::FragmentType::CHTL_JS;
    fragment.content = R"(printMylove({
        url: "https://example.com/chtholly.png",
        mode: "Pixel",
        width: "300px", 
        height: "200px",
        scale: 1.0
    }))";
    fragment.startPos = 0;
    fragment.endPos = fragment.content.length();
    
    std::cout << "🎯 处理来自统一扫描器的CodeFragment:\n";
    std::cout << "   " << fragment.content.substr(0, 40) << "...\n\n";
    
    // 创建简化的CHTL JS函数
    auto simplifiedProcessor = createCHTLJSFunction("printMylove");
    
    std::cout << "⚡ 简化流程 - 自动集成7步原始API:\n";
    
    // 准备参数处理器
    std::unordered_map<std::string, std::function<std::string(const std::string&)>> processors = {
        {"url", [](const std::string& value) { return "'" + value + "'"; }},
        {"mode", [](const std::string& value) { return "'" + value + "'"; }},
        {"width", [](const std::string& value) { return value; }},
        {"height", [](const std::string& value) { return value; }},
        {"scale", [](const std::string& value) { return value; }}
    };
    
    // 一键处理 - 自动执行完整的7步流程
    std::string jsCode = simplifiedProcessor->autoProcess(fragment, "printMylove({})", processors);
    
    std::cout << "\n✅ 简化流程生成的JS代码:\n";
    std::cout << "```javascript\n" << jsCode << "```\n";
    
    std::cout << "\n🎉 简化流程优势:\n";
    std::cout << "   ✅ 自动集成原始API的7步流程\n";
    std::cout << "   ✅ 一行代码完成复杂处理\n";
    std::cout << "   ✅ 保持与统一扫描器的完美协作\n";
}

void demonstrateVirtualObjectIntegration() {
    std::cout << "\n=== 🔮 虚对象集成演示 - 委托给CHTL JS ===\n";
    
    // 模拟虚对象语法的CodeFragment
    Scanner::CodeFragment virFragment;
    virFragment.type = Scanner::FragmentType::CHTL_JS;
    virFragment.content = R"(vir test = listen({
        click: function() { console.log("点击"); },
        hover: function() { console.log("悬停"); }
    }); test->click();)";
    virFragment.startPos = 0;
    virFragment.endPos = virFragment.content.length();
    
    std::cout << "🎯 处理虚对象语法:\n";
    std::cout << "   " << virFragment.content << "\n\n";
    
    // 使用虚对象委托
    std::cout << "🔮 虚对象委托处理:\n";
    
    // 注册虚对象（委托给CHTL JS）
    VirtualObjectDelegate::registerVirtualObject("test", "listen_function");
    std::cout << "1️⃣ 注册虚对象: test -> listen_function\n";
    
    // 委托虚对象调用
    std::string delegatedCall = VirtualObjectDelegate::delegateVirtualCall("test", "click");
    std::cout << "2️⃣ 委托调用: test->click() -> " << delegatedCall << "\n";
    
    // 检查虚对象状态
    bool isRegistered = VirtualObjectDelegate::isVirtualObjectRegistered("test");
    std::cout << "3️⃣ 虚对象状态: " << (isRegistered ? "已注册" : "未注册") << "\n";
    
    std::cout << "\n✅ 虚对象委托优势:\n";
    std::cout << "   ✅ 委托给CHTL JS，不重新实现\n";
    std::cout << "   ✅ 利用现有的虚对象系统\n";
    std::cout << "   ✅ 保持架构的一致性\n";
}

void demonstrateArchitectureAdvantages() {
    std::cout << "\n=== 🏆 CHTL架构优势总结 ===\n";
    
    std::cout << "🎯 统一扫描器的职责:\n";
    std::cout << "   ✅ 代码片段切割和分类\n";
    std::cout << "   ✅ 状态机驱动的扫描\n";
    std::cout << "   ✅ 双指针滑动窗口机制\n";
    std::cout << "   ❌ 不进行词法分析（这是关键！）\n\n";
    
    std::cout << "🔧 CJMOD API的职责:\n";
    std::cout << "   ✅ 接收统一扫描器的CodeFragment\n";
    std::cout << "   ✅ 使用peekKeyword技术提取参数\n";
    std::cout << "   ✅ 专注于参数处理和JS代码生成\n";
    std::cout << "   ❌ 不重新实现扫描和词法分析\n\n";
    
    std::cout << "🚀 架构协作流程:\n";
    std::cout << "   源代码 → 统一扫描器 → CodeFragment → CJMOD API → 标准JS\n\n";
    
    std::cout << "💡 设计哲学:\n";
    std::cout << "   🎯 职责分离：各组件专注自己的核心功能\n";
    std::cout << "   🔄 协作优先：组件间完美配合，避免重复实现\n";
    std::cout << "   ⚡ 效率至上：利用现有优秀架构，不重新发明轮子\n";
    std::cout << "   🏗️ 可扩展性：新的CJMOD扩展可以轻松集成\n";
}

int main() {
    std::cout << "🏗️ CHTL正确架构演示 - 与统一扫描器完美协作\n";
    std::cout << "========================================================\n";
    
    try {
        // 演示正确的架构设计
        demonstrateProperArchitecture();
        
        // 演示标准流程
        demonstrateStandardFlow();
        
        // 演示简化流程
        demonstrateSimplifiedFlow();
        
        // 演示虚对象集成
        demonstrateVirtualObjectIntegration();
        
        // 总结架构优势
        demonstrateArchitectureAdvantages();
        
        std::cout << "\n🎉 演示完成！CHTL的优秀架构设计展现无遗！\n";
        std::cout << "   这就是为什么要使用CHTL现有的优秀架构，\n";
        std::cout << "   而不是重新发明轮子的原因！\n";
        
    } catch (const std::exception& e) {
        std::cerr << "❌ 演示过程中发生错误: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}