#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <functional>
#include <memory>

namespace CHTL { namespace CJMOD {

// Forward declarations
class Arg;
class Syntax;
class CHTLJSFunction;

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

// ============================================================================
// CJMOD 全局注册表 - 根据语法文档第1425行：允许编写全局变量管理状态
// ============================================================================

/**
 * @brief 全局虚对象注册表
 * 
 * 根据语法文档第1230行：需要创建一个全局注册表，管理所有事件委托的父元素
 * 根据语法文档第1425行：CJMOD可以自由包含头文件，编写全局变量管理状态
 */
extern std::unordered_map<std::string, std::string> g_virtualObjectRegistry;

/**
 * @brief 全局函数注册表
 * 
 * 根据语法文档第1310行：创建对应的全局函数并加入注册表之中防止重复创建
 */
extern std::unordered_map<std::string, std::string> g_globalFunctionRegistry;

/**
 * @brief 事件委托注册表
 * 
 * 根据语法文档第1230行：需要创建一个全局注册表，管理所有事件委托的父元素
 */
extern std::unordered_map<std::string, std::vector<std::string>> g_eventDelegateRegistry;

// ============================================================================
// 核心三类：Arg, Syntax, CHTLJSFunction
// ============================================================================

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
            // 这里需要类型转换，简化处理
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

// ============================================================================
// 简化流程 - createCHTLJSFunction自动集成原始API流程
// ============================================================================

/**
 * @brief createCHTLJSFunction - 自动集成原始API流程的简化版本
 * 
 * 根据用户要求：简化流程只是能够通过createCHTLJSFunction自动集成原始API的流程
 * 不是重新实现复杂的功能，而是让原始API更容易使用
 */
class SimplifiedCHTLJSFunction : public CHTLJSFunction {
public:
    SimplifiedCHTLJSFunction(const std::string& functionName);
    
    /**
     * @brief 自动化的原始API流程
     * 
     * 内部自动执行：syntaxAnalys -> bind -> transform -> scanKeyword -> match -> result -> generateCode
     * 用户只需要提供模式和参数值即可
     */
    std::string autoProcess(const std::string& pattern, 
                          const std::vector<std::string>& values,
                          const std::string& ignoreChars = ",:{};()");
    
    /**
     * @brief 获取函数名称
     */
    const std::string& getFunctionName() const { return functionName_; }

private:
    std::string functionName_;
};

// ============================================================================
// CHTL JS 虚对象委托接口 - 委托给CHTL JS，不重新实现
// ============================================================================

/**
 * @brief 虚对象委托接口
 * 
 * 根据用户要求：vir功能需要的是委托给CHTL JS，不是重新实现
 * 根据语法文档第1296-1310行：vir是CHTL JS层面的语法糖，编译器处理
 */
namespace VirtualObjectDelegate {
    
    /**
     * @brief 注册虚对象到全局注册表
     * 
     * 根据语法文档第1309行：记录vir虚对象的名称
     */
    void registerVirtualObject(const std::string& virName, const std::string& sourceFunction);
    
    /**
     * @brief 委托虚对象调用给CHTL JS编译器
     * 
     * 根据语法文档第1310行：遇到Test->click时，创建对应的全局函数并加入注册表
     */
    std::string delegateVirtualCall(const std::string& virName, const std::string& methodName);
    
    /**
     * @brief 检查虚对象是否已注册
     */
    bool isVirtualObjectRegistered(const std::string& virName);
    
    /**
     * @brief 清理虚对象注册表
     */
    void clearVirtualObjectRegistry();
}

// ============================================================================
// 全局便捷函数
// ============================================================================

/**
 * @brief syntaxAnalys全局函数
 */
std::unique_ptr<Syntax> syntaxAnalys(const std::string& pattern, const std::string& ignoreChars = "");

/**
 * @brief generateCode全局函数
 */
std::string generateCode(const Syntax& syntax);

/**
 * @brief createCJMODProcessor - 创建标准CJMOD处理器
 */
std::unique_ptr<CHTLJSFunction> createCJMODProcessor();

/**
 * @brief createCHTLJSFunction - 创建简化的CHTL JS函数处理器
 * 
 * 这就是简化流程：自动集成原始API流程，让开发更简单
 */
std::unique_ptr<SimplifiedCHTLJSFunction> createCHTLJSFunction(const std::string& functionName);

} // namespace CJMOD
} // namespace CHTL

/**
 * @brief 🎯 正确的CJMOD API设计总结：
 * 
 * 【遵循CHTL语法文档】
 * ✅ CJMOD允许包含头文件和全局变量作为注册表（第1425行）
 * ✅ 全局注册表管理虚对象、全局函数、事件委托（第1230、1310行）
 * ✅ vir是CHTL JS层面的语法糖，委托给编译器处理（第1296-1310行）
 * 
 * 【三个核心类保持不变】
 * ✅ Arg：参数处理核心，支持operator()重载
 * ✅ Syntax：语法容器，维护args向量
 * ✅ CHTLJSFunction：完整的CJMOD处理器
 * 
 * 【简化流程的正确理解】
 * ✅ createCHTLJSFunction：自动集成原始API流程
 * ✅ SimplifiedCHTLJSFunction：继承CHTLJSFunction，提供autoProcess
 * ✅ 不重新实现复杂功能，只是让原始API更容易使用
 * 
 * 【虚对象的正确处理】
 * ✅ VirtualObjectDelegate：委托接口，不重新实现vir功能
 * ✅ 使用全局注册表管理虚对象状态
 * ✅ 委托给CHTL JS编译器处理vir语法糖
 * 
 * 💡 设计哲学：
 * - 严格遵循CHTL语法文档的定义
 * - 不用C++开发者眼光过度设计
 * - CJMOD通过接口扩展，不是任意C++代码
 * - 简化流程 = 自动化原始API，不是重新发明
 * - vir功能委托给CHTL JS，不重新实现
 */