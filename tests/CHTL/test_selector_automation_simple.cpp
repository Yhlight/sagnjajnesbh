#include <iostream>
#include <cassert>
#include <memory>
#include "CHTL/Selector/SelectorAutomation.h"
#include "CHTL/AST/CHTLASTNodes.h"
#include "CHTL/Core/CHTLToken.h"

using namespace CHTL;
using namespace CHTL::Selector;

// 测试选择器自动化配置
void test_selector_automation_config() {
    std::cout << "测试选择器自动化配置...\n";
    
    SelectorAutomationConfig config;
    
    // 测试默认配置
    assert(!config.disableStyleAutoAddClass);   // 默认启用样式块类自动化
    assert(!config.disableStyleAutoAddId);      // 默认启用样式块ID自动化
    assert(config.disableScriptAutoAddClass);   // 默认禁用脚本块类自动化
    assert(config.disableScriptAutoAddId);      // 默认禁用脚本块ID自动化
    
    std::cout << "  默认配置验证:\n";
    std::cout << "    样式块类自动化: " << (!config.disableStyleAutoAddClass ? "启用" : "禁用") << "\n";
    std::cout << "    样式块ID自动化: " << (!config.disableStyleAutoAddId ? "启用" : "禁用") << "\n";
    std::cout << "    脚本块类自动化: " << (!config.disableScriptAutoAddClass ? "启用" : "禁用") << "\n";
    std::cout << "    脚本块ID自动化: " << (!config.disableScriptAutoAddId ? "启用" : "禁用") << "\n";
    
    // 测试配置节点加载
    auto configNode = std::make_shared<AST::ConfigurationNode>("SelectorConfig", Core::CHTLToken());
    configNode->AddSetting("DISABLE_STYLE_AUTO_ADD_CLASS", "true");
    configNode->AddSetting("DISABLE_STYLE_AUTO_ADD_ID", "false");
    configNode->AddSetting("DISABLE_SCRIPT_AUTO_ADD_CLASS", "false");
    configNode->AddSetting("DISABLE_SCRIPT_AUTO_ADD_ID", "true");
    
    config.LoadFromConfigNode(configNode);
    
    assert(config.disableStyleAutoAddClass);    // 应该被设置为true
    assert(!config.disableStyleAutoAddId);      // 应该被设置为false
    assert(!config.disableScriptAutoAddClass);  // 应该被设置为false
    assert(config.disableScriptAutoAddId);      // 应该被设置为true
    
    std::cout << "  配置加载验证:\n";
    std::cout << "    样式块类自动化: " << (!config.disableStyleAutoAddClass ? "启用" : "禁用") << "\n";
    std::cout << "    样式块ID自动化: " << (!config.disableStyleAutoAddId ? "启用" : "禁用") << "\n";
    std::cout << "    脚本块类自动化: " << (!config.disableScriptAutoAddClass ? "启用" : "禁用") << "\n";
    std::cout << "    脚本块ID自动化: " << (!config.disableScriptAutoAddId ? "启用" : "禁用") << "\n";
    
    std::cout << "  ✅ 选择器自动化配置测试通过\n";
}

// 测试选择器分析器的正则表达式
void test_selector_regex_parsing() {
    std::cout << "测试选择器正则表达式解析...\n";
    
    SelectorAnalyzer analyzer;
    
    // 测试CSS选择器解析
    std::cout << "  测试CSS选择器正则解析...\n";
    
    std::string cssContent = R"(
        .box {
            color: red;
        }
        
        .container {
            width: 100%;
        }
        
        #main {
            background: blue;
        }
        
        #header {
            font-size: 24px;
        }
        
        & {
            border: 1px solid black;
        }
        
        &:hover {
            opacity: 0.8;
        }
    )";
    
    auto cssSelectors = analyzer.ParseCSSSelectors(cssContent);
    
    // 验证解析结果
    int classCount = 0;
    int idCount = 0;
    std::vector<std::string> classNames;
    std::vector<std::string> idNames;
    
    for (const auto& selector : cssSelectors) {
        if (selector.type == SelectorType::CLASS) {
            classCount++;
            classNames.push_back(selector.name);
        } else if (selector.type == SelectorType::ID) {
            idCount++;
            idNames.push_back(selector.name);
        }
    }
    
    assert(classCount == 2);  // .box, .container
    assert(idCount == 2);     // #main, #header
    assert(std::find(classNames.begin(), classNames.end(), "box") != classNames.end());
    assert(std::find(classNames.begin(), classNames.end(), "container") != classNames.end());
    assert(std::find(idNames.begin(), idNames.end(), "main") != idNames.end());
    assert(std::find(idNames.begin(), idNames.end(), "header") != idNames.end());
    
    std::cout << "    找到类选择器: " << classCount << " 个 (box, container)\n";
    std::cout << "    找到ID选择器: " << idCount << " 个 (main, header)\n";
    
    // 测试引用选择器解析
    auto refSelectors = analyzer.ParseReferenceSelectors(cssContent);
    assert(refSelectors.size() >= 2); // & 和 &:hover
    
    std::cout << "    找到引用选择器: " << refSelectors.size() << " 个\n";
    
    // 测试CHTL JS增强选择器解析
    std::cout << "  测试CHTL JS增强选择器正则解析...\n";
    
    std::string jsContent = R"(
        {{.button}}->listen({
            click: () => {
                console.log('button clicked');
            }
        });
        
        {{.card}}->animate({
            duration: 300
        });
        
        {{#modal}}->listen({
            show: () => {
                console.log('modal shown');
            }
        });
        
        {{#dialog}}->animate({
            duration: 500
        });
    )";
    
    auto enhancedSelectors = analyzer.ParseEnhancedSelectors(jsContent);
    
    int enhancedClassCount = 0;
    int enhancedIdCount = 0;
    std::vector<std::string> enhancedClassNames;
    std::vector<std::string> enhancedIdNames;
    
    for (const auto& selector : enhancedSelectors) {
        if (selector.type == SelectorType::ENHANCED_CLASS) {
            enhancedClassCount++;
            enhancedClassNames.push_back(selector.name);
        } else if (selector.type == SelectorType::ENHANCED_ID) {
            enhancedIdCount++;
            enhancedIdNames.push_back(selector.name);
        }
    }
    
    assert(enhancedClassCount == 2);  // {{.button}}, {{.card}}
    assert(enhancedIdCount == 2);     // {{#modal}}, {{#dialog}}
    assert(std::find(enhancedClassNames.begin(), enhancedClassNames.end(), "button") != enhancedClassNames.end());
    assert(std::find(enhancedClassNames.begin(), enhancedClassNames.end(), "card") != enhancedClassNames.end());
    assert(std::find(enhancedIdNames.begin(), enhancedIdNames.end(), "modal") != enhancedIdNames.end());
    assert(std::find(enhancedIdNames.begin(), enhancedIdNames.end(), "dialog") != enhancedIdNames.end());
    
    std::cout << "    找到增强类选择器: " << enhancedClassCount << " 个 (button, card)\n";
    std::cout << "    找到增强ID选择器: " << enhancedIdCount << " 个 (modal, dialog)\n";
    
    std::cout << "  ✅ 选择器正则表达式解析测试通过\n";
}

// 测试选择器自动化引擎基础功能
void test_selector_automation_engine_basic() {
    std::cout << "测试选择器自动化引擎基础功能...\n";
    
    // 创建配置：启用样式块自动化
    SelectorAutomationConfig config;
    config.disableStyleAutoAddClass = false;
    config.disableStyleAutoAddId = false;
    
    SelectorAutomationEngine engine(config);
    
    // 创建测试元素
    auto element = std::make_shared<AST::ElementNode>("div", Core::CHTLToken());
    
    // 测试属性检查
    assert(!engine.HasClassAttribute(element));  // 初始没有class
    assert(!engine.HasIdAttribute(element));     // 初始没有id
    
    // 测试自动添加属性
    bool classAdded = engine.AutoAddClassAttribute(element, "test-class");
    bool idAdded = engine.AutoAddIdAttribute(element, "test-id");
    
    assert(classAdded);
    assert(idAdded);
    
    // 验证属性已添加
    assert(engine.HasClassAttribute(element));   // 现在应该有class
    assert(engine.HasIdAttribute(element));      // 现在应该有id
    assert(element->GetAttribute("class") == "test-class");
    assert(element->GetAttribute("id") == "test-id");
    
    std::cout << "  自动添加属性测试:\n";
    std::cout << "    添加的class: " << element->GetAttribute("class") << "\n";
    std::cout << "    添加的id: " << element->GetAttribute("id") << "\n";
    
    std::cout << "  ✅ 选择器自动化引擎基础功能测试通过\n";
}

// 测试引用选择器优先级
void test_reference_selector_priority() {
    std::cout << "测试引用选择器优先级...\n";
    
    ReferenceSelectorResolver resolver;
    
    // 创建同时有class和id的元素
    auto element = std::make_shared<AST::ElementNode>("div", Core::CHTLToken());
    element->AddAttribute("class", "widget");
    element->AddAttribute("id", "sidebar");
    
    // 测试样式块优先级（优先class）
    std::cout << "  测试样式块引用优先级（优先class）...\n";
    
    std::string stylePriority = resolver.GetStylePrioritySelector(element);
    assert(stylePriority == ".widget");  // 应该优先使用class
    
    std::cout << "    样式块优先选择器: " << stylePriority << "\n";
    
    // 测试脚本块优先级（优先id）
    std::cout << "  测试脚本块引用优先级（优先id）...\n";
    
    std::string scriptPriority = resolver.GetScriptPrioritySelector(element);
    assert(scriptPriority == "{{#sidebar}}");  // 应该优先使用id
    
    std::cout << "    脚本块优先选择器: " << scriptPriority << "\n";
    
    // 测试引用选择器替换
    std::cout << "  测试引用选择器替换...\n";
    
    std::string contentWithRef = R"(
        & {
            padding: 10px;
        }
        
        &:hover {
            background: gray;
        }
    )";
    
    std::string replacedContent = resolver.ReplaceReferenceSelectors(contentWithRef, ".widget");
    
    assert(replacedContent.find(".widget") != std::string::npos);
    assert(replacedContent.find("&") == std::string::npos || 
           replacedContent.find("&") > replacedContent.find(".widget"));
    
    std::cout << "    引用选择器替换成功: & -> .widget\n";
    
    std::cout << "  ✅ 引用选择器优先级测试通过\n";
}

// 测试选择器自动化管理器基础功能
void test_selector_automation_manager_basic() {
    std::cout << "测试选择器自动化管理器基础功能...\n";
    
    SelectorAutomationManager manager;
    
    // 测试配置设置
    SelectorAutomationConfig config;
    config.disableStyleAutoAddClass = true;
    config.disableStyleAutoAddId = false;
    config.disableScriptAutoAddClass = false;
    config.disableScriptAutoAddId = true;
    
    manager.SetConfig(config);
    
    // 验证配置已设置
    const auto& currentConfig = manager.GetConfig();
    assert(currentConfig.disableStyleAutoAddClass == true);
    assert(currentConfig.disableStyleAutoAddId == false);
    assert(currentConfig.disableScriptAutoAddClass == false);
    assert(currentConfig.disableScriptAutoAddId == true);
    
    std::cout << "  配置设置验证:\n";
    std::cout << "    样式块类自动化: " << (!currentConfig.disableStyleAutoAddClass ? "启用" : "禁用") << "\n";
    std::cout << "    样式块ID自动化: " << (!currentConfig.disableStyleAutoAddId ? "启用" : "禁用") << "\n";
    std::cout << "    脚本块类自动化: " << (!currentConfig.disableScriptAutoAddClass ? "启用" : "禁用") << "\n";
    std::cout << "    脚本块ID自动化: " << (!currentConfig.disableScriptAutoAddId ? "启用" : "禁用") << "\n";
    
    // 测试统计信息
    std::string stats = manager.GetStatistics();
    assert(!stats.empty());
    
    std::cout << "  统计信息:\n" << stats;
    
    // 测试重置统计
    manager.ResetStatistics();
    std::string resetStats = manager.GetStatistics();
    assert(resetStats.find("0") != std::string::npos); // 应该包含0的统计
    
    std::cout << "  ✅ 选择器自动化管理器基础功能测试通过\n";
}

// 测试选择器类型和信息结构
void test_selector_info_structure() {
    std::cout << "测试选择器信息结构...\n";
    
    // 测试选择器信息创建
    SelectorInfo classInfo;
    classInfo.type = SelectorType::CLASS;
    classInfo.name = "button";
    classInfo.fullSelector = ".button";
    classInfo.position = 0;
    classInfo.isAutoGenerated = false;
    
    assert(classInfo.type == SelectorType::CLASS);
    assert(classInfo.name == "button");
    assert(classInfo.fullSelector == ".button");
    assert(!classInfo.isAutoGenerated);
    
    SelectorInfo enhancedInfo;
    enhancedInfo.type = SelectorType::ENHANCED_CLASS;
    enhancedInfo.name = "modal";
    enhancedInfo.fullSelector = "{{.modal}}";
    enhancedInfo.position = 0;
    enhancedInfo.isAutoGenerated = true;
    
    assert(enhancedInfo.type == SelectorType::ENHANCED_CLASS);
    assert(enhancedInfo.name == "modal");
    assert(enhancedInfo.fullSelector == "{{.modal}}");
    assert(enhancedInfo.isAutoGenerated);
    
    SelectorInfo refInfo;
    refInfo.type = SelectorType::REFERENCE;
    refInfo.name = "&";
    refInfo.fullSelector = "&";
    refInfo.position = 0;
    refInfo.isAutoGenerated = false;
    
    assert(refInfo.type == SelectorType::REFERENCE);
    assert(refInfo.name == "&");
    assert(refInfo.fullSelector == "&");
    
    std::cout << "  选择器信息结构验证:\n";
    std::cout << "    类选择器: " << classInfo.fullSelector << " -> " << classInfo.name << "\n";
    std::cout << "    增强选择器: " << enhancedInfo.fullSelector << " -> " << enhancedInfo.name << "\n";
    std::cout << "    引用选择器: " << refInfo.fullSelector << " -> " << refInfo.name << "\n";
    
    std::cout << "  ✅ 选择器信息结构测试通过\n";
}

// 测试选择器名称提取
void test_selector_name_extraction() {
    std::cout << "测试选择器名称提取...\n";
    
    SelectorAnalyzer analyzer;
    
    // 测试各种选择器的名称提取
    assert(analyzer.ExtractSelectorName(".button") == "button");
    assert(analyzer.ExtractSelectorName("#modal") == "modal");
    assert(analyzer.ExtractSelectorName("{{.card}}") == "card");
    assert(analyzer.ExtractSelectorName("{{#dialog}}") == "dialog");
    assert(analyzer.ExtractSelectorName("&") == "&");
    assert(analyzer.ExtractSelectorName("div") == "div");
    
    std::cout << "  选择器名称提取验证:\n";
    std::cout << "    .button -> " << analyzer.ExtractSelectorName(".button") << "\n";
    std::cout << "    #modal -> " << analyzer.ExtractSelectorName("#modal") << "\n";
    std::cout << "    {{.card}} -> " << analyzer.ExtractSelectorName("{{.card}}") << "\n";
    std::cout << "    {{#dialog}} -> " << analyzer.ExtractSelectorName("{{#dialog}}") << "\n";
    std::cout << "    & -> " << analyzer.ExtractSelectorName("&") << "\n";
    
    std::cout << "  ✅ 选择器名称提取测试通过\n";
}

int main() {
    std::cout << "运行选择器自动化简化测试...\n\n";
    
    try {
        test_selector_automation_config();
        test_selector_regex_parsing();
        test_selector_automation_engine_basic();
        test_reference_selector_priority();
        test_selector_automation_manager_basic();
        test_selector_info_structure();
        test_selector_name_extraction();
        
        std::cout << "\n🎉 所有选择器自动化简化测试通过!\n";
        std::cout << "✅ 选择器自动化配置正常\n";
        std::cout << "✅ 选择器正则解析正常\n";
        std::cout << "✅ 选择器自动化引擎基础功能正常\n";
        std::cout << "✅ 引用选择器优先级正常\n";
        std::cout << "✅ 选择器自动化管理器基础功能正常\n";
        std::cout << "✅ 选择器信息结构正常\n";
        std::cout << "✅ 选择器名称提取正常\n";
        
        std::cout << "\n📋 选择器自动化功能总结:\n";
        std::cout << "🔧 [Configuration]配置支持:\n";
        std::cout << "   DISABLE_STYLE_AUTO_ADD_CLASS = false (默认启用)\n";
        std::cout << "   DISABLE_STYLE_AUTO_ADD_ID = false (默认启用)\n";
        std::cout << "   DISABLE_SCRIPT_AUTO_ADD_CLASS = true (默认禁用，启用特殊逻辑)\n";
        std::cout << "   DISABLE_SCRIPT_AUTO_ADD_ID = true (默认禁用，启用特殊逻辑)\n";
        std::cout << "\n🎯 自动化规则:\n";
        std::cout << "   局部样式块：class/id属性缺失时自动添加第一个选择器\n";
        std::cout << "   局部脚本块：特殊逻辑下，没有样式块时自动添加{{.box}}/{{#box}}\n";
        std::cout << "\n🔗 引用选择器规则:\n";
        std::cout << "   样式块&引用：优先选择class (.class)\n";
        std::cout << "   脚本块&引用：优先选择id ({{#id}})\n";
        std::cout << "\n✨ 特殊说明:\n";
        std::cout << "   {{box}}不会自动添加，只有{{.box}}和{{#box}}能够触发自动化\n";
        std::cout << "   引用选择器&会根据上下文自动替换为合适的选择器\n";
        
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "测试失败: " << e.what() << std::endl;
        return 1;
    }
}