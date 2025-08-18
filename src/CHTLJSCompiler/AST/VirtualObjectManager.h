#pragma once
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <memory>

namespace chtl {
namespace ast {

// 函数类型枚举
enum class FunctionType {
    INLINE_ARROW,      // () => { }
    INLINE_FUNCTION,   // function() { }
    REFERENCE,         // 函数引用（已存在的函数名）
    UNKNOWN
};

// 函数信息结构
struct FunctionInfo {
    std::string name;              // 函数名或键名
    FunctionType type;             // 函数类型
    std::string content;           // 函数内容（仅对内联函数）
    std::string reference_name;    // 引用的函数名（仅对引用类型）
    bool is_generated;             // 是否已生成代码
    std::string generated_name;    // 生成的全局函数名
};

// 虚对象信息
struct VirtualObjectInfo {
    std::string name;              // 虚对象名
    std::string source_function;   // 源函数（listen、delegate等）
    std::unordered_map<std::string, FunctionInfo> functions; // 函数映射
    bool is_processed;             // 是否已处理
};

// 虚对象管理器
class VirtualObjectManager {
public:
    VirtualObjectManager();
    ~VirtualObjectManager();
    
    // 虚对象注册
    bool registerVirtualObject(const std::string& vir_name, const std::string& source_function);
    bool addFunctionToVirtualObject(const std::string& vir_name, const std::string& key, 
                                   const std::string& function_content);
    
    // 函数类型检测
    FunctionType detectFunctionType(const std::string& function_content);
    bool isInlineFunction(const std::string& content);
    bool isFunctionReference(const std::string& content);
    
    // 函数引用管理
    bool registerExistingFunction(const std::string& function_name);
    bool isFunctionExists(const std::string& function_name);
    std::vector<std::string> getExistingFunctions() const;
    
    // 代码生成优化
    std::string generateVirtualObjectCode(const std::string& vir_name);
    std::string generateFunctionWrapper(const VirtualObjectInfo& vir_info, const std::string& key);
    bool shouldGenerateFunction(const FunctionInfo& func_info);
    
    // 全局函数名管理
    std::string generateUniqueFunctionName(const std::string& vir_name, const std::string& key);
    bool isGlobalFunctionNameUsed(const std::string& name);
    void markGlobalFunctionNameUsed(const std::string& name);
    
    // 虚对象访问
    VirtualObjectInfo* getVirtualObject(const std::string& vir_name);
    std::vector<std::string> getVirtualObjectNames() const;
    bool hasVirtualObject(const std::string& vir_name);
    
    // 函数调用解析
    struct VirtualCall {
        std::string vir_name;
        std::string function_key;
        std::string resolved_function_name;
        bool is_valid;
    };
    
    VirtualCall parseVirtualCall(const std::string& call_expression);
    std::string resolveVirtualCall(const std::string& vir_name, const std::string& function_key);
    
    // 依赖分析
    std::vector<std::string> analyzeFunctionDependencies(const std::string& function_content);
    bool hasFunctionDependency(const std::string& function_name);
    
    // 清理和重置
    void clear();
    void clearVirtualObject(const std::string& vir_name);
    
    // 调试和诊断
    void printVirtualObjects() const;
    std::string getStatistics() const;
    
private:
    std::unordered_map<std::string, std::unique_ptr<VirtualObjectInfo>> virtual_objects_;
    std::unordered_set<std::string> existing_functions_;        // 已存在的函数名注册表
    std::unordered_set<std::string> generated_function_names_; // 已生成的全局函数名
    std::unordered_map<std::string, std::string> function_name_mapping_; // 函数名映射
    
    // 内部辅助方法
    std::string extractFunctionName(const std::string& content);
    std::string extractFunctionParameters(const std::string& content);
    std::string extractFunctionBody(const std::string& content);
    
    // 代码分析
    bool isArrowFunction(const std::string& content);
    bool isRegularFunction(const std::string& content);
    bool isSimpleIdentifier(const std::string& content);
    
    // 名称生成
    std::string sanitizeFunctionName(const std::string& name);
    std::string generateSafeFunctionName(const std::string& base_name);
    
    // 错误处理
    std::string last_error_;
    void setError(const std::string& error);
};

// 虚对象相关的工具函数
namespace VirtualObjectUtils {
    // 函数内容解析
    std::pair<std::string, std::string> parseFunctionSignature(const std::string& content);
    std::string extractFunctionBody(const std::string& content);
    
    // 代码生成辅助
    std::string generateFunctionWrapper(const std::string& original_name, 
                                       const std::string& wrapper_name,
                                       const std::string& parameters);
    
    // 函数名验证
    bool isValidJavaScriptIdentifier(const std::string& name);
    bool isReservedKeyword(const std::string& name);
    
    // 依赖分析
    std::vector<std::string> extractFunctionCalls(const std::string& code);
    std::vector<std::string> extractVariableReferences(const std::string& code);
}

} // namespace ast
} // namespace chtl