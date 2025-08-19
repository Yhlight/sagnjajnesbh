#include "scanner/chtl_unified_scanner.h"
#include <algorithm>
#include <sstream>

namespace chtl {

CHTLUnifiedScanner::CHTLUnifiedScanner() 
    : context_(std::make_unique<ScannerContext>()), currentPos_(0) {
}

CHTLUnifiedScanner::~CHTLUnifiedScanner() = default;

std::vector<std::unique_ptr<CodeFragment>> CHTLUnifiedScanner::scan(const std::string& source) {
    source_ = source;
    currentPos_ = 0;
    errors_.clear();
    context_->reset();
    
    std::vector<std::unique_ptr<CodeFragment>> fragments;
    
    while (!isAtEnd()) {
        skipWhitespace();
        if (isAtEnd()) break;
        
        size_t startPos = currentPos_;
        size_t startLine = context_->getCurrentLine();
        size_t startColumn = context_->getCurrentColumn();
        
        // 检测代码片段类型
        FragmentType type = detectFragmentType();
        std::string content;
        
        switch (type) {
            case FragmentType::LOCAL_STYLE:
                content = extractBlock('{', '}');
                fragments.push_back(std::make_unique<LocalStyleFragment>(
                    content, startLine, startColumn));
                break;
                
            case FragmentType::CSS:
                // 全局样式块
                content = extractBlock('{', '}');
                fragments.push_back(std::make_unique<CSSFragment>(
                    content, startLine, startColumn));
                break;
                
            case FragmentType::JAVASCRIPT:
                // 标准JavaScript
                content = extractBlock('{', '}');
                fragments.push_back(std::make_unique<JavaScriptFragment>(
                    content, startLine, startColumn));
                break;
                
            case FragmentType::CHTL_JS:
                // CHTL JS代码
                content = extractBlock('{', '}');
                fragments.push_back(std::make_unique<CHTLJSFragment>(
                    content, startLine, startColumn));
                break;
                
            case FragmentType::CHTL:
            default:
                // 普通CHTL代码
                content = extractCHTLBlock();
                fragments.push_back(std::make_unique<CHTLFragment>(
                    content, startLine, startColumn));
                break;
        }
    }
    
    return fragments;
}

void CHTLUnifiedScanner::reset() {
    source_.clear();
    currentPos_ = 0;
    errors_.clear();
    context_->reset();
    while (!contextStack_.empty()) {
        contextStack_.pop();
    }
}

FragmentType CHTLUnifiedScanner::detectFragmentType() {
    // 保存当前位置
    size_t savedPos = currentPos_;
    
    // 跳过空白
    skipWhitespace();
    
    // 检查是否是style块
    if (match("style")) {
        skipWhitespace();
        if (peek() == '{') {
            // 检查是否在元素内部（局部样式）或全局
            if (isInElementContext()) {
                currentPos_ = savedPos;
                return FragmentType::LOCAL_STYLE;
            } else {
                currentPos_ = savedPos;
                return FragmentType::CSS;
            }
        }
    }
    
    // 检查是否是script块
    if (match("script")) {
        skipWhitespace();
        if (peek() == '{') {
            // 检查是否包含CHTL JS语法
            if (containsCHTLJSSyntax()) {
                currentPos_ = savedPos;
                return FragmentType::CHTL_JS;
            } else {
                currentPos_ = savedPos;
                return FragmentType::JAVASCRIPT;
            }
        }
    }
    
    // 检查是否是[Origin]块
    if (match("[Origin]")) {
        skipWhitespace();
        if (match("@Html")) {
            currentPos_ = savedPos;
            return FragmentType::ORIGIN_HTML;
        } else if (match("@Style")) {
            currentPos_ = savedPos;
            return FragmentType::ORIGIN_CSS;
        } else if (match("@JavaScript")) {
            currentPos_ = savedPos;
            return FragmentType::ORIGIN_JS;
        }
    }
    
    // 默认为CHTL代码
    currentPos_ = savedPos;
    return FragmentType::CHTL;
}

char CHTLUnifiedScanner::peek(size_t offset) const {
    size_t pos = currentPos_ + offset;
    if (pos >= source_.length()) {
        return '\0';
    }
    return source_[pos];
}

char CHTLUnifiedScanner::advance() {
    if (isAtEnd()) return '\0';
    char ch = source_[currentPos_++];
    context_->updatePosition(ch);
    return ch;
}

bool CHTLUnifiedScanner::match(const std::string& expected) {
    if (currentPos_ + expected.length() > source_.length()) {
        return false;
    }
    
    for (size_t i = 0; i < expected.length(); ++i) {
        if (source_[currentPos_ + i] != expected[i]) {
            return false;
        }
    }
    
    currentPos_ += expected.length();
    for (char ch : expected) {
        context_->updatePosition(ch);
    }
    return true;
}

bool CHTLUnifiedScanner::isAtEnd() const {
    return currentPos_ >= source_.length();
}

void CHTLUnifiedScanner::skipWhitespace() {
    while (!isAtEnd()) {
        char ch = peek();
        if (ch == ' ' || ch == '\t' || ch == '\r' || ch == '\n') {
            advance();
        } else if (ch == '/' && peek(1) == '/') {
            // 单行注释
            skipSingleLineComment();
        } else if (ch == '/' && peek(1) == '*') {
            // 多行注释
            skipMultiLineComment();
        } else if (ch == '-' && peek(1) == '-') {
            // 生成器注释
            skipGeneratorComment();
        } else {
            break;
        }
    }
}

void CHTLUnifiedScanner::skipComment() {
    if (peek() == '/' && peek(1) == '/') {
        skipSingleLineComment();
    } else if (peek() == '/' && peek(1) == '*') {
        skipMultiLineComment();
    }
}

void CHTLUnifiedScanner::skipSingleLineComment() {
    advance(); // /
    advance(); // /
    while (!isAtEnd() && peek() != '\n') {
        advance();
    }
}

void CHTLUnifiedScanner::skipMultiLineComment() {
    advance(); // /
    advance(); // *
    while (!isAtEnd()) {
        if (peek() == '*' && peek(1) == '/') {
            advance(); // *
            advance(); // /
            break;
        }
        advance();
    }
}

void CHTLUnifiedScanner::skipGeneratorComment() {
    advance(); // -
    advance(); // -
    while (!isAtEnd() && peek() != '\n') {
        advance();
    }
}

std::string CHTLUnifiedScanner::extractBlock(char openDelim, char closeDelim) {
    std::stringstream result;
    int depth = 0;
    
    // 跳过开始定界符
    while (!isAtEnd() && peek() != openDelim) {
        advance();
    }
    
    if (!isAtEnd()) {
        advance(); // 跳过开始定界符
        depth = 1;
    }
    
    while (!isAtEnd() && depth > 0) {
        char ch = peek();
        
        if (ch == openDelim) {
            depth++;
        } else if (ch == closeDelim) {
            depth--;
            if (depth == 0) {
                advance(); // 跳过结束定界符
                break;
            }
        }
        
        result << advance();
    }
    
    return result.str();
}

std::string CHTLUnifiedScanner::extractUntil(const std::string& delimiter) {
    std::stringstream result;
    
    while (!isAtEnd()) {
        if (match(delimiter)) {
            break;
        }
        result << advance();
    }
    
    return result.str();
}

std::string CHTLUnifiedScanner::extractCHTLBlock() {
    std::stringstream result;
    int braceDepth = 0;
    
    while (!isAtEnd()) {
        skipWhitespace();
        
        // 检查是否遇到了其他类型的代码块
        FragmentType nextType = detectFragmentType();
        if (nextType != FragmentType::CHTL && braceDepth == 0) {
            break;
        }
        
        char ch = peek();
        
        if (ch == '{') {
            braceDepth++;
        } else if (ch == '}') {
            braceDepth--;
            if (braceDepth < 0) {
                break;
            }
        }
        
        result << advance();
    }
    
    return result.str();
}

bool CHTLUnifiedScanner::isInElementContext() {
    // 简化实现：检查上下文栈中是否有元素
    return !contextStack_.empty() && 
           contextStack_.top() == FragmentType::CHTL;
}

bool CHTLUnifiedScanner::containsCHTLJSSyntax() {
    // 保存当前位置
    size_t savedPos = currentPos_;
    
    std::string content = extractBlock('{', '}');
    
    // 检查CHTL JS特有语法
    bool hasCHTLJS = false;
    if (content.find("{{") != std::string::npos ||     // 增强选择器
        content.find("->") != std::string::npos ||     // 箭头操作符
        content.find("vir ") != std::string::npos ||   // 虚对象
        content.find("listen(") != std::string::npos || // 增强监听器
        content.find("delegate(") != std::string::npos || // 事件委托
        content.find("animate(") != std::string::npos) { // 动画API
        hasCHTLJS = true;
    }
    
    // 恢复位置
    currentPos_ = savedPos;
    return hasCHTLJS;
}

std::unique_ptr<CodeFragment> CHTLUnifiedScanner::createFragment(
    FragmentType type, 
    const std::string& content,
    size_t line,
    size_t column) {
    
    switch (type) {
        case FragmentType::CHTL:
            return std::make_unique<CHTLFragment>(content, line, column);
        case FragmentType::CHTL_JS:
            return std::make_unique<CHTLJSFragment>(content, line, column);
        case FragmentType::CSS:
            return std::make_unique<CSSFragment>(content, line, column);
        case FragmentType::JAVASCRIPT:
            return std::make_unique<JavaScriptFragment>(content, line, column);
        case FragmentType::LOCAL_STYLE:
            return std::make_unique<LocalStyleFragment>(content, line, column);
        default:
            return std::make_unique<CHTLFragment>(content, line, column);
    }
}

void CHTLUnifiedScanner::addError(const std::string& message, size_t line, size_t column) {
    std::stringstream ss;
    ss << "Error at line " << line << ", column " << column << ": " << message;
    errors_.push_back(ss.str());
}

} // namespace chtl