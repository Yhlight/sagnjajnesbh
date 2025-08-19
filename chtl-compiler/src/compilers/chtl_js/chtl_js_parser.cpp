#include "compilers/chtl_js/chtl_js_parser.h"
#include <regex>
#include <sstream>

namespace chtl {

CHTLJSParser::CHTLJSParser() : current_(0) {
}

CHTLJSParser::~CHTLJSParser() = default;

std::shared_ptr<CHTLJSASTNode> CHTLJSParser::parse(const std::string& source) {
    source_ = source;
    current_ = 0;
    errors_.clear();
    
    auto program = std::make_shared<CHTLJSProgramNode>();
    
    while (!isAtEnd()) {
        skipWhitespace();
        if (isAtEnd()) break;
        
        auto statement = parseStatement();
        if (statement) {
            program->addChild(statement);
        }
    }
    
    return program;
}

void CHTLJSParser::reset() {
    source_.clear();
    current_ = 0;
    errors_.clear();
}

std::shared_ptr<CHTLJSASTNode> CHTLJSParser::parseStatement() {
    skipWhitespace();
    
    // 检查虚对象声明 vir
    if (match("vir ")) {
        return parseVirtualObject();
    }
    
    // 检查增强选择器 {{
    if (match("{{")) {
        size_t start = current_ - 2;
        std::string selector = extractUntil("}}");
        if (match("}}")) {
            // 检查是否有箭头调用
            skipWhitespace();
            if (match("->")) {
                return parseArrowCall("{{" + selector + "}}");
            }
            return std::make_shared<EnhancedSelectorNode>(selector);
        }
    }
    
    // 检查函数调用
    if (matchRegex(R"(listen\s*\()")) {
        return parseListen();
    }
    
    if (matchRegex(R"(delegate\s*\()")) {
        return parseDelegate();
    }
    
    if (matchRegex(R"(animate\s*\()")) {
        return parseAnimate();
    }
    
    // 检查箭头操作符
    size_t savedPos = current_;
    std::string beforeArrow;
    while (!isAtEnd() && peek() != '\n' && peek() != ';') {
        if (peek() == '-' && peek(1) == '>') {
            std::string object = source_.substr(savedPos, current_ - savedPos);
            return parseArrowCall(object);
        }
        advance();
    }
    
    // 恢复位置，作为普通JavaScript处理
    current_ = savedPos;
    
    // 提取到语句结束
    std::string jsCode;
    int braceDepth = 0;
    int parenDepth = 0;
    bool inString = false;
    char stringChar = 0;
    
    while (!isAtEnd()) {
        char ch = peek();
        
        if (!inString) {
            if (ch == '"' || ch == '\'' || ch == '`') {
                inString = true;
                stringChar = ch;
            } else if (ch == '{') {
                braceDepth++;
            } else if (ch == '}') {
                braceDepth--;
                if (braceDepth < 0) break;
            } else if (ch == '(') {
                parenDepth++;
            } else if (ch == ')') {
                parenDepth--;
            } else if (ch == ';' && braceDepth == 0 && parenDepth == 0) {
                advance();
                break;
            } else if (ch == '\n' && braceDepth == 0 && parenDepth == 0) {
                break;
            }
        } else {
            if (ch == stringChar && source_[current_ - 1] != '\\') {
                inString = false;
            }
        }
        
        jsCode += advance();
    }
    
    if (!jsCode.empty()) {
        return std::make_shared<RegularJSNode>(jsCode);
    }
    
    return nullptr;
}

std::shared_ptr<CHTLJSASTNode> CHTLJSParser::parseEnhancedSelector() {
    if (!match("{{")) {
        return nullptr;
    }
    
    std::string selector = extractUntil("}}");
    
    if (!match("}}")) {
        addError("Expected '}}' after selector");
        return nullptr;
    }
    
    return std::make_shared<EnhancedSelectorNode>(selector);
}

std::shared_ptr<CHTLJSASTNode> CHTLJSParser::parseVirtualObject() {
    skipWhitespace();
    
    // 获取虚对象名称
    std::string name;
    while (!isAtEnd() && std::isalnum(peek()) || peek() == '_') {
        name += advance();
    }
    
    skipWhitespace();
    if (!match("=")) {
        addError("Expected '=' after virtual object name");
        return nullptr;
    }
    
    skipWhitespace();
    
    // 获取定义内容
    std::string definition;
    int braceDepth = 0;
    bool started = false;
    
    while (!isAtEnd()) {
        char ch = peek();
        
        if (ch == '{') {
            braceDepth++;
            started = true;
        } else if (ch == '}') {
            braceDepth--;
            if (braceDepth == 0 && started) {
                definition += advance();
                break;
            }
        } else if (ch == ';' && braceDepth == 0) {
            advance();
            break;
        }
        
        definition += advance();
    }
    
    return std::make_shared<VirtualObjectNode>(name, definition);
}

std::shared_ptr<CHTLJSASTNode> CHTLJSParser::parseArrowCall(const std::string& object) {
    if (!match("->")) {
        return nullptr;
    }
    
    skipWhitespace();
    
    // 获取方法调用
    std::string method;
    while (!isAtEnd() && (std::isalnum(peek()) || peek() == '_' || peek() == '(' || peek() == ')')) {
        method += advance();
        if (peek(-1) == ')') {
            // 方法调用结束
            break;
        }
    }
    
    return std::make_shared<ArrowCallNode>(object, method);
}

std::shared_ptr<CHTLJSASTNode> CHTLJSParser::parseListen() {
    // listen( 已经被匹配了
    std::string content = extractBetween('(', ')');
    return std::make_shared<ListenCallNode>("", content);
}

std::shared_ptr<CHTLJSASTNode> CHTLJSParser::parseDelegate() {
    // delegate( 已经被匹配了
    std::string content = extractBetween('(', ')');
    return std::make_shared<DelegateCallNode>("", content);
}

std::shared_ptr<CHTLJSASTNode> CHTLJSParser::parseAnimate() {
    // animate( 已经被匹配了
    std::string content = extractBetween('(', ')');
    return std::make_shared<AnimateCallNode>(content);
}

bool CHTLJSParser::match(const std::string& str) {
    if (current_ + str.length() > source_.length()) {
        return false;
    }
    
    for (size_t i = 0; i < str.length(); ++i) {
        if (source_[current_ + i] != str[i]) {
            return false;
        }
    }
    
    current_ += str.length();
    return true;
}

bool CHTLJSParser::matchRegex(const std::string& pattern) {
    std::regex regex(pattern);
    std::string remaining = source_.substr(current_);
    std::smatch match;
    
    if (std::regex_search(remaining, match, regex) && match.position() == 0) {
        current_ += match.length();
        return true;
    }
    
    return false;
}

std::string CHTLJSParser::extractUntil(const std::string& delimiter) {
    std::string result;
    
    while (!isAtEnd()) {
        if (current_ + delimiter.length() <= source_.length()) {
            bool found = true;
            for (size_t i = 0; i < delimiter.length(); ++i) {
                if (source_[current_ + i] != delimiter[i]) {
                    found = false;
                    break;
                }
            }
            if (found) {
                return result;
            }
        }
        
        result += advance();
    }
    
    return result;
}

std::string CHTLJSParser::extractBetween(char open, char close) {
    std::string result;
    int depth = 1; // 开始括号已经被匹配
    
    while (!isAtEnd() && depth > 0) {
        char ch = advance();
        
        if (ch == open) {
            depth++;
        } else if (ch == close) {
            depth--;
            if (depth == 0) {
                break;
            }
        }
        
        result += ch;
    }
    
    return result;
}

void CHTLJSParser::skipWhitespace() {
    while (!isAtEnd() && std::isspace(peek())) {
        advance();
    }
}

bool CHTLJSParser::isAtEnd() const {
    return current_ >= source_.length();
}

char CHTLJSParser::peek(size_t offset) const {
    size_t pos = current_ + offset;
    if (pos >= source_.length() || pos < 0) {
        return '\0';
    }
    return source_[pos];
}

char CHTLJSParser::advance() {
    if (isAtEnd()) return '\0';
    return source_[current_++];
}

void CHTLJSParser::addError(const std::string& message) {
    errors_.push_back("CHTL JS Parser Error at position " + 
                     std::to_string(current_) + ": " + message);
}

} // namespace chtl