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

// 测试选择器分析器
void test_selector_analyzer() {
    std::cout << "测试选择器分析器...\n";
    
    SelectorAnalyzer analyzer;
    
    // 测试CSS选择器分析
    std::cout << "  测试CSS选择器分析...\n";
    auto styleBlock = std::make_shared<AST::StyleBlockNode>(Core::CHTLToken());
    styleBlock->SetContent(R"(
        .box {
            color: red;
        }
        
        #main {
            background: blue;
        }
        
        & {
            border: 1px solid black;
        }
    )");
    
    auto styleSelectors = analyzer.AnalyzeStyleSelectors(styleBlock);
    
    bool foundClassSelector = false;
    bool foundIdSelector = false;
    bool foundReferenceSelector = false;
    
    for (const auto& selector : styleSelectors) {
        if (selector.type == SelectorType::CLASS && selector.name == "box") {
            foundClassSelector = true;
        } else if (selector.type == SelectorType::ID && selector.name == "main") {
            foundIdSelector = true;
        } else if (selector.type == SelectorType::REFERENCE) {
            foundReferenceSelector = true;
        }
    }
    
    assert(foundClassSelector);
    assert(foundIdSelector);
    assert(foundReferenceSelector);
    
    std::cout << "    找到类选择器: " << foundClassSelector << " (box)\n";
    std::cout << "    找到ID选择器: " << foundIdSelector << " (main)\n";
    std::cout << "    找到引用选择器: " << foundReferenceSelector << " (&)\n";
    
    // 测试CHTL JS增强选择器分析
    std::cout << "  测试CHTL JS增强选择器分析...\n";
    auto scriptBlock = std::make_shared<AST::ScriptBlockNode>(Core::CHTLToken());
    scriptBlock->SetContent(R"(
        {{.button}}->listen({
            click: () => {
                console.log('clicked');
            }
        });
        
        {{#modal}}->animate({
            duration: 300
        });
        
        & {
            console.log('reference');
        }
    )");
    
    auto scriptSelectors = analyzer.AnalyzeScriptSelectors(scriptBlock);
    
    bool foundEnhancedClass = false;
    bool foundEnhancedId = false;
    bool foundScriptReference = false;
    
    for (const auto& selector : scriptSelectors) {
        if (selector.type == SelectorType::ENHANCED_CLASS && selector.name == "button") {
            foundEnhancedClass = true;
        } else if (selector.type == SelectorType::ENHANCED_ID && selector.name == "modal") {
            foundEnhancedId = true;
        } else if (selector.type == SelectorType::REFERENCE) {
            foundScriptReference = true;
        }
    }
    
    assert(foundEnhancedClass);
    assert(foundEnhancedId);
    assert(foundScriptReference);
    
    std::cout << "    找到增强类选择器: " << foundEnhancedClass << " ({{.button}})\n";
    std::cout << "    找到增强ID选择器: " << foundEnhancedId << " ({{#modal}})\n";
    std::cout << "    找到脚本引用选择器: " << foundScriptReference << " (&)\n";
    
    std::cout << "  ✅ 选择器分析器测试通过\n";
}

// 测试样式块选择器自动化
void test_style_block_automation() {
    std::cout << "测试样式块选择器自动化...\n";
    
    // 创建配置：启用样式块自动化
    SelectorAutomationConfig config;
    config.disableStyleAutoAddClass = false;
    config.disableStyleAutoAddId = false;
    
    SelectorAutomationEngine engine(config);
    
    // 创建没有class和id属性的元素
    auto element = std::make_shared<AST::ElementNode>("div", Core::CHTLToken());
    
    // 创建包含类选择器的样式块
    auto styleBlock = std::make_shared<AST::StyleBlockNode>(Core::CHTLToken());
    styleBlock->SetContent(R"(
        .container {
            width: 100%;
        }
        
        .box {
            height: 200px;
        }
        
        #main {
            background: white;
        }
    )");
    
    element->AddChild(styleBlock);
    
    // 处理自动化
    bool processed = engine.ProcessElementAutomation(element);
    assert(processed);
    
    // 验证自动添加的属性
    const auto& attributes = element->GetAttributes();
    
    bool hasClass = false;
    bool hasId = false;
    std::string className;
    std::string idName;
    
    for (const auto& attr : attributes) {
        if (attr.name == "class") {
            hasClass = true;
            className = attr.value;
        } else if (attr.name == "id") {
            hasId = true;
            idName = attr.value;
        }
    }
    
    assert(hasClass);  // 应该自动添加第一个类选择器
    assert(hasId);     // 应该自动添加第一个ID选择器
    assert(className == "container");  // 第一个类选择器
    assert(idName == "main");          // 第一个ID选择器
    
    std::cout << "  自动添加的类选择器: " << className << "\n";
    std::cout << "  自动添加的ID选择器: " << idName << "\n";
    std::cout << "  ✅ 样式块选择器自动化测试通过\n";
}

// 测试脚本块选择器自动化
void test_script_block_automation() {
    std::cout << "测试脚本块选择器自动化...\n";
    
    // 创建配置：启用脚本块自动化（DISABLE_SCRIPT_AUTO_ADD_CLASS = true的特殊逻辑）
    SelectorAutomationConfig config;
    config.disableScriptAutoAddClass = true;  // 启用特殊逻辑
    config.disableScriptAutoAddId = true;     // 启用特殊逻辑
    
    SelectorAutomationEngine engine(config);
    
    // 创建没有class和id属性的元素
    auto element = std::make_shared<AST::ElementNode>("div", Core::CHTLToken());
    
    // 创建包含增强选择器的脚本块
    auto scriptBlock = std::make_shared<AST::ScriptBlockNode>(Core::CHTLToken());
    scriptBlock->SetContent(R"(
        {{.button}}->listen({
            click: () => {
                console.log('button clicked');
            }
        });
        
        {{#modal}}->animate({
            duration: 300
        });
        
        {{.card}}->listen({
            hover: () => {
                console.log('card hovered');
            }
        });
    )");
    
    element->AddChild(scriptBlock);
    
    // 处理自动化
    bool processed = engine.ProcessElementAutomation(element);
    assert(processed);
    
    // 验证自动添加的属性
    const auto& attributes = element->GetAttributes();
    
    bool hasClass = false;
    bool hasId = false;
    std::string className;
    std::string idName;
    
    for (const auto& attr : attributes) {
        if (attr.name == "class") {
            hasClass = true;
            className = attr.value;
        } else if (attr.name == "id") {
            hasId = true;
            idName = attr.value;
        }
    }
    
    // 当DISABLE_SCRIPT_AUTO_ADD_CLASS为true且没有局部style时，第一个{{.box}}会被自动添加
    assert(hasClass);  // 应该自动添加第一个增强类选择器
    assert(hasId);     // 应该自动添加第一个增强ID选择器
    assert(className == "button");  // 第一个增强类选择器
    assert(idName == "modal");      // 第一个增强ID选择器
    
    std::cout << "  自动添加的类选择器: " << className << " (来自{{.button}})\n";
    std::cout << "  自动添加的ID选择器: " << idName << " (来自{{#modal}})\n";
    std::cout << "  ✅ 脚本块选择器自动化测试通过\n";
}

// 测试引用选择器解析
void test_reference_selector_resolver() {
    std::cout << "测试引用选择器解析...\n";
    
    ReferenceSelectorResolver resolver;
    
    // 创建有class和id属性的元素
    auto element = std::make_shared<AST::ElementNode>("div", Core::CHTLToken());
    
    AST::AttributeNode classAttr;
    classAttr.name = "class";
    classAttr.value = "container";
    element->AddAttribute(classAttr);
    
    AST::AttributeNode idAttr;
    idAttr.name = "id";
    idAttr.value = "main";
    element->AddAttribute(idAttr);
    
    // 测试样式块引用选择器解析（优先class）
    std::cout << "  测试样式块引用选择器解析...\n";
    auto styleBlock = std::make_shared<AST::StyleBlockNode>(Core::CHTLToken());
    styleBlock->SetContent(R"(
        & {
            padding: 20px;
        }
        
        &:hover {
            background: lightgray;
        }
    )");
    
    element->AddChild(styleBlock);
    
    int styleReferences = resolver.ResolveStyleReferences(styleBlock, element);
    assert(styleReferences > 0);
    
    // 验证&被替换为.container（优先class）
    std::string styleContent = styleBlock->GetContent();
    assert(styleContent.find(".container") != std::string::npos);
    assert(styleContent.find("&") == std::string::npos || 
           styleContent.find("&") > styleContent.find(".container")); // &应该被替换
    
    std::cout << "    样式块中&引用选择器已替换为: .container\n";
    
    // 测试脚本块引用选择器解析（优先id）
    std::cout << "  测试脚本块引用选择器解析...\n";
    auto scriptBlock = std::make_shared<AST::ScriptBlockNode>(Core::CHTLToken());
    scriptBlock->SetContent(R"(
        &->listen({
            click: () => {
                console.log('element clicked');
            }
        });
        
        &->animate({
            duration: 500
        });
    )");
    
    element->AddChild(scriptBlock);
    
    int scriptReferences = resolver.ResolveScriptReferences(scriptBlock, element);
    assert(scriptReferences > 0);
    
    // 验证&被替换为{{#main}}（优先id）
    std::string scriptContent = scriptBlock->GetContent();
    assert(scriptContent.find("{{#main}}") != std::string::npos);
    assert(scriptContent.find("&") == std::string::npos || 
           scriptContent.find("&") > scriptContent.find("{{#main}}")); // &应该被替换
    
    std::cout << "    脚本块中&引用选择器已替换为: {{#main}}\n";
    
    std::cout << "  ✅ 引用选择器解析测试通过\n";
}

// 测试选择器自动化管理器
void test_selector_automation_manager() {
    std::cout << "测试选择器自动化管理器...\n";
    
    SelectorAutomationManager manager;
    
    // 设置配置
    SelectorAutomationConfig config;
    config.disableStyleAutoAddClass = false;
    config.disableStyleAutoAddId = false;
    config.disableScriptAutoAddClass = true;
    config.disableScriptAutoAddId = true;
    
    manager.SetConfig(config);
    
    // 创建测试元素
    auto element = std::make_shared<AST::ElementNode>("div", Core::CHTLToken());
    
    // 创建样式块
    auto styleBlock = std::make_shared<AST::StyleBlockNode>(Core::CHTLToken());
    styleBlock->SetContent(R"(
        .primary {
            color: blue;
        }
        
        #header {
            font-size: 24px;
        }
        
        & {
            margin: 10px;
        }
    )");
    
    element->AddChild(styleBlock);
    
    // 创建脚本块
    auto scriptBlock = std::make_shared<AST::ScriptBlockNode>(Core::CHTLToken());
    scriptBlock->SetContent(R"(
        {{.secondary}}->listen({
            click: () => { console.log('secondary'); }
        });
        
        {{#footer}}->animate({
            duration: 200
        });
        
        &->listen({
            hover: () => { console.log('hover'); }
        });
    )");
    
    element->AddChild(scriptBlock);
    
    // 处理元素
    bool processed = manager.ProcessElement(element);
    assert(processed);
    
    // 验证结果
    const auto& attributes = element->GetAttributes();
    
    bool hasClass = false;
    bool hasId = false;
    std::string className;
    std::string idName;
    
    for (const auto& attr : attributes) {
        if (attr.name == "class") {
            hasClass = true;
            className = attr.value;
        } else if (attr.name == "id") {
            hasId = true;
            idName = attr.value;
        }
    }
    
    // 由于样式块有选择器，应该从样式块自动添加
    assert(hasClass);
    assert(hasId);
    assert(className == "primary");  // 第一个样式类选择器
    assert(idName == "header");      // 第一个样式ID选择器
    
    std::cout << "  自动化处理结果:\n";
    std::cout << "    自动添加的类: " << className << " (来自样式块)\n";
    std::cout << "    自动添加的ID: " << idName << " (来自样式块)\n";
    
    // 验证引用选择器被正确替换
    std::string finalStyleContent = styleBlock->GetContent();
    std::string finalScriptContent = scriptBlock->GetContent();
    
    // 样式块中&应该被替换为.primary（优先class）
    assert(finalStyleContent.find(".primary") != std::string::npos);
    
    // 脚本块中&应该被替换为{{#header}}（优先id）
    assert(finalScriptContent.find("{{#header}}") != std::string::npos);
    
    std::cout << "  引用选择器解析结果:\n";
    std::cout << "    样式块&引用: .primary\n";
    std::cout << "    脚本块&引用: {{#header}}\n";
    
    // 获取统计信息
    std::string stats = manager.GetStatistics();
    std::cout << "  " << stats;
    
    std::cout << "  ✅ 选择器自动化管理器测试通过\n";
}

// 测试复杂的自动化场景
void test_complex_automation_scenario() {
    std::cout << "测试复杂的自动化场景...\n";
    
    SelectorAutomationManager manager;
    
    // 场景1：仅有脚本块，没有样式块
    std::cout << "  场景1：仅有脚本块自动化...\n";
    
    SelectorAutomationConfig config1;
    config1.disableScriptAutoAddClass = true;  // 启用脚本块特殊逻辑
    config1.disableScriptAutoAddId = true;
    manager.SetConfig(config1);
    
    auto element1 = std::make_shared<AST::ElementNode>("button", Core::CHTLToken());
    
    auto scriptBlock1 = std::make_shared<AST::ScriptBlockNode>(Core::CHTLToken());
    scriptBlock1->SetContent(R"(
        {{.btn}}->listen({
            click: () => { console.log('btn clicked'); }
        });
        
        {{#submit}}->listen({
            submit: () => { console.log('form submitted'); }
        });
    )");
    
    element1->AddChild(scriptBlock1);
    
    bool processed1 = manager.ProcessElement(element1);
    assert(processed1);
    
    // 验证脚本块自动化生效
    const auto& attrs1 = element1->GetAttributes();
    bool hasClassFromScript = false;
    bool hasIdFromScript = false;
    
    for (const auto& attr : attrs1) {
        if (attr.name == "class" && attr.value == "btn") {
            hasClassFromScript = true;
        } else if (attr.name == "id" && attr.value == "submit") {
            hasIdFromScript = true;
        }
    }
    
    assert(hasClassFromScript);  // 第一个{{.btn}}应该被自动添加
    assert(hasIdFromScript);     // 第一个{{#submit}}应该被自动添加
    
    std::cout << "    ✅ 脚本块自动化：class=btn, id=submit\n";
    
    // 场景2：同时有样式块和脚本块
    std::cout << "  场景2：样式块优先级测试...\n";
    
    auto element2 = std::make_shared<AST::ElementNode>("section", Core::CHTLToken());
    
    // 样式块（应该优先处理）
    auto styleBlock2 = std::make_shared<AST::StyleBlockNode>(Core::CHTLToken());
    styleBlock2->SetContent(R"(
        .section {
            display: block;
        }
    )");
    
    // 脚本块
    auto scriptBlock2 = std::make_shared<AST::ScriptBlockNode>(Core::CHTLToken());
    scriptBlock2->SetContent(R"(
        {{.different}}->listen({
            click: () => { console.log('different'); }
        });
    )");
    
    element2->AddChild(styleBlock2);
    element2->AddChild(scriptBlock2);
    
    bool processed2 = manager.ProcessElement(element2);
    assert(processed2);
    
    // 验证样式块优先级
    const auto& attrs2 = element2->GetAttributes();
    bool foundStyleClass = false;
    
    for (const auto& attr : attrs2) {
        if (attr.name == "class" && attr.value == "section") {
            foundStyleClass = true;
            break;
        }
    }
    
    assert(foundStyleClass);  // 应该使用样式块的类选择器，而不是脚本块的
    
    std::cout << "    ✅ 样式块优先级正确：使用.section而不是.different\n";
    
    std::cout << "  ✅ 复杂自动化场景测试通过\n";
}

// 测试引用选择器优先级规则
void test_reference_selector_priority() {
    std::cout << "测试引用选择器优先级规则...\n";
    
    ReferenceSelectorResolver resolver;
    
    // 创建同时有class和id的元素
    auto element = std::make_shared<AST::ElementNode>("div", Core::CHTLToken());
    
    AST::AttributeNode classAttr;
    classAttr.name = "class";
    classAttr.value = "widget";
    element->AddAttribute(classAttr);
    
    AST::AttributeNode idAttr;
    idAttr.name = "id";
    idAttr.value = "sidebar";
    element->AddAttribute(idAttr);
    
    // 测试样式块引用选择器（优先class）
    std::cout << "  测试样式块引用优先级（优先class）...\n";
    
    auto styleBlock = std::make_shared<AST::StyleBlockNode>(Core::CHTLToken());
    styleBlock->SetContent(R"(
        & {
            background: white;
        }
        
        &:hover {
            background: gray;
        }
    )");
    
    int styleRefs = resolver.ResolveStyleReferences(styleBlock, element);
    assert(styleRefs > 0);
    
    std::string styleContent = styleBlock->GetContent();
    assert(styleContent.find(".widget") != std::string::npos);  // 应该优先使用class
    assert(styleContent.find("#sidebar") == std::string::npos); // 不应该使用id
    
    std::cout << "    样式块&引用优先使用: .widget (class优先)\n";
    
    // 测试脚本块引用选择器（优先id）
    std::cout << "  测试脚本块引用优先级（优先id）...\n";
    
    auto scriptBlock = std::make_shared<AST::ScriptBlockNode>(Core::CHTLToken());
    scriptBlock->SetContent(R"(
        &->listen({
            click: () => {
                console.log('clicked');
            }
        });
        
        &->animate({
            duration: 300
        });
    )");
    
    int scriptRefs = resolver.ResolveScriptReferences(scriptBlock, element);
    assert(scriptRefs > 0);
    
    std::string scriptContent = scriptBlock->GetContent();
    assert(scriptContent.find("{{#sidebar}}") != std::string::npos); // 应该优先使用id
    assert(scriptContent.find("{{.widget}}") == std::string::npos);  // 不应该使用class
    
    std::cout << "    脚本块&引用优先使用: {{#sidebar}} (id优先)\n";
    
    std::cout << "  ✅ 引用选择器优先级规则测试通过\n";
}

// 测试配置禁用功能
void test_config_disable_functionality() {
    std::cout << "测试配置禁用功能...\n";
    
    // 测试禁用样式块自动化
    std::cout << "  测试禁用样式块自动化...\n";
    
    SelectorAutomationConfig config;
    config.disableStyleAutoAddClass = true;   // 禁用样式块类自动化
    config.disableStyleAutoAddId = true;      // 禁用样式块ID自动化
    
    SelectorAutomationEngine engine(config);
    
    auto element = std::make_shared<AST::ElementNode>("div", Core::CHTLToken());
    
    auto styleBlock = std::make_shared<AST::StyleBlockNode>(Core::CHTLToken());
    styleBlock->SetContent(R"(
        .test {
            color: red;
        }
        
        #test {
            background: blue;
        }
    )");
    
    element->AddChild(styleBlock);
    
    bool processed = engine.ProcessElementAutomation(element);
    
    // 验证没有自动添加属性
    const auto& attributes = element->GetAttributes();
    assert(attributes.empty()); // 应该没有自动添加任何属性
    
    std::cout << "    ✅ 样式块自动化已正确禁用\n";
    
    // 测试禁用脚本块自动化
    std::cout << "  测试禁用脚本块自动化...\n";
    
    SelectorAutomationConfig config2;
    config2.disableScriptAutoAddClass = false; // 禁用脚本块特殊逻辑
    config2.disableScriptAutoAddId = false;
    
    SelectorAutomationEngine engine2(config2);
    
    auto element2 = std::make_shared<AST::ElementNode>("span", Core::CHTLToken());
    
    auto scriptBlock2 = std::make_shared<AST::ScriptBlockNode>(Core::CHTLToken());
    scriptBlock2->SetContent(R"(
        {{.test}}->listen({
            click: () => { console.log('test'); }
        });
    )");
    
    element2->AddChild(scriptBlock2);
    
    bool processed2 = engine2.ProcessElementAutomation(element2);
    
    // 当DISABLE_SCRIPT_AUTO_ADD_CLASS为false时，不应该触发脚本块自动化
    const auto& attributes2 = element2->GetAttributes();
    assert(attributes2.empty()); // 应该没有自动添加任何属性
    
    std::cout << "    ✅ 脚本块自动化已正确禁用\n";
    
    std::cout << "  ✅ 配置禁用功能测试通过\n";
}

// 测试完整的选择器自动化工作流程
void test_complete_selector_workflow() {
    std::cout << "测试完整的选择器自动化工作流程...\n";
    
    try {
        SelectorAutomationManager manager;
        
        // 1. 配置加载
        auto configNode = std::make_shared<AST::ConfigurationNode>("", Core::CHTLToken());
        configNode->AddSetting("DISABLE_STYLE_AUTO_ADD_CLASS", "false");
        configNode->AddSetting("DISABLE_STYLE_AUTO_ADD_ID", "false");
        configNode->AddSetting("DISABLE_SCRIPT_AUTO_ADD_CLASS", "true");
        configNode->AddSetting("DISABLE_SCRIPT_AUTO_ADD_ID", "true");
        
        manager.LoadConfigFromNode(configNode);
        
        // 2. 创建复杂的测试元素
        auto element = std::make_shared<AST::ElementNode>("article", Core::CHTLToken());
        
        // 样式块
        auto styleBlock = std::make_shared<AST::StyleBlockNode>(Core::CHTLToken());
        styleBlock->SetContent(R"(
            .article {
                padding: 20px;
                margin: 10px;
            }
            
            .content {
                line-height: 1.6;
            }
            
            #main-article {
                background: white;
            }
            
            & {
                border: 1px solid #ccc;
            }
            
            &:hover {
                box-shadow: 0 4px 8px rgba(0,0,0,0.1);
            }
        )");
        
        // 脚本块
        auto scriptBlock = std::make_shared<AST::ScriptBlockNode>(Core::CHTLToken());
        scriptBlock->SetContent(R"(
            {{.interactive}}->listen({
                click: (e) => {
                    console.log('Interactive element clicked');
                }
            });
            
            {{#dynamic}}->animate({
                duration: 400,
                easing: 'ease-in-out'
            });
            
            &->listen({
                scroll: () => {
                    console.log('Article scrolled');
                }
            });
        )");
        
        element->AddChild(styleBlock);
        element->AddChild(scriptBlock);
        
        // 3. 处理完整工作流程
        bool processed = manager.ProcessElement(element);
        assert(processed);
        
        // 4. 验证最终结果
        const auto& finalAttrs = element->GetAttributes();
        
        std::string finalClass;
        std::string finalId;
        
        for (const auto& attr : finalAttrs) {
            if (attr.name == "class") {
                finalClass = attr.value;
            } else if (attr.name == "id") {
                finalId = attr.value;
            }
        }
        
        // 应该使用样式块的第一个选择器
        assert(finalClass == "article");      // 第一个样式类选择器
        assert(finalId == "main-article");    // 第一个样式ID选择器
        
        // 验证引用选择器替换
        std::string finalStyleContent = styleBlock->GetContent();
        std::string finalScriptContent = scriptBlock->GetContent();
        
        assert(finalStyleContent.find(".article") != std::string::npos);     // 样式块&->class
        assert(finalScriptContent.find("{{#main-article}}") != std::string::npos); // 脚本块&->id
        
        std::cout << "  最终结果:\n";
        std::cout << "    元素class: " << finalClass << "\n";
        std::cout << "    元素id: " << finalId << "\n";
        std::cout << "    样式块&引用: .article\n";
        std::cout << "    脚本块&引用: {{#main-article}}\n";
        
        // 5. 统计信息
        std::string stats = manager.GetStatistics();
        std::cout << "  " << stats;
        
        std::cout << "  ✅ 完整选择器自动化工作流程测试通过\n";
        
    } catch (const std::exception& e) {
        std::cerr << "完整工作流程测试异常: " << e.what() << std::endl;
        assert(false);
    }
}

int main() {
    std::cout << "运行选择器自动化与引用规则测试...\n\n";
    
    try {
        test_selector_automation_config();
        test_selector_analyzer();
        test_style_block_automation();
        test_script_block_automation();
        test_reference_selector_resolver();
        test_selector_automation_manager();
        test_config_disable_functionality();
        test_complete_selector_workflow();
        
        std::cout << "\n🎉 所有选择器自动化与引用规则测试通过!\n";
        std::cout << "✅ 选择器自动化配置正常\n";
        std::cout << "✅ 选择器分析器正常\n";
        std::cout << "✅ 样式块选择器自动化正常\n";
        std::cout << "✅ 脚本块选择器自动化正常\n";
        std::cout << "✅ 引用选择器解析正常\n";
        std::cout << "✅ 选择器自动化管理器正常\n";
        std::cout << "✅ 配置禁用功能正常\n";
        std::cout << "✅ 完整选择器工作流程正常\n";
        
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
        std::cout << "   原始嵌入和注释不受选择器自动化影响\n";
        
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "测试失败: " << e.what() << std::endl;
        return 1;
    }
}