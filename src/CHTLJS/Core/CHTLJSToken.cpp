#include "CHTLJS/Core/CHTLJSToken.h"
#include <sstream>

namespace CHTL {
namespace CHTLJS {
namespace Core {

// 静态成员初始化
std::unordered_map<std::string, TokenType> CHTLJSToken::keywordMap_;
bool CHTLJSToken::keywordMapInitialized_ = false;

CHTLJSToken::CHTLJSToken() 
    : type_(TokenType::ERROR_TOKEN), value_(""), position_(1, 1, 0) {}

CHTLJSToken::CHTLJSToken(TokenType type, const std::string& value, const TokenPosition& position)
    : type_(type), value_(value), position_(position) {
    if (!keywordMapInitialized_) {
        InitializeKeywordMap();
    }
}

bool CHTLJSToken::IsCHTLJSKeyword() const {
    return type_ == TokenType::VIR || 
           type_ == TokenType::LISTEN || 
           type_ == TokenType::DELEGATE || 
           type_ == TokenType::ANIMATE ||
           type_ == TokenType::ANIMATE;
}

bool CHTLJSToken::IsVirtualObjectToken() const {
    return type_ == TokenType::VIR;
}

bool CHTLJSToken::IsEventToken() const {
    return type_ == TokenType::LISTEN || 
           type_ == TokenType::DELEGATE ||
           type_ == TokenType::TARGET;
}

bool CHTLJSToken::IsAnimationToken() const {
    return type_ == TokenType::ANIMATE ||
           type_ == TokenType::DURATION ||
           type_ == TokenType::EASING ||
           type_ == TokenType::BEGIN ||
           type_ == TokenType::WHEN ||
           type_ == TokenType::END ||
           type_ == TokenType::LOOP ||
           type_ == TokenType::DIRECTION ||
           type_ == TokenType::DELAY ||
           type_ == TokenType::CALLBACK ||
           type_ == TokenType::AT;
}

std::string CHTLJSToken::ToString() const {
    std::ostringstream oss;
    oss << GetTokenTypeName(type_) << "('" << value_ << "') at " 
        << position_.line << ":" << position_.column;
    return oss.str();
}

std::string CHTLJSToken::GetTokenTypeName(TokenType type) {
    switch (type) {
        case TokenType::IDENTIFIER: return "IDENTIFIER";
        case TokenType::NUMBER: return "NUMBER";
        case TokenType::STRING: return "STRING";
        case TokenType::EQUAL: return "EQUAL";
        case TokenType::ARROW: return "ARROW";
        case TokenType::DOT: return "DOT";
        case TokenType::COMMA: return "COMMA";
        case TokenType::SEMICOLON: return "SEMICOLON";
        case TokenType::COLON: return "COLON";
        case TokenType::LEFT_PAREN: return "LEFT_PAREN";
        case TokenType::RIGHT_PAREN: return "RIGHT_PAREN";
        case TokenType::LEFT_BRACE: return "LEFT_BRACE";
        case TokenType::RIGHT_BRACE: return "RIGHT_BRACE";
        case TokenType::LEFT_BRACKET: return "LEFT_BRACKET";
        case TokenType::RIGHT_BRACKET: return "RIGHT_BRACKET";
        case TokenType::ENHANCED_SELECTOR: return "ENHANCED_SELECTOR";
        case TokenType::VIR: return "VIR";
        case TokenType::LISTEN: return "LISTEN";
        case TokenType::DELEGATE: return "DELEGATE";
        case TokenType::ANIMATE: return "ANIMATE";
        case TokenType::SELECTOR_START: return "SELECTOR_START";
        case TokenType::SELECTOR_END: return "SELECTOR_END";
        case TokenType::SELECTOR_CONTENT: return "SELECTOR_CONTENT";
        case TokenType::TARGET: return "TARGET";
        case TokenType::DURATION: return "DURATION";
        case TokenType::EASING: return "EASING";
        case TokenType::BEGIN: return "BEGIN";
        case TokenType::WHEN: return "WHEN";
        case TokenType::END: return "END";
        case TokenType::LOOP: return "LOOP";
        case TokenType::DIRECTION: return "DIRECTION";
        case TokenType::DELAY: return "DELAY";
        case TokenType::CALLBACK: return "CALLBACK";
        case TokenType::AT: return "AT";
        // 错误的Token已移除：I_NEVER_AWAY、VOID、VOID_STATE不是CHTL JS的内容
        case TokenType::COMMENT: return "COMMENT";
        case TokenType::WHITESPACE: return "WHITESPACE";
        case TokenType::NEWLINE: return "NEWLINE";
        case TokenType::END_OF_FILE: return "END_OF_FILE";
        case TokenType::ERROR_TOKEN: return "ERROR_TOKEN";
        default: return "UNKNOWN";
    }
}

bool CHTLJSToken::IsKeywordString(const std::string& str) {
    if (!keywordMapInitialized_) {
        InitializeKeywordMap();
    }
    return keywordMap_.find(str) != keywordMap_.end();
}

TokenType CHTLJSToken::GetKeywordTokenType(const std::string& keyword) {
    if (!keywordMapInitialized_) {
        InitializeKeywordMap();
    }
    
    auto it = keywordMap_.find(keyword);
    return (it != keywordMap_.end()) ? it->second : TokenType::IDENTIFIER;
}

void CHTLJSToken::InitializeKeywordMap() {
    keywordMap_ = {
        // CHTL JS特殊关键字
        {"vir", TokenType::VIR},
        {"listen", TokenType::LISTEN},
        {"delegate", TokenType::DELEGATE},
        {"animate", TokenType::ANIMATE},
        
        // 事件委托关键字
        {"target", TokenType::TARGET},
        
        // 动画关键字
        {"duration", TokenType::DURATION},
        {"easing", TokenType::EASING},
        {"begin", TokenType::BEGIN},
        {"when", TokenType::WHEN},
        {"end", TokenType::END},
        {"loop", TokenType::LOOP},
        {"direction", TokenType::DIRECTION},
        {"delay", TokenType::DELAY},
        {"callback", TokenType::CALLBACK},
        {"at", TokenType::AT},
        
        // 其他关键字已移除，iNeverAway、function、const、let、var、Void不是CHTL JS的内容
    };
    
    keywordMapInitialized_ = true;
}

// CHTLJSTokenStream 实现
CHTLJSTokenStream::CHTLJSTokenStream() 
    : currentPosition_(0), eofToken_(TokenType::END_OF_FILE, "", TokenPosition()) {}

CHTLJSTokenStream::CHTLJSTokenStream(const std::vector<CHTLJSToken>& tokens)
    : tokens_(tokens), currentPosition_(0), eofToken_(TokenType::END_OF_FILE, "", TokenPosition()) {}

void CHTLJSTokenStream::AddToken(const CHTLJSToken& token) {
    tokens_.push_back(token);
}

const CHTLJSToken& CHTLJSTokenStream::Current() const {
    if (currentPosition_ >= tokens_.size()) {
        return eofToken_;
    }
    return tokens_[currentPosition_];
}

void CHTLJSTokenStream::Advance() {
    if (currentPosition_ < tokens_.size()) {
        currentPosition_++;
    }
}

const CHTLJSToken& CHTLJSTokenStream::Peek(int offset) const {
    size_t peekPosition = currentPosition_ + offset;
    if (peekPosition >= tokens_.size()) {
        return eofToken_;
    }
    return tokens_[peekPosition];
}

bool CHTLJSTokenStream::IsAtEnd() const {
    return currentPosition_ >= tokens_.size();
}

void CHTLJSTokenStream::Reset() {
    currentPosition_ = 0;
}

void CHTLJSTokenStream::SetPosition(size_t position) {
    currentPosition_ = std::min(position, tokens_.size());
}

} // namespace Core
} // namespace CHTLJS
} // namespace CHTL
