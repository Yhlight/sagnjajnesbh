#pragma once
#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include "CHTL/Compiler/GlobalMap.h"
#include "CHTL/Compiler/State.h"
#include "CHTL/AST/ASTNode.h"

namespace CHTL {
namespace Compiler {

// 前向声明
struct Token;

// 编译上下文配置
struct ContextConfig {
    // 配置选项
    size_t indexInitialCount = 0;
    bool disableNameGroup = false;
    bool disableCustomOriginType = false;
    bool debugMode = false;
    
    // 关键字映射（从[Name]配置块加载）
    std::unordered_map<std::string, std::vector<std::string>> keywordMappings;
    
    // 自定义原始类型（从[OriginType]配置块加载）
    std::unordered_map<std::string, std::string> customOriginTypes;
};

// 局部作用域信息
struct LocalScope {
    std::string name;
    std::unordered_map<std::string, std::shared_ptr<ASTNode>> localVariables;
    std::vector<std::string> autoClasses;     // 自动生成的类名
    std::vector<std::string> autoIds;         // 自动生成的ID
    std::shared_ptr<LocalScope> parent;       // 父作用域
    
    // 查找局部变量
    std::shared_ptr<ASTNode> FindVariable(const std::string& name) const;
};

// CHTL编译上下文
class CompileContext {
public:
    CompileContext();
    ~CompileContext();
    
    // 配置管理
    void SetConfig(const ContextConfig& config);
    const ContextConfig& GetConfig() const;
    
    // 全局映射表访问
    GlobalMap& GetGlobalMap();
    const GlobalMap& GetGlobalMap() const;
    
    // 状态管理器访问
    StateManager& GetStateManager();
    const StateManager& GetStateManager() const;
    
    // 作用域管理
    void EnterScope(const std::string& name = "");
    void ExitScope();
    std::shared_ptr<LocalScope> GetCurrentScope() const;
    
    // 局部变量管理
    bool RegisterLocalVariable(const std::string& name, std::shared_ptr<ASTNode> node);
    std::shared_ptr<ASTNode> FindLocalVariable(const std::string& name) const;
    
    // 自动类名/ID生成
    std::string GenerateAutoClass();
    std::string GenerateAutoId();
    void RegisterAutoClass(const std::string& className);
    void RegisterAutoId(const std::string& id);
    
    // 错误和警告管理
    void AddError(const std::string& message, size_t line, size_t column);
    void AddWarning(const std::string& message, size_t line, size_t column);
    bool HasErrors() const;
    const std::vector<std::string>& GetErrors() const;
    const std::vector<std::string>& GetWarnings() const;
    
    // 文件信息
    void SetCurrentFile(const std::string& filePath);
    std::string GetCurrentFile() const;
    
    // 导入管理
    void AddImportedFile(const std::string& filePath);
    bool IsFileImported(const std::string& filePath) const;
    
    // 模块路径管理
    void AddModulePath(const std::string& path);
    std::vector<std::string> GetModulePaths() const;
    std::string ResolveModulePath(const std::string& moduleName, const std::string& extension) const;
    
    // 调试信息
    void DumpContext() const;
    
private:
    class Impl;
    std::unique_ptr<Impl> pImpl;
};

// 上下文守卫（RAII作用域管理）
class ScopeGuard {
public:
    ScopeGuard(CompileContext* context, const std::string& scopeName = "");
    ~ScopeGuard();
    
    // 禁止拷贝
    ScopeGuard(const ScopeGuard&) = delete;
    ScopeGuard& operator=(const ScopeGuard&) = delete;
    
    // 允许移动
    ScopeGuard(ScopeGuard&& other) noexcept;
    ScopeGuard& operator=(ScopeGuard&& other) noexcept;
    
private:
    CompileContext* context_;
    bool active_;
};

} // namespace Compiler  
} // namespace CHTL