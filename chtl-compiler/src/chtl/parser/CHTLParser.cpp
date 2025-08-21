#include "chtl/parser/CHTLParser.h"
#include "chtl/ast/ElementNode.h"
#include "chtl/ast/AttributeNode.h"
#include "chtl/ast/TextNode.h"
#include "chtl/ast/StyleBlockNode.h"
#include "chtl/ast/ScriptBlockNode.h"
#include "chtl/ast/TemplateNode.h"
#include "chtl/ast/CustomNode.h"
#include "chtl/ast/ImportNode.h"
#include "chtl/ast/NamespaceNode.h"
#include "chtl/ast/ConfigurationNode.h"
#include "chtl/ast/OriginNode.h"
#include "chtl/ast/ExceptNode.h"
#include "chtl/ast/SpecializationNodes.h"
#include "chtl/ast/LiteralNodes.h"
#include "chtl/ast/CommentNode.h"
#include "utils/Logger.h"
#include <stdexcept>

namespace chtl {
namespace parser {

using namespace compiler;
using namespace ast;

CHTLParser::CHTLParser()
    : m_Current(0)
    , m_DebugMode(false) {
    m_StateMachine = std::make_unique<CHTLStateMachine>();
    m_Context = std::make_unique<CHTLContext>();
    m_GlobalMap = std::make_shared<CHTLGlobalMap>();
}

CHTLParser::~CHTLParser() = default;

std::shared_ptr<DocumentNode> CHTLParser::Parse(
    const std::vector<CHTLToken>& tokens,
    const std::string& filename) {
    
    m_Tokens = tokens;
    m_Current = 0;
    m_Errors.clear();
    
    auto document = std::make_shared<DocumentNode>();
    document->SetFilename(filename);
    
    // 进入文档状态
    EnterState(CHTLStateType::DOCUMENT);
    
    // 解析顶层元素
    while (!IsAtEnd()) {
        try {
            auto node = ParseTopLevel();
            if (node) {
                document->AddChild(node);
            }
        } catch (const std::exception& e) {
            ReportError(std::string("解析错误: ") + e.what());
            Synchronize();
        }
    }
    
    // 退出文档状态
    ExitState();
    
    return document;
}

bool CHTLParser::IsAtEnd() const {
    return m_Current >= m_Tokens.size() || 
           Current().GetType() == CHTLTokenType::END_OF_FILE;
}

const CHTLToken& CHTLParser::Current() const {
    if (m_Current < m_Tokens.size()) {
        return m_Tokens[m_Current];
    }
    static CHTLToken eof(CHTLTokenType::END_OF_FILE, "", 0, 0);
    return eof;
}

const CHTLToken& CHTLParser::Previous() const {
    if (m_Current > 0 && m_Current <= m_Tokens.size()) {
        return m_Tokens[m_Current - 1];
    }
    return Current();
}

const CHTLToken& CHTLParser::Peek(int offset) const {
    size_t index = m_Current + offset;
    if (index < m_Tokens.size()) {
        return m_Tokens[index];
    }
    static CHTLToken eof(CHTLTokenType::END_OF_FILE, "", 0, 0);
    return eof;
}

CHTLToken CHTLParser::Advance() {
    if (!IsAtEnd()) {
        m_Current++;
    }
    return Previous();
}

bool CHTLParser::Check(CHTLTokenType type) const {
    if (IsAtEnd()) return false;
    return Current().GetType() == type;
}

bool CHTLParser::Match(CHTLTokenType type) {
    if (Check(type)) {
        Advance();
        return true;
    }
    return false;
}

bool CHTLParser::Match(std::initializer_list<CHTLTokenType> types) {
    for (auto type : types) {
        if (Check(type)) {
            Advance();
            return true;
        }
    }
    return false;
}

std::shared_ptr<ASTNode> CHTLParser::ParseTopLevel() {
    // 跳过注释
    while (Match({CHTLTokenType::COMMENT_SINGLE, 
                  CHTLTokenType::COMMENT_MULTI,
                  CHTLTokenType::COMMENT_GENERATOR})) {
        auto commentType = Previous().GetType();
        auto content = Previous().GetValue();
        
        CommentNode::CommentType type;
        if (commentType == CHTLTokenType::COMMENT_SINGLE) {
            type = CommentNode::SINGLE_LINE;
        } else if (commentType == CHTLTokenType::COMMENT_MULTI) {
            type = CommentNode::MULTI_LINE;
        } else {
            type = CommentNode::GENERATOR;
        }
        
        auto comment = std::make_shared<CommentNode>(type, content);
        comment->SetLocation(Previous().GetLine(), Previous().GetColumn());
        
        // 只有生成器注释才返回节点
        if (type == CommentNode::GENERATOR) {
            return comment;
        }
    }
    
    // [Template]
    if (Check(CHTLTokenType::BRACKET_TEMPLATE)) {
        return ParseTemplate();
    }
    
    // [Custom]
    if (Check(CHTLTokenType::BRACKET_CUSTOM)) {
        return ParseCustom();
    }
    
    // [Import]
    if (Check(CHTLTokenType::BRACKET_IMPORT)) {
        return ParseImport();
    }
    
    // [Namespace]
    if (Check(CHTLTokenType::BRACKET_NAMESPACE)) {
        return ParseNamespace();
    }
    
    // [Configuration]
    if (Check(CHTLTokenType::BRACKET_CONFIGURATION)) {
        return ParseConfiguration();
    }
    
    // [Origin]
    if (Check(CHTLTokenType::BRACKET_ORIGIN)) {
        return ParseOrigin();
    }
    
    // 元素
    if (Check(CHTLTokenType::IDENTIFIER)) {
        return ParseElement();
    }
    
    // text
    if (Check(CHTLTokenType::KEYWORD_TEXT)) {
        return ParseTextNode();
    }
    
    // style (全局)
    if (Check(CHTLTokenType::KEYWORD_STYLE)) {
        return ParseStyleBlock();
    }
    
    // script (全局)
    if (Check(CHTLTokenType::KEYWORD_SCRIPT)) {
        return ParseScriptBlock();
    }
    
    ReportError("意外的token: " + Current().GetValue());
    Advance();
    return nullptr;
}

std::shared_ptr<ElementNode> CHTLParser::ParseElement() {
    auto elementName = Advance().GetValue();
    auto element = std::make_shared<ElementNode>(elementName);
    element->SetLocation(Previous().GetLine(), Previous().GetColumn());
    
    // 进入元素状态
    EnterState(CHTLStateType::ELEMENT);
    m_Context->PushElement(elementName);
    
    // 期望 {
    if (!ExpectToken(CHTLTokenType::LEFT_BRACE, "期望 '{' 在元素 " + elementName + " 后")) {
        ExitState();
        m_Context->PopElement();
        return element;
    }
    
    // 解析元素内容
    while (!Check(CHTLTokenType::RIGHT_BRACE) && !IsAtEnd()) {
        // 属性 (id: value; 或 class: value;)
        if (Check(CHTLTokenType::IDENTIFIER)) {
            auto next = Peek(1);
            if (next.GetType() == CHTLTokenType::COLON || 
                next.GetType() == CHTLTokenType::EQUAL) {
                auto attr = ParseAttribute();
                if (attr) {
                    element->AddAttribute(attr);
                }
                continue;
            }
        }
        
        // style块
        if (Check(CHTLTokenType::KEYWORD_STYLE)) {
            auto styleBlock = ParseStyleBlock();
            if (styleBlock) {
                element->SetStyleBlock(styleBlock);
            }
            continue;
        }
        
        // script块
        if (Check(CHTLTokenType::KEYWORD_SCRIPT)) {
            auto scriptBlock = ParseScriptBlock();
            if (scriptBlock) {
                element->SetScriptBlock(scriptBlock);
            }
            continue;
        }
        
        // text节点
        if (Check(CHTLTokenType::KEYWORD_TEXT)) {
            auto textNode = ParseTextNode();
            if (textNode) {
                element->AddChild(textNode);
            }
            continue;
        }
        
        // 子元素
        if (Check(CHTLTokenType::IDENTIFIER)) {
            auto child = ParseElement();
            if (child) {
                element->AddChild(child);
            }
            continue;
        }
        
        // 引用 (@Element, @Style等)
        if (Check(CHTLTokenType::AT_ELEMENT)) {
            auto elemRef = ParseElementReference();
            if (elemRef) {
                element->AddChild(elemRef);
            }
            continue;
        }
        
        // except约束
        if (Check(CHTLTokenType::KEYWORD_EXCEPT)) {
            auto except = ParseExcept();
            if (except) {
                element->AddChild(except);
            }
            continue;
        }
        
        // 跳过未知token
        ReportError("元素内部意外的token: " + Current().GetValue());
        Advance();
    }
    
    // 期望 }
    ExpectToken(CHTLTokenType::RIGHT_BRACE, "期望 '}' 结束元素 " + elementName);
    
    // 退出元素状态
    ExitState();
    m_Context->PopElement();
    
    return element;
}

std::shared_ptr<AttributeNode> CHTLParser::ParseAttribute() {
    auto name = Advance().GetValue();
    auto attr = std::make_shared<AttributeNode>(name);
    attr->SetLocation(Previous().GetLine(), Previous().GetColumn());
    
    // : 或 = (CE对等式)
    bool useColon = Check(CHTLTokenType::COLON);
    if (!Match({CHTLTokenType::COLON, CHTLTokenType::EQUAL})) {
        ReportError("期望 ':' 或 '=' 在属性 " + name + " 后");
        return attr;
    }
    attr->SetUseColon(useColon);
    
    // 属性值
    if (Check(CHTLTokenType::STRING_LITERAL)) {
        auto value = Advance().GetValue();
        attr->SetAttributeValue(value);
        attr->SetValueIsLiteral(true);
    } else if (Check(CHTLTokenType::IDENTIFIER) || Check(CHTLTokenType::NUMBER)) {
        auto value = Advance().GetValue();
        attr->SetAttributeValue(value);
        attr->SetValueIsLiteral(false);
    } else {
        ReportError("期望属性值");
    }
    
    // 期望 ;
    ExpectToken(CHTLTokenType::SEMICOLON, "期望 ';' 在属性后");
    
    return attr;
}

std::shared_ptr<TextNode> CHTLParser::ParseTextNode() {
    Advance(); // 消费 text
    auto textNode = std::make_shared<TextNode>();
    textNode->SetLocation(Previous().GetLine(), Previous().GetColumn());
    
    // 期望 {
    if (!ExpectToken(CHTLTokenType::LEFT_BRACE, "期望 '{' 在 text 后")) {
        return textNode;
    }
    
    // 文本内容
    if (Check(CHTLTokenType::STRING_LITERAL)) {
        auto content = Advance().GetValue();
        textNode->SetContent(content);
        textNode->SetIsLiteral(true);
    } else if (Check(CHTLTokenType::IDENTIFIER)) {
        // 无引号字面量
        auto content = Advance().GetValue();
        textNode->SetContent(content);
        textNode->SetIsLiteral(false);
    }
    
    // 期望 }
    ExpectToken(CHTLTokenType::RIGHT_BRACE, "期望 '}' 结束 text");
    
    return textNode;
}

std::shared_ptr<StyleBlockNode> CHTLParser::ParseStyleBlock() {
    Advance(); // 消费 style
    auto styleBlock = std::make_shared<StyleBlockNode>();
    styleBlock->SetLocation(Previous().GetLine(), Previous().GetColumn());
    
    // 判断是否是局部样式块
    bool isLocal = m_StateMachine->GetCurrentState() == CHTLStateType::ELEMENT;
    styleBlock->SetIsLocal(isLocal);
    
    // 期望 {
    if (!ExpectToken(CHTLTokenType::LEFT_BRACE, "期望 '{' 在 style 后")) {
        return styleBlock;
    }
    
    // 进入样式块状态
    EnterState(CHTLStateType::STYLE);
    
    // 解析样式内容
    while (!Check(CHTLTokenType::RIGHT_BRACE) && !IsAtEnd()) {
        // @Style引用
        if (Check(CHTLTokenType::AT_STYLE)) {
            auto styleRef = ParseStyleReference();
            if (styleRef) {
                styleBlock->AddStyleReference(styleRef);
            }
            continue;
        }
        
        // 选择器 (.class, #id, &, 标签)
        if (Check(CHTLTokenType::DOT) || 
            Check(CHTLTokenType::HASH) ||
            Check(CHTLTokenType::AMPERSAND) ||
            Check(CHTLTokenType::IDENTIFIER)) {
            
            std::string selector;
            StyleRuleNode::SelectorType selectorType = StyleRuleNode::COMPLEX;
            
            if (Match(CHTLTokenType::DOT)) {
                selector = ".";
                if (Check(CHTLTokenType::IDENTIFIER)) {
                    selector += Advance().GetValue();
                    selectorType = StyleRuleNode::CLASS;
                    // 自动添加类名
                    if (isLocal) {
                        styleBlock->AddAutoClass(selector.substr(1));
                    }
                }
            } else if (Match(CHTLTokenType::HASH)) {
                selector = "#";
                if (Check(CHTLTokenType::IDENTIFIER)) {
                    selector += Advance().GetValue();
                    selectorType = StyleRuleNode::ID;
                    // 自动添加ID
                    if (isLocal) {
                        styleBlock->AddAutoId(selector.substr(1));
                    }
                }
            } else if (Match(CHTLTokenType::AMPERSAND)) {
                selector = "&";
                selectorType = StyleRuleNode::AMPERSAND;
                // 可能有伪类/伪元素
                if (Check(CHTLTokenType::COLON)) {
                    selector += Advance().GetValue();
                    if (Check(CHTLTokenType::IDENTIFIER)) {
                        selector += Advance().GetValue();
                    }
                }
            } else {
                // 标签选择器或属性
                auto identifier = Advance().GetValue();
                
                // 检查是否是属性（后面跟着:）
                if (Check(CHTLTokenType::COLON)) {
                    Advance(); // 消费 :
                    
                    // 解析属性值
                    std::string value;
                    while (!Check(CHTLTokenType::SEMICOLON) && !IsAtEnd()) {
                        value += Current().GetValue() + " ";
                        Advance();
                    }
                    
                    if (!value.empty()) {
                        value.pop_back(); // 移除最后的空格
                    }
                    
                    ExpectToken(CHTLTokenType::SEMICOLON, "期望 ';' 在样式属性后");
                    
                    // 添加内联属性
                    styleBlock->AddInlineProperty(identifier, value);
                    continue;
                } else {
                    selector = identifier;
                    selectorType = StyleRuleNode::TAG;
                }
            }
            
            // 如果是选择器，解析规则块
            if (!selector.empty() && Check(CHTLTokenType::LEFT_BRACE)) {
                Advance(); // 消费 {
                
                auto rule = std::make_shared<StyleRuleNode>(selector);
                rule->SetSelectorType(selectorType);
                
                // 解析样式属性
                while (!Check(CHTLTokenType::RIGHT_BRACE) && !IsAtEnd()) {
                    if (Check(CHTLTokenType::IDENTIFIER)) {
                        auto prop = Advance().GetValue();
                        
                        if (Match(CHTLTokenType::COLON)) {
                            std::string value;
                            while (!Check(CHTLTokenType::SEMICOLON) && !IsAtEnd()) {
                                value += Current().GetValue() + " ";
                                Advance();
                            }
                            if (!value.empty()) {
                                value.pop_back();
                            }
                            
                            ExpectToken(CHTLTokenType::SEMICOLON, "期望 ';'");
                            rule->AddProperty(prop, value);
                        }
                    } else {
                        Advance(); // 跳过
                    }
                }
                
                ExpectToken(CHTLTokenType::RIGHT_BRACE, "期望 '}' 结束样式规则");
                styleBlock->AddStyleRule(rule);
            }
            continue;
        }
        
        // 跳过未知token
        Advance();
    }
    
    // 期望 }
    ExpectToken(CHTLTokenType::RIGHT_BRACE, "期望 '}' 结束 style");
    
    // 退出样式块状态
    ExitState();
    
    return styleBlock;
}

std::shared_ptr<ScriptBlockNode> CHTLParser::ParseScriptBlock() {
    Advance(); // 消费 script
    auto scriptBlock = std::make_shared<ScriptBlockNode>();
    scriptBlock->SetLocation(Previous().GetLine(), Previous().GetColumn());
    
    // 判断是否是局部脚本块
    bool isLocal = m_StateMachine->GetCurrentState() == CHTLStateType::ELEMENT;
    scriptBlock->SetIsLocal(isLocal);
    
    // 期望 {
    if (!ExpectToken(CHTLTokenType::LEFT_BRACE, "期望 '{' 在 script 后")) {
        return scriptBlock;
    }
    
    // 收集脚本内容（原始内容，包含CHTL JS和JavaScript）
    std::string content;
    int braceCount = 1;
    
    while (!IsAtEnd() && braceCount > 0) {
        auto token = Current();
        
        if (token.GetType() == CHTLTokenType::LEFT_BRACE) {
            braceCount++;
        } else if (token.GetType() == CHTLTokenType::RIGHT_BRACE) {
            braceCount--;
            if (braceCount == 0) {
                break;
            }
        }
        
        content += token.GetValue() + " ";
        Advance();
    }
    
    scriptBlock->SetContent(content);
    
    // 期望 }
    ExpectToken(CHTLTokenType::RIGHT_BRACE, "期望 '}' 结束 script");
    
    return scriptBlock;
}

// ... 继续实现其他解析方法 ...

bool CHTLParser::ExpectToken(CHTLTokenType type, const std::string& message) {
    if (Check(type)) {
        Advance();
        return true;
    }
    ReportError(message);
    return false;
}

bool CHTLParser::ConsumeToken(CHTLTokenType type, const std::string& message) {
    if (Match(type)) {
        return true;
    }
    ReportError(message);
    return false;
}

void CHTLParser::Synchronize() {
    Advance();
    
    while (!IsAtEnd()) {
        if (Previous().GetType() == CHTLTokenType::SEMICOLON) {
            return;
        }
        
        switch (Current().GetType()) {
            case CHTLTokenType::BRACKET_TEMPLATE:
            case CHTLTokenType::BRACKET_CUSTOM:
            case CHTLTokenType::BRACKET_IMPORT:
            case CHTLTokenType::BRACKET_NAMESPACE:
            case CHTLTokenType::BRACKET_CONFIGURATION:
            case CHTLTokenType::BRACKET_ORIGIN:
            case CHTLTokenType::IDENTIFIER:
            case CHTLTokenType::KEYWORD_TEXT:
            case CHTLTokenType::KEYWORD_STYLE:
            case CHTLTokenType::KEYWORD_SCRIPT:
                return;
            default:
                break;
        }
        
        Advance();
    }
}

void CHTLParser::ReportError(const std::string& message) {
    auto& token = Current();
    ReportError(token, message);
}

void CHTLParser::ReportError(const CHTLToken& token, const std::string& message) {
    std::string error = "[行 " + std::to_string(token.GetLine()) + 
                       ", 列 " + std::to_string(token.GetColumn()) + "] " + message;
    m_Errors.push_back(error);
    
    if (m_DebugMode) {
        utils::Logger::GetInstance().Error(error);
    }
}

void CHTLParser::EnterState(CHTLStateType state) {
    m_StateMachine->PushState(state);
}

void CHTLParser::ExitState() {
    m_StateMachine->PopState();
}

void CHTLParser::PushScope(const std::string& name) {
    m_Context->PushScope(name);
}

void CHTLParser::PopScope() {
    m_Context->PopScope();
}

} // namespace parser
} // namespace chtl