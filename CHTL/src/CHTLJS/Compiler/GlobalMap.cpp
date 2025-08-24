#include "CHTLJS/Compiler/GlobalMap.h"
#include <iostream>
#include <sstream>
#include <algorithm>

namespace CHTL {
namespace JSCompiler {

class GlobalMap::Impl {
public:
    // 虚对象注册表
    std::unordered_map<std::string, std::shared_ptr<GlobalObject>> virObjects;
    
    // 函数映射表：virName.methodName -> globalFuncName
    std::unordered_map<std::string, std::string> functionMappings;
    
    // 事件委托注册表
    std::vector<std::tuple<std::string, std::string, std::string>> delegates;
    
    // 全局函数计数器
    size_t globalFuncCounter = 0;
    
    bool RegisterVirObject(const std::string& name, const GlobalObject& obj) {
        if (virObjects.find(name) != virObjects.end()) {
            std::cerr << "错误：虚对象已存在：" << name << std::endl;
            return false;
        }
        
        virObjects[name] = std::make_shared<GlobalObject>(obj);
        return true;
    }
    
    bool RegisterFunctionMapping(const std::string& virName, 
                                const std::string& methodName,
                                const std::string& globalFuncName) {
        std::string key = virName + "." + methodName;
        
        if (functionMappings.find(key) != functionMappings.end()) {
            std::cerr << "警告：函数映射已存在：" << key << std::endl;
            return false;
        }
        
        functionMappings[key] = globalFuncName;
        return true;
    }
    
    bool RegisterDelegate(const std::string& parentSelector,
                         const std::string& targetSelector,
                         const std::string& event) {
        // 检查是否已存在相同的委托
        for (const auto& [p, t, e] : delegates) {
            if (p == parentSelector && t == targetSelector && e == event) {
                return false; // 已存在
            }
        }
        
        delegates.emplace_back(parentSelector, targetSelector, event);
        return true;
    }
    
    std::shared_ptr<GlobalObject> FindVirObject(const std::string& name) const {
        auto it = virObjects.find(name);
        if (it != virObjects.end()) {
            return it->second;
        }
        return nullptr;
    }
    
    std::string FindFunctionMapping(const std::string& virName,
                                   const std::string& methodName) const {
        std::string key = virName + "." + methodName;
        auto it = functionMappings.find(key);
        if (it != functionMappings.end()) {
            return it->second;
        }
        return "";
    }
    
    std::vector<std::tuple<std::string, std::string, std::string>> GetAllDelegates() const {
        return delegates;
    }
    
    std::string GenerateGlobalFunctionName(const std::string& prefix) {
        std::stringstream ss;
        ss << prefix << "_" << ++globalFuncCounter;
        return ss.str();
    }
    
    void Clear() {
        virObjects.clear();
        functionMappings.clear();
        delegates.clear();
        globalFuncCounter = 0;
    }
    
    void Dump() const {
        std::cout << "=== CHTL JS GlobalMap ===" << std::endl;
        
        if (!virObjects.empty()) {
            std::cout << "\n虚对象:" << std::endl;
            for (const auto& [name, obj] : virObjects) {
                std::cout << "  - " << name << " (" << obj->sourceFile 
                          << ":" << obj->line << ":" << obj->column << ")" << std::endl;
            }
        }
        
        if (!functionMappings.empty()) {
            std::cout << "\n函数映射:" << std::endl;
            for (const auto& [key, funcName] : functionMappings) {
                std::cout << "  - " << key << " -> " << funcName << std::endl;
            }
        }
        
        if (!delegates.empty()) {
            std::cout << "\n事件委托:" << std::endl;
            for (const auto& [parent, target, event] : delegates) {
                std::cout << "  - " << parent << " 代理 " << target 
                          << " 的 " << event << " 事件" << std::endl;
            }
        }
        
        std::cout << "\n总计: " << virObjects.size() << " 个虚对象, "
                  << functionMappings.size() << " 个函数映射, "
                  << delegates.size() << " 个事件委托" << std::endl;
        std::cout << "===================" << std::endl;
    }
};

// GlobalMap 实现
GlobalMap::GlobalMap() : pImpl(std::make_unique<Impl>()) {}
GlobalMap::~GlobalMap() = default;

bool GlobalMap::RegisterVirObject(const std::string& name, const GlobalObject& obj) {
    return pImpl->RegisterVirObject(name, obj);
}

bool GlobalMap::RegisterFunctionMapping(const std::string& virName, 
                                       const std::string& methodName,
                                       const std::string& globalFuncName) {
    return pImpl->RegisterFunctionMapping(virName, methodName, globalFuncName);
}

bool GlobalMap::RegisterDelegate(const std::string& parentSelector,
                                const std::string& targetSelector,
                                const std::string& event) {
    return pImpl->RegisterDelegate(parentSelector, targetSelector, event);
}

std::shared_ptr<GlobalObject> GlobalMap::FindVirObject(const std::string& name) const {
    return pImpl->FindVirObject(name);
}

std::string GlobalMap::FindFunctionMapping(const std::string& virName,
                                          const std::string& methodName) const {
    return pImpl->FindFunctionMapping(virName, methodName);
}

std::vector<std::tuple<std::string, std::string, std::string>> GlobalMap::GetAllDelegates() const {
    return pImpl->GetAllDelegates();
}

std::string GlobalMap::GenerateGlobalFunctionName(const std::string& prefix) {
    return pImpl->GenerateGlobalFunctionName(prefix);
}

void GlobalMap::Clear() {
    pImpl->Clear();
}

void GlobalMap::Dump() const {
    pImpl->Dump();
}

} // namespace JSCompiler
} // namespace CHTL