#include "CHTLParserV2.h"
#include <sstream>

namespace chtl {

CHTLParserV2::CHTLParserV2() : m_Current(0) {
    m_ContextStack.push(Context::TOP_LEVEL);
}

std::shared_ptr<ast::DocumentNode> CHTLParserV2::Parse(const std::string& source, const std::string& filename) {
    m_Source = source;  // 保存源代码
    
    CHTLLexerV2 lexer;
    lexer.SetInput(source, filename);
    
    auto tokens = lexer.Tokenize();
    
    if (lexer.HasErrors()) {
        m_Errors = lexer.GetErrors();
        return nullptr;
    }
    
    return ParseTokens(tokens);
}

std::shared_ptr<ast::DocumentNode> CHTLParserV2::ParseTokens(const std::vector<CHTLToken>& tokens) {
    m_Tokens = tokens;
    m_Current = 0;
    m_Errors.clear();
    
    // 重置上下文栈
    while (!m_ContextStack.empty()) {
        m_ContextStack.pop();
    }
    m_ContextStack.push(Context::TOP_LEVEL);
    
    auto document = std::make_shared<ast::DocumentNode>();
    
    while (!IsAtEnd()) {
        // 保存当前位置以检测进度
        size_t oldPos = m_Current;
        
        auto node = ParseTopLevel();
        if (node) {
            document->AddChild(node);
        }
        
        // 确保我们在前进，避免死循环
        if (m_Current == oldPos && !IsAtEnd()) {
            ReportError("Parser stuck at token: " + Peek().value);
            Advance(); // 强制前进
        }
    }
    
    return document;
}

std::shared_ptr<ast::ASTNode> CHTLParserV2::ParseTopLevel() {
    // 跳过顶级的分号
    while (Match(CHTLTokenType::SEMICOLON)) {}
    
    if (IsAtEnd()) return nullptr;
    
    // 在顶级遇到右大括号是错误
    if (Check(CHTLTokenType::RBRACE)) {
        ReportError("Unexpected '}' at top level");
        Advance(); // 消费这个错误的括号
        return nullptr;
    }
    
    // 注释
    if (Check(CHTLTokenType::COMMENT_HTML)) {
        return ParseComment();
    }
    
    // 模板定义
    if (Check(CHTLTokenType::TEMPLATE)) {
        return ParseTemplate();
    }
    
    // 自定义定义
    if (Check(CHTLTokenType::CUSTOM)) {
        return ParseCustom();
    }
    
    // Import
    if (Check(CHTLTokenType::IMPORT)) {
        return ParseImport();
    }
    
    // Namespace
    if (Check(CHTLTokenType::NAMESPACE)) {
        return ParseNamespace();
    }
    
    // Configuration
    if (Check(CHTLTokenType::CONFIGURATION)) {
        return ParseConfiguration();
    }
    
    // Origin
    if (Check(CHTLTokenType::ORIGIN)) {
        return ParseOrigin();
    }
    
    // 全局样式块
    if (Check(CHTLTokenType::STYLE)) {
        return ParseStyle();
    }
    
    // 全局脚本块
    if (Check(CHTLTokenType::SCRIPT)) {
        return ParseScript();
    }
    
    // 文本块
    if (Check(CHTLTokenType::TEXT)) {
        return ParseText();
    }
    
    // 元素
    if (Check(CHTLTokenType::IDENTIFIER)) {
        return ParseElement();
    }
    
    // 错误处理
    ReportError("Unexpected token at top level: " + Peek().value);
    Advance();
    return nullptr;
}

std::shared_ptr<ast::ASTNode> CHTLParserV2::ParseElement() {
    auto token = Advance(); // 元素名
    auto element = std::make_shared<ast::ElementNode>(token.value);
    element->SetLocation(token.line, token.column);
    
    // 元素必须有大括号
    if (!Match(CHTLTokenType::LBRACE)) {
        ReportError("Expected '{' after element name");
        return element;
    }
    
    PushContext(Context::ELEMENT);
    ParseElementContent(element.get());
    PopContext();
    
    if (!Match(CHTLTokenType::RBRACE)) {
        ReportError("Expected '}' to close element");
    }
    
    return element;
}

void CHTLParserV2::ParseElementContent(ast::ElementNode* element) {
    while (!IsAtEnd() && !Check(CHTLTokenType::RBRACE)) {
        // 跳过分号
        if (Match(CHTLTokenType::SEMICOLON)) continue;
        
        // 注释
        if (Check(CHTLTokenType::COMMENT_HTML)) {
            element->AddChild(ParseComment());
            continue;
        }
        
        // 局部样式块
        if (Check(CHTLTokenType::STYLE)) {
            ParseLocalStyle(element);
            continue;
        }
        
        // 局部脚本块
        if (Check(CHTLTokenType::SCRIPT)) {
            ParseLocalScript(element);
            continue;
        }
        
        // 文本块
        if (Check(CHTLTokenType::TEXT)) {
            element->AddChild(ParseText());
            continue;
        }
        
        // 特殊化操作
        if (CheckAny({CHTLTokenType::DELETE, CHTLTokenType::INSERT})) {
            element->AddChild(ParseSpecialization());
            continue;
        }
        
        // 变量
        if (Check(CHTLTokenType::DOLLAR)) {
            element->AddChild(ParseVar());
            continue;
        }
        
        // Origin块
        if (Check(CHTLTokenType::ORIGIN)) {
            element->AddChild(ParseOrigin());
            continue;
        }
        
        // @Style, @Element, @Var 模板使用
        if (CheckAny({CHTLTokenType::AT_STYLE, CHTLTokenType::AT_ELEMENT, CHTLTokenType::AT_VAR})) {
            auto type = Advance();
            if (Check(CHTLTokenType::IDENTIFIER)) {
                auto name = Advance();
                
                // 创建一个Custom节点来表示模板使用
                ast::CustomNode::CustomType customType;
                if (type.type == CHTLTokenType::AT_STYLE) {
                    customType = ast::CustomNode::STYLE;
                } else if (type.type == CHTLTokenType::AT_ELEMENT) {
                    customType = ast::CustomNode::ELEMENT;
                } else {
                    customType = ast::CustomNode::VAR;
                }
                
                auto custom = std::make_shared<ast::CustomNode>(customType, name.value);
                custom->SetLocation(type.line, type.column);
                
                // 检查是否有参数或属性块
                if (Match(CHTLTokenType::LPAREN)) {
                    // TODO: 解析参数
                    while (!IsAtEnd() && !Check(CHTLTokenType::RPAREN)) {
                        Advance();
                    }
                    Match(CHTLTokenType::RPAREN);
                } else if (Match(CHTLTokenType::LBRACE)) {
                    // 属性块（用于自定义）
                    while (!IsAtEnd() && !Check(CHTLTokenType::RBRACE)) {
                        auto node = ParseTopLevel();
                        if (node) {
                            custom->AddChild(node);
                        }
                    }
                    Match(CHTLTokenType::RBRACE);
                }
                
                element->AddChild(custom);
                Match(CHTLTokenType::SEMICOLON);
            }
            continue;
        }
        
        // 属性或子元素
        if (Check(CHTLTokenType::IDENTIFIER)) {
            auto savedPos = m_Current;
            auto identifier = Advance();
            
            // 检查是否是属性（后面跟着 : 或 =）
            if (MatchColonOrEquals()) {
                // 回退并解析属性
                m_Current = savedPos;
                ParseAttributes(element);
            } else if (Match(CHTLTokenType::LPAREN)) {
                // 可能是模板函数调用（如 ThemeColor(tableColor)）
                // 暂时跳过参数
                while (!IsAtEnd() && !Check(CHTLTokenType::RPAREN)) {
                    Advance();
                }
                Match(CHTLTokenType::RPAREN);
                Match(CHTLTokenType::SEMICOLON);
            } else {
                // 回退并解析子元素
                m_Current = savedPos;
                element->AddChild(ParseElement());
            }
            continue;
        }
        
        // 错误处理
        ReportError("Unexpected token in element: " + Peek().value);
        Advance();
    }
}

void CHTLParserV2::ParseAttributes(ast::ElementNode* element) {
    auto name = Advance(); // 属性名
    
    if (!MatchColonOrEquals()) {
        ReportError("Expected ':' or '=' after attribute name");
        return;
    }
    
    // 属性值 - 处理数字作为值
    std::string value;
    if (Check(CHTLTokenType::NUMBER)) {
        value = Advance().value;
    } else {
        value = ParseStringOrUnquoted();
    }
    element->AddAttribute(name.value, value);
    
    // 可选的分号
    Match(CHTLTokenType::SEMICOLON);
}

std::shared_ptr<ast::ASTNode> CHTLParserV2::ParseText() {
    auto startToken = Advance(); // text
    
    if (!Match(CHTLTokenType::LBRACE)) {
        ReportError("Expected '{' after 'text'");
        return nullptr;
    }
    
    // 收集文本内容，处理引号字符串或直接内容
    std::string content;
    
    // 检查是否是引号字符串
    if (Check(CHTLTokenType::STRING_DOUBLE) || Check(CHTLTokenType::STRING_SINGLE)) {
        content = Advance().value;
    } else {
        // 收集到下一个匹配的 } 为止
        int braceDepth = 1;
        bool lastWasSpace = true; // 避免开头空格
        
        while (!IsAtEnd() && braceDepth > 0) {
            auto token = Peek();
            
            if (token.type == CHTLTokenType::LBRACE) {
                braceDepth++;
                if (!lastWasSpace && !content.empty()) {
                    content += " ";
                    lastWasSpace = true;
                }
                content += token.value;
                lastWasSpace = false;
                Advance();
            } else if (token.type == CHTLTokenType::RBRACE) {
                braceDepth--;
                if (braceDepth > 0) {
                    if (!lastWasSpace && !content.empty()) {
                        content += " ";
                        lastWasSpace = true;
                    }
                    content += token.value;
                    lastWasSpace = false;
                }
                Advance();
            } else {
                // 智能添加空格
                bool needSpace = false;
                if (!content.empty() && !lastWasSpace) {
                    char lastChar = content.back();
                    // 检查是否需要空格
                    if (token.type == CHTLTokenType::IDENTIFIER ||
                        token.type == CHTLTokenType::LITERAL_UNQUOTED) {
                        // 前一个字符是字母数字时需要空格
                        if (std::isalnum(lastChar) || lastChar == '_') {
                            needSpace = true;
                        }
                    }
                }
                
                if (needSpace) {
                    content += " ";
                    lastWasSpace = true;
                }
                
                content += token.value;
                lastWasSpace = (token.value == " " || token.value == "\t" || token.value == "\n");
                Advance();
            }
        }
        
        // 去除首尾空白
        size_t start = content.find_first_not_of(" \t\n\r");
        size_t end = content.find_last_not_of(" \t\n\r");
        if (start != std::string::npos && end != std::string::npos) {
            content = content.substr(start, end - start + 1);
        } else if (start == std::string::npos) {
            content = "";
        }
    }
    
    // 确保关闭的大括号被消费
    if (!Match(CHTLTokenType::RBRACE)) {
        ReportError("Expected '}' to close text block");
    }
    
    auto text = std::make_shared<ast::TextNode>(content);
    text->SetLocation(startToken.line, startToken.column);
    return text;
}

std::shared_ptr<ast::ASTNode> CHTLParserV2::ParseStyle() {
    auto token = Advance(); // style
    
    if (!Match(CHTLTokenType::LBRACE)) {
        ReportError("Expected '{' after 'style'");
        return nullptr;
    }
    
    auto style = std::make_shared<ast::StyleNode>(
        CurrentContext() == Context::ELEMENT ? ast::StyleNode::LOCAL : ast::StyleNode::GLOBAL
    );
    style->SetLocation(token.line, token.column);
    
    PushContext(CurrentContext() == Context::ELEMENT ? Context::STYLE_LOCAL : Context::STYLE_GLOBAL);
    ParseStyleContent(style.get());
    PopContext();
    
    if (!Match(CHTLTokenType::RBRACE)) {
        ReportError("Expected '}' to close style block");
    }
    
    return style;
}

void CHTLParserV2::ParseLocalStyle(ast::ElementNode* element) {
    auto styleNode = std::dynamic_pointer_cast<ast::StyleNode>(ParseStyle());
    if (!styleNode) return;
    
    // 处理自动生成的类名和ID
    for (const auto& rule : styleNode->GetRules()) {
        const std::string& selector = rule.first;
        
        // 检查是否是类选择器
        if (selector.length() > 1 && selector[0] == '.') {
            std::string className = selector.substr(1);
            // 去除伪类和伪元素
            size_t colonPos = className.find(':');
            if (colonPos != std::string::npos) {
                className = className.substr(0, colonPos);
            }
            
            // 自动添加类名到元素
            if (!element->HasAttribute("class")) {
                element->AddAttribute("class", className);
                styleNode->SetAutoClassName(className);
            }
        }
        // 检查是否是ID选择器
        else if (selector.length() > 1 && selector[0] == '#') {
            std::string id = selector.substr(1);
            // 去除伪类和伪元素
            size_t colonPos = id.find(':');
            if (colonPos != std::string::npos) {
                id = id.substr(0, colonPos);
            }
            
            // 自动添加ID到元素
            if (!element->HasAttribute("id")) {
                element->AddAttribute("id", id);
                styleNode->SetAutoId(id);
            }
        }
    }
    
    element->AddChild(styleNode);
}

void CHTLParserV2::ParseStyleContent(ast::StyleNode* style) {
    while (!IsAtEnd() && !Check(CHTLTokenType::RBRACE)) {
        // 跳过分号
        if (Match(CHTLTokenType::SEMICOLON)) continue;
        
        // 局部样式块中的直接属性变成内联样式
        if (CurrentContext() == Context::STYLE_LOCAL && 
            Check(CHTLTokenType::IDENTIFIER)) {
            
            auto savedPos = m_Current;
            auto identifier = Advance();
            
            // 检查是否是CSS属性（property: value）
            if (MatchColonOrEquals()) {
                std::string value = ParseStringOrUnquoted();
                style->AddInlineProperty(identifier.value, value);
                Match(CHTLTokenType::SEMICOLON);
                continue;
            }
            
            // 不是属性，回退
            m_Current = savedPos;
        }
        
        // CSS规则
        ParseStyleRule(style);
    }
}

void CHTLParserV2::ParseStyleRule(ast::StyleNode* style) {
    // 收集选择器
    std::string selector;
    
    while (!IsAtEnd() && !Check(CHTLTokenType::LBRACE)) {
        auto token = Advance();
        selector += token.value;
        
        // 添加必要的空格
        if (token.type == CHTLTokenType::IDENTIFIER && 
            !IsAtEnd() && !Check(CHTLTokenType::LBRACE)) {
            selector += " ";
        }
    }
    
    if (!Match(CHTLTokenType::LBRACE)) {
        ReportError("Expected '{' after CSS selector");
        return;
    }
    
    // 收集属性
    std::string properties;
    int braceDepth = 1;
    
    while (!IsAtEnd() && braceDepth > 0) {
        auto token = Peek();
        
        if (token.type == CHTLTokenType::LBRACE) {
            braceDepth++;
            properties += token.value;
            Advance();
        } else if (token.type == CHTLTokenType::RBRACE) {
            braceDepth--;
            if (braceDepth > 0) {
                properties += token.value;
                Advance();
            } else {
                Advance(); // 消费最后的 }
            }
        } else {
            properties += token.value;
            if (token.type == CHTLTokenType::COLON) {
                properties += " ";
            } else if (token.type == CHTLTokenType::SEMICOLON) {
                properties += " ";
            }
            Advance();
        }
    }
    
    style->AddRule(selector, properties);
}

std::shared_ptr<ast::ASTNode> CHTLParserV2::ParseScript() {
    auto token = Advance(); // script
    
    if (!Match(CHTLTokenType::LBRACE)) {
        ReportError("Expected '{' after 'script'");
        return nullptr;
    }
    
    auto script = std::make_shared<ast::ScriptNode>(
        CurrentContext() == Context::ELEMENT ? ast::ScriptNode::LOCAL : ast::ScriptNode::GLOBAL
    );
    script->SetLocation(token.line, token.column);
    
    // 收集脚本内容
    std::string content;
    int braceDepth = 1;
    
    while (!IsAtEnd() && braceDepth > 0) {
        auto token = Peek();
        
        if (token.type == CHTLTokenType::LBRACE) {
            braceDepth++;
        } else if (token.type == CHTLTokenType::RBRACE) {
            braceDepth--;
            if (braceDepth == 0) {
                Advance(); // 消费最后的 }
                break;
            }
        }
        
        content += token.value;
        // 添加适当的空格
        if (token.type != CHTLTokenType::SEMICOLON && 
            token.type != CHTLTokenType::COMMA &&
            token.type != CHTLTokenType::DOT) {
            content += " ";
        }
        
        Advance();
    }
    
    script->SetContent(content);
    return script;
}

void CHTLParserV2::ParseLocalScript(ast::ElementNode* element) {
    element->AddChild(ParseScript());
}

std::shared_ptr<ast::ASTNode> CHTLParserV2::ParseComment() {
    auto token = Advance();
    
    ast::CommentNode::CommentType type;
    if (token.type == CHTLTokenType::COMMENT_HTML) {
        type = ast::CommentNode::HTML_COMMENT;
    } else if (token.type == CHTLTokenType::COMMENT_SINGLE) {
        type = ast::CommentNode::SINGLE_LINE;
    } else {
        type = ast::CommentNode::MULTI_LINE;
    }
    
    auto comment = std::make_shared<ast::CommentNode>(type, token.value);
    comment->SetLocation(token.line, token.column);
    return comment;
}

// 其他解析方法的简化实现...

std::shared_ptr<ast::ASTNode> CHTLParserV2::ParseTemplate() {
    auto token = Advance(); // [Template]
    
    // 期望 @Style, @Element 或 @Var
    ast::TemplateNode::TemplateType type;
    if (Match(CHTLTokenType::AT_STYLE)) {
        type = ast::TemplateNode::STYLE;
    } else if (Match(CHTLTokenType::AT_ELEMENT)) {
        type = ast::TemplateNode::ELEMENT;
    } else if (Match(CHTLTokenType::AT_VAR)) {
        type = ast::TemplateNode::VAR;
    } else {
        ReportError("Expected @Style, @Element or @Var after [Template]");
        return nullptr;
    }
    
    // 模板名称
    if (!Check(CHTLTokenType::IDENTIFIER)) {
        ReportError("Expected template name");
        return nullptr;
    }
    
    auto name = Advance();
    auto tmpl = std::make_shared<ast::TemplateNode>(type, name.value);
    tmpl->SetLocation(token.line, token.column);
    
    // 可选的参数列表
    if (Match(CHTLTokenType::LPAREN)) {
        ParseTemplateParameters(tmpl.get());
        if (!Match(CHTLTokenType::RPAREN)) {
            ReportError("Expected ')' after template parameters");
        }
    }
    
    // 模板体
    if (!Match(CHTLTokenType::LBRACE)) {
        ReportError("Expected '{' for template body");
        return tmpl;
    }
    
    PushContext(Context::TEMPLATE);
    ParseTemplateDefinition(tmpl.get());
    PopContext();
    
    if (!Match(CHTLTokenType::RBRACE)) {
        ReportError("Expected '}' to close template");
    }
    
    return tmpl;
}

// 辅助方法实现

bool CHTLParserV2::Match(CHTLTokenType type) {
    if (Check(type)) {
        Advance();
        return true;
    }
    return false;
}

bool CHTLParserV2::Check(CHTLTokenType type) const {
    if (IsAtEnd()) return false;
    return Peek().type == type;
}

bool CHTLParserV2::CheckAny(std::initializer_list<CHTLTokenType> types) const {
    for (auto type : types) {
        if (Check(type)) return true;
    }
    return false;
}

CHTLToken CHTLParserV2::Advance() {
    if (!IsAtEnd()) m_Current++;
    return Previous();
}

CHTLToken CHTLParserV2::Peek() const {
    return m_Tokens[m_Current];
}

CHTLToken CHTLParserV2::Previous() const {
    return m_Tokens[m_Current - 1];
}

bool CHTLParserV2::IsAtEnd() const {
    return m_Current >= m_Tokens.size() || Peek().type == CHTLTokenType::EOF_TOKEN;
}

bool CHTLParserV2::MatchColonOrEquals() {
    return Match(CHTLTokenType::COLON) || Match(CHTLTokenType::EQUALS);
}

std::string CHTLParserV2::ParseStringOrUnquoted() {
    if (Check(CHTLTokenType::STRING_DOUBLE) || Check(CHTLTokenType::STRING_SINGLE)) {
        return Advance().value;
    } else if (Check(CHTLTokenType::LITERAL_UNQUOTED) || Check(CHTLTokenType::IDENTIFIER)) {
        return Advance().value;
    } else if (Check(CHTLTokenType::NUMBER)) {
        return Advance().value;
    }
    
    ReportError("Expected string or literal value");
    return "";
}

void CHTLParserV2::ReportError(const std::string& message) {
    std::stringstream error;
    auto token = Peek();
    error << "Parse error at line " << token.line << ", column " << token.column 
          << ": " << message;
    m_Errors.push_back(error.str());
}

CHTLParserV2::Context CHTLParserV2::CurrentContext() const {
    return m_ContextStack.top();
}

void CHTLParserV2::PushContext(Context ctx) {
    m_ContextStack.push(ctx);
}

void CHTLParserV2::PopContext() {
    if (!m_ContextStack.empty()) {
        m_ContextStack.pop();
    }
}

// 模板参数解析
void CHTLParserV2::ParseTemplateParameters(ast::TemplateNode* tmpl) {
    while (!IsAtEnd() && !Check(CHTLTokenType::RPAREN)) {
        if (Check(CHTLTokenType::DOLLAR)) {
            Advance(); // $
            if (Check(CHTLTokenType::IDENTIFIER)) {
                tmpl->AddParameter(Advance().value);
            } else {
                ReportError("Expected parameter name after $");
            }
        } else {
            ReportError("Expected $ before parameter name");
            Advance();
        }
        
        if (!Check(CHTLTokenType::RPAREN)) {
            if (!Match(CHTLTokenType::COMMA)) {
                ReportError("Expected ',' between parameters");
            }
        }
    }
}

// 模板定义解析
void CHTLParserV2::ParseTemplateDefinition(ast::TemplateNode* tmpl) {
    while (!IsAtEnd() && !Check(CHTLTokenType::RBRACE)) {
        // 跳过分号
        if (Match(CHTLTokenType::SEMICOLON)) continue;
        
        if (tmpl->GetType() == ast::TemplateNode::STYLE) {
            // 样式模板：解析CSS属性
            if (Check(CHTLTokenType::IDENTIFIER)) {
                auto prop = Advance();
                if (MatchColonOrEquals()) {
                    std::string value = ParseStringOrUnquoted();
                    
                    // 创建一个样式节点来存储属性
                    auto styleNode = std::make_shared<ast::StyleNode>(ast::StyleNode::INLINE);
                    styleNode->AddInlineProperty(prop.value, value);
                    tmpl->AddChild(styleNode);
                    
                    Match(CHTLTokenType::SEMICOLON);
                } else {
                    ReportError("Expected ':' or '=' after property name in style template");
                }
            } else if (Check(CHTLTokenType::AT_STYLE)) {
                // 继承其他样式模板
                Advance(); // @Style
                if (Check(CHTLTokenType::IDENTIFIER)) {
                    auto name = Advance();
                    auto inheritNode = std::make_shared<ast::InheritNode>(name.value);
                    tmpl->AddChild(inheritNode);
                    Match(CHTLTokenType::SEMICOLON);
                }
            } else {
                ReportError("Unexpected token in style template");
                Advance();
            }
        } else if (tmpl->GetType() == ast::TemplateNode::ELEMENT) {
            // 元素模板：解析元素
            auto node = ParseTopLevel();
            if (node) {
                tmpl->AddChild(node);
            }
        } else if (tmpl->GetType() == ast::TemplateNode::VAR) {
            // 变量模板：解析变量定义
            if (Check(CHTLTokenType::IDENTIFIER)) {
                auto name = Advance();
                if (MatchColonOrEquals()) {
                    std::string value = ParseStringOrUnquoted();
                    
                    auto varNode = std::make_shared<ast::VarNode>(name.value);
                    varNode->SetValue(value);
                    tmpl->AddChild(varNode);
                    
                    Match(CHTLTokenType::SEMICOLON);
                }
            } else {
                ReportError("Expected variable name in var template");
                Advance();
            }
        }
    }
}

// 自定义元素解析
std::shared_ptr<ast::ASTNode> CHTLParserV2::ParseCustom() {
    auto token = Advance(); // [Custom]
    
    // 期望 @Style, @Element 或 @Var
    ast::CustomNode::CustomType type;
    if (Match(CHTLTokenType::AT_STYLE)) {
        type = ast::CustomNode::STYLE;
    } else if (Match(CHTLTokenType::AT_ELEMENT)) {
        type = ast::CustomNode::ELEMENT;
    } else if (Match(CHTLTokenType::AT_VAR)) {
        type = ast::CustomNode::VAR;
    } else {
        ReportError("Expected @Style, @Element or @Var after [Custom]");
        return nullptr;
    }
    
    // 自定义名称
    if (!Check(CHTLTokenType::IDENTIFIER)) {
        ReportError("Expected custom name");
        return nullptr;
    }
    
    auto name = Advance();
    auto custom = std::make_shared<ast::CustomNode>(type, name.value);
    custom->SetLocation(token.line, token.column);
    
    // 自定义体
    if (!Match(CHTLTokenType::LBRACE)) {
        ReportError("Expected '{' for custom body");
        return custom;
    }
    
    while (!IsAtEnd() && !Check(CHTLTokenType::RBRACE)) {
        auto node = ParseTopLevel();
        if (node) {
            custom->AddChild(node);
        }
    }
    
    if (!Match(CHTLTokenType::RBRACE)) {
        ReportError("Expected '}' to close custom");
    }
    
    return custom;
}

// Import解析
std::shared_ptr<ast::ASTNode> CHTLParserV2::ParseImport() {
    auto token = Advance(); // [Import]
    
    // 路径
    std::string path = ParseStringOrUnquoted();
    if (path.empty()) {
        ReportError("Expected import path");
        return nullptr;
    }
    
    auto import = std::make_shared<ast::ImportNode>(path);
    import->SetLocation(token.line, token.column);
    
    // 可选的类型指定
    if (CheckAny({CHTLTokenType::AT_HTML, CHTLTokenType::AT_STYLE, 
                  CHTLTokenType::AT_JAVASCRIPT, CHTLTokenType::AT_CHTL, 
                  CHTLTokenType::AT_CJMOD})) {
        auto typeToken = Advance();
        switch (typeToken.type) {
            case CHTLTokenType::AT_HTML:
                import = std::make_shared<ast::ImportNode>(path, ast::ImportNode::HTML);
                break;
            case CHTLTokenType::AT_STYLE:
                import = std::make_shared<ast::ImportNode>(path, ast::ImportNode::STYLE);
                break;
            case CHTLTokenType::AT_JAVASCRIPT:
                import = std::make_shared<ast::ImportNode>(path, ast::ImportNode::JAVASCRIPT);
                break;
            case CHTLTokenType::AT_CHTL:
                import = std::make_shared<ast::ImportNode>(path, ast::ImportNode::CHTL);
                break;
            case CHTLTokenType::AT_CJMOD:
                import = std::make_shared<ast::ImportNode>(path, ast::ImportNode::CJMOD);
                break;
            default:
                break;
        }
    }
    
    // 可选的 from 子句
    if (Match(CHTLTokenType::FROM)) {
        std::string fromPath = ParseStringOrUnquoted();
        import->SetFromClause(fromPath);
    }
    
    // 可选的具体导入项
    if (Match(CHTLTokenType::LBRACE)) {
        while (!IsAtEnd() && !Check(CHTLTokenType::RBRACE)) {
            if (Check(CHTLTokenType::IDENTIFIER)) {
                import->AddImportItem(Advance().value);
            }
            
            if (!Check(CHTLTokenType::RBRACE)) {
                if (!Match(CHTLTokenType::COMMA)) {
                    ReportError("Expected ',' between import items");
                }
            }
        }
        
        if (!Match(CHTLTokenType::RBRACE)) {
            ReportError("Expected '}' to close import items");
        }
    }
    
    Match(CHTLTokenType::SEMICOLON); // 可选的分号
    
    return import;
}

// Namespace解析
std::shared_ptr<ast::ASTNode> CHTLParserV2::ParseNamespace() {
    auto token = Advance(); // [Namespace]
    
    // 命名空间名称
    if (!Check(CHTLTokenType::IDENTIFIER)) {
        ReportError("Expected namespace name");
        return nullptr;
    }
    
    auto name = Advance();
    auto ns = std::make_shared<ast::NamespaceNode>(name.value);
    ns->SetLocation(token.line, token.column);
    
    // 可选的 from 子句
    if (Match(CHTLTokenType::FROM)) {
        std::string fromNs = ParseStringOrUnquoted();
        ns->SetFromClause(fromNs);
    }
    
    // 命名空间体
    if (!Match(CHTLTokenType::LBRACE)) {
        ReportError("Expected '{' for namespace body");
        return ns;
    }
    
    PushContext(Context::NAMESPACE);
    
    while (!IsAtEnd() && !Check(CHTLTokenType::RBRACE)) {
        auto node = ParseTopLevel();
        if (node) {
            ns->AddChild(node);
        }
    }
    
    PopContext();
    
    if (!Match(CHTLTokenType::RBRACE)) {
        ReportError("Expected '}' to close namespace");
    }
    
    return ns;
}

// Configuration解析
std::shared_ptr<ast::ASTNode> CHTLParserV2::ParseConfiguration() {
    auto token = Advance(); // [Configuration]
    
    auto config = std::make_shared<ast::ConfigurationNode>();
    config->SetLocation(token.line, token.column);
    
    if (!Match(CHTLTokenType::LBRACE)) {
        ReportError("Expected '{' for configuration body");
        return config;
    }
    
    PushContext(Context::CONFIGURATION);
    
    while (!IsAtEnd() && !Check(CHTLTokenType::RBRACE)) {
        // 跳过分号
        if (Match(CHTLTokenType::SEMICOLON)) continue;
        
        // 配置项
        if (Check(CHTLTokenType::IDENTIFIER)) {
            auto name = Advance();
            
            if (MatchColonOrEquals()) {
                std::string value = ParseStringOrUnquoted();
                config->AddConfig(name.value, value);
                Match(CHTLTokenType::SEMICOLON); // 可选的分号
            } else {
                ReportError("Expected ':' or '=' after configuration name");
            }
        } else {
            ReportError("Expected configuration name");
            Advance();
        }
    }
    
    PopContext();
    
    if (!Match(CHTLTokenType::RBRACE)) {
        ReportError("Expected '}' to close configuration");
    }
    
    return config;
}

// Origin解析
std::shared_ptr<ast::ASTNode> CHTLParserV2::ParseOrigin() {
    auto startToken = Advance(); // [Origin]
    
    ast::OriginNode::OriginType type = ast::OriginNode::HTML;
    std::string customType;
    
    // 检查类型
    if (Match(CHTLTokenType::AT_HTML)) {
        type = ast::OriginNode::HTML;
    } else if (Match(CHTLTokenType::AT_STYLE)) {
        type = ast::OriginNode::STYLE;
    } else if (Match(CHTLTokenType::AT_JAVASCRIPT)) {
        type = ast::OriginNode::JAVASCRIPT;
    } else if (Check(CHTLTokenType::IDENTIFIER)) {
        type = ast::OriginNode::CUSTOM;
        customType = Advance().value;
    }
    
    if (!Match(CHTLTokenType::LBRACE)) {
        ReportError("Expected '{' for origin content");
        return nullptr;
    }
    
    // 收集所有内容直到匹配的 }
    std::string content;
    int braceDepth = 1;
    bool inTag = false;
    bool lastWasGreater = false;
    
    while (!IsAtEnd() && braceDepth > 0) {
        auto token = Peek();
        
        if (token.type == CHTLTokenType::LBRACE) {
            braceDepth++;
            if (braceDepth > 1) content += token.value;
        } else if (token.type == CHTLTokenType::RBRACE) {
            braceDepth--;
            if (braceDepth > 0) content += token.value;
        } else {
            // 处理HTML标签中的空格
            if (token.value == "<") {
                inTag = true;
                content += token.value;
                lastWasGreater = false;
            } else if (token.value == ">") {
                inTag = false;
                content += token.value;
                lastWasGreater = true;
            } else if (inTag) {
                // 标签内部直接连接
                content += token.value;
                lastWasGreater = false;
            } else {
                // 标签外部，智能添加空格
                if (!content.empty() && !lastWasGreater && 
                    token.type == CHTLTokenType::IDENTIFIER) {
                    content += " ";
                }
                content += token.value;
                lastWasGreater = false;
            }
        }
        
        Advance();
    }
    
    auto origin = std::make_shared<ast::OriginNode>(type, content);
    origin->SetLocation(startToken.line, startToken.column);
    
    if (type == ast::OriginNode::CUSTOM) {
        origin->SetCustomTypeName(customType);
    }
    
    return origin;
}

// 特殊化解析
std::shared_ptr<ast::ASTNode> CHTLParserV2::ParseSpecialization() {
    if (Check(CHTLTokenType::DELETE)) {
        return ParseDelete();
    } else if (Check(CHTLTokenType::INSERT)) {
        return ParseInsert();
    }
    
    ReportError("Expected 'delete' or 'insert'");
    return nullptr;
}

// 继承解析
std::shared_ptr<ast::ASTNode> CHTLParserV2::ParseInherit() {
    // 继承语法: ElementName : BaseElement
    std::string base;
    std::string qualifier;
    
    if (Check(CHTLTokenType::IDENTIFIER)) {
        base = Advance().value;
        
        // 可选的限定符
        if (Check(CHTLTokenType::DOT)) {
            Advance(); // .
            if (Check(CHTLTokenType::IDENTIFIER)) {
                qualifier = Advance().value;
            }
        }
    }
    
    auto inherit = std::make_shared<ast::InheritNode>(base, qualifier);
    return inherit;
}

// 删除解析
std::shared_ptr<ast::ASTNode> CHTLParserV2::ParseDelete() {
    auto token = Advance(); // delete
    
    ast::DeleteNode::DeleteType type = ast::DeleteNode::PROPERTY;
    std::string target;
    
    // 检查删除类型
    if (Check(CHTLTokenType::IDENTIFIER)) {
        target = Advance().value;
        
        // 判断类型
        if (target == "property" || MatchColonOrEquals()) {
            type = ast::DeleteNode::PROPERTY;
            if (target == "property") {
                target = ParseStringOrUnquoted();
            }
        } else if (target == "inheritance") {
            type = ast::DeleteNode::INHERITANCE;
            target = "";
        } else {
            type = ast::DeleteNode::ELEMENT;
        }
    }
    
    auto deleteNode = std::make_shared<ast::DeleteNode>(type, target);
    deleteNode->SetLocation(token.line, token.column);
    
    Match(CHTLTokenType::SEMICOLON); // 可选的分号
    
    return deleteNode;
}

// 插入解析
std::shared_ptr<ast::ASTNode> CHTLParserV2::ParseInsert() {
    auto token = Advance(); // insert
    
    std::shared_ptr<ast::InsertNode> insertNode;
    
    // 检查插入位置
    if (Match(CHTLTokenType::IDENTIFIER)) {
        auto position = Previous().value;
        
        if (position == "before") {
            std::string target = ParseStringOrUnquoted();
            insertNode = std::make_shared<ast::InsertNode>(ast::InsertNode::BEFORE, target);
        } else if (position == "after") {
            std::string target = ParseStringOrUnquoted();
            insertNode = std::make_shared<ast::InsertNode>(ast::InsertNode::AFTER, target);
        } else {
            ReportError("Expected 'before' or 'after'");
            insertNode = std::make_shared<ast::InsertNode>(ast::InsertNode::BEFORE);
        }
    } else if (Check(CHTLTokenType::NUMBER)) {
        int index = std::stoi(Advance().value);
        insertNode = std::make_shared<ast::InsertNode>(index);
    } else {
        ReportError("Expected position specifier");
        insertNode = std::make_shared<ast::InsertNode>(ast::InsertNode::BEFORE);
    }
    
    insertNode->SetLocation(token.line, token.column);
    
    // 插入的内容
    if (Match(CHTLTokenType::LBRACE)) {
        while (!IsAtEnd() && !Check(CHTLTokenType::RBRACE)) {
            auto node = ParseTopLevel();
            if (node) {
                insertNode->AddChild(node);
            }
        }
        
        if (!Match(CHTLTokenType::RBRACE)) {
            ReportError("Expected '}' to close insert content");
        }
    }
    
    return insertNode;
}

// 变量解析
std::shared_ptr<ast::ASTNode> CHTLParserV2::ParseVar() {
    auto token = Advance(); // $
    
    if (!Check(CHTLTokenType::IDENTIFIER)) {
        ReportError("Expected variable name after $");
        return nullptr;
    }
    
    auto name = Advance();
    auto var = std::make_shared<ast::VarNode>(name.value);
    var->SetLocation(token.line, token.column);
    
    // 可选的赋值
    if (MatchColonOrEquals()) {
        // 检查是否是表达式还是简单值
        if (Check(CHTLTokenType::LBRACE)) {
            // 表达式
            var->SetExpression(ParseExpression());
        } else {
            // 简单值
            var->SetValue(ParseStringOrUnquoted());
        }
    }
    
    Match(CHTLTokenType::SEMICOLON); // 可选的分号
    
    return var;
}

// 表达式解析
std::shared_ptr<ast::ASTNode> CHTLParserV2::ParseExpression() {
    // 简化的表达式解析，暂时只支持元素
    if (Check(CHTLTokenType::IDENTIFIER)) {
        return ParseElement();
    }
    
    ReportError("Expected expression");
    return nullptr;
}

} // namespace chtl