#ifndef CHTL_CHTL_GLOBALMAP_H
#define CHTL_CHTL_GLOBALMAP_H

#include <string>
#include <unordered_map>
#include <vector>
#include <memory>
#include <optional>

namespace CHTL {

// 符号类型
enum class SymbolType {
    Element,            // HTML元素
    TemplateStyle,      // 模板样式组
    TemplateElement,    // 模板元素
    TemplateVar,        // 模板变量组
    CustomStyle,        // 自定义样式组
    CustomElement,      // 自定义元素
    CustomVar,          // 自定义变量组
    OriginBlock,        // 原始嵌入块
    Namespace,          // 命名空间
    Configuration,      // 配置
    ImportedSymbol,     // 导入的符号
    LocalVariable,      // 局部变量
    Unknown
};

// 符号信息
struct Symbol {
    std::string name;                   // 符号名称
    SymbolType type;                    // 符号类型
    std::string fullName;               // 全限定名（包含命名空间）
    std::string sourceFile;             // 定义所在文件
    size_t line;                        // 定义行号
    size_t column;                      // 定义列号
    
    // 额外信息
    std::unordered_map<std::string, std::string> attributes;  // 属性
    std::vector<std::string> children;                        // 子元素
    std::string parentNamespace;                               // 父命名空间
    bool isExported = false;                                   // 是否导出
    bool isImported = false;                                   // 是否导入
    
    Symbol() : type(SymbolType::Unknown), line(0), column(0) {}
    
    Symbol(const std::string& n, SymbolType t, const std::string& file, 
           size_t l, size_t c)
        : name(n), type(t), fullName(n), sourceFile(file), 
          line(l), column(c) {}
};

// 命名空间信息
struct NamespaceInfo {
    std::string name;
    std::string parentNamespace;
    std::vector<std::string> childNamespaces;
    std::vector<std::string> symbols;
    std::unordered_map<std::string, std::string> aliases;  // 导入别名
};

// 全局符号映射表
class GlobalMap {
public:
    GlobalMap();
    ~GlobalMap();
    
    // 符号管理
    bool AddSymbol(const Symbol& symbol);
    bool RemoveSymbol(const std::string& fullName);
    std::optional<Symbol> FindSymbol(const std::string& name) const;
    std::optional<Symbol> FindSymbolInNamespace(const std::string& name, 
                                                const std::string& namespaceName) const;
    
    // 命名空间管理
    bool CreateNamespace(const std::string& name, const std::string& parent = "");
    bool EnterNamespace(const std::string& name);
    bool ExitNamespace();
    std::string GetCurrentNamespace() const;
    std::vector<std::string> GetNamespaceHierarchy() const;
    
    // 导入管理
    bool ImportSymbol(const std::string& symbolName, 
                     const std::string& fromNamespace,
                     const std::string& asName = "");
    bool ImportAll(const std::string& fromNamespace);
    
    // 查询功能
    std::vector<Symbol> GetSymbolsByType(SymbolType type) const;
    std::vector<Symbol> GetSymbolsInNamespace(const std::string& namespaceName) const;
    std::vector<std::string> GetAllNamespaces() const;
    bool HasSymbol(const std::string& name) const;
    bool HasNamespace(const std::string& name) const;
    
    // 冲突检测
    bool CheckNameConflict(const std::string& name) const;
    std::vector<std::string> GetConflictingSymbols(const std::string& name) const;
    
    // 作用域管理
    void PushScope();
    void PopScope();
    
    // 清理
    void Clear();
    void ClearNamespace(const std::string& namespaceName);
    
    // 调试
    void DumpSymbols() const;
    void DumpNamespaces() const;
    
private:
    // 内部实现
    class Impl;
    std::unique_ptr<Impl> impl_;
    
    // 辅助方法
    std::string BuildFullName(const std::string& name) const;
    std::string ResolveSymbolName(const std::string& name) const;
};

// 符号表工具
class SymbolTableUtils {
public:
    static std::string GetSymbolTypeName(SymbolType type);
    static bool IsTemplateType(SymbolType type);
    static bool IsCustomType(SymbolType type);
    static bool CanBeImported(SymbolType type);
    static bool CanBeExported(SymbolType type);
};

} // namespace CHTL

#endif // CHTL_CHTL_GLOBALMAP_H