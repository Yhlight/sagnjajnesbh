#pragma once

#include <string>
#include <unordered_map>
#include <vector>
#include <memory>
#include <optional>
#include <set>

namespace chtl {
namespace chtljs {

// 前向声明
class VirtualObject;
class EnhancedSelector;
class EventDelegation;
class Animation;
class CHTLJSFunction;

/**
 * CHTL JS 全局映射表
 * 管理CHTL JS特有的全局对象和函数
 * 注意：这是CHTL JS专用的，完全独立于CHTL的GlobalMap
 */
class CHTLJSGlobalMap {
public:
    CHTLJSGlobalMap();
    ~CHTLJSGlobalMap();
    
    // 虚对象管理
    void RegisterVirtualObject(const std::string& name,
                              std::shared_ptr<VirtualObject> vir);
    std::shared_ptr<VirtualObject> GetVirtualObject(const std::string& name) const;
    bool IsVirtualObject(const std::string& name) const;
    
    // 增强选择器管理
    void RegisterSelector(const std::string& selector,
                         std::shared_ptr<EnhancedSelector> enhancedSelector);
    std::shared_ptr<EnhancedSelector> GetSelector(const std::string& selector) const;
    std::vector<std::string> GetAllSelectors() const;
    
    // 事件委托管理
    void RegisterEventDelegation(const std::string& parentSelector,
                                std::shared_ptr<EventDelegation> delegation);
    std::shared_ptr<EventDelegation> GetEventDelegation(const std::string& parentSelector) const;
    bool HasEventDelegation(const std::string& parentSelector) const;
    
    // 动画管理
    void RegisterAnimation(const std::string& animationId,
                          std::shared_ptr<Animation> animation);
    std::shared_ptr<Animation> GetAnimation(const std::string& animationId) const;
    
    // CHTL JS函数管理
    void RegisterCHTLJSFunction(const std::string& name,
                               std::shared_ptr<CHTLJSFunction> function);
    std::shared_ptr<CHTLJSFunction> GetCHTLJSFunction(const std::string& name) const;
    bool IsCHTLJSFunction(const std::string& name) const;
    
    // 函数调用链管理
    void StartCallChain(const std::string& object);
    void AddToCallChain(const std::string& method);
    std::vector<std::string> GetCurrentCallChain() const;
    void EndCallChain();
    
    // 状态标记管理（用于iNeverAway）
    void RegisterStateFunction(const std::string& functionName,
                              const std::string& state,
                              const std::string& uniqueName);
    std::string ResolveStateFunction(const std::string& functionName,
                                    const std::string& state) const;
    
    // 全局函数名生成（确保唯一性）
    std::string GenerateUniqueFunctionName(const std::string& prefix);
    
    // 清理
    void Clear();
    
    // 调试信息
    void DumpVirtualObjects() const;
    void DumpSelectors() const;
    void DumpFunctions() const;
    
private:
    // 虚对象存储
    std::unordered_map<std::string, std::shared_ptr<VirtualObject>> m_VirtualObjects;
    
    // 增强选择器存储
    std::unordered_map<std::string, std::shared_ptr<EnhancedSelector>> m_Selectors;
    
    // 事件委托存储（父选择器 -> 委托信息）
    std::unordered_map<std::string, std::shared_ptr<EventDelegation>> m_EventDelegations;
    
    // 动画存储
    std::unordered_map<std::string, std::shared_ptr<Animation>> m_Animations;
    
    // CHTL JS函数存储
    std::unordered_map<std::string, std::shared_ptr<CHTLJSFunction>> m_CHTLJSFunctions;
    
    // 函数调用链
    std::vector<std::string> m_CurrentCallChain;
    
    // 状态函数映射（functionName:state -> uniqueName）
    std::unordered_map<std::string, std::string> m_StateFunctions;
    
    // 函数名计数器（用于生成唯一名称）
    std::unordered_map<std::string, size_t> m_FunctionNameCounters;
    
    // 初始化内置函数
    void InitializeBuiltinFunctions();
    
public:
    // 内置函数管理
    void AddBuiltinFunction(const std::string& name);
    bool IsBuiltinFunction(const std::string& name) const;
    
    // 增强选择器管理
    void AddEnhancedSelector(const std::string& selector);
    const std::set<std::string>& GetEnhancedSelectors() const;
    
private:
    std::set<std::string> m_BuiltinFunctions;
    std::set<std::string> m_EnhancedSelectors;
};

} // namespace chtljs
} // namespace chtl