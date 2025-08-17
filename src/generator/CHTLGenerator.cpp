#include "../../include/CHTLGenerator.h"
#include <iostream>
#include <sstream>
#include <regex> // Added for regex
#include <algorithm> // Added for std::find

namespace chtl {

CHTLGenerator::CHTLGenerator(const CompilerOptions& options)
    : options_(options), debugMode_(options.debugMode), autoClassCounter_(0), autoIdCounter_(0) {
    
    if (debugMode_) {
        debugLog("CHTL生成器初始化完成");
    }
}

GeneratorResult CHTLGenerator::generate(std::shared_ptr<CHTLASTNode> ast) {
    result_ = GeneratorResult{};
    
    if (!ast) {
        result_.addError(GeneratorErrorType::AST_ERROR, "AST节点为空");
        return result_;
    }
    
    if (debugMode_) {
        debugLog("开始生成HTML/CSS/JS代码");
    }
    
    try {
        // 重置上下文
        styleContext_ = StyleContext{};
        templateContext_ = TemplateContext{};
        
        // 生成HTML
        std::ostringstream htmlStream;
        generateHTMLRecursive(ast, htmlStream);
        result_.htmlOutput = htmlStream.str();
        
        // 生成CSS
        std::ostringstream cssStream;
        generateGlobalCSS(cssStream);
        result_.cssOutput = cssStream.str();
        
        // 生成JS
        std::ostringstream jsStream;
        generateGlobalJS(jsStream);
        result_.jsOutput = jsStream.str();
        
        result_.success = true;
        
        if (debugMode_) {
            debugLog("代码生成完成");
        }
        
    } catch (const std::exception& e) {
        result_.addError(GeneratorErrorType::OUTPUT_ERROR, "生成异常: " + String(e.what()));
    }
    
    return result_;
}

void CHTLGenerator::generateHTMLRecursive(std::shared_ptr<CHTLASTNode> node, std::ostringstream& stream) {
    if (!node) return;
    
    switch (node->getType()) {
        case ASTNodeType::ROOT:
            // 根节点，处理子节点
            for (const auto& child : node->getChildren()) {
                generateHTMLRecursive(child, stream);
            }
            break;
            
        case ASTNodeType::ELEMENT:
            generateElementHTML(std::static_pointer_cast<ElementNode>(node), stream);
            break;
            
        case ASTNodeType::TEXT_NODE:
            generateTextHTML(std::static_pointer_cast<TextNode>(node), stream);
            break;
            
        case ASTNodeType::STYLE_BLOCK:
            // 局部样式块不直接输出HTML，而是处理样式
            processStyleBlock(std::static_pointer_cast<StyleBlockNode>(node));
            break;
            
        case ASTNodeType::SCRIPT_BLOCK:
            // 局部脚本块不直接输出HTML，而是处理脚本
            processScriptBlock(std::static_pointer_cast<ScriptBlockNode>(node));
            break;
            
        case ASTNodeType::TEMPLATE_STYLE:
            // 注册模板样式
            registerTemplateStyle(std::static_pointer_cast<TemplateStyleNode>(node));
            break;
            
        case ASTNodeType::TEMPLATE_ELEMENT:
            // 注册模板元素
            registerTemplateElement(std::static_pointer_cast<TemplateElementNode>(node));
            break;
            
        case ASTNodeType::TEMPLATE_VAR:
            // 注册模板变量
            registerTemplateVar(std::static_pointer_cast<TemplateVarNode>(node));
            break;
            
        case ASTNodeType::CUSTOM_STYLE:
            // 注册自定义样式
            registerCustomStyle(std::static_pointer_cast<CustomStyleNode>(node));
            break;
            
        case ASTNodeType::CUSTOM_ELEMENT:
            // 注册自定义元素
            registerCustomElement(std::static_pointer_cast<CustomElementNode>(node));
            break;
            
        case ASTNodeType::CUSTOM_VAR:
            // 注册自定义变量
            registerCustomVar(std::static_pointer_cast<CustomVarNode>(node));
            break;
            
        case ASTNodeType::UNKNOWN_NODE:
            // 处理模板使用节点（类型为UNKNOWN_NODE的特殊情况）
            if (auto usage = std::dynamic_pointer_cast<TemplateUsageNode>(node)) {
                String usageHTML = generateTemplateUsage(usage->getName(), usage->getType());
                stream << usageHTML;
            } else if (auto varRef = std::dynamic_pointer_cast<VariableReferenceNode>(node)) {
                String varValue = resolveVariableReference(varRef);
                stream << varValue;
            } else {
                // 其他UNKNOWN_NODE类型递归处理子节点
                for (const auto& child : node->getChildren()) {
                    generateHTMLRecursive(child, stream);
                }
            }
            break;
            
        default:
            // 其他节点类型递归处理子节点
            for (const auto& child : node->getChildren()) {
                generateHTMLRecursive(child, stream);
            }
            break;
    }
}

void CHTLGenerator::generateElementHTML(std::shared_ptr<ElementNode> element, std::ostringstream& stream) {
    const String& tagName = element->getTagName();
    
    stream << "<" << tagName;
    
    // 生成属性
    StringMap attributes = element->getAttributes();
    
    // 添加自动生成的class和id
    if (!styleContext_.autoClasses.empty()) {
        String classAttr;
        for (const auto& className : styleContext_.autoClasses) {
            if (!classAttr.empty()) classAttr += " ";
            classAttr += className;
        }
        if (!classAttr.empty()) {
            if (attributes.find("class") != attributes.end()) {
                attributes["class"] += " " + classAttr;
            } else {
                attributes["class"] = classAttr;
            }
        }
    }
    
    if (!styleContext_.autoIds.empty() && !styleContext_.autoIds.begin()->empty()) {
        attributes["id"] = *styleContext_.autoIds.begin();
    }
    
    // 添加内联样式
    if (!styleContext_.inlineStyles.empty()) {
        String styleAttr;
        for (const auto& [property, value] : styleContext_.inlineStyles) {
            if (!styleAttr.empty()) styleAttr += "; ";
            styleAttr += property + ": " + value;
        }
        if (!styleAttr.empty()) {
            attributes["style"] = styleAttr;
        }
    }
    
    // 输出属性
    for (const auto& [name, value] : attributes) {
        stream << " " << name << "=\"" << escapeHTMLAttribute(value) << "\"";
    }
    
    // 检查是否为自闭合标签
    if (isSelfClosingTag(tagName)) {
        stream << " />";
    } else {
        stream << ">";
        
        // 处理子节点
        for (const auto& child : element->getChildren()) {
            generateHTMLRecursive(child, stream);
        }
        
        stream << "</" << tagName << ">";
    }
    
    // 重置当前元素的样式上下文
    styleContext_.inlineStyles.clear();
    styleContext_.autoClasses.clear();
    styleContext_.autoIds.clear();
}

void CHTLGenerator::generateTextHTML(std::shared_ptr<TextNode> text, std::ostringstream& stream) {
    String content = text->getContent();
    
    // 处理无修饰字面量和引号字符串
    if (content.front() == '"' && content.back() == '"') {
        content = content.substr(1, content.length() - 2);
    } else if (content.front() == '\'' && content.back() == '\'') {
        content = content.substr(1, content.length() - 2);
    }
    // 无修饰字面量直接使用
    
    stream << escapeHTML(content);
}

void CHTLGenerator::processStyleBlock(std::shared_ptr<StyleBlockNode> styleBlock) {
    styleContext_.inLocalStyle = styleBlock->isLocal();
    
    // 重置当前选择器
    styleContext_.currentSelector.clear();
    
    if (debugMode_) {
        debugLog("处理" + String(styleContext_.inLocalStyle ? "局部" : "全局") + "样式块");
    }
    
    for (const auto& child : styleBlock->getChildren()) {
        switch (child->getType()) {
            case ASTNodeType::CSS_PROPERTY:
                // 直接CSS属性 - 内联样式
                processCSSProperty(std::static_pointer_cast<CSSPropertyNode>(child));
                break;
                
            case ASTNodeType::CSS_RULE:
                // CSS规则（包括类选择器、ID选择器、元素选择器等）
                processCSSRule(std::static_pointer_cast<CSSRuleNode>(child));
                break;
                
            default:
                // 处理其他类型的节点
                if (auto classSelector = std::dynamic_pointer_cast<ClassSelectorNode>(child)) {
                    processClassSelector(classSelector);
                } else if (auto idSelector = std::dynamic_pointer_cast<IdSelectorNode>(child)) {
                    processIdSelector(idSelector);
                } else if (auto pseudoSelector = std::dynamic_pointer_cast<PseudoSelectorNode>(child)) {
                    processPseudoSelector(pseudoSelector);
                } else if (auto usage = std::dynamic_pointer_cast<TemplateUsageNode>(child)) {
                    processTemplateUsage(usage);
                } else {
                    // 递归处理其他节点
                    for (const auto& grandChild : child->getChildren()) {
                        if (grandChild->getType() == ASTNodeType::CSS_PROPERTY) {
                            processCSSProperty(std::static_pointer_cast<CSSPropertyNode>(grandChild));
                        }
                    }
                }
                break;
        }
    }
}

void CHTLGenerator::processCSSProperty(std::shared_ptr<CSSPropertyNode> property) {
    const String& prop = property->getProperty();
    const String& value = property->getValue();
    
    if (styleContext_.inLocalStyle) {
        // 局部样式块中的直接属性作为内联样式
        if (styleContext_.currentSelector.empty()) {
            styleContext_.inlineStyles[prop] = value;
        } else {
            // 有选择器的情况，添加到相应的样式表
            addToGlobalCSS(styleContext_.currentSelector, prop, value);
        }
    } else {
        // 全局样式块
        addToGlobalCSS(styleContext_.currentSelector, prop, value);
    }
}

void CHTLGenerator::processCSSRule(std::shared_ptr<CSSRuleNode> rule) {
    String selector = rule->getSelector();
    
    // 处理自动化类名/id和上下文推导
    if (selector.front() == '.') {
        // 类选择器，自动添加类名到当前元素
        String className = selector.substr(1);
        styleContext_.autoClasses.insert(className);
        styleContext_.currentSelector = selector;
        
        if (debugMode_) {
            debugLog("自动添加类名: " + className);
        }
    } else if (selector.front() == '#') {
        // ID选择器，自动添加ID到当前元素
        String idName = selector.substr(1);
        styleContext_.autoIds.insert(idName);
        styleContext_.currentSelector = selector;
        
        if (debugMode_) {
            debugLog("自动添加ID: " + idName);
        }
    } else if (selector.front() == '&') {
        // 上下文推导
        String resolvedSelector = resolveContextSelector(selector);
        styleContext_.currentSelector = resolvedSelector;
        
        if (debugMode_) {
            debugLog("上下文推导: " + selector + " -> " + resolvedSelector);
        }
    } else {
        // 普通选择器（元素选择器等）
        styleContext_.currentSelector = selector;
    }
    
    // 处理规则内的属性
    for (const auto& child : rule->getChildren()) {
        if (child->getType() == ASTNodeType::CSS_PROPERTY) {
            processCSSProperty(std::static_pointer_cast<CSSPropertyNode>(child));
        }
    }
}

// 解析上下文选择器
String CHTLGenerator::resolveContextSelector(const String& selector) {
    if (selector == "&") {
        // 单独的&，使用当前类名或ID
        if (!styleContext_.autoClasses.empty()) {
            return "." + *styleContext_.autoClasses.begin();
        } else if (!styleContext_.autoIds.empty()) {
            return "#" + *styleContext_.autoIds.begin();
        } else {
            // 如果没有自动生成的类名或ID，生成一个
            String autoClass = generateAutoClassName();
            styleContext_.autoClasses.insert(autoClass);
            return "." + autoClass;
        }
    } else if (selector.find(":") != String::npos) {
        // 伪类选择器 &:hover, &:active 等
        String baseSelector;
        if (!styleContext_.autoClasses.empty()) {
            baseSelector = "." + *styleContext_.autoClasses.begin();
        } else if (!styleContext_.autoIds.empty()) {
            baseSelector = "#" + *styleContext_.autoIds.begin();
        } else {
            // 生成自动类名
            String autoClass = generateAutoClassName();
            styleContext_.autoClasses.insert(autoClass);
            baseSelector = "." + autoClass;
        }
        return baseSelector + selector.substr(1); // 替换&为基础选择器
    } else if (selector.find("::") != String::npos) {
        // 伪元素选择器 &::before, &::after 等
        String baseSelector;
        if (!styleContext_.autoClasses.empty()) {
            baseSelector = "." + *styleContext_.autoClasses.begin();
        } else if (!styleContext_.autoIds.empty()) {
            baseSelector = "#" + *styleContext_.autoIds.begin();
        } else {
            // 生成自动类名
            String autoClass = generateAutoClassName();
            styleContext_.autoClasses.insert(autoClass);
            baseSelector = "." + autoClass;
        }
        return baseSelector + selector.substr(1); // 替换&为基础选择器
    }
    
    return selector; // 如果无法解析，返回原选择器
}

// 处理类选择器节点
void CHTLGenerator::processClassSelector(std::shared_ptr<ClassSelectorNode> classSelector) {
    const String& className = classSelector->getClassName();
    String selector = "." + className;
    
    // 自动添加类名到当前元素
    styleContext_.autoClasses.insert(className);
    styleContext_.currentSelector = selector;
    
    if (debugMode_) {
        debugLog("处理类选择器: " + selector);
    }
    
    // 处理选择器内的属性
    for (const auto& child : classSelector->getChildren()) {
        if (child->getType() == ASTNodeType::CSS_PROPERTY) {
            processCSSProperty(std::static_pointer_cast<CSSPropertyNode>(child));
        }
    }
}

// 处理ID选择器节点
void CHTLGenerator::processIdSelector(std::shared_ptr<IdSelectorNode> idSelector) {
    const String& idName = idSelector->getIdName();
    String selector = "#" + idName;
    
    // 自动添加ID到当前元素
    styleContext_.autoIds.insert(idName);
    styleContext_.currentSelector = selector;
    
    if (debugMode_) {
        debugLog("处理ID选择器: " + selector);
    }
    
    // 处理选择器内的属性
    for (const auto& child : idSelector->getChildren()) {
        if (child->getType() == ASTNodeType::CSS_PROPERTY) {
            processCSSProperty(std::static_pointer_cast<CSSPropertyNode>(child));
        }
    }
}

// 处理伪选择器节点
void CHTLGenerator::processPseudoSelector(std::shared_ptr<PseudoSelectorNode> pseudoSelector) {
    const String& pseudoSel = pseudoSelector->getSelector();
    String resolvedSelector = resolveContextSelector(pseudoSel);
    styleContext_.currentSelector = resolvedSelector;
    
    if (debugMode_) {
        debugLog("处理伪选择器: " + pseudoSel + " -> " + resolvedSelector);
    }
    
    // 处理选择器内的属性
    for (const auto& child : pseudoSelector->getChildren()) {
        if (child->getType() == ASTNodeType::CSS_PROPERTY) {
            processCSSProperty(std::static_pointer_cast<CSSPropertyNode>(child));
        }
    }
}

void CHTLGenerator::processScriptBlock(std::shared_ptr<ScriptBlockNode> scriptBlock) {
    // 局部脚本块处理
    if (scriptBlock->isLocal()) {
        // 添加到全局脚本中，但使用高优先级且不污染全局的方式
        for (const auto& child : scriptBlock->getChildren()) {
            // 处理CHTL JS语法
            String jsCode = generateCHTLJSCode(child);
            globalJS_ += jsCode + "\n";
        }
    }
}

void CHTLGenerator::addToGlobalCSS(const String& selector, const String& property, const String& value) {
    globalCSS_[selector][property] = value;
}

void CHTLGenerator::generateGlobalCSS(std::ostringstream& stream) {
    for (const auto& [selector, properties] : globalCSS_) {
        stream << selector << " {\n";
        for (const auto& [property, value] : properties) {
            stream << "  " << property << ": " << value << ";\n";
        }
        stream << "}\n\n";
    }
}

void CHTLGenerator::generateGlobalJS(std::ostringstream& stream) {
    if (!globalJS_.empty()) {
        stream << "(function() {\n";
        stream << "  'use strict';\n";
        stream << globalJS_;
        stream << "})();\n";
    }
}

String CHTLGenerator::generateCHTLJSCode(std::shared_ptr<CHTLASTNode> node) {
    // 基础CHTL JS代码生成
    // 这里先返回占位符，后续任务会完整实现
    return "// CHTL JS Code";
}

String CHTLGenerator::escapeHTML(const String& text) {
    String result = text;
    size_t pos = 0;
    
    while ((pos = result.find('&', pos)) != String::npos) {
        result.replace(pos, 1, "&amp;");
        pos += 5;
    }
    
    pos = 0;
    while ((pos = result.find('<', pos)) != String::npos) {
        result.replace(pos, 1, "&lt;");
        pos += 4;
    }
    
    pos = 0;
    while ((pos = result.find('>', pos)) != String::npos) {
        result.replace(pos, 1, "&gt;");
        pos += 4;
    }
    
    return result;
}

String CHTLGenerator::escapeHTMLAttribute(const String& text) {
    String result = escapeHTML(text);
    size_t pos = 0;
    
    while ((pos = result.find('"', pos)) != String::npos) {
        result.replace(pos, 1, "&quot;");
        pos += 6;
    }
    
    return result;
}

bool CHTLGenerator::isSelfClosingTag(const String& tagName) const {
    static const StringSet selfClosingTags = {
        "area", "base", "br", "col", "embed", "hr", "img", "input",
        "link", "meta", "param", "source", "track", "wbr"
    };
    
    return selfClosingTags.find(tagName) != selfClosingTags.end();
}

GeneratorResult CHTLGenerator::generateFromFile(const String& filename) {
    // 占位符实现
    result_.addError(GeneratorErrorType::OUTPUT_ERROR, "generateFromFile未实现");
    return result_;
}

String CHTLGenerator::generateHTML(std::shared_ptr<CHTLASTNode> ast) {
    std::ostringstream stream;
    generateHTMLRecursive(ast, stream);
    return stream.str();
}

String CHTLGenerator::generateCSS(std::shared_ptr<CHTLASTNode> ast) {
    std::ostringstream stream;
    generateGlobalCSS(stream);
    return stream.str();
}

String CHTLGenerator::generateJS(std::shared_ptr<CHTLASTNode> ast) {
    std::ostringstream stream;
    generateGlobalJS(stream);
    return stream.str();
}

// 占位符方法实现
String CHTLGenerator::generateElement(std::shared_ptr<ElementNode> element) { 
    std::ostringstream stream;
    generateElementHTML(element, stream);
    return stream.str();
}

String CHTLGenerator::generateText(std::shared_ptr<TextNode> text) { 
    std::ostringstream stream;
    generateTextHTML(text, stream);
    return stream.str();
}

String CHTLGenerator::generateAttribute(std::shared_ptr<AttributeNode> attribute) { 
    return attribute->getName() + "=\"" + escapeHTMLAttribute(attribute->getValue()) + "\"";
}

String CHTLGenerator::generateStyle(std::shared_ptr<StyleBlockNode> style) { 
    processStyleBlock(style);
    return "";
}

String CHTLGenerator::generateScript(std::shared_ptr<ScriptBlockNode> script) { 
    processScriptBlock(script);
    return "";
}

String CHTLGenerator::generateStyleProperty(std::shared_ptr<CSSPropertyNode> property) { return ""; }
String CHTLGenerator::generateClassSelector(const String& className) { return ""; }
String CHTLGenerator::generateIdSelector(const String& idName) { return ""; }
String CHTLGenerator::generatePseudoSelector(const String& pseudoSelector) { return ""; }
String CHTLGenerator::generateInlineStyles(const StringMap& styles) { return ""; }
String CHTLGenerator::generateTemplateStyle(std::shared_ptr<TemplateStyleNode> templateStyle) {
    if (!templateStyle) return "";
    
    std::ostringstream result;
    const String& templateName = templateStyle->getName();
    
    // 处理继承
    for (const String& inheritedName : templateStyle->getInheritedTemplates()) {
        auto inheritedTemplate = templateContext_.styleTemplates.find(inheritedName);
        if (inheritedTemplate != templateContext_.styleTemplates.end()) {
            String inheritedCSS = generateTemplateStyle(inheritedTemplate->second);
            result << inheritedCSS;
        }
    }
    
    // 处理当前模板的样式属性
    for (const auto& child : templateStyle->getChildren()) {
        if (child->getType() == ASTNodeType::CSS_PROPERTY) {
            auto property = std::static_pointer_cast<CSSPropertyNode>(child);
            String expandedValue = expandTemplateVariables(property->getValue(), templateName);
            result << "  " << property->getProperty() << ": " << expandedValue << ";\n";
        }
    }
    
    return result.str();
}
String CHTLGenerator::generateTemplateElement(std::shared_ptr<TemplateElementNode> templateElement) {
    if (!templateElement) return "";
    
    std::ostringstream result;
    const String& templateName = templateElement->getName();
    
    // 处理继承
    for (const String& inheritedName : templateElement->getInheritedTemplates()) {
        auto inheritedTemplate = templateContext_.elementTemplates.find(inheritedName);
        if (inheritedTemplate != templateContext_.elementTemplates.end()) {
            String inheritedHTML = generateTemplateElement(inheritedTemplate->second);
            result << inheritedHTML;
        }
    }
    
    // 处理当前模板的元素
    for (const auto& child : templateElement->getChildren()) {
        generateHTMLRecursive(child, result);
    }
    
    return result.str();
}
String CHTLGenerator::generateTemplateVar(std::shared_ptr<TemplateVarNode> templateVar) {
    // 变量模板不直接生成内容，而是提供变量值
    return "";
}
String CHTLGenerator::generateTemplateUsage(const String& templateName, const String& templateType) {
    if (templateType == "@Style") {
        auto it = templateContext_.styleTemplates.find(templateName);
        if (it != templateContext_.styleTemplates.end()) {
            return generateTemplateStyle(it->second);
        }
    } else if (templateType == "@Element") {
        auto it = templateContext_.elementTemplates.find(templateName);
        if (it != templateContext_.elementTemplates.end()) {
            return generateTemplateElement(it->second);
        }
    } else if (templateType == "@Var") {
        auto it = templateContext_.varTemplates.find(templateName);
        if (it != templateContext_.varTemplates.end()) {
            return generateTemplateVar(it->second);
        }
    }
    
    if (debugMode_) {
        debugLog("警告: 未找到模板 " + templateType + " " + templateName);
    }
    
    return "";
}
String CHTLGenerator::resolveVariable(const String& varName, const String& templateName) {
    // 首先尝试在指定模板中查找
    if (!templateName.empty()) {
        String fullName = templateName + "." + varName;
        auto it = templateContext_.variables.find(fullName);
        if (it != templateContext_.variables.end()) {
            return it->second;
        }
    }
    
    // 在全局变量中查找
    auto it = templateContext_.variables.find(varName);
    if (it != templateContext_.variables.end()) {
        return it->second;
    }
    
    return varName; // 如果找不到，返回原始名称
}
String CHTLGenerator::resolveVariableGroup(const String& groupName, const String& varName) {
    String fullName = groupName + "." + varName;
    auto it = templateContext_.variables.find(fullName);
    if (it != templateContext_.variables.end()) {
        return it->second;
    }
    
    if (debugMode_) {
        debugLog("警告: 未找到变量组变量 " + fullName);
    }
    
    return groupName + "(" + varName + ")"; // 返回原始形式
}
String CHTLGenerator::expandTemplateVariables(const String& text, const String& templateName) const {
    String result = text;
    
    // 处理变量组引用: ThemeColor(tableColor)
    std::regex varGroupRegex(R"((\w+)\((\w+)\))");
    std::smatch match;
    
    while (std::regex_search(result, match, varGroupRegex)) {
        String groupName = match[1].str();
        String varName = match[2].str();
        String fullVarName = groupName + "." + varName;
        
        // 查找变量值
        auto it = templateContext_.variables.find(fullVarName);
        if (it != templateContext_.variables.end()) {
            result = std::regex_replace(result, varGroupRegex, it->second, std::regex_constants::format_first_only);
        } else {
            // 如果找不到，保持原样
            if (debugMode_) {
                debugLog("警告: 未找到变量组变量 " + fullVarName);
            }
            break; // 避免无限循环
        }
    }
    
    // 处理简单变量引用: ${variableName}
    std::regex simpleVarRegex(R"(\$\{(\w+)\})");
    while (std::regex_search(result, match, simpleVarRegex)) {
        String varName = match[1].str();
        String fullVarName = templateName.empty() ? varName : templateName + "." + varName;
        
        // 首先尝试模板作用域的变量
        auto it = templateContext_.variables.find(fullVarName);
        if (it != templateContext_.variables.end()) {
            result = std::regex_replace(result, simpleVarRegex, it->second, std::regex_constants::format_first_only);
        } else {
            // 然后尝试全局变量
            it = templateContext_.variables.find(varName);
            if (it != templateContext_.variables.end()) {
                result = std::regex_replace(result, simpleVarRegex, it->second, std::regex_constants::format_first_only);
            } else {
                if (debugMode_) {
                    debugLog("警告: 未找到变量 " + varName);
                }
                break; // 避免无限循环
            }
        }
    }
    
    return result;
}

String CHTLGenerator::resolveVariableReference(std::shared_ptr<VariableReferenceNode> varRef) {
    if (!varRef) return "";
    
    // 如果是变量组引用
    if (!varRef->getGroupName().empty() && !varRef->getVariableName().empty()) {
        return resolveVariableGroup(varRef->getGroupName(), varRef->getVariableName());
    }
    
    // 普通变量引用
    return resolveVariable(varRef->getName(), "");
}

void CHTLGenerator::processTemplateUsage(std::shared_ptr<TemplateUsageNode> usage) {
    if (!usage) return;
    
    const String& templateType = usage->getType();
    const String& templateName = usage->getName();
    
    if (templateType == "@Style") {
        // 样式模板或自定义样式使用
        bool foundTemplate = false;
        
        // 首先尝试从模板中查找
        auto templateIt = templateContext_.styleTemplates.find(templateName);
        if (templateIt != templateContext_.styleTemplates.end()) {
            foundTemplate = true;
            processStyleTemplateUsage(templateIt->second, usage);
        } else {
            // 然后尝试从自定义中查找
            auto customIt = templateContext_.customStyles.find(templateName);
            if (customIt != templateContext_.customStyles.end()) {
                foundTemplate = true;
                processCustomStyleUsage(customIt->second, usage);
            }
        }
        
        if (!foundTemplate && debugMode_) {
            debugLog("警告: 未找到样式模板或自定义: " + templateName);
        }
    } else if (templateType == "@Element") {
        // 元素模板或自定义元素使用
        bool foundTemplate = false;
        
        auto templateIt = templateContext_.elementTemplates.find(templateName);
        if (templateIt != templateContext_.elementTemplates.end()) {
            foundTemplate = true;
            if (debugMode_) {
                debugLog("处理元素模板使用: " + templateName);
            }
        } else {
            auto customIt = templateContext_.customElements.find(templateName);
            if (customIt != templateContext_.customElements.end()) {
                foundTemplate = true;
                if (debugMode_) {
                    debugLog("处理自定义元素使用: " + templateName);
                }
            }
        }
        
        if (!foundTemplate && debugMode_) {
            debugLog("警告: 未找到元素模板或自定义: " + templateName);
        }
    } else if (templateType == "@Var") {
        // 变量模板或自定义变量使用
        if (debugMode_) {
            debugLog("处理变量模板使用: " + templateName);
        }
    }
}

void CHTLGenerator::processStyleTemplateUsage(std::shared_ptr<TemplateStyleNode> templateStyle, std::shared_ptr<TemplateUsageNode> usage) {
    // 展开样式模板的属性
    for (const auto& child : templateStyle->getChildren()) {
        if (child->getType() == ASTNodeType::CSS_PROPERTY) {
            auto property = std::static_pointer_cast<CSSPropertyNode>(child);
            String expandedValue = expandTemplateVariables(property->getValue(), templateStyle->getName());
            
            if (styleContext_.inLocalStyle && styleContext_.currentSelector.empty()) {
                // 作为内联样式添加
                styleContext_.inlineStyles[property->getProperty()] = expandedValue;
            } else {
                // 添加到全局样式
                addToGlobalCSS(styleContext_.currentSelector.empty() ? "body" : styleContext_.currentSelector, 
                             property->getProperty(), expandedValue);
            }
        }
    }
    
    // 处理特例化操作
    for (const auto& child : usage->getChildren()) {
        if (auto specialization = std::dynamic_pointer_cast<SpecializationNode>(child)) {
            String specializationCSS = processStyleSpecialization(specialization);
            // 将特例化的CSS添加到当前上下文
        }
    }
}

void CHTLGenerator::processCustomStyleUsage(std::shared_ptr<CustomStyleNode> customStyle, std::shared_ptr<TemplateUsageNode> usage) {
    // 处理自定义样式的使用，包括无值样式组的特例化
    for (const auto& child : customStyle->getChildren()) {
        if (child->getType() == ASTNodeType::CSS_PROPERTY) {
            auto property = std::static_pointer_cast<CSSPropertyNode>(child);
            String propName = property->getProperty();
            String propValue = property->getValue();
            
            if (propValue.empty()) {
                // 无值属性，需要从使用时的特例化中获取值
                String specializationValue = getSpecializationValue(usage, propName);
                if (!specializationValue.empty()) {
                    if (styleContext_.inLocalStyle && styleContext_.currentSelector.empty()) {
                        styleContext_.inlineStyles[propName] = specializationValue;
                    } else {
                        addToGlobalCSS(styleContext_.currentSelector.empty() ? "body" : styleContext_.currentSelector, 
                                     propName, specializationValue);
                    }
                }
            } else {
                // 有值属性，直接使用
                String expandedValue = expandTemplateVariables(propValue, customStyle->getName());
                if (styleContext_.inLocalStyle && styleContext_.currentSelector.empty()) {
                    styleContext_.inlineStyles[propName] = expandedValue;
                } else {
                    addToGlobalCSS(styleContext_.currentSelector.empty() ? "body" : styleContext_.currentSelector, 
                                 propName, expandedValue);
                }
            }
        }
    }
}

String CHTLGenerator::getSpecializationValue(std::shared_ptr<TemplateUsageNode> usage, const String& propertyName) {
    // 从模板使用的特例化块中查找属性值
    for (const auto& child : usage->getChildren()) {
        if (child->getType() == ASTNodeType::CSS_PROPERTY) {
            auto property = std::static_pointer_cast<CSSPropertyNode>(child);
            if (property->getProperty() == propertyName) {
                return property->getValue();
            }
        }
    }
    return "";
}

void CHTLGenerator::registerTemplateStyle(std::shared_ptr<TemplateStyleNode> templateStyle) {
    const String& name = templateStyle->getName();
    templateContext_.styleTemplates[name] = templateStyle;
    
    if (debugMode_) {
        debugLog("注册样式模板: " + name);
    }
}

void CHTLGenerator::registerTemplateElement(std::shared_ptr<TemplateElementNode> templateElement) {
    const String& name = templateElement->getName();
    templateContext_.elementTemplates[name] = templateElement;
    
    if (debugMode_) {
        debugLog("注册元素模板: " + name);
    }
}

void CHTLGenerator::registerTemplateVar(std::shared_ptr<TemplateVarNode> templateVar) {
    const String& name = templateVar->getName();
    templateContext_.varTemplates[name] = templateVar;
    
    // 将变量添加到全局变量映射
    const auto& variables = templateVar->getVariables();
    for (const auto& [varName, varValue] : variables) {
        String fullName = name + "." + varName;
        templateContext_.variables[fullName] = varValue;
    }
    
    if (debugMode_) {
        debugLog("注册变量模板: " + name + " (包含 " + std::to_string(variables.size()) + " 个变量)");
    }
}
void CHTLGenerator::registerCustomStyle(std::shared_ptr<CustomStyleNode> customStyle) {
    const String& name = customStyle->getName();
    templateContext_.customStyles[name] = customStyle;
    
    if (debugMode_) {
        debugLog("注册自定义样式: " + name);
    }
}

void CHTLGenerator::registerCustomElement(std::shared_ptr<CustomElementNode> customElement) {
    const String& name = customElement->getName();
    templateContext_.customElements[name] = customElement;
    
    if (debugMode_) {
        debugLog("注册自定义元素: " + name);
    }
}

void CHTLGenerator::registerCustomVar(std::shared_ptr<CustomVarNode> customVar) {
    const String& name = customVar->getName();
    templateContext_.customVars[name] = customVar;
    
    // 将变量添加到全局变量映射
    const auto& variables = customVar->getVariables();
    for (const auto& [varName, varValue] : variables) {
        String fullName = name + "." + varName;
        templateContext_.variables[fullName] = varValue;
    }
    
    if (debugMode_) {
        debugLog("注册自定义变量: " + name + " (包含 " + std::to_string(variables.size()) + " 个变量)");
    }
}

String CHTLGenerator::generateCustomStyle(std::shared_ptr<CustomStyleNode> customStyle) {
    if (!customStyle) return "";
    
    std::ostringstream result;
    const String& customName = customStyle->getName();
    
    // 处理继承
    for (const String& inheritedName : customStyle->getInheritedTemplates()) {
        // 首先尝试从模板中查找
        auto templateIt = templateContext_.styleTemplates.find(inheritedName);
        if (templateIt != templateContext_.styleTemplates.end()) {
            String inheritedCSS = generateTemplateStyle(templateIt->second);
            result << inheritedCSS;
        } else {
            // 然后尝试从自定义中查找
            auto customIt = templateContext_.customStyles.find(inheritedName);
            if (customIt != templateContext_.customStyles.end()) {
                String inheritedCSS = generateCustomStyle(customIt->second);
                result << inheritedCSS;
            }
        }
    }
    
    // 处理当前自定义的样式属性
    for (const auto& child : customStyle->getChildren()) {
        if (child->getType() == ASTNodeType::CSS_PROPERTY) {
            auto property = std::static_pointer_cast<CSSPropertyNode>(child);
            String propName = property->getProperty();
            String propValue = property->getValue();
            
            // 检查是否为无值样式组属性
            if (propValue.empty()) {
                // 无值属性，需要在使用时填入值
                if (debugMode_) {
                    debugLog("发现无值样式属性: " + propName);
                }
            } else {
                String expandedValue = expandTemplateVariables(propValue, customName);
                result << "  " << propName << ": " << expandedValue << ";\n";
            }
        } else if (auto specialization = std::dynamic_pointer_cast<SpecializationNode>(child)) {
            // 处理特例化操作
            String specializationCSS = processStyleSpecialization(specialization);
            result << specializationCSS;
        }
    }
    
    return result.str();
}

String CHTLGenerator::generateCustomElement(std::shared_ptr<CustomElementNode> customElement) {
    if (!customElement) return "";
    
    std::ostringstream result;
    const String& customName = customElement->getName();
    
    // 处理继承
    for (const String& inheritedName : customElement->getInheritedTemplates()) {
        // 首先尝试从模板中查找
        auto templateIt = templateContext_.elementTemplates.find(inheritedName);
        if (templateIt != templateContext_.elementTemplates.end()) {
            String inheritedHTML = generateTemplateElement(templateIt->second);
            result << inheritedHTML;
        } else {
            // 然后尝试从自定义中查找
            auto customIt = templateContext_.customElements.find(inheritedName);
            if (customIt != templateContext_.customElements.end()) {
                String inheritedHTML = generateCustomElement(customIt->second);
                result << inheritedHTML;
            }
        }
    }
    
    // 处理当前自定义的元素
    for (const auto& child : customElement->getChildren()) {
        if (auto insertNode = std::dynamic_pointer_cast<InsertNode>(child)) {
            // 处理插入操作
            String insertHTML = processInsertOperation(insertNode);
            result << insertHTML;
        } else {
            // 普通元素
            generateHTMLRecursive(child, result);
        }
    }
    
    return result.str();
}

String CHTLGenerator::generateCustomVar(std::shared_ptr<CustomVarNode> customVar) {
    // 自定义变量不直接生成内容，而是提供变量值
    return "";
}

String CHTLGenerator::processStyleSpecialization(std::shared_ptr<SpecializationNode> specialization) {
    if (!specialization) return "";
    
    std::ostringstream result;
    const String& specType = specialization->getType();
    const String& specName = specialization->getName();
    
    // 处理删除的属性
    const auto& deletedProps = specialization->getDeletedProperties();
    if (!deletedProps.empty() && debugMode_) {
        debugLog("特例化删除属性: " + specName);
        for (const auto& prop : deletedProps) {
            debugLog("  删除属性: " + prop);
        }
    }
    
    // 处理删除的继承
    const auto& deletedInheritances = specialization->getDeletedInheritances();
    if (!deletedInheritances.empty() && debugMode_) {
        debugLog("特例化删除继承: " + specName);
        for (const auto& inheritance : deletedInheritances) {
            debugLog("  删除继承: " + inheritance);
        }
    }
    
    // 处理特例化的属性
    for (const auto& child : specialization->getChildren()) {
        if (child->getType() == ASTNodeType::CSS_PROPERTY) {
            auto property = std::static_pointer_cast<CSSPropertyNode>(child);
            String propName = property->getProperty();
            String propValue = property->getValue();
            
            // 检查是否为被删除的属性
            bool isDeleted = std::find(deletedProps.begin(), deletedProps.end(), propName) != deletedProps.end();
            if (!isDeleted) {
                String expandedValue = expandTemplateVariables(propValue, specName);
                result << "  " << propName << ": " << expandedValue << ";\n";
            }
        }
    }
    
    return result.str();
}

String CHTLGenerator::processInsertOperation(std::shared_ptr<InsertNode> insertNode) {
    if (!insertNode) return "";
    
    std::ostringstream result;
    
    // 生成插入的内容
    for (const auto& child : insertNode->getChildren()) {
        generateHTMLRecursive(child, result);
    }
    
    // 这里简化处理，实际应该根据位置和选择器进行DOM操作
    if (debugMode_) {
        debugLog("处理插入操作: " + insertNode->getSelector());
    }
    
    return result.str();
}
String CHTLGenerator::generateImport(std::shared_ptr<ImportNode> import) { return ""; }
String CHTLGenerator::generateNamespace(std::shared_ptr<NamespaceNode> namespaceNode) { return ""; }
String CHTLGenerator::generateConfiguration(const StringMap& configMap) { return ""; }
String CHTLGenerator::generateOrigin(std::shared_ptr<OriginNode> origin) { return ""; }

void CHTLGenerator::addInlineStyle(const String& property, const String& value) {}
void CHTLGenerator::addClassStyle(const String& className, const String& property, const String& value) {}
void CHTLGenerator::addIdStyle(const String& idName, const String& property, const String& value) {}
void CHTLGenerator::addPseudoStyle(const String& selector, const String& property, const String& value) {}

String CHTLGenerator::generateAutoClassName() {
    return "chtl-auto-class-" + std::to_string(autoClassCounter_++);
}

String CHTLGenerator::generateAutoIdName() {
    return "chtl-auto-id-" + std::to_string(autoIdCounter_++);
}

void CHTLGenerator::registerTemplate(const String& type, const String& name, std::shared_ptr<CHTLASTNode> node) {}
void CHTLGenerator::registerCustom(const String& type, const String& name, std::shared_ptr<CHTLASTNode> node) {}
void CHTLGenerator::registerVariable(const String& name, const String& value, const String& scope) {}

void CHTLGenerator::enterNamespace(const String& namespaceName) {
    namespaceStack_.push(namespaceName);
    currentNamespace_ = namespaceName;
}

void CHTLGenerator::exitNamespace() {
    if (!namespaceStack_.empty()) {
        namespaceStack_.pop();
    }
    currentNamespace_ = namespaceStack_.empty() ? "" : namespaceStack_.top();
}

String CHTLGenerator::getCurrentNamespace() const {
    return currentNamespace_;
}

String CHTLGenerator::resolveNamespacedName(const String& name) const {
    if (currentNamespace_.empty()) {
        return name;
    }
    return currentNamespace_ + "::" + name;
}

void CHTLGenerator::reportError(GeneratorErrorType type, const String& message, const String& context) {
    result_.addError(type, message, context);
    
    if (debugMode_) {
        std::cerr << "生成器错误: " << message << std::endl;
    }
}

void CHTLGenerator::reportWarning(const String& message) {
    result_.addWarning(message);
    
    if (debugMode_) {
        std::cerr << "生成器警告: " << message << std::endl;
    }
}

void CHTLGenerator::debugLog(const String& message) const {
    if (debugMode_) {
        std::cout << "[CHTL Generator] " << message << std::endl;
    }
}

// 工厂方法实现
std::unique_ptr<CHTLGenerator> CHTLGeneratorFactory::createGenerator(const CompilerOptions& options) {
    return std::make_unique<CHTLGenerator>(options);
}

std::unique_ptr<CHTLGenerator> CHTLGeneratorFactory::createDebugGenerator() {
    CompilerOptions debugOptions;
    debugOptions.debugMode = true;
    return std::make_unique<CHTLGenerator>(debugOptions);
}

std::unique_ptr<CHTLGenerator> CHTLGeneratorFactory::createOptimizedGenerator() {
    CompilerOptions optimizedOptions;
    optimizedOptions.debugMode = false;
    return std::make_unique<CHTLGenerator>(optimizedOptions);
}

} // namespace chtl