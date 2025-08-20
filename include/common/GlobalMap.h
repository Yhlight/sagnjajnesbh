#ifndef CHTL_COMMON_GLOBAL_MAP_H
#define CHTL_COMMON_GLOBAL_MAP_H

#include <string>
#include <unordered_map>
#include <unordered_set>
#include <memory>
#include <vector>
#include <mutex>

namespace chtl {

// 符号类型
enum class SymbolType {
    UNKNOWN,
    HTML_ELEMENT,           // HTML元素
    CSS_PROPERTY,           // CSS属性
    TEMPLATE_STYLE,         // 模板样式组
    TEMPLATE_ELEMENT,       // 模板元素
    TEMPLATE_VAR,           // 模板变量组
    CUSTOM_STYLE,           // 自定义样式组
    CUSTOM_ELEMENT,         // 自定义元素
    CUSTOM_VAR,             // 自定义变量组
    ORIGIN_BLOCK,           // 原始嵌入块
    NAMESPACE,              // 命名空间
    CONFIGURATION,          // 配置
    IMPORT,                 // 导入项
    LOCAL_CLASS,            // 局部类名
    LOCAL_ID,               // 局部ID
    CHTLJS_FUNCTION,        // CHTL JS函数
    VIR_OBJECT              // 虚对象
};

// 符号信息
struct SymbolInfo {
    SymbolType type;
    std::string name;
    std::string fullName;       // 完整名称（包含命名空间）
    std::string sourceFile;     // 定义所在文件
    size_t definitionLine;      // 定义所在行
    std::unordered_map<std::string, std::string> attributes;  // 额外属性
    
    SymbolInfo(SymbolType t, const std::string& n)
        : type(t), name(n), fullName(n), definitionLine(0) {}
};

// 符号表
class SymbolTable {
private:
    std::unordered_map<std::string, std::shared_ptr<SymbolInfo>> symbols_;
    std::weak_ptr<SymbolTable> parent_;  // 父符号表
    std::string scopeName_;
    
public:
    explicit SymbolTable(const std::string& scope = "")
        : scopeName_(scope) {}
    
    // 设置父符号表
    void setParent(std::shared_ptr<SymbolTable> p) {
        parent_ = p;
    }
    
    // 添加符号
    bool addSymbol(const std::string& name, std::shared_ptr<SymbolInfo> info);
    
    // 查找符号（包括父作用域）
    std::shared_ptr<SymbolInfo> lookupSymbol(const std::string& name) const;
    
    // 查找符号（仅当前作用域）
    std::shared_ptr<SymbolInfo> lookupLocal(const std::string& name) const;
    
    // 获取所有符号
    std::vector<std::shared_ptr<SymbolInfo>> getAllSymbols() const;
    
    // 获取特定类型的符号
    std::vector<std::shared_ptr<SymbolInfo>> getSymbolsByType(SymbolType type) const;
    
    const std::string& getScopeName() const { return scopeName_; }
};

// 全局映射表
class GlobalMap {
private:
    // HTML元素集合
    std::unordered_set<std::string> htmlElements_;
    
    // CSS属性集合
    std::unordered_set<std::string> cssProperties_;
    
    // CHTL关键字映射
    std::unordered_map<std::string, std::string> keywordMap_;
    
    // 全局符号表
    std::shared_ptr<SymbolTable> globalSymbolTable_;
    
    // 自动生成的名称计数器
    std::unordered_map<std::string, size_t> nameCounters_;
    
    // 线程安全
    mutable std::mutex mutex_;
    
    // 初始化标准HTML元素和CSS属性
    void initializeStandardElements();
    void initializeStandardProperties();
    void initializeKeywords();
    
public:
    GlobalMap();
    
    // HTML元素相关
    bool isHTMLElement(const std::string& name) const;
    void registerHTMLElement(const std::string& name);
    std::vector<std::string> getAllHTMLElements() const;
    
    // CSS属性相关
    bool isCSSProperty(const std::string& name) const;
    void registerCSSProperty(const std::string& name);
    std::vector<std::string> getAllCSSProperties() const;
    
    // 关键字映射
    std::string mapKeyword(const std::string& keyword) const;
    void registerKeywordMapping(const std::string& from, const std::string& to);
    
    // 符号表相关
    std::shared_ptr<SymbolTable> getGlobalSymbolTable() { return globalSymbolTable_; }
    const std::shared_ptr<SymbolTable> getGlobalSymbolTable() const { return globalSymbolTable_; }
    
    // 生成唯一名称
    std::string generateUniqueName(const std::string& prefix);
    
    // 重置
    void reset();
    
    // 单例模式
    static GlobalMap& getInstance();
};

// HTML标准元素列表（部分）
const std::vector<std::string> STANDARD_HTML_ELEMENTS = {
    // 文档结构
    "html", "head", "body", "title", "meta", "link", "style", "script",
    
    // 内容分区
    "header", "nav", "main", "section", "article", "aside", "footer",
    "h1", "h2", "h3", "h4", "h5", "h6", "address",
    
    // 文本内容
    "div", "p", "hr", "pre", "blockquote", "ol", "ul", "li", "dl", "dt", "dd",
    "figure", "figcaption",
    
    // 内联文本语义
    "a", "em", "strong", "small", "s", "cite", "q", "dfn", "abbr", "data",
    "time", "code", "var", "samp", "kbd", "sub", "sup", "i", "b", "u",
    "mark", "ruby", "rt", "rp", "bdi", "bdo", "span", "br", "wbr",
    
    // 图片和多媒体
    "img", "iframe", "embed", "object", "param", "video", "audio", "source",
    "track", "map", "area", "svg", "math",
    
    // 表格
    "table", "caption", "colgroup", "col", "tbody", "thead", "tfoot",
    "tr", "td", "th",
    
    // 表单
    "form", "label", "input", "button", "select", "datalist", "optgroup",
    "option", "textarea", "output", "progress", "meter", "fieldset", "legend",
    
    // 交互元素
    "details", "summary", "dialog", "menu",
    
    // Web组件
    "slot", "template"
};

// CSS标准属性列表（部分）
const std::vector<std::string> STANDARD_CSS_PROPERTIES = {
    // 布局
    "display", "position", "top", "right", "bottom", "left", "z-index",
    "float", "clear", "overflow", "overflow-x", "overflow-y",
    
    // 盒模型
    "width", "height", "max-width", "max-height", "min-width", "min-height",
    "margin", "margin-top", "margin-right", "margin-bottom", "margin-left",
    "padding", "padding-top", "padding-right", "padding-bottom", "padding-left",
    "border", "border-width", "border-style", "border-color",
    "border-top", "border-right", "border-bottom", "border-left",
    "border-radius", "box-sizing",
    
    // 背景
    "background", "background-color", "background-image", "background-repeat",
    "background-position", "background-size", "background-attachment",
    
    // 文本
    "color", "font", "font-family", "font-size", "font-weight", "font-style",
    "line-height", "text-align", "text-decoration", "text-transform",
    "text-indent", "letter-spacing", "word-spacing", "white-space",
    
    // Flexbox
    "flex", "flex-direction", "flex-wrap", "flex-flow", "justify-content",
    "align-items", "align-content", "align-self", "order", "flex-grow",
    "flex-shrink", "flex-basis",
    
    // Grid
    "grid", "grid-template", "grid-template-columns", "grid-template-rows",
    "grid-template-areas", "grid-column", "grid-row", "gap", "column-gap", "row-gap",
    
    // 动画和过渡
    "transition", "transition-property", "transition-duration",
    "transition-timing-function", "transition-delay",
    "animation", "animation-name", "animation-duration",
    "animation-timing-function", "animation-delay", "animation-iteration-count",
    "animation-direction", "animation-fill-mode", "animation-play-state",
    "transform", "transform-origin",
    
    // 其他
    "opacity", "visibility", "cursor", "user-select", "pointer-events",
    "content", "list-style", "vertical-align", "box-shadow", "text-shadow"
};

} // namespace chtl

#endif // CHTL_COMMON_GLOBAL_MAP_H