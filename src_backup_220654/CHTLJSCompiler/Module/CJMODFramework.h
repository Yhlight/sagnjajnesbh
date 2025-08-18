#pragma once
#include <string>
#include <functional>
#include <vector>
#include <unordered_map>
#include <memory>
#include <any>

namespace chtl {
namespace cjmod {

/**
 * CJMOD Framework - 让CHTL JS扩展开发变得简单！
 * 
 * 设计理念：
 * 1. 零C++经验也能开发CHTL JS扩展
 * 2. 自动处理所有底层细节
 * 3. 提供直观的API和丰富的示例
 * 4. 完善的错误处理和调试支持
 */

// 前向声明
class CJMODModule;
class CJMODFunction;
class CJMODRegistry;

/**
 * CHTL JS 值类型 - 自动处理类型转换
 */
class JSValue {
public:
    enum Type {
        UNDEFINED,
        NULL_VALUE,
        BOOLEAN,
        NUMBER,
        STRING,
        OBJECT,
        ARRAY,
        FUNCTION
    };
    
    JSValue();
    JSValue(bool value);
    JSValue(int value);
    JSValue(double value);
    JSValue(const std::string& value);
    JSValue(const char* value);
    JSValue(const std::vector<JSValue>& array);
    JSValue(const std::unordered_map<std::string, JSValue>& object);
    
    // 类型检查
    Type getType() const { return type_; }
    bool isUndefined() const { return type_ == UNDEFINED; }
    bool isNull() const { return type_ == NULL_VALUE; }
    bool isBool() const { return type_ == BOOLEAN; }
    bool isNumber() const { return type_ == NUMBER; }
    bool isString() const { return type_ == STRING; }
    bool isObject() const { return type_ == OBJECT; }
    bool isArray() const { return type_ == ARRAY; }
    bool isFunction() const { return type_ == FUNCTION; }
    
    // 值获取 - 自动类型转换
    bool toBool() const;
    int toInt() const;
    double toDouble() const;
    std::string toString() const;
    std::vector<JSValue> toArray() const;
    std::unordered_map<std::string, JSValue> toObject() const;
    
    // 便利操作
    JSValue& operator[](const std::string& key);
    const JSValue& operator[](const std::string& key) const;
    JSValue& operator[](size_t index);
    const JSValue& operator[](size_t index) const;
    
    // 调试支持
    std::string toDebugString() const;
    
private:
    Type type_;
    std::any value_;
    
    void setUndefined();
};

/**
 * CHTL JS 函数上下文 - 提供执行环境信息
 */
struct JSContext {
    std::string module_name;           // 模块名称
    std::string function_name;         // 函数名称
    std::vector<JSValue> arguments;    // 参数列表
    std::unordered_map<std::string, JSValue> globals;  // 全局变量
    
    // 便利方法
    size_t getArgCount() const { return arguments.size(); }
    JSValue getArg(size_t index) const;
    JSValue getArg(size_t index, const JSValue& default_value) const;
    
    // 错误处理
    void throwError(const std::string& message) const;
    void logWarning(const std::string& message) const;
    void logInfo(const std::string& message) const;
};

/**
 * CJMOD 函数类型定义
 * 
 * 简单易用的函数签名：
 * JSValue myFunction(const JSContext& ctx)
 */
using CJMODFunctionType = std::function<JSValue(const JSContext&)>;

/**
 * CJMOD 函数定义 - 让函数注册变得超级简单
 */
class CJMODFunction {
public:
    CJMODFunction(const std::string& name, CJMODFunctionType func);
    CJMODFunction(const std::string& name, CJMODFunctionType func, const std::string& description);
    
    // 设置函数信息
    CJMODFunction& setDescription(const std::string& desc);
    CJMODFunction& addParameter(const std::string& name, const std::string& type, const std::string& desc = "");
    CJMODFunction& setReturnType(const std::string& type, const std::string& desc = "");
    CJMODFunction& addExample(const std::string& code, const std::string& desc = "");
    
    // 执行函数
    JSValue call(const JSContext& context) const;
    
    // 获取信息
    const std::string& getName() const { return name_; }
    const std::string& getDescription() const { return description_; }
    std::string generateDocumentation() const;
    
private:
    std::string name_;
    std::string description_;
    CJMODFunctionType function_;
    
    struct Parameter {
        std::string name;
        std::string type;
        std::string description;
    };
    
    std::vector<Parameter> parameters_;
    std::string return_type_;
    std::string return_description_;
    
    struct Example {
        std::string code;
        std::string description;
    };
    
    std::vector<Example> examples_;
};

/**
 * CJMOD 模块 - 超级简单的模块定义
 */
class CJMODModule {
public:
    CJMODModule(const std::string& name);
    CJMODModule(const std::string& name, const std::string& version);
    
    // 设置模块信息
    CJMODModule& setDescription(const std::string& desc);
    CJMODModule& setAuthor(const std::string& author);
    CJMODModule& setVersion(const std::string& version);
    CJMODModule& setHomepage(const std::string& url);
    
    // 注册函数 - 超级简单的方式！
    template<typename Func>
    CJMODModule& registerFunction(const std::string& name, Func func) {
        auto wrapper = [func](const JSContext& ctx) -> JSValue {
            return func(ctx);
        };
        functions_[name] = std::make_unique<CJMODFunction>(name, wrapper);
        return *this;
    }
    
    // 带描述的函数注册
    template<typename Func>
    CJMODModule& registerFunction(const std::string& name, Func func, const std::string& description) {
        auto wrapper = [func](const JSContext& ctx) -> JSValue {
            return func(ctx);
        };
        functions_[name] = std::make_unique<CJMODFunction>(name, wrapper, description);
        return *this;
    }
    
    // Lambda函数注册 - 最简单的方式！
    CJMODModule& addFunction(const std::string& name, CJMODFunctionType func);
    CJMODModule& addFunction(const std::string& name, CJMODFunctionType func, const std::string& description);
    
    // 批量注册 - 链式调用
    CJMODModule& functions(std::initializer_list<std::pair<std::string, CJMODFunctionType>> funcs);
    
    // 获取信息
    const std::string& getName() const { return name_; }
    const std::string& getVersion() const { return version_; }
    const std::string& getDescription() const { return description_; }
    
    // 函数管理
    bool hasFunction(const std::string& name) const;
    CJMODFunction* getFunction(const std::string& name) const;
    std::vector<std::string> getFunctionNames() const;
    
    // 代码生成
    std::string generateJavaScript() const;
    std::string generateDocumentation() const;
    
private:
    std::string name_;
    std::string version_;
    std::string description_;
    std::string author_;
    std::string homepage_;
    
    std::unordered_map<std::string, std::unique_ptr<CJMODFunction>> functions_;
};

/**
 * CJMOD 注册表 - 全局模块管理
 */
class CJMODRegistry {
public:
    static CJMODRegistry& getInstance();
    
    // 模块管理
    void registerModule(std::unique_ptr<CJMODModule> module);
    bool hasModule(const std::string& name) const;
    CJMODModule* getModule(const std::string& name) const;
    std::vector<std::string> getModuleNames() const;
    
    // 函数调用
    JSValue callFunction(const std::string& module_name, const std::string& function_name, 
                        const std::vector<JSValue>& args) const;
    
    // 代码生成
    std::string generateAllJavaScript() const;
    std::string generateModuleJavaScript(const std::string& module_name) const;
    
    // 文档生成
    std::string generateAllDocumentation() const;
    std::string generateModuleDocumentation(const std::string& module_name) const;
    
    // 调试和统计
    void printRegistryInfo() const;
    size_t getModuleCount() const { return modules_.size(); }
    size_t getTotalFunctionCount() const;
    
private:
    CJMODRegistry() = default;
    std::unordered_map<std::string, std::unique_ptr<CJMODModule>> modules_;
    static std::unique_ptr<CJMODRegistry> instance_;
};

/**
 * 便利宏定义 - 让开发更简单！
 */

// 定义模块
#define CJMOD_MODULE(name) \
    static std::unique_ptr<chtl::cjmod::CJMODModule> create_##name##_module() { \
        auto module = std::make_unique<chtl::cjmod::CJMODModule>(#name);

// 结束模块定义
#define CJMOD_MODULE_END(name) \
        return module; \
    } \
    static bool name##_registered = []() { \
        chtl::cjmod::CJMODRegistry::getInstance().registerModule(create_##name##_module()); \
        return true; \
    }();

// 简单函数定义
#define CJMOD_FUNCTION(name, func) \
    module->addFunction(#name, func);

// 带描述的函数定义
#define CJMOD_FUNCTION_DESC(name, func, desc) \
    module->addFunction(#name, func, desc);

/**
 * 辅助函数 - 让开发更轻松
 */
namespace helpers {
    // 类型转换辅助
    template<typename T>
    T getValue(const JSValue& value);
    
    template<typename T>
    JSValue makeValue(const T& value);
    
    // 参数验证辅助
    bool validateArgCount(const JSContext& ctx, size_t expected);
    bool validateArgCount(const JSContext& ctx, size_t min, size_t max);
    bool validateArgType(const JSContext& ctx, size_t index, JSValue::Type expected);
    
    // 错误处理辅助
    JSValue makeError(const std::string& message);
    JSValue makeSuccess(const JSValue& result = JSValue());
    
    // 异步支持辅助
    JSValue makePromise(std::function<JSValue()> executor);
    
    // 调试辅助
    void logDebug(const std::string& message);
    void logInfo(const std::string& message);
    void logWarning(const std::string& message);
    void logError(const std::string& message);
}

} // namespace cjmod
} // namespace chtl