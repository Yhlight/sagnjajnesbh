#include <iostream>
#include <fstream>
#include <filesystem>

int main() {
    std::cout << "🔍 CHTL高级功能支持检查\n";
    std::cout << "=======================\n\n";
    
    // 检查配置系统支持
    std::cout << "⚙️ 1. 配置系统支持检查\n";
    std::cout << "======================\n";
    
    std::vector<std::string> configFiles = {
        "src/CHTLCompiler/Configuration/ConfigurationManager.h",
        "src/CHTLCompiler/Configuration/ConfigurationManager.cpp"
    };
    
    int configSupport = 0;
    for (const auto& file : configFiles) {
        if (std::filesystem::exists(file)) {
            std::cout << "✅ " << file << "\n";
            configSupport++;
        } else {
            std::cout << "❌ " << file << " (缺失)\n";
        }
    }
    
    std::cout << "📊 配置系统支持: " << configSupport << "/" << configFiles.size() << "\n\n";
    
    // 检查OriginType支持
    std::cout << "🔗 2. OriginType支持检查\n";
    std::cout << "=======================\n";
    
    std::ifstream originManagerFile("src/CHTLCompiler/Origin/OriginManager.h");
    if (originManagerFile.is_open()) {
        std::string content((std::istreambuf_iterator<char>(originManagerFile)),
                           std::istreambuf_iterator<char>());
        originManagerFile.close();
        
        bool hasCustomType = content.find("CUSTOM") != std::string::npos;
        bool hasRegisterMethod = content.find("registerCustomOriginType") != std::string::npos;
        bool hasTypeValidation = content.find("validateOriginType") != std::string::npos;
        
        std::cout << (hasCustomType ? "✅" : "❌") << " 自定义原始类型支持\n";
        std::cout << (hasRegisterMethod ? "✅" : "❌") << " 类型注册方法\n";
        std::cout << (hasTypeValidation ? "✅" : "❌") << " 类型验证功能\n";
        
        std::cout << "📊 OriginType支持: " << (hasCustomType + hasRegisterMethod + hasTypeValidation) << "/3\n\n";
    } else {
        std::cout << "❌ OriginManager.h 不存在\n\n";
    }
    
    // 检查Import系统支持
    std::cout << "📥 3. Import系统支持检查\n";
    std::cout << "=======================\n";
    
    std::ifstream importManagerFile("src/CHTLCompiler/Import/ImportManager.h");
    if (importManagerFile.is_open()) {
        std::string content((std::istreambuf_iterator<char>(importManagerFile)),
                           std::istreambuf_iterator<char>());
        importManagerFile.close();
        
        // 检查各种导入类型支持
        std::vector<std::pair<std::string, std::string>> importFeatures = {
            {"HTML导入", "parseHtmlImport"},
            {"样式导入", "parseStyleImport"},
            {"脚本导入", "parseJavaScriptImport"},
            {"CHTL导入", "parseChtlImport"},
            {"CJMOD导入", "parseCJmodImport"},
            {"配置导入", "parseConfigImport"},
            {"原始嵌入导入", "parseOriginImport"},
            {"通配符导入", "parseWildcardImport"},
            {"全缀名导入", "parseFullNameImport"}
        };
        
        int importSupport = 0;
        for (const auto& [feature, method] : importFeatures) {
            bool supported = content.find(method) != std::string::npos;
            std::cout << (supported ? "✅" : "❌") << " " << feature << "\n";
            if (supported) importSupport++;
        }
        
        std::cout << "📊 Import功能支持: " << importSupport << "/" << importFeatures.size() << "\n\n";
    } else {
        std::cout << "❌ ImportManager.h 不存在\n\n";
    }
    
    // 检查模块系统支持
    std::cout << "📦 4. 模块系统支持检查\n";
    std::cout << "======================\n";
    
    std::vector<std::pair<std::string, std::string>> moduleFeatures = {
        {"CMOD管理器", "src/CHTLCompiler/Module/CmodManager.h"},
        {"CJMOD管理器", "src/CHTLJSCompiler/Module/CJmodManager.h"},
        {"模块路径管理", "src/Common/ModulePathManager.h"},
        {"模块打包器", "src/CHTLCompiler/Module/ModulePackager.h"}
    };
    
    int moduleSupport = 0;
    for (const auto& [feature, file] : moduleFeatures) {
        bool exists = std::filesystem::exists(file);
        std::cout << (exists ? "✅" : "❌") << " " << feature << "\n";
        if (exists) moduleSupport++;
    }
    
    std::cout << "📊 模块系统支持: " << moduleSupport << "/" << moduleFeatures.size() << "\n\n";
    
    // 检查CHTL JS支持
    std::cout << "🎯 5. CHTL JS支持检查\n";
    std::cout << "====================\n";
    
    std::vector<std::pair<std::string, std::string>> chtljsFeatures = {
        {"增强选择器", "src/CHTLJSCompiler/Selector/EnhancedSelectorProcessor.h"},
        {"箭头操作符", "src/CHTLJSCompiler/Operator/ArrowOperatorProcessor.h"},
        {"Listen系统", "src/CHTLJSCompiler/Event/ListenSystemProcessor.h"},
        {"Delegate系统", "src/CHTLJSCompiler/Event/DelegateSystemProcessor.h"},
        {"动画系统", "src/CHTLJSCompiler/Animation/AnimateSystemProcessor.h"},
        {"虚对象", "src/CHTLJSCompiler/Virtual/VirtualObjectProcessor.h"},
        {"局部脚本", "src/CHTLJSCompiler/Script/LocalScriptProcessor.h"},
        {"事件注册表", "src/CHTLJSCompiler/Core/JSEventRegistry.h"}
    };
    
    int chtljsSupport = 0;
    for (const auto& [feature, file] : chtljsFeatures) {
        bool exists = std::filesystem::exists(file);
        std::cout << (exists ? "✅" : "❌") << " " << feature << "\n";
        if (exists) chtljsSupport++;
    }
    
    std::cout << "📊 CHTL JS支持: " << chtljsSupport << "/" << chtljsFeatures.size() << "\n\n";
    
    // 生成总体报告
    std::cout << "📊 高级功能支持总结\n";
    std::cout << "==================\n";
    
    int totalSupport = configSupport + moduleSupport + chtljsSupport;
    int totalFeatures = configFiles.size() + moduleFeatures.size() + chtljsFeatures.size();
    
    double supportPercentage = (double)totalSupport / totalFeatures * 100.0;
    
    std::cout << "🎯 总体支持度: " << totalSupport << "/" << totalFeatures << " (";
    std::cout.precision(1);
    std::cout << std::fixed << supportPercentage << "%)\n\n";
    
    if (supportPercentage >= 90.0) {
        std::cout << "🏆 优秀！高级功能支持度很高\n";
    } else if (supportPercentage >= 70.0) {
        std::cout << "🥈 良好！大部分高级功能已支持\n";
    } else {
        std::cout << "⚠️ 需要继续完善高级功能支持\n";
    }
    
    std::cout << "\n🚀 需要重点关注的功能:\n";
    std::cout << "======================\n";
    std::cout << "1. 命名配置组 ([Configuration] @Config)\n";
    std::cout << "2. OriginType自定义嵌入类型\n";
    std::cout << "3. 更多Import类型支持\n";
    std::cout << "4. delete语法的正确实现\n";
    std::cout << "5. IDE的完整集成\n";
    
    return 0;
}