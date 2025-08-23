#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <functional>
#include <memory>

namespace CHTL {
namespace CJMOD {

// 前向声明
class Args;
class Syntax;
class CJMODScanner;

/**
 * @brief Policy策略枚举
 */
enum class Policy {
    NORMAL,     // 常规模式
    COLLECT,    // 收集模式
    SKIP        // 跳过模式
};

/**
 * @brief Arg类 - 保持原有的实用设计
 */
class Arg {
public:
    Arg(const std::string& name = "", bool isPlaceholder = false);
    
    std::string operator()(const std::string& peekedValue);
    
    template<typename T>
    void bind(const std::function<std::string(T)>& valueProcessor) {
        valueProcessor_ = [valueProcessor](const std::string& rawValue) -> std::string {
            if constexpr (std::is_same_v<T, std::string>) {
                return valueProcessor(rawValue);
            } else if constexpr (std::is_same_v<T, int>) {
                try { return valueProcessor(std::stoi(rawValue)); }
                catch (...) { return valueProcessor(0); }
            } else if constexpr (std::is_same_v<T, double>) {
                try { return valueProcessor(std::stod(rawValue)); }
                catch (...) { return valueProcessor(0.0); }
            }
            return rawValue;
        };
        hasBind_ = true;
    }
    
    void transform(const std::string& jsTemplate);
    void match(const std::string& rawValue);
    
    const std::string& getJSCode() const { return jsCode_; }
    const std::string& getValue() const { return processedValue_; }
    const std::string& getName() const { return name_; }
    bool isPlaceholder() const { return isPlaceholder_; }
    bool hasBind() const { return hasBind_; }
    bool hasTransform() const { return !jsTemplate_.empty(); }

private:
    std::string name_;
    bool isPlaceholder_;
    bool hasBind_;
    std::string rawValue_;
    std::string processedValue_;
    std::string jsTemplate_;
    std::string jsCode_;
    std::function<std::string(const std::string&)> valueProcessor_;
    
    void applyJSTransform();
};

/**
 * @brief Args类 - 增强链式调用支持
 */
class Args {
public:
    Args() = default;
    
    void addArg(const std::string& name, bool isPlaceholder = false);
    
    Arg& operator[](size_t index);
    const Arg& operator[](size_t index) const;
    
    /**
     * @brief 增强的链式bind - 返回自身引用支持链式调用
     */
    template<typename T>
    Args& bind(const std::string& name, const std::function<std::string(T)>& processor) {
        if (name == "$") {
            for (auto& arg : args_) {
                if (arg.isPlaceholder() && !arg.hasBind()) {
                    arg.bind(processor);
                    placeholderCounter_++;
                    break;
                }
            }
        } else {
            Arg* arg = findArg(name);
            if (arg) {
                arg->bind(processor);
            }
        }
        return *this;  // 支持链式调用
    }
    
    /**
     * @brief 增强的链式transform - 返回自身引用支持链式调用
     */
    Args& transform(const std::string& name, const std::string& jsTemplate);
    
    /**
     * @brief 链式match - 返回自身引用支持链式调用
     */
    Args& match(const std::string& name, const std::string& value);
    
    /**
     * @brief 批量操作 - 链式绑定多个占位符
     */
    template<typename... Ts>
    Args& bindSequence(const std::function<std::string(Ts)>&... processors) {
        auto bindNext = [this](auto&& processor) {
            using T = typename std::decay_t<decltype(processor)>::argument_type;
            this->bind<T>("$", processor);
        };
        (bindNext(processors), ...);
        return *this;
    }
    
    /**
     * @brief 批量转换 - 链式设置多个JS模板
     */
    Args& transformSequence(const std::vector<std::string>& jsTemplates);
    
    /**
     * @brief 批量匹配 - 链式匹配多个值
     */
    Args& matchSequence(const std::vector<std::string>& values);
    
    std::string result() const;
    size_t length() const { return args_.size(); }

private:
    std::vector<Arg> args_;
    std::unordered_map<std::string, size_t> nameToIndex_;
    size_t placeholderCounter_ = 0;
    
    Arg* findArg(const std::string& name);
};

/**
 * @brief Syntax类 - 大幅增强链式调用能力
 * 重点：流畅的API体验，高效的链式操作
 */
class Syntax {
public:
    Syntax() = default;
    
    /**
     * @brief 直接访问args - 保持原生API风格
     */
    Args args;
    
    /**
     * @brief 增强的链式bind - 直接在Syntax层面操作
     */
    template<typename T>
    Syntax& bind(const std::function<std::string(T)>& processor) {
        args.bind<T>("$", processor);
        return *this;
    }
    
    /**
     * @brief 命名参数链式bind
     */
    template<typename T>
    Syntax& bind(const std::string& name, const std::function<std::string(T)>& processor) {
        args.bind<T>(name, processor);
        return *this;
    }
    
    /**
     * @brief 链式transform
     */
    Syntax& transform(const std::string& jsTemplate) {
        args.transform("$", jsTemplate);
        return *this;
    }
    
    /**
     * @brief 命名参数链式transform
     */
    Syntax& transform(const std::string& name, const std::string& jsTemplate) {
        args.transform(name, jsTemplate);
        return *this;
    }
    
    /**
     * @brief 链式match
     */
    Syntax& match(const std::string& value) {
        args.match("$", value);
        return *this;
    }
    
    /**
     * @brief 命名参数链式match
     */
    Syntax& match(const std::string& name, const std::string& value) {
        args.match(name, value);
        return *this;
    }
    
    /**
     * @brief 高效的批量操作 - 一次性绑定处理器和JS模板
     */
    template<typename T>
    Syntax& bindAndTransform(const std::function<std::string(T)>& processor, 
                            const std::string& jsTemplate) {
        args.bind<T>("$", processor);
        args.transform("$", jsTemplate);
        return *this;
    }
    
    /**
     * @brief 命名参数的bindAndTransform
     */
    template<typename T>
    Syntax& bindAndTransform(const std::string& name,
                            const std::function<std::string(T)>& processor, 
                            const std::string& jsTemplate) {
        args.bind<T>(name, processor);
        args.transform(name, jsTemplate);
        return *this;
    }
    
    /**
     * @brief 快速配置 - 一步完成bind、transform、match
     */
    template<typename T>
    Syntax& configure(const std::function<std::string(T)>& processor,
                     const std::string& jsTemplate,
                     const std::string& value) {
        args.bind<T>("$", processor);
        args.transform("$", jsTemplate);
        args.match("$", value);
        return *this;
    }
    
    /**
     * @brief 批量快速配置
     */
    template<typename... Ts>
    Syntax& configureAll(const std::tuple<std::function<std::string(Ts)>, std::string, std::string>&... configs) {
        auto configureOne = [this](auto&& config) {
            this->configure(std::get<0>(config), std::get<1>(config), std::get<2>(config));
        };
        (configureOne(configs), ...);
        return *this;
    }
    
    /**
     * @brief 流式操作 - 支持条件链式调用
     */
    template<typename Condition>
    Syntax& when(Condition&& condition, std::function<Syntax&(Syntax&)> action) {
        if (condition) {
            return action(*this);
        }
        return *this;
    }
    
    /**
     * @brief 扫描器集成的链式调用
     */
    Syntax& withScanner(CJMODScanner& scanner, 
                       const std::vector<int>& peekOffsets = {});
    
    /**
     * @brief 便捷的result访问
     */
    std::string result() const { return args.result(); }
    
    /**
     * @brief 调试支持 - 打印当前状态
     */
    Syntax& debug(const std::string& message = "") const;
};

/**
 * @brief CJMODScanner类 - 保持原有设计
 */
class CJMODScanner {
public:
    CJMODScanner();
    
    void scanKeyword(const Arg& arg, std::function<void()> handler);
    void scanKeyword(const std::string& keyword, std::function<void()> handler);
    std::string peekKeyword(int offset) const;
    void policyChangeBegin(const std::string& trigger, Policy policy);
    std::string policyChangeEnd(const std::string& trigger, Policy policy);
    bool isObject(const std::string& content) const;
    bool isFunction(const std::string& content) const;
    void setTokens(const std::vector<std::string>& tokens, size_t position = 0);

private:
    std::unordered_map<std::string, std::function<void()>> keywordHandlers_;
    std::vector<std::string> currentTokens_;
    size_t currentPosition_;
    Policy currentPolicy_;
    std::string collectBuffer_;
    
    void processCollectPolicy(const std::string& content);
    void processSkipPolicy();
};

/**
 * @brief CHTLJSFunction类 - 增强链式调用支持
 */
class CHTLJSFunction {
public:
    CHTLJSFunction(const std::string& functionName, 
                   const std::vector<std::string>& paramNames = {});
    
    /**
     * @brief 增强的链式bindKeyProcessor
     */
    template<typename T>
    CHTLJSFunction& bindKeyProcessor(const std::string& keyName, 
                                    const std::function<std::string(T)>& processor) {
        if (!syntax_) createSyntax();
        syntax_->args.bind(keyName, processor);
        return *this;  // 支持链式调用
    }
    
    /**
     * @brief 链式transformKey
     */
    CHTLJSFunction& transformKey(const std::string& keyName, const std::string& jsTemplate);
    
    /**
     * @brief 链式bindAndTransform - 一步完成绑定和转换
     */
    template<typename T>
    CHTLJSFunction& bindAndTransform(const std::string& keyName,
                                    const std::function<std::string(T)>& processor,
                                    const std::string& jsTemplate) {
        if (!syntax_) createSyntax();
        syntax_->bindAndTransform(keyName, processor, jsTemplate);
        return *this;
    }
    
    /**
     * @brief 批量配置 - 高效设置多个参数
     */
    template<typename... Ts>
    CHTLJSFunction& configureBatch(const std::tuple<std::string, std::function<std::string(Ts)>, std::string>&... configs) {
        if (!syntax_) createSyntax();
        auto configureOne = [this](auto&& config) {
            this->bindAndTransform(std::get<0>(config), std::get<1>(config), std::get<2>(config));
        };
        (configureOne(configs), ...);
        return *this;
    }
    
    /**
     * @brief 链式matchKeyword
     */
    CHTLJSFunction& matchKeyword(const std::string& keyword, const std::string& value);
    
    std::unique_ptr<Syntax> createSyntax();
    void scanKeyword(CJMODScanner& scanner);
    std::string generateCode();
    
    const std::string& getFunctionName() const { return functionName_; }
    Syntax* getSyntax() const { return syntax_.get(); }

private:
    std::string functionName_;
    std::vector<std::string> paramNames_;
    std::unique_ptr<Syntax> syntax_;
    
    std::string generateSyntaxPattern();
};

/**
 * @brief 全局函数 - 增强的链式调用支持
 */

/**
 * @brief 增强的syntaxAnalys - 返回可链式调用的Syntax
 */
std::unique_ptr<Syntax> syntaxAnalys(const std::string& pattern, 
                                    const std::string& ignoreChars = ",:{};()");

/**
 * @brief 流式创建 - 支持链式初始化
 */
std::unique_ptr<Syntax> createSyntax(const std::string& pattern);

/**
 * @brief 流式CHTLJSFunction创建
 */
std::unique_ptr<CHTLJSFunction> createCHTLJSFunction(
    const std::string& functionName,
    const std::vector<std::string>& paramNames = {});

std::string generateCode(const Syntax& syntax);
CJMODScanner& getCJMODScanner();

/**
 * @brief 链式操作辅助宏 - 简化常见操作
 */
#define CJMOD_BIND_TRANSFORM(type, processor, template) \
    bindAndTransform<type>(processor, template)

#define CJMOD_CONFIGURE(type, processor, template, value) \
    configure<type>(processor, template, value)

} // namespace CJMOD
} // namespace CHTL