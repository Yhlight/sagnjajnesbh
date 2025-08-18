#pragma once
#include <string>
#include <unordered_map>
#include <vector>
#include <functional>
#include <memory>
#include <sstream>

namespace chtl {
namespace chtholly {

/**
 * 函数签名信息
 */
struct FunctionSignature {
    std::string name;           // 函数名（可能包含状态标识如<A>）
    std::string state;          // 状态标识（如A, B）
    std::vector<std::string> parameter_types;  // 参数类型列表
    std::string return_type;    // 返回类型
    std::string body;           // 函数体内容
    bool is_inline_function;    // 是否为内联函数
    
    FunctionSignature() : is_inline_function(false) {}
    
    // 生成唯一标识符
    std::string getUniqueId() const;
    
    // 生成JavaScript函数名
    std::string generateJSFunctionName(const std::string& namespace_prefix) const;
    
    // 检查是否与另一个签名冲突
    bool conflictsWith(const FunctionSignature& other) const;
};

/**
 * 虚对象定义
 */
struct VirtualObjectDefinition {
    std::string name;           // 虚对象名称
    std::string namespace_id;   // 命名空间标识
    std::unordered_map<std::string, FunctionSignature> functions;  // 函数映射
    
    VirtualObjectDefinition() = default;
    VirtualObjectDefinition(const std::string& n) : name(n) {}
    
    // 添加函数
    bool addFunction(const FunctionSignature& func);
    
    // 检查函数是否存在
    bool hasFunction(const std::string& func_name, const std::string& state = "") const;
    
    // 获取函数
    const FunctionSignature* getFunction(const std::string& func_name, const std::string& state = "") const;
    
    // 生成所有相关的JavaScript代码
    std::string generateAllJSFunctions(const std::string& namespace_prefix) const;
};

/**
 * iNeverAway函数调用信息
 */
struct INeverAwayCall {
    std::string virtual_object_name;    // 虚对象名称
    std::string function_name;          // 调用的函数名
    std::string state;                  // 状态标识
    std::vector<std::string> arguments; // 参数列表
    
    INeverAwayCall() = default;
    
    // 解析调用语法 如 "Test->Void<A>()"
    static INeverAwayCall parseCall(const std::string& call_syntax);
    
    // 生成对应的JavaScript调用代码
    std::string generateJSCall(const std::string& namespace_prefix) const;
};

/**
 * iNeverAway核心管理器
 * 管理虚对象的创建、函数重载、代码生成等
 */
class INeverAwayManager {
public:
    static INeverAwayManager& getInstance();
    
    // 虚对象管理
    bool registerVirtualObject(const std::string& name, const VirtualObjectDefinition& definition);
    bool hasVirtualObject(const std::string& name) const;
    const VirtualObjectDefinition* getVirtualObject(const std::string& name) const;
    
    // 函数重载处理
    std::string processINeverAwayDefinition(const std::string& definition_code);
    VirtualObjectDefinition parseVirtualObjectDefinition(const std::string& definition_code);
    
    // 调用处理
    std::string processINeverAwayCall(const std::string& call_code);
    std::string generateJSCallCode(const INeverAwayCall& call);
    
    // 代码生成
    std::string generateAllJSCode();
    std::string generateVirtualObjectJS(const std::string& virtual_object_name);
    
    // 命名空间管理
    void setNamespacePrefix(const std::string& prefix) { namespace_prefix_ = prefix; }
    std::string getNamespacePrefix() const { return namespace_prefix_; }
    std::string generateUniqueFunctionName(const std::string& base_name);
    
    // 状态和重载管理
    bool validateOverloading(const VirtualObjectDefinition& definition);
    std::vector<std::string> detectOverloadConflicts(const VirtualObjectDefinition& definition);
    
    // 调试和统计
    struct ManagerStatistics {
        size_t virtual_objects_count;
        size_t total_functions_count;
        size_t overloaded_functions_count;
        size_t generated_js_functions_count;
        
        ManagerStatistics() : virtual_objects_count(0), total_functions_count(0),
                             overloaded_functions_count(0), generated_js_functions_count(0) {}
    };
    
    ManagerStatistics getStatistics() const;
    void printDebugInfo() const;
    std::string getErrorMessage() const { return last_error_; }
    
    // 清理和重置
    void clear();
    void clearVirtualObject(const std::string& name);
    
private:
    INeverAwayManager() : namespace_prefix_("chtl_ineveraway_"), function_counter_(0) {}
    
    std::unordered_map<std::string, VirtualObjectDefinition> virtual_objects_;
    std::string namespace_prefix_;
    size_t function_counter_;
    std::string last_error_;
    
    static std::unique_ptr<INeverAwayManager> instance_;
    
    // 内部解析方法
    FunctionSignature parseFunctionSignature(const std::string& signature_line);
    std::string extractFunctionName(const std::string& signature);
    std::string extractState(const std::string& function_name);
    std::vector<std::string> parseParameterTypes(const std::string& params_str);
    std::string extractReturnType(const std::string& signature);
    
    // 代码生成辅助
    std::string generateJSFunctionBody(const FunctionSignature& func);
    std::string wrapInNamespace(const std::string& code);
    
    // 验证辅助
    bool isValidFunctionName(const std::string& name);
    bool isValidState(const std::string& state);
    
    // 错误处理
    void setError(const std::string& error) { last_error_ = error; }
    void clearError() { last_error_.clear(); }
};

/**
 * iNeverAway CHTL JS接口
 * 这是暴露给CHTL JS编译器的接口
 */
class INeverAwayInterface {
public:
    static INeverAwayInterface& getInstance();
    
    // CHTL JS语法处理
    std::string processVirDeclaration(const std::string& vir_code);
    std::string processVirCall(const std::string& call_code);
    
    // 编译器集成
    std::string generatePreludeCode();  // 生成前导代码
    std::string generateEpilogueCode(); // 生成结尾代码
    bool hasGeneratedCode() const;
    
    // 配置管理
    void setCompilerContext(const std::string& context) { compiler_context_ = context; }
    std::string getCompilerContext() const { return compiler_context_; }
    
    // 错误处理
    bool hasError() const;
    std::string getLastError() const;
    void clearError();
    
    // 调试支持
    void enableDebugMode(bool enable) { debug_mode_ = enable; }
    bool isDebugMode() const { return debug_mode_; }
    void printProcessingInfo() const;
    
private:
    INeverAwayInterface() : debug_mode_(false) {}
    
    std::unique_ptr<INeverAwayManager> manager_;
    std::string compiler_context_;
    bool debug_mode_;
    
    static std::unique_ptr<INeverAwayInterface> instance_;
    
    // 语法解析辅助
    bool isVirDeclaration(const std::string& code);
    bool isVirCall(const std::string& code);
    std::string extractVirName(const std::string& declaration);
    std::string extractINeverAwayBody(const std::string& declaration);
};

/**
 * 函数重载解析器
 * 专门处理同名函数的状态区分和参数重载
 */
class OverloadResolver {
public:
    struct OverloadGroup {
        std::string base_name;
        std::vector<FunctionSignature> overloads;
        
        OverloadGroup(const std::string& name) : base_name(name) {}
    };
    
    static std::vector<OverloadGroup> groupOverloads(const std::vector<FunctionSignature>& functions);
    static bool validateOverloadGroup(const OverloadGroup& group);
    static std::string generateDispatcherCode(const OverloadGroup& group, const std::string& namespace_prefix);
    
private:
    static std::string generateParameterTypeCheck(const std::vector<std::string>& types);
    static std::string generateStateCheck(const std::string& state);
};

} // namespace chtholly
} // namespace chtl