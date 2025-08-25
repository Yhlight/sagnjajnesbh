#include "CHTL/Compiler/GlobalMap.h"
#include <iostream>
#include <algorithm>

namespace CHTL {
namespace Compiler {

// 自定义hash函数
struct KeyHash {
    std::size_t operator()(const std::pair<GlobalObjectType, std::string>& key) const {
        // 组合两个hash值
        auto h1 = std::hash<int>{}(static_cast<int>(key.first));
        auto h2 = std::hash<std::string>{}(key.second);
        return h1 ^ (h2 << 1); // 简单的hash组合
    }
};

class GlobalMap::Impl {
public:
    // 使用复合键存储：类型 + 完整限定名
    using Key = std::pair<GlobalObjectType, std::string>;
    std::unordered_map<Key, std::shared_ptr<GlobalObject>, KeyHash> objects;
    
    // 命名空间索引：命名空间路径 -> 对象列表
    std::unordered_map<std::string, std::vector<std::shared_ptr<GlobalObject>>> namespaceIndex;
    
    bool Register(const GlobalObject& obj) {
        Key key{obj.type, obj.GetQualifiedName()};
        
        // 检查是否已存在
        if (objects.find(key) != objects.end()) {
            std::cerr << "错误：全局对象已存在：" << obj.GetQualifiedName() 
                      << " (类型: " << static_cast<int>(obj.type) << ")" << std::endl;
            return false;
        }
        
        auto sharedObj = std::make_shared<GlobalObject>(obj);
        objects[key] = sharedObj;
        
        // 更新命名空间索引
        namespaceIndex[obj.namespacePath].push_back(sharedObj);
        
        return true;
    }
    
    std::shared_ptr<GlobalObject> Find(const std::string& name, 
                                       GlobalObjectType type,
                                       const std::string& currentNamespace) const {
        // 首先尝试在当前命名空间查找
        if (!currentNamespace.empty()) {
            Key key{type, currentNamespace + "." + name};
            auto it = objects.find(key);
            if (it != objects.end()) {
                return it->second;
            }
        }
        
        // 尝试查找完整限定名
        Key key{type, name};
        auto it = objects.find(key);
        if (it != objects.end()) {
            return it->second;
        }
        
        // 尝试在全局命名空间查找
        if (name.find('.') == std::string::npos) {
            Key globalKey{type, name};
            it = objects.find(globalKey);
            if (it != objects.end()) {
                return it->second;
            }
        }
        
        return nullptr;
    }
    
    std::vector<std::shared_ptr<GlobalObject>> FindAll(GlobalObjectType type) const {
        std::vector<std::shared_ptr<GlobalObject>> result;
        
        for (const auto& [key, obj] : objects) {
            if (key.first == type) {
                result.push_back(obj);
            }
        }
        
        return result;
    }
    
    bool Exists(const std::string& name, GlobalObjectType type,
                const std::string& currentNamespace) const {
        return Find(name, type, currentNamespace) != nullptr;
    }
    
    std::vector<std::shared_ptr<GlobalObject>> GetNamespaceObjects(
        const std::string& namespacePath) const {
        auto it = namespaceIndex.find(namespacePath);
        if (it != namespaceIndex.end()) {
            return it->second;
        }
        return {};
    }
    
    void Clear() {
        objects.clear();
        namespaceIndex.clear();
    }
    
    void Merge(const GlobalMap::Impl& other, const std::string& namespacePrefix) {
        for (const auto& [key, obj] : other.objects) {
            GlobalObject newObj = *obj;
            
            // 添加命名空间前缀
            if (!namespacePrefix.empty()) {
                if (newObj.namespacePath.empty()) {
                    newObj.namespacePath = namespacePrefix;
                } else {
                    newObj.namespacePath = namespacePrefix + "." + newObj.namespacePath;
                }
            }
            
            Register(newObj);
        }
    }
    
    void Dump() const {
        std::cout << "=== CHTL GlobalMap ===" << std::endl;
        
        // 按类型分组输出
        std::unordered_map<GlobalObjectType, std::vector<std::shared_ptr<GlobalObject>>> byType;
        for (const auto& [key, obj] : objects) {
            byType[key.first].push_back(obj);
        }
        
        const char* typeNames[] = {
            "Template Style", "Template Element", "Template Var",
            "Custom Style", "Custom Element", "Custom Var",
            "Origin Embed", "Configuration", "Namespace"
        };
        
        for (int i = 0; i < 9; ++i) {
            auto type = static_cast<GlobalObjectType>(i);
            auto it = byType.find(type);
            if (it != byType.end() && !it->second.empty()) {
                std::cout << "\n" << typeNames[i] << ":" << std::endl;
                for (const auto& obj : it->second) {
                    std::cout << "  - " << obj->GetQualifiedName() 
                              << " (" << obj->sourceFile << ":" 
                              << obj->line << ":" << obj->column << ")" << std::endl;
                }
            }
        }
        
        std::cout << "\n总计: " << objects.size() << " 个对象" << std::endl;
        std::cout << "===================" << std::endl;
    }
};

// GlobalMap 实现
GlobalMap::GlobalMap() : pImpl(std::make_unique<Impl>()) {}
GlobalMap::~GlobalMap() = default;

bool GlobalMap::Register(const GlobalObject& obj) {
    return pImpl->Register(obj);
}

std::shared_ptr<GlobalObject> GlobalMap::Find(const std::string& name, 
                                             GlobalObjectType type,
                                             const std::string& currentNamespace) const {
    return pImpl->Find(name, type, currentNamespace);
}

std::vector<std::shared_ptr<GlobalObject>> GlobalMap::FindAll(GlobalObjectType type) const {
    return pImpl->FindAll(type);
}

bool GlobalMap::Exists(const std::string& name, GlobalObjectType type,
                       const std::string& currentNamespace) const {
    return pImpl->Exists(name, type, currentNamespace);
}

std::vector<std::shared_ptr<GlobalObject>> GlobalMap::GetNamespaceObjects(
    const std::string& namespacePath) const {
    return pImpl->GetNamespaceObjects(namespacePath);
}

void GlobalMap::Clear() {
    pImpl->Clear();
}

void GlobalMap::Merge(const GlobalMap& other, const std::string& namespacePrefix) {
    pImpl->Merge(*other.pImpl, namespacePrefix);
}

void GlobalMap::Dump() const {
    pImpl->Dump();
}

} // namespace Compiler
} // namespace CHTL