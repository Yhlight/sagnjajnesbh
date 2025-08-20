#include "chtl_js/core/CHTLJSGlobalMap.h"
#include "utils/Logger.h"
#include <iostream>
#include <sstream>

namespace chtl {
namespace chtljs {

CHTLJSGlobalMap::CHTLJSGlobalMap() {
    InitializeBuiltinFunctions();
}

CHTLJSGlobalMap::~CHTLJSGlobalMap() = default;

void CHTLJSGlobalMap::RegisterVirtualObject(const std::string& name,
                                           std::shared_ptr<VirtualObject> vir) {
    m_VirtualObjects[name] = vir;
    utils::Logger::GetInstance().Debug("注册虚对象: " + name);
}

std::shared_ptr<VirtualObject> CHTLJSGlobalMap::GetVirtualObject(const std::string& name) const {
    auto it = m_VirtualObjects.find(name);
    if (it != m_VirtualObjects.end()) {
        return it->second;
    }
    return nullptr;
}

bool CHTLJSGlobalMap::IsVirtualObject(const std::string& name) const {
    return m_VirtualObjects.find(name) != m_VirtualObjects.end();
}

void CHTLJSGlobalMap::RegisterSelector(const std::string& selector,
                                      std::shared_ptr<EnhancedSelector> enhancedSelector) {
    m_Selectors[selector] = enhancedSelector;
    utils::Logger::GetInstance().Debug("注册增强选择器: " + selector);
}

std::shared_ptr<EnhancedSelector> CHTLJSGlobalMap::GetSelector(const std::string& selector) const {
    auto it = m_Selectors.find(selector);
    if (it != m_Selectors.end()) {
        return it->second;
    }
    return nullptr;
}

std::vector<std::string> CHTLJSGlobalMap::GetAllSelectors() const {
    std::vector<std::string> selectors;
    for (const auto& pair : m_Selectors) {
        selectors.push_back(pair.first);
    }
    return selectors;
}

void CHTLJSGlobalMap::RegisterEventDelegation(const std::string& parentSelector,
                                             std::shared_ptr<EventDelegation> delegation) {
    m_EventDelegations[parentSelector] = delegation;
    utils::Logger::GetInstance().Debug("注册事件委托: " + parentSelector);
}

std::shared_ptr<EventDelegation> CHTLJSGlobalMap::GetEventDelegation(
    const std::string& parentSelector) const {
    auto it = m_EventDelegations.find(parentSelector);
    if (it != m_EventDelegations.end()) {
        return it->second;
    }
    return nullptr;
}

bool CHTLJSGlobalMap::HasEventDelegation(const std::string& parentSelector) const {
    return m_EventDelegations.find(parentSelector) != m_EventDelegations.end();
}

void CHTLJSGlobalMap::RegisterAnimation(const std::string& animationId,
                                       std::shared_ptr<Animation> animation) {
    m_Animations[animationId] = animation;
    utils::Logger::GetInstance().Debug("注册动画: " + animationId);
}

std::shared_ptr<Animation> CHTLJSGlobalMap::GetAnimation(const std::string& animationId) const {
    auto it = m_Animations.find(animationId);
    if (it != m_Animations.end()) {
        return it->second;
    }
    return nullptr;
}

void CHTLJSGlobalMap::RegisterCHTLJSFunction(const std::string& name,
                                            std::shared_ptr<CHTLJSFunction> function) {
    m_CHTLJSFunctions[name] = function;
    utils::Logger::GetInstance().Debug("注册CHTL JS函数: " + name);
}

std::shared_ptr<CHTLJSFunction> CHTLJSGlobalMap::GetCHTLJSFunction(
    const std::string& name) const {
    auto it = m_CHTLJSFunctions.find(name);
    if (it != m_CHTLJSFunctions.end()) {
        return it->second;
    }
    return nullptr;
}

bool CHTLJSGlobalMap::IsCHTLJSFunction(const std::string& name) const {
    return m_CHTLJSFunctions.find(name) != m_CHTLJSFunctions.end();
}

void CHTLJSGlobalMap::StartCallChain(const std::string& object) {
    m_CurrentCallChain.clear();
    m_CurrentCallChain.push_back(object);
    utils::Logger::GetInstance().Debug("开始调用链: " + object);
}

void CHTLJSGlobalMap::AddToCallChain(const std::string& method) {
    m_CurrentCallChain.push_back(method);
    utils::Logger::GetInstance().Debug("添加到调用链: " + method);
}

std::vector<std::string> CHTLJSGlobalMap::GetCurrentCallChain() const {
    return m_CurrentCallChain;
}

void CHTLJSGlobalMap::EndCallChain() {
    utils::Logger::GetInstance().Debug("结束调用链");
    m_CurrentCallChain.clear();
}

void CHTLJSGlobalMap::RegisterStateFunction(const std::string& functionName,
                                           const std::string& state,
                                           const std::string& uniqueName) {
    std::string key = functionName + ":" + state;
    m_StateFunctions[key] = uniqueName;
    utils::Logger::GetInstance().Debug("注册状态函数: " + key + " -> " + uniqueName);
}

std::string CHTLJSGlobalMap::ResolveStateFunction(const std::string& functionName,
                                                 const std::string& state) const {
    std::string key = functionName + ":" + state;
    auto it = m_StateFunctions.find(key);
    if (it != m_StateFunctions.end()) {
        return it->second;
    }
    return "";
}

std::string CHTLJSGlobalMap::GenerateUniqueFunctionName(const std::string& prefix) {
    size_t& counter = m_FunctionNameCounters[prefix];
    counter++;
    
    std::stringstream ss;
    ss << "__chtljs_" << prefix << "_" << counter;
    return ss.str();
}

void CHTLJSGlobalMap::Clear() {
    m_VirtualObjects.clear();
    m_Selectors.clear();
    m_EventDelegations.clear();
    m_Animations.clear();
    m_CHTLJSFunctions.clear();
    m_CurrentCallChain.clear();
    m_StateFunctions.clear();
    m_FunctionNameCounters.clear();
    
    // 重新初始化内置函数
    InitializeBuiltinFunctions();
}

void CHTLJSGlobalMap::DumpVirtualObjects() const {
    std::cout << "=== 虚对象 ===" << std::endl;
    for (const auto& pair : m_VirtualObjects) {
        std::cout << "  " << pair.first << std::endl;
    }
}

void CHTLJSGlobalMap::DumpSelectors() const {
    std::cout << "=== 增强选择器 ===" << std::endl;
    for (const auto& pair : m_Selectors) {
        std::cout << "  " << pair.first << std::endl;
    }
}

void CHTLJSGlobalMap::DumpFunctions() const {
    std::cout << "=== CHTL JS函数 ===" << std::endl;
    for (const auto& pair : m_CHTLJSFunctions) {
        std::cout << "  " << pair.first << std::endl;
    }
    std::cout << "=== 状态函数 ===" << std::endl;
    for (const auto& pair : m_StateFunctions) {
        std::cout << "  " << pair.first << " -> " << pair.second << std::endl;
    }
}

void CHTLJSGlobalMap::InitializeBuiltinFunctions() {
    // 注册内置的CHTL JS函数名
    // 这些函数的具体实现将在解析阶段处理
    std::vector<std::string> builtinFunctions = {
        "listen",
        "delegate", 
        "animate",
        "iNeverAway",
        "printMylove"
    };
    
    for (const auto& func : builtinFunctions) {
        // 创建占位符，实际的函数对象将在使用时创建
        RegisterCHTLJSFunction(func, nullptr);
    }
}

void CHTLJSGlobalMap::AddBuiltinFunction(const std::string& name) {
    m_BuiltinFunctions.insert(name);
    utils::Logger::GetInstance().Debug("添加内置函数: " + name);
}

bool CHTLJSGlobalMap::IsBuiltinFunction(const std::string& name) const {
    return m_BuiltinFunctions.find(name) != m_BuiltinFunctions.end();
}

void CHTLJSGlobalMap::AddEnhancedSelector(const std::string& selector) {
    m_EnhancedSelectors.insert(selector);
    utils::Logger::GetInstance().Debug("添加增强选择器: " + selector);
}

const std::set<std::string>& CHTLJSGlobalMap::GetEnhancedSelectors() const {
    return m_EnhancedSelectors;
}

} // namespace chtljs
} // namespace chtl