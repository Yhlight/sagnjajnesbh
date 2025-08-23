#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <functional>
#include <memory>

// ============================================================================
// CJMOD 全局注册表 - 根据语法文档第1425行：允许编写全局变量管理状态
// ============================================================================

// 全局虚对象注册表
extern std::unordered_map<std::string, std::string> g_virtualObjectRegistry;

// 全局函数注册表 - 防止重复创建（语法文档第1310行）
extern std::unordered_map<std::string, std::string> g_globalFunctionRegistry;

// 事件委托注册表 - 管理所有事件委托的父元素（语法文档第1230行）
extern std::unordered_map<std::string, std::vector<std::string>> g_eventDelegateRegistry;

// ============================================================================
// 基础CJMOD API - 三个核心类
// ============================================================================

namespace CHTL { namespace CJMOD {

enum class Policy { NORMAL, COLLECT, SKIP };

struct CodeFragment {
    std::string originalCode;
    std::vector<std::string> tokens;
    std::string triggerKeyword;
    size_t position;
    
    CodeFragment() : position(0) {}
    CodeFragment(const std::string& code, const std::string& keyword, size_t pos)
        : originalCode(code), triggerKeyword(keyword), position(pos) {}
};

// 前向声明
class Arg;
class Syntax;
class CHTLJSFunction;

class Arg {
public:
    Arg(const std::string& name = "", bool isPlaceholder = false);
    
    // 用户核心设计：operator()重载
    template<typename T>
    std::string operator()(T value) {
        if (hasBind_ && bindFunction_) {
            processedValue_ = bindFunction_(std::to_string(value));
        } else {
            processedValue_ = std::to_string(value);
        }
        hasValue_ = true;
        applyTransform();
        return processedValue_;
    }
    
    // 绑定函数
    template<typename T>
    void bind(const std::function<std::string(T)>& func) {
        bindFunction_ = [func](const std::string& str) {
            return func(str);
        };
        hasBind_ = true;
    }
    
    void bind(const std::function<std::string()>& func);
    void match(const std::string& value);
    void transform(const std::string& jsTemplate);
    
    const std::string& getName() const { return name_; }
    bool isPlaceholder() const { return isPlaceholder_; }
    bool hasBind() const { return hasBind_; }
    bool hasValue() const { return hasValue_; }
    const std::string& getValue() const { return processedValue_; }
    const std::string& getJSCode() const { return jsCode_; }

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

class Syntax {
public:
    Syntax() = default;
    
    // 用户核心设计：args向量
    std::vector<Arg> args;
    
    // 绑定方法
    template<typename T>
    void bind(const std::string& name, const std::function<std::string(T)>& func) {
        auto* arg = findArg(name);
        if (arg) {
            arg->bind(func);
        }
    }
    
    void match(const std::string& name, const std::string& value);
    void transform(const std::string& name, const std::string& jsTemplate);
    std::string result();
    
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

class CHTLJSFunction {
public:
    CHTLJSFunction();
    
    // 原始API核心流程：syntaxAnalys -> bind -> transform -> scanKeyword -> match -> result -> generateCode
    std::unique_ptr<Syntax> syntaxAnalys(const std::string& pattern, const std::string& ignoreChars = "");
    void scanKeyword(const Arg& arg, std::function<void()> handler);
    void scanKeyword(const std::string& keyword, std::function<void()> handler);
    std::string peekKeyword(int offset) const;
    void policyChangeBegin(const std::string& trigger, Policy policy);
    std::string policyChangeEnd(const std::string& trigger, Policy policy);
    std::string generateCode(const Syntax& syntax);
    bool isObject(const std::string& content);
    bool isFunction(const std::string& content);
    std::string slice(const std::string& content, size_t start, size_t end = std::string::npos);
    
    void setCodeFragment(const CodeFragment& fragment);
    std::string processSourceCode(const std::string& sourceCode, const std::string& pattern);

private:
    CodeFragment currentFragment_;
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
    std::string formatJSCode(const std::string& jsCode);
};

// 全局便捷函数
std::unique_ptr<Syntax> syntaxAnalys(const std::string& pattern, const std::string& ignoreChars = "");
std::string generateCode(const Syntax& syntax);
std::unique_ptr<CHTLJSFunction> createCJMODProcessor();

} // namespace CJMOD
} // namespace CHTL

// ============================================================================
// Test CJMOD 扩展函数声明
// ============================================================================

/**
 * @brief testFunction - 测试基础CJMOD功能
 * 
 * 演示标准流程：syntaxAnalys -> bind -> transform -> scanKeyword -> match -> result -> generateCode
 * 从外部获取值，不是手动填写
 */
std::string testFunction(const std::string& sourceCode);

/**
 * @brief simpleTest - 简单测试函数
 * 
 * 演示最基础的CJMOD扩展
 */
std::string simpleTest(const std::string& input, const std::string& mode);