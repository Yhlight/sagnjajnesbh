#include "../include/CJMOD/CoreCJMODApi.h"
#include <iostream>
#include <string>

using namespace CHTL::CJMOD;

/**
 * @brief 简单而强大的CJMOD API使用演示
 * 
 * 🎯 核心理念：API使用简单，功能强大
 * - 自动语法分析，自动分解arg
 * - 手动match将代码片段的value填入需要的位置
 * - 简单但功能强大
 */

// ============================================================================
// 演示1: 超简单的基础用法
// ============================================================================

void demonstrateSimpleUsage() {
    std::cout << "\n=== 🚀 超简单的基础用法 ===\n";
    
    // 🔥 第1步：自动语法分析，自动分解arg
    auto keyword = syntaxAnalys("arg ** arg2");
    
    std::cout << "✅ 自动分析完成！发现参数：\n";
    for (size_t i = 0; i < keyword->args.size(); ++i) {
        std::cout << "   args[" << i << "] = \"" << keyword->args[i].getName() << "\"\n";
    }
    
    // 🔥 第2步：简单的手动match，强大的功能
    std::cout << "\n🎯 手动match填入值：\n";
    
    // 模拟从代码片段得到的值
    std::string leftValue = "loadLeft";   // 从peekKeyword(-1)得到
    std::string rightValue = "loadRight"; // 从peekKeyword(1)得到
    
    keyword->args[0].match(leftValue);    // 左参数
    keyword->args[2].match(rightValue);   // 右参数
    
    std::cout << "   左参数填入: \"" << leftValue << "\"\n";
    std::cout << "   右参数填入: \"" << rightValue << "\"\n";
    
    // 🔥 第3步：自动生成结果
    std::string result = keyword->result();
    std::cout << "\n🎉 自动生成结果: " << result << "\n";
    
    std::cout << "💡 就这么简单！3步搞定复杂的CJMOD处理\n";
}

// ============================================================================
// 演示2: printMylove - 稍微复杂但依然简单
// ============================================================================

void demonstratePrintMylove() {
    std::cout << "\n=== 🚀 printMylove - 复杂功能，简单使用 ===\n";
    
    // 🔥 自动语法分析 - 一行搞定复杂的语法解析
    std::string ignoreChars = ",:{};()";
    auto keyword = syntaxAnalys(R"(
        printMylove({
            url: $,
            mode: $,
            width: $,
            height: $,
            scale: $
        })
    )", ignoreChars);
    
    std::cout << "✅ 自动分析复杂语法完成！发现 " << keyword->length() << " 个参数\n";
    
    // 🔥 简单的值绑定 - 可选的预处理
    std::cout << "\n🔧 可选：预先绑定值处理函数\n";
    keyword->bind<std::string>("url", [](const std::string& s) {
        return "\"" + s + "\"";  // 自动加引号
    });
    
    keyword->bind<std::string>("mode", [](const std::string& s) {
        return s == "ASCII" ? "\"ASCII\"" : "\"Pixel\"";  // 智能转换
    });
    
    // 🔥 简单的JS模板定义 - 可选的输出控制
    std::cout << "🎨 可选：预定义JS代码模板\n";
    keyword->transform("url", "loadImage('${value}')");
    keyword->transform("mode", "setRenderMode('${value}')");
    keyword->transform("width", "setWidth('${value}')");
    keyword->transform("height", "setHeight('${value}')");
    keyword->transform("scale", "setScale(${value})");
    
    // 🔥 核心：简单的手动match - 从代码片段填入实际值
    std::cout << "\n🎯 核心操作：手动match填入代码片段的值\n";
    
    // 这些值来自统一扫描器解析的代码片段，通过peekKeyword获得
    keyword->match("url", "photo.jpg");      // 实际的URL参数
    keyword->match("mode", "ASCII");         // 实际的模式参数  
    keyword->match("width", "800px");        // 实际的宽度参数
    keyword->match("height", "600px");       // 实际的高度参数
    keyword->match("scale", "2.5");          // 实际的缩放参数
    
    std::cout << "   ✓ url = \"photo.jpg\"\n";
    std::cout << "   ✓ mode = \"ASCII\"\n";
    std::cout << "   ✓ width = \"800px\"\n";
    std::cout << "   ✓ height = \"600px\"\n";
    std::cout << "   ✓ scale = \"2.5\"\n";
    
    // 🔥 自动生成最终代码
    std::string jsCode = keyword->result();
    std::cout << "\n🎉 自动生成JS代码:\n" << jsCode << "\n";
    
    // 🔥 自动输出标准JS
    std::string finalCode = generateCode(*keyword);
    std::cout << "\n✨ 最终标准JS代码:\n" << finalCode << "\n";
    
    std::cout << "💡 复杂的图片处理功能，几行代码搞定！\n";
}

// ============================================================================
// 演示3: 占位符的威力 - 超简单的通用处理
// ============================================================================

void demonstratePlaceholderPower() {
    std::cout << "\n=== 🚀 占位符的威力 - 通用处理 ===\n";
    
    // 🔥 使用占位符，更加通用
    auto keyword = syntaxAnalys("$ ** $ + $ - $");
    
    std::cout << "✅ 占位符语法自动解析完成！\n";
    
    // 🔥 批量绑定占位符处理函数 - 超简单
    std::cout << "🔧 批量绑定占位符处理：\n";
    
    keyword->bind<std::string>("$", [](const std::string& s) {
        std::cout << "   处理参数: " << s << "\n";
        return "process(" + s + ")";
    });
    
    keyword->bind<std::string>("$", [](const std::string& s) {
        std::cout << "   处理参数: " << s << "\n";
        return "process(" + s + ")";
    });
    
    keyword->bind<std::string>("$", [](const std::string& s) {
        std::cout << "   处理参数: " << s << "\n";
        return "process(" + s + ")";
    });
    
    keyword->bind<std::string>("$", [](const std::string& s) {
        std::cout << "   处理参数: " << s << "\n";
        return "process(" + s + ")";
    });
    
    // 🔥 简单match，自动按顺序填入
    std::cout << "\n🎯 按顺序填入值：\n";
    keyword->match("$", "value1");  // 第1个$
    keyword->match("$", "value2");  // 第2个$
    keyword->match("$", "value3");  // 第3个$
    keyword->match("$", "value4");  // 第4个$
    
    // 🔥 自动生成复杂表达式
    std::string result = keyword->result();
    std::cout << "\n🎉 自动生成复杂表达式: " << result << "\n";
    
    std::cout << "💡 占位符 + 自动匹配 = 无限可能！\n";
}

// ============================================================================
// 演示4: 实际工作流程 - 模拟统一扫描器的配合
// ============================================================================

void demonstrateRealWorkflow() {
    std::cout << "\n=== 🚀 真实工作流程 - 与统一扫描器配合 ===\n";
    
    // 🔥 第1步：定义要处理的语法
    auto keyword = syntaxAnalys("iNeverAway($, $)");
    std::cout << "✅ 步骤1：语法分析完成\n";
    
    // 🔥 第2步：绑定处理逻辑（可选）
    keyword->bind<std::string>("$", [](const std::string& s) {
        return "\"" + s + "\"";  // 字符串参数加引号
    });
    
    keyword->bind<std::string>("$", [](const std::string& s) {
        try {
            std::stod(s);  // 尝试转换为数字
            return s;      // 数字参数不加引号
        } catch (...) {
            return "\"" + s + "\"";  // 非数字加引号
        }
    });
    std::cout << "✅ 步骤2：参数处理逻辑绑定完成\n";
    
    // 🔥 第3步：定义JS输出模板（可选）
    keyword->transform("$", "setName(${value})");     // 第1个参数
    keyword->transform("$", "setDuration(${value})"); // 第2个参数
    std::cout << "✅ 步骤3：JS输出模板定义完成\n";
    
    // 🔥 第4步：模拟统一扫描器找到代码片段
    std::cout << "\n🔍 步骤4：模拟统一扫描器工作...\n";
    std::cout << "   源代码: iNeverAway(\"Chtholly\", 3.14)\n";
    std::cout << "   找到关键字: iNeverAway\n";
    std::cout << "   提取代码片段: \"Chtholly\", 3.14\n";
    
    // 🔥 第5步：超简单的match操作 - 填入实际值
    std::cout << "\n🎯 步骤5：简单match填入值\n";
    keyword->match("$", "Chtholly");  // 第1个参数值
    keyword->match("$", "3.14");      // 第2个参数值
    std::cout << "   ✓ 参数1: \"Chtholly\"\n";
    std::cout << "   ✓ 参数2: \"3.14\"\n";
    
    // 🔥 第6步：自动生成JS代码
    std::string jsCode = keyword->result();
    std::cout << "\n🎉 步骤6：自动生成JS代码\n";
    std::cout << "   JS代码: " << jsCode << "\n";
    
    // 🔥 第7步：生成器输出标准JS
    std::string finalCode = generateCode(*keyword);
    std::cout << "\n✨ 步骤7：标准JS输出\n";
    std::cout << "   最终代码: " << finalCode << "\n";
    
    std::cout << "\n💡 7个步骤，但开发者只需要关心第1、5步！\n";
    std::cout << "   自动语法分析 + 简单match = 强大功能\n";
}

// ============================================================================
// 演示5: 高级功能的简单使用
// ============================================================================

void demonstrateAdvancedSimple() {
    std::cout << "\n=== 🚀 高级功能的简单使用 ===\n";
    
    // 🔥 复杂的嵌套语法，依然简单处理
    auto keyword = syntaxAnalys(R"(
        createComponent({
            name: $,
            props: {
                width: $,
                height: $,
                onClick: $
            },
            methods: {
                render: $,
                update: $
            }
        })
    )", ",:{};()");
    
    std::cout << "✅ 复杂嵌套语法分析完成！参数数量: " << keyword->length() << "\n";
    
    // 🔥 智能类型处理
    std::cout << "\n🧠 智能参数类型处理：\n";
    
    keyword->bind<std::string>("$", [](const std::string& s) {
        std::cout << "   🏷️  组件名称: " << s << "\n";
        return "\"" + s + "\"";
    });
    
    keyword->bind<std::string>("$", [](const std::string& s) {
        std::cout << "   📏 宽度: " << s << "\n";
        return s.find("px") != std::string::npos ? ("\"" + s + "\"") : s;
    });
    
    keyword->bind<std::string>("$", [](const std::string& s) {
        std::cout << "   📐 高度: " << s << "\n";
        return s.find("px") != std::string::npos ? ("\"" + s + "\"") : s;
    });
    
    keyword->bind<std::string>("$", [](const std::string& s) {
        std::cout << "   🖱️  点击事件: " << s << "\n";
        return s;  // 函数不处理
    });
    
    keyword->bind<std::string>("$", [](const std::string& s) {
        std::cout << "   🎨 渲染函数: " << s << "\n";
        return s;  // 函数不处理
    });
    
    keyword->bind<std::string>("$", [](const std::string& s) {
        std::cout << "   🔄 更新函数: " << s << "\n";
        return s;  // 函数不处理
    });
    
    // 🔥 一次性match所有参数
    std::cout << "\n🎯 一次性填入所有参数值：\n";
    keyword->match("$", "MyButton");                    // 组件名
    keyword->match("$", "100px");                       // 宽度
    keyword->match("$", "50px");                        // 高度
    keyword->match("$", "() => { alert('clicked'); }"); // 点击事件
    keyword->match("$", "function() { return this.render(); }"); // 渲染函数
    keyword->match("$", "function() { this.forceUpdate(); }");   // 更新函数
    
    // 🔥 瞬间生成复杂的JS组件代码
    std::string result = keyword->result();
    std::cout << "\n🎉 瞬间生成复杂JS组件:\n" << result << "\n";
    
    std::cout << "💡 复杂的组件创建，几行match搞定！\n";
}

// ============================================================================
// 主函数
// ============================================================================

int main() {
    std::cout << "🚀 CJMOD API - 简单使用，强大功能\n";
    std::cout << "=================================\n";
    std::cout << "🎯 核心理念：API使用起来简单，自动的语法分析，\n";
    std::cout << "   通过手动match将代码片段的value填入需要的位置，\n";
    std::cout << "   简单但是又功能强大！\n";
    
    // 各种使用场景演示
    demonstrateSimpleUsage();        // 超简单基础用法
    demonstratePrintMylove();        // 复杂功能简单使用
    demonstratePlaceholderPower();   // 占位符威力
    demonstrateRealWorkflow();       // 真实工作流程
    demonstrateAdvancedSimple();     // 高级功能简单使用
    
    std::cout << "\n=================================\n";
    std::cout << "🎯 CJMOD API的魅力总结：\n";
    std::cout << "   ✅ 自动语法分析 - 智能分解arg\n";
    std::cout << "   ✅ 简单match操作 - 强大的值填充\n";
    std::cout << "   ✅ 智能类型处理 - 自动优化输出\n";
    std::cout << "   ✅ 占位符支持 - 无限扩展可能\n";
    std::cout << "   ✅ 模板系统 - 灵活的JS生成\n";
    std::cout << "   ✅ 一行代码 - 复杂功能实现\n";
    std::cout << "\n💡 底层复杂，使用简单 - 这就是好API的标准！\n";
    
    return 0;
}

/**
 * @brief 🎯 CJMOD API使用哲学：
 * 
 * 1. 【自动语法分析】
 *    - 一行 syntaxAnalys() 解决复杂语法解析
 *    - 自动识别参数、占位符、结构
 *    - 开发者无需关心内部实现
 * 
 * 2. 【简单match操作】  
 *    - keyword.match("param", value) 一行填值
 *    - 来自代码片段的peekKeyword结果
 *    - 简单但功能强大
 * 
 * 3. 【可选的增强功能】
 *    - bind() - 可选的值预处理
 *    - transform() - 可选的JS模板
 *    - 不用也能工作，用了更强大
 * 
 * 4. 【强大的底层支持】
 *    - 统一扫描器 - 自动处理复杂代码片段
 *    - 双指针扫描 - 高效准确的分析
 *    - 前置截取 - 智能避免编译器冲突
 *    - 代码生成器 - 标准JS输出
 * 
 * 💡 开发者体验：
 *    写代码：简单 -> 一行分析 + 几行match
 *    功能：强大 -> 复杂语法处理 + 智能JS生成
 *    学习：容易 -> 核心就是match操作
 *    扩展：灵活 -> 占位符 + 模板系统
 */