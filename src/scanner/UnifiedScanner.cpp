#include "../../include/scanner/UnifiedScanner.h"
#include <algorithm>
#include <regex>
#include <sstream>
#include <iostream>
#include <cctype>

namespace chtl {

// UnifiedScanner实现
UnifiedScanner::UnifiedScanner(const std::string& filename, 
                             const std::string& content,
                             DiagnosticCollector* diag)
    : filename_(filename), content_(content), diagnostics_(diag) {
    buffer_.reserve(config_.initialBufferSize);
}

char UnifiedScanner::peek(size_t offset) const {
    size_t pos = position_ + offset;
    return pos < content_.size() ? content_[pos] : '\0';
}

char UnifiedScanner::advance() {
    if (position_ >= content_.size()) {
        return '\0';
    }
    
    char ch = content_[position_++];
    updatePosition(ch);
    return ch;
}

void UnifiedScanner::skipWhitespace() {
    while (position_ < content_.size()) {
        char ch = peek();
        if (ch == ' ' || ch == '\t' || ch == '\r') {
            advance();
        } else if (ch == '\n') {
            advance();
        } else {
            break;
        }
    }
}

bool UnifiedScanner::match(const std::string& text) const {
    if (position_ + text.size() > content_.size()) {
        return false;
    }
    
    return content_.substr(position_, text.size()) == text;
}

bool UnifiedScanner::matchRegex(const std::string& pattern) const {
    if (position_ >= content_.size()) {
        return false;
    }
    
    std::regex re(pattern);
    std::string remaining = content_.substr(position_);
    std::smatch match;
    
    return std::regex_search(remaining, match, re) && match.position() == 0;
}

SourceLocation UnifiedScanner::getCurrentLocation() const {
    return SourceLocation(filename_, line_, column_, position_);
}

void UnifiedScanner::updatePosition(char ch) {
    if (ch == '\n') {
        line_++;
        column_ = 1;
    } else {
        column_++;
    }
}

std::vector<CodeFragmentPtr> UnifiedScanner::scan() {
    reset();
    std::vector<CodeFragmentPtr> fragments;
    
    while (!this->isAtEnd()) {
        skipWhitespace();
        if (this->isAtEnd()) break;
        
        auto fragment = scanNext();
        if (fragment) {
            fragments.push_back(fragment);
        }
    }
    
    return fragments;
}

CodeFragmentPtr UnifiedScanner::scanNext() {
    if (this->isAtEnd()) {
        return nullptr;
    }
    
    skipWhitespace();
    if (this->isAtEnd()) {
        return nullptr;
    }
    
    SourceLocation startLoc = getCurrentLocation();
    FragmentType type = identifyFragmentType();
    
    CodeFragmentPtr fragment = nullptr;
    
    switch (type) {
        case FragmentType::COMMENT:
            fragment = extractCommentFragment();
            break;
        case FragmentType::CHTL:
            fragment = extractCHTLFragment();
            break;
        case FragmentType::CHTL_JS:
            fragment = extractCHTLJSFragment();
            break;
        case FragmentType::CSS:
            fragment = extractCSSFragment();
            break;
        case FragmentType::JAVASCRIPT:
            fragment = extractJavaScriptFragment();
            break;
        case FragmentType::HTML:
            fragment = extractHTMLFragment();
            break;
        default:
            // 默认作为CHTL处理
            fragment = extractCHTLFragment();
            break;
    }
    
    // 验证片段上下文
    if (fragment && !validateFragmentContext(fragment)) {
        if (diagnostics_) {
            diagnostics_->warning("片段上下文验证失败", fragment->location);
        }
    }
    
    return fragment;
}

bool UnifiedScanner::hasMore() const {
    size_t tempPos = position_;
    while (tempPos < content_.size()) {
        char ch = content_[tempPos];
        if (ch != ' ' && ch != '\t' && ch != '\r' && ch != '\n') {
            return true;
        }
        tempPos++;
    }
    return false;
}

void UnifiedScanner::reset() {
    position_ = 0;
    line_ = 1;
    column_ = 1;
    fragmentQueue_ = std::queue<CodeFragmentPtr>();
    typeStack_.clear();
}

FragmentType UnifiedScanner::identifyFragmentType() {
    // 检查注释
    if (isCommentStart()) {
        return FragmentType::COMMENT;
    }
    
    // 检查CHTL JS特殊语法
    if (isCHTLJSStart()) {
        return FragmentType::CHTL_JS;
    }
    
    // 检查是否在特定上下文中
    if (!typeStack_.empty()) {
        FragmentType contextType = typeStack_.back();
        
        // 在CSS上下文中
        if (contextType == FragmentType::CSS) {
            return FragmentType::CSS;
        }
        
        // 在JavaScript上下文中
        if (contextType == FragmentType::JAVASCRIPT) {
            return FragmentType::JAVASCRIPT;
        }
    }
    
    // 检查是否是CSS开始
    if (isCSSStart()) {
        return FragmentType::CSS;
    }
    
    // 检查是否是JavaScript开始
    if (isJavaScriptStart()) {
        return FragmentType::JAVASCRIPT;
    }
    
    // 默认为CHTL
    return FragmentType::CHTL;
}

bool UnifiedScanner::isCHTLStart() const {
    // CHTL特征：
    // 1. 方括号关键字 [Template], [Custom], etc.
    // 2. 类型标识符 @Style, @Element, etc.
    // 3. HTML元素名
    // 4. text, style, script关键字
    
    if (peek() == '[') {
        return match("[Template]") || match("[Custom]") || 
               match("[Origin]") || match("[Import]") ||
               match("[Namespace]") || match("[Configuration]");
    }
    
    if (peek() == '@') {
        return matchRegex("@[A-Z][a-zA-Z]*");
    }
    
    if (matchRegex("[a-zA-Z][a-zA-Z0-9]*")) {
        // 检查是否是已知的HTML元素或CHTL关键字
        return true;
    }
    
    return false;
}

bool UnifiedScanner::isCHTLJSStart() const {
    // CHTL JS特征：
    // 1. {{ 选择器开始
    // 2. vir 关键字
    // 3. -> 箭头操作符
    
    if (match("{{")) {
        return true;
    }
    
    if (match("vir ") || match("vir\t") || match("vir\n")) {
        return true;
    }
    
    // 在script上下文中检查->
    if (!typeStack_.empty() && typeStack_.back() == FragmentType::JAVASCRIPT) {
        if (match("->")) {
            return true;
        }
    }
    
    return false;
}

bool UnifiedScanner::isCSSStart() const {
    // 在style块中或者是CSS选择器的开始
    return false;  // 由上下文决定
}

bool UnifiedScanner::isJavaScriptStart() const {
    // 在script块中
    return false;  // 由上下文决定
}

bool UnifiedScanner::isCommentStart() const {
    return match("//") || match("/*") || match("--");
}

CodeFragmentPtr UnifiedScanner::extractCHTLFragment() {
    SourceLocation startLoc = getCurrentLocation();
    size_t startPos = position_;
    
    // 提取CHTL片段，直到遇到其他类型的片段开始
    while (!this->isAtEnd()) {
        // 检查是否是其他片段的开始
        if (isCommentStart() || isCHTLJSStart()) {
            break;
        }
        
        // 检查是否是片段边界
        if (isFragmentBoundary(FragmentType::CHTL)) {
            break;
        }
        
        advance();
    }
    
    std::string content = content_.substr(startPos, position_ - startPos);
    SourceLocation endLoc = getCurrentLocation();
    SourceLocation loc = SourceLocation::merge(startLoc, endLoc);
    
    auto fragment = std::make_shared<CodeFragment>(FragmentType::CHTL, content, loc);
    
    // 分割成最小单元
    splitIntoMinimalUnits(fragment);
    
    return fragment;
}

CodeFragmentPtr UnifiedScanner::extractCHTLJSFragment() {
    SourceLocation startLoc = getCurrentLocation();
    size_t startPos = position_;
    
    // 处理不同的CHTL JS语法
    if (match("{{")) {
        // 提取选择器
        advance(); advance();  // 跳过 {{
        
        int braceCount = 2;
        while (!this->isAtEnd() && braceCount > 0) {
            if (peek() == '{') {
                braceCount++;
            } else if (peek() == '}') {
                braceCount--;
                if (braceCount == 0 && peek(1) == '}') {
                    advance(); // 跳过第一个 }
                    advance(); // 跳过第二个 }
                    break;
                }
            }
            advance();
        }
    } else if (match("vir")) {
        // 提取vir对象定义
        while (!this->isAtEnd() && peek() != ';' && peek() != '\n') {
            advance();
        }
        if (peek() == ';') advance();
    } else if (match("->")) {
        // 提取箭头操作
        advance(); advance();  // 跳过 ->
        
        // 提取后面的标识符或函数调用
        while (!this->isAtEnd() && (std::isalnum(peek()) || peek() == '_')) {
            advance();
        }
        
        // 如果是函数调用，提取参数
        if (peek() == '(') {
            int parenCount = 1;
            advance();
            while (!this->isAtEnd() && parenCount > 0) {
                if (peek() == '(') parenCount++;
                else if (peek() == ')') parenCount--;
                advance();
            }
        }
    }
    
    std::string content = content_.substr(startPos, position_ - startPos);
    SourceLocation endLoc = getCurrentLocation();
    SourceLocation loc = SourceLocation::merge(startLoc, endLoc);
    
    auto fragment = std::make_shared<CodeFragment>(FragmentType::CHTL_JS, content, loc);
    splitIntoMinimalUnits(fragment);
    
    return fragment;
}

CodeFragmentPtr UnifiedScanner::extractCSSFragment() {
    SourceLocation startLoc = getCurrentLocation();
    size_t startPos = position_;
    
    // 提取CSS片段，直到遇到CHTL或CHTL JS语法
    while (!this->isAtEnd()) {
        if (isCHTLStart() || isCHTLJSStart()) {
            break;
        }
        
        // 检查CSS片段结束（比如遇到}结束规则）
        if (peek() == '}' && !typeStack_.empty() && 
            typeStack_.back() == FragmentType::CSS) {
            advance();
            break;
        }
        
        advance();
    }
    
    std::string content = content_.substr(startPos, position_ - startPos);
    SourceLocation endLoc = getCurrentLocation();
    SourceLocation loc = SourceLocation::merge(startLoc, endLoc);
    
    return std::make_shared<CodeFragment>(FragmentType::CSS, content, loc);
}

CodeFragmentPtr UnifiedScanner::extractJavaScriptFragment() {
    SourceLocation startLoc = getCurrentLocation();
    size_t startPos = position_;
    
    // 提取JavaScript片段，注意处理字符串和注释
    bool inString = false;
    char stringChar = 0;
    bool inComment = false;
    bool inLineComment = false;
    
    while (!this->isAtEnd()) {
        char ch = peek();
        char next = peek(1);
        
        // 处理字符串
        if (!inComment) {
            if (!inString && (ch == '"' || ch == '\'' || ch == '`')) {
                inString = true;
                stringChar = ch;
            } else if (inString && ch == stringChar) {
                // 检查是否是转义的引号
                size_t backslashCount = 0;
                size_t checkPos = position_ - 1;
                while (checkPos > 0 && content_[checkPos] == '\\') {
                    backslashCount++;
                    checkPos--;
                }
                if (backslashCount % 2 == 0) {
                    inString = false;
                }
            }
        }
        
        // 处理注释
        if (!inString) {
            if (!inComment && ch == '/' && next == '/') {
                inLineComment = true;
                inComment = true;
            } else if (!inComment && ch == '/' && next == '*') {
                inComment = true;
            } else if (inLineComment && ch == '\n') {
                inLineComment = false;
                inComment = false;
            } else if (inComment && !inLineComment && ch == '*' && next == '/') {
                advance();
                inComment = false;
            }
        }
        
        // 检查CHTL JS语法
        if (!inString && !inComment && isCHTLJSStart()) {
            break;
        }
        
        advance();
    }
    
    std::string content = content_.substr(startPos, position_ - startPos);
    SourceLocation endLoc = getCurrentLocation();
    SourceLocation loc = SourceLocation::merge(startLoc, endLoc);
    
    return std::make_shared<CodeFragment>(FragmentType::JAVASCRIPT, content, loc);
}

CodeFragmentPtr UnifiedScanner::extractCommentFragment() {
    SourceLocation startLoc = getCurrentLocation();
    size_t startPos = position_;
    
    if (match("//")) {
        // 单行注释
        while (!this->isAtEnd() && peek() != '\n') {
            advance();
        }
    } else if (match("/*")) {
        // 多行注释
        advance(); advance();  // 跳过 /*
        while (!this->isAtEnd()) {
            if (peek() == '*' && peek(1) == '/') {
                advance(); advance();  // 跳过 */
                break;
            }
            advance();
        }
    } else if (match("--")) {
        // 生成器注释
        while (!this->isAtEnd() && peek() != '\n') {
            advance();
        }
    }
    
    std::string content = content_.substr(startPos, position_ - startPos);
    SourceLocation endLoc = getCurrentLocation();
    SourceLocation loc = SourceLocation::merge(startLoc, endLoc);
    
    return std::make_shared<CodeFragment>(FragmentType::COMMENT, content, loc);
}

CodeFragmentPtr UnifiedScanner::extractHTMLFragment() {
    SourceLocation startLoc = getCurrentLocation();
    size_t startPos = position_;
    
    // 提取原始HTML片段（在[Origin] @Html块中）
    while (!this->isAtEnd()) {
        // 查找结束标记
        if (peek() == '}' && !typeStack_.empty()) {
            break;
        }
        advance();
    }
    
    std::string content = content_.substr(startPos, position_ - startPos);
    SourceLocation endLoc = getCurrentLocation();
    SourceLocation loc = SourceLocation::merge(startLoc, endLoc);
    
    return std::make_shared<CodeFragment>(FragmentType::HTML, content, loc);
}

bool UnifiedScanner::isFragmentBoundary(FragmentType type) const {
    // 根据不同的片段类型判断边界
    switch (type) {
        case FragmentType::CHTL:
            // CHTL片段的边界：遇到其他类型的开始
            return isCommentStart() || isCHTLJSStart() ||
                   peek() == '}' || peek() == '{';
            
        case FragmentType::CHTL_JS:
            // CHTL JS片段通常较小，已在提取时处理
            return true;
            
        case FragmentType::CSS:
        case FragmentType::JAVASCRIPT:
            // CSS和JS片段的边界由上下文决定
            return peek() == '}';
            
        default:
            return false;
    }
}

void UnifiedScanner::splitIntoMinimalUnits(CodeFragmentPtr fragment) {
    // 根据片段类型分割成最小单元
    switch (fragment->type) {
        case FragmentType::CHTL:
            fragment->tokens = splitCHTLUnits(fragment->content);
            break;
            
        case FragmentType::CHTL_JS:
            fragment->tokens = splitCHTLJSUnits(fragment->content);
            break;
            
        default:
            // CSS、JavaScript、HTML片段由各自的编译器处理
            break;
    }
}

std::vector<TokenPtr> UnifiedScanner::splitCHTLUnits(const std::string& content) {
    std::vector<TokenPtr> tokens;
    size_t pos = 0;
    
    while (pos < content.size()) {
        // 跳过空白
        while (pos < content.size() && ScannerUtils::isWhitespace(content[pos])) {
            pos++;
        }
        
        if (pos >= content.size()) break;
        
        size_t tokenStart = pos;
        char ch = content[pos];
        
        // 单字符符号
        if (ch == '{' || ch == '}' || ch == '(' || ch == ')' || 
            ch == '[' || ch == ']' || ch == ';' || ch == ':' || 
            ch == '=' || ch == ',' || ch == '.') {
            pos++;
            auto token = std::make_shared<Token>(TokenType::UNKNOWN, 
                std::string(1, ch), TokenLocation(filename_, 0, 0, tokenStart));
            tokens.push_back(token);
        }
        // 字符串
        else if (ch == '"' || ch == '\'') {
            char quote = ch;
            pos++;
            while (pos < content.size() && content[pos] != quote) {
                if (content[pos] == '\\' && pos + 1 < content.size()) {
                    pos += 2;
                } else {
                    pos++;
                }
            }
            if (pos < content.size()) pos++; // 跳过结束引号
            
            std::string value = content.substr(tokenStart, pos - tokenStart);
            auto token = std::make_shared<Token>(TokenType::STRING_LITERAL, 
                value, TokenLocation(filename_, 0, 0, tokenStart));
            tokens.push_back(token);
        }
        // 标识符或关键字
        else if (ScannerUtils::isIdentifierStart(ch)) {
            while (pos < content.size() && ScannerUtils::isIdentifierPart(content[pos])) {
                pos++;
            }
            
            std::string value = content.substr(tokenStart, pos - tokenStart);
            auto token = std::make_shared<Token>(TokenType::IDENTIFIER, 
                value, TokenLocation(filename_, 0, 0, tokenStart));
            tokens.push_back(token);
        }
        // 其他字符
        else {
            pos++;
        }
    }
    
    return tokens;
}

std::vector<TokenPtr> UnifiedScanner::splitCHTLJSUnits(const std::string& content) {
    std::vector<TokenPtr> tokens;
    size_t pos = 0;
    
    while (pos < content.size()) {
        // 跳过空白
        while (pos < content.size() && ScannerUtils::isWhitespace(content[pos])) {
            pos++;
        }
        
        if (pos >= content.size()) break;
        
        size_t tokenStart = pos;
        
        // {{选择器}}
        if (pos + 1 < content.size() && content[pos] == '{' && content[pos + 1] == '{') {
            pos += 2;
            int braceCount = 2;
            while (pos < content.size() && braceCount > 0) {
                if (content[pos] == '{') braceCount++;
                else if (content[pos] == '}') {
                    braceCount--;
                    if (braceCount == 0 && pos + 1 < content.size() && content[pos + 1] == '}') {
                        pos += 2; // 跳过 }}
                        break;
                    }
                }
                pos++;
            }
            
            std::string value = content.substr(tokenStart, pos - tokenStart);
            auto token = std::make_shared<Token>(TokenType::DOUBLE_LEFT_BRACE, 
                value, TokenLocation(filename_, 0, 0, tokenStart));
            tokens.push_back(token);
        }
        // ->
        else if (pos + 1 < content.size() && content[pos] == '-' && content[pos + 1] == '>') {
            pos += 2;
            auto token = std::make_shared<Token>(TokenType::ARROW, 
                std::string("->"), TokenLocation(filename_, 0, 0, tokenStart));
            tokens.push_back(token);
        }
        // 标识符
        else if (ScannerUtils::isIdentifierStart(content[pos])) {
            while (pos < content.size() && ScannerUtils::isIdentifierPart(content[pos])) {
                pos++;
            }
            
            std::string value = content.substr(tokenStart, pos - tokenStart);
            TokenType type = (value == "vir") ? TokenType::KEYWORD_VIR : TokenType::IDENTIFIER;
            auto token = std::make_shared<Token>(type, 
                value, TokenLocation(filename_, 0, 0, tokenStart));
            tokens.push_back(token);
        }
        // 其他字符
        else {
            pos++;
        }
    }
    
    return tokens;
}

bool UnifiedScanner::validateFragmentContext(const CodeFragmentPtr& /*fragment*/) {
    // 验证片段是否在正确的上下文中
    // 例如：CHTL JS代码应该在script块中
    // CSS代码应该在style块中
    
    return true;  // 暂时返回true，后续实现验证逻辑
}

std::vector<CodeFragmentPtr> UnifiedScanner::getAllFragments() {
    if (!fragmentQueue_.empty()) {
        std::vector<CodeFragmentPtr> result;
        while (!fragmentQueue_.empty()) {
            result.push_back(fragmentQueue_.front());
            fragmentQueue_.pop();
        }
        return result;
    }
    
    return scan();
}

void UnifiedScanner::dumpFragments(const std::vector<CodeFragmentPtr>& fragments) const {
    std::cout << "=== 代码片段 ===\n";
    for (size_t i = 0; i < fragments.size(); ++i) {
        const auto& fragment = fragments[i];
        std::cout << "片段 " << i + 1 << ":\n";
        std::cout << "  类型: " << ScannerUtils::fragmentTypeToString(fragment->type) << "\n";
        std::cout << "  位置: " << fragment->location.toString() << "\n";
        std::cout << "  内容: " << fragment->content << "\n";
        std::cout << "  Token数: " << fragment->tokens.size() << "\n";
        std::cout << "---\n";
    }
}

// ScannerUtils实现
namespace ScannerUtils {

bool isIdentifierStart(char ch) {
    return (ch >= 'a' && ch <= 'z') || 
           (ch >= 'A' && ch <= 'Z') || 
           ch == '_' || ch == '$';
}

bool isIdentifierPart(char ch) {
    return isIdentifierStart(ch) || 
           (ch >= '0' && ch <= '9') || 
           ch == '-';  // CHTL允许标识符中包含连字符
}

bool isWhitespace(char ch) {
    return ch == ' ' || ch == '\t' || ch == '\r' || ch == '\n';
}

bool isNewline(char ch) {
    return ch == '\n' || ch == '\r';
}

std::string unescapeString(const std::string& str) {
    std::string result;
    bool escaped = false;
    
    for (char ch : str) {
        if (escaped) {
            switch (ch) {
                case 'n': result += '\n'; break;
                case 't': result += '\t'; break;
                case 'r': result += '\r'; break;
                case '\\': result += '\\'; break;
                case '"': result += '"'; break;
                case '\'': result += '\''; break;
                default: result += ch; break;
            }
            escaped = false;
        } else {
            if (ch == '\\') {
                escaped = true;
            } else {
                result += ch;
            }
        }
    }
    
    return result;
}

std::string escapeString(const std::string& str) {
    std::string result;
    
    for (char ch : str) {
        switch (ch) {
            case '\n': result += "\\n"; break;
            case '\t': result += "\\t"; break;
            case '\r': result += "\\r"; break;
            case '\\': result += "\\\\"; break;
            case '"': result += "\\\""; break;
            case '\'': result += "\\'"; break;
            default: result += ch; break;
        }
    }
    
    return result;
}

std::string fragmentTypeToString(FragmentType type) {
    switch (type) {
        case FragmentType::CHTL: return "CHTL";
        case FragmentType::CHTL_JS: return "CHTL_JS";
        case FragmentType::CSS: return "CSS";
        case FragmentType::JAVASCRIPT: return "JAVASCRIPT";
        case FragmentType::HTML: return "HTML";
        case FragmentType::COMMENT: return "COMMENT";
        case FragmentType::UNKNOWN: return "UNKNOWN";
        default: return "UNKNOWN";
    }
}

FragmentType stringToFragmentType(const std::string& str) {
    if (str == "CHTL") return FragmentType::CHTL;
    if (str == "CHTL_JS") return FragmentType::CHTL_JS;
    if (str == "CSS") return FragmentType::CSS;
    if (str == "JAVASCRIPT") return FragmentType::JAVASCRIPT;
    if (str == "HTML") return FragmentType::HTML;
    if (str == "COMMENT") return FragmentType::COMMENT;
    return FragmentType::UNKNOWN;
}

} // namespace ScannerUtils

} // namespace chtl