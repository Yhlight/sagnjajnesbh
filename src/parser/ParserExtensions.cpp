#include "Parser.h"
#include <sstream>

namespace chtl {
namespace parser {

// 解析样式块内容
std::vector<std::unique_ptr<ast::ASTNode>> CHTLParser::parseStyleBlockContent() {
    std::vector<std::unique_ptr<ast::ASTNode>> content;
    
    while (current_token_.type != TokenType::RBRACE && current_token_.type != TokenType::EOF_TOKEN) {
        try {
            if (current_token_.type == TokenType::DOT || current_token_.type == TokenType::IDENTIFIER) {
                // CSS选择器或属性
                if (current_token_.type == TokenType::DOT || 
                    (current_token_.type == TokenType::IDENTIFIER && peek().type == TokenType::LBRACE)) {
                    content.push_back(parseCSSSelector());
                } else if (current_token_.type == TokenType::IDENTIFIER && 
                          (peek().type == TokenType::COLON || peek().type == TokenType::EQUAL)) {
                    content.push_back(parseCSSProperty());
                }
            } else if (current_token_.type == TokenType::AMPERSAND) {
                // &选择器（上下文推导）
                content.push_back(parseCSSSelector());
            } else if (current_token_.type == TokenType::AT_STYLE) {
                // 样式组引用
                content.push_back(parseStyleInheritance());
            } else if (current_token_.type == TokenType::INHERIT) {
                // 显式继承
                content.push_back(parseStyleInheritance());
            } else if (current_token_.type == TokenType::DELETE) {
                // 删除操作
                content.push_back(parseStyleDelete());
            } else if (isVariableReferenceStart()) {
                // 变量引用
                content.push_back(parseVariableReference());
            } else if (current_token_.type == TokenType::SINGLE_COMMENT ||
                      current_token_.type == TokenType::MULTI_COMMENT ||
                      current_token_.type == TokenType::GENERATOR_COMMENT) {
                content.push_back(parseComment());
            } else if (isOriginStart()) {
                content.push_back(parseOriginBlock());
            } else {
                reportError(ParseError::UNEXPECTED_TOKEN, 
                           "样式块中意外的Token: " + current_token_.value);
                advance();
            }
        } catch (const ParseException& e) {
            errors_.push_back(e);
            synchronize();
        }
    }
    
    return content;
}

// 解析脚本块内容
std::vector<std::unique_ptr<ast::ASTNode>> CHTLParser::parseScriptBlockContent() {
    std::vector<std::unique_ptr<ast::ASTNode>> content;
    
    while (current_token_.type != TokenType::RBRACE && current_token_.type != TokenType::EOF_TOKEN) {
        try {
            if (isCHTLJSStart()) {
                // CHTL JS语法
                content.push_back(parseCHTLJSSelector());
            } else if (current_token_.type == TokenType::LISTEN) {
                content.push_back(parseCHTLJSListen());
            } else if (current_token_.type == TokenType::DELEGATE) {
                content.push_back(parseCHTLJSDelegate());
            } else if (current_token_.type == TokenType::VIR) {
                content.push_back(parseCHTLJSVir());
            } else if (isVariableReferenceStart()) {
                // 变量引用（局部脚本允许）
                content.push_back(parseVariableReference());
            } else if (current_token_.type == TokenType::SINGLE_COMMENT ||
                      current_token_.type == TokenType::MULTI_COMMENT ||
                      current_token_.type == TokenType::GENERATOR_COMMENT) {
                content.push_back(parseComment());
            } else if (isOriginStart()) {
                content.push_back(parseOriginBlock());
            } else {
                // 普通JavaScript代码，作为字面量处理
                std::string js_code;
                while (current_token_.type != TokenType::RBRACE && 
                       current_token_.type != TokenType::EOF_TOKEN &&
                       !isCHTLJSStart() && 
                       current_token_.type != TokenType::LISTEN &&
                       current_token_.type != TokenType::DELEGATE &&
                       current_token_.type != TokenType::VIR) {
                    js_code += current_token_.value + " ";
                    advance();
                }
                
                if (!js_code.empty()) {
                    content.push_back(std::make_unique<ast::LiteralNode>(
                        ast::NodeType::STRING_LITERAL, js_code, current_token_.position));
                }
            }
        } catch (const ParseException& e) {
            errors_.push_back(e);
            synchronize();
        }
    }
    
    return content;
}

// 解析模板内容
std::vector<std::unique_ptr<ast::ASTNode>> CHTLParser::parseTemplateContent() {
    std::vector<std::unique_ptr<ast::ASTNode>> content;
    
    while (current_token_.type != TokenType::RBRACE && current_token_.type != TokenType::EOF_TOKEN) {
        try {
            if (isElementStart()) {
                content.push_back(parseElement());
            } else if (current_token_.type == TokenType::IDENTIFIER && 
                      (peek().type == TokenType::COLON || peek().type == TokenType::EQUAL)) {
                // CSS属性或变量定义
                content.push_back(parseCSSProperty());
            } else if (current_token_.type == TokenType::AT_STYLE) {
                // 样式组继承
                content.push_back(parseStyleInheritance());
            } else if (current_token_.type == TokenType::INHERIT) {
                content.push_back(parseStyleInheritance());
            } else if (current_token_.type == TokenType::SINGLE_COMMENT ||
                      current_token_.type == TokenType::MULTI_COMMENT ||
                      current_token_.type == TokenType::GENERATOR_COMMENT) {
                content.push_back(parseComment());
            } else {
                reportError(ParseError::UNEXPECTED_TOKEN, 
                           "模板中意外的Token: " + current_token_.value);
                advance();
            }
        } catch (const ParseException& e) {
            errors_.push_back(e);
            synchronize();
        }
    }
    
    return content;
}

// 解析自定义内容
std::vector<std::unique_ptr<ast::ASTNode>> CHTLParser::parseCustomContent() {
    std::vector<std::unique_ptr<ast::ASTNode>> content;
    
    while (current_token_.type != TokenType::RBRACE && current_token_.type != TokenType::EOF_TOKEN) {
        try {
            if (isElementStart()) {
                content.push_back(parseElement());
            } else if (current_token_.type == TokenType::IDENTIFIER) {
                if (peek().type == TokenType::COLON || peek().type == TokenType::EQUAL) {
                    // 属性定义
                    content.push_back(parseCSSProperty());
                } else if (peek().type == TokenType::COMMA || peek().type == TokenType::SEMICOLON) {
                    // 无值样式组
                    std::string prop_name = current_token_.value;
                    advance();
                    
                    auto css_prop = std::make_unique<ast::CSSPropertyNode>(prop_name, "", current_token_.position);
                    content.push_back(std::move(css_prop));
                    
                    if (current_token_.type == TokenType::COMMA) {
                        advance();
                    } else if (current_token_.type == TokenType::SEMICOLON) {
                        advance();
                    }
                }
            } else if (current_token_.type == TokenType::AT_STYLE) {
                content.push_back(parseStyleInheritance());
            } else if (current_token_.type == TokenType::INHERIT) {
                content.push_back(parseStyleInheritance());
            } else if (current_token_.type == TokenType::DELETE) {
                content.push_back(parseStyleDelete());
            } else if (current_token_.type == TokenType::INSERT) {
                content.push_back(parseElementInsert());
            } else if (current_token_.type == TokenType::SINGLE_COMMENT ||
                      current_token_.type == TokenType::MULTI_COMMENT ||
                      current_token_.type == TokenType::GENERATOR_COMMENT) {
                content.push_back(parseComment());
            } else {
                reportError(ParseError::UNEXPECTED_TOKEN, 
                           "自定义中意外的Token: " + current_token_.value);
                advance();
            }
        } catch (const ParseException& e) {
            errors_.push_back(e);
            synchronize();
        }
    }
    
    return content;
}

// 解析CSS属性
std::unique_ptr<ast::CSSPropertyNode> CHTLParser::parseCSSProperty() {
    if (current_token_.type != TokenType::IDENTIFIER) {
        reportError(ParseError::INVALID_SYNTAX, "期望CSS属性名");
        return nullptr;
    }
    
    std::string property_name = current_token_.value;
    TokenPosition position = current_token_.position;
    advance();
    
    // 处理CE对等式
    if (current_token_.type == TokenType::COLON || current_token_.type == TokenType::EQUAL) {
        advance();
    } else {
        reportError(ParseError::MISSING_TOKEN, "期望 ':' 或 '='");
    }
    
    std::string property_value = parseStringValue();
    
    auto css_prop = std::make_unique<ast::CSSPropertyNode>(property_name, property_value, position);
    
    // 检查是否有 !important
    if (current_token_.type == TokenType::IDENTIFIER && current_token_.value == "!important") {
        css_prop->is_important = true;
        advance();
    }
    
    consume(TokenType::SEMICOLON, "期望 ';'");
    
    return css_prop;
}

// 解析CSS选择器
std::unique_ptr<ast::CSSSelectorNode> CHTLParser::parseCSSSelector() {
    std::string selector_text;
    TokenPosition position = current_token_.position;
    bool auto_generated = false;
    
    if (current_token_.type == TokenType::DOT) {
        // 类选择器
        selector_text = current_token_.value;
        advance();
        
        if (current_token_.type == TokenType::IDENTIFIER) {
            selector_text += current_token_.value;
            auto_generated = true; // 自动添加类名
            advance();
        }
    } else if (current_token_.type == TokenType::AMPERSAND) {
        // &选择器（上下文推导）
        selector_text = current_token_.value;
        advance();
        
        // 可能跟着伪类或伪元素
        if (current_token_.type == TokenType::COLON) {
            selector_text += current_token_.value;
            advance();
            
            if (current_token_.type == TokenType::IDENTIFIER) {
                selector_text += current_token_.value;
                advance();
            }
        }
    } else if (current_token_.type == TokenType::IDENTIFIER) {
        selector_text = current_token_.value;
        advance();
    }
    
    auto css_selector = std::make_unique<ast::CSSSelectorNode>(selector_text, position);
    css_selector->auto_generated = auto_generated;
    
    // 解析选择器块内容
    if (current_token_.type == TokenType::LBRACE) {
        advance();
        
        while (current_token_.type != TokenType::RBRACE && current_token_.type != TokenType::EOF_TOKEN) {
            if (current_token_.type == TokenType::IDENTIFIER && 
                (peek().type == TokenType::COLON || peek().type == TokenType::EQUAL)) {
                css_selector->addChild(parseCSSProperty());
            } else {
                reportError(ParseError::UNEXPECTED_TOKEN, 
                           "选择器块中意外的Token: " + current_token_.value);
                advance();
            }
        }
        
        consume(TokenType::RBRACE, "期望 '}'");
    }
    
    return css_selector;
}

// 解析样式继承
std::unique_ptr<ast::StyleInheritanceNode> CHTLParser::parseStyleInheritance() {
    TokenPosition position = current_token_.position;
    bool is_explicit = false;
    std::string inherited_style;
    
    if (current_token_.type == TokenType::INHERIT) {
        is_explicit = true;
        advance();
    }
    
    if (current_token_.type == TokenType::AT_STYLE) {
        advance();
        inherited_style = parseIdentifierValue();
    } else {
        reportError(ParseError::INVALID_SYNTAX, "期望样式组名");
    }
    
    consume(TokenType::SEMICOLON, "期望 ';'");
    
    return std::make_unique<ast::StyleInheritanceNode>(inherited_style, is_explicit, position);
}

// 解析样式删除
std::unique_ptr<ast::StyleDeleteNode> CHTLParser::parseStyleDelete() {
    TokenPosition position = current_token_.position;
    
    consume(TokenType::DELETE, "期望 'delete' 关键字");
    
    auto style_delete = std::make_unique<ast::StyleDeleteNode>(position);
    
    // 解析删除列表
    do {
        if (current_token_.type == TokenType::AT_STYLE) {
            // 删除继承
            advance();
            std::string inherited_name = parseIdentifierValue();
            style_delete->addDeletedInheritance("@Style " + inherited_name);
        } else if (current_token_.type == TokenType::IDENTIFIER) {
            // 删除属性
            std::string property_name = current_token_.value;
            style_delete->addDeletedProperty(property_name);
            advance();
        }
        
        if (current_token_.type == TokenType::COMMA) {
            advance();
        } else {
            break;
        }
    } while (current_token_.type != TokenType::SEMICOLON && current_token_.type != TokenType::EOF_TOKEN);
    
    consume(TokenType::SEMICOLON, "期望 ';'");
    
    return style_delete;
}

// 解析CHTL JS选择器
std::unique_ptr<ast::CHTLJSSelectorNode> CHTLParser::parseCHTLJSSelector() {
    TokenPosition position = current_token_.position;
    
    consume(TokenType::DOUBLE_LBRACE, "期望 '{{'");
    
    std::string selector_content;
    while (current_token_.type != TokenType::DOUBLE_RBRACE && current_token_.type != TokenType::EOF_TOKEN) {
        selector_content += current_token_.value;
        advance();
    }
    
    consume(TokenType::DOUBLE_RBRACE, "期望 '}}'");
    
    return std::make_unique<ast::CHTLJSSelectorNode>(selector_content, position);
}

// 解析CHTL JS Listen
std::unique_ptr<ast::CHTLJSListenNode> CHTLParser::parseCHTLJSListen() {
    TokenPosition position = current_token_.position;
    
    // 解析目标选择器
    auto target_selector = parseCHTLJSSelector();
    std::string selector_text = target_selector->selector_content;
    
    consume(TokenType::ARROW, "期望 '->'");
    consume(TokenType::LISTEN, "期望 'listen' 关键字");
    consume(TokenType::LPAREN, "期望 '('");
    consume(TokenType::LBRACE, "期望 '{'");
    
    auto listen_node = std::make_unique<ast::CHTLJSListenNode>(selector_text, position);
    
    // 解析事件处理器
    while (current_token_.type != TokenType::RBRACE && current_token_.type != TokenType::EOF_TOKEN) {
        if (current_token_.type == TokenType::IDENTIFIER) {
            std::string event_name = current_token_.value;
            advance();
            
            consume(TokenType::COLON, "期望 ':'");
            
            std::string handler_code;
            // 读取处理器代码直到逗号或结束
            while (current_token_.type != TokenType::COMMA && 
                   current_token_.type != TokenType::RBRACE && 
                   current_token_.type != TokenType::EOF_TOKEN) {
                handler_code += current_token_.value + " ";
                advance();
            }
            
            listen_node->addEventHandler(event_name, handler_code);
            
            if (current_token_.type == TokenType::COMMA) {
                advance();
            }
        } else {
            advance();
        }
    }
    
    consume(TokenType::RBRACE, "期望 '}'");
    consume(TokenType::RPAREN, "期望 ')'");
    consume(TokenType::SEMICOLON, "期望 ';'");
    
    return listen_node;
}

// 解析CHTL JS Delegate
std::unique_ptr<ast::CHTLJSDelegateNode> CHTLParser::parseCHTLJSDelegate() {
    TokenPosition position = current_token_.position;
    
    // 解析父元素选择器
    auto parent_selector = parseCHTLJSSelector();
    std::string parent_text = parent_selector->selector_content;
    
    consume(TokenType::ARROW, "期望 '->'");
    consume(TokenType::DELEGATE, "期望 'delegate' 关键字");
    consume(TokenType::LPAREN, "期望 '('");
    consume(TokenType::LBRACE, "期望 '{'");
    
    auto delegate_node = std::make_unique<ast::CHTLJSDelegateNode>(parent_text, position);
    
    // 解析目标选择器和事件处理器
    while (current_token_.type != TokenType::RBRACE && current_token_.type != TokenType::EOF_TOKEN) {
        if (current_token_.type == TokenType::IDENTIFIER && current_token_.value == "target") {
            advance();
            consume(TokenType::COLON, "期望 ':'");
            
            // 解析目标选择器
            if (isCHTLJSStart()) {
                auto target = parseCHTLJSSelector();
                delegate_node->addTargetSelector(target->selector_content);
            }
        } else if (current_token_.type == TokenType::IDENTIFIER) {
            // 事件处理器
            std::string event_name = current_token_.value;
            advance();
            consume(TokenType::COLON, "期望 ':'");
            
            std::string handler_code;
            while (current_token_.type != TokenType::COMMA && 
                   current_token_.type != TokenType::RBRACE && 
                   current_token_.type != TokenType::EOF_TOKEN) {
                handler_code += current_token_.value + " ";
                advance();
            }
            
            delegate_node->addEventHandler(event_name, handler_code);
        }
        
        if (current_token_.type == TokenType::COMMA) {
            advance();
        } else {
            break;
        }
    }
    
    consume(TokenType::RBRACE, "期望 '}'");
    consume(TokenType::RPAREN, "期望 ')'");
    consume(TokenType::SEMICOLON, "期望 ';'");
    
    return delegate_node;
}

// 解析CHTL JS Vir
std::unique_ptr<ast::CHTLJSVirNode> CHTLParser::parseCHTLJSVir() {
    TokenPosition position = current_token_.position;
    
    consume(TokenType::VIR, "期望 'vir' 关键字");
    
    std::string vir_name = parseIdentifierValue();
    
    consume(TokenType::EQUAL, "期望 '='");
    
    std::string function_name = parseIdentifierValue();
    
    auto vir_node = std::make_unique<ast::CHTLJSVirNode>(vir_name, function_name, position);
    
    consume(TokenType::LPAREN, "期望 '('");
    consume(TokenType::LBRACE, "期望 '{'");
    
    // 解析函数映射
    while (current_token_.type != TokenType::RBRACE && current_token_.type != TokenType::EOF_TOKEN) {
        if (current_token_.type == TokenType::IDENTIFIER) {
            std::string key = current_token_.value;
            advance();
            consume(TokenType::COLON, "期望 ':'");
            
            std::string func_code;
            while (current_token_.type != TokenType::COMMA && 
                   current_token_.type != TokenType::RBRACE && 
                   current_token_.type != TokenType::EOF_TOKEN) {
                func_code += current_token_.value + " ";
                advance();
            }
            
            vir_node->addFunction(key, func_code);
            
            if (current_token_.type == TokenType::COMMA) {
                advance();
            }
        } else {
            advance();
        }
    }
    
    consume(TokenType::RBRACE, "期望 '}'");
    consume(TokenType::RPAREN, "期望 ')'");
    consume(TokenType::SEMICOLON, "期望 ';'");
    
    return vir_node;
}

// 解析变量引用
std::unique_ptr<ast::VariableReferenceNode> CHTLParser::parseVariableReference() {
    if (current_token_.type != TokenType::IDENTIFIER) {
        reportError(ParseError::INVALID_SYNTAX, "期望变量组名");
        return nullptr;
    }
    
    std::string group_name = current_token_.value;
    TokenPosition position = current_token_.position;
    advance();
    
    consume(TokenType::LPAREN, "期望 '('");
    
    std::string var_name = parseIdentifierValue();
    
    consume(TokenType::RPAREN, "期望 ')'");
    
    return std::make_unique<ast::VariableReferenceNode>(group_name, var_name, position);
}

// 解析约束
std::unique_ptr<ast::ExceptConstraintNode> CHTLParser::parseExceptConstraint() {
    TokenPosition position = current_token_.position;
    
    consume(TokenType::EXCEPT, "期望 'except' 关键字");
    
    auto constraint = std::make_unique<ast::ExceptConstraintNode>(position);
    
    // 解析约束列表
    do {
        if (current_token_.type == TokenType::IDENTIFIER) {
            constraint->addExceptElement(current_token_.value);
            advance();
        } else if (current_token_.type == TokenType::AT_HTML ||
                  current_token_.type == TokenType::TEMPLATE ||
                  current_token_.type == TokenType::CUSTOM) {
            constraint->addExceptType(current_token_.value);
            advance();
        }
        
        if (current_token_.type == TokenType::COMMA) {
            advance();
        } else {
            break;
        }
    } while (current_token_.type != TokenType::SEMICOLON && current_token_.type != TokenType::EOF_TOKEN);
    
    consume(TokenType::SEMICOLON, "期望 ';'");
    
    return constraint;
}

// 解析元素插入
std::unique_ptr<ast::ElementInsertNode> CHTLParser::parseElementInsert() {
    TokenPosition position = current_token_.position;
    
    consume(TokenType::INSERT, "期望 'insert' 关键字");
    
    // 解析插入位置
    ast::ElementInsertNode::InsertPosition insert_pos;
    if (current_token_.type == TokenType::AFTER) {
        insert_pos = ast::ElementInsertNode::AFTER;
        advance();
    } else if (current_token_.type == TokenType::BEFORE) {
        insert_pos = ast::ElementInsertNode::BEFORE;
        advance();
    } else if (current_token_.type == TokenType::REPLACE) {
        insert_pos = ast::ElementInsertNode::REPLACE;
        advance();
    } else if (current_token_.type == TokenType::AT_TOP) {
        insert_pos = ast::ElementInsertNode::AT_TOP;
        advance();
    } else if (current_token_.type == TokenType::AT_BOTTOM) {
        insert_pos = ast::ElementInsertNode::AT_BOTTOM;
        advance();
    } else {
        reportError(ParseError::INVALID_SYNTAX, "期望插入位置关键字");
        insert_pos = ast::ElementInsertNode::AFTER;
    }
    
    std::string target_selector = parseIdentifierValue();
    
    auto insert_node = std::make_unique<ast::ElementInsertNode>(insert_pos, target_selector, position);
    
    consume(TokenType::LBRACE, "期望 '{'");
    
    // 解析插入内容
    while (current_token_.type != TokenType::RBRACE && current_token_.type != TokenType::EOF_TOKEN) {
        if (isElementStart()) {
            insert_node->addChild(parseElement());
        } else {
            advance();
        }
    }
    
    consume(TokenType::RBRACE, "期望 '}'");
    
    return insert_node;
}

// 辅助判断方法
bool CHTLParser::isVariableReferenceStart() const {
    return current_token_.type == TokenType::IDENTIFIER && 
           peek().type == TokenType::LPAREN;
}

} // namespace parser
} // namespace chtl