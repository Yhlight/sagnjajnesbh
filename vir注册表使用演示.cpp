#include "CJMOD/ElegantCJMODApi.h"
#include <iostream>

using namespace CHTL::CJMOD;

// 演示：createCHTLJSFunction自动注册vir支持
void demonstrateAutoVirRegistration() {
    std::cout << "\n🌸 演示：createCHTLJSFunction自动注册vir支持\n" << std::endl;
    
    // 使用createCHTLJSFunction创建函数 - 自动注册vir支持
    auto iNeverAwayFunc = createCHTLJSFunction("iNeverAway", 
        {"MyPromise", "GameEvent", "UserAction"});
    
    auto printMyloveFunc = createCHTLJSFunction("printMylove", 
        {"url", "mode", "width", "height", "scale"});
    
    // 验证注册结果
    std::cout << "\n=== vir支持验证 ===" << std::endl;
    
    std::vector<std::string> testFunctions = {"iNeverAway", "printMylove", "unknownFunction"};
    
    for (const auto& funcName : testFunctions) {
        if (CHTLJSVirRegistry::isSupported(funcName)) {
            auto keys = CHTLJSVirRegistry::getSupportedKeys(funcName);
            std::cout << "✅ " << funcName << " 支持vir，键: ";
            for (const auto& key : keys) {
                std::cout << key << " ";
            }
            std::cout << std::endl;
        } else {
            std::cout << "❌ " << funcName << " 不支持vir" << std::endl;
        }
    }
}

// 演示：标准CJMOD流程手动注册vir支持
void demonstrateManualVirRegistration() {
    std::cout << "\n🛠️ 演示：标准CJMOD流程手动注册vir支持\n" << std::endl;
    
    // 模拟标准CJMOD流程创建函数
    std::cout << "=== 标准CJMOD流程创建自定义函数 ===" << std::endl;
    
    // 1. 创建关键字对象
    auto keyword = std::make_unique<Keyword>();
    
    // 2. 语法分析
    keyword->syntaxAnalys("customImageProcessor({$});");
    std::cout << "✓ 语法分析完成: customImageProcessor({$});" << std::endl;
    
    // 3. 参数绑定
    keyword->args.bind<std::string>("imageUrl", [](const std::string& url) -> std::string {
        std::cout << "  → 处理图片URL: " << url << std::endl;
        return url;
    });
    
    keyword->args.bind<std::string>("filterType", [](const std::string& filter) -> std::string {
        std::cout << "  → 处理滤镜类型: " << filter << std::endl;
        return filter;
    });
    
    std::cout << "✓ 参数绑定完成" << std::endl;
    
    // 4. 手动注册vir支持
    std::cout << "\n--- 手动注册vir支持 ---" << std::endl;
    registerCJMODFunctionForVir("customImageProcessor", {"imageUrl", "filterType"});
    
    // 5. 验证注册结果
    std::cout << "\n=== 验证vir支持 ===" << std::endl;
    if (CHTLJSVirRegistry::isSupported("customImageProcessor")) {
        auto keys = CHTLJSVirRegistry::getSupportedKeys("customImageProcessor");
        std::cout << "✅ customImageProcessor 现在支持vir语法" << std::endl;
        std::cout << "✅ 支持的键: ";
        for (const auto& key : keys) {
            std::cout << key << " ";
        }
        std::cout << std::endl;
        std::cout << "✅ 可以使用: vir processor = customImageProcessor({...});" << std::endl;
    }
}

// 演示：统一扫描器的vir验证机制
void demonstrateUnifiedScannerValidation() {
    std::cout << "\n🔍 演示：统一扫描器的vir验证机制\n" << std::endl;
    
    // 模拟统一扫描器处理vir语句
    std::vector<std::pair<std::string, std::string>> virStatements = {
        {"myVir1", "iNeverAway"},           // 支持vir
        {"myVir2", "printMylove"},          // 支持vir  
        {"myVir3", "customImageProcessor"}, // 手动注册支持vir
        {"myVir4", "unknownFunction"}       // 不支持vir
    };
    
    std::cout << "=== 模拟统一扫描器处理vir语句 ===" << std::endl;
    
    for (const auto& statement : virStatements) {
        const std::string& varName = statement.first;
        const std::string& funcName = statement.second;
        
        std::cout << "\n处理: vir " << varName << " = " << funcName << "({...});" << std::endl;
        
        // 统一扫描器的验证逻辑
        if (CHTLJSVirRegistry::isSupported(funcName)) {
            auto keys = CHTLJSVirRegistry::getSupportedKeys(funcName);
            std::cout << "✅ vir验证通过" << std::endl;
            std::cout << "✅ 函数 " << funcName << " 支持vir语法" << std::endl;
            std::cout << "✅ 支持的键: ";
            for (const auto& key : keys) {
                std::cout << key << " ";
            }
            std::cout << std::endl;
            std::cout << "✅ 继续处理vir语法..." << std::endl;
        } else {
            std::cout << "❌ vir验证失败" << std::endl;
            std::cout << "❌ 函数 '" << funcName << "' 不支持vir语法" << std::endl;
            std::cout << "💡 建议: 使用createCHTLJSFunction创建或手动调用registerCJMODFunctionForVir" << std::endl;
        }
    }
}

// 演示：查看所有支持vir的函数
void demonstrateVirRegistryQuery() {
    std::cout << "\n📋 演示：查看所有支持vir的函数\n" << std::endl;
    
    auto supportedFunctions = CHTLJSVirRegistry::getAllSupportedFunctions();
    
    std::cout << "=== 当前vir注册表内容 ===" << std::endl;
    std::cout << "支持vir的函数总数: " << supportedFunctions.size() << std::endl;
    
    for (const auto& funcName : supportedFunctions) {
        auto keys = CHTLJSVirRegistry::getSupportedKeys(funcName);
        std::cout << "📋 " << funcName << " → 键: ";
        for (const auto& key : keys) {
            std::cout << key << " ";
        }
        std::cout << std::endl;
    }
    
    std::cout << "\n✅ 这些函数都可以在vir语句中使用！" << std::endl;
}

// 演示：vir注册表的实际应用场景
void demonstrateRealWorldScenarios() {
    std::cout << "\n🚀 演示：vir注册表的实际应用场景\n" << std::endl;
    
    std::cout << "=== 场景1：珂朵莉模块开发 ===" << std::endl;
    
    // 珂朵莉模块使用createCHTLJSFunction - 自动支持vir
    auto chthollyFunc = createCHTLJSFunction("chthollyMagic", 
        {"spell", "power", "emotion"});
    
    std::cout << "\n可以这样使用:" << std::endl;
    std::cout << "vir chtholly = chthollyMagic({" << std::endl;
    std::cout << "    spell<Light>: function() { console.log('光明魔法'); }," << std::endl;
    std::cout << "    power: function(level) { return level * 100; }," << std::endl;
    std::cout << "    emotion<Happy>: function() { console.log('珂朵莉很开心'); }" << std::endl;
    std::cout << "});" << std::endl;
    
    std::cout << "\n=== 场景2：第三方模块开发 ===" << std::endl;
    
    // 第三方模块使用标准流程 + 手动注册
    std::cout << "第三方开发者创建自定义函数..." << std::endl;
    registerCJMODFunctionForVir("thirdPartyFunction", {"config", "callback"});
    
    std::cout << "\n现在可以这样使用:" << std::endl;
    std::cout << "vir thirdParty = thirdPartyFunction({" << std::endl;
    std::cout << "    config: { theme: 'dark' }," << std::endl;
    std::cout << "    callback: function(data) { console.log(data); }" << std::endl;
    std::cout << "});" << std::endl;
    
    std::cout << "\n=== 场景3：统一扫描器保护 ===" << std::endl;
    
    std::cout << "如果开发者忘记注册vir支持:" << std::endl;
    std::cout << "vir broken = unregisteredFunction({...});" << std::endl;
    std::cout << "↓" << std::endl;
    std::cout << "❌ 编译错误: 函数 'unregisteredFunction' 不支持vir语法" << std::endl;
    std::cout << "💡 这样可以防止运行时错误，提高开发体验！" << std::endl;
}

int main() {
    std::cout << "🌸 vir注册表机制完整演示" << std::endl;
    std::cout << "解决统一扫描器识别问题，确保vir语法的正确性" << std::endl;
    
    // 演示各种使用场景
    demonstrateAutoVirRegistration();
    demonstrateManualVirRegistration();
    demonstrateUnifiedScannerValidation();
    demonstrateVirRegistryQuery();
    demonstrateRealWorldScenarios();
    
    std::cout << "\n💝 珂朵莉的总结" << std::endl;
    std::cout << "✅ createCHTLJSFunction创建的函数自动支持vir" << std::endl;
    std::cout << "✅ 标准CJMOD流程可以手动注册vir支持" << std::endl;
    std::cout << "✅ 统一扫描器可以正确验证vir语法" << std::endl;
    std::cout << "✅ 防止vir调用失败，提高开发体验" << std::endl;
    std::cout << "\n珂朵莉永远在大家心中 ❀" << std::endl;
    
    return 0;
}