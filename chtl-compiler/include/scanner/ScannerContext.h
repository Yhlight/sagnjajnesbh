#pragma once

#include <string>
#include <stack>
#include <vector>

namespace chtl {

/**
 * 扫描器上下文状态
 */
enum class ScannerState {
    NORMAL,             // 普通状态
    IN_CHTL_ELEMENT,    // 在CHTL元素内
    IN_STYLE_BLOCK,     // 在style块内（全局或局部）
    IN_SCRIPT_BLOCK,    // 在script块内（全局或局部）
    IN_LOCAL_STYLE,     // 在局部style块内
    IN_LOCAL_SCRIPT,    // 在局部script块内
    IN_TEMPLATE,        // 在Template块内
    IN_CUSTOM,          // 在Custom块内
    IN_CONFIGURATION,   // 在Configuration块内
    IN_NAMESPACE,       // 在Namespace块内
    IN_IMPORT,          // 在Import块内
    IN_ORIGIN,          // 在Origin块内
    IN_TEXT_NODE,       // 在text节点内
    IN_STRING_LITERAL,  // 在字符串字面量内
    IN_COMMENT,         // 在注释内
    IN_MULTILINE_COMMENT // 在多行注释内
};

/**
 * 括号类型
 */
enum class BraceType {
    CURLY,      // {}
    SQUARE,     // []
    PARENTHESIS // ()
};

/**
 * 扫描器上下文
 * 维护扫描过程中的状态信息
 */
class ScannerContext {
public:
    ScannerContext();

    // 状态管理
    void PushState(ScannerState state);
    void PopState();
    ScannerState GetCurrentState() const;
    bool IsInState(ScannerState state) const;

    // 括号匹配
    void PushBrace(BraceType type, size_t line, size_t column);
    bool PopBrace(BraceType type);
    bool IsBalanced() const;
    int GetBraceDepth(BraceType type) const;

    // 位置追踪
    void UpdatePosition(char ch);
    size_t GetCurrentLine() const { return m_CurrentLine; }
    size_t GetCurrentColumn() const { return m_CurrentColumn; }

    // 上下文信息
    void SetCurrentElementName(const std::string& name) { m_CurrentElementName = name; }
    const std::string& GetCurrentElementName() const { return m_CurrentElementName; }

    void SetCurrentBlockType(const std::string& type) { m_CurrentBlockType = type; }
    const std::string& GetCurrentBlockType() const { return m_CurrentBlockType; }

    // 字符串和注释处理
    void SetStringDelimiter(char delimiter) { m_StringDelimiter = delimiter; }
    char GetStringDelimiter() const { return m_StringDelimiter; }

    // 重置上下文
    void Reset();

    // 判断是否在特定上下文中
    bool IsInCHTLContext() const;
    bool IsInCHTLJSContext() const;
    bool IsInCSSContext() const;
    bool IsInJSContext() const;
    bool IsInHTMLContext() const;

private:
    struct BraceInfo {
        BraceType type;
        size_t line;
        size_t column;
    };

    std::stack<ScannerState> m_StateStack;
    std::stack<BraceInfo> m_BraceStack;
    size_t m_CurrentLine;
    size_t m_CurrentColumn;
    std::string m_CurrentElementName;
    std::string m_CurrentBlockType;
    char m_StringDelimiter;
    
    // 括号计数器
    int m_CurlyBraceCount;
    int m_SquareBraceCount;
    int m_ParenthesisCount;
};

} // namespace chtl