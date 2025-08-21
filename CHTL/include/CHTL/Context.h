#ifndef CHTL_CHTL_CONTEXT_H
#define CHTL_CHTL_CONTEXT_H

#include <string>
#include <memory>
#include <stack>
#include <unordered_map>
#include <vector>
#include "CHTL/Token.h"
#include "CHTL/State.h"

namespace CHTL {

// 前向声明
class GlobalMap;

// 上下文类型
enum class ContextType {
    Global,         // 全局上下文
    Element,        // 元素上下文
    Style,          // 样式上下文
    Script,         // 脚本上下文
    Template,       // 模板上下文
    Custom,         // 自定义上下文
    Namespace,      // 命名空间上下文
    Import,         // 导入上下文
    Configuration   // 配置上下文
};

// 上下文信息
struct ContextInfo {
    ContextType type;
    std::string name;                   // 上下文名称
    std::string parentName;             // 父上下文名称
    size_t startLine;                   // 开始行号
    size_t startColumn;                 // 开始列号
    size_t endLine;                     // 结束行号
    size_t endColumn;                   // 结束列号
    
    // 元素相关
    std::string elementType;            // 元素类型（div, span等）
    std::unordered_map<std::string, std::string> attributes;  // 属性
    std::vector<std::string> children;  // 子元素
    
    // 样式相关
    bool isLocalStyle = false;          // 是否为局部样式
    bool isGlobalStyle = false;         // 是否为全局样式
    std::string currentSelector;        // 当前选择器
    
    // 脚本相关
    bool isLocalScript = false;         // 是否为局部脚本
    bool isGlobalScript = false;        // 是否为全局脚本
    
    // 模板/自定义相关
    std::string templateType;           // 模板类型（@Style, @Element, @Var）
    bool isSpecialization = false;      // 是否在特例化中
    
    ContextInfo(ContextType t = ContextType::Global) 
        : type(t), startLine(0), startColumn(0), endLine(0), endColumn(0) {}
};

// 上下文管理器
class ContextManager {
public:
    ContextManager();
    ~ContextManager();
    
    // 上下文操作
    void PushContext(const ContextInfo& context);
    bool PopContext();
    ContextInfo& GetCurrentContext();
    const ContextInfo& GetCurrentContext() const;
    
    // 上下文查询
    ContextType GetCurrentContextType() const;
    std::string GetCurrentContextName() const;
    bool IsInContext(ContextType type) const;
    size_t GetContextDepth() const;
    std::vector<ContextInfo> GetContextStack() const;
    
    // 属性管理
    void SetAttribute(const std::string& key, const std::string& value);
    std::string GetAttribute(const std::string& key) const;
    bool HasAttribute(const std::string& key) const;
    void RemoveAttribute(const std::string& key);
    
    // 子元素管理
    void AddChild(const std::string& childName);
    std::vector<std::string> GetChildren() const;
    bool HasChild(const std::string& childName) const;
    
    // 样式上下文
    void EnterStyleContext(bool isLocal);
    void ExitStyleContext();
    void SetCurrentSelector(const std::string& selector);
    std::string GetCurrentSelector() const;
    bool IsInLocalStyle() const;
    bool IsInGlobalStyle() const;
    
    // 脚本上下文
    void EnterScriptContext(bool isLocal);
    void ExitScriptContext();
    bool IsInLocalScript() const;
    bool IsInGlobalScript() const;
    
    // 特例化上下文
    void EnterSpecialization();
    void ExitSpecialization();
    bool IsInSpecialization() const;
    
    // 符号表集成
    void SetGlobalMap(std::shared_ptr<GlobalMap> globalMap);
    std::shared_ptr<GlobalMap> GetGlobalMap() const;
    
    // 状态机集成
    void SetStateMachine(std::shared_ptr<StateMachine> stateMachine);
    std::shared_ptr<StateMachine> GetStateMachine() const;
    
    // 重置
    void Reset();
    
    // 调试
    void DumpContextStack() const;
    std::string GetContextPath() const;
    
private:
    class Impl;
    std::unique_ptr<Impl> impl_;
};

// 上下文工具类
class ContextUtils {
public:
    static std::string GetContextTypeName(ContextType type);
    static bool IsBlockContext(ContextType type);
    static bool CanContainElements(ContextType type);
    static bool CanContainStyles(ContextType type);
    static bool CanContainScripts(ContextType type);
    static bool RequiresBraces(ContextType type);
    
    // 创建特定上下文
    static ContextInfo CreateElementContext(const std::string& elementName);
    static ContextInfo CreateStyleContext(bool isLocal);
    static ContextInfo CreateScriptContext(bool isLocal);
    static ContextInfo CreateTemplateContext(const std::string& templateType);
    static ContextInfo CreateCustomContext(const std::string& customType);
    static ContextInfo CreateNamespaceContext(const std::string& namespaceName);
};

} // namespace CHTL

#endif // CHTL_CHTL_CONTEXT_H