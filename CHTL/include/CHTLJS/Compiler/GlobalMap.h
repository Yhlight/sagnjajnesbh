#pragma once
#include <string>
#include <unordered_map>
#include <memory>
#include <vector>
#include "CHTLJS/AST/ASTNode.h"

namespace CHTL {
namespace JSCompiler {

// CHTL JS全局对象类型
enum class GlobalObjectType {
    VIR_OBJECT,           // 虚对象
    FUNCTION_MAPPING,     // 函数映射（用于vir对象）
    DELEGATE_REGISTRY,    // 事件委托注册表
    ANIMATION_OBJECT,     // 动画对象
    GLOBAL_FUNCTION       // 全局函数（由vir生成）
};

// 全局对象信息
struct GlobalObject {
    GlobalObjectType type;
    std::string name;
    std::string mappedName;       // 映射后的名称（如全局函数名）
    std::shared_ptr<ASTNode> astNode;
    std::string sourceFile;
    size_t line;
    size_t column;
    
    // 额外信息
    std::unordered_map<std::string, std::string> metadata;
    
    std::string GetQualifiedName() const {
        return name;
    }
};

// CHTL JS全局映射表
class GlobalMap {
public:
    GlobalMap();
    ~GlobalMap();
    
    // 注册虚对象
    bool RegisterVirObject(const std::string& name, const GlobalObject& obj);
    
    // 注册函数映射（vir对象的方法 -> 全局函数）
    bool RegisterFunctionMapping(const std::string& virName, 
                                const std::string& methodName,
                                const std::string& globalFuncName);
    
    // 注册事件委托
    bool RegisterDelegate(const std::string& parentSelector,
                         const std::string& targetSelector,
                         const std::string& event);
    
    // 查找虚对象
    std::shared_ptr<GlobalObject> FindVirObject(const std::string& name) const;
    
    // 查找函数映射
    std::string FindFunctionMapping(const std::string& virName,
                                   const std::string& methodName) const;
    
    // 获取所有委托
    std::vector<std::tuple<std::string, std::string, std::string>> GetAllDelegates() const;
    
    // 生成唯一的全局函数名
    std::string GenerateGlobalFunctionName(const std::string& prefix = "chtljs_func");
    
    // 清空映射表
    void Clear();
    
    // 调试输出
    void Dump() const;
    
private:
    class Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace JSCompiler
} // namespace CHTL