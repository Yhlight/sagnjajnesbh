#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <functional>
#include <memory>
#include <variant>
#include <type_traits>

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
 * @brief Arg类 - 按照您的原始设计哲学
 * 外表简单使用，内部设计精妙，支持模板和类型安全
 */
class Arg {
public:
    /**
     * @brief 构造函数
     */
    Arg(const std::string& name, bool isPlaceholder = false);
    
    /**
     * @brief 模板化bind方法 - 支持不同类型的参数处理
     * 按照您的设计：keyword.arg[0]([](int num) { return std::string(num); });
     */
    template<typename T>
    void bind(const std::function<std::string(T)>& func) {
        static_assert(!std::is_void_v<T>, "Function parameter cannot be void");
        
        // 存储类型擦除的绑定函数
        bindFunction_ = [func](const std::string& value) -> std::string {
            if constexpr (std::is_same_v<T, std::string>) {
                return func(value);
            } else if constexpr (std::is_same_v<T, int>) {
                try {
                    return func(std::stoi(value));
                } catch (...) {
                    return func(0); // 默认值
                }
            } else if constexpr (std::is_same_v<T, double>) {
                try {
                    return func(std::stod(value));
                } catch (...) {
                    return func(0.0); // 默认值
                }
            } else {
                // 其他类型处理
                return func(T{});
            }
        };
    }
    
    /**
     * @brief 简化的bind方法 - 直接返回字符串
     */
    void bind(const std::function<std::string(const std::string&)>& func);
    
    /**
     * @brief 无参数bind - 按照您的设计：keyword.arg[1] = []() { return "**"; }
     */
    void bind(const std::function<std::string()>& func);
    
    /**
     * @brief 函数调用操作符重载 - 按照您的设计
     * keyword.arg[0](peekKeyword(-1)); 
     */
    template<typename T>
    std::string operator()(T&& value) {
        if (bindFunction_) {
            if constexpr (std::is_same_v<std::decay_t<T>, std::string>) {
                return bindFunction_(std::forward<T>(value));
            } else {
                return bindFunction_(std::to_string(std::forward<T>(value)));
            }
        }
        return "";
    }
    
    /**
     * @brief 无参数调用 - 按照您的设计
     */
    std::string operator()() {
        if (noArgFunction_) {
            return noArgFunction_();
        }
        return "";
    }
    
    /**
     * @brief match方法 - 匹配并处理值
     */
    void match(const std::string& value);
    
    /**
     * @brief transform方法 - 转换为JS代码
     */
    const std::string& transform(const std::string& jsCode);
    
    /**
     * @brief 获取参数名称
     */
    const std::string& getName() const { return name_; }
    
    /**
     * @brief 是否为占位符
     */
    bool isPlaceholder() const { return isPlaceholder_; }
    
    /**
     * @brief 获取转换后的值
     */
    const std::string& getValue() const { return transformedValue_; }
    
    /**
     * @brief 检查是否已绑定
     */
    bool isBound() const { return static_cast<bool>(bindFunction_) || static_cast<bool>(noArgFunction_); }

private:
    std::string name_;                                           // 参数名称
    bool isPlaceholder_;                                        // 是否为占位符($)
    std::string value_;                                         // 原始值
    std::string transformedValue_;                              // 转换后的JS代码
    std::function<std::string(const std::string&)> bindFunction_;   // 有参数的绑定函数
    std::function<std::string()> noArgFunction_;               // 无参数的绑定函数
    int current_ = 0;                                          // 当前索引（按照您的原始设计）
};

/**
 * @brief Args集合 - 按照您的原始设计哲学
 * 外表简单，内部处理复杂的占位符计数和绑定逻辑
 */
class Args {
public:
    Args() = default;
    
    /**
     * @brief 添加参数
     */
    void addArg(const std::string& name, bool isPlaceholder = false);
    
    /**
     * @brief 索引访问 - keyword.args[0]
     */
    Arg& operator[](size_t index);
    const Arg& operator[](size_t index) const;
    
    /**
     * @brief 名称访问便捷方法
     */
    Arg* find(const std::string& name);
    
    /**
     * @brief 模板化bind方法 - 按您的设计：keyword.args.bind("url", [](std::string str) { return str; });
     */
    template<typename T>
    void bind(const std::string& name, const std::function<std::string(T)>& func) {
        if (name == "$") {
            // 占位符绑定：自动计数，绑定到下一个未绑定的占位符
            for (auto& arg : args_) {
                if (arg.isPlaceholder() && !arg.isBound()) {
                    arg.bind<T>(func);
                    break;
                }
            }
        } else {
            // 普通参数绑定
            Arg* arg = find(name);
            if (arg) {
                arg->bind<T>(func);
            }
        }
    }
    
    /**
     * @brief 简化bind方法
     */
    void bind(const std::string& name, const std::function<std::string(const std::string&)>& func);
    
    /**
     * @brief match方法 - keyword.args.match("url", peekKeyword(1));
     */
    void match(const std::string& name, const std::string& value);
    
    /**
     * @brief result方法 - 组合所有转换后的JS代码
     */
    std::string result() const;
    
    /**
     * @brief 获取参数数量 - 按照您的设计：keyword.args.length()
     */
    size_t length() const { return args_.size(); }

private:
    std::vector<Arg> args_;
    std::unordered_map<std::string, size_t> nameToIndex_;
    int placeholderCount_ = 0;  // 占位符计数，用于重复绑定
};

/**
 * @brief Syntax类 - 按照您的原始设计
 * 直接暴露args成员，简洁优雅
 */
class Syntax {
public:
    /**
     * @brief 构造函数
     */
    Syntax() = default;
    
    /**
     * @brief 直接访问：keyword.args - 按照您的设计
     */
    Args args;
    
    /**
     * @brief result成员 - 按照您的设计
     */
    std::string result;

private:
    // 内部实现可以更复杂，但使用接口保持简单
};

/**
 * @brief CJMODScanner类 - 按照您的原始设计
 * 与统一扫描器集成的智能扫描接口
 */
class CJMODScanner {
public:
    CJMODScanner();
    
    /**
     * @brief scanKeyword - 核心扫描接口
     * scanner.scanKeyword(keyword.args[1], []() { ... });
     */
    void scanKeyword(const Arg& arg, const std::function<void()>& handler);
    void scanKeyword(const std::string& keyword, const std::function<void()>& handler);
    
    /**
     * @brief peekKeyword - 查看偏移关键字
     * 按照您的设计：peekKeyword(-1), peekKeyword(1)
     */
    std::string peekKeyword(int offset) const;
    
    /**
     * @brief policy系列 - 策略改变
     * 按照您的设计：scanner.policyChangeBegin(":", Policy::COLLECT);
     */
    void policyChangeBegin(const std::string& trigger, Policy policy);
    std::string policyChangeEnd(const std::string& trigger, Policy policy);
    
    /**
     * @brief 辅助函数 - 按照您的设计
     */
    bool isObject(const std::string& content) const;
    bool isFunction(const std::string& content) const;
    
    /**
     * @brief 设置当前token流 - 与统一扫描器集成
     */
    void setTokens(const std::vector<std::string>& tokens, size_t position = 0);

private:
    std::unordered_map<std::string, std::function<void()>> keywordHandlers_;
    std::vector<std::string> currentTokens_;
    size_t currentPosition_;
    Policy currentPolicy_;
    std::string collectBuffer_;
    
    // 与统一扫描器的集成接口
    void processCollectPolicy(const std::string& content);
    void processSkipPolicy();
};

/**
 * @brief 全局函数 - 按照您的原始设计
 * 简单使用，复杂实现
 */

/**
 * @brief syntaxAnalys - 语法分析核心函数
 * 按照您的设计：const keyword = syntaxAnalys("printMylove({ url: $, mode: $ });", ",:{};()");
 */
std::unique_ptr<Syntax> syntaxAnalys(const std::string& pattern, const std::string& ignoreChars = "");

/**
 * @brief generateCode - 生成JS代码
 * 按照您的设计：generateCode(keyword);
 */
std::string generateCode(const Syntax& syntax);

/**
 * @brief 获取全局CJMODScanner实例
 * 与统一扫描器系统集成
 */
CJMODScanner& getCJMODScanner();

} // namespace CJMOD
} // namespace CHTL