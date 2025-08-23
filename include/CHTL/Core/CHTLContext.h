#pragma once

#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <memory>
#include <stack>

namespace CHTL {
namespace Core {

/**
 * @brief CHTL编译上下文类型
 */
enum class ContextType {
    GLOBAL,                 // 全局上下文
    NAMESPACE,              // 命名空间上下文
    TEMPLATE_STYLE,         // 样式组模板上下文
    TEMPLATE_ELEMENT,       // 元素模板上下文
    TEMPLATE_VAR,           // 变量组模板上下文
    CUSTOM_STYLE,           // 自定义样式组上下文
    CUSTOM_ELEMENT,         // 自定义元素上下文
    CUSTOM_VAR,             // 自定义变量组上下文
    ORIGIN_HTML,            // 原始HTML嵌入上下文
    ORIGIN_STYLE,           // 原始样式嵌入上下文
    ORIGIN_JAVASCRIPT,      // 原始JavaScript嵌入上下文
    ORIGIN_CUSTOM,          // 自定义类型原始嵌入上下文
    CONFIGURATION,          // 配置上下文
    CONFIGURATION_CONFIG,   // 配置组上下文
    IMPORT,                 // 导入上下文
    HTML_ELEMENT,           // HTML元素上下文
    STYLE_BLOCK,            // 样式块上下文
    SCRIPT_BLOCK,           // 脚本块上下文
    INHERITANCE,            // 继承上下文
    SPECIALIZATION,         // 特化上下文
    DELETION,               // 删除上下文
    INSERTION,              // 插入上下文
    INDEX_ACCESS,           // 索引访问上下文
    CONSTRAINT              // 约束上下文
};

/**
 * @brief CHTL上下文范围
 */
enum class ContextScope {
    FILE,                   // 文件范围
    NAMESPACE,              // 命名空间范围
    TEMPLATE,               // 模板范围
    CUSTOM,                 // 自定义范围
    ELEMENT,                // 元素范围
    BLOCK,                  // 块范围
    LOCAL                   // 局部范围
};

/**
 * @brief CHTL符号信息
 */
struct CHTLSymbol {
    std::string name;           // 符号名称
    std::string fullName;       // 完整名称（包含命名空间）
    ContextType contextType;    // 上下文类型
    ContextScope scope;         // 作用域
    std::string value;          // 符号值
    std::string sourceFile;     // 来源文件
    int lineNumber;             // 行号
    bool isExported;            // 是否导出
    bool isImported;            // 是否导入
    std::unordered_map<std::string, std::string> attributes; // 属性
};

/**
 * @brief CHTL上下文栈项
 */
struct ContextStackItem {
    ContextType type;           // 上下文类型
    ContextScope scope;         // 作用域
    std::string name;           // 上下文名称
    std::unordered_map<std::string, std::string> variables; // 局部变量
    std::unordered_map<std::string, CHTLSymbol> symbols;    // 局部符号
    int depth;                  // 嵌套深度
};

/**
 * @brief CHTL编译上下文管理器
 */
class CHTLContext {
public:
    /**
     * @brief 构造函数
     * @param sourceFile 源文件路径
     */
    CHTLContext(const std::string& sourceFile = "");
    
    /**
     * @brief 析构函数
     */
    ~CHTLContext();
    
    /**
     * @brief 进入新的上下文
     * @param type 上下文类型
     * @param name 上下文名称
     * @param scope 作用域
     */
    void EnterContext(ContextType type, const std::string& name = "", ContextScope scope = ContextScope::LOCAL);
    
    /**
     * @brief 退出当前上下文
     */
    void ExitContext();
    
    /**
     * @brief 获取当前上下文类型
     * @return 当前上下文类型
     */
    ContextType GetCurrentContextType() const;
    
    /**
     * @brief 获取当前上下文名称
     * @return 当前上下文名称
     */
    std::string GetCurrentContextName() const;
    
    /**
     * @brief 获取当前作用域
     * @return 当前作用域
     */
    ContextScope GetCurrentScope() const;
    
    /**
     * @brief 获取上下文深度
     * @return 上下文嵌套深度
     */
    int GetContextDepth() const;
    
    /**
     * @brief 添加符号
     * @param symbol 符号信息
     */
    void AddSymbol(const CHTLSymbol& symbol);
    
    /**
     * @brief 查找符号
     * @param name 符号名称
     * @param searchGlobal 是否搜索全局
     * @return 符号信息指针，未找到返回nullptr
     */
    const CHTLSymbol* FindSymbol(const std::string& name, bool searchGlobal = true) const;
    
    /**
     * @brief 添加变量
     * @param name 变量名
     * @param value 变量值
     */
    void AddVariable(const std::string& name, const std::string& value);
    
    /**
     * @brief 获取变量值
     * @param name 变量名
     * @return 变量值，未找到返回空字符串
     */
    std::string GetVariable(const std::string& name) const;
    
    /**
     * @brief 检查变量是否存在
     * @param name 变量名
     * @return 是否存在
     */
    bool HasVariable(const std::string& name) const;
    
    /**
     * @brief 设置当前命名空间
     * @param namespaceName 命名空间名称
     */
    void SetCurrentNamespace(const std::string& namespaceName);
    
    /**
     * @brief 获取当前命名空间
     * @return 当前命名空间名称
     */
    std::string GetCurrentNamespace() const;
    
    /**
     * @brief 获取完整符号名称
     * @param symbolName 符号名称
     * @return 包含命名空间的完整名称
     */
    std::string GetFullSymbolName(const std::string& symbolName) const;
    
    /**
     * @brief 检查符号是否在当前作用域
     * @param symbolName 符号名称
     * @return 是否在当前作用域
     */
    bool IsSymbolInCurrentScope(const std::string& symbolName) const;
    
    /**
     * @brief 获取当前上下文的所有符号
     * @return 符号映射
     */
    std::unordered_map<std::string, CHTLSymbol> GetCurrentContextSymbols() const;
    
    /**
     * @brief 获取全局符号表
     * @return 全局符号映射
     */
    const std::unordered_map<std::string, CHTLSymbol>& GetGlobalSymbols() const;
    
    /**
     * @brief 检查上下文是否允许特定操作
     * @param operation 操作类型
     * @return 是否允许
     */
    bool IsOperationAllowed(const std::string& operation) const;
    
    /**
     * @brief 添加导入记录
     * @param importPath 导入路径
     * @param importType 导入类型
     */
    void AddImportRecord(const std::string& importPath, const std::string& importType);
    
    /**
     * @brief 获取导入记录
     * @return 导入记录列表
     */
    std::vector<std::pair<std::string, std::string>> GetImportRecords() const;
    
    /**
     * @brief 重置上下文
     */
    void Reset();
    
    /**
     * @brief 获取上下文状态字符串
     * @return 状态描述
     */
    std::string GetContextStatus() const;

private:
    std::string sourceFile_;                                    // 源文件路径
    std::string currentNamespace_;                              // 当前命名空间
    std::stack<ContextStackItem> contextStack_;                 // 上下文栈
    std::unordered_map<std::string, CHTLSymbol> globalSymbols_; // 全局符号表
    std::vector<std::pair<std::string, std::string>> importRecords_; // 导入记录
    
    /**
     * @brief 获取当前上下文栈项
     * @return 当前上下文栈项引用
     */
    ContextStackItem& GetCurrentContext();
    
    /**
     * @brief 获取当前上下文栈项（只读）
     * @return 当前上下文栈项常量引用
     */
    const ContextStackItem& GetCurrentContext() const;
    
    /**
     * @brief 检查上下文栈是否为空
     * @return 是否为空
     */
    bool IsContextStackEmpty() const;
};

} // namespace Core
} // namespace CHTL