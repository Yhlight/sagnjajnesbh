#include "CHTLParser.h"
#include "../AST/CHTLNodes.h"
#include <iostream>
#include <sstream>

namespace chtl {
namespace parser {

CHTLParser::CHTLParser() : current_token_(0), debug_mode_(false) {
    owned_context_ = std::make_unique<CHTLContext>();
    context_ = owned_context_.get();
}

CHTLParser::CHTLParser(CHTLContext& context) 
    : current_token_(0), context_(&context), debug_mode_(false) {
}

void CHTLParser::setTokens(const std::vector<Token>& tokens) {
    tokens_ = tokens;
    current_token_ = 0;
    clearErrors();
}

void CHTLParser::setInput(const std::string& input) {
    // 简化实现：使用词法分析器
    // 这里应该调用词法分析器，但为了简化，我们跳过
    addError("setInput方法需要词法分析器支持");
}

std::unique_ptr<ast::ASTNode> CHTLParser::parseProgram() {
    if (debug_mode_) {
        std::cout << "开始解析程序" << std::endl;
    }
    
    auto program = std::make_unique<ast::ProgramNode>();
    
    while (!isAtEnd()) {
        try {
            auto stmt = parseStatement();
            if (stmt) {
                program->addChild(std::move(stmt));
            }
        } catch (const std::exception& e) {
            addError("解析语句时发生异常: " + std::string(e.what()));
            synchronize();
        }
    }
    
    if (debug_mode_) {
        std::cout << "程序解析完成，共 " << program->getChildCount() << " 个语句" << std::endl;
    }
    
    return std::move(program);
}

std::unique_ptr<ast::ASTNode> CHTLParser::parseStatement() {
    Token token = getCurrentToken();
    
    switch (token.type) {
        case TokenType::TEMPLATE:
            return parseTemplate();
        case TokenType::CUSTOM:
            return parseCustom();
        case TokenType::IDENTIFIER:
            return parseElement();
        default:
            addError("未识别的语句类型: " + token.value);
            advance(); // 跳过未知token
            return nullptr;
    }
}

std::unique_ptr<ast::ASTNode> CHTLParser::parseTemplate() {
    if (debug_mode_) {
        std::cout << "解析模板定义" << std::endl;
    }
    
    consume(TokenType::TEMPLATE, "期望 'Template' 关键字");
    
    // 简化的模板解析
    auto template_node = std::make_unique<ast::TemplateNode>();
    
    if (check(TokenType::IDENTIFIER)) {
        template_node->name = getCurrentToken().value;
        advance();
    }
    
    return std::move(template_node);
}

std::unique_ptr<ast::ASTNode> CHTLParser::parseCustom() {
    if (debug_mode_) {
        std::cout << "解析自定义定义" << std::endl;
    }
    
    consume(TokenType::CUSTOM, "期望 'Custom' 关键字");
    
    // 简化的自定义解析
    auto custom_node = std::make_unique<ast::CustomNode>();
    
    if (check(TokenType::IDENTIFIER)) {
        custom_node->name = getCurrentToken().value;
        advance();
    }
    
    return std::move(custom_node);
}

std::unique_ptr<ast::ASTNode> CHTLParser::parseElement() {
    if (debug_mode_) {
        std::cout << "解析元素" << std::endl;
    }
    
    // 简化的元素解析
    auto element_node = std::make_unique<ast::ElementNode>();
    
    if (check(TokenType::IDENTIFIER)) {
        element_node->tag = getCurrentToken().value;
        advance();
    }
    
    return std::move(element_node);
}

bool CHTLParser::hasMoreTokens() const {
    return !isAtEnd();
}

Token CHTLParser::getCurrentToken() const {
    if (isAtEnd()) {
        return Token(TokenType::EOF_TOKEN, "", TokenPosition());
    }
    return tokens_[current_token_];
}

Token CHTLParser::peekToken(size_t offset) const {
    size_t pos = current_token_ + offset;
    if (pos >= tokens_.size()) {
        return Token(TokenType::EOF_TOKEN, "", TokenPosition());
    }
    return tokens_[pos];
}

std::vector<std::string> CHTLParser::getErrors() const {
    return errors_;
}

void CHTLParser::clearErrors() {
    errors_.clear();
}

void CHTLParser::advance() {
    if (!isAtEnd()) {
        current_token_++;
    }
}

bool CHTLParser::match(TokenType type) {
    if (check(type)) {
        advance();
        return true;
    }
    return false;
}

bool CHTLParser::check(TokenType type) const {
    if (isAtEnd()) return false;
    return getCurrentToken().type == type;
}

Token CHTLParser::consume(TokenType type, const std::string& message) {
    if (check(type)) {
        Token token = getCurrentToken();
        advance();
        return token;
    }
    
    addError(message + "，但得到了 " + getCurrentToken().value);
    return Token(TokenType::INVALID, "", TokenPosition());
}

void CHTLParser::addError(const std::string& message) {
    std::ostringstream oss;
    Token token = getCurrentToken();
    oss << "行 " << token.position.line << ", 列 " << token.position.column 
        << ": " << message;
    
    errors_.push_back(oss.str());
    
    if (debug_mode_) {
        std::cerr << "解析错误: " << oss.str() << std::endl;
    }
}

void CHTLParser::synchronize() {
    advance();
    
    while (!isAtEnd()) {
        if (getCurrentToken().type == TokenType::SEMICOLON) {
            advance();
            return;
        }
        
        switch (getCurrentToken().type) {
            case TokenType::TEMPLATE:
            case TokenType::CUSTOM:
            case TokenType::IMPORT:
            case TokenType::NAMESPACE:
                return;
            default:
                break;
        }
        
        advance();
    }
}

bool CHTLParser::isAtEnd() const {
    return current_token_ >= tokens_.size() || getCurrentToken().type == TokenType::EOF_TOKEN;
}

std::string CHTLParser::tokenTypeToString(TokenType type) const {
    return Token::typeToString(type);
}

} // namespace parser
} // namespace chtl