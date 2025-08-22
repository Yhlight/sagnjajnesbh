#pragma once

#include "CJMOD/CJMODApi.h"
#include <memory>
#include <unordered_map>

namespace CHTL {
namespace CJMOD {
namespace Extensions {

/**
 * @brief iNeverAway CJMOD扩展
 * 
 * 语法文档第1485-1520行：iNeverAway用于创建一组标记函数
 * iNeverAway允许开发者定义键，而不是使用键，并可以使用状态区分同名的键
 * iNeverAway需要与虚对象共用
 * 
 * 实现原理：
 * iNeverAway -> 创建一组JS全局函数，名称由CHTL编译器统一管理，在调用时才生成对应的JS函数代码
 * vir对象本身不存在，最终转变成相对应的函数的引用
 */
class INeverAwayExtension : public CJMODExtension {
public:
    /**
     * @brief 构造函数
     */
    INeverAwayExtension();
    
    /**
     * @brief 初始化扩展
     */
    bool Initialize(CJMODScanner& scanner) override;
    
    /**
     * @brief 获取扩展名称
     */
    std::string GetName() const override { return "INeverAway"; }
    
    /**
     * @brief 获取扩展版本
     */
    std::string GetVersion() const override { return "1.0.0"; }
    
    /**
     * @brief 获取支持的语法列表
     */
    std::vector<std::string> GetSupportedSyntax() const override;

private:
    /**
     * @brief 函数状态类型
     */
    enum class FunctionState {
        VOID_A,     // Void<A>状态
        VOID_B,     // Void<B>状态
        VOID,       // Void状态
        NORMAL      // 普通状态
    };
    
    /**
     * @brief 函数定义信息
     */
    struct FunctionDef {
        std::string name;               // 函数名
        FunctionState state;            // 函数状态
        std::vector<std::string> params; // 参数列表
        std::string body;               // 函数体
        std::string globalName;         // 全局函数名
    };
    
    /**
     * @brief 虚对象定义信息
     */
    struct VirtualObjectDef {
        std::string name;                               // 虚对象名
        std::unordered_map<std::string, FunctionDef> functions; // 函数映射
        int globalFunctionCounter;                      // 全局函数计数器
    };

private:
    /**
     * @brief 处理iNeverAway语法
     */
    void HandleINeverAway();
    
    /**
     * @brief 处理虚对象调用语法
     */
    void HandleVirtualObjectCall();
    
    /**
     * @brief 解析函数状态
     * @param stateStr 状态字符串（如"Void<A>"）
     * @return 函数状态枚举
     */
    FunctionState ParseFunctionState(const std::string& stateStr);
    
    /**
     * @brief 生成全局函数名
     * @param objectName 虚对象名
     * @param functionName 函数名
     * @param state 函数状态
     * @return 生成的全局函数名
     */
    std::string GenerateGlobalFunctionName(const std::string& objectName, 
                                          const std::string& functionName, 
                                          FunctionState state);
    
    /**
     * @brief 生成iNeverAway的JavaScript实现
     * @param objectName 虚对象名
     * @param functions 函数定义列表
     * @return 生成的JavaScript代码
     */
    std::string GenerateINeverAwayCode(const std::string& objectName, 
                                      const std::vector<FunctionDef>& functions);
    
    /**
     * @brief 生成虚对象调用的JavaScript代码
     * @param objectName 虚对象名
     * @param functionName 函数名
     * @param state 函数状态
     * @param args 调用参数
     * @return 生成的JavaScript代码
     */
    std::string GenerateVirtualCallCode(const std::string& objectName,
                                       const std::string& functionName,
                                       FunctionState state,
                                       const std::vector<std::string>& args);
    
    /**
     * @brief 解析函数定义块
     * @param functionsBlock 函数定义块字符串
     * @param virtualObj 虚对象定义引用
     */
    void ParseFunctionDefinitions(const std::string& functionsBlock, VirtualObjectDef& virtualObj);

private:
    std::unique_ptr<Syntax> iNeverAwaySyntax_;          // iNeverAway语法
    std::unique_ptr<Syntax> virtualCallSyntax_;         // 虚对象调用语法
    std::unordered_map<std::string, VirtualObjectDef> virtualObjects_; // 虚对象映射
    CJMODScanner* scanner_;                             // 扫描器指针
    int globalCounter_;                                 // 全局计数器
};

} // namespace Extensions
} // namespace CJMOD
} // namespace CHTL