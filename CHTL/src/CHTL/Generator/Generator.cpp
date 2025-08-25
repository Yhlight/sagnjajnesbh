#include "CHTL/Generator/Generator.h"
#include <sstream>
#include <algorithm>
#include <unordered_set>
#include <cctype>

namespace CHTL {
namespace Compiler {

class Generator::Impl {
public:
    CompileContext* context;
    GeneratorConfig config;
    
    std::stringstream htmlOutput;
    std::stringstream cssOutput;
    std::stringstream jsOutput;
    
    int currentIndent = 0;
    std::unordered_set<std::string> generatedClasses;
    std::unordered_set<std::string> generatedIds;
    
    Impl(CompileContext* ctx, const GeneratorConfig& cfg) 
        : context(ctx), config(cfg) {}
    
    std::string Generate(std::shared_ptr<ASTNode> ast) {
        if (!ast) return "";
        
        // 清空输出
        htmlOutput.str("");
        htmlOutput.clear();
        cssOutput.str("");
        cssOutput.clear();
        jsOutput.str("");
        jsOutput.clear();
        
        // 生成HTML文档
        GenerateNode(ast);
        
        return htmlOutput.str();
    }
    
private:
    void GenerateNode(std::shared_ptr<ASTNode> node) {
        if (!node) return;
        
        switch (node->type) {
            case ASTNodeType::ROOT:
                GenerateRoot(node);
                break;
                
            case ASTNodeType::HTML_ELEMENT:
                GenerateHTMLElement(node);
                break;
                
            case ASTNodeType::TEXT_NODE:
                GenerateTextNode(node);
                break;
                
            case ASTNodeType::STYLE_BLOCK:
                GenerateStyleBlock(node);
                break;
                
            case ASTNodeType::SCRIPT_BLOCK:
                GenerateScriptBlock(node);
                break;
                
            case ASTNodeType::ATTRIBUTE_STATEMENT:
                // 属性在元素生成时处理
                break;
                
            case ASTNodeType::GENERATOR_COMMENT:
                GenerateComment(node);
                break;
                
            case ASTNodeType::TEMPLATE_DECLARATION:
            case ASTNodeType::CUSTOM_DECLARATION:
                // 声明不直接生成代码
                break;
                
            case ASTNodeType::STYLE_USE:
                // 样式使用在样式块处理中展开
                break;
                
            case ASTNodeType::ELEMENT_USE:
                GenerateElementUse(node);
                break;
                
            default:
                // 其他节点类型暂不处理
                break;
        }
    }
    
    void GenerateRoot(std::shared_ptr<ASTNode> node) {
        // 处理所有子节点
        for (const auto& child : node->children) {
            GenerateNode(child);
        }
    }
    
    void GenerateHTMLElement(std::shared_ptr<ASTNode> node) {
        auto* htmlNode = dynamic_cast<HTMLElementNode*>(node.get());
        std::string tagName = htmlNode ? htmlNode->tagName : node->value;
        
        // 收集属性
        std::unordered_map<std::string, std::string> attributes;
        std::vector<std::string> classes;
        
        // 处理属性和样式
        for (const auto& child : node->children) {
            if (child->type == ASTNodeType::ATTRIBUTE_STATEMENT) {
                std::string name = child->GetAttribute("name");
                std::string value = child->value;
                
                if (name == "class") {
                    // 收集类名
                    classes.push_back(value);
                } else {
                    attributes[name] = value;
                }
            } else if (child->type == ASTNodeType::STYLE_BLOCK) {
                // 处理局部样式块，可能需要生成自动类名
                ProcessLocalStyleBlock(child, tagName, attributes, classes);
            }
        }
        
        // 输出开始标签
        Indent();
        htmlOutput << "<" << tagName;
        
        // 输出属性
        for (const auto& [name, value] : attributes) {
            htmlOutput << " " << name << "=\"" << EscapeHTML(value) << "\"";
        }
        
        // 输出类名
        if (!classes.empty()) {
            htmlOutput << " class=\"";
            for (size_t i = 0; i < classes.size(); ++i) {
                if (i > 0) htmlOutput << " ";
                htmlOutput << classes[i];
            }
            htmlOutput << "\"";
        }
        
        // 自闭合标签
        if (htmlNode && htmlNode->isSelfClosing) {
            htmlOutput << " />";
            if (config.prettyPrint) htmlOutput << "\n";
            return;
        }
        
        htmlOutput << ">";
        
        // 检查是否有非属性子节点
        bool hasContent = false;
        for (const auto& child : node->children) {
            if (child->type != ASTNodeType::ATTRIBUTE_STATEMENT &&
                child->type != ASTNodeType::STYLE_BLOCK &&
                child->type != ASTNodeType::SCRIPT_BLOCK) {
                hasContent = true;
                break;
            }
        }
        
        if (hasContent && config.prettyPrint) {
            htmlOutput << "\n";
            currentIndent++;
        }
        
        // 生成子节点
        for (const auto& child : node->children) {
            if (child->type != ASTNodeType::ATTRIBUTE_STATEMENT) {
                GenerateNode(child);
            }
        }
        
        if (hasContent && config.prettyPrint) {
            currentIndent--;
            Indent();
        }
        
        // 输出结束标签
        htmlOutput << "</" << tagName << ">";
        if (config.prettyPrint) htmlOutput << "\n";
    }
    
    void GenerateTextNode(std::shared_ptr<ASTNode> node) {
        if (!node->value.empty()) {
            Indent();
            htmlOutput << EscapeHTML(node->value);
            if (config.prettyPrint) htmlOutput << "\n";
        }
    }
    
    void GenerateStyleBlock(std::shared_ptr<ASTNode> node) {
        // 局部样式块不直接输出到HTML，而是收集到CSS中
        // 这在ProcessLocalStyleBlock中处理
    }
    
    void GenerateScriptBlock(std::shared_ptr<ASTNode> node) {
        // 收集脚本内容到全局JS
        if (!node->value.empty()) {
            jsOutput << "// 局部脚本块 (" << node->line << ":" << node->column << ")\n";
            jsOutput << "(function() {\n";
            jsOutput << node->value;
            jsOutput << "\n})();\n\n";
        }
    }
    
    void GenerateComment(std::shared_ptr<ASTNode> node) {
        if (config.generateComments) {
            Indent();
            htmlOutput << "<!-- " << node->value.substr(2) << " -->";
            if (config.prettyPrint) htmlOutput << "\n";
        }
    }
    
    void ProcessLocalStyleBlock(std::shared_ptr<ASTNode> styleBlock,
                                const std::string& elementTag,
                                std::unordered_map<std::string, std::string>& attributes,
                                std::vector<std::string>& classes) {
        // 处理局部样式块中的内容
        std::stringstream inlineStyles;
        
        for (const auto& child : styleBlock->children) {
            switch (child->type) {
                case ASTNodeType::STYLE_PROPERTY:
                    // 内联样式
                                    if (auto* prop = dynamic_cast<StylePropertyNode*>(child.get())) {
                    if (!inlineStyles.str().empty()) inlineStyles << " ";
                    std::string resolvedValue = ResolveVariables(prop->propertyValue);
                    inlineStyles << prop->property << ": " << resolvedValue << ";";
                    }
                    break;
                    
                case ASTNodeType::CLASS_SELECTOR:
                    // 自动添加类名并生成CSS
                    {
                        std::string className = child->value;
                        classes.push_back(className);
                        GenerateClassCSS(className, child);
                    }
                    break;
                    
                case ASTNodeType::ID_SELECTOR:
                    // 自动添加ID并生成CSS
                    {
                        std::string idName = child->value;
                        attributes["id"] = idName;
                        GenerateIdCSS(idName, child);
                    }
                    break;
                    
                case ASTNodeType::AMPERSAND_SELECTOR:
                    // & 选择器需要上下文推导
                    ProcessAmpersandSelector(child, elementTag, attributes, classes);
                    break;
                    
                case ASTNodeType::STYLE_USE:
                    // 展开样式使用
                    ProcessStyleUse(child, inlineStyles);
                    break;
                    
                default:
                    break;
            }
        }
        
        // 如果有内联样式，添加到属性中
        if (!inlineStyles.str().empty()) {
            if (attributes.find("style") != attributes.end()) {
                attributes["style"] += " " + inlineStyles.str();
            } else {
                attributes["style"] = inlineStyles.str();
            }
        }
    }
    
    void GenerateClassCSS(const std::string& className, std::shared_ptr<ASTNode> node) {
        if (generatedClasses.find(className) != generatedClasses.end()) {
            return; // 已生成
        }
        
        generatedClasses.insert(className);
        
        cssOutput << "." << className << " {\n";
        
        for (const auto& child : node->children) {
            if (child->type == ASTNodeType::STYLE_PROPERTY) {
                if (auto* prop = dynamic_cast<StylePropertyNode*>(child.get())) {
                    std::string resolvedValue = ResolveVariables(prop->propertyValue);
                    cssOutput << "  " << prop->property << ": " << resolvedValue << ";\n";
                }
            }
        }
        
        cssOutput << "}\n\n";
    }
    
    void GenerateIdCSS(const std::string& idName, std::shared_ptr<ASTNode> node) {
        if (generatedIds.find(idName) != generatedIds.end()) {
            return; // 已生成
        }
        
        generatedIds.insert(idName);
        
        cssOutput << "#" << idName << " {\n";
        
        for (const auto& child : node->children) {
            if (child->type == ASTNodeType::STYLE_PROPERTY) {
                if (auto* prop = dynamic_cast<StylePropertyNode*>(child.get())) {
                    std::string resolvedValue = ResolveVariables(prop->propertyValue);
                    cssOutput << "  " << prop->property << ": " << resolvedValue << ";\n";
                }
            }
        }
        
        cssOutput << "}\n\n";
    }
    
    void GenerateElementUse(std::shared_ptr<ASTNode> node) {
        std::string elementName = node->value;
        
        // 查找元素定义
        auto elementObj = context->GetGlobalMap().Find(elementName, GlobalObjectType::TEMPLATE_ELEMENT);
        if (!elementObj) {
            elementObj = context->GetGlobalMap().Find(elementName, GlobalObjectType::CUSTOM_ELEMENT);
        }
        
        if (!elementObj || !elementObj->astNode) {
            return; // 错误已在解析阶段报告
        }
        
        // 生成元素内容（展开元素定义）
        for (const auto& child : elementObj->astNode->children) {
            GenerateNode(child);
        }
    }
    
    void ProcessStyleUse(std::shared_ptr<ASTNode> node, std::stringstream& inlineStyles) {
        std::string styleName = node->value;
        
        // 查找样式定义
        auto styleObj = context->GetGlobalMap().Find(styleName, GlobalObjectType::TEMPLATE_STYLE);
        if (!styleObj) {
            styleObj = context->GetGlobalMap().Find(styleName, GlobalObjectType::CUSTOM_STYLE);
        }
        
        if (!styleObj || !styleObj->astNode) {
            return; // 错误已在解析阶段报告
        }
        
        // 将样式属性添加到内联样式
        for (const auto& child : styleObj->astNode->children) {
            if (child->type == ASTNodeType::STYLE_PROPERTY) {
                if (auto* prop = dynamic_cast<StylePropertyNode*>(child.get())) {
                    if (!inlineStyles.str().empty()) inlineStyles << " ";
                    // 解析属性值中的变量
                    std::string resolvedValue = ResolveVariables(prop->propertyValue);
                    inlineStyles << prop->property << ": " << resolvedValue << ";";
                }
            }
        }
    }
    
    void ProcessAmpersandSelector(std::shared_ptr<ASTNode> node,
                                 const std::string& elementTag,
                                 std::unordered_map<std::string, std::string>& attributes,
                                 std::vector<std::string>& classes) {
        // 尝试从上下文推导选择器
        std::string selector;
        
        // 优先使用类名
        if (!classes.empty()) {
            selector = "." + classes[0];
        } else if (attributes.find("id") != attributes.end()) {
            selector = "#" + attributes["id"];
        } else {
            // 生成自动类名
            std::string autoClass = context->GenerateAutoClass();
            classes.push_back(autoClass);
            selector = "." + autoClass;
        }
        
        // 替换 & 为实际选择器
        std::string fullSelector = node->value;
        size_t pos = fullSelector.find('&');
        if (pos != std::string::npos) {
            fullSelector.replace(pos, 1, selector);
        }
        
        cssOutput << fullSelector << " {\n";
        
        for (const auto& child : node->children) {
            if (child->type == ASTNodeType::STYLE_PROPERTY) {
                if (auto* prop = dynamic_cast<StylePropertyNode*>(child.get())) {
                    std::string resolvedValue = ResolveVariables(prop->propertyValue);
                    cssOutput << "  " << prop->property << ": " << resolvedValue << ";\n";
                }
            }
        }
        
        cssOutput << "}\n\n";
    }
    
    std::string ResolveVariables(const std::string& value) {
        std::string result = value;
        
        // 查找所有的变量表达式 VarGroupName(key)
        size_t pos = 0;
        while ((pos = result.find('(', pos)) != std::string::npos) {
            // 查找变量组名
            size_t groupStart = pos;
            while (groupStart > 0 && 
                   (std::isalnum(result[groupStart-1]) || result[groupStart-1] == '_')) {
                groupStart--;
            }
            
            if (groupStart < pos) {
                std::string varGroupName = result.substr(groupStart, pos - groupStart);
                
                // 查找右括号
                size_t closePos = result.find(')', pos);
                if (closePos != std::string::npos) {
                    std::string keyPart = result.substr(pos + 1, closePos - pos - 1);
                    
                    // 提取key（可能包含默认值）
                    std::string key = keyPart;
                    std::string defaultValue;
                    size_t eqPos = keyPart.find('=');
                    if (eqPos != std::string::npos) {
                        key = keyPart.substr(0, eqPos);
                        defaultValue = keyPart.substr(eqPos + 1);
                        // 去除空格
                        key.erase(0, key.find_first_not_of(" \t"));
                        key.erase(key.find_last_not_of(" \t") + 1);
                        defaultValue.erase(0, defaultValue.find_first_not_of(" \t"));
                        defaultValue.erase(defaultValue.find_last_not_of(" \t") + 1);
                    }
                    
                    // 查找变量值
                    std::string varValue = LookupVariable(varGroupName, key, defaultValue);
                    
                    // 替换
                    if (!varValue.empty()) {
                        result.replace(groupStart, closePos - groupStart + 1, varValue);
                        pos = groupStart + varValue.length();
                    } else {
                        pos = closePos + 1;
                    }
                } else {
                    pos++;
                }
            } else {
                pos++;
            }
        }
        
        return result;
    }
    
    std::string LookupVariable(const std::string& groupName, 
                               const std::string& key,
                               const std::string& defaultValue) {
        // 查找变量组
        auto varObj = context->GetGlobalMap().Find(groupName, GlobalObjectType::TEMPLATE_VAR);
        if (!varObj) {
            varObj = context->GetGlobalMap().Find(groupName, GlobalObjectType::CUSTOM_VAR);
        }
        
        if (varObj && varObj->astNode) {
            // 在变量组中查找指定的key
            for (const auto& child : varObj->astNode->children) {
                if (child->type == ASTNodeType::CONFIG_PROPERTY) {
                    std::string propName = child->GetAttribute("name");
                    if (propName == key) {
                        return child->value;
                    }
                }
            }
        }
        
        // 没找到，返回默认值
        return defaultValue;
    }
    
    void Indent() {
        if (config.prettyPrint) {
            for (int i = 0; i < currentIndent; ++i) {
                if (config.useSpaces) {
                    for (int j = 0; j < config.indentSize; ++j) {
                        htmlOutput << " ";
                    }
                } else {
                    htmlOutput << "\t";
                }
            }
        }
    }
    
    std::string EscapeHTML(const std::string& text) {
        std::string escaped;
        for (char c : text) {
            switch (c) {
                case '<': escaped += "&lt;"; break;
                case '>': escaped += "&gt;"; break;
                case '&': escaped += "&amp;"; break;
                case '"': escaped += "&quot;"; break;
                case '\'': escaped += "&#39;"; break;
                default: escaped += c; break;
            }
        }
        return escaped;
    }
};

// Generator公共接口实现
Generator::Generator(CompileContext* context, const GeneratorConfig& config)
    : pImpl(std::make_unique<Impl>(context, config)) {}

Generator::~Generator() = default;

std::string Generator::Generate(std::shared_ptr<ASTNode> ast) {
    return pImpl->Generate(ast);
}

std::string Generator::GetGeneratedCSS() const {
    return pImpl->cssOutput.str();
}

std::string Generator::GetGeneratedJS() const {
    return pImpl->jsOutput.str();
}

} // namespace Compiler
} // namespace CHTL