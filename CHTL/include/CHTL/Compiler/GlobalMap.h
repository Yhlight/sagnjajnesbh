#pragma once
#include <string>
#include <unordered_map>
#include <memory>
#include <vector>
#include "CHTL/AST/ASTNode.h"

namespace CHTL {
namespace Compiler {

// 全局对象类型
enum class GlobalObjectType {
    TEMPLATE_STYLE,
    TEMPLATE_ELEMENT,
    TEMPLATE_VAR,
    CUSTOM_STYLE,
    CUSTOM_ELEMENT,
    CUSTOM_VAR,
    ORIGIN_EMBED,
    CONFIGURATION,
    NAMESPACE
};

// 全局对象信息
struct GlobalObject {
    GlobalObjectType type;
    std::string name;
    std::string namespacePath;  // 命名空间路径，如 "space.room"
    std::shared_ptr<ASTNode> astNode;
    std::string sourceFile;
    size_t line;
    size_t column;
    
    // 获取完整限定名
    std::string GetQualifiedName() const {
        if (namespacePath.empty()) {
            return name;
        }
        return namespacePath + "." + name;
    }
};

// CHTL全局映射表
class GlobalMap {
public:
    GlobalMap();
    ~GlobalMap();
    
    // 注册全局对象
    bool Register(const GlobalObject& obj);
    
    // 查找对象
    std::shared_ptr<GlobalObject> Find(const std::string& name, 
                                       GlobalObjectType type,
                                       const std::string& currentNamespace = "") const;
    
    // 查找所有匹配类型的对象
    std::vector<std::shared_ptr<GlobalObject>> FindAll(GlobalObjectType type) const;
    
    // 检查是否存在
    bool Exists(const std::string& name, GlobalObjectType type,
                const std::string& currentNamespace = "") const;
    
    // 获取命名空间下的所有对象
    std::vector<std::shared_ptr<GlobalObject>> GetNamespaceObjects(
        const std::string& namespacePath) const;
    
    // 清空映射表
    void Clear();
    
    // 合并另一个GlobalMap（用于导入）
    void Merge(const GlobalMap& other, const std::string& namespacePrefix = "");
    
    // 调试输出
    void Dump() const;
    
private:
    class Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace Compiler
} // namespace CHTL