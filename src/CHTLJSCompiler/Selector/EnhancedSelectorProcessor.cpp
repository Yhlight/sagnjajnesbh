#include "EnhancedSelectorProcessor.h"
#include <iostream>
#include <regex>
#include <algorithm>
#include <sstream>

namespace chtl {
namespace chtljs {

EnhancedSelectorProcessor::EnhancedSelectorProcessor() {
    initializeHtmlTags();
}

EnhancedSelectorProcessor::~EnhancedSelectorProcessor() = default;

// === 按语法文档：增强选择器处理 ===

std::string EnhancedSelectorProcessor::processEnhancedSelector(const std::string& selector) {
    // 按语法文档：{{CSS选择器}} 转换为DOM对象
    
    if (!isValidSelector(selector)) {
        addError("无效的增强选择器格式: " + selector);
        return "";
    }
    
    // 检查缓存
    auto it = selectorCache_.find(selector);
    if (it != selectorCache_.end()) {
        return it->second.jsCode;
    }
    
    // 解析选择器
    SelectorInfo selectorInfo = parseSelector(selector);
    
    // 生成JavaScript代码
    std::string jsCode = generateJavaScriptCode(selectorInfo);
    
    // 缓存结果
    selectorInfo.jsCode = jsCode;
    selectorCache_[selector] = selectorInfo;
    
    return jsCode;
}

EnhancedSelectorProcessor::SelectorInfo EnhancedSelectorProcessor::parseSelector(const std::string& selector) {
    // 按语法文档：解析各种选择器类型
    
    SelectorInfo info;
    info.originalSelector = selector;
    
    std::string cleanedSelector = cleanSelector(selector);
    
    if (cleanedSelector.empty()) {
        addError("空的选择器");
        return info;
    }
    
    // 检查是否为索引选择器：{{button[0]}}
    auto indexPair = extractIndex(cleanedSelector);
    if (indexPair.second != -1) {
        info.selectorType = "indexed";
        info.targetElement = indexPair.first;
        info.index = indexPair.second;
        return info;
    }
    
    // 检查是否为复合选择器：{{.box button}}
    if (cleanedSelector.find(' ') != std::string::npos) {
        info.selectorType = "complex";
        info.isComplex = true;
        auto parts = parseComplexSelector(cleanedSelector);
        if (parts.size() >= 2) {
            info.className = parts[0];
            info.targetElement = parts[1];
        }
        return info;
    }
    
    // 检查选择器类型
    if (cleanedSelector[0] == '.') {
        // 类选择器：{{.box}}
        info.selectorType = "class";
        info.className = cleanedSelector.substr(1);
    } else if (cleanedSelector[0] == '#') {
        // ID选择器：{{#box}}
        info.selectorType = "id";
        info.idName = cleanedSelector.substr(1);
    } else if (isHtmlTag(cleanedSelector)) {
        // 标签选择器：{{button}}
        info.selectorType = "tag";
        info.targetElement = cleanedSelector;
    } else {
        // 混合选择器：{{box}} - 先判断tag，然后查找类名/id (id优先)
        info.selectorType = "mixed";
        info.targetElement = cleanedSelector;
    }
    
    return info;
}

std::string EnhancedSelectorProcessor::generateJavaScriptCode(const SelectorInfo& selectorInfo) {
    // 按语法文档：生成对应的JavaScript DOM对象创建代码
    
    switch (selectorInfo.selectorType[0]) {
        case 't': // tag
            return processTagSelector(selectorInfo.targetElement);
        case 'c': // class
            return processClassSelector(selectorInfo.className);
        case 'i': // id
            return processIdSelector(selectorInfo.idName);
        case 'm': // mixed
            return processMixedSelector(selectorInfo.targetElement);
        case 'C': // complex (capital C)
            return processComplexSelector(selectorInfo.className + " " + selectorInfo.targetElement);
        case 'I': // indexed (capital I)
            return processIndexedSelector(selectorInfo.targetElement, selectorInfo.index);
        default:
            addError("未知的选择器类型: " + selectorInfo.selectorType);
            return "";
    }
}

// === 选择器类型处理 ===

std::string EnhancedSelectorProcessor::processTagSelector(const std::string& tagName) {
    // 按语法文档：{{button}} → 所有的button元素
    return "document.querySelectorAll('" + tagName + "')";
}

std::string EnhancedSelectorProcessor::processClassSelector(const std::string& className) {
    // 按语法文档：{{.box}} → 查找类名为box的元素
    return "document.querySelectorAll('." + className + "')";
}

std::string EnhancedSelectorProcessor::processIdSelector(const std::string& idName) {
    // 按语法文档：{{#box}} → 查找id为box的元素
    return "document.getElementById('" + idName + "')";
}

std::string EnhancedSelectorProcessor::processMixedSelector(const std::string& name) {
    // 按语法文档：{{box}} → 先判断是否为tag，然后查找类名/id (id优先)
    
    std::string jsCode = "(function() {\n";
    jsCode += "    // 按CHTL语法文档：先判断tag，然后查找类名/id (id优先)\n";
    jsCode += "    let element = document.getElementById('" + name + "');\n";
    jsCode += "    if (element) return element;\n";
    jsCode += "    \n";
    jsCode += "    let elements = document.querySelectorAll('." + name + "');\n";
    jsCode += "    if (elements.length > 0) return elements.length === 1 ? elements[0] : elements;\n";
    jsCode += "    \n";
    
    // 检查是否为HTML标签
    if (isHtmlTag(name)) {
        jsCode += "    elements = document.querySelectorAll('" + name + "');\n";
        jsCode += "    return elements.length === 1 ? elements[0] : elements;\n";
    } else {
        jsCode += "    return null; // 未找到匹配的元素\n";
    }
    
    jsCode += "})()";
    
    return jsCode;
}

std::string EnhancedSelectorProcessor::processComplexSelector(const std::string& complexSelector) {
    // 按语法文档：{{.box button}} → 查找类名为box的元素的所有button后代
    
    auto parts = parseComplexSelector(complexSelector);
    if (parts.size() < 2) {
        addError("复合选择器格式错误: " + complexSelector);
        return "";
    }
    
    std::string parentSelector = parts[0];
    std::string childSelector = parts[1];
    
    // 构建CSS选择器
    std::string cssSelector = parentSelector + " " + childSelector;
    
    return "document.querySelectorAll('" + cssSelector + "')";
}

std::string EnhancedSelectorProcessor::processIndexedSelector(const std::string& element, int index) {
    // 按语法文档：{{button[0]}} → 第一个button元素
    
    if (index < 0) {
        addError("无效的索引: " + std::to_string(index));
        return "";
    }
    
    std::string jsCode = "(function() {\n";
    jsCode += "    // 按CHTL语法文档：精确访问索引元素\n";
    jsCode += "    let elements = document.querySelectorAll('" + element + "');\n";
    jsCode += "    return elements[" + std::to_string(index) + "] || null;\n";
    jsCode += "})()";
    
    return jsCode;
}

// === 辅助方法 ===

bool EnhancedSelectorProcessor::isValidSelector(const std::string& selector) {
    // 按语法文档：验证选择器格式
    
    if (selector.length() < 4) return false; // 至少需要 {{x}}
    
    if (selector.substr(0, 2) != "{{" || selector.substr(selector.length() - 2) != "}}") {
        return false;
    }
    
    std::string content = selector.substr(2, selector.length() - 4);
    if (content.empty()) return false;
    
    // 按语法文档：检查支持的格式
    // 简单选择器：box, .box, #box, button
    // 复合选择器：.box button
    // 索引选择器：button[0]
    
    return true;
}

bool EnhancedSelectorProcessor::isHtmlTag(const std::string& tagName) {
    return std::find(htmlTags_.begin(), htmlTags_.end(), tagName) != htmlTags_.end();
}

std::vector<std::string> EnhancedSelectorProcessor::parseComplexSelector(const std::string& selector) {
    std::vector<std::string> parts;
    std::stringstream ss(selector);
    std::string part;
    
    while (ss >> part) {
        parts.push_back(part);
    }
    
    return parts;
}

std::pair<std::string, int> EnhancedSelectorProcessor::extractIndex(const std::string& selector) {
    std::regex indexRegex(R"((\w+)\[(\d+)\])");
    std::smatch match;
    
    if (std::regex_match(selector, match, indexRegex)) {
        std::string element = match[1].str();
        int index = std::stoi(match[2].str());
        return {element, index};
    }
    
    return {selector, -1};
}

std::string EnhancedSelectorProcessor::cleanSelector(const std::string& selector) {
    if (selector.length() >= 4 && 
        selector.substr(0, 2) == "{{" && 
        selector.substr(selector.length() - 2) == "}}") {
        return selector.substr(2, selector.length() - 4);
    }
    return selector;
}

void EnhancedSelectorProcessor::initializeHtmlTags() {
    // 按语法文档：初始化HTML标签列表
    htmlTags_ = {
        "html", "head", "body", "title", "meta", "link", "script", "style",
        "div", "span", "p", "a", "img", "input", "button", "form", "label",
        "table", "tr", "td", "th", "thead", "tbody", "tfoot",
        "ul", "ol", "li", "dl", "dt", "dd",
        "h1", "h2", "h3", "h4", "h5", "h6",
        "section", "article", "header", "footer", "nav", "main", "aside",
        "figure", "figcaption", "blockquote", "pre", "code",
        "strong", "em", "small", "mark", "del", "ins", "sub", "sup",
        "iframe", "embed", "object", "param", "video", "audio", "source", "track",
        "canvas", "svg", "math",
        "details", "summary", "dialog",
        "textarea", "select", "option", "optgroup", "fieldset", "legend",
        "progress", "meter", "output"
    };
}

// === 错误处理 ===

void EnhancedSelectorProcessor::addError(const std::string& message) {
    errors_.push_back("EnhancedSelector: " + message);
}

void EnhancedSelectorProcessor::clearErrors() {
    errors_.clear();
}

bool EnhancedSelectorProcessor::hasErrors() const {
    return !errors_.empty();
}

std::vector<std::string> EnhancedSelectorProcessor::getErrors() const {
    return errors_;
}

} // namespace chtljs
} // namespace chtl