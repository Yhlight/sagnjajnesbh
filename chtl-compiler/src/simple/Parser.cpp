#include "Parser.h"
#include <sstream>

namespace chtl {
namespace simple {

Parser::Parser() : m_Current(0) {
}

std::shared_ptr<ASTNode> Parser::Parse(const std::vector<Token>& tokens) {
    m_Tokens = tokens;
    m_Current = 0;
    m_Errors.clear();
    
    auto root = std::make_shared<ASTNode>(ASTNodeType::DOCUMENT);
    
    while (!IsAtEnd()) {
        auto node = ParseContent();
        if (node) {
            root->AddChild(node);
        }
    }
    
    return root;
}

std::shared_ptr<ASTNode> Parser::ParseContent() {
    if (Check(TokenType::IDENTIFIER)) {
        Token identifier = Peek();
        
        // 检查是否是特殊块
        if (identifier.value == "text") {
            return ParseText();
        } else if (identifier.value == "style") {
            return ParseStyle();
        } else if (identifier.value == "script") {
            return ParseScript();
        } else if (identifier.value == "[Template]") {
            return ParseTemplate();
        } else {
            // 否则是元素
            return ParseElement();
        }
    }
    
    // 跳过不认识的 token
    if (!IsAtEnd()) {
        ReportError("Unexpected token: " + Peek().value);
        Advance();
    }
    return nullptr;
}

std::shared_ptr<ASTNode> Parser::ParseElement() {
    Token name = Advance(); // 获取元素名
    
    auto node = std::make_shared<ASTNode>(ASTNodeType::ELEMENT);
    node->value = name.value;
    
    if (!Match(TokenType::LBRACE)) {
        ReportError("Expected '{' after element name '" + name.value + "'");
        Synchronize();
        return nullptr;
    }
    
    // 解析内容
    while (!Check(TokenType::RBRACE) && !IsAtEnd()) {
        // 检查是否是属性
        if (Check(TokenType::IDENTIFIER)) {
            size_t savedPos = m_Current;
            Token identifier = Advance();
            
            if (Check(TokenType::COLON)) {
                // 回退并解析属性
                m_Current = savedPos;
                auto attr = ParseAttribute();
                if (!attr.first.empty()) {
                    node->AddAttribute(attr.first, attr.second);
                }
            } else {
                // 回退并解析子元素
                m_Current = savedPos;
                auto child = ParseContent();
                if (child) {
                    node->AddChild(child);
                }
            }
        } else {
            auto child = ParseContent();
            if (child) {
                node->AddChild(child);
            }
        }
    }
    
    if (!Match(TokenType::RBRACE)) {
        ReportError("Expected '}' after element content");
        return node; // 返回部分解析的节点
    }
    
    return node;
}

std::shared_ptr<ASTNode> Parser::ParseText() {
    Advance(); // 跳过 'text'
    
    if (!Match(TokenType::LBRACE)) {
        ReportError("Expected '{' after 'text'");
        return nullptr;
    }
    
    auto node = std::make_shared<ASTNode>(ASTNodeType::TEXT);
    
    // 收集文本内容，直到遇到匹配的 }
    std::string content;
    int braceCount = 1;
    
    while (!IsAtEnd() && braceCount > 0) {
        Token token = Peek();
        
        if (token.type == TokenType::LBRACE) {
            braceCount++;
            content += "{";
            Advance();
        } else if (token.type == TokenType::RBRACE) {
            braceCount--;
            if (braceCount > 0) {
                content += "}";
                Advance();
            } else {
                // 找到匹配的 }，不添加到内容中
                Advance();
            }
        } else {
            // 保留原始格式
            content += token.value;
            // 根据上下文添加适当的空格
            Token next = m_Current + 1 < m_Tokens.size() ? m_Tokens[m_Current + 1] : Token();
            if (token.type == TokenType::IDENTIFIER && 
                next.type != TokenType::COLON && 
                next.type != TokenType::SEMICOLON &&
                next.type != TokenType::RBRACE) {
                content += " ";
            }
            Advance();
        }
    }
    
    // 去除首尾空白
    size_t start = content.find_first_not_of(" \t\n\r");
    size_t end = content.find_last_not_of(" \t\n\r");
    if (start != std::string::npos && end != std::string::npos) {
        node->value = content.substr(start, end - start + 1);
    } else {
        node->value = content;
    }
    
    return node;
}

std::shared_ptr<ASTNode> Parser::ParseStyle() {
    Advance(); // 跳过 'style'
    
    if (!Match(TokenType::LBRACE)) {
        ReportError("Expected '{' after 'style'");
        return nullptr;
    }
    
    auto node = std::make_shared<ASTNode>(ASTNodeType::STYLE);
    
    // 收集CSS内容
    std::string content;
    int braceCount = 1;
    
    while (!IsAtEnd() && braceCount > 0) {
        Token token = Peek();
        
        if (token.type == TokenType::LBRACE) {
            braceCount++;
            content += " {";
            Advance();
        } else if (token.type == TokenType::RBRACE) {
            braceCount--;
            if (braceCount > 0) {
                content += "} ";
                Advance();
            } else {
                Advance();
            }
        } else if (token.type == TokenType::COLON) {
            content += ": ";
            Advance();
        } else if (token.type == TokenType::SEMICOLON) {
            content += "; ";
            Advance();
        } else {
            content += token.value;
            // 智能空格处理
            if (m_Current + 1 < m_Tokens.size()) {
                Token next = m_Tokens[m_Current + 1];
                if (token.type == TokenType::IDENTIFIER && 
                    next.type != TokenType::COLON && 
                    next.type != TokenType::LBRACE) {
                    content += " ";
                }
            }
            Advance();
        }
    }
    
    node->value = content;
    return node;
}

std::shared_ptr<ASTNode> Parser::ParseScript() {
    Advance(); // 跳过 'script'
    
    if (!Match(TokenType::LBRACE)) {
        ReportError("Expected '{' after 'script'");
        return nullptr;
    }
    
    auto node = std::make_shared<ASTNode>(ASTNodeType::SCRIPT);
    
    // 收集JavaScript内容
    std::string content;
    int braceCount = 1;
    
    while (!IsAtEnd() && braceCount > 0) {
        Token token = Peek();
        
        if (token.type == TokenType::LBRACE) {
            braceCount++;
            content += " {";
            Advance();
        } else if (token.type == TokenType::RBRACE) {
            braceCount--;
            if (braceCount > 0) {
                content += "} ";
                Advance();
            } else {
                Advance();
            }
        } else if (token.type == TokenType::SEMICOLON) {
            content += "; ";
            Advance();
        } else {
            content += token.value;
            // 智能空格处理
            if (m_Current + 1 < m_Tokens.size()) {
                Token next = m_Tokens[m_Current + 1];
                if ((token.type == TokenType::IDENTIFIER || token.type == TokenType::STRING) && 
                    next.type != TokenType::SEMICOLON && 
                    next.type != TokenType::LBRACE &&
                    next.type != TokenType::RBRACE &&
                    next.value != "(" && next.value != ")") {
                    content += " ";
                }
            }
            Advance();
        }
    }
    
    node->value = content;
    return node;
}

std::pair<std::string, std::string> Parser::ParseAttribute() {
    Token name = Advance(); // 属性名
    
    if (!Match(TokenType::COLON)) {
        ReportError("Expected ':' after attribute name '" + name.value + "'");
        return {"", ""};
    }
    
    std::string value;
    
    if (Match(TokenType::STRING)) {
        value = Previous().value;
    } else if (Match(TokenType::NUMBER)) {
        value = Previous().value;
    } else if (Match(TokenType::IDENTIFIER)) {
        value = Previous().value;
    } else {
        ReportError("Expected attribute value");
        return {"", ""};
    }
    
    Match(TokenType::SEMICOLON); // 分号是可选的
    
    return {name.value, value};
}

std::shared_ptr<ASTNode> Parser::ParseTemplate() {
    Advance(); // 跳过 '[Template]'
    
    // 跳过模板定义，暂不处理
    // 简单地消费到下一个顶级结构
    int braceCount = 0;
    
    while (!IsAtEnd()) {
        Token token = Peek();
        
        if (token.type == TokenType::LBRACE) {
            braceCount++;
            Advance();
        } else if (token.type == TokenType::RBRACE) {
            braceCount--;
            Advance();
            if (braceCount == 0) {
                break;
            }
        } else {
            Advance();
        }
    }
    
    // 返回空节点，模板定义暂时被忽略
    return nullptr;
}

std::shared_ptr<ASTNode> Parser::ParseCustomElement(const std::string& name) {
    // 暂时将自定义元素当作普通元素处理
    auto node = std::make_shared<ASTNode>(ASTNodeType::ELEMENT);
    node->value = name;
    
    if (!Match(TokenType::LBRACE)) {
        ReportError("Expected '{' after custom element name '" + name + "'");
        return nullptr;
    }
    
    // 解析内容（与普通元素相同）
    while (!Check(TokenType::RBRACE) && !IsAtEnd()) {
        if (Check(TokenType::IDENTIFIER)) {
            size_t savedPos = m_Current;
            Token identifier = Advance();
            
            if (Check(TokenType::COLON)) {
                m_Current = savedPos;
                auto attr = ParseAttribute();
                if (!attr.first.empty()) {
                    node->AddAttribute(attr.first, attr.second);
                }
            } else {
                m_Current = savedPos;
                auto child = ParseContent();
                if (child) {
                    node->AddChild(child);
                }
            }
        } else {
            auto child = ParseContent();
            if (child) {
                node->AddChild(child);
            }
        }
    }
    
    if (!Match(TokenType::RBRACE)) {
        ReportError("Expected '}' after custom element content");
    }
    
    return node;
}

// 辅助方法实现

bool Parser::Match(TokenType type) {
    if (Check(type)) {
        Advance();
        return true;
    }
    return false;
}

bool Parser::Check(TokenType type) {
    if (IsAtEnd()) return false;
    return Peek().type == type;
}

Token Parser::Advance() {
    if (!IsAtEnd()) m_Current++;
    return Previous();
}

Token Parser::Peek() {
    return m_Tokens[m_Current];
}

Token Parser::Previous() {
    return m_Tokens[m_Current - 1];
}

bool Parser::IsAtEnd() {
    return m_Current >= m_Tokens.size() || Peek().type == TokenType::EOF_TOKEN;
}

void Parser::ReportError(const std::string& message) {
    Token token = Peek();
    std::stringstream error;
    error << "Parse error at line " << token.line << ", column " << token.column 
          << ": " << message;
    m_Errors.push_back(error.str());
}

void Parser::Synchronize() {
    // 错误恢复：跳到下一个语句边界
    Advance();
    
    while (!IsAtEnd()) {
        if (Previous().type == TokenType::SEMICOLON) return;
        if (Previous().type == TokenType::RBRACE) return;
        
        switch (Peek().type) {
            case TokenType::IDENTIFIER:
                // 可能是新的元素或语句
                return;
            default:
                break;
        }
        
        Advance();
    }
}

} // namespace simple
} // namespace chtl