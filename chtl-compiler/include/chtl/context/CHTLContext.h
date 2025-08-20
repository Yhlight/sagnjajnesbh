#pragma once

#include <string>
#include <vector>
#include <memory>
#include <stack>
#include <unordered_map>

namespace chtl {
namespace compiler {

// 前向声明
class CHTLElement;
class CHTLAttribute;
class CHTLStyleRule;
class CHTLImportInfo;

/**
 * 作用域类型
 */
enum class CHTLScopeType {
    GLOBAL,           // 全局作用域
    NAMESPACE,        // 命名空间作用域
    ELEMENT,          // 元素作用域
    STYLE_BLOCK,      // 样式块作用域
    TEMPLATE,         // 模板作用域
    CUSTOM,           // 自定义作用域
    CONFIGURATION     // 配置作用域
};

/**
 * 作用域信息
 */
struct CHTLScope {
    CHTLScopeType type;
    std::string name;
    std::unordered_map<std::string, std::string> symbols;  // 符号表
    
    CHTLScope(CHTLScopeType t, const std::string& n = "")
        : type(t), name(n) {}
};

/**
 * CHTL 编译上下文
 * 管理编译过程中的上下文信息
 * 注意：这是CHTL专用的上下文，不与CHTL JS共享
 */
class CHTLContext {
public:
    CHTLContext();
    ~CHTLContext();
    
    // 作用域管理
    void PushScope(CHTLScopeType type, const std::string& name = "");
    void PopScope();
    CHTLScope* GetCurrentScope();
    const CHTLScope* GetCurrentScope() const;
    
    // 符号管理
    void DefineSymbol(const std::string& name, const std::string& value);
    bool IsSymbolDefined(const std::string& name) const;
    std::string GetSymbolValue(const std::string& name) const;
    
    // 元素上下文
    void SetCurrentElement(const std::string& name);
    std::string GetCurrentElement() const;
    void PushElementContext(std::shared_ptr<CHTLElement> element);
    void PopElementContext();
    std::shared_ptr<CHTLElement> GetCurrentElementContext() const;
    
    // 属性上下文
    void AddAttribute(const std::string& name, const std::string& value);
    std::vector<std::pair<std::string, std::string>> GetCurrentAttributes() const;
    void ClearAttributes();
    
    // 样式上下文
    void EnterStyleBlock(bool isLocal = false);
    void ExitStyleBlock();
    bool IsInLocalStyleBlock() const;
    bool IsInGlobalStyleBlock() const;
    void AddStyleRule(std::shared_ptr<CHTLStyleRule> rule);
    
    // 脚本上下文（注意：只标记位置，内容由CHTL JS处理）
    void EnterScriptBlock(bool isLocal = false);
    void ExitScriptBlock();
    bool IsInLocalScriptBlock() const;
    bool IsInGlobalScriptBlock() const;
    
    // 导入上下文
    void AddImport(std::shared_ptr<CHTLImportInfo> import);
    std::vector<std::shared_ptr<CHTLImportInfo>> GetImports() const;
    
    // 错误上下文
    void SetCurrentFile(const std::string& filename);
    std::string GetCurrentFile() const;
    void SetCurrentLine(size_t line);
    size_t GetCurrentLine() const;
    void SetCurrentColumn(size_t column);
    size_t GetCurrentColumn() const;
    
    // 配置上下文
    void SetConfigValue(const std::string& key, const std::string& value);
    std::string GetConfigValue(const std::string& key) const;
    bool HasConfigValue(const std::string& key) const;
    
    // 清理
    void Clear();
    
    // 调试
    void DumpScopes() const;
    std::string GetContextTrace() const;
    
private:
    // 作用域栈
    std::stack<CHTLScope> m_ScopeStack;
    
    // 元素上下文栈
    std::stack<std::shared_ptr<CHTLElement>> m_ElementStack;
    std::string m_CurrentElementName;
    
    // 属性临时存储
    std::vector<std::pair<std::string, std::string>> m_CurrentAttributes;
    
    // 样式和脚本上下文
    int m_LocalStyleDepth;
    int m_GlobalStyleDepth;
    int m_LocalScriptDepth;
    int m_GlobalScriptDepth;
    std::vector<std::shared_ptr<CHTLStyleRule>> m_StyleRules;
    
    // 导入信息
    std::vector<std::shared_ptr<CHTLImportInfo>> m_Imports;
    
    // 位置信息
    std::string m_CurrentFile;
    size_t m_CurrentLine;
    size_t m_CurrentColumn;
    
    // 配置信息
    std::unordered_map<std::string, std::string> m_ConfigValues;
    
    // 辅助方法
    std::string* FindSymbolInScopes(const std::string& name);
    const std::string* FindSymbolInScopes(const std::string& name) const;
};

/**
 * RAII 作用域管理器
 */
class CHTLScopeGuard {
public:
    CHTLScopeGuard(CHTLContext& context, CHTLScopeType type,
                   const std::string& name = "")
        : m_Context(context)
        , m_Pushed(true) {
        m_Context.PushScope(type, name);
    }
    
    ~CHTLScopeGuard() {
        if (m_Pushed) {
            m_Context.PopScope();
        }
    }
    
    // 禁止拷贝
    CHTLScopeGuard(const CHTLScopeGuard&) = delete;
    CHTLScopeGuard& operator=(const CHTLScopeGuard&) = delete;
    
    // 允许移动
    CHTLScopeGuard(CHTLScopeGuard&& other) noexcept
        : m_Context(other.m_Context)
        , m_Pushed(other.m_Pushed) {
        other.m_Pushed = false;
    }
    
    void Dismiss() {
        m_Pushed = false;
    }
    
private:
    CHTLContext& m_Context;
    bool m_Pushed;
};

} // namespace compiler
} // namespace chtl