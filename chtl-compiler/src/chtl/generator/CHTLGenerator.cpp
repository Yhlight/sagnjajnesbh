#include "chtl/generator/CHTLGenerator.h"
#include "chtl/ast/DocumentNode.h"
#include "chtl/ast/ElementNode.h"
#include "chtl/ast/AttributeNode.h"
#include "chtl/ast/TextNode.h"
#include "chtl/ast/StyleBlockNode.h"
#include "chtl/ast/ScriptBlockNode.h"
#include "chtl/ast/TemplateNode.h"
#include "chtl/ast/CustomNode.h"
#include "chtl/ast/CommentNode.h"
#include "chtl/ast/LiteralNodes.h"
#include "utils/Logger.h"
#include "utils/StringUtils.h"
#include <algorithm>

namespace chtl {
namespace generator {

using namespace ast;
using namespace compiler;

CHTLGenerator::CHTLGenerator()
    : m_CurrentStream(&m_HtmlStream)
    , m_IndentLevel(0)
    , m_InStyleBlock(false)
    , m_InScriptBlock(false)
    , m_PrettyPrint(true)
    , m_IndentSize(2)
    , m_GenerateComments(true) {
    m_GlobalMap = std::make_shared<CHTLGlobalMap>();
}

CHTLGenerator::~CHTLGenerator() = default;

GenerateResult CHTLGenerator::Generate(ASTNode* root) {
    GenerateResult result;
    
    // 清空流
    m_HtmlStream.str("");
    m_HtmlStream.clear();
    m_CssStream.str("");
    m_CssStream.clear();
    m_JsStream.str("");
    m_JsStream.clear();
    
    // 清空状态
    m_IndentLevel = 0;
    m_InStyleBlock = false;
    m_InScriptBlock = false;
    m_CurrentElement.clear();
    m_GeneratedClasses.clear();
    m_GeneratedIds.clear();
    m_AutoClassCounter.clear();
    m_AutoIdCounter.clear();
    m_Errors.clear();
    
    // 访问AST
    if (root) {
        root->Accept(this);
    }
    
    // 构建结果
    result.Success = m_Errors.empty();
    result.Html = m_HtmlStream.str();
    result.Css = m_CssStream.str();
    result.JavaScript = m_JsStream.str();
    result.Errors = m_Errors;
    result.GeneratedClasses = m_GeneratedClasses;
    result.GeneratedIds = m_GeneratedIds;
    
    return result;
}

void CHTLGenerator::Write(const std::string& text) {
    *m_CurrentStream << text;
}

void CHTLGenerator::WriteLine(const std::string& text) {
    if (m_PrettyPrint) {
        WriteIndent();
    }
    Write(text);
    if (m_PrettyPrint) {
        Write("\n");
    }
}

void CHTLGenerator::WriteIndent() {
    if (m_PrettyPrint) {
        for (int i = 0; i < m_IndentLevel * m_IndentSize; ++i) {
            Write(" ");
        }
    }
}

void CHTLGenerator::VisitDocument(DocumentNode* node) {
    // 生成DOCTYPE
    WriteLine("<!DOCTYPE html>");
    
    // 处理子节点
    for (const auto& child : node->GetChildren()) {
        if (child) {
            child->Accept(this);
        }
    }
}

void CHTLGenerator::VisitComment(CommentNode* node) {
    if (!m_GenerateComments) {
        return;
    }
    
    // 只有生成器注释（--）会被输出到HTML
    if (node->GetCommentType() == CommentNode::GENERATOR) {
        WriteLine("<!-- " + node->GetValue() + " -->");
    }
}

void CHTLGenerator::VisitText(TextNode* node) {
    const std::string& content = node->GetContent();
    if (!content.empty()) {
        if (m_InStyleBlock || m_InScriptBlock) {
            // 在样式或脚本块中，直接输出
            Write(content);
        } else {
            // 在HTML中，需要转义
            WriteLine(EscapeHtml(content));
        }
    }
}

void CHTLGenerator::VisitElement(ElementNode* node) {
    const std::string& tagName = node->GetTagName();
    m_CurrentElement = tagName;
    
    // 自闭合标签
    static const std::unordered_set<std::string> selfClosingTags = {
        "area", "base", "br", "col", "embed", "hr", "img", "input",
        "link", "meta", "param", "source", "track", "wbr"
    };
    
    bool isSelfClosing = selfClosingTags.count(tagName) > 0 || node->IsSelfClosing();
    
    // 开始标签
    if (m_PrettyPrint) WriteIndent();
    Write("<" + tagName);
    
    // 生成属性
    GenerateAttributes(node);
    
    // 处理局部样式块生成的类名和ID
    if (node->HasStyleBlock()) {
        auto styleBlock = node->GetStyleBlock();
        
        // 添加自动生成的类名
        for (const auto& className : styleBlock->GetAutoClasses()) {
            Write(" class=\"" + className + "\"");
            m_GeneratedClasses.insert(className);
        }
        
        // 添加自动生成的ID
        for (const auto& id : styleBlock->GetAutoIds()) {
            Write(" id=\"" + id + "\"");
            m_GeneratedIds.insert(id);
        }
    }
    
    if (isSelfClosing) {
        Write(" />");
        if (m_PrettyPrint) Write("\n");
    } else {
        Write(">");
        
        bool hasBlockContent = false;
        
        // 检查是否有块级内容
        for (const auto& child : node->GetChildren()) {
            if (child->GetType() != ASTNodeType::ATTRIBUTE &&
                child->GetType() != ASTNodeType::TEXT_NODE) {
                hasBlockContent = true;
                break;
            }
        }
        
        if (hasBlockContent && m_PrettyPrint) {
            Write("\n");
            IncreaseIndent();
        }
        
        // 处理子节点
        for (const auto& child : node->GetChildren()) {
            if (child->GetType() != ASTNodeType::ATTRIBUTE) {
                child->Accept(this);
            }
        }
        
        // 处理局部样式块
        if (node->HasStyleBlock()) {
            GenerateStyleBlock(node->GetStyleBlock().get());
        }
        
        // 处理局部脚本块
        if (node->HasScriptBlock()) {
            GenerateScriptBlock(node->GetScriptBlock().get());
        }
        
        if (hasBlockContent && m_PrettyPrint) {
            DecreaseIndent();
            WriteIndent();
        }
        
        // 结束标签
        Write("</" + tagName + ">");
        if (m_PrettyPrint) Write("\n");
    }
    
    m_CurrentElement.clear();
}

void CHTLGenerator::GenerateAttributes(ElementNode* node) {
    auto attributes = node->GetAttributes();
    for (const auto& attr : attributes) {
        Write(" " + attr->GetName());
        const std::string& value = attr->GetAttributeValue();
        if (!value.empty()) {
            Write("=\"" + EscapeHtml(value) + "\"");
        }
    }
}

void CHTLGenerator::VisitAttribute(AttributeNode* node) {
    // 属性在GenerateAttributes中处理
}

void CHTLGenerator::VisitStyleBlock(StyleBlockNode* node) {
    if (node->IsLocal()) {
        // 局部样式块的内容需要被提取到全局CSS
        auto oldStream = m_CurrentStream;
        m_CurrentStream = &m_CssStream;
        
        // 处理内联样式（直接应用到元素）
        if (!node->GetInlineProperties().empty()) {
            // 内联样式在元素生成时处理
            m_CurrentStream = oldStream;
            return;
        }
        
        // 生成样式规则
        GenerateStyleRules(node);
        
        m_CurrentStream = oldStream;
    } else {
        // 全局样式块
        WriteLine("<style>");
        IncreaseIndent();
        
        m_InStyleBlock = true;
        GenerateStyleRules(node);
        m_InStyleBlock = false;
        
        DecreaseIndent();
        WriteLine("</style>");
    }
}

void CHTLGenerator::GenerateStyleRules(StyleBlockNode* node) {
    // 生成样式规则
    for (const auto& rule : node->GetStyleRules()) {
        std::string selector = rule->GetSelector();
        
        // 处理&符号（上下文推导）
        if (selector.find('&') != std::string::npos) {
            // 优先使用类名，其次ID
            std::string replacement;
            if (!node->GetAutoClasses().empty()) {
                replacement = "." + *node->GetAutoClasses().begin();
            } else if (!node->GetAutoIds().empty()) {
                replacement = "#" + *node->GetAutoIds().begin();
            } else if (!m_CurrentElement.empty()) {
                replacement = m_CurrentElement;
            }
            
            size_t pos = 0;
            while ((pos = selector.find('&', pos)) != std::string::npos) {
                selector.replace(pos, 1, replacement);
                pos += replacement.length();
            }
        }
        
        WriteLine(selector + " {");
        IncreaseIndent();
        
        for (const auto& prop : rule->GetProperties()) {
            WriteLine(prop.first + ": " + prop.second + ";");
        }
        
        DecreaseIndent();
        WriteLine("}");
        
        if (m_PrettyPrint) {
            WriteLine();
        }
    }
    
    // 处理样式引用
    for (const auto& ref : node->GetStyleReferences()) {
        // TODO: 展开样式模板
        ExpandTemplate(ref->GetStyleName(), "Style");
    }
}

void CHTLGenerator::VisitScriptBlock(ScriptBlockNode* node) {
    if (node->IsLocal()) {
        // 局部脚本块需要被包装
        auto oldStream = m_CurrentStream;
        m_CurrentStream = &m_JsStream;
        
        std::string wrappedContent = WrapLocalScript(node->GetContent(), m_CurrentElement);
        WriteLine(wrappedContent);
        
        m_CurrentStream = oldStream;
    } else {
        // 全局脚本块
        WriteLine("<script>");
        IncreaseIndent();
        
        m_InScriptBlock = true;
        // 脚本内容已经在解析时收集，直接输出
        WriteLine(node->GetContent());
        m_InScriptBlock = false;
        
        DecreaseIndent();
        WriteLine("</script>");
    }
}

std::string CHTLGenerator::WrapLocalScript(const std::string& content, const std::string& element) {
    // 局部脚本需要被包装以避免全局污染
    std::string wrapped = "(function() {\n";
    wrapped += "  'use strict';\n";
    
    if (!element.empty()) {
        wrapped += "  // 局部脚本 - 元素: " + element + "\n";
    }
    
    wrapped += content;
    wrapped += "\n})();\n";
    
    return wrapped;
}

void CHTLGenerator::VisitTemplate(TemplateNode* node) {
    // 模板定义不生成输出，只注册到全局映射表
    ProcessTemplateUsage(node);
}

void CHTLGenerator::VisitCustom(CustomNode* node) {
    // 自定义定义不生成输出，只注册到全局映射表
    ProcessCustomUsage(node);
}

void CHTLGenerator::VisitImport(ImportNode* node) {
    // 导入语句不直接生成输出
    // TODO: 处理导入逻辑
}

void CHTLGenerator::VisitNamespace(NamespaceNode* node) {
    // 命名空间不直接生成输出
    // 处理子节点
    for (const auto& child : node->GetChildren()) {
        if (child) {
            child->Accept(this);
        }
    }
}

void CHTLGenerator::VisitConfiguration(ConfigurationNode* node) {
    // 配置不生成输出
}

void CHTLGenerator::VisitOrigin(OriginNode* node) {
    ProcessOriginBlock(node);
}

void CHTLGenerator::ProcessOriginBlock(OriginNode* node) {
    const std::string& type = node->GetOriginType();
    const std::string& content = node->GetContent();
    
    if (type == "@Html") {
        // 直接输出HTML
        WriteLine(content);
    } else if (type == "@Style") {
        // 输出到CSS流
        auto oldStream = m_CurrentStream;
        m_CurrentStream = &m_CssStream;
        WriteLine(content);
        m_CurrentStream = oldStream;
    } else if (type == "@JavaScript") {
        // 输出到JS流
        auto oldStream = m_CurrentStream;
        m_CurrentStream = &m_JsStream;
        WriteLine(content);
        m_CurrentStream = oldStream;
    } else {
        // 自定义类型，作为HTML输出
        WriteLine("<!-- Origin: " + type + " -->");
        WriteLine(content);
    }
}

void CHTLGenerator::VisitExcept(ExceptNode* node) {
    // 约束不生成输出，只用于编译时检查
}

void CHTLGenerator::VisitLiteral(LiteralNode* node) {
    Write(node->GetValue());
}

void CHTLGenerator::VisitIdentifier(IdentifierNode* node) {
    Write(node->GetName());
}

void CHTLGenerator::VisitReference(ReferenceNode* node) {
    // 引用需要根据类型处理
    const std::string& refName = node->GetReferenceName();
    
    if (node->GetType() == ASTNodeType::STYLE_REFERENCE) {
        ExpandTemplate(refName, "Style");
    } else if (node->GetType() == ASTNodeType::ELEMENT_REFERENCE) {
        ExpandTemplate(refName, "Element");
    } else if (node->GetType() == ASTNodeType::VAR_REFERENCE) {
        // 变量引用在使用时解析
    }
}

std::string CHTLGenerator::GenerateAutoClassName(const std::string& selector) {
    // 生成唯一的类名
    std::string baseName = selector;
    if (baseName[0] == '.') {
        baseName = baseName.substr(1);
    }
    
    int& counter = m_AutoClassCounter[baseName];
    std::string className = baseName;
    if (counter > 0) {
        className += std::to_string(counter);
    }
    counter++;
    
    return className;
}

std::string CHTLGenerator::GenerateAutoId(const std::string& selector) {
    // 生成唯一的ID
    std::string baseName = selector;
    if (baseName[0] == '#') {
        baseName = baseName.substr(1);
    }
    
    int& counter = m_AutoIdCounter[baseName];
    std::string id = baseName;
    if (counter > 0) {
        id += std::to_string(counter);
    }
    counter++;
    
    return id;
}

void CHTLGenerator::ProcessTemplateUsage(TemplateNode* node) {
    // 注册模板到全局映射表
    // TODO: 实现模板注册
}

void CHTLGenerator::ExpandTemplate(const std::string& templateName, const std::string& type) {
    // TODO: 实现模板展开
    WriteLine("/* Template: " + templateName + " */");
}

void CHTLGenerator::ProcessCustomUsage(CustomNode* node) {
    // 注册自定义到全局映射表
    // TODO: 实现自定义注册
}

void CHTLGenerator::ExpandCustom(const std::string& customName, const std::string& type) {
    // TODO: 实现自定义展开
    WriteLine("/* Custom: " + customName + " */");
}

std::string CHTLGenerator::ResolveVarReference(const std::string& varGroup, const std::string& varName) {
    // TODO: 实现变量解析
    return varGroup + "(" + varName + ")";
}

void CHTLGenerator::ReportError(const std::string& message) {
    m_Errors.push_back(message);
    utils::Logger::GetInstance().Error("生成错误: " + message);
}

std::string CHTLGenerator::EscapeHtml(const std::string& text) {
    return utils::StringUtils::EscapeHtml(text);
}

std::string CHTLGenerator::EscapeCss(const std::string& text) {
    // TODO: 实现CSS转义
    return text;
}

std::string CHTLGenerator::EscapeJs(const std::string& text) {
    // TODO: 实现JavaScript转义
    return text;
}

} // namespace generator
} // namespace chtl