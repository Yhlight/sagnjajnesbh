#include "CHTLParserV3.h"
#include "../ast/CHTLASTNodesV3.h"
#include <sstream>
#include <algorithm>

namespace chtl {

CHTLParserV3::CHTLParserV3() : m_Current(0) {
    m_ContextStack.push(Context::TOP_LEVEL);
}

std::shared_ptr<ast::v3::DocumentNode> CHTLParserV3::Parse(const std::string& source, const std::string& filename) {
    m_Source = source;
    
    CHTLLexerV2 lexer;
    lexer.SetInput(source, filename);
    
    auto tokens = lexer.Tokenize();
    
    if (lexer.HasErrors()) {
        m_Errors = lexer.GetErrors();
        return nullptr;
    }
    
    return ParseTokens(tokens);
}

std::shared_ptr<ast::v3::DocumentNode> CHTLParserV3::ParseTokens(const std::vector<CHTLToken>& tokens) {
    m_Tokens = tokens;
    m_Current = 0;
    m_Errors.clear();
    
    // 重置上下文栈
    while (!m_ContextStack.empty()) {
        m_ContextStack.pop();
    }
    m_ContextStack.push(Context::TOP_LEVEL);
    
    auto document = std::make_shared<ast::v3::DocumentNode>();
    
    while (!IsAtEnd()) {
        auto node = ParseTopLevel();
        if (node) {
            document->AddChild(node);
        }
    }
    
    return document;
}

std::shared_ptr<ast::v3::ASTNode> CHTLParserV3::ParseTopLevel() {
    // 跳过顶级的分号
    while (Match(CHTLTokenType::SEMICOLON)) {}
    
    if (IsAtEnd()) return nullptr;
    
    // 注释
    if (Check(CHTLTokenType::COMMENT_HTML) || 
        Check(CHTLTokenType::COMMENT_SINGLE) || 
        Check(CHTLTokenType::COMMENT_MULTI)) {
        return ParseComment();
    }
    
    // 配置组
    if (Check(CHTLTokenType::CONFIGURATION)) {
        return ParseConfiguration();
    }
    
    // 导入
    if (Check(CHTLTokenType::IMPORT)) {
        return ParseImport();
    }
    
    // 命名空间
    if (Check(CHTLTokenType::NAMESPACE)) {
        return ParseNamespace();
    }
    
    // 模板定义
    if (Check(CHTLTokenType::TEMPLATE)) {
        return ParseTemplate();
    }
    
    // 自定义定义
    if (Check(CHTLTokenType::CUSTOM)) {
        return ParseCustom();
    }
    
    // 原始嵌入
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
    
    // 约束
    if (Check(CHTLTokenType::EXCEPT)) {
        return ParseExcept();
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

// 元素解析
std::shared_ptr<ast::v3::ASTNode> CHTLParserV3::ParseElement() {
    auto token = Advance(); // 元素名
    auto element = std::make_shared<ast::v3::ElementNode>(token.value);
    element->SetLocation(token.line, token.column);
    
    // 检查索引访问 element[index]
    if (Match(CHTLTokenType::LBRACKET)) {
        if (Check(CHTLTokenType::NUMBER)) {
            element->SetIndex(std::stoi(Advance().value));
            if (!Match(CHTLTokenType::RBRACKET)) {
                ReportError("Expected ']' after element index");
            }
        } else {
            ReportError("Expected number in element index");
        }
    }
    
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

void CHTLParserV3::ParseElementContent(ast::v3::ElementNode* element) {
    while (!IsAtEnd() && !Check(CHTLTokenType::RBRACE)) {
        // 跳过分号
        if (Match(CHTLTokenType::SEMICOLON)) continue;
        
        // 约束
        if (Check(CHTLTokenType::EXCEPT)) {
            element->AddChild(ParseExcept());
            continue;
        }
        
        // 注释
        if (Check(CHTLTokenType::COMMENT_HTML) || 
            Check(CHTLTokenType::COMMENT_SINGLE) || 
            Check(CHTLTokenType::COMMENT_MULTI)) {
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
        
        // @Style, @Element, @Var 使用
        if (CheckAny({CHTLTokenType::AT_STYLE, CHTLTokenType::AT_ELEMENT, CHTLTokenType::AT_VAR})) {
            auto type = Advance();
            
            // 检查是否是全缀名
            std::string fullName = ParseFullQualifiedName();
            
            ast::v3::CustomNode::CustomType customType;
            if (type.type == CHTLTokenType::AT_STYLE) {
                customType = ast::v3::CustomNode::STYLE;
            } else if (type.type == CHTLTokenType::AT_ELEMENT) {
                customType = ast::v3::CustomNode::ELEMENT;
            } else {
                customType = ast::v3::CustomNode::VAR;
            }
            
            auto custom = std::make_shared<ast::v3::CustomNode>(customType, fullName);
            custom->SetLocation(type.line, type.column);
            
            // 检查是否有特例化块
            if (Match(CHTLTokenType::LBRACE)) {
                custom->SetSpecialization(true);
                PushContext(Context::SPECIALIZATION);
                ParseCustomDefinition(custom.get());
                PopContext();
                Match(CHTLTokenType::RBRACE);
            }
            
            // 检查from子句
            if (Match(CHTLTokenType::FROM)) {
                // 解析命名空间
                ParseStringOrUnquoted(); // 暂时忽略
            }
            
            element->AddChild(custom);
            Match(CHTLTokenType::SEMICOLON);
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

void CHTLParserV3::ParseAttributes(ast::v3::ElementNode* element) {
    auto name = Advance(); // 属性名
    
    if (!MatchColonOrEquals()) {
        ReportError("Expected ':' or '=' after attribute name");
        return;
    }
    
    // 属性值
    std::string value = ParseStringOrUnquoted();
    element->AddAttribute(name.value, value);
    
    // 可选的分号
    Match(CHTLTokenType::SEMICOLON);
}

// 文本解析
std::shared_ptr<ast::v3::ASTNode> CHTLParserV3::ParseText() {
    auto startToken = Advance(); // text
    
    if (!Match(CHTLTokenType::LBRACE)) {
        ReportError("Expected '{' after 'text'");
        return nullptr;
    }
    
    // 收集文本内容
    std::string content;
    
    // 支持引号字符串和无修饰字面量
    if (Check(CHTLTokenType::STRING_DOUBLE) || Check(CHTLTokenType::STRING_SINGLE)) {
        content = Advance().value;
    } else {
        // 无修饰字面量模式
        int braceDepth = 1;
        
        while (!IsAtEnd() && braceDepth > 0) {
            auto token = Peek();
            
            if (token.type == CHTLTokenType::LBRACE) {
                braceDepth++;
                content += token.value;
                Advance();
            } else if (token.type == CHTLTokenType::RBRACE) {
                braceDepth--;
                if (braceDepth > 0) {
                    content += token.value;
                    Advance();
                }
            } else {
                content += token.value;
                if (m_Current + 1 < m_Tokens.size() && 
                    m_Tokens[m_Current + 1].line == token.line &&
                    m_Tokens[m_Current + 1].column > token.column + token.value.length()) {
                    content += " ";
                }
                Advance();
            }
        }
        
        // 去除首尾空白
        size_t start = content.find_first_not_of(" \t\n\r");
        size_t end = content.find_last_not_of(" \t\n\r");
        if (start != std::string::npos && end != std::string::npos) {
            content = content.substr(start, end - start + 1);
        }
    }
    
    if (!Match(CHTLTokenType::RBRACE)) {
        ReportError("Expected '}' to close text block");
    }
    
    auto text = std::make_shared<ast::v3::TextNode>(content);
    text->SetLocation(startToken.line, startToken.column);
    return text;
}

// 样式解析
std::shared_ptr<ast::v3::ASTNode> CHTLParserV3::ParseStyle() {
    auto token = Advance(); // style
    
    if (!Match(CHTLTokenType::LBRACE)) {
        ReportError("Expected '{' after 'style'");
        return nullptr;
    }
    
    auto style = std::make_shared<ast::v3::StyleNode>(
        CurrentContext() == Context::ELEMENT ? ast::v3::StyleNode::LOCAL : ast::v3::StyleNode::GLOBAL
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

void CHTLParserV3::ParseStyleContent(ast::v3::StyleNode* style) {
    while (!IsAtEnd() && !Check(CHTLTokenType::RBRACE)) {
        // 跳过分号
        if (Match(CHTLTokenType::SEMICOLON)) continue;
        
        // 处理 @Style 模板使用或继承
        if (Check(CHTLTokenType::AT_STYLE)) {
            Advance(); // @Style
            
            std::string templateName = ParseFullQualifiedName();
            
            // 检查是否有特例化
            if (Match(CHTLTokenType::LBRACE)) {
                // 特例化使用
                auto custom = std::make_shared<ast::v3::CustomNode>(ast::v3::CustomNode::STYLE, templateName);
                custom->SetSpecialization(true);
                
                PushContext(Context::SPECIALIZATION);
                ParseCustomStyleDefinition(custom.get());
                PopContext();
                
                Match(CHTLTokenType::RBRACE);
                style->AddChild(custom);
            } else {
                // 简单继承
                auto inherit = std::make_shared<ast::v3::InheritNode>(templateName);
                style->AddChild(inherit);
                Match(CHTLTokenType::SEMICOLON);
            }
            continue;
        }
        
        // 显性继承 inherit @Style Name
        if (Check(CHTLTokenType::INHERIT)) {
            Advance(); // inherit
            if (Match(CHTLTokenType::AT_STYLE)) {
                std::string templateName = ParseFullQualifiedName();
                auto inherit = std::make_shared<ast::v3::InheritNode>(templateName);
                inherit->SetExplicit(true);
                style->AddChild(inherit);
                Match(CHTLTokenType::SEMICOLON);
            } else {
                ReportError("Expected '@Style' after 'inherit'");
            }
            continue;
        }
        
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
            } else if (Match(CHTLTokenType::SEMICOLON)) {
                // 无值属性
                style->AddNoValueProperty(identifier.value);
                continue;
            }
            
            // 不是属性，回退
            m_Current = savedPos;
        }
        
        // CSS规则
        ParseStyleRule(style);
    }
}

void CHTLParserV3::ParseStyleRule(ast::v3::StyleNode* style) {
    // 收集选择器
    std::string selector;
    
    while (!IsAtEnd() && !Check(CHTLTokenType::LBRACE)) {
        auto token = Advance();
        
        if (token.type == CHTLTokenType::AMPERSAND) {
            selector += token.value;
        } else {
            if (!selector.empty() && selector.back() != '&' && selector.back() != ' ') {
                if (token.type == CHTLTokenType::IDENTIFIER) {
                    selector += " ";
                }
            }
            selector += token.value;
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
            if (token.type == CHTLTokenType::COLON || 
                token.type == CHTLTokenType::EQUALS ||
                token.type == CHTLTokenType::SEMICOLON) {
                properties += " ";
            }
            Advance();
        }
    }
    
    style->AddRule(selector, properties);
}

// 模板解析
std::shared_ptr<ast::v3::ASTNode> CHTLParserV3::ParseTemplate() {
    auto token = Advance(); // [Template]
    
    // 期望 @Style, @Element 或 @Var
    ast::v3::TemplateNode::TemplateType type;
    if (Match(CHTLTokenType::AT_STYLE)) {
        type = ast::v3::TemplateNode::STYLE;
    } else if (Match(CHTLTokenType::AT_ELEMENT)) {
        type = ast::v3::TemplateNode::ELEMENT;
    } else if (Match(CHTLTokenType::AT_VAR)) {
        type = ast::v3::TemplateNode::VAR;
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
    auto tmpl = std::make_shared<ast::v3::TemplateNode>(type, name.value);
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

void CHTLParserV3::ParseTemplateDefinition(ast::v3::TemplateNode* tmpl) {
    while (!IsAtEnd() && !Check(CHTLTokenType::RBRACE)) {
        // 跳过分号
        if (Match(CHTLTokenType::SEMICOLON)) continue;
        
        if (tmpl->GetType() == ast::v3::TemplateNode::STYLE) {
            // 样式模板
            auto styleNode = std::make_shared<ast::v3::StyleNode>(ast::v3::StyleNode::INLINE);
            
            // 解析样式属性和继承
            while (!IsAtEnd() && !Check(CHTLTokenType::RBRACE)) {
                if (Match(CHTLTokenType::SEMICOLON)) continue;
                
                // 继承
                if (Check(CHTLTokenType::AT_STYLE)) {
                    Advance();
                    std::string baseName = ParseFullQualifiedName();
                    auto inherit = std::make_shared<ast::v3::InheritNode>(baseName);
                    tmpl->AddChild(inherit);
                    Match(CHTLTokenType::SEMICOLON);
                } else if (Check(CHTLTokenType::INHERIT)) {
                    Advance();
                    Match(CHTLTokenType::AT_STYLE);
                    std::string baseName = ParseFullQualifiedName();
                    auto inherit = std::make_shared<ast::v3::InheritNode>(baseName);
                    inherit->SetExplicit(true);
                    tmpl->AddChild(inherit);
                    Match(CHTLTokenType::SEMICOLON);
                } else if (Check(CHTLTokenType::IDENTIFIER)) {
                    // 样式属性
                    auto prop = Advance();
                    if (MatchColonOrEquals()) {
                        std::string value = ParseStringOrUnquoted();
                        styleNode->AddInlineProperty(prop.value, value);
                        Match(CHTLTokenType::SEMICOLON);
                    } else if (Match(CHTLTokenType::SEMICOLON)) {
                        // 无值属性
                        styleNode->AddNoValueProperty(prop.value);
                    }
                } else {
                    ReportError("Unexpected token in style template");
                    Advance();
                }
            }
            
            if (!styleNode->GetInlineProperties().empty() || 
                !styleNode->GetNoValueProperties().empty()) {
                tmpl->AddChild(styleNode);
            }
        } else if (tmpl->GetType() == ast::v3::TemplateNode::ELEMENT) {
            // 元素模板
            auto node = ParseTopLevel();
            if (node) {
                tmpl->AddChild(node);
            }
        } else if (tmpl->GetType() == ast::v3::TemplateNode::VAR) {
            // 变量模板
            if (Check(CHTLTokenType::IDENTIFIER)) {
                auto name = Advance();
                if (MatchColonOrEquals()) {
                    std::string value = ParseStringOrUnquoted();
                    auto varNode = std::make_shared<ast::v3::VarNode>(name.value);
                    varNode->SetValue(value);
                    tmpl->AddChild(varNode);
                    Match(CHTLTokenType::SEMICOLON);
                }
            }
        }
    }
}

// 自定义解析
std::shared_ptr<ast::v3::ASTNode> CHTLParserV3::ParseCustom() {
    auto token = Advance(); // [Custom]
    
    // 期望 @Style, @Element 或 @Var
    ast::v3::CustomNode::CustomType type;
    if (Match(CHTLTokenType::AT_STYLE)) {
        type = ast::v3::CustomNode::STYLE;
    } else if (Match(CHTLTokenType::AT_ELEMENT)) {
        type = ast::v3::CustomNode::ELEMENT;
    } else if (Match(CHTLTokenType::AT_VAR)) {
        type = ast::v3::CustomNode::VAR;
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
    auto custom = std::make_shared<ast::v3::CustomNode>(type, name.value);
    custom->SetLocation(token.line, token.column);
    
    // 自定义体
    if (!Match(CHTLTokenType::LBRACE)) {
        ReportError("Expected '{' for custom body");
        return custom;
    }
    
    PushContext(Context::CUSTOM);
    ParseCustomDefinition(custom.get());
    PopContext();
    
    if (!Match(CHTLTokenType::RBRACE)) {
        ReportError("Expected '}' to close custom");
    }
    
    return custom;
}

void CHTLParserV3::ParseCustomDefinition(ast::v3::CustomNode* custom) {
    if (custom->GetType() == ast::v3::CustomNode::STYLE) {
        ParseCustomStyleDefinition(custom);
    } else if (custom->GetType() == ast::v3::CustomNode::ELEMENT) {
        ParseCustomElementDefinition(custom);
    } else if (custom->GetType() == ast::v3::CustomNode::VAR) {
        ParseCustomVarDefinition(custom);
    }
}

void CHTLParserV3::ParseCustomStyleDefinition(ast::v3::CustomNode* custom) {
    auto styleNode = std::make_shared<ast::v3::StyleNode>(ast::v3::StyleNode::INLINE);
    
    while (!IsAtEnd() && !Check(CHTLTokenType::RBRACE)) {
        if (Match(CHTLTokenType::SEMICOLON)) continue;
        
        // delete操作
        if (Check(CHTLTokenType::DELETE)) {
            auto deleteNode = ParseDelete();
            custom->AddChild(deleteNode);
            continue;
        }
        
        // 继承其他样式
        if (Check(CHTLTokenType::AT_STYLE)) {
            Advance();
            std::string templateName = ParseFullQualifiedName();
            
            if (Match(CHTLTokenType::LBRACE)) {
                // 特例化继承
                auto subCustom = std::make_shared<ast::v3::CustomNode>(ast::v3::CustomNode::STYLE, templateName);
                subCustom->SetSpecialization(true);
                
                PushContext(Context::SPECIALIZATION);
                ParseCustomStyleDefinition(subCustom.get());
                PopContext();
                
                Match(CHTLTokenType::RBRACE);
                custom->AddChild(subCustom);
            } else {
                // 简单继承
                auto inherit = std::make_shared<ast::v3::InheritNode>(templateName);
                custom->AddChild(inherit);
                Match(CHTLTokenType::SEMICOLON);
            }
            continue;
        }
        
        // 样式属性
        if (Check(CHTLTokenType::IDENTIFIER)) {
            auto prop = Advance();
            if (MatchColonOrEquals()) {
                std::string value = ParseStringOrUnquoted();
                styleNode->AddInlineProperty(prop.value, value);
                Match(CHTLTokenType::SEMICOLON);
            } else if (Match(CHTLTokenType::SEMICOLON) || Check(CHTLTokenType::COMMA)) {
                // 无值属性
                styleNode->AddNoValueProperty(prop.value);
                custom->SetHasNoValueProperties(true);
                
                // 继续解析逗号分隔的无值属性
                while (Match(CHTLTokenType::COMMA)) {
                    if (Check(CHTLTokenType::IDENTIFIER)) {
                        auto nextProp = Advance();
                        styleNode->AddNoValueProperty(nextProp.value);
                    }
                }
                Match(CHTLTokenType::SEMICOLON);
            }
        } else {
            ReportError("Unexpected token in custom style");
            Advance();
        }
    }
    
    if (!styleNode->GetInlineProperties().empty() || 
        !styleNode->GetNoValueProperties().empty()) {
        custom->AddChild(styleNode);
    }
}

// 配置解析
std::shared_ptr<ast::v3::ASTNode> CHTLParserV3::ParseConfiguration() {
    auto token = Advance(); // [Configuration]
    
    std::string name;
    
    // 检查是否有 @Config 命名
    if (Match(CHTLTokenType::AT_CONFIG)) {
        if (Check(CHTLTokenType::IDENTIFIER)) {
            name = Advance().value;
        }
    }
    
    auto config = name.empty() ? 
        std::make_shared<ast::v3::ConfigurationNode>() : 
        std::make_shared<ast::v3::ConfigurationNode>(name);
    config->SetLocation(token.line, token.column);
    
    if (!Match(CHTLTokenType::LBRACE)) {
        ReportError("Expected '{' for configuration body");
        return config;
    }
    
    PushContext(Context::CONFIGURATION);
    ParseConfigurationContent(config.get());
    PopContext();
    
    if (!Match(CHTLTokenType::RBRACE)) {
        ReportError("Expected '}' to close configuration");
    }
    
    return config;
}

void CHTLParserV3::ParseConfigurationContent(ast::v3::ConfigurationNode* config) {
    while (!IsAtEnd() && !Check(CHTLTokenType::RBRACE)) {
        if (Match(CHTLTokenType::SEMICOLON)) continue;
        
        // [Name] 配置块
        if (Match(CHTLTokenType::LBRACKET)) {
            if (Check(CHTLTokenType::IDENTIFIER) && Peek().value == "Name") {
                Advance();
                Match(CHTLTokenType::RBRACKET);
                Match(CHTLTokenType::LBRACE);
                ParseNameConfig(config);
                Match(CHTLTokenType::RBRACE);
            } else if (Check(CHTLTokenType::IDENTIFIER) && Peek().value == "OriginType") {
                Advance();
                Match(CHTLTokenType::RBRACKET);
                Match(CHTLTokenType::LBRACE);
                ParseOriginTypeConfig(config);
                Match(CHTLTokenType::RBRACE);
            }
            continue;
        }
        
        // 普通配置项
        if (Check(CHTLTokenType::IDENTIFIER)) {
            auto key = Advance().value;
            if (Match(CHTLTokenType::EQUALS)) {
                std::string value = ParseStringOrUnquoted();
                config->AddConfig(key, value);
                Match(CHTLTokenType::SEMICOLON);
            }
        } else {
            ReportError("Expected configuration key");
            Advance();
        }
    }
}

// 导入解析
std::shared_ptr<ast::v3::ASTNode> CHTLParserV3::ParseImport() {
    auto token = Advance(); // [Import]
    
    ast::v3::ImportNode::ImportType importType = ast::v3::ImportNode::AUTO;
    std::string itemName;
    bool isSpecific = false;
    
    // 检查导入类型
    if (Match(CHTLTokenType::AT_HTML)) {
        importType = ast::v3::ImportNode::HTML;
    } else if (Match(CHTLTokenType::AT_STYLE)) {
        importType = ast::v3::ImportNode::STYLE;
    } else if (Match(CHTLTokenType::AT_JAVASCRIPT)) {
        importType = ast::v3::ImportNode::JAVASCRIPT;
    } else if (Match(CHTLTokenType::AT_CHTL)) {
        importType = ast::v3::ImportNode::CHTL;
    } else if (Match(CHTLTokenType::AT_CJMOD)) {
        importType = ast::v3::ImportNode::CJMOD;
    } else if (Match(CHTLTokenType::AT_CONFIG)) {
        importType = ast::v3::ImportNode::CONFIG;
    } else if (Match(CHTLTokenType::LBRACKET)) {
        // 特定类型导入 [Custom] 或 [Template]
        isSpecific = true;
        if (Match(CHTLTokenType::CUSTOM)) {
            Match(CHTLTokenType::RBRACKET);
            // 后续解析
        } else if (Match(CHTLTokenType::TEMPLATE)) {
            Match(CHTLTokenType::RBRACKET);
            // 后续解析
        }
    }
    
    // 检查from子句位置
    std::string path;
    if (Match(CHTLTokenType::FROM)) {
        path = ParseImportPath();
    } else if (!isSpecific) {
        // 直接路径
        path = ParseImportPath();
    }
    
    auto import = std::make_shared<ast::v3::ImportNode>(path, importType);
    import->SetLocation(token.line, token.column);
    
    // as子句
    if (Match(CHTLTokenType::AS)) {
        if (Check(CHTLTokenType::IDENTIFIER)) {
            import->SetAlias(Advance().value);
        }
    }
    
    Match(CHTLTokenType::SEMICOLON);
    
    return import;
}

// 命名空间解析
std::shared_ptr<ast::v3::ASTNode> CHTLParserV3::ParseNamespace() {
    auto token = Advance(); // [Namespace]
    
    if (!Check(CHTLTokenType::IDENTIFIER)) {
        ReportError("Expected namespace name");
        return nullptr;
    }
    
    auto name = Advance();
    auto ns = std::make_shared<ast::v3::NamespaceNode>(name.value);
    ns->SetLocation(token.line, token.column);
    
    // 检查是否有 from 子句
    if (Match(CHTLTokenType::FROM)) {
        std::string fromNs = ParseIdentifierPath();
        ns->SetFromClause(fromNs);
    }
    
    // 命名空间体（可选）
    if (Match(CHTLTokenType::LBRACE)) {
        PushContext(Context::NAMESPACE);
        m_CurrentNamespace = name.value;
        
        while (!IsAtEnd() && !Check(CHTLTokenType::RBRACE)) {
            auto node = ParseTopLevel();
            if (node) {
                ns->AddChild(node);
            }
        }
        
        m_CurrentNamespace.clear();
        PopContext();
        
        Match(CHTLTokenType::RBRACE);
    }
    
    return ns;
}

// 原始嵌入解析
std::shared_ptr<ast::v3::ASTNode> CHTLParserV3::ParseOrigin() {
    auto startToken = Advance(); // [Origin]
    
    ast::v3::OriginNode::OriginType type = ast::v3::OriginNode::HTML;
    std::string customType;
    std::string name;
    
    // 检查类型
    if (Match(CHTLTokenType::AT_HTML)) {
        type = ast::v3::OriginNode::HTML;
    } else if (Match(CHTLTokenType::AT_STYLE)) {
        type = ast::v3::OriginNode::STYLE;
    } else if (Match(CHTLTokenType::AT_JAVASCRIPT)) {
        type = ast::v3::OriginNode::JAVASCRIPT;
    } else if (Check(CHTLTokenType::IDENTIFIER) && Peek().value.starts_with("@")) {
        // 自定义类型
        type = ast::v3::OriginNode::CUSTOM;
        customType = Advance().value;
    }
    
    // 检查是否有名称
    if (Check(CHTLTokenType::IDENTIFIER)) {
        name = Advance().value;
    }
    
    // 如果只是引用已命名的原始嵌入
    if (Match(CHTLTokenType::SEMICOLON)) {
        auto origin = std::make_shared<ast::v3::OriginNode>(type, "");
        origin->SetLocation(startToken.line, startToken.column);
        origin->SetName(name);
        if (type == ast::v3::OriginNode::CUSTOM) {
            origin->SetCustomTypeName(customType);
        }
        return origin;
    }
    
    if (!Match(CHTLTokenType::LBRACE)) {
        ReportError("Expected '{' for origin content");
        return nullptr;
    }
    
    // 收集原始内容
    size_t startPos = m_Current;
    int braceDepth = 1;
    
    while (!IsAtEnd() && braceDepth > 0) {
        if (Peek().type == CHTLTokenType::LBRACE) {
            braceDepth++;
        } else if (Peek().type == CHTLTokenType::RBRACE) {
            braceDepth--;
            if (braceDepth == 0) break;
        }
        Advance();
    }
    
    // 提取原始内容
    std::string content = ExtractOriginContent(startPos, m_Current);
    
    Match(CHTLTokenType::RBRACE);
    
    auto origin = std::make_shared<ast::v3::OriginNode>(type, content);
    origin->SetLocation(startToken.line, startToken.column);
    origin->SetName(name);
    if (type == ast::v3::OriginNode::CUSTOM) {
        origin->SetCustomTypeName(customType);
    }
    
    return origin;
}

// 约束解析
std::shared_ptr<ast::v3::ASTNode> CHTLParserV3::ParseExcept() {
    auto token = Advance(); // except
    
    auto except = std::make_shared<ast::v3::ExceptNode>();
    except->SetLocation(token.line, token.column);
    
    // 解析约束目标
    auto targets = ParseExceptTargets();
    for (const auto& target : targets) {
        except->AddTarget(target);
    }
    
    Match(CHTLTokenType::SEMICOLON);
    
    return except;
}

std::vector<std::string> CHTLParserV3::ParseExceptTargets() {
    std::vector<std::string> targets;
    
    do {
        std::string target;
        
        // 检查类型约束
        if (Match(CHTLTokenType::AT_HTML)) {
            target = "@Html";
        } else if (Match(CHTLTokenType::LBRACKET)) {
            // [Custom] 或 [Template]
            if (Match(CHTLTokenType::CUSTOM)) {
                target = "[Custom]";
                Match(CHTLTokenType::RBRACKET);
                
                // 可能有具体类型
                if (CheckAny({CHTLTokenType::AT_STYLE, CHTLTokenType::AT_ELEMENT, CHTLTokenType::AT_VAR})) {
                    target += " " + Advance().value;
                    if (Check(CHTLTokenType::IDENTIFIER)) {
                        target += " " + Advance().value;
                    }
                }
            } else if (Match(CHTLTokenType::TEMPLATE)) {
                target = "[Template]";
                Match(CHTLTokenType::RBRACKET);
            }
        } else if (Check(CHTLTokenType::IDENTIFIER)) {
            // 精确约束
            target = Advance().value;
        }
        
        if (!target.empty()) {
            targets.push_back(target);
        }
        
    } while (Match(CHTLTokenType::COMMA));
    
    return targets;
}

// 变量解析
std::shared_ptr<ast::v3::ASTNode> CHTLParserV3::ParseVar() {
    auto token = Advance(); // $
    
    if (!Check(CHTLTokenType::IDENTIFIER)) {
        ReportError("Expected variable name after $");
        return nullptr;
    }
    
    auto name = Advance();
    auto var = std::make_shared<ast::v3::VarNode>(name.value);
    var->SetLocation(token.line, token.column);
    
    // 可选的赋值
    if (MatchColonOrEquals()) {
        var->SetValue(ParseStringOrUnquoted());
    }
    
    Match(CHTLTokenType::SEMICOLON);
    
    return var;
}

// 变量调用解析
std::shared_ptr<ast::v3::ASTNode> CHTLParserV3::ParseVarCall() {
    // 格式: VarGroup(varName) 或 VarGroup(varName = value)
    std::string varGroup = Previous().value;
    
    if (!Match(CHTLTokenType::LPAREN)) {
        ReportError("Expected '(' after variable group name");
        return nullptr;
    }
    
    if (!Check(CHTLTokenType::IDENTIFIER)) {
        ReportError("Expected variable name");
        return nullptr;
    }
    
    std::string varName = Advance().value;
    auto varCall = std::make_shared<ast::v3::VarCallNode>(varGroup, varName);
    
    // 检查特例化
    if (Match(CHTLTokenType::EQUALS)) {
        std::string overrideValue = ParseStringOrUnquoted();
        varCall->SetOverrideValue(overrideValue);
    }
    
    if (!Match(CHTLTokenType::RPAREN)) {
        ReportError("Expected ')' after variable call");
    }
    
    return varCall;
}

// 特殊化解析
std::shared_ptr<ast::v3::ASTNode> CHTLParserV3::ParseSpecialization() {
    if (Check(CHTLTokenType::DELETE)) {
        return ParseDelete();
    } else if (Check(CHTLTokenType::INSERT)) {
        return ParseInsert();
    }
    
    ReportError("Expected 'delete' or 'insert'");
    return nullptr;
}

// 删除解析
std::shared_ptr<ast::v3::ASTNode> CHTLParserV3::ParseDelete() {
    auto token = Advance(); // delete
    
    // 判断删除类型
    std::vector<std::string> targets;
    ast::v3::DeleteNode::DeleteType type = ast::v3::DeleteNode::PROPERTY;
    
    // 收集删除目标
    do {
        if (Match(CHTLTokenType::AT_STYLE)) {
            // 删除样式继承
            type = ast::v3::DeleteNode::INHERITANCE;
            if (Check(CHTLTokenType::IDENTIFIER)) {
                targets.push_back(Advance().value);
            }
        } else if (Match(CHTLTokenType::AT_ELEMENT)) {
            // 删除元素继承
            type = ast::v3::DeleteNode::INHERITANCE;
            if (Check(CHTLTokenType::IDENTIFIER)) {
                targets.push_back(Advance().value);
            }
        } else if (Check(CHTLTokenType::IDENTIFIER)) {
            // 删除属性或元素
            auto identifier = Advance();
            
            // 检查是否有索引
            if (Match(CHTLTokenType::LBRACKET)) {
                if (Check(CHTLTokenType::NUMBER)) {
                    type = ast::v3::DeleteNode::ELEMENT;
                    targets.push_back(identifier.value + "[" + Advance().value + "]");
                    Match(CHTLTokenType::RBRACKET);
                }
            } else {
                // 默认为属性删除
                targets.push_back(identifier.value);
            }
        }
    } while (Match(CHTLTokenType::COMMA));
    
    Match(CHTLTokenType::SEMICOLON);
    
    auto deleteNode = std::make_shared<ast::v3::DeleteNode>(type, targets.empty() ? "" : targets[0]);
    deleteNode->SetLocation(token.line, token.column);
    
    // 添加所有目标
    for (size_t i = 1; i < targets.size(); ++i) {
        deleteNode->AddTarget(targets[i]);
    }
    
    return deleteNode;
}

// 插入解析
std::shared_ptr<ast::v3::ASTNode> CHTLParserV3::ParseInsert() {
    auto token = Advance(); // insert
    
    ast::v3::InsertNode::Position position = ast::v3::InsertNode::BEFORE;
    std::string target;
    
    // 解析位置
    if (Match(CHTLTokenType::BEFORE)) {
        position = ast::v3::InsertNode::BEFORE;
    } else if (Match(CHTLTokenType::AFTER)) {
        position = ast::v3::InsertNode::AFTER;
    } else if (Match(CHTLTokenType::REPLACE)) {
        position = ast::v3::InsertNode::REPLACE;
    } else if (Match(CHTLTokenType::AT)) {
        if (Match(CHTLTokenType::IDENTIFIER)) {
            std::string pos = Previous().value;
            if (pos == "top") {
                position = ast::v3::InsertNode::AT_TOP;
            } else if (pos == "bottom") {
                position = ast::v3::InsertNode::AT_BOTTOM;
            }
        }
    }
    
    // 解析目标
    if (position == ast::v3::InsertNode::BEFORE || 
        position == ast::v3::InsertNode::AFTER || 
        position == ast::v3::InsertNode::REPLACE) {
        if (Check(CHTLTokenType::IDENTIFIER)) {
            target = Advance().value;
            
            // 检查索引
            if (Match(CHTLTokenType::LBRACKET)) {
                if (Check(CHTLTokenType::NUMBER)) {
                    target += "[" + Advance().value + "]";
                    Match(CHTLTokenType::RBRACKET);
                }
            }
        }
    }
    
    auto insertNode = std::make_shared<ast::v3::InsertNode>(position, target);
    insertNode->SetLocation(token.line, token.column);
    
    // 解析插入内容
    if (Match(CHTLTokenType::LBRACE)) {
        while (!IsAtEnd() && !Check(CHTLTokenType::RBRACE)) {
            auto node = ParseTopLevel();
            if (node) {
                insertNode->AddChild(node);
            }
        }
        
        Match(CHTLTokenType::RBRACE);
    }
    
    return insertNode;
}

// 全缀名解析
std::string CHTLParserV3::ParseFullQualifiedName() {
    std::string name;
    
    if (Check(CHTLTokenType::IDENTIFIER)) {
        name = Advance().value;
        
        // 检查命名空间路径 (使用.分隔)
        while (Match(CHTLTokenType::DOT)) {
            if (Check(CHTLTokenType::IDENTIFIER)) {
                name += "." + Advance().value;
            }
        }
    }
    
    return name;
}

// 辅助方法实现...

bool CHTLParserV3::Match(CHTLTokenType type) {
    if (Check(type)) {
        Advance();
        return true;
    }
    return false;
}

bool CHTLParserV3::Check(CHTLTokenType type) const {
    if (IsAtEnd()) return false;
    return Peek().type == type;
}

bool CHTLParserV3::CheckAny(std::initializer_list<CHTLTokenType> types) const {
    for (auto type : types) {
        if (Check(type)) return true;
    }
    return false;
}

CHTLToken CHTLParserV3::Advance() {
    if (!IsAtEnd()) m_Current++;
    return Previous();
}

CHTLToken CHTLParserV3::Peek() const {
    return m_Tokens[m_Current];
}

CHTLToken CHTLParserV3::Previous() const {
    return m_Tokens[m_Current - 1];
}

bool CHTLParserV3::IsAtEnd() const {
    return m_Current >= m_Tokens.size() || Peek().type == CHTLTokenType::EOF_TOKEN;
}

bool CHTLParserV3::MatchColonOrEquals() {
    return Match(CHTLTokenType::COLON) || Match(CHTLTokenType::EQUALS);
}

std::string CHTLParserV3::ParseStringOrUnquoted() {
    if (Check(CHTLTokenType::STRING_DOUBLE) || Check(CHTLTokenType::STRING_SINGLE)) {
        return Advance().value;
    } else if (Check(CHTLTokenType::LITERAL_UNQUOTED) || 
               Check(CHTLTokenType::IDENTIFIER) || 
               Check(CHTLTokenType::NUMBER)) {
        return Advance().value;
    }
    
    ReportError("Expected string or literal value");
    return "";
}

void CHTLParserV3::ReportError(const std::string& message) {
    std::stringstream error;
    auto token = Peek();
    error << "Parse error at line " << token.line << ", column " << token.column 
          << ": " << message;
    m_Errors.push_back(error.str());
}

CHTLParserV3::Context CHTLParserV3::CurrentContext() const {
    return m_ContextStack.top();
}

void CHTLParserV3::PushContext(Context ctx) {
    m_ContextStack.push(ctx);
}

void CHTLParserV3::PopContext() {
    if (!m_ContextStack.empty()) {
        m_ContextStack.pop();
    }
}

std::string CHTLParserV3::ExtractOriginContent(size_t startPos, size_t endPos) {
    std::string content;
    
    for (size_t i = startPos; i < endPos && i < m_Tokens.size(); ++i) {
        content += m_Tokens[i].value;
        
        // 智能添加空格
        if (i + 1 < endPos && i + 1 < m_Tokens.size()) {
            auto& current = m_Tokens[i];
            auto& next = m_Tokens[i + 1];
            
            if (current.line != next.line) {
                content += "\n";
                // 保持缩进
                for (int j = 1; j < next.column; j++) {
                    content += " ";
                }
            } else if (next.column > current.column + current.value.length()) {
                // 保持原始空格
                for (size_t j = current.column + current.value.length(); j < next.column; j++) {
                    content += " ";
                }
            }
        }
    }
    
    return content;
}

// 其他辅助方法...

void CHTLParserV3::ParseLocalStyle(ast::v3::ElementNode* element) {
    auto styleNode = std::dynamic_pointer_cast<ast::v3::StyleNode>(ParseStyle());
    if (styleNode) {
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
}

void CHTLParserV3::ParseLocalScript(ast::v3::ElementNode* element) {
    element->AddChild(ParseScript());
}

std::shared_ptr<ast::v3::ASTNode> CHTLParserV3::ParseScript() {
    auto token = Advance(); // script
    
    if (!Match(CHTLTokenType::LBRACE)) {
        ReportError("Expected '{' after 'script'");
        return nullptr;
    }
    
    auto script = std::make_shared<ast::v3::ScriptNode>(
        CurrentContext() == Context::ELEMENT ? ast::v3::ScriptNode::LOCAL : ast::v3::ScriptNode::GLOBAL
    );
    script->SetLocation(token.line, token.column);
    
    // 收集脚本内容
    std::string content;
    int braceDepth = 1;
    size_t startPos = m_Current;
    
    while (!IsAtEnd() && braceDepth > 0) {
        auto token = Peek();
        
        if (token.type == CHTLTokenType::LBRACE) {
            braceDepth++;
        } else if (token.type == CHTLTokenType::RBRACE) {
            braceDepth--;
            if (braceDepth == 0) {
                break;
            }
        }
        Advance();
    }
    
    // 提取内容
    content = ExtractOriginContent(startPos, m_Current);
    
    Match(CHTLTokenType::RBRACE);
    
    script->SetContent(content);
    
    // 标记是否包含CHTL JS语法
    bool hasCHTLJS = content.find("{{") != std::string::npos ||
                     content.find("->") != std::string::npos ||
                     content.find("listen") != std::string::npos ||
                     content.find("delegate") != std::string::npos ||
                     content.find("animate") != std::string::npos ||
                     content.find("vir") != std::string::npos;
    
    script->SetHasCHTLJS(hasCHTLJS);
    
    return script;
}

std::shared_ptr<ast::v3::ASTNode> CHTLParserV3::ParseComment() {
    auto token = Advance();
    
    ast::v3::CommentNode::CommentType type;
    if (token.type == CHTLTokenType::COMMENT_HTML) {
        type = ast::v3::CommentNode::HTML_COMMENT;
    } else if (token.type == CHTLTokenType::COMMENT_SINGLE) {
        type = ast::v3::CommentNode::SINGLE_LINE;
    } else {
        type = ast::v3::CommentNode::MULTI_LINE;
    }
    
    auto comment = std::make_shared<ast::v3::CommentNode>(type, token.value);
    comment->SetLocation(token.line, token.column);
    return comment;
}

void CHTLParserV3::ParseTemplateParameters(ast::v3::TemplateNode* tmpl) {
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

void CHTLParserV3::ParseCustomElementDefinition(ast::v3::CustomNode* custom) {
    while (!IsAtEnd() && !Check(CHTLTokenType::RBRACE)) {
        auto node = ParseTopLevel();
        if (node) {
            custom->AddChild(node);
        }
    }
}

void CHTLParserV3::ParseCustomVarDefinition(ast::v3::CustomNode* custom) {
    while (!IsAtEnd() && !Check(CHTLTokenType::RBRACE)) {
        if (Match(CHTLTokenType::SEMICOLON)) continue;
        
        if (Check(CHTLTokenType::IDENTIFIER)) {
            auto name = Advance();
            if (MatchColonOrEquals()) {
                std::string value = ParseStringOrUnquoted();
                auto varNode = std::make_shared<ast::v3::VarNode>(name.value);
                varNode->SetValue(value);
                custom->AddChild(varNode);
                Match(CHTLTokenType::SEMICOLON);
            }
        } else {
            ReportError("Expected variable name in var custom");
            Advance();
        }
    }
}

void CHTLParserV3::ParseNameConfig(ast::v3::ConfigurationNode* config) {
    while (!IsAtEnd() && !Check(CHTLTokenType::RBRACE)) {
        if (Match(CHTLTokenType::SEMICOLON)) continue;
        
        if (Check(CHTLTokenType::IDENTIFIER)) {
            auto key = Advance().value;
            if (Match(CHTLTokenType::EQUALS)) {
                // 检查是否是数组形式 [value1, value2]
                if (Match(CHTLTokenType::LBRACKET)) {
                    std::vector<std::string> values;
                    while (!IsAtEnd() && !Check(CHTLTokenType::RBRACKET)) {
                        if (Check(CHTLTokenType::IDENTIFIER) || 
                            Check(CHTLTokenType::AT_STYLE) ||
                            Check(CHTLTokenType::AT_ELEMENT)) {
                            values.push_back(Advance().value);
                        }
                        Match(CHTLTokenType::COMMA);
                    }
                    Match(CHTLTokenType::RBRACKET);
                    config->AddNameConfig(key, values);
                } else {
                    // 单个值
                    std::vector<std::string> values;
                    values.push_back(ParseStringOrUnquoted());
                    config->AddNameConfig(key, values);
                }
                Match(CHTLTokenType::SEMICOLON);
            }
        }
    }
}

void CHTLParserV3::ParseOriginTypeConfig(ast::v3::ConfigurationNode* config) {
    while (!IsAtEnd() && !Check(CHTLTokenType::RBRACE)) {
        if (Match(CHTLTokenType::SEMICOLON)) continue;
        
        if (Check(CHTLTokenType::IDENTIFIER)) {
            auto key = Advance().value;
            if (Match(CHTLTokenType::EQUALS)) {
                std::string value = ParseStringOrUnquoted();
                config->AddOriginType(key, value);
                Match(CHTLTokenType::SEMICOLON);
            }
        }
    }
}

std::string CHTLParserV3::ParseImportPath() {
    std::string path;
    
    if (Check(CHTLTokenType::STRING_DOUBLE) || Check(CHTLTokenType::STRING_SINGLE)) {
        path = Advance().value;
    } else if (Check(CHTLTokenType::IDENTIFIER)) {
        // 支持点号路径
        path = ParseIdentifierPath();
    }
    
    return path;
}

std::string CHTLParserV3::ParseIdentifierPath() {
    std::string path;
    
    if (Check(CHTLTokenType::IDENTIFIER)) {
        path = Advance().value;
        
        // 支持.和/作为路径分隔符
        while (Match(CHTLTokenType::DOT) || 
               (Check(CHTLTokenType::LITERAL_UNQUOTED) && Peek().value == "/")) {
            if (Previous().type == CHTLTokenType::LITERAL_UNQUOTED) {
                path += "/";
            } else {
                path += ".";
            }
            
            if (Check(CHTLTokenType::IDENTIFIER)) {
                path += Advance().value;
            } else if (Check(CHTLTokenType::STAR)) {
                path += Advance().value;
            }
        }
    }
    
    return path;
}

} // namespace chtl