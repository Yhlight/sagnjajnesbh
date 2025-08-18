#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <functional>
#include <memory>

namespace chtl {
namespace cjmod {
namespace iNeverAway {

// iNeverAway 虚拟对象和标记函数系统
template<typename StateType>
class VirtualObject {
public:
    using FunctionType = std::function<void()>;
    using StateKey = std::pair<std::string, StateType>;
    
    void defineFunction(const std::string& name, const StateType& state, FunctionType func) {
        functions_[{name, state}] = func;
    }
    
    void defineFunction(const std::string& name, FunctionType func) {
        // 无状态版本，使用默认状态
        functions_[{name, StateType{}}] = func;
    }
    
    bool callFunction(const std::string& name, const StateType& state = StateType{}) {
        auto it = functions_.find({name, state});
        if (it != functions_.end()) {
            it->second();
            return true;
        }
        return false;
    }
    
    std::vector<std::string> getAvailableFunctions() const {
        std::vector<std::string> names;
        for (const auto& pair : functions_) {
            names.push_back(pair.first.first);
        }
        return names;
    }

private:
    std::unordered_map<StateKey, FunctionType, 
                      decltype([](const StateKey& k) {
                          return std::hash<std::string>{}(k.first) ^ 
                                 std::hash<StateType>{}(k.second);
                      })> functions_;
};

// iNeverAway 工厂函数
template<typename StateType = std::string>
class INeverAway {
public:
    static std::shared_ptr<VirtualObject<StateType>> create() {
        return std::make_shared<VirtualObject<StateType>>();
    }
    
    // 全局虚拟对象管理
    static void registerVirtualObject(const std::string& name, 
                                    std::shared_ptr<VirtualObject<StateType>> obj) {
        globalObjects_[name] = obj;
    }
    
    static std::shared_ptr<VirtualObject<StateType>> getVirtualObject(const std::string& name) {
        auto it = globalObjects_.find(name);
        return (it != globalObjects_.end()) ? it->second : nullptr;
    }

private:
    static inline std::unordered_map<std::string, 
                                   std::shared_ptr<VirtualObject<StateType>>> globalObjects_;
};

// CHTL JS 接口函数
extern "C" {
    void* chtl_iNeverAway_create(const char* name);
    void chtl_iNeverAway_define(void* obj, const char* funcName, 
                               const char* state, void* callback);
    int chtl_iNeverAway_call(void* obj, const char* funcName, const char* state);
    void chtl_iNeverAway_destroy(void* obj);
}

} // namespace iNeverAway
} // namespace cjmod
} // namespace chtl