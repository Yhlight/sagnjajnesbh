#pragma once
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <memory>
#include <stack>
#include <functional>

namespace chtl {

// 符号类型
enum class SymbolType {
    TEMPLATE_STYLE,     // 模板样式组
    TEMPLATE_ELEMENT,   // 模板元素
    TEMPLATE_VAR,       // 模板变量组
    CUSTOM_STYLE,       // 自定义样式组
    CUSTOM_ELEMENT,     // 自定义元素
    CUSTOM_VAR,         // 自定义变量组
    ORIGIN_BLOCK,       // 原始嵌入块
    NAMESPACE,          // 命名空间
    VARIABLE,           // 普通变量
    FUNCTION,           // 函数
    HTML_ELEMENT        // HTML元素
};

// 符号信息
struct Symbol {
    std::string name;           // 符号名称
    SymbolType type;            // 符号类型
    std::string value;          // 符号值
    std::string namespace_path; // 命名空间路径
    size_t line;                // 定义行号
    size_t column;              // 定义列号
    bool is_exported;           // 是否导出
    std::unordered_map<std::string, std::string> attributes; // 附加属性
    
    Symbol(const std::string& n, SymbolType t, const std::string& v = "",
           const std::string& ns = "", size_t l = 0, size_t c = 0)
        : name(n), type(t), value(v), namespace_path(ns), 
          line(l), column(c), is_exported(false) {}
};

// 作用域类
class Scope {
public:
    std::string name;           // 作用域名称
    Scope* parent;              // 父作用域
    std::vector<std::unique_ptr<Scope>> children; // 子作用域
    std::unordered_map<std::string, std::shared_ptr<Symbol>> symbols; // 符号表
    
    Scope(const std::string& scope_name, Scope* parent_scope = nullptr)
        : name(scope_name), parent(parent_scope) {}
    
    // 添加符号
    bool addSymbol(std::shared_ptr<Symbol> symbol);
    
    // 查找符号（仅在当前作用域）
    std::shared_ptr<Symbol> findSymbol(const std::string& name);
    
    // 查找符号（递归查找父作用域）
    std::shared_ptr<Symbol> findSymbolRecursive(const std::string& name);
    
    // 创建子作用域
    Scope* createChildScope(const std::string& scope_name);
    
    // 获取完整路径
    std::string getFullPath() const;
};

// 全局映射管理器
class GlobalMap {
public:
    GlobalMap();
    ~GlobalMap();
    
    // 作用域管理
    void enterScope(const std::string& scope_name);
    void exitScope();
    Scope* getCurrentScope();
    const Scope* getCurrentScope() const;
    Scope* getRootScope();
    const Scope* getRootScope() const;
    
    // 符号管理
    bool addSymbol(const std::string& name, SymbolType type, 
                   const std::string& value = "", size_t line = 0, size_t column = 0);
    std::shared_ptr<Symbol> findSymbol(const std::string& name);
    std::shared_ptr<Symbol> findSymbolInNamespace(const std::string& namespace_path, 
                                                   const std::string& name);
    
    // 命名空间管理
    void enterNamespace(const std::string& namespace_name);
    void exitNamespace();
    std::string getCurrentNamespace() const;
    bool isNamespaceExists(const std::string& namespace_path);
    
    // 导入管理
    struct ImportInfo {
        std::string source_path;    // 源路径
        std::string alias;          // 别名
        std::vector<std::string> imported_symbols; // 导入的符号
        bool import_all;            // 是否导入全部
    };
    
    void addImport(const ImportInfo& import_info);
    std::vector<ImportInfo> getImports() const;
    
    // 模板和自定义管理
    void registerTemplate(const std::string& name, SymbolType type, 
                          const std::string& content);
    void registerCustom(const std::string& name, SymbolType type, 
                        const std::string& content);
    
    // 冲突检测
    bool hasConflict(const std::string& name, SymbolType type);
    std::vector<std::shared_ptr<Symbol>> getConflicts(const std::string& name);
    
    // 循环依赖检测
    bool hasCyclicDependency(const std::string& target_path);
    void addDependency(const std::string& from_path, const std::string& to_path);
    
    // 导出管理
    void exportSymbol(const std::string& name);
    std::vector<std::shared_ptr<Symbol>> getExportedSymbols();
    
    // 调试和诊断
    void printSymbolTable() const;
    void printScopeHierarchy() const;
    std::string getStatistics() const;
    
    // 清理
    void clear();
    
private:
    std::unique_ptr<Scope> root_scope_;         // 根作用域
    Scope* current_scope_;                      // 当前作用域
    std::stack<std::string> namespace_stack_;   // 命名空间栈
    std::vector<ImportInfo> imports_;           // 导入信息
    std::unordered_map<std::string, std::vector<std::string>> dependencies_; // 依赖关系
    
    // 内部辅助方法
    void printScope(const Scope* scope, int indent = 0) const;
    bool detectCyclicDependencyHelper(const std::string& current, 
                                     const std::string& target,
                                     std::unordered_set<std::string>& visited) const;
};

} // namespace chtl