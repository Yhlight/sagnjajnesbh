#include "../../../include/compiler/chtljs/Lexer.h"
#include "../../../include/common/GlobalMap.h"
#include <cctype>
#include <iostream>
#include <unordered_set>
#include <algorithm>

namespace chtl {
namespace compiler {

// 静态成员初始化已在头文件中完成

CHTLJSLexer::CHTLJSLexer(const std::string& filename, 
                         const std::string& content,
                         DiagnosticCollector* diag)
    : filename_(filename), content_(content), diagnostics_(diag) {
    stateMachine_ = std::make_unique<StateMachine>();
}

// 基础方法实现 - 简化版本，后续会完善
char CHTLJSLexer::peek(size_t offset) const {
    size_t pos = position_ + offset;
    return pos < content_.size() ? content_[pos] : '\0';
}

char CHTLJSLexer::advance() {
    if (position_ >= content_.size()) return '\0';
    char ch = content_[position_++];
    updatePosition(ch);
    return ch;
}

void CHTLJSLexer::skipWhitespace() {
    while (position_ < content_.size() && std::isspace(peek())) {
        advance();
    }
}

bool CHTLJSLexer::match(const std::string& expected) {
    if (position_ + expected.size() > content_.size()) return false;
    if (content_.substr(position_, expected.size()) == expected) {
        for (size_t i = 0; i < expected.size(); ++i) advance();
        return true;
    }
    return false;
}

bool CHTLJSLexer::isAtEnd() const {
    return position_ >= content_.size();
}

TokenLocation CHTLJSLexer::getCurrentLocation() const {
    return TokenLocation(filename_, line_, column_, position_);
}

void CHTLJSLexer::updatePosition(char ch) {
    if (ch == '\n') {
        line_++;
        column_ = 1;
    } else {
        column_++;
    }
}

void CHTLJSLexer::markTokenStart() {
    tokenStart_ = position_;
}

TokenPtr CHTLJSLexer::makeToken(TokenType type) {
    std::string value = content_.substr(tokenStart_, position_ - tokenStart_);
    return std::make_shared<Token>(type, value, getCurrentLocation());
}

TokenPtr CHTLJSLexer::makeToken(TokenType type, const std::string& value) {
    return std::make_shared<Token>(type, value, getCurrentLocation());
}

// 主要接口实现
std::vector<TokenPtr> CHTLJSLexer::tokenize() {
    reset();
    std::vector<TokenPtr> tokens;
    while (!isAtEnd()) {
        skipWhitespace();
        if (isAtEnd()) break;
        auto token = scanToken();
        if (token) tokens.push_back(token);
    }
    tokens.push_back(makeToken(TokenType::EOF_TOKEN, ""));
    return tokens;
}

TokenPtr CHTLJSLexer::nextToken() {
    skipWhitespace();
    if (isAtEnd()) return makeToken(TokenType::EOF_TOKEN, "");
    return scanToken();
}

// 暂时的简化实现
TokenPtr CHTLJSLexer::scanToken() {
    markTokenStart();
    advance();
    return makeToken(TokenType::UNKNOWN);
}

// 其他方法的空实现
TokenPtr CHTLJSLexer::scanSelector() { return nullptr; }
TokenPtr CHTLJSLexer::scanVirObject() { return nullptr; }
TokenPtr CHTLJSLexer::scanArrowAccess() { return nullptr; }
TokenPtr CHTLJSLexer::scanTemplateVarCall() { return nullptr; }
TokenPtr CHTLJSLexer::scanJSIdentifier() { return nullptr; }
TokenPtr CHTLJSLexer::scanJSString(char) { return nullptr; }
TokenPtr CHTLJSLexer::scanJSNumber() { return nullptr; }
TokenPtr CHTLJSLexer::scanJSOperator() { return nullptr; }
TokenPtr CHTLJSLexer::scanJSComment() { return nullptr; }
void CHTLJSLexer::parseSelectorContent(const std::string&, std::vector<TokenPtr>&) {}
bool CHTLJSLexer::isValidSelector(const std::string&) const { return true; }
bool CHTLJSLexer::isCHTLJSFunction(const std::string&) const { return false; }
bool CHTLJSLexer::isVirObjectName(const std::string&) const { return false; }
void CHTLJSLexer::reportError(const std::string&) {}
void CHTLJSLexer::reportWarning(const std::string&) {}
TokenPtr CHTLJSLexer::peekToken(size_t) { return nullptr; }
bool CHTLJSLexer::hasMoreTokens() const { return false; }
void CHTLJSLexer::reset() {
    position_ = 0;
    line_ = 1;
    column_ = 1;
    tokenStart_ = 0;
    tokenBuffer_.clear();
    bufferPosition_ = 0;
    inCHTLJSContext_ = false;
    stateMachine_->reset();
}
void CHTLJSLexer::dumpTokens(const std::vector<TokenPtr>&) const {}

// CHTLJSLexerUtils实现
namespace CHTLJSLexerUtils {
bool isCHTLJSSyntaxStart(const std::string&, size_t) { return false; }
bool isValidSelectorSyntax(const std::string&) { return true; }
std::string extractSelectorType(const std::string&) { return ""; }
bool isCHTLJSBuiltinFunction(const std::string&) { return false; }
}

} // namespace compiler
} // namespace chtl