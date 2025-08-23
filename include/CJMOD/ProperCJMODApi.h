#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <functional>
#include <memory>
#include <regex>

namespace CHTL {
namespace CJMOD {

// 前向声明
class CJMODScanner;
class Args;

/**
 * @brief 扫描策略枚举
 */
enum class Policy {
    NORMAL,     // 常规
    COLLECT,    // 收集
    SKIP        // 跳过
};

/**
 * @brief Arg类 - 按照您的原始设计逻辑
 * 
 * 设计理念：
 * - 表面简单：提供直观的bind()和match()接口
 * - 内在精妙：支持占位符自动计数、类型推导、转换链
 */
class Arg {
public:
    /**
     * @brief 构造函数
     */
    Arg(const std::string& name, bool isPlaceholder = false);
    
    /**
     * @brief bind方法 - 核心设计
     * 
     * 设计逻辑：
     * 1. 支持任意类型的值处理函数
     * 2. 自动类型推导和转换
     * 3. 占位符自动计数（重复绑定不同$）
     */
    template<typename T>
    void bind(const std::function<std::string(T)>& func) {
        static_assert(std::is_convertible_v<T, std::string> || 
                     std::is_arithmetic_v<T> || 
                     std::is_same_v<T, std::string>, 
                     "Unsupported bind function parameter type");
        
        // 将类型擦除的函数存储，内部实现精妙的类型处理
        bindFunction_ = [func](const std::string& value) -> std::string {
            return processTypedValue<T>(func, value);
        };
        
        hasBind_ = true;
    }
    
    /**
     * @brief 重载操作符 - 直接调用绑定函数
     * 按照您的设计：keyword.arg[0](peekKeyword(-1));
     */
    template<typename T>
    std::string operator()(T value) {
        if (hasBind_ && bindFunction_) {
            return bindFunction_(convertToString(value));
        }
        return convertToString(value);
    }
    
    /**
     * @brief match方法 - 值匹配和处理
     * 按照您的设计：keyword.args.match("url", peekKeyword(1));
     */
    void match(const std::string& value);
    
    /**
     * @brief transform方法 - JS代码转换
     * 按照您的设计：keyword.args.transform("url", "URL url = new URL()");
     */
    const std::string& transform(const std::string& jsTemplate);
    
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
    
    // 精妙的内部实现：类型处理
    template<typename T>
    static std::string processTypedValue(const std::function<std::string(T)>& func, 
                                       const std::string& value) {
        if constexpr (std::is_same_v<T, std::string>) {
            return func(value);
        } else if constexpr (std::is_arithmetic_v<T>) {
            try {
                if constexpr (std::is_integral_v<T>) {
                    return func(static_cast<T>(std::stoll(value)));
                } else {
                    return func(static_cast<T>(std::stod(value)));
                }
            } catch (...) {
                return func(T{});  // 默认值
            }
        } else {
            return func(T{});
        }
    }
    
    template<typename T>
    static std::string convertToString(T value) {
        if constexpr (std::is_same_v<T, std::string>) {
            return value;
        } else if constexpr (std::is_arithmetic_v<T>) {
            return std::to_string(value);
        } else {
            return std::string(value);
        }
    }
};

/**
 * @brief Args类 - 精妙的参数集合管理
 * 
 * 设计理念：
 * - 表面简单：args[0], args.bind(), args.match()
 * - 内在精妙：占位符智能计数、自动搜索、批量处理
 */
class Args {
public:
    Args() = default;
    
    /**
     * @brief 索引访问 - keyword.args[0]
     */
    Arg& operator[](size_t index);
    const Arg& operator[](size_t index) const;
    
    /**
     * @brief bind方法 - 按照您的设计逻辑
     * 
     * 精妙之处：
     * 1. 自动处理占位符重复绑定
     * 2. 智能参数搜索和匹配
     * 3. 支持lambda的自动类型推导
     */
    template<typename T>
    void bind(const std::string& name, const std::function<std::string(T)>& func) {
        if (name == "$") {
            // 占位符绑定：智能找到下一个未绑定的占位符
            bindNextPlaceholder(func);
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
     * keyword.args.match("url", peekKeyword(1));
     */
    void match(const std::string& name, const std::string& value);
    
    /**
     * @brief transform方法 - 批量转换
     */
    void transform(const std::string& name, const std::string& jsTemplate);
    
    /**
     * @brief result方法 - 智能结果组合
     * 按照您的设计：keyword.result();
     */
    std::string result() const;
    
    /**
     * @brief 获取参数数量
     */
    size_t length() const { return args_.size(); }
    
    /**
     * @brief 内部使用：添加参数
     */
    void addArg(const std::string& name, bool isPlaceholder = false);
    
    /**
     * @brief 获取所有参数名（调试用）
     */
    std::vector<std::string> getArgNames() const;

private:
    std::vector<Arg> args_;
    std::unordered_map<std::string, size_t> nameToIndex_;
    size_t placeholderCounter_ = 0;  // 占位符智能计数
    
    // 精妙的内部实现
    Arg* findArg(const std::string& name);
    
    template<typename T>
    void bindNextPlaceholder(const std::function<std::string(T)>& func) {
        for (auto& arg : args_) {
            if (arg.isPlaceholder() && !arg.hasBind()) {
                arg.bind(func);
                placeholderCounter_++;
                break;
            }
        }
    }
};

/**
 * @brief Syntax类 - 按照您的原始设计
 * 
 * 设计理念：简洁的容器，但内部维护复杂的参数关系
 */
class Syntax {
public:
    Syntax() = default;
    
    /**
     * @brief 便捷访问：keyword.args
     */
    Args args;
    
    /**
     * @brief 获取语法分析的原始模式
     */
    const std::string& getPattern() const { return pattern_; }
    
    /**
     * @brief 设置模式（内部使用）
     */
    void setPattern(const std::string& pattern) { pattern_ = pattern; }

private:
    std::string pattern_;
};

/**
 * @brief CJMODScanner类 - 精妙的扫描器设计
 * 
 * 设计理念：
 * - 表面简单：scanKeyword(), peekKeyword()
 * - 内在精妙：状态机、策略模式、智能缓存
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
     * peekKeyword(-1), peekKeyword(1)
     * 
     * 精妙之处：支持相对位置访问，自动边界处理
     */
    std::string peekKeyword(int offset) const;
    
    /**
     * @brief policy系列 - 策略模式实现
     * 
     * 设计逻辑：
     * scanner.policyChangeBegin(":", Policy::COLLECT);
     * const std::string& str = scanner.policyChangeEnd("}", Policy::NORMAL);
     */
    void policyChangeBegin(const std::string& trigger, Policy policy);
    std::string policyChangeEnd(const std::string& trigger, Policy policy);
    
    /**
     * @brief 辅助函数 - 智能类型判断
     */
    bool isObject(const std::string& content);
    bool isFunction(const std::string& content);
    
    /**
     * @brief 设置当前扫描上下文（内部使用）
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
    
    // 智能状态机
    struct ScannerState {
        std::string trigger;
        Policy policy;
        std::string buffer;
    };
    std::vector<ScannerState> stateStack_;
    
    // 内部精妙实现
    void processCollectPolicy(const std::string& content);
    void processSkipPolicy();
    bool matchesTrigger(const std::string& content, const std::string& trigger) const;
};

/**
 * @brief 全局函数 - 按照您的原始设计
 * 
 * 简洁的接口，精妙的实现
 */

/**
 * @brief syntaxAnalys - 核心语法分析函数
 * 
 * 按照您的设计：
 * const keyword = syntaxAnalys("printMylove({ url: $, mode: $ });", ",:{};()");
 * 
 * 精妙之处：
 * 1. 智能分词算法
 * 2. 占位符自动识别
 * 3. 忽略字符的智能处理
 * 4. 注释过滤
 */
std::unique_ptr<Syntax> syntaxAnalys(const std::string& pattern, 
                                    const std::string& ignoreChars = "");

/**
 * @brief generateCode - 智能代码生成
 * 
 * 按照您的设计：generateCode(keyword);
 * 
 * 精妙之处：
 * 1. 智能模板替换
 * 2. 类型感知的代码生成
 * 3. 格式化和优化
 */
std::string generateCode(const Syntax& syntax);

/**
 * @brief 扩展接口 - 支持复杂场景
 */

/**
 * @brief 高级语法分析 - 支持嵌套和复杂结构
 */
std::unique_ptr<Syntax> advancedSyntaxAnalys(const std::string& pattern,
                                           const std::string& ignoreChars = "",
                                           bool supportNesting = true);

/**
 * @brief 批量代码生成 - 支持多个语法的组合生成
 */
std::string batchGenerateCode(const std::vector<Syntax>& syntaxList);

} // namespace CJMOD
} // namespace CHTL