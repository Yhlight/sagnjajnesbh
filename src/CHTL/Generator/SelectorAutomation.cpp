#include "CHTL/Generator/SelectorAutomation.h"
#include "Utils/StringUtils.h"
#include <regex>
#include <sstream>

namespace CHTL {
namespace Generator {

SelectorAutomation::SelectorAutomation() {
    // 默认配置
}

void SelectorAutomation::SetConfig(const AutomationConfig& config) {
    config_ = config;
}

void SelectorAutomation::UpdateConfigFromContext(const std::unordered_map<std::string, std::string>& contextVariables) {
    auto it = contextVariables.find("__disable_style_auto_add_class");
    if (it != contextVariables.end()) {
        config_.disableStyleAutoAddClass = (it->second == "true");
    }
    
    it = contextVariables.find("__disable_style_auto_add_id");
    if (it != contextVariables.end()) {
        config_.disableStyleAutoAddId = (it->second == "true");
    }
    
    it = contextVariables.find("__disable_script_auto_add_class");
    if (it != contextVariables.end()) {
        config_.disableScriptAutoAddClass = (it->second == "true");
    }
    
    it = contextVariables.find("__disable_script_auto_add_id");
    if (it != contextVariables.end()) {
        config_.disableScriptAutoAddId = (it->second == "true");
    }
}

std::vector<SelectorInfo> SelectorAutomation::AnalyzeStyleBlockSelectors(std::shared_ptr<AST::StyleBlockNode> styleBlock) {
    std::vector<SelectorInfo> selectors;
    
    if (!styleBlock) {
        return selectors;
    }
    
    // 遍历样式块的子节点，查找CSS选择器
    for (const auto& child : styleBlock->GetChildren()) {
        ExtractSelectorsFromNode(child, selectors);
    }
    
    // 标记第一个类选择器和ID选择器
    bool firstClassFound = false;
    bool firstIdFound = false;
    
    for (auto& selector : selectors) {
        if (selector.type == SelectorType::CLASS_SELECTOR && !firstClassFound) {
            selector.isFirst = true;
            firstClassFound = true;
        } else if (selector.type == SelectorType::ID_SELECTOR && !firstIdFound) {
            selector.isFirst = true;
            firstIdFound = true;
        }
    }
    
    return selectors;
}

std::vector<SelectorInfo> SelectorAutomation::AnalyzeScriptBlockSelectors(std::shared_ptr<AST::ScriptBlockNode> scriptBlock) {
    std::vector<SelectorInfo> selectors;
    
    if (!scriptBlock) {
        return selectors;
    }
    
    // 遍历脚本块的子节点，查找选择器引用
    for (const auto& child : scriptBlock->GetChildren()) {
        ExtractSelectorsFromNode(child, selectors);
    }
    
    return selectors;
}

std::string SelectorAutomation::ProcessStyleReference(std::shared_ptr<AST::StyleBlockNode> styleBlock,
                                                     std::shared_ptr<AST::ElementNode> element) {
    if (!styleBlock || !element) {
        return "";
    }
    
    // 样式块中的&引用优先选择class
    std::string classValue = GetElementAttribute(element, "class");
    if (!classValue.empty()) {
        // 返回第一个class
        std::istringstream iss(classValue);
        std::string firstClass;
        iss >> firstClass;
        return "." + firstClass;
    }
    
    // 如果没有class，尝试id
    std::string idValue = GetElementAttribute(element, "id");
    if (!idValue.empty()) {
        return "#" + idValue;
    }
    
    // 如果都没有，返回元素名
    return element->GetTagName();
}

std::string SelectorAutomation::ProcessScriptReference(std::shared_ptr<AST::ScriptBlockNode> scriptBlock,
                                                      std::shared_ptr<AST::ElementNode> element) {
    if (!scriptBlock || !element) {
        return "";
    }
    
    // 脚本块中的{{&}}引用优先选择id
    std::string idValue = GetElementAttribute(element, "id");
    if (!idValue.empty()) {
        return "#" + idValue;
    }
    
    // 如果没有id，尝试class
    std::string classValue = GetElementAttribute(element, "class");
    if (!classValue.empty()) {
        std::istringstream iss(classValue);
        std::string firstClass;
        iss >> firstClass;
        return "." + firstClass;
    }
    
    // 如果都没有，返回元素名
    return element->GetTagName();
}

bool SelectorAutomation::AutoAddClassAttribute(std::shared_ptr<AST::ElementNode> element, 
                                              const std::string& firstClassSelector) {
    if (!element || firstClassSelector.empty()) {
        return false;
    }
    
    // 检查配置是否禁用
    if (config_.disableStyleAutoAddClass) {
        return false;
    }
    
    // 检查元素是否已有class属性
    if (ElementHasAttribute(element, "class")) {
        return false;
    }
    
    // 提取类名（去掉.前缀）
    std::string className = ExtractSelectorName(firstClassSelector, SelectorType::CLASS_SELECTOR);
    if (className.empty()) {
        return false;
    }
    
    // 添加class属性
    SetElementAttribute(element, "class", className);
    return true;
}

bool SelectorAutomation::AutoAddIdAttribute(std::shared_ptr<AST::ElementNode> element, 
                                           const std::string& firstIdSelector) {
    if (!element || firstIdSelector.empty()) {
        return false;
    }
    
    // 检查配置是否禁用
    if (config_.disableStyleAutoAddId) {
        return false;
    }
    
    // 检查元素是否已有id属性
    if (ElementHasAttribute(element, "id")) {
        return false;
    }
    
    // 提取ID名（去掉#前缀）
    std::string idName = ExtractSelectorName(firstIdSelector, SelectorType::ID_SELECTOR);
    if (idName.empty()) {
        return false;
    }
    
    // 添加id属性
    SetElementAttribute(element, "id", idName);
    return true;
}

bool SelectorAutomation::CheckScriptSelectorAutomation(std::shared_ptr<AST::ElementNode> element,
                                                      std::shared_ptr<AST::StyleBlockNode> styleBlock) {
    if (!element) {
        return false;
    }
    
    // 检查配置
    bool needClassAuto = !config_.disableScriptAutoAddClass;
    bool needIdAuto = !config_.disableScriptAutoAddId;
    
    if (!needClassAuto && !needIdAuto) {
        return false;
    }
    
    // 检查样式块是否触发了自动化
    bool styleAutoTriggered = false;
    if (styleBlock) {
        auto styleSelectors = AnalyzeStyleBlockSelectors(styleBlock);
        
        for (const auto& selector : styleSelectors) {
            if (selector.isFirst) {
                if (selector.type == SelectorType::CLASS_SELECTOR && 
                    ElementHasAttribute(element, "class")) {
                    styleAutoTriggered = true;
                    break;
                }
                if (selector.type == SelectorType::ID_SELECTOR && 
                    ElementHasAttribute(element, "id")) {
                    styleAutoTriggered = true;
                    break;
                }
            }
        }
    }
    
    // 如果局部style没有触发自动化添加，脚本可以进行自动化
    return !styleAutoTriggered;
}

std::string SelectorAutomation::ExtractSelectorName(const std::string& selector, SelectorType type) {
    if (selector.empty()) {
        return "";
    }
    
    switch (type) {
        case SelectorType::CLASS_SELECTOR:
            if (selector.front() == '.') {
                return selector.substr(1);
            }
            break;
        case SelectorType::ID_SELECTOR:
            if (selector.front() == '#') {
                return selector.substr(1);
            }
            break;
        case SelectorType::ELEMENT_SELECTOR:
            return selector;
    }
    
    return selector;
}

bool SelectorAutomation::ElementHasAttribute(std::shared_ptr<AST::ElementNode> element, 
                                            const std::string& attributeName) {
    if (!element) {
        return false;
    }
    
    const auto& attributes = element->GetAttributes();
    return attributes.find(attributeName) != attributes.end();
}

std::string SelectorAutomation::GetElementAttribute(std::shared_ptr<AST::ElementNode> element, 
                                                   const std::string& attributeName) {
    if (!element) {
        return "";
    }
    
    const auto& attributes = element->GetAttributes();
    auto it = attributes.find(attributeName);
    return (it != attributes.end()) ? it->second : "";
}

void SelectorAutomation::SetElementAttribute(std::shared_ptr<AST::ElementNode> element, 
                                            const std::string& attributeName, 
                                            const std::string& attributeValue) {
    if (!element) {
        return;
    }
    
    element->SetAttribute(attributeName, attributeValue);
}

bool SelectorAutomation::IsClassSelector(const std::string& selector) {
    return !selector.empty() && selector.front() == '.';
}

bool SelectorAutomation::IsIdSelector(const std::string& selector) {
    return !selector.empty() && selector.front() == '#';
}

void SelectorAutomation::ExtractSelectorsFromNode(std::shared_ptr<AST::ASTNode> node, 
                                                  std::vector<SelectorInfo>& selectors) {
    if (!node) {
        return;
    }
    
    switch (node->GetType()) {
        case AST::NodeType::CSS_SELECTOR: {
            auto selectorNode = std::dynamic_pointer_cast<AST::CSSSelectorNode>(node);
            if (selectorNode) {
                std::string selectorText = selectorNode->GetSelector();
                SelectorInfo info;
                
                if (IsClassSelector(selectorText)) {
                    info.type = SelectorType::CLASS_SELECTOR;
                    info.name = ExtractSelectorName(selectorText, SelectorType::CLASS_SELECTOR);
                } else if (IsIdSelector(selectorText)) {
                    info.type = SelectorType::ID_SELECTOR;
                    info.name = ExtractSelectorName(selectorText, SelectorType::ID_SELECTOR);
                } else {
                    info.type = SelectorType::ELEMENT_SELECTOR;
                    info.name = selectorText;
                }
                
                selectors.push_back(info);
            }
            break;
        }
        
        case AST::NodeType::CHTL_JS: {
            auto chtljsNode = std::dynamic_pointer_cast<AST::CHTLJSNode>(node);
            if (chtljsNode) {
                ExtractSelectorsFromCHTLJS(chtljsNode, selectors);
            }
            break;
        }
        
        default:
            // 递归处理子节点
            for (const auto& child : node->GetChildren()) {
                ExtractSelectorsFromNode(child, selectors);
            }
            break;
    }
}

void SelectorAutomation::ExtractSelectorsFromCHTLJS(std::shared_ptr<AST::CHTLJSNode> chtljsNode,
                                                   std::vector<SelectorInfo>& selectors) {
    if (!chtljsNode) {
        return;
    }
    
    std::string expression = chtljsNode->GetExpression();
    
    // 使用正则表达式匹配{{.box}}和{{#box}}格式
    std::regex classPattern(R"(\{\{\.([a-zA-Z_][a-zA-Z0-9_-]*)\}\})");
    std::regex idPattern(R"(\{\{\#([a-zA-Z_][a-zA-Z0-9_-]*)\}\})");
    
    std::smatch match;
    
    // 查找类选择器引用
    std::string::const_iterator searchStart(expression.cbegin());
    while (std::regex_search(searchStart, expression.cend(), match, classPattern)) {
        SelectorInfo info;
        info.type = SelectorType::CLASS_SELECTOR;
        info.name = match[1].str();
        info.isUsed = true;
        selectors.push_back(info);
        
        searchStart = match.suffix().first;
    }
    
    // 查找ID选择器引用
    searchStart = expression.cbegin();
    while (std::regex_search(searchStart, expression.cend(), match, idPattern)) {
        SelectorInfo info;
        info.type = SelectorType::ID_SELECTOR;
        info.name = match[1].str();
        info.isUsed = true;
        selectors.push_back(info);
        
        searchStart = match.suffix().first;
    }
}

} // namespace Generator
} // namespace CHTL