#include "Dispatcher/FragmentProcessors.h"
#include "Utils/StringUtils.h"
#include "Utils/ErrorHandler.h"
#include "CHTL/Lexer/CHTLLexer.h"
#include "CHTL/Parser/CHTLParser.h"
#include "CHTLJS/Parser/CHTLJSParser.h"
#include <unordered_set>
#include <regex>

namespace CHTL {
namespace Dispatcher {

// CHTL片段处理器实现
CHTLFragmentProcessor::CHTLFragmentProcessor(Parser::CHTLParser* parser) 
    : parser_(parser) {}

ProcessedFragment CHTLFragmentProcessor::ProcessFragment(const Scanner::CodeFragment& fragment) {
    ProcessedFragment result;
    result.originalType = fragment.type;
    result.originalPosition = fragment.startPos;
    
    std::string content = Utils::StringUtils::Trim(fragment.content);
    
    if (content.empty()) {
        return result;
    }
    
    // 使用真正的CHTL解析器来处理片段
    if (parser_) {
        try {
            // 创建词法分析器来处理片段
            CHTL::Lexer::CHTLLexer lexer;
            auto tokens = lexer.Tokenize(content, "fragment");
            
            // 使用解析器解析tokens
            auto ast = parser_->Parse(tokens, "fragment");
            if (ast) {
                // 这里需要生成器来生成HTML
                // 暂时返回原内容，等待生成器集成
                result.generatedCode = content;
                result.isContent = true;
                return result;
            }
        } catch (const std::exception& e) {
            // 解析失败，回退到简单处理
            Utils::ErrorHandler::GetInstance().LogWarning("CHTL片段解析失败: " + std::string(e.what()));
        }
    }
    
    // 回退到简单的字符串处理（保持原有逻辑作为备用）
    
    // 处理HTML元素
    if (IsHTMLElement(content)) {
        result.isOpenTag = true;
        result.elementType = content;
        result.generatedCode = "<" + content;
        return result;
    }
    
    // 处理开始大括号
    if (content == "{") {
        result.generatedCode = ""; // 不直接生成代码，由合并器处理
        return result;
    }
    
    // 处理结束大括号
    if (content == "}" || content.find("}") == 0) {
        result.isCloseTag = true;
        result.generatedCode = ""; // 结束标签将由智能合并器处理
        return result;
    }
    
    // 处理属性
    if (IsAttribute(content)) {
        result.attributes = ParseAttribute(content);
        result.generatedCode = " " + result.attributes;
        return result;
    }
    
    // 处理文本内容
    if (IsTextContent(content)) {
        result.isContent = true;
        result.generatedCode = content;
        return result;
    }
    
    // 跳过CHTL特殊语法
    if (content.substr(0, 1) == "[" || content.substr(0, 2) == "//" ||
        content == "text" || content == "style" || content == "script" ||
        content.substr(0, 6) == "except" || content.substr(0, 7) == "inherit") {
        result.generatedCode = ""; // 不生成代码
        return result;
    }
    
    // 默认作为内容处理
    result.isContent = true;
    result.generatedCode = content;
    return result;
}

bool CHTLFragmentProcessor::IsHTMLElement(const std::string& content) {
    static std::unordered_set<std::string> htmlElements = {
        "html", "head", "body", "title", "meta", "link", "style", "script",
        "div", "span", "p", "a", "img", "h1", "h2", "h3", "h4", "h5", "h6",
        "ul", "ol", "li", "table", "tr", "td", "th", "thead", "tbody", "tfoot",
        "form", "input", "textarea", "button", "select", "option", "label",
        "header", "footer", "nav", "main", "section", "article", "aside",
        "br", "hr", "strong", "em", "b", "i", "u", "small", "sub", "sup"
    };
    
    return htmlElements.count(content) > 0;
}

bool CHTLFragmentProcessor::IsAttribute(const std::string& content) {
    return content.find(":") != std::string::npos && 
           (content.find("id:") == 0 || content.find("class:") == 0 ||
            content.find("src:") == 0 || content.find("href:") == 0 ||
            content.find("alt:") == 0 || content.find("title:") == 0);
}

std::string CHTLFragmentProcessor::ParseAttribute(const std::string& content) {
    auto parts = Utils::StringUtils::Split(content, ":");
    if (parts.size() == 2) {
        std::string attrName = Utils::StringUtils::Trim(parts[0]);
        std::string attrValue = Utils::StringUtils::Trim(parts[1]);
        
        // 移除分号
        if (!attrValue.empty() && attrValue.back() == ';') {
            attrValue.pop_back();
        }
        
        // 移除引号（如果有）
        if (attrValue.length() >= 2 && 
            ((attrValue.front() == '"' && attrValue.back() == '"') ||
             (attrValue.front() == '\'' && attrValue.back() == '\''))) {
            attrValue = attrValue.substr(1, attrValue.length() - 2);
        }
        
        return attrName + "=\"" + attrValue + "\"";
    }
    
    return "";
}

bool CHTLFragmentProcessor::IsTextContent(const std::string& content) {
    // 检查是否为文本内容（不是关键字、不是符号）
    return !content.empty() && 
           content != "{" && content != "}" &&
           content.substr(0, 1) != "[" && content.substr(0, 2) != "//" &&
           content != "text" && content != "style" && content != "script" &&
           !IsHTMLElement(content) && !IsAttribute(content);
}

// CHTL JS片段处理器实现
CHTLJSFragmentProcessor::CHTLJSFragmentProcessor(CHTLJS::Parser::CHTLJSParser* parser) 
    : parser_(parser) {}

ProcessedFragment CHTLJSFragmentProcessor::ProcessFragment(const Scanner::CodeFragment& fragment) {
    ProcessedFragment result;
    result.originalType = fragment.type;
    result.originalPosition = fragment.startPos;
    
    std::string content = Utils::StringUtils::Trim(fragment.content);
    
    if (content.empty()) {
        return result;
    }
    
    // 处理增强选择器：{{.box}} → document.querySelector('.box')
    if (content.find("{{") != std::string::npos) {
        result.generatedCode = ProcessEnhancedSelector(content);
        return result;
    }
    
    // 处理虚对象调用：ButtonController->click() → ButtonController_click()
    if (content.find("->") != std::string::npos) {
        result.generatedCode = ProcessVirtualObjectCall(content);
        return result;
    }
    
    // 处理监听器语法：listen({ click: ... })
    if (content.find("listen") != std::string::npos) {
        result.generatedCode = ProcessListenerSyntax(content);
        return result;
    }
    
    // 处理虚对象声明：vir Test = 
    if (content.find("vir ") == 0) {
        std::string varName = content.substr(4);
        if (varName.find(" = ") != std::string::npos) {
            varName = varName.substr(0, varName.find(" = "));
        }
        result.generatedCode = "// Virtual object: " + varName;
        return result;
    }
    
    // 默认保持原样
    result.generatedCode = content;
    return result;
}

std::string CHTLJSFragmentProcessor::ProcessEnhancedSelector(const std::string& content) {
    // 处理增强选择器：{{.box}} → document.querySelector('.box')
    std::regex selectorRegex(R"(\{\{([^}]+)\}\})");
    std::smatch match;
    
    if (std::regex_search(content, match, selectorRegex)) {
        std::string selector = match[1].str();
        return "document.querySelector('" + selector + "')";
    }
    
    return content;
}

std::string CHTLJSFragmentProcessor::ProcessVirtualObjectCall(const std::string& content) {
    // 处理虚对象调用：ButtonController->click() → ButtonController_click()
    auto parts = Utils::StringUtils::Split(content, "->");
    if (parts.size() == 2) {
        std::string objName = Utils::StringUtils::Trim(parts[0]);
        std::string methodName = Utils::StringUtils::Trim(parts[1]);
        
        // 移除括号
        if (methodName.find("()") != std::string::npos) {
            methodName = methodName.substr(0, methodName.find("()"));
        }
        
        return objName + "_" + methodName + "()";
    }
    
    return content;
}

std::string CHTLJSFragmentProcessor::ProcessListenerSyntax(const std::string& content) {
    // 处理监听器语法：listen({ click: ... }) → addEventListener
    if (content.find("listen({") != std::string::npos) {
        return ".addEventListener("; // 简化实现
    }
    
    return content;
}

// CSS片段处理器实现
ProcessedFragment CSSFragmentProcessor::ProcessFragment(const Scanner::CodeFragment& fragment) {
    ProcessedFragment result;
    result.originalType = fragment.type;
    result.originalPosition = fragment.startPos;
    
    std::string content = Utils::StringUtils::Trim(fragment.content);
    
    if (content.empty()) {
        return result;
    }
    
    // 处理CSS选择器和属性
    if (content.find("{") != std::string::npos || content.find("}") != std::string::npos) {
        result.generatedCode = content; // CSS结构保持不变
    } else if (content.find(":") != std::string::npos && content.find(";") != std::string::npos) {
        // CSS属性
        result.generatedCode = "  " + content; // 添加缩进
    } else if (content.substr(0, 1) == "." || content.substr(0, 1) == "#") {
        // CSS选择器
        result.generatedCode = content + " {";
    } else {
        result.generatedCode = content;
    }
    
    return result;
}

std::string CSSFragmentProcessor::ProcessLocalStyle(const std::string& content) {
    // 处理局部样式，转换为全局样式
    return content;
}

std::string CSSFragmentProcessor::ProcessGlobalStyle(const std::string& content) {
    // 处理全局样式
    return content;
}

// JavaScript片段处理器实现
ProcessedFragment JavaScriptFragmentProcessor::ProcessFragment(const Scanner::CodeFragment& fragment) {
    ProcessedFragment result;
    result.originalType = fragment.type;
    result.originalPosition = fragment.startPos;
    
    std::string content = Utils::StringUtils::Trim(fragment.content);
    
    if (content.empty()) {
        return result;
    }
    
    // 验证JavaScript语法
    if (ValidateJavaScript(content)) {
        result.generatedCode = content;
    } else {
        result.generatedCode = "// Invalid JavaScript: " + content;
    }
    
    return result;
}

bool JavaScriptFragmentProcessor::ValidateJavaScript(const std::string& content) {
    // 基础JavaScript语法验证
    return true; // 简化实现
}

// 智能合并器实现
IntelligentMerger::MergedResult IntelligentMerger::Merge(const std::vector<ProcessedFragment>& fragments, bool supportSPA) {
    MergedResult result;
    
    // 检测是否为SPA组件
    result.isSPAComponent = supportSPA && DetectSPAComponent(fragments);
    
    // 分别合并各类型的片段
    result.htmlOutput = MergeHTMLFragments(fragments);
    result.cssOutput = MergeCSSFragments(fragments);
    result.jsOutput = MergeJavaScriptFragments(fragments);
    
    return result;
}

bool IntelligentMerger::DetectSPAComponent(const std::vector<ProcessedFragment>& fragments) {
    // 检测是否包含html根元素
    for (const auto& fragment : fragments) {
        if (fragment.elementType == "html") {
            return false; // 包含html根元素，不是SPA组件
        }
    }
    return true; // 没有html根元素，是SPA组件
}

std::string IntelligentMerger::MergeHTMLFragments(const std::vector<ProcessedFragment>& fragments) {
    std::string html = "";
    
    // 简化的状态机合并逻辑
    enum State { EXPECTING_ELEMENT, EXPECTING_OPEN_BRACE, IN_ELEMENT, EXPECTING_CLOSE_BRACE };
    State state = EXPECTING_ELEMENT;
    std::string currentElement = "";
    std::string currentAttributes = "";
    std::string currentContent = "";
    
    for (const auto& fragment : fragments) {
        if (fragment.originalType != Scanner::FragmentType::CHTL) {
            continue;
        }
        
        std::string content = Utils::StringUtils::Trim(fragment.generatedCode);
        
        if (content.empty()) {
            continue;
        }
        
        // 简化逻辑：直接根据片段类型和内容生成HTML
        if (fragment.isOpenTag) {
            // 开始元素
            currentElement = fragment.elementType;
            html += "<" + currentElement;
            state = EXPECTING_OPEN_BRACE;
            
        } else if (!fragment.attributes.empty()) {
            // 属性
            html += " " + fragment.attributes;
            
        } else if (content == "{") {
            // 开始大括号 - 完成开始标签
            html += ">";
            state = IN_ELEMENT;
            
        } else if (fragment.isContent && content != "text") {
            // 文本内容
            currentContent += content;
            
        } else if (fragment.isCloseTag || content.find("}") == 0) {
            // 结束大括号 - 完成元素
            if (!currentContent.empty()) {
                html += currentContent;
                currentContent = "";
            }
            html += "</" + currentElement + ">\n";
            state = EXPECTING_ELEMENT;
        }
    }
    
    return html;
}

std::string IntelligentMerger::MergeCSSFragments(const std::vector<ProcessedFragment>& fragments) {
    std::string css = "";
    
    for (const auto& fragment : fragments) {
        if (fragment.originalType == Scanner::FragmentType::CSS) {
            css += fragment.generatedCode + "\n";
        }
    }
    
    return css;
}

std::string IntelligentMerger::MergeJavaScriptFragments(const std::vector<ProcessedFragment>& fragments) {
    std::string js = "";
    
    for (const auto& fragment : fragments) {
        if (fragment.originalType == Scanner::FragmentType::CHTL_JS || 
            fragment.originalType == Scanner::FragmentType::JS) {
            js += fragment.generatedCode + "\n";
        }
    }
    
    return js;
}

std::string IntelligentMerger::BuildHTMLStructure(const std::string& htmlContent, const std::string& cssContent, 
                                                 const std::string& jsContent, bool isSPA) {
    if (isSPA) {
        // SPA组件模式 - 不生成完整HTML结构
        std::string result = "";
        
        if (!cssContent.empty()) {
            result += "<style>\n" + cssContent + "</style>\n\n";
        }
        
        result += htmlContent;
        
        if (!jsContent.empty()) {
            result += "\n<script>\n" + jsContent + "</script>";
        }
        
        return result;
    } else {
        // 完整页面模式
        std::string result = "<!DOCTYPE html>\n<html>\n<head>\n";
        result += "<meta charset=\"UTF-8\">\n";
        result += "<title>CHTL Generated Page</title>\n";
        
        if (!cssContent.empty()) {
            result += "<style>\n" + cssContent + "</style>\n";
        }
        
        result += "</head>\n<body>\n";
        result += htmlContent;
        
        if (!jsContent.empty()) {
            result += "<script>\n" + jsContent + "</script>\n";
        }
        
        result += "</body>\n</html>";
        return result;
    }
}

} // namespace Dispatcher
} // namespace CHTL