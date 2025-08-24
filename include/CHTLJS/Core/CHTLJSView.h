#pragma once

#include "CHTLJS/Core/CHTLJSToken.h"
#include <string>
#include <unordered_map>
#include <vector>
#include <memory>

namespace CHTL {
namespace CHTLJS {
namespace Core {

/**
 * @brief 虚对象View类 - 负责虚对象的解析和管理
 * 
 * 根据语法文档第1307-1310行的要求：
 * 1. 记录vir虚对象的名称
 * 2. 解析listen CHTL JS函数中的函数键，并创建对应表
 * 3. 管理虚对象方法调用的全局函数注册表
 */
class CHTLJSView {
public:
    /**
     * @brief 构造函数
     */
    CHTLJSView();
    
    /**
     * @brief 析构函数
     */
    ~CHTLJSView() = default;
    
    /**
     * @brief 记录虚对象名称
     * @param objectName 虚对象名称
     */
    void RecordVirtualObjectName(const std::string& objectName);
    
    /**
     * @brief 解析CHTL JS函数中的函数键并创建对应表
     * @param objectName 虚对象名称
     * @param chtlJSFunction CHTL JS函数（listen、delegate、animate）
     * @param functionKeys 函数键列表
     */
    void ParseCHTLJSFunctionKeys(const std::string& objectName, 
                                const std::string& chtlJSFunction,
                                const std::vector<std::string>& functionKeys);
    
    /**
     * @brief 处理虚对象方法调用 objectName->methodName
     * @param objectName 虚对象名称
     * @param methodName 方法名称
     * @return 生成的全局函数名称
     */
    std::string ProcessVirtualMethodCall(const std::string& objectName, 
                                       const std::string& methodName);
    
    /**
     * @brief 检查虚对象是否存在
     * @param objectName 虚对象名称
     * @return 是否存在
     */
    bool HasVirtualObject(const std::string& objectName) const;
    
    /**
     * @brief 检查虚对象是否有指定的函数键
     * @param objectName 虚对象名称
     * @param functionKey 函数键名称
     * @return 是否存在该函数键
     */
    bool HasFunctionKey(const std::string& objectName, const std::string& functionKey) const;
    
    /**
     * @brief 获取虚对象的所有函数键
     * @param objectName 虚对象名称
     * @return 函数键列表
     */
    std::vector<std::string> GetFunctionKeys(const std::string& objectName) const;
    
    /**
     * @brief 获取全局函数注册表
     * @return 全局函数映射表
     */
    const std::unordered_map<std::string, std::string>& GetGlobalFunctionRegistry() const;
    
    /**
     * @brief 清空所有虚对象数据
     */
    void Clear();
    
    /**
     * @brief 获取虚对象统计信息
     * @return 统计信息字符串
     */
    std::string GetStatistics() const;

private:
    // 虚对象名称集合
    std::vector<std::string> virtualObjectNames_;
    
    // 虚对象 -> 函数键查询表
    // objectName -> vector<functionKeys>
    std::unordered_map<std::string, std::vector<std::string>> virtualObjectFunctionTable_;
    
    // 虚对象 -> CHTL JS函数类型映射
    // objectName -> "listen"|"delegate"|"animate"
    std::unordered_map<std::string, std::string> virtualObjectCHTLJSFunction_;
    
    // 全局函数注册表（防止重复创建）
    // "objectName->methodName" -> "generated_global_function_name"
    std::unordered_map<std::string, std::string> globalFunctionRegistry_;
    
    // 全局函数计数器（用于生成唯一函数名）
    static int globalFunctionCounter_;
    
    /**
     * @brief 生成唯一的全局函数名
     * @param objectName 虚对象名称
     * @param methodName 方法名称
     * @return 全局函数名
     */
    std::string GenerateGlobalFunctionName(const std::string& objectName, 
                                         const std::string& methodName);
    
    /**
     * @brief 验证CHTL JS函数类型
     * @param functionType 函数类型
     * @return 是否为有效的CHTL JS函数
     */
    bool IsValidCHTLJSFunction(const std::string& functionType) const;
};

} // namespace Core
} // namespace CHTLJS
} // namespace CHTL