#include "compilers/chtl/chtl_parser.h"
#include <sstream>
#include <cctype>
#include <algorithm>

namespace chtl {

CHTLParser::CHTLParser() : current_(0) {
}

CHTLParser::~CHTLParser() = default;

std::shared_ptr<CHTLASTNode> CHTLParser::parse(const std::string& source) {
    reset();
    tokenize(source);
    
    if (tokens_.empty()) {
        return nullptr;
    }
    
    return parseDocument();
}

void CHTLParser::reset() {
    tokens_.clear();
    current_ = 0;
    errors_.clear();
}

void CHTLParser::tokenize(const std::string& source) {
    size_t pos = 0;
    size_t line = 1;
    size_t column = 1;
    
    while (pos < source.length()) {
        // 跳过空白字符
        while (pos < source.length() && std::isspace(source[pos])) {
            if (source[pos] == '\n') {
                line++;
                column = 1;
            } else {
                column++;
            }
            pos++;
        }
        
        if (pos >= source.length()) break;
        
        // 识别注释
        if (pos + 1 < source.length() && source[pos] == '/' && source[pos + 1] == '/') {
            // 单行注释
            pos += 2;
            column += 2;
            while (pos < source.length() && source[pos] != '\n') {
                pos++;
                column++;
            }
            continue;
        }
        
        if (pos + 1 < source.length() && source[pos] == '/' && source[pos + 1] == '*') {
            // 多行注释
            pos += 2;
            column += 2;
            while (pos + 1 < source.length()) {
                if (source[pos] == '*' && source[pos + 1] == '/') {
                    pos += 2;
                    column += 2;
                    break;
                }
                if (source[pos] == '\n') {
                    line++;
                    column = 1;
                } else {
                    column++;
                }
                pos++;
            }
            continue;
        }
        
        // 识别字符串字面量
        if (source[pos] == '"' || source[pos] == '\'') {
            char quote = source[pos];
            size_t startPos = pos;
            size_t startColumn = column;
            pos++;
            column++;
            
            std::string value;
            while (pos < source.length() && source[pos] != quote) {
                if (source[pos] == '\\' && pos + 1 < source.length()) {
                    pos++;
                    column++;
                }
                value += source[pos];
                if (source[pos] == '\n') {
                    line++;
                    column = 1;
                } else {
                    column++;
                }
                pos++;
            }
            
            if (pos < source.length()) {
                pos++; // 跳过结束引号
                column++;
            }
            
            TokenType type = (quote == '"') ? TokenType::STRING_LITERAL : TokenType::SINGLE_STRING_LITERAL;
            tokens_.emplace_back(type, value, value, line, startColumn);
            continue;
        }
        
        // 识别操作符和标点
        if (pos + 1 < source.length() && source[pos] == '-' && source[pos + 1] == '>') {
            tokens_.emplace_back(TokenType::ARROW, "->", line, column);
            pos += 2;
            column += 2;
            continue;
        }
        
        if (pos + 1 < source.length() && source[pos] == '{' && source[pos + 1] == '{') {
            tokens_.emplace_back(TokenType::DOUBLE_LEFT_BRACE, "{{", line, column);
            pos += 2;
            column += 2;
            continue;
        }
        
        if (pos + 1 < source.length() && source[pos] == '}' && source[pos + 1] == '}') {
            tokens_.emplace_back(TokenType::DOUBLE_RIGHT_BRACE, "}}", line, column);
            pos += 2;
            column += 2;
            continue;
        }
        
        // 单字符操作符
        char ch = source[pos];
        TokenType singleCharType = TokenType::ERROR;
        
        switch (ch) {
            case ':': singleCharType = TokenType::COLON; break;
            case ';': singleCharType = TokenType::SEMICOLON; break;
            case ',': singleCharType = TokenType::COMMA; break;
            case '.': singleCharType = TokenType::DOT; break;
            case '@': singleCharType = TokenType::AT; break;
            case '&': singleCharType = TokenType::AMPERSAND; break;
            case '=': singleCharType = TokenType::EQUALS; break;
            case '{': singleCharType = TokenType::LEFT_BRACE; break;
            case '}': singleCharType = TokenType::RIGHT_BRACE; break;
            case '(': singleCharType = TokenType::LEFT_PAREN; break;
            case ')': singleCharType = TokenType::RIGHT_PAREN; break;
            case '[': singleCharType = TokenType::LEFT_BRACKET; break;
            case ']': singleCharType = TokenType::RIGHT_BRACKET; break;
        }
        
        if (singleCharType != TokenType::ERROR) {
            tokens_.emplace_back(singleCharType, std::string(1, ch), line, column);
            pos++;
            column++;
            continue;
        }
        
        // 识别标识符和关键字
        if (std::isalpha(ch) || ch == '_') {
            size_t startPos = pos;
            size_t startColumn = column;
            std::string identifier;
            
            while (pos < source.length() && (std::isalnum(source[pos]) || source[pos] == '_' || source[pos] == '-')) {
                identifier += source[pos];
                pos++;
                column++;
            }
            
            // 检查是否是关键字
            TokenType type = TokenType::IDENTIFIER;
            if (identifier == "text") type = TokenType::KEYWORD_TEXT;
            else if (identifier == "style") type = TokenType::KEYWORD_STYLE;
            else if (identifier == "script") type = TokenType::KEYWORD_SCRIPT;
            else if (identifier == "inherit") type = TokenType::KEYWORD_INHERIT;
            else if (identifier == "delete") type = TokenType::KEYWORD_DELETE;
            else if (identifier == "insert") type = TokenType::KEYWORD_INSERT;
            else if (identifier == "except") type = TokenType::KEYWORD_EXCEPT;
            else if (identifier == "from") type = TokenType::KEYWORD_FROM;
            else if (identifier == "as") type = TokenType::KEYWORD_AS;
            else if (identifier == "vir") type = TokenType::KEYWORD_VIR;
            
            tokens_.emplace_back(type, identifier, line, startColumn);
            continue;
        }
        
        // 识别数字
        if (std::isdigit(ch)) {
            size_t startPos = pos;
            size_t startColumn = column;
            std::string number;
            bool hasDecimal = false;
            
            while (pos < source.length() && (std::isdigit(source[pos]) || source[pos] == '.')) {
                if (source[pos] == '.') {
                    if (hasDecimal) break;
                    hasDecimal = true;
                }
                number += source[pos];
                pos++;
                column++;
            }
            
            double value = std::stod(number);
            tokens_.emplace_back(TokenType::NUMBER, number, value, line, startColumn);
            continue;
        }
        
        // 无法识别的字符
        error(Token(TokenType::ERROR, std::string(1, ch), line, column), "Unexpected character");
        pos++;
        column++;
    }
    
    // 添加EOF标记
    tokens_.emplace_back(TokenType::EOF_TOKEN, "", line, column);
}

std::shared_ptr<CHTLASTNode> CHTLParser::parseDocument() {
    auto document = std::make_shared<DocumentNode>();
    
    while (!isAtEnd()) {
        if (match(TokenType::KEYWORD_TEXT)) {
            document->addChild(parseText());
        } else if (match(TokenType::LEFT_BRACKET)) {
            // 处理[Template], [Custom], [Origin]等
            if (match(TokenType::IDENTIFIER)) {
                std::string keyword = previous().lexeme;
                consume(TokenType::RIGHT_BRACKET, "Expected ']' after " + keyword);
                
                if (keyword == "Template") {
                    document->addChild(parseTemplate());
                } else if (keyword == "Custom") {
                    document->addChild(parseCustom());
                } else if (keyword == "Origin") {
                    document->addChild(parseOrigin());
                } else if (keyword == "Import") {
                    document->addChild(parseImport());
                } else if (keyword == "Namespace") {
                    document->addChild(parseNamespace());
                } else if (keyword == "Configuration") {
                    document->addChild(parseConfiguration());
                }
            }
        } else if (check(TokenType::IDENTIFIER)) {
            // 元素
            document->addChild(parseElement());
        } else {
            advance(); // 跳过未知的token
        }
    }
    
    return document;
}

std::shared_ptr<CHTLASTNode> CHTLParser::parseElement() {
    Token tagName = advance();
    auto element = std::make_shared<ElementNode>(tagName.lexeme);
    
    consume(TokenType::LEFT_BRACE, "Expected '{' after element name");
    
    // 解析元素内容
    while (!check(TokenType::RIGHT_BRACE) && !isAtEnd()) {
        if (match(TokenType::IDENTIFIER)) {
            Token attrName = previous();
            
            if (match(TokenType::COLON) || match(TokenType::EQUALS)) {
                // 属性
                if (check(TokenType::STRING_LITERAL) || check(TokenType::SINGLE_STRING_LITERAL) || 
                    check(TokenType::UNQUOTED_LITERAL) || check(TokenType::IDENTIFIER)) {
                    Token attrValue = advance();
                    element->addAttribute(attrName.lexeme, attrValue.lexeme);
                    consume(TokenType::SEMICOLON, "Expected ';' after attribute value");
                }
            } else {
                // 可能是子元素
                current_--; // 退回标识符
                element->addChild(parseElement());
            }
        } else if (match(TokenType::KEYWORD_TEXT)) {
            element->addChild(parseText());
        } else if (match(TokenType::KEYWORD_STYLE)) {
            element->addChild(parseStyleBlock());
        } else if (match(TokenType::KEYWORD_SCRIPT)) {
            element->addChild(parseScriptBlock());
        } else {
            advance(); // 跳过未知内容
        }
    }
    
    consume(TokenType::RIGHT_BRACE, "Expected '}' after element content");
    
    return element;
}

std::shared_ptr<CHTLASTNode> CHTLParser::parseText() {
    consume(TokenType::LEFT_BRACE, "Expected '{' after 'text'");
    
    std::string content;
    
    // 跳过空白
    while (!isAtEnd() && (peek().type == TokenType::IDENTIFIER && peek().lexeme.empty())) {
        advance();
    }
    
    if (check(TokenType::STRING_LITERAL) || check(TokenType::SINGLE_STRING_LITERAL)) {
        content = std::get<std::string>(advance().literal);
    } else if (check(TokenType::IDENTIFIER)) {
        // 无引号字面量
        content = advance().lexeme;
    }
    
    consume(TokenType::RIGHT_BRACE, "Expected '}' after text content");
    
    return std::make_shared<TextNode>(content);
}

std::shared_ptr<CHTLASTNode> CHTLParser::parseStyleBlock() {
    consume(TokenType::LEFT_BRACE, "Expected '{' after 'style'");
    
    std::stringstream styleContent;
    int braceDepth = 1;
    
    while (braceDepth > 0 && !isAtEnd()) {
        if (check(TokenType::LEFT_BRACE)) {
            braceDepth++;
        } else if (check(TokenType::RIGHT_BRACE)) {
            braceDepth--;
            if (braceDepth == 0) {
                advance(); // 消费最后的 }
                break;
            }
        }
        
        Token token = advance();
        styleContent << token.lexeme << " ";
    }
    
    return std::make_shared<StyleBlockNode>(styleContent.str(), true);
}

std::shared_ptr<CHTLASTNode> CHTLParser::parseScriptBlock() {
    // 暂时简化实现
    consume(TokenType::LEFT_BRACE, "Expected '{' after 'script'");
    
    std::stringstream scriptContent;
    int braceDepth = 1;
    
    while (braceDepth > 0 && !isAtEnd()) {
        if (check(TokenType::LEFT_BRACE)) {
            braceDepth++;
        } else if (check(TokenType::RIGHT_BRACE)) {
            braceDepth--;
            if (braceDepth == 0) {
                advance();
                break;
            }
        }
        
        Token token = advance();
        scriptContent << token.lexeme << " ";
    }
    
    // 创建一个脚本节点（需要扩展AST定义）
    return std::make_shared<StyleBlockNode>(scriptContent.str(), false); // 临时使用StyleBlockNode
}

std::shared_ptr<CHTLASTNode> CHTLParser::parseTemplate() {
    // 简化实现
    if (match(TokenType::AT)) {
        Token type = advance(); // Style, Element, Var
        Token name = advance(); // 模板名称
        
        auto templateNode = std::make_shared<TemplateDefinitionNode>(
            name.lexeme, 
            type.lexeme == "Style" ? TemplateDefinitionNode::TemplateType::STYLE :
            type.lexeme == "Element" ? TemplateDefinitionNode::TemplateType::ELEMENT :
            TemplateDefinitionNode::TemplateType::VAR
        );
        
        consume(TokenType::LEFT_BRACE, "Expected '{' after template name");
        
        // 解析模板内容
        while (!check(TokenType::RIGHT_BRACE) && !isAtEnd()) {
            advance(); // 简化：跳过内容
        }
        
        consume(TokenType::RIGHT_BRACE, "Expected '}' after template content");
        
        return templateNode;
    }
    
    return nullptr;
}

std::shared_ptr<CHTLASTNode> CHTLParser::parseCustom() {
    // 简化实现，类似parseTemplate
    return parseTemplate();
}

std::shared_ptr<CHTLASTNode> CHTLParser::parseImport() {
    // 简化实现
    return std::make_shared<DocumentNode>();
}

std::shared_ptr<CHTLASTNode> CHTLParser::parseNamespace() {
    // 简化实现
    return std::make_shared<DocumentNode>();
}

std::shared_ptr<CHTLASTNode> CHTLParser::parseConfiguration() {
    // 简化实现
    return std::make_shared<DocumentNode>();
}

std::shared_ptr<CHTLASTNode> CHTLParser::parseOrigin() {
    // 简化实现
    return std::make_shared<DocumentNode>();
}

Token& CHTLParser::peek(size_t offset) {
    size_t pos = current_ + offset;
    if (pos >= tokens_.size()) {
        return tokens_.back(); // 返回EOF token
    }
    return tokens_[pos];
}

Token& CHTLParser::previous() {
    return tokens_[current_ - 1];
}

Token CHTLParser::advance() {
    if (!isAtEnd()) current_++;
    return previous();
}

bool CHTLParser::check(TokenType type) {
    if (isAtEnd()) return false;
    return peek().type == type;
}

bool CHTLParser::match(const std::vector<TokenType>& types) {
    for (auto type : types) {
        if (check(type)) {
            advance();
            return true;
        }
    }
    return false;
}

bool CHTLParser::match(TokenType type) {
    if (check(type)) {
        advance();
        return true;
    }
    return false;
}

void CHTLParser::consume(TokenType type, const std::string& message) {
    if (check(type)) {
        advance();
        return;
    }
    
    error(peek(), message);
}

bool CHTLParser::isAtEnd() {
    return peek().type == TokenType::EOF_TOKEN;
}

void CHTLParser::error(const Token& token, const std::string& message) {
    std::stringstream ss;
    ss << "Error at line " << token.line << ", column " << token.column << ": " << message;
    errors_.push_back(ss.str());
}

void CHTLParser::synchronize() {
    advance();
    
    while (!isAtEnd()) {
        if (previous().type == TokenType::SEMICOLON) return;
        
        switch (peek().type) {
            case TokenType::KEYWORD_TEXT:
            case TokenType::KEYWORD_STYLE:
            case TokenType::KEYWORD_SCRIPT:
            case TokenType::IDENTIFIER:
                return;
            default:
                break;
        }
        
        advance();
    }
}

} // namespace chtl