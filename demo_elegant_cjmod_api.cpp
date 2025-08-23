/**
 * @file demo_elegant_cjmod_api.cpp
 * @brief 演示新设计的CJMOD API如何实现iNeverAway和printMylove功能
 * @author CHTL Team
 * @date 2024
 * 
 * 设计理念：表面简单，内在精妙
 * - 用户接口极其简单直观
 * - 内部实现复杂而强大
 * - 利用CJMOD的高自由度特性
 */

#include "CJMOD/ElegantCJMODApi.h"
#include <iostream>
#include <string>
#include <regex>

using namespace CHTL::CJMOD;
using namespace CHTL::CJMOD::Chtholly;

// ==========================================
// 演示1: iNeverAway功能实现
// ==========================================

void demonstrateINeverAwayImplementation() {
    std::cout << "\n=== 演示 iNeverAway 功能的完整实现流程 ===" << std::endl;
    
    // 第1步：初始化全局状态管理（利用CJMOD高自由度）
    std::cout << "\n【步骤1】初始化全局状态管理" << std::endl;
    INeverAwaySystem::initializeGlobalState();
    
    // 第2步：syntaxAnalys - 语法分析（表面简单）
    std::cout << "\n【步骤2】语法分析 - 表面简单的接口" << std::endl;
    std::string ignoreChars = ",:{};()";
    auto keyword = syntaxAnalys(R"(
        vir $ = iNeverAway({
            $: $
        });
    )", ignoreChars);
    
    std::cout << "✓ 语法模式解析完成" << std::endl;
    std::cout << "✓ 识别到参数占位符: " << keyword->args.size() << "个" << std::endl;
    
    // 第3步：定义任意自定义键（CJMOD的高自由度体现）
    std::cout << "\n【步骤3】定义任意自定义键 - 体现CJMOD高自由度" << std::endl;
    
    // 演示：开发者可以定义任何符合命名规范的键
    INeverAwaySystem::defineCustomKey("MyPromise", "Happy");
    INeverAwaySystem::defineCustomKey("MyPromise", "Sad");
    INeverAwaySystem::defineCustomKey("GameEvent", "Start");
    INeverAwaySystem::defineCustomKey("GameEvent", "End");
    INeverAwaySystem::defineCustomKey("UserAction");  // 无状态
    INeverAwaySystem::defineCustomKey("ChthollyMood", "Excited");
    
    std::cout << "✓ 自定义键定义完成，支持任意键名和可选状态" << std::endl;
    
    // 第4步：bind - 参数绑定（内在精妙）
    std::cout << "\n【步骤4】参数绑定 - 内在精妙的处理" << std::endl;
    
    // 绑定虚对象名称处理器
    keyword->args.bind<std::string>("vir", [](const std::string& virName) -> std::string {
        std::cout << "  → 处理虚对象名称: " << virName << std::endl;
        return virName;
    });
    
    // 绑定自定义键处理器
    keyword->args.bind<std::string>("customKey", [](const std::string& keyDef) -> std::string {
        std::cout << "  → 处理自定义键定义: " << keyDef << std::endl;
        
        // 解析键名和状态
        std::regex stateRegex(R"(^(.+?)<(.+?)>$)");
        std::smatch matches;
        
        if (std::regex_match(keyDef, matches, stateRegex)) {
            std::string keyName = matches[1].str();
            std::string state = matches[2].str();
            std::cout << "    - 键名: " << keyName << ", 状态: " << state << std::endl;
            
            // 生成全局函数名
            std::string globalName = INeverAwaySystem::getGlobalFunctionName(keyName, state);
            std::cout << "    - 生成全局函数名: " << globalName << std::endl;
            return globalName;
        } else {
            std::cout << "    - 无状态键: " << keyDef << std::endl;
            std::string globalName = INeverAwaySystem::getGlobalFunctionName(keyDef, "");
            std::cout << "    - 生成全局函数名: " << globalName << std::endl;
            return globalName;
        }
    });
    
    // 绑定函数体处理器
    keyword->args.bind<std::string>("functionBody", [](const std::string& funcBody) -> std::string {
        std::cout << "  → 处理函数体: " << funcBody.substr(0, 50) << "..." << std::endl;
        return funcBody;
    });
    
    std::cout << "✓ 参数绑定完成" << std::endl;
    
    // 第5步：scanKeyword - 扫描和匹配
    std::cout << "\n【步骤5】扫描和匹配 - 自动化流程" << std::endl;
    
    // 模拟CHTL代码输入
    std::string chtlCode = R"(
        vir ChthollyPromise = iNeverAway({
            MyPromise<Happy>: function(message, duration) {
                console.log("珂朵莉的快乐承诺: " + message);
                return new Promise((resolve) => {
                    setTimeout(() => resolve(message), duration * 1000);
                });
            },
            
            MyPromise<Sad>: function(message, duration) {
                console.log("珂朵莉的忧伤承诺: " + message);
                return new Promise((resolve) => {
                    setTimeout(() => resolve(message), duration * 1000);
                });
            },
            
            UserAction: function(action) {
                console.log("珂朵莉执行: " + action);
                return "完成: " + action;
            }
        });
    )";
    
    auto scanner = getCJMODScanner();
    scanner->scanKeyword(*keyword, chtlCode, [&](const std::vector<std::string>& matches) {
        std::cout << "✓ 匹配到iNeverAway调用" << std::endl;
        std::cout << "  - 虚对象名: " << matches[0] << std::endl;
        std::cout << "  - 对象内容长度: " << matches[1].length() << " 字符" << std::endl;
        
        // 处理虚对象内容
        std::string processedContent = INeverAwaySystem::processVirObject(matches[0], matches[1]);
        std::cout << "✓ 虚对象处理完成" << std::endl;
    });
    
    // 第6步：generateCode - 生成最终JavaScript代码
    std::cout << "\n【步骤6】生成最终JavaScript代码" << std::endl;
    
    std::string finalCode = generateCode(*keyword);
    std::cout << "✓ JavaScript代码生成完成" << std::endl;
    std::cout << "生成的代码片段:\n" << finalCode.substr(0, 200) << "...\n" << std::endl;
}

// ==========================================
// 演示2: printMylove功能实现
// ==========================================

void demonstratePrintMyloveImplementation() {
    std::cout << "\n=== 演示 printMylove 功能的完整实现流程 ===" << std::endl;
    
    // 第1步：syntaxAnalys - 语法分析
    std::cout << "\n【步骤1】语法分析 - 识别printMylove模式" << std::endl;
    
    std::string ignoreChars = ",()\"'";
    auto keyword = syntaxAnalys(R"(
        printMylove("$", "$", "$", "$", $)
    )", ignoreChars);
    
    std::cout << "✓ printMylove语法模式解析完成" << std::endl;
    std::cout << "✓ 参数数量: " << keyword->args.size() << std::endl;
    
    // 第2步：bind - 智能参数处理（内在精妙）
    std::cout << "\n【步骤2】智能参数处理 - 内在精妙的验证和转换" << std::endl;
    
    // URL处理器
    keyword->args.bind<std::string>("url", [](const std::string& url) -> std::string {
        std::cout << "  → 处理图片URL: " << url << std::endl;
        std::string processedUrl = PrintMyloveSystem::processImageUrl(url);
        std::cout << "    - 验证结果: " << (processedUrl.empty() ? "无效" : "有效") << std::endl;
        return processedUrl;
    });
    
    // 模式处理器
    keyword->args.bind<std::string>("mode", [](const std::string& mode) -> std::string {
        std::cout << "  → 处理转换模式: " << mode << std::endl;
        std::string validatedMode = PrintMyloveSystem::validateMode(mode);
        std::cout << "    - 标准化模式: " << validatedMode << std::endl;
        return validatedMode;
    });
    
    // 尺寸处理器
    keyword->args.bind<std::string>("width", [](const std::string& width) -> std::string {
        std::cout << "  → 处理宽度: " << width << std::endl;
        std::string processedWidth = PrintMyloveSystem::processDimension(width);
        std::cout << "    - 标准化宽度: " << processedWidth << std::endl;
        return processedWidth;
    });
    
    keyword->args.bind<std::string>("height", [](const std::string& height) -> std::string {
        std::cout << "  → 处理高度: " << height << std::endl;
        std::string processedHeight = PrintMyloveSystem::processDimension(height);
        std::cout << "    - 标准化高度: " << processedHeight << std::endl;
        return processedHeight;
    });
    
    // 缩放比例处理器
    keyword->args.bind<double>("scale", [](double scale) -> double {
        std::cout << "  → 处理缩放比例: " << scale << std::endl;
        if (scale <= 0 || scale > 10) {
            std::cout << "    - 无效比例，使用默认值: 1.0" << std::endl;
            return 1.0;
        }
        std::cout << "    - 有效比例: " << scale << std::endl;
        return scale;
    });
    
    std::cout << "✓ 智能参数处理器绑定完成" << std::endl;
    
    // 第3步：scanKeyword - 扫描CHTL代码
    std::cout << "\n【步骤3】扫描CHTL代码 - 自动匹配和处理" << std::endl;
    
    std::string chtlCode = R"(
        // 珂朵莉的图片处理
        let chthollyArt1 = printMylove("https://example.com/chtholly1.jpg", "ascii", "80", "40", 1.2);
        let chthollyArt2 = printMylove("./images/chtholly2.png", "pixel", "100", "50", 0.8);
        let chthollyArt3 = printMylove("data:image/jpeg;base64,/9j/4AAQ...", "block", "60", "30", 1.0);
    )";
    
    auto scanner = getCJMODScanner();
    int matchCount = 0;
    
    scanner->scanKeyword(*keyword, chtlCode, [&](const std::vector<std::string>& matches) {
        matchCount++;
        std::cout << "✓ 匹配到printMylove调用 #" << matchCount << std::endl;
        std::cout << "  - URL: " << matches[0] << std::endl;
        std::cout << "  - 模式: " << matches[1] << std::endl;
        std::cout << "  - 尺寸: " << matches[2] << "x" << matches[3] << std::endl;
        std::cout << "  - 缩放: " << matches[4] << std::endl;
        
        // 生成处理代码
        std::string processorCode = PrintMyloveSystem::generateImageProcessor(
            matches[0], matches[1], matches[2], matches[3], std::stod(matches[4])
        );
        std::cout << "  - 生成处理器代码长度: " << processorCode.length() << " 字符" << std::endl;
    });
    
    std::cout << "✓ 总共处理了 " << matchCount << " 个printMylove调用" << std::endl;
    
    // 第4步：generateCode - 生成最终代码
    std::cout << "\n【步骤4】生成最终JavaScript代码" << std::endl;
    
    std::string finalCode = generateCode(*keyword);
    std::cout << "✓ JavaScript代码生成完成" << std::endl;
    std::cout << "生成的代码片段:\n" << finalCode.substr(0, 300) << "...\n" << std::endl;
}

// ==========================================
// 演示3: 完整的CJMOD工作流程
// ==========================================

void demonstrateCompleteWorkflow() {
    std::cout << "\n=== 演示完整的CJMOD工作流程 ===" << std::endl;
    
    std::cout << "\n【CJMOD设计理念】" << std::endl;
    std::cout << "✓ 表面简单：用户只需要调用简单的函数" << std::endl;
    std::cout << "✓ 内在精妙：复杂的语法分析、参数处理、代码生成" << std::endl;
    std::cout << "✓ 高自由度：可以包含头文件、定义全局变量、管理状态" << std::endl;
    
    std::cout << "\n【工作流程总结】" << std::endl;
    std::cout << "1. syntaxAnalys()  - 解析语法模式" << std::endl;
    std::cout << "2. bind()          - 绑定参数处理器" << std::endl;
    std::cout << "3. scanKeyword()   - 扫描和匹配代码" << std::endl;
    std::cout << "4. match()         - 参数值匹配" << std::endl;
    std::cout << "5. generateCode()  - 生成最终JavaScript" << std::endl;
    
    std::cout << "\n【特色功能】" << std::endl;
    std::cout << "✓ iNeverAway: 支持任意自定义键的承诺函数系统" << std::endl;
    std::cout << "✓ printMylove: 智能图片转字符/像素块处理" << std::endl;
    std::cout << "✓ 全局状态管理: 利用CJMOD的高自由度特性" << std::endl;
    std::cout << "✓ 虚对象支持: 扩展CHTL JS原生vir功能" << std::endl;
    
    std::cout << "\n【珂朵莉的祝福】" << std::endl;
    std::cout << "珂朵莉永远在大家心中 ❀" << std::endl;
    std::cout << "新的CJMOD API让开发变得简单而强大！" << std::endl;
}

// ==========================================
// 主函数 - 完整演示
// ==========================================

int main() {
    std::cout << "========================================" << std::endl;
    std::cout << "    新设计CJMOD API功能演示" << std::endl;
    std::cout << "    设计理念：表面简单，内在精妙" << std::endl;
    std::cout << "========================================" << std::endl;
    
    try {
        // 演示iNeverAway功能实现
        demonstrateINeverAwayImplementation();
        
        // 演示printMylove功能实现  
        demonstratePrintMyloveImplementation();
        
        // 演示完整工作流程
        demonstrateCompleteWorkflow();
        
        std::cout << "\n✅ 所有演示完成！新CJMOD API设计成功！" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "❌ 演示过程中出现错误: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}