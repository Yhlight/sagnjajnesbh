#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <functional>
#include <memory>

namespace chtl {
namespace cjmod {
namespace chtholly {

/**
 * iNeverAway - 珂朵莉标记函数组
 * 严格按照CHTL语法文档实现
 * 
 * 功能：创建一组标记函数，允许开发者定义键而不是使用键，支持状态区分同名键
 * 
 * 使用方式：
 * vir Test = iNeverAway({
 *     Void<A>: function(int, int) { },
 *     Void<B>: function(int, int) { },  // 通过状态同名同参重载
 *     Void: { },
 *     Ax: { }
 * });
 * 
 * Test->Void<A>();
 * 
 * 实现原理：
 * - 创建一组JS全局函数，名称由CHTL编译器统一管理
 * - 在调用时才生成对应的JS函数代码
 * - vir对象本身不存在，最终转变成相对应的函数的引用
 */

struct StateOverloadKey {
    std::string baseName;      // 基础名称 (如 "Void")
    std::string state;         // 状态标识 (如 "A", "B")
    std::string fullKey;       // 完整键名 (如 "Void<A>")
    std::vector<std::string> paramTypes; // 参数类型列表
};

struct MarkerFunction {
    std::string key;                    // 函数键名
    std::string functionCode;           // 函数代码
    std::string generatedName;          // 生成的全局函数名
    std::vector<std::string> paramTypes; // 参数类型
    bool isStateOverload = false;       // 是否为状态重载
    StateOverloadKey stateInfo;         // 状态信息（如果是状态重载）
    bool isGenerated = false;           // 是否已生成
};

struct INeverAwayObject {
    std::string virName;                                    // 虚对象名称
    std::unordered_map<std::string, MarkerFunction> functions; // 标记函数映射
    std::unordered_map<std::string, std::vector<MarkerFunction>> stateOverloads; // 状态重载组
    std::vector<std::string> generatedFunctionNames;        // 生成的全局函数名列表
};

class INeverAwayProcessor {
public:
    INeverAwayProcessor();
    ~INeverAwayProcessor();
    
    /**
     * 按语法文档：处理iNeverAway定义
     * vir Test = iNeverAway({ ... });
     */
    INeverAwayObject processINeverAwayDefinition(const std::string& virName, const std::string& functionObject);
    
    /**
     * 按语法文档：处理iNeverAway调用
     * Test->Void<A>(); 或 Test->Ax();
     */
    std::string processINeverAwayCall(const std::string& virName, const std::string& functionKey);
    
    /**
     * 解析函数对象
     * 从 { Void<A>: function() {}, Void<B>: function() {} } 中解析所有函数
     */
    std::unordered_map<std::string, MarkerFunction> parseFunctionObject(const std::string& functionObject);
    
    /**
     * 解析状态重载键
     * 从 "Void<A>" 中解析出 baseName="Void", state="A"
     */
    StateOverloadKey parseStateOverloadKey(const std::string& key);
    
    /**
     * 生成全局函数
     * 按语法文档：创建一组JS全局函数，名称由CHTL编译器统一管理
     */
    std::string generateGlobalFunction(const MarkerFunction& function, const std::string& virName);
    
    /**
     * 生成函数调用代码
     * 按语法文档：在调用时才生成对应的JS函数代码
     */
    std::string generateFunctionCall(const std::string& virName, const std::string& functionKey);
    
    /**
     * 处理状态重载
     * 按语法文档：通过状态区分同名的键
     */
    std::string processStateOverload(const std::string& virName, const StateOverloadKey& stateKey);
    
    /**
     * 生成所有标记函数的JavaScript代码
     * 按语法文档：vir对象本身不存在，最终转变成相对应的函数的引用
     */
    std::string generateAllMarkerFunctions(const INeverAwayObject& obj);
    
    /**
     * 验证函数键格式
     * 支持普通键名和状态重载键名
     */
    bool isValidFunctionKey(const std::string& key);
    
    /**
     * 检查是否为状态重载格式
     * Void<A> 格式检查
     */
    bool isStateOverloadKey(const std::string& key);
    
    /**
     * 生成唯一的全局函数名
     * 按语法文档：名称由CHTL编译器统一管理
     */
    std::string generateUniqueFunctionName(const std::string& virName, const std::string& functionKey);

private:
    // 全局函数名管理
    static int functionCounter_;
    std::unordered_map<std::string, INeverAwayObject> virObjects_;
    
    // 辅助方法
    std::string cleanFunctionCode(const std::string& code);
    std::vector<std::string> parseParameterTypes(const std::string& functionSignature);
    std::string sanitizeName(const std::string& name);
};

// === CHTL JS接口函数 ===

/**
 * 按语法文档：CHTL JS接口函数
 * 在CHTL JS中调用：vir Test = iNeverAway({ ... });
 */
extern "C" {
    const char* chtl_iNeverAway_create(const char* virName, const char* functionObjectJson);
    const char* chtl_iNeverAway_call(const char* virName, const char* functionKey);
    void chtl_iNeverAway_cleanup(const char* result);
}

} // namespace chtholly
} // namespace cjmod
} // namespace chtl