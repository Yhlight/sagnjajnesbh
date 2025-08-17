#pragma once

#include "CHTLCommon.h"
#include <fstream>
#include <sstream>

namespace CHTL {

/**
 * CHTL统一扫描器 - 精准代码切割器
 * 负责将源代码切割成不同类型的代码片段：CHTL、CHTL JS、CSS、JavaScript
 * 基于可变长度切片进行工作，确保代码片段的完整性和精确性
 */
class CHTLUnifiedScanner {
public:
    explicit CHTLUnifiedScanner(const String& sourceCode);
    explicit CHTLUnifiedScanner(std::istream& input);
    
    // 扫描并返回所有代码片段
    std::vector<CodeFragment> scanAllFragments();
    
    // 逐个扫描代码片段
    CodeFragment scanNextFragment();
    
    // 检查是否还有更多片段
    bool hasMoreFragments() const;
    
    // 获取当前扫描位置
    size_t getCurrentPosition() const { return currentPos_; }
    size_t getCurrentLine() const { return currentLine_; }
    size_t getCurrentColumn() const { return currentColumn_; }
    
    // 错误处理
    const std::vector<CompilerError>& getErrors() const { return errors_; }
    
private:
    String sourceCode_;
    size_t currentPos_;
    size_t currentLine_;
    size_t currentColumn_;
    std::vector<CompilerError> errors_;
    
    // 辅助方法
    char peekChar(size_t offset = 0) const;
    char nextChar();
    void skipWhitespace();
    void skipComment();
    
    // 片段识别方法
    FragmentType identifyFragmentType(size_t startPos) const;
    bool isHTMLTag(size_t pos) const;
    bool isCHTLKeyword(size_t pos) const;
    bool isCHTLJSCode(size_t pos) const;
    bool isCSSCode(size_t pos) const;
    bool isJavaScriptCode(size_t pos) const;
    
    // 片段边界检测
    size_t findFragmentEnd(FragmentType type, size_t startPos);
    size_t findCHTLFragmentEnd(size_t startPos);
    size_t findCHTLJSFragmentEnd(size_t startPos);
    size_t findStyleBlockEnd(size_t startPos);
    size_t findScriptBlockEnd(size_t startPos);
    
    // 括号匹配
    size_t findMatchingBrace(size_t startPos);
    int countNestedBraces(size_t startPos, size_t endPos);
    
    // 字符串和注释处理
    size_t skipString(size_t pos, char quote);
    size_t skipComment(size_t pos);
    
    // 上下文分析
    bool isInString(size_t pos) const;
    bool isInComment(size_t pos) const;
    bool needsExpansion(FragmentType type, size_t startPos, size_t endPos);
    
    // 错误报告
    void reportError(const String& message, size_t line = 0, size_t column = 0);
    
    // 位置更新
    void updatePosition(char ch);
    std::pair<size_t, size_t> getLineColumn(size_t pos) const;
};

} // namespace CHTL