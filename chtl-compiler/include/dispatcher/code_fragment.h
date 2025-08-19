#ifndef CHTL_CODE_FRAGMENT_H
#define CHTL_CODE_FRAGMENT_H

#include <string>
#include <memory>

namespace chtl {

/**
 * 代码片段类型
 */
enum class FragmentType {
    CHTL,           // 普通CHTL代码
    CHTL_JS,        // CHTL JS代码
    CSS,            // CSS代码（全局样式）
    JAVASCRIPT,     // JavaScript代码
    LOCAL_STYLE,    // 局部样式（由CHTL编译器处理）
    ORIGIN_HTML,    // 原始HTML代码
    ORIGIN_CSS,     // 原始CSS代码
    ORIGIN_JS       // 原始JavaScript代码
};

/**
 * 代码片段基类
 */
class CodeFragment {
public:
    CodeFragment(FragmentType type, const std::string& content, 
                 size_t startLine, size_t startColumn)
        : type_(type), content_(content), 
          startLine_(startLine), startColumn_(startColumn) {}
    
    virtual ~CodeFragment() = default;
    
    // 获取片段类型
    FragmentType getType() const { return type_; }
    
    // 获取片段内容
    const std::string& getContent() const { return content_; }
    
    // 获取起始位置
    size_t getStartLine() const { return startLine_; }
    size_t getStartColumn() const { return startColumn_; }
    
    // 设置编译结果
    void setCompiledResult(const std::string& result) { compiledResult_ = result; }
    const std::string& getCompiledResult() const { return compiledResult_; }
    
    // 获取片段类型的字符串表示
    static std::string fragmentTypeToString(FragmentType type);
    
protected:
    FragmentType type_;
    std::string content_;
    size_t startLine_;
    size_t startColumn_;
    std::string compiledResult_;
};

/**
 * CHTL代码片段
 */
class CHTLFragment : public CodeFragment {
public:
    CHTLFragment(const std::string& content, size_t startLine, size_t startColumn)
        : CodeFragment(FragmentType::CHTL, content, startLine, startColumn) {}
};

/**
 * CHTL JS代码片段
 */
class CHTLJSFragment : public CodeFragment {
public:
    CHTLJSFragment(const std::string& content, size_t startLine, size_t startColumn)
        : CodeFragment(FragmentType::CHTL_JS, content, startLine, startColumn) {}
};

/**
 * CSS代码片段
 */
class CSSFragment : public CodeFragment {
public:
    CSSFragment(const std::string& content, size_t startLine, size_t startColumn)
        : CodeFragment(FragmentType::CSS, content, startLine, startColumn) {}
};

/**
 * JavaScript代码片段
 */
class JavaScriptFragment : public CodeFragment {
public:
    JavaScriptFragment(const std::string& content, size_t startLine, size_t startColumn)
        : CodeFragment(FragmentType::JAVASCRIPT, content, startLine, startColumn) {}
};

/**
 * 局部样式片段
 */
class LocalStyleFragment : public CodeFragment {
public:
    LocalStyleFragment(const std::string& content, size_t startLine, size_t startColumn)
        : CodeFragment(FragmentType::LOCAL_STYLE, content, startLine, startColumn) {}
};

} // namespace chtl

#endif // CHTL_CODE_FRAGMENT_H