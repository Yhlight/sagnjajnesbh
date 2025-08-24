#include "../include/CJMOD/CoreCJMODApi.h"
#include <iostream>

using namespace CHTL::CJMOD;

/**
 * @brief 🚀 CJMOD API - 极简使用演示
 * 
 * 核心：自动语法分析 + 简单match = 强大功能
 */

int main() {
    std::cout << "🎯 CJMOD API - 3行代码搞定复杂功能\n\n";
    
    // ============================================================================
    // 示例1: 最简单的使用 - 3行代码
    // ============================================================================
    
    std::cout << "📝 示例1: 处理 'arg ** arg2' 语法\n";
    
    // 🔥 第1行：自动语法分析
    auto keyword = syntaxAnalys("arg ** arg2");
    
    // 🔥 第2行：填入参数值（来自代码片段的peekKeyword）
    keyword->args[0].match("loadData");     // 左参数
    keyword->args[2].match("processData");  // 右参数
    
    // 🔥 第3行：获取结果
    std::string result = keyword->result();
    
    std::cout << "✅ 结果: " << result << "\n";
    std::cout << "💡 3行代码完成复杂的CJMOD处理！\n\n";
    
    // ============================================================================
    // 示例2: 占位符语法 - 同样3行
    // ============================================================================
    
    std::cout << "📝 示例2: 处理占位符语法\n";
    
    // 🔥 自动分析占位符
    auto keyword2 = syntaxAnalys("printMylove($, $, $)");
    
    // 🔥 按顺序填入值
    keyword2->match("$", "photo.jpg");
    keyword2->match("$", "ASCII");  
    keyword2->match("$", "2.5");
    
    // 🔥 获取JS代码
    std::string js = keyword2->result();
    
    std::cout << "✅ JS代码: " << js << "\n";
    std::cout << "💡 占位符让语法更通用！\n\n";
    
    // ============================================================================
    // 示例3: 复杂语法 - 依然简单
    // ============================================================================
    
    std::cout << "📝 示例3: 复杂对象语法\n";
    
    // 🔥 一行解析复杂语法
    auto keyword3 = syntaxAnalys("createUser({ name: $, age: $, email: $ })", ",:{};()");
    
    // 🔥 简单填值
    keyword3->match("$", "Chtholly");
    keyword3->match("$", "18");
    keyword3->match("$", "chtholly@example.com");
    
    // 🔥 生成标准JS
    std::string userJS = generateCode(*keyword3);
    
    std::cout << "✅ 用户创建JS: " << userJS << "\n";
    std::cout << "💡 复杂对象创建，3行搞定！\n\n";
    
    // ============================================================================
    // 核心价值总结
    // ============================================================================
    
    std::cout << "🎯 CJMOD API核心价值：\n";
    std::cout << "   🚀 简单：syntaxAnalys() + match() + result()\n";
    std::cout << "   💪 强大：处理任意复杂的CHTL JS语法\n";
    std::cout << "   🔄 自动：智能语法分析，无需手动解析\n";
    std::cout << "   🎯 精准：从代码片段精确提取参数值\n";
    std::cout << "   ✨ 优雅：底层复杂，使用简单\n\n";
    
    std::cout << "💡 这就是好API的标准：让复杂的事情变简单！\n";
    
    return 0;
}

/**
 * @brief 🎯 核心使用模式：
 * 
 * 【极简3步法】
 * 1. auto keyword = syntaxAnalys(pattern);     // 自动分析
 * 2. keyword->match(param, value);             // 填入值
 * 3. std::string result = keyword->result();   // 获取结果
 * 
 * 【value来源】
 * - 来自统一扫描器解析的代码片段
 * - 通过peekKeyword(-1), peekKeyword(1)获得
 * - CJMOD API自动处理复杂的扫描逻辑
 * 
 * 【强大之处】
 * - 自动识别任意复杂的语法结构
 * - 智能分解为可操作的arg
 * - 简单match完成复杂的值填充
 * - 自动生成标准的JS代码
 * 
 * 【开发者只需关心】
 * - 定义要处理的语法模式
 * - 将从代码片段得到的值填入对应位置
 * - 其他一切都是自动的！
 */