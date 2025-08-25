#pragma once
#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include "CHTLJS/Compiler/GlobalMap.h"
#include "CHTLJS/Compiler/State.h"

namespace CHTL {
namespace JSCompiler {

// 前向声明
struct Token;

// CHTL JS编译上下文配置
struct ContextConfig {
    bool enableVirObjects = true;         // 是否启用虚对象
    bool enableEventDelegation = true;    // 是否启用事件委托
    bool enableAnimations = true;         // 是否启用动画API
    bool debugMode = false;               // 调试模式
    
    // 生成选项
    bool generateSourceMap = false;       // 是否生成source map
    bool minifyOutput = false;           // 是否压缩输出
};

// 选择器解析结果
struct SelectorInfo {
    enum Type {
        TAG,        // 标签选择器
        CLASS,      // 类选择器
        ID,         // ID选择器
        COMPLEX     // 复合选择器
    };
    
    Type type;
    std::string value;
    std::string index;  // 可选的索引，如 [0]
};

// CHTL JS编译上下文
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
    
    // 选择器解析
    SelectorInfo ParseSelector(const std::string& selector);
    std::string GenerateQuerySelector(const SelectorInfo& info);
    
    // 虚对象管理
    bool RegisterCurrentVirObject(const std::string& name);
    std::string GetCurrentVirObject() const;
    
    // 函数名生成
    std::string GenerateUniqueFunctionName(const std::string& prefix = "");
    
    // 事件委托管理
    void RegisterEventDelegate(const std::string& parent, 
                              const std::string& target,
                              const std::string& event);
    std::string GenerateDelegateCode();
    
    // 错误和警告管理
    void AddError(const std::string& message, size_t line, size_t column);
    void AddWarning(const std::string& message, size_t line, size_t column);
    bool HasErrors() const;
    const std::vector<std::string>& GetErrors() const;
    const std::vector<std::string>& GetWarnings() const;
    
    // 文件信息
    void SetCurrentFile(const std::string& filePath);
    std::string GetCurrentFile() const;
    
    // 输出管理
    void AppendOutput(const std::string& code);
    std::string GetOutput() const;
    void ClearOutput();
    
    // 调试信息
    void DumpContext() const;
    
private:
    class Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace JSCompiler  
} // namespace CHTL