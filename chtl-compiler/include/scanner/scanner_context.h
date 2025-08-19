#ifndef CHTL_SCANNER_CONTEXT_H
#define CHTL_SCANNER_CONTEXT_H

#include <string>
#include <stack>
#include <unordered_set>

namespace chtl {

/**
 * 扫描器上下文状态
 */
enum class ScannerState {
    NORMAL,         // 普通CHTL代码
    IN_STYLE,       // 在style块中
    IN_SCRIPT,      // 在script块中
    IN_TEXT,        // 在text块中
    IN_ORIGIN,      // 在[Origin]块中
    IN_STRING,      // 在字符串中
    IN_COMMENT      // 在注释中
};

/**
 * 扫描器上下文，用于追踪扫描状态
 */
class ScannerContext {
public:
    ScannerContext();
    
    // 状态管理
    void pushState(ScannerState state);
    void popState();
    ScannerState getCurrentState() const;
    bool isInState(ScannerState state) const;
    
    // 位置追踪
    void updatePosition(char ch);
    size_t getCurrentLine() const { return currentLine_; }
    size_t getCurrentColumn() const { return currentColumn_; }
    
    // 重置上下文
    void reset();
    
private:
    std::stack<ScannerState> stateStack_;
    size_t currentLine_;
    size_t currentColumn_;
};

} // namespace chtl

#endif // CHTL_SCANNER_CONTEXT_H