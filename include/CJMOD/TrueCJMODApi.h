#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <functional>
#include <memory>

namespace CHTL {
namespace CJMOD {

// 前向声明
class Scanner;
struct CodeFragment;

/**
 * @brief 扫描策略枚举 - 按照用户原始设计
 */
enum class Policy {
    NORMAL,     // 常规
    COLLECT,    // 收集  
    SKIP        // 跳过
};

/**
 * @brief Arg类 - 严格按照用户原始API设计
 * 
 * 核心特性：
 * 1. 支持operator()重载接受函数
 * 2. 支持bind函数处理
 * 3. 占位符自动计数
 */
class Arg {
public:
    Arg(const std::string& name = "", bool isPlaceholder = false);
    
    // 用户核心设计：operator()重载 - 接受函数而不是值
    template<typename T>
    void operator()(std::function<std::string(T)> func) {
        bindFunction_ = [func](const std::string& str) -> std::string {
            // 根据类型转换并调用函数
            if constexpr (std::is_same_v<T, std::string>) {
                return func(str);
            } else if constexpr (std::is_same_v<T, int>) {
                try {
                    int value = std::stoi(str);
                    return func(value);
                } catch (...) {
                    return func(0);
                }
            } else {
                return func(T{});
            }
        };
        hasBind_ = true;
    }
    
    // 无参数函数重载
    void operator()(std::function<std::string()> func) {
        bindFunction_ = [func](const std::string&) -> std::string {
            return func();
        };
        hasBind_ = true;
    }
    
    // bind方法
    template<typename T>
    void bind(const std::function<std::string(T)>& func) {
        operator()(func);
    }
    
    void bind(const std::function<std::string()>& func) {
        operator()(func);
    }
    
    // match方法 - 匹配参数值并调用绑定函数
    void match(const std::string& value);
    
    // transform方法 - 转换为JS代码
    void transform(const std::string& jsTemplate);
    
    // 获取器
    const std::string& getName() const { return name_; }
    bool isPlaceholder() const { return isPlaceholder_; }
    bool hasBind() const { return hasBind_; }
    bool hasValue() const { return hasValue_; }
    const std::string& getValue() const { return processedValue_; }
    const std::string& getJSCode() const { return jsCode_; }
    
    // 输出重载
    friend std::ostream& operator<<(std::ostream& os, const Arg& arg);

private:
    std::string name_;
    bool isPlaceholder_;
    bool hasBind_;
    bool hasValue_;
    std::string rawValue_;
    std::string processedValue_;
    std::string jsTemplate_;
    std::string jsCode_;
    std::function<std::string(const std::string&)> bindFunction_;
    
    void applyTransform();
};

/**
 * @brief Syntax类 - 严格按照用户原始设计
 * 
 * 核心特性：
 * 1. 维护args向量
 * 2. 提供bind方法自动搜索参数
 * 3. 支持索引访问
 */
class Syntax {
public:
    Syntax() = default;
    
    // 用户核心设计：args向量
    std::vector<Arg> args;
    
    // 绑定方法 - 自动搜索参数名
    template<typename T>
    void bind(const std::string& name, const std::function<std::string(T)>& func) {
        auto* arg = findArg(name);
        if (arg) {
            arg->bind(func);
        }
    }
    
    void bind(const std::string& name, const std::function<std::string()>& func) {
        auto* arg = findArg(name);
        if (arg) {
            arg->bind(func);
        }
    }
    
    // match方法
    void match(const std::string& name, const std::string& value);
    
    // transform方法
    void transform(const std::string& name, const std::string& jsTemplate);
    
    // result方法 - 组合所有参数的JS代码
    std::string result();
    
    // 工具方法
    size_t length() const { return args.size(); }
    void addArg(const std::string& name, bool isPlaceholder = false);
    void setTriggerKeyword(const std::string& keyword);
    const std::string& getTriggerKeyword() const { return triggerKeyword_; }

private:
    std::unordered_map<std::string, size_t> nameToIndex_;
    size_t placeholderCounter_ = 0;
    std::string triggerKeyword_;
    
    Arg* findArg(const std::string& name);
};

/**
 * @brief CHTLJSFunction类 - 严格按照用户原始设计
 * 
 * 核心特性：
 * 1. 完整的原始API流程
 * 2. 与统一扫描器集成
 * 3. 辅助函数支持
 */
class CHTLJSFunction {
public:
    CHTLJSFunction();
    
    // 原始API核心流程：syntaxAnalys -> bind -> transform -> scanKeyword -> match -> result -> generateCode
    std::unique_ptr<Syntax> syntaxAnalys(const std::string& pattern, const std::string& ignoreChars = "");
    
    void scanKeyword(const Arg& arg, std::function<void()> handler);
    void scanKeyword(const std::string& keyword, std::function<void()> handler);
    
    // 辅助函数 - 按照用户原始设计
    std::string peekKeyword(int offset) const;
    bool isObject(const std::string& content);
    bool isFunction(const std::string& content);
    std::string slice(const std::string& content, size_t start, size_t end = std::string::npos);
    
    // Policy系统 - 按照用户原始设计
    void policyChangeBegin(const std::string& trigger, Policy policy);
    std::string policyChangeEnd(const std::string& trigger, Policy policy);
    
    // 代码生成
    std::string generateCode(const Syntax& syntax);
    
    // 与统一扫描器集成
    void setCodeFragment(const CodeFragment& fragment);
    std::string processSourceCode(const std::string& sourceCode, const std::string& pattern);

private:
    CodeFragment* currentFragment_;
    bool hasFragment_;
    Policy currentPolicy_;
    std::unordered_map<std::string, std::function<void()>> keywordHandlers_;
    
    struct PolicyState {
        std::string trigger;
        Policy policy;
    };
    std::vector<PolicyState> policyStack_;
    
    // 内部辅助方法
    std::vector<std::string> tokenize(const std::string& input, const std::string& ignoreChars);
    void processPolicyChange(const std::string& trigger, Policy policy);
    std::string optimizeJSCode(const std::string& jsCode);
};

// ============================================================================
// 全局函数 - 按照用户原始设计
// ============================================================================

/**
 * @brief syntaxAnalys全局函数 - 用户原始设计的入口点
 */
std::unique_ptr<Syntax> syntaxAnalys(const std::string& pattern, const std::string& ignoreChars = "");

/**
 * @brief generateCode全局函数 - 用户原始设计的代码生成
 */
std::string generateCode(const Syntax& syntax);

/**
 * @brief createCHTLJSFunction - 简化流程，自动化原始API流程
 * 
 * 根据用户要求：自动化官方流程，让用户通过一个函数就能快速创建CHTL JS函数
 */
std::string createCHTLJSFunction(const std::string& chtlJsCode);

} // namespace CJMOD
} // namespace CHTL

/**
 * @brief 🎯 真正的CJMOD API设计 - 严格按照用户原始设计
 * 
 * 【核心流程】
 * syntaxAnalys -> bind -> transform -> scanKeyword -> match -> result -> generateCode
 * 
 * 【三个核心类】
 * ✅ Arg：支持operator()重载接受函数，占位符处理
 * ✅ Syntax：维护args向量，提供bind自动搜索
 * ✅ CHTLJSFunction：完整流程，辅助函数，Policy系统
 * 
 * 【关键特性】
 * ✅ 占位符使用$符号
 * ✅ peekKeyword获取前后关键字内容
 * ✅ Policy系统：NORMAL, COLLECT, SKIP
 * ✅ 辅助函数：isObject, isFunction, slice
 * ✅ 与统一扫描器集成
 * ✅ createCHTLJSFunction简化流程
 */