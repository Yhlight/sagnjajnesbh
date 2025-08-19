#include "compilers/chtl/chtl_code_generator.h"
#include <algorithm>
#include <regex>
#include <unordered_set>

namespace chtl {

CHTLCodeGenerator::CHTLCodeGenerator() 
    : indentLevel_(0), classNameCounter_(0) {
}

CHTLCodeGenerator::~CHTLCodeGenerator() = default;

std::string CHTLCodeGenerator::generate(std::shared_ptr<CHTLASTNode> ast) {
    output_.str("");
    globalStyles_.str("");
    globalScripts_.str("");
    indentLevel_ = 0;
    
    if (ast) {
        ast->accept(this);
    }
    
    return output_.str();
}

void CHTLCodeGenerator::visitDocument(DocumentNode* node) {
    for (const auto& child : node->getChildren()) {
        child->accept(this);
    }
}

void CHTLCodeGenerator::visitElement(ElementNode* node) {
    std::string indent = getIndent();
    const std::string& tagName = node->getTagName();
    
    // 开始标签
    output_ << indent << "<" << tagName;
    
    // 添加属性
    std::string attributes = generateAttributes(node->getAttributes());
    if (!attributes.empty()) {
        output_ << " " << attributes;
    }
    
    // 检查是否是自闭合标签
    static const std::unordered_set<std::string> selfClosingTags = {
        "area", "base", "br", "col", "embed", "hr", "img", "input",
        "link", "meta", "param", "source", "track", "wbr"
    };
    
    if (selfClosingTags.find(tagName) != selfClosingTags.end()) {
        output_ << " />\n";
        return;
    }
    
    output_ << ">\n";
    
    // 处理子节点
    indentLevel_++;
    for (const auto& child : node->getChildren()) {
        child->accept(this);
    }
    indentLevel_--;
    
    // 结束标签
    output_ << indent << "</" << tagName << ">\n";
}

void CHTLCodeGenerator::visitText(TextNode* node) {
    std::string indent = getIndent();
    std::string content = escapeHTML(node->getContent());
    
    if (!content.empty()) {
        output_ << indent << content << "\n";
    }
}

void CHTLCodeGenerator::visitStyleBlock(StyleBlockNode* node) {
    if (node->isLocal()) {
        // 局部样式处理
        std::string elementClass = generateClassName();
        std::string processedStyle = processLocalStyle(node->getContent(), elementClass);
        
        // 局部样式被提升到全局样式中
        globalStyles_ << processedStyle << "\n";
        
        // 返回生成的类名供元素使用
        // 这里需要一种机制将类名传回给父元素
    } else {
        // 全局样式直接添加
        globalStyles_ << node->getContent() << "\n";
    }
}

void CHTLCodeGenerator::visitTemplateDefinition(TemplateDefinitionNode* node) {
    // 模板定义不生成直接的HTML输出
    // 它们被存储供后续使用
}

std::string CHTLCodeGenerator::getIndent() const {
    return std::string(indentLevel_ * 2, ' ');
}

std::string CHTLCodeGenerator::generateClassName() {
    return "chtl-gen-" + std::to_string(++classNameCounter_);
}

std::string CHTLCodeGenerator::processLocalStyle(const std::string& style, const std::string& elementClass) {
    std::stringstream result;
    
    // 处理内联样式
    std::regex inlineStyleRegex(R"(^\s*([a-zA-Z-]+)\s*:\s*([^;]+);\s*$)", std::regex::multiline);
    std::string processedStyle = style;
    std::stringstream inlineStyles;
    
    auto begin = std::sregex_iterator(style.begin(), style.end(), inlineStyleRegex);
    auto end = std::sregex_iterator();
    
    // 收集所有内联样式
    std::vector<std::pair<std::string, std::string>> inlineProps;
    for (std::sregex_iterator i = begin; i != end; ++i) {
        std::smatch match = *i;
        inlineProps.push_back({match[1].str(), match[2].str()});
    }
    
    // 如果有内联样式，创建一个针对该元素的样式规则
    if (!inlineProps.empty()) {
        result << "." << elementClass << " {\n";
        for (const auto& prop : inlineProps) {
            result << "  " << prop.first << ": " << prop.second << ";\n";
        }
        result << "}\n";
    }
    
    // 处理选择器
    std::regex selectorRegex(R"((\.[a-zA-Z_][\w-]*|\#[a-zA-Z_][\w-]*|&[:\w\s-]*)\s*\{([^}]*)\})");
    
    auto selectorBegin = std::sregex_iterator(style.begin(), style.end(), selectorRegex);
    auto selectorEnd = std::sregex_iterator();
    
    for (std::sregex_iterator i = selectorBegin; i != selectorEnd; ++i) {
        std::smatch match = *i;
        std::string selector = match[1].str();
        std::string rules = match[2].str();
        
        // 处理 & 符号
        if (selector[0] == '&') {
            selector = "." + elementClass + selector.substr(1);
        }
        
        result << selector << " {" << rules << "}\n";
    }
    
    return result.str();
}

std::string CHTLCodeGenerator::escapeHTML(const std::string& text) {
    std::string result;
    for (char c : text) {
        switch (c) {
            case '&': result += "&amp;"; break;
            case '<': result += "&lt;"; break;
            case '>': result += "&gt;"; break;
            case '"': result += "&quot;"; break;
            case '\'': result += "&#39;"; break;
            default: result += c; break;
        }
    }
    return result;
}

std::string CHTLCodeGenerator::generateAttributes(const std::unordered_map<std::string, std::string>& attributes) {
    std::stringstream result;
    bool first = true;
    
    for (const auto& [name, value] : attributes) {
        if (!first) {
            result << " ";
        }
        first = false;
        
        result << name << "=\"" << escapeHTML(value) << "\"";
    }
    
    return result.str();
}

} // namespace chtl