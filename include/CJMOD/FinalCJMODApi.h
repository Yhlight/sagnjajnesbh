#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <functional>
#include <memory>

namespace CHTL {
namespace CJMOD {

// 前向声明
class Arg;
class Syntax;
class CHTLJSFunction;

/**
 * @brief 扫描策略枚举
 */
enum class Policy {
    NORMAL,     // 常规
    COLLECT,    // 收集 (原文档中的COLLENT)
    SKIP        // 跳过
};

/**
 * @brief 代码片段结构 - 统一扫描器返回的结果
 */
struct CodeFragment {
    std::string originalCode;       // 原始代码片段
    std::vector<std::string> tokens; // 分词结果
    std::string triggerKeyword;     // 触发的关键字
    size_t keywordPosition;         // 关键字在片段中的位置
    
    CodeFragment() : keywordPosition(0) {}
};

// ============================================================================
// 核心类 1: Arg - 参数处理核心
// ============================================================================

/**
 * @brief Arg类 - 参数处理的核心
 * 
 * 基于您的原始API设计：
 * - operator() 重载支持函数绑定和值传递
 * - bind() 绑定值处理函数
 * - match() 匹配参数值
 * - transform() 转换为JS代码
 */
class Arg {
public:
    /**
     * @brief 构造函数
     */
    Arg(const std::string& name = "", bool isPlaceholder = false);
    
    /**
     * @brief operator() 重载 - 您的核心设计
     * 用法：keyword.arg[0](peekKeyword(-1)); // 把内容传递给参数
     */
    template<typename T>
    std::string operator()(T value) {
        if constexpr (std::is_invocable_v<T>) {
            // 如果T是函数，绑定该函数
            bindFunction_ = [value](const std::string& input) -> std::string {
                if constexpr (std::is_same_v<std::invoke_result_t<T>, std::string>) {
                    return value();
                } else {
                    return std::to_string(value());
                }
            };
            hasBind_ = true;
            return "";
        } else {
            // 如果T是值，直接处理
            if (hasBind_) {
                processedValue_ = bindFunction_(std::to_string(value));
            } else {
                processedValue_ = std::to_string(value);
            }
            hasValue_ = true;
            return processedValue_;
        }
    }
    
    /**
     * @brief bind方法 - 绑定值处理函数
     * 用法：keyword.args[1].bind([]() { return x; });
     */
    template<typename T>
    void bind(const std::function<std::string(T)>& func) {
        bindFunction_ = [func](const std::string& rawValue) -> std::string {
            if constexpr (std::is_same_v<T, std::string>) {
                return func(rawValue);
            } else if constexpr (std::is_same_v<T, int>) {
                try {
                    return func(std::stoi(rawValue));
                } catch (...) {
                    return func(0);
                }
            } else if constexpr (std::is_same_v<T, double>) {
                try {
                    return func(std::stod(rawValue));
                } catch (...) {
                    return func(0.0);
                }
            }
            return rawValue;
        };
        hasBind_ = true;
    }
    
    /**
     * @brief bind方法 - 无参数版本
     * 用法：keyword.args[1].bind([]() { return x; });
     */
    void bind(const std::function<std::string()>& func);
    
    /**
     * @brief match方法 - 匹配参数值
     * 用法：keyword.args[0].match(peekKeyword(-1));
     */
    void match(const std::string& value);
    
    /**
     * @brief transform方法 - 转换为JS代码
     * 用法：keyword.args[0].transform("URL url = new URL()");
     */
    void transform(const std::string& jsTemplate);
    
    /**
     * @brief 获取参数名称
     */
    const std::string& getName() const { return name_; }
    
    /**
     * @brief 是否为占位符
     */
    bool isPlaceholder() const { return isPlaceholder_; }
    
    /**
     * @brief 是否已绑定函数
     */
    bool hasBind() const { return hasBind_; }
    
    /**
     * @brief 是否已有值
     */
    bool hasValue() const { return hasValue_; }
    
    /**
     * @brief 获取处理后的值
     */
    const std::string& getValue() const { return processedValue_; }
    
    /**
     * @brief 获取JS代码
     */
    const std::string& getJSCode() const { return jsCode_; }
    
    /**
     * @brief 输出到流 - 支持 std::cout << keyword.args[0]
     */
    friend std::ostream& operator<<(std::ostream& os, const Arg& arg) {
        os << arg.name_;
        return os;
    }

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

// ============================================================================
// 核心类 2: Syntax - 语法分析结果容器和主控制器
// ============================================================================

/**
 * @brief Syntax类 - 语法分析结果的容器和主控制器
 * 
 * 基于您的原始API设计：
 * - args向量：keyword.args[0], keyword.args[1]...
 * - bind方法：keyword.args.bind("url", func)
 * - match方法：keyword.args.match("url", value)
 * - transform方法：keyword.args.transform("url", template)
 * - result方法：keyword.args.result()
 * - length方法：keyword.args.length()
 */
class Syntax {
public:
    Syntax() = default;
    
    /**
     * @brief args向量 - 您的核心设计
     * 用法：keyword.args[0], keyword.args[1]...
     * 支持：std::cout << keyword.args[0] << std::endl;
     */
    std::vector<Arg> args;
    
    /**
     * @brief bind方法 - 按名称绑定处理函数
     * 用法：keyword.args.bind("url", [](const std::string& str) { return str; });
     * 占位符支持：bind函数会自动计数占位符，重复写的语句会自动绑定到下一个$
     */
    template<typename T>
    void bind(const std::string& name, const std::function<std::string(T)>& func) {
        if (name == "$") {
            // 占位符绑定：自动找到下一个未绑定的占位符
            for (auto& arg : args) {
                if (arg.isPlaceholder() && !arg.hasBind()) {
                    arg.bind(func);
                    placeholderCounter_++;
                    break;
                }
            }
        } else {
            // 命名参数绑定
            Arg* targetArg = findArg(name);
            if (targetArg) {
                targetArg->bind(func);
            }
        }
    }
    
    /**
     * @brief match方法 - 匹配相应的arg，传递值并调用函数
     * 用法：keyword.args.match("url", peekKeyword(-1));
     * 占位符支持：对于占位符，重复绑定和bind的行为一样
     */
    void match(const std::string& name, const std::string& value);
    
    /**
     * @brief transform方法 - 将arg转变为JS代码
     * 用法：keyword.args.transform("url", "URL url = new URL()");
     */
    void transform(const std::string& name, const std::string& jsTemplate);
    
    /**
     * @brief result方法 - 将转变为JS代码后的arg组合起来
     * 用法：keyword.args.result();
     */
    std::string result();
    
    /**
     * @brief length方法 - 获取参数数量
     * 用法：keyword.args.length()
     */
    size_t length() const { return args.size(); }
    
    /**
     * @brief 添加参数（内部使用）
     */
    void addArg(const std::string& name, bool isPlaceholder = false);
    
    /**
     * @brief scanKeyword支持 - 设置触发关键字
     */
    void setTriggerKeyword(const std::string& keyword) { triggerKeyword_ = keyword; }
    
    /**
     * @brief 获取触发关键字
     */
    const std::string& getTriggerKeyword() const { return triggerKeyword_; }

private:
    std::unordered_map<std::string, size_t> nameToIndex_;
    size_t placeholderCounter_ = 0;
    std::string triggerKeyword_;
    
    Arg* findArg(const std::string& name);
};

// ============================================================================
// 核心类 3: CHTLJSFunction - 完整的CJMOD处理器
// ============================================================================

/**
 * @brief CHTLJSFunction类 - 完整的CJMOD处理器
 * 
 * 整合所有功能：
 * - 语法分析：syntaxAnalys
 * - 扫描器：scanKeyword, peekKeyword, policy系列
 * - 辅助函数：isObject, isFunction, slice
 * - 代码生成：generateCode
 */
class CHTLJSFunction {
public:
    CHTLJSFunction();
    
    /**
     * @brief syntaxAnalys - 分析CHTL JS语法，并分解成多个arg
     * 用法：
     * const keyword = syntaxAnalys("arg ** arg2");
     * std::string str = ",:{};()";
     * const keyword = syntaxAnalys("printMylove({ url: $ });", str);
     */
    std::unique_ptr<Syntax> syntaxAnalys(const std::string& pattern, 
                                        const std::string& ignoreChars = "");
    
    /**
     * @brief scanKeyword - 扫描器暴露的接口，扩展CHTL JS的核心
     * 用法：scanner.scanKeyword(keyword.args[1], []() { ... });
     */
    void scanKeyword(const Arg& arg, std::function<void()> handler);
    void scanKeyword(const std::string& keyword, std::function<void()> handler);
    
    /**
     * @brief peekKeyword - 获取前后关键字的内容
     * 用法：peekKeyword(-1), peekKeyword(1)
     */
    std::string peekKeyword(int offset) const;
    
    /**
     * @brief policy系列 - 改变扫描政策
     * 用法：
     * scanner.policyChangeBegin(":", Policy::COLLECT);
     * const std::string& str = scanner.policyChangeEnd("}", Policy::NORMAL);
     */
    void policyChangeBegin(const std::string& trigger, Policy policy);
    std::string policyChangeEnd(const std::string& trigger, Policy policy);
    
    /**
     * @brief generateCode - 生成JS代码
     * 用法：generateCode(keyword);
     */
    std::string generateCode(const Syntax& syntax);
    
    /**
     * @brief isObject - 辅助函数，判断是不是对象
     */
    bool isObject(const std::string& content);
    
    /**
     * @brief isFunction - 辅助函数，判断是不是函数
     */
    bool isFunction(const std::string& content);
    
    /**
     * @brief slice - 辅助函数，切割字符串（处理函数与对象）
     */
    std::string slice(const std::string& content, size_t start, size_t end = std::string::npos);
    
    /**
     * @brief 设置代码片段上下文
     */
    void setCodeFragment(const CodeFragment& fragment);
    
    /**
     * @brief 处理源代码的完整流程
     */
    std::string processSourceCode(const std::string& sourceCode, const std::string& pattern);

private:
    CodeFragment currentFragment_;
    bool hasFragment_;
    Policy currentPolicy_;
    std::unordered_map<std::string, std::function<void()>> keywordHandlers_;
    
    // Policy状态栈
    struct PolicyState {
        std::string trigger;
        Policy policy;
        std::string buffer;
    };
    std::vector<PolicyState> policyStack_;
    
    // 内部辅助方法
    std::vector<std::string> tokenize(const std::string& pattern, const std::string& ignoreChars);
    bool isIgnoredChar(char c, const std::string& ignoreChars);
    void processCollectPolicy(const std::string& content);
    void processSkipPolicy();
    bool matchesTrigger(const std::string& content, const std::string& trigger) const;
};

// ============================================================================
// 全局函数 - 便捷接口
// ============================================================================

/**
 * @brief syntaxAnalys - 全局便捷函数
 * 用法：auto keyword = syntaxAnalys("arg ** arg2");
 */
std::unique_ptr<Syntax> syntaxAnalys(const std::string& pattern, 
                                    const std::string& ignoreChars = "");

/**
 * @brief generateCode - 全局便捷函数
 * 用法：generateCode(keyword);
 */
std::string generateCode(const Syntax& syntax);

/**
 * @brief 创建CJMOD处理器
 */
std::unique_ptr<CHTLJSFunction> createCJMODProcessor();

} // namespace CJMOD
} // namespace CHTL

/**
 * @brief 🎯 最终三类核心架构：
 * 
 * 【Arg类】- 参数处理核心
 * - operator() 重载：keyword.arg[0](peekKeyword(-1))
 * - bind方法：绑定值处理函数
 * - match方法：匹配参数值
 * - transform方法：转换为JS代码
 * - 支持输出：std::cout << keyword.args[0]
 * 
 * 【Syntax类】- 语法容器和主控制器
 * - args向量：keyword.args[0], keyword.args[1]...
 * - bind方法：keyword.args.bind("url", func)
 * - match方法：keyword.args.match("url", value)
 * - transform方法：keyword.args.transform("url", template)
 * - result方法：keyword.args.result()
 * - length方法：keyword.args.length()
 * - 占位符自动计数支持
 * 
 * 【CHTLJSFunction类】- 完整处理器
 * - syntaxAnalys：语法分析入口
 * - scanKeyword：扫描器接口
 * - peekKeyword：获取前后关键字
 * - policy系列：扫描策略控制
 * - generateCode：代码生成
 * - isObject/isFunction/slice：辅助函数
 * 
 * 🔧 完整流程：
 * syntaxAnalys(拆分成多个arg) 
 * -> bind(绑定获取值的函数) 
 * -> transform(最终的JS代码) 
 * -> scanKeyword(什么关键字作为统一扫描器的搜索) 
 * -> match(捕获参数值，要拿到代码片段才能使用peekKeyword) 
 * -> result(组合所有参数的最终JS代码) 
 * -> generateCode(生成器输出标准JS)
 */