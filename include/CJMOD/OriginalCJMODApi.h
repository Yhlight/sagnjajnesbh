#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <functional>
#include <memory>

namespace CHTL {
namespace CJMOD {

// 前向声明
class CJMODScanner;

/**
 * @brief 扫描策略枚举
 */
enum class Policy {
    NORMAL,     // 常规
    COLLECT,    // 收集  
    SKIP        // 跳过
};

/**
 * @brief Arg类 - 按照您的原始设计
 * 核心特性：
 * 1. 支持operator()重载
 * 2. 智能bind函数处理
 * 3. 占位符自动计数
 */
class Arg {
public:
    /**
     * @brief 构造函数
     */
    Arg(const std::string& name, bool isPlaceholder = false);
    
    /**
     * @brief bind方法 - 按照您的原始设计
     * Template<typename T>
     * void bind(const std::function<std::string(T)>& func)
     */
    template<typename T>
    void bind(const std::function<std::string(T)>& func) {
        // 类型擦除存储，内部精妙处理
        bindFunction_ = [func](const std::string& value) -> std::string {
            if constexpr (std::is_same_v<T, std::string>) {
                return func(value);
            } else if constexpr (std::is_arithmetic_v<T>) {
                try {
                    if constexpr (std::is_integral_v<T>) {
                        T num = static_cast<T>(std::stoll(value));
                        return func(num);
                    } else {
                        T num = static_cast<T>(std::stod(value));
                        return func(num);
                    }
                } catch (...) {
                    return func(T{});
                }
            }
            return value;
        };
        hasBind_ = true;
    }
    
    /**
     * @brief 重载operator() - 按照您的原始设计
     * keyword.arg[0](peekKeyword(-1));
     */
    std::string operator()(const std::string& value);
    
    /**
     * @brief match方法 - 值匹配和处理
     */
    void match(const std::string& value);
    
    /**
     * @brief transform方法 - JS代码转换
     */
    const std::string& transform(const std::string& jsCode);
    
    /**
     * @brief 获取参数信息
     */
    const std::string& getName() const { return name_; }
    bool isPlaceholder() const { return isPlaceholder_; }
    const std::string& getValue() const { return transformedValue_; }
    bool hasBind() const { return hasBind_; }

private:
    std::string name_;
    bool isPlaceholder_;
    bool hasBind_;
    std::string rawValue_;
    std::string transformedValue_;
    std::function<std::string(const std::string&)> bindFunction_;
};

/**
 * @brief Args类 - 按照您的原始设计
 * 核心特性：
 * 1. 支持索引访问 args[0]
 * 2. 智能占位符重复绑定
 * 3. 简洁的bind/match接口
 */
class Args {
public:
    Args() = default;
    
    /**
     * @brief 添加参数（内部使用）
     */
    void addArg(const std::string& name, bool isPlaceholder = false);
    
    /**
     * @brief 索引访问 - keyword.args[0]
     */
    Arg& operator[](size_t index);
    const Arg& operator[](size_t index) const;
    
    /**
     * @brief bind方法 - 按照您的设计逻辑
     * 支持占位符重复绑定：args.bind("$", func)会自动绑定下一个$
     */
    template<typename T>
    void bind(const std::string& name, const std::function<std::string(T)>& func) {
        if (name == "$") {
            // 占位符绑定：智能找到下一个未绑定的占位符
            for (auto& arg : args_) {
                if (arg.isPlaceholder() && !arg.hasBind()) {
                    arg.bind(func);
                    placeholderCounter_++;
                    break;
                }
            }
        } else {
            // 命名参数绑定
            Arg* arg = findArg(name);
            if (arg) {
                arg->bind(func);
            }
        }
    }
    
    /**
     * @brief match方法 - 智能值匹配
     * 支持占位符重复匹配：args.match("$", value)会自动匹配下一个$
     */
    void match(const std::string& name, const std::string& value);
    
    /**
     * @brief transform方法 - 批量转换
     */
    void transform(const std::string& name, const std::string& jsTemplate);
    
    /**
     * @brief result方法 - 组合所有转换后的JS代码
     * 按照您的设计：keyword.result()
     */
    std::string result() const;
    
    /**
     * @brief 获取参数数量
     */
    size_t length() const { return args_.size(); }

private:
    std::vector<Arg> args_;
    std::unordered_map<std::string, size_t> nameToIndex_;
    size_t placeholderCounter_ = 0;  // 占位符智能计数
    
    // 内部精妙实现
    Arg* findArg(const std::string& name);
};

/**
 * @brief Syntax类 - 按照您的原始设计
 * 简洁的语法分析结果容器
 */
class Syntax {
public:
    Syntax() = default;
    
    /**
     * @brief 便捷访问：keyword.args
     */
    Args args;
    
    /**
     * @brief result方法 - 便捷访问
     * keyword.result() 等同于 keyword.args.result()
     */
    std::string result() const { return args.result(); }

private:
    std::string pattern_;  // 原始模式（调试用）
};

/**
 * @brief CJMODScanner类 - 按照您的原始设计
 * 核心特性：
 * 1. scanKeyword接口
 * 2. peekKeyword相对位置访问  
 * 3. policy策略模式
 * 4. 辅助判断函数
 */
class CJMODScanner {
public:
    CJMODScanner();
    
    /**
     * @brief scanKeyword - 核心扫描接口
     * scanner.scanKeyword(keyword.args[0], []() { ... });
     */
    void scanKeyword(const Arg& arg, std::function<void()> handler);
    void scanKeyword(const std::string& keyword, std::function<void()> handler);
    
    /**
     * @brief peekKeyword - 智能关键字查看
     * 按照您的设计：peekKeyword(-1), peekKeyword(1)
     * 支持相对位置访问，自动边界处理
     */
    std::string peekKeyword(int offset) const;
    
    /**
     * @brief policy系列 - 策略模式实现
     * 按照您的设计：
     * scanner.policyChangeBegin(":", Policy::COLLECT);
     * const std::string& str = scanner.policyChangeEnd("}", Policy::NORMAL);
     */
    void policyChangeBegin(const std::string& trigger, Policy policy);
    std::string policyChangeEnd(const std::string& trigger, Policy policy);
    
    /**
     * @brief 辅助函数 - 按照您的设计
     */
    bool isObject(const std::string& content);
    bool isFunction(const std::string& content);
    
    /**
     * @brief 设置扫描上下文（内部使用）
     */
    void setTokens(const std::vector<std::string>& tokens);
    void setPosition(size_t position);

private:
    // 精妙的内部状态管理
    std::unordered_map<std::string, std::function<void()>> keywordHandlers_;
    std::vector<std::string> currentTokens_;
    size_t currentPosition_;
    Policy currentPolicy_;
    std::string collectBuffer_;
    
    // 策略状态栈（精妙的嵌套策略支持）
    struct PolicyState {
        std::string trigger;
        Policy policy;
        std::string buffer;
    };
    std::vector<PolicyState> policyStack_;
    
    // 内部精妙实现
    void processCollectPolicy(const std::string& content);
    void processSkipPolicy();
    bool matchesTrigger(const std::string& content, const std::string& trigger) const;
};

/**
 * @brief 全局函数 - 按照您的原始设计
 * 简洁的接口，精妙的实现
 */

/**
 * @brief syntaxAnalys - 核心语法分析函数
 * 按照您的设计：
 * const keyword = syntaxAnalys("printMylove({ url: $, mode: $ });", ",:{};()");
 * 
 * 精妙特性：
 * 1. 智能分词算法
 * 2. 占位符自动识别
 * 3. 忽略字符的智能处理
 * 4. 注释过滤
 */
std::unique_ptr<Syntax> syntaxAnalys(const std::string& pattern, 
                                    const std::string& ignoreChars = "");

/**
 * @brief generateCode - 智能代码生成
 * 按照您的设计：generateCode(keyword);
 * 
 * 精妙特性：
 * 1. 智能模板替换
 * 2. 格式化和优化
 * 3. 错误恢复
 */
std::string generateCode(const Syntax& syntax);

} // namespace CJMOD
} // namespace CHTL