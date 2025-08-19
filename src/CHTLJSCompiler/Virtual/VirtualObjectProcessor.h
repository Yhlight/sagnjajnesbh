#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <memory>

namespace chtl {
namespace chtljs {

// 前向声明
class EnhancedSelectorProcessor;

/**
 * 虚对象处理器
 * 严格按照CHTL语法文档实现 vir 虚对象功能
 */
class VirtualObjectProcessor {
public:
    struct FunctionReference {
        std::string originalKey;        // 原始键名 (如 "click")
        std::string globalFunctionName; // 生成的全局函数名
        std::string functionCode;       // 函数代码
        bool isGenerated = false;       // 是否已生成
    };
    
    struct VirtualObject {
        std::string name;                                    // 虚对象名称
        std::string sourceFunction;                         // 源函数 (listen, iNeverAway等)
        std::unordered_map<std::string, FunctionReference> functionMap; // 函数映射表
        std::vector<std::string> generatedFunctions;        // 生成的全局函数列表
    };
    
    /**
     * 状态重载支持 (用于iNeverAway)
     * 按语法文档：Void<A>, Void<B> 通过状态区分同名键
     */
    struct StateOverload {
        std::string baseName;           // 基础名称 (如 "Void")
        std::string state;              // 状态标识 (如 "A", "B")
        std::string fullName;           // 完整名称 (如 "Void<A>")
        std::string functionCode;       // 函数代码
    };
    
    VirtualObjectProcessor();
    ~VirtualObjectProcessor();
    
    // === 按语法文档：vir虚对象处理 ===
    
    /**
     * 处理vir语句
     * 按语法文档：vir test = listen({ click: () => {} }); 或 vir Test = iNeverAway({ ... });
     */
    std::string processVirtualObjectDefinition(const std::string& virCode);
    
    /**
     * 处理虚对象调用
     * 按语法文档：test->click(); 或 Test->Void<A>();
     */
    std::string processVirtualObjectCall(const std::string& callCode);
    
    /**
     * 解析vir定义
     * 提取虚对象名称和源函数
     */
    VirtualObject parseVirtualObjectDefinition(const std::string& virCode);
    
    /**
     * 生成虚对象的JavaScript实现
     * 按语法文档：创建特殊命名的全局函数，然后解析成函数引用
     */
    std::string generateVirtualObjectImplementation(const VirtualObject& virtualObj);
    
    // === listen虚对象处理 ===
    
    /**
     * 处理listen虚对象
     * 按语法文档：vir test = listen({ click: () => {} });
     */
    VirtualObject parseListenVirtualObject(const std::string& name, const std::string& listenObject);
    
    /**
     * 从listen对象中提取函数键
     * 按语法文档：解析listen函数中的函数键，并创建对应表
     */
    std::unordered_map<std::string, FunctionReference> extractListenFunctions(const std::string& listenObject);
    
    // === iNeverAway虚对象处理 ===
    
    /**
     * 处理iNeverAway虚对象
     * 按语法文档：vir Test = iNeverAway({ Void<A>: function() {}, Void<B>: function() {} });
     */
    VirtualObject parseINeverAwayVirtualObject(const std::string& name, const std::string& iNeverAwayObject);
    
    /**
     * 解析状态重载函数
     * 按语法文档：Void<A>, Void<B> 通过状态同名同参重载
     */
    std::vector<StateOverload> parseStateOverloads(const std::string& iNeverAwayObject);
    
    /**
     * 生成状态重载的全局函数
     */
    std::string generateStateOverloadFunctions(const std::vector<StateOverload>& overloads);
    
    // === 全局函数管理 ===
    
    /**
     * 生成唯一的全局函数名
     * 按语法文档：名称由CHTL编译器统一管理
     */
    std::string generateGlobalFunctionName(const std::string& virName, const std::string& functionKey);
    
    /**
     * 注册全局函数
     * 按语法文档：加入注册表防止重复创建
     */
    void registerGlobalFunction(const std::string& functionName, const std::string& functionCode);
    
    /**
     * 检查函数是否已存在
     */
    bool isFunctionRegistered(const std::string& functionName);
    
    /**
     * 生成所有全局函数的JavaScript代码
     */
    std::string generateAllGlobalFunctions();
    
    // === 虚对象调用处理 ===
    
    /**
     * 解析虚对象调用
     * test->click() 或 Test->Void<A>()
     */
    std::pair<std::string, std::string> parseVirtualCall(const std::string& callCode);
    
    /**
     * 转换为函数引用
     * 按语法文档：将虚对象调用替换成函数引用(函数的名称)
     */
    std::string convertToFunctionReference(const std::string& virName, const std::string& functionKey);
    
    /**
     * 处理状态重载调用
     * Test->Void<A>() 转换为对应的全局函数调用
     */
    std::string processStateOverloadCall(const std::string& virName, const std::string& stateFunction);
    
    // === 辅助方法 ===
    
    /**
     * 提取vir名称
     * 从 vir test = ... 中提取 test
     */
    std::string extractVirName(const std::string& virCode);
    
    /**
     * 提取源函数名称
     * 从 vir test = listen(...) 中提取 listen
     */
    std::string extractSourceFunction(const std::string& virCode);
    
    /**
     * 提取函数对象
     * 从 listen({ ... }) 或 iNeverAway({ ... }) 中提取 { ... }
     */
    std::string extractFunctionObject(const std::string& virCode);
    
    /**
     * 解析状态标识
     * 从 Void<A> 中提取 "Void" 和 "A"
     */
    std::pair<std::string, std::string> parseStateIdentifier(const std::string& stateFunction);
    
    /**
     * 验证虚对象名称
     */
    bool isValidVirName(const std::string& name);
    
    /**
     * 解析配置键值对
     * 从配置对象中解析键值对
     */
    std::vector<std::pair<std::string, std::string>> parseConfigKeyValuePairs(const std::string& configObject);
    
    /**
     * 分割配置条目
     * 将配置内容分割为独立条目
     */
    std::vector<std::string> splitConfigEntries(const std::string& content);
    
    /**
     * 验证函数键名称
     */
    bool isValidFunctionKey(const std::string& key);
    
    // === 错误处理 ===
    void addError(const std::string& message);
    void clearErrors();
    bool hasErrors() const;
    std::vector<std::string> getErrors() const;

private:
    std::shared_ptr<EnhancedSelectorProcessor> selectorProcessor_;
    std::vector<std::string> errors_;
    
    // 虚对象注册表
    std::unordered_map<std::string, VirtualObject> virtualObjectRegistry_;
    
    // 全局函数注册表
    std::unordered_map<std::string, std::string> globalFunctionRegistry_;
    std::unordered_set<std::string> registeredFunctionNames_;
    
    // 函数名计数器
    static int functionCounter_;
};

} // namespace chtljs
} // namespace chtl