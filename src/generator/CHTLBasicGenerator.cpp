#include "../../include/CHTLGenerator.h"
#include <iostream>
#include <sstream>

namespace chtl {

/**
 * CHTL基础语法生成器
 * 专门处理基础语法的HTML生成：文本节点、元素节点、属性
 */
class CHTLBasicGenerator {
public:
    explicit CHTLBasicGenerator(const CompilerOptions& options = CompilerOptions{})
        : options_(options) {
        
        if (options_.debugMode) {
            debugLog("CHTL基础生成器初始化完成");
        }
    }

    // 生成基础HTML
    GeneratorResult generateBasicHTML(std::shared_ptr<CHTLASTNode> ast) {
        result_ = GeneratorResult{};
        
        if (!ast) {
            result_.addError(GeneratorErrorType::AST_ERROR, "AST节点为空");
            return result_;
        }
        
        if (options_.debugMode) {
            debugLog("开始生成基础HTML代码");
        }
        
        try {
            std::ostringstream htmlStream;
            generateHTMLRecursive(ast, htmlStream);
            result_.htmlOutput = htmlStream.str();
            result_.success = true;
            
            if (options_.debugMode) {
                debugLog("基础HTML生成完成");
            }
            
        } catch (const std::exception& e) {
            result_.addError(GeneratorErrorType::OUTPUT_ERROR, 
                           "生成异常: " + String(e.what()));
        }
        
        return result_;
    }

private:
    CompilerOptions options_;
    GeneratorResult result_;

    // 递归生成HTML
    void generateHTMLRecursive(std::shared_ptr<CHTLASTNode> node, std::ostringstream& stream) {
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
                
            case ASTNodeType::ATTRIBUTE:
                // 属性节点在元素生成时处理，这里不直接处理
                break;
                
            default:
                if (options_.debugMode) {
                    debugLog("跳过未知节点类型: " + std::to_string(static_cast<int>(node->getType())));
                }
                break;
        }
    }

    // 生成元素HTML
    void generateElementHTML(std::shared_ptr<ElementNode> element, std::ostringstream& stream) {
        const String& tagName = element->getTagName();
        
        stream << "<" << tagName;
        
        // 收集属性
        StringMap attributes;
        for (const auto& child : element->getChildren()) {
            if (child->getType() == ASTNodeType::ATTRIBUTE) {
                auto attr = std::static_pointer_cast<AttributeNode>(child);
                attributes[attr->getName()] = attr->getValue();
            }
        }
        
        // 生成属性
        for (const auto& [name, value] : attributes) {
            stream << " " << name << "=\"" << escapeHTMLAttribute(value) << "\"";
        }
        
        // 检查是否为自闭合标签
        if (isSelfClosingTag(tagName)) {
            stream << " />";
        } else {
            stream << ">";
            
            // 处理子节点（非属性节点）
            for (const auto& child : element->getChildren()) {
                if (child->getType() != ASTNodeType::ATTRIBUTE) {
                    generateHTMLRecursive(child, stream);
                }
            }
            
            stream << "</" << tagName << ">";
        }
    }

    // 生成文本HTML
    void generateTextHTML(std::shared_ptr<TextNode> text, std::ostringstream& stream) {
        String content = text->getContent();
        
        // 检查是否为生成器注释
        if (text->getAttribute("type") && 
            text->getAttribute("type").value() == "generator-comment") {
            stream << "<!-- " << escapeHTML(content) << " -->";
        } else {
            stream << escapeHTML(content);
        }
    }

    // HTML转义
    String escapeHTML(const String& text) const {
        String result;
        result.reserve(text.size() * 1.1); // 预留空间
        
        for (char ch : text) {
            switch (ch) {
                case '<': result += "&lt;"; break;
                case '>': result += "&gt;"; break;
                case '&': result += "&amp;"; break;
                case '"': result += "&quot;"; break;
                case '\'': result += "&#x27;"; break;
                default: result += ch; break;
            }
        }
        
        return result;
    }

    // HTML属性转义
    String escapeHTMLAttribute(const String& text) const {
        String result;
        result.reserve(text.size() * 1.1);
        
        for (char ch : text) {
            switch (ch) {
                case '"': result += "&quot;"; break;
                case '&': result += "&amp;"; break;
                case '<': result += "&lt;"; break;
                case '>': result += "&gt;"; break;
                default: result += ch; break;
            }
        }
        
        return result;
    }

    // 检查是否为自闭合标签
    bool isSelfClosingTag(const String& tagName) const {
        static std::unordered_set<std::string> selfClosingTags = {
            "area", "base", "br", "col", "embed", "hr", "img", "input",
            "link", "meta", "param", "source", "track", "wbr"
        };
        
        return selfClosingTags.find(tagName) != selfClosingTags.end();
    }

    // 调试日志
    void debugLog(const String& message) const {
        if (options_.debugMode) {
            std::cout << "[CHTLBasicGenerator] " << message << std::endl;
        }
    }
};

} // namespace chtl