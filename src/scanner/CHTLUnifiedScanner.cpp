#include "../../include/CHTLUnifiedScanner.h"
#include <algorithm>
#include <cctype>
#include <regex>

namespace chtl {

CHTLUnifiedScanner::CHTLUnifiedScanner(const String& sourceCode)
    : sourceCode_(sourceCode), currentPos_(0), currentLine_(1), currentColumn_(1) {
}

CHTLUnifiedScanner::CHTLUnifiedScanner(std::istream& input)
    : currentPos_(0), currentLine_(1), currentColumn_(1) {
    std::ostringstream buffer;
    buffer << input.rdbuf();
    sourceCode_ = buffer.str();
}

std::vector<CodeFragment> CHTLUnifiedScanner::scanAllFragments() {
    std::vector<CodeFragment> fragments;
    currentPos_ = 0;
    currentLine_ = 1;
    currentColumn_ = 1;
    
    while (hasMoreFragments()) {
        CodeFragment fragment = scanNextFragment();
        if (fragment.type != FragmentType::UNKNOWN) {
            fragments.push_back(fragment);
        }
    }
    
    return fragments;
}

CodeFragment CHTLUnifiedScanner::scanNextFragment() {
    // 跳过空白字符和注释
    skipWhitespace();
    
    if (!hasMoreFragments()) {
        return CodeFragment();
    }
    
    size_t startPos = currentPos_;
    auto [startLine, startColumn] = getLineColumn(startPos);
    
    // 识别片段类型
    FragmentType type = identifyFragmentType(startPos);
    
    // 查找片段结束位置
    size_t endPos = findFragmentEnd(type, startPos);
    
    // 检查是否需要扩展片段
    while (needsExpansion(type, startPos, endPos)) {
        endPos = findFragmentEnd(type, startPos);
    }
    
    auto [endLine, endColumn] = getLineColumn(endPos);
    
    String content = sourceCode_.substr(startPos, endPos - startPos);
    currentPos_ = endPos;
    
    return CodeFragment(type, content, startLine, startColumn, endLine, endColumn);
}

bool CHTLUnifiedScanner::hasMoreFragments() const {
    return currentPos_ < sourceCode_.length();
}

char CHTLUnifiedScanner::peekChar(size_t offset) const {
    size_t pos = currentPos_ + offset;
    if (pos >= sourceCode_.length()) {
        return '\0';
    }
    return sourceCode_[pos];
}

char CHTLUnifiedScanner::nextChar() {
    if (currentPos_ >= sourceCode_.length()) {
        return '\0';
    }
    
    char ch = sourceCode_[currentPos_++];
    updatePosition(ch);
    return ch;
}

void CHTLUnifiedScanner::skipWhitespace() {
    while (currentPos_ < sourceCode_.length() && 
           std::isspace(sourceCode_[currentPos_])) {
        nextChar();
    }
}

void CHTLUnifiedScanner::skipComment() {
    if (currentPos_ + 1 < sourceCode_.length()) {
        // 单行注释 //
        if (sourceCode_[currentPos_] == '/' && sourceCode_[currentPos_ + 1] == '/') {
            while (currentPos_ < sourceCode_.length() && sourceCode_[currentPos_] != '\n') {
                nextChar();
            }
        }
        // 多行注释 /* */
        else if (sourceCode_[currentPos_] == '/' && sourceCode_[currentPos_ + 1] == '*') {
            nextChar(); // skip '/'
            nextChar(); // skip '*'
            while (currentPos_ + 1 < sourceCode_.length()) {
                if (sourceCode_[currentPos_] == '*' && sourceCode_[currentPos_ + 1] == '/') {
                    nextChar(); // skip '*'
                    nextChar(); // skip '/'
                    break;
                }
                nextChar();
            }
        }
        // CHTL生成器注释 --
        else if (sourceCode_[currentPos_] == '-' && sourceCode_[currentPos_ + 1] == '-') {
            while (currentPos_ < sourceCode_.length() && sourceCode_[currentPos_] != '\n') {
                nextChar();
            }
        }
    }
}

FragmentType CHTLUnifiedScanner::identifyFragmentType(size_t startPos) const {
    // 检查CHTL关键字
    if (isCHTLKeyword(startPos)) {
        return FragmentType::CHTL;
    }
    
    // 检查CHTL JS代码
    if (isCHTLJSCode(startPos)) {
        return FragmentType::CHTL_JS;
    }
    
    // 检查HTML标签
    if (isHTMLTag(startPos)) {
        return FragmentType::HTML;
    }
    
    // 检查CSS代码
    if (isCSSCode(startPos)) {
        return FragmentType::CSS;
    }
    
    // 检查JavaScript代码
    if (isJavaScriptCode(startPos)) {
        return FragmentType::JAVASCRIPT;
    }
    
    // 默认为CHTL
    return FragmentType::CHTL;
}

bool CHTLUnifiedScanner::isCHTLKeyword(size_t pos) const {
    if (pos >= sourceCode_.length()) return false;
    
    // 检查模板声明
    if (sourceCode_.substr(pos, 10) == "[Template]") return true;
    if (sourceCode_.substr(pos, 8) == "[Custom]") return true;
    if (sourceCode_.substr(pos, 8) == "[Origin]") return true;
    if (sourceCode_.substr(pos, 8) == "[Import]") return true;
    if (sourceCode_.substr(pos, 11) == "[Namespace]") return true;
    if (sourceCode_.substr(pos, 15) == "[Configuration]") return true;
    
    // 检查基本关键字
    String keywords[] = {"text", "style", "script", "inherit", "delete", "insert", "except"};
    for (const auto& keyword : keywords) {
        if (pos + keyword.length() <= sourceCode_.length() &&
            sourceCode_.substr(pos, keyword.length()) == keyword) {
            // 检查是否为完整单词
            if (pos + keyword.length() == sourceCode_.length() ||
                !std::isalnum(sourceCode_[pos + keyword.length()])) {
                return true;
            }
        }
    }
    
    // 检查@类型标识
    if (pos + 1 < sourceCode_.length() && sourceCode_[pos] == '@') {
        String types[] = {"@Style", "@Element", "@Var", "@Html", "@JavaScript", "@Chtl", "@CJmod"};
        for (const auto& type : types) {
            if (pos + type.length() <= sourceCode_.length() &&
                sourceCode_.substr(pos, type.length()) == type) {
                return true;
            }
        }
    }
    
    return false;
}

bool CHTLUnifiedScanner::isCHTLJSCode(size_t pos) const {
    if (pos + 1 >= sourceCode_.length()) return false;
    
    // 检查{{选择器}}语法
    if (sourceCode_[pos] == '{' && sourceCode_[pos + 1] == '{') {
        return true;
    }
    
    // 检查->操作符
    if (sourceCode_[pos] == '-' && sourceCode_[pos + 1] == '>') {
        return true;
    }
    
    // 检查CHTL JS关键字
    String keywords[] = {"vir", "listen", "delegate", "animate"};
    for (const auto& keyword : keywords) {
        if (pos + keyword.length() <= sourceCode_.length() &&
            sourceCode_.substr(pos, keyword.length()) == keyword) {
            // 检查是否为完整单词
            if (pos + keyword.length() == sourceCode_.length() ||
                !std::isalnum(sourceCode_[pos + keyword.length()])) {
                return true;
            }
        }
    }
    
    return false;
}

bool CHTLUnifiedScanner::isHTMLTag(size_t pos) const {
    if (pos >= sourceCode_.length() || sourceCode_[pos] != '<') {
        return false;
    }
    
    // 简单的HTML标签检测
    size_t endPos = pos + 1;
    while (endPos < sourceCode_.length() && sourceCode_[endPos] != '>') {
        endPos++;
    }
    
    if (endPos >= sourceCode_.length()) return false;
    
    String tagContent = sourceCode_.substr(pos + 1, endPos - pos - 1);
    return !tagContent.empty() && std::isalpha(tagContent[0]);
}

bool CHTLUnifiedScanner::isCSSCode(size_t pos) const {
    // CSS代码通常在style块中或以选择器开始
    if (pos >= sourceCode_.length()) return false;
    
    // 检查CSS选择器模式
    std::regex cssSelector(R"(^[.#]?[a-zA-Z][a-zA-Z0-9_-]*\s*\{)");
    String sample = sourceCode_.substr(pos, std::min(size_t(50), sourceCode_.length() - pos));
    return std::regex_search(sample, cssSelector);
}

bool CHTLUnifiedScanner::isJavaScriptCode(size_t pos) const {
    if (pos >= sourceCode_.length()) return false;
    
    // JavaScript关键字检测
    String jsKeywords[] = {"function", "var", "let", "const", "if", "for", "while", "return"};
    for (const auto& keyword : jsKeywords) {
        if (pos + keyword.length() <= sourceCode_.length() &&
            sourceCode_.substr(pos, keyword.length()) == keyword) {
            // 检查是否为完整单词
            if (pos + keyword.length() == sourceCode_.length() ||
                !std::isalnum(sourceCode_[pos + keyword.length()])) {
                return true;
            }
        }
    }
    
    return false;
}

size_t CHTLUnifiedScanner::findFragmentEnd(FragmentType type, size_t startPos) {
    switch (type) {
        case FragmentType::CHTL:
            return findCHTLFragmentEnd(startPos);
        case FragmentType::CHTL_JS:
            return findCHTLJSFragmentEnd(startPos);
        case FragmentType::CSS:
            return findStyleBlockEnd(startPos);
        case FragmentType::JAVASCRIPT:
            return findScriptBlockEnd(startPos);
        case FragmentType::HTML:
            return findMatchingBrace(startPos);
        default:
            return startPos + 1;
    }
}

size_t CHTLUnifiedScanner::findCHTLFragmentEnd(size_t startPos) {
    size_t pos = startPos;
    
    // 查找下一个{
    while (pos < sourceCode_.length() && sourceCode_[pos] != '{') {
        pos++;
    }
    
    if (pos >= sourceCode_.length()) {
        return sourceCode_.length();
    }
    
    // 查找匹配的}
    return findMatchingBrace(pos);
}

size_t CHTLUnifiedScanner::findCHTLJSFragmentEnd(size_t startPos) {
    size_t pos = startPos;
    
    // 对于{{}}语法
    if (pos + 1 < sourceCode_.length() && 
        sourceCode_[pos] == '{' && sourceCode_[pos + 1] == '{') {
        pos += 2;
        while (pos + 1 < sourceCode_.length()) {
            if (sourceCode_[pos] == '}' && sourceCode_[pos + 1] == '}') {
                return pos + 2;
            }
            pos++;
        }
    }
    
    // 对于其他CHTL JS语法，查找到分号或换行
    while (pos < sourceCode_.length() && 
           sourceCode_[pos] != ';' && sourceCode_[pos] != '\n') {
        pos++;
    }
    
    return pos + 1;
}

size_t CHTLUnifiedScanner::findStyleBlockEnd(size_t startPos) {
    return findMatchingBrace(startPos);
}

size_t CHTLUnifiedScanner::findScriptBlockEnd(size_t startPos) {
    return findMatchingBrace(startPos);
}

size_t CHTLUnifiedScanner::findMatchingBrace(size_t startPos) {
    if (startPos >= sourceCode_.length() || sourceCode_[startPos] != '{') {
        return startPos + 1;
    }
    
    int braceCount = 1;
    size_t pos = startPos + 1;
    
    while (pos < sourceCode_.length() && braceCount > 0) {
        char ch = sourceCode_[pos];
        
        // 跳过字符串
        if (ch == '"' || ch == '\'') {
            pos = skipString(pos, ch);
            continue;
        }
        
        // 跳过注释
        if (ch == '/' && pos + 1 < sourceCode_.length() && 
            (sourceCode_[pos + 1] == '/' || sourceCode_[pos + 1] == '*')) {
            pos = skipComment(pos);
            continue;
        }
        
        if (ch == '{') {
            braceCount++;
        } else if (ch == '}') {
            braceCount--;
        }
        
        pos++;
    }
    
    return pos;
}

size_t CHTLUnifiedScanner::skipString(size_t pos, char quote) {
    if (pos >= sourceCode_.length() || sourceCode_[pos] != quote) {
        return pos;
    }
    
    pos++; // 跳过开始引号
    while (pos < sourceCode_.length()) {
        char ch = sourceCode_[pos];
        if (ch == quote) {
            return pos + 1; // 跳过结束引号
        }
        if (ch == '\\' && pos + 1 < sourceCode_.length()) {
            pos++; // 跳过转义字符
        }
        pos++;
    }
    
    return pos;
}

size_t CHTLUnifiedScanner::skipComment(size_t pos) {
    if (pos + 1 >= sourceCode_.length()) return pos;
    
    if (sourceCode_[pos] == '/' && sourceCode_[pos + 1] == '/') {
        // 单行注释
        while (pos < sourceCode_.length() && sourceCode_[pos] != '\n') {
            pos++;
        }
    } else if (sourceCode_[pos] == '/' && sourceCode_[pos + 1] == '*') {
        // 多行注释
        pos += 2;
        while (pos + 1 < sourceCode_.length()) {
            if (sourceCode_[pos] == '*' && sourceCode_[pos + 1] == '/') {
                return pos + 2;
            }
            pos++;
        }
    }
    
    return pos;
}

bool CHTLUnifiedScanner::needsExpansion(FragmentType type, size_t startPos, size_t endPos) {
    // 检查片段是否完整
    if (endPos >= sourceCode_.length()) return false;
    
    // 对于CHTL和CHTL JS，检查下一个token是否可能是同一个片段的一部分
    if (type == FragmentType::CHTL || type == FragmentType::CHTL_JS) {
        // 跳过空白
        size_t nextPos = endPos;
        while (nextPos < sourceCode_.length() && std::isspace(sourceCode_[nextPos])) {
            nextPos++;
        }
        
        if (nextPos < sourceCode_.length()) {
            // 检查下一个字符是否可能继续当前片段
            char nextChar = sourceCode_[nextPos];
            if (nextChar == '.' || nextChar == '-' || nextChar == '{' || nextChar == '[') {
                return true;
            }
        }
    }
    
    return false;
}

void CHTLUnifiedScanner::reportError(const String& message, size_t line, size_t column) {
    if (line == 0) line = currentLine_;
    if (column == 0) column = currentColumn_;
    errors_.emplace_back(message, "", line, column);
}

void CHTLUnifiedScanner::updatePosition(char ch) {
    if (ch == '\n') {
        currentLine_++;
        currentColumn_ = 1;
    } else {
        currentColumn_++;
    }
}

std::pair<size_t, size_t> CHTLUnifiedScanner::getLineColumn(size_t pos) const {
    size_t line = 1;
    size_t column = 1;
    
    for (size_t i = 0; i < pos && i < sourceCode_.length(); ++i) {
        if (sourceCode_[i] == '\n') {
            line++;
            column = 1;
        } else {
            column++;
        }
    }
    
    return {line, column};
}

} // namespace chtl