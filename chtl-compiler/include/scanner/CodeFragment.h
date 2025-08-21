#pragma once

#include <string>
#include <vector>

namespace chtl {

/**
 * 代码片段类型
 */
enum class FragmentType {
    CHTL,           // CHTL 代码片段
    CHTL_JS,        // CHTL JS 代码片段（局部script块中的内容）
    CSS,            // 纯 CSS 代码片段
    JAVASCRIPT,     // 纯 JavaScript 代码片段
    HTML,           // HTML 代码片段
    TEXT,           // 文本内容
    UNKNOWN         // 未知类型
};

/**
 * 代码片段
 * 表示源代码中的一个片段，包含类型、内容和位置信息
 */
class CodeFragment {
public:
    CodeFragment(FragmentType type, const std::string& content, 
                 size_t startLine, size_t startColumn,
                 size_t endLine, size_t endColumn)
        : m_Type(type)
        , m_Content(content)
        , m_StartLine(startLine)
        , m_StartColumn(startColumn)
        , m_EndLine(endLine)
        , m_EndColumn(endColumn) {}

    // Getters
    FragmentType GetType() const { return m_Type; }
    const std::string& GetContent() const { return m_Content; }
    size_t GetStartLine() const { return m_StartLine; }
    size_t GetStartColumn() const { return m_StartColumn; }
    size_t GetEndLine() const { return m_EndLine; }
    size_t GetEndColumn() const { return m_EndColumn; }

    // 设置父片段（用于嵌套结构）
    void SetParent(CodeFragment* parent) { m_Parent = parent; }
    CodeFragment* GetParent() const { return m_Parent; }

    // 添加子片段
    void AddChild(const CodeFragment& child) { m_Children.push_back(child); }
    const std::vector<CodeFragment>& GetChildren() const { return m_Children; }

    // 获取片段的字符串表示（用于调试）
    std::string ToString() const;

    // 判断是否是特定的CHTL结构
    bool IsCHTLElement() const;
    bool IsCHTLTemplate() const;
    bool IsCHTLCustom() const;
    bool IsCHTLImport() const;
    bool IsCHTLConfiguration() const;
    bool IsCHTLNamespace() const;
    bool IsCHTLOrigin() const;
    bool IsLocalStyle() const;
    bool IsLocalScript() const;

private:
    FragmentType m_Type;
    std::string m_Content;
    size_t m_StartLine;
    size_t m_StartColumn;
    size_t m_EndLine;
    size_t m_EndColumn;
    CodeFragment* m_Parent = nullptr;
    std::vector<CodeFragment> m_Children;
};

} // namespace chtl