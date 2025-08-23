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
 * @brief Policy策略枚举 - 扫描器策略模式
 */
enum class Policy {
    NORMAL,     // 常规模式
    COLLECT,    // 收集模式
    SKIP        // 跳过模式
};

/**
 * @brief Arg类 - 真正实用的参数处理
 * 核心特性：
 * 1. operator()重载 - 用于peekKeyword结果处理
 * 2. bind方法 - 专门的值处理器绑定
 * 3. transform方法 - 专门的JS代码转换
 * 4. 不统一功能，各司其职
 */
class Arg {
public:
    Arg(const std::string& name = "", bool isPlaceholder = false);
    
    /**
     * @brief operator()重载 - 处理peekKeyword结果
     * 使用场景：keyword.args[0](scanner.peekKeyword(-1))
     * 这不是通用处理器，而是专门处理扫描器返回的关键字
     */
    std::string operator()(const std::string& peekedValue);
    
    /**
     * @brief bind方法 - 绑定值处理器
     * 专门处理从源码中扫描到的原始值
     * 不是通用转换，而是针对特定类型的值预处理
     */
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
    
    /**
     * @brief transform方法 - 专门的JS代码转换
     * 这是核心功能：将处理后的值转换为实际的JS代码
     * 使用场景：arg.transform("loadImage(\"${value}\")")
     */
    void transform(const std::string& jsTemplate);
    
    /**
     * @brief match方法 - 匹配原始值
     * 从扫描器获得的原始值，经过bind处理后存储
     */
    void match(const std::string& rawValue);
    
    /**
     * @brief 获取最终的JS代码
     */
    const std::string& getJSCode() const { return jsCode_; }
    
    /**
     * @brief 获取处理后的值
     */
    const std::string& getValue() const { return processedValue_; }
    
    /**
     * @brief 访问器
     */
    const std::string& getName() const { return name_; }
    bool isPlaceholder() const { return isPlaceholder_; }
    bool hasBind() const { return hasBind_; }
    bool hasTransform() const { return !jsTemplate_.empty(); }

private:
    std::string name_;
    bool isPlaceholder_;
    bool hasBind_;
    
    std::string rawValue_;           // 原始值
    std::string processedValue_;     // 经过bind处理的值
    std::string jsTemplate_;         // JS模板
    std::string jsCode_;             // 最终生成的JS代码
    
    std::function<std::string(const std::string&)> valueProcessor_;
    
    // 应用JS模板转换
    void applyJSTransform();
};

/**
 * @brief Args类 - 实用的参数集合
 * 核心特性：
 * 1. 智能占位符处理
 * 2. 批量JS转换
 * 3. 与扫描器深度集成
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
     * @brief bind方法 - 占位符智能绑定
     * 使用场景：
     * keyword.args.bind("$", [](const string& url) { return "\"" + url + "\""; });
     * keyword.args.bind("$", [](int width) { return to_string(width); });
     */
    template<typename T>
    void bind(const std::string& name, const std::function<std::string(T)>& processor) {
        if (name == "$") {
            // 占位符绑定：自动找到下一个未绑定的占位符
            for (auto& arg : args_) {
                if (arg.isPlaceholder() && !arg.hasBind()) {
                    arg.bind(processor);
                    placeholderCounter_++;
                    break;
                }
            }
        } else {
            // 命名参数绑定
            Arg* arg = findArg(name);
            if (arg) {
                arg->bind(processor);
            }
        }
    }
    
    /**
     * @brief transform方法 - 批量JS转换
     * 使用场景：
     * keyword.args.transform("url", "loadImage(\"${value}\")");
     * keyword.args.transform("$", "window.innerWidth || ${value}");
     */
    void transform(const std::string& name, const std::string& jsTemplate);
    
    /**
     * @brief match方法 - 智能值匹配
     */
    void match(const std::string& name, const std::string& value);
    
    /**
     * @brief result方法 - 生成最终的JS代码组合
     */
    std::string result() const;
    
    /**
     * @brief length方法
     */
    size_t length() const { return args_.size(); }

private:
    std::vector<Arg> args_;
    std::unordered_map<std::string, size_t> nameToIndex_;
    size_t placeholderCounter_ = 0;
    
    Arg* findArg(const std::string& name);
};

/**
 * @brief Syntax类 - 简洁的接口封装
 * 保持原生API的简洁性：keyword.args, keyword.result()
 */
class Syntax {
public:
    Syntax() = default;
    
    /**
     * @brief 直接访问args - 原生API风格
     */
    Args args;
    
    /**
     * @brief 便捷的result访问
     */
    std::string result() const { return args.result(); }
};

/**
 * @brief CJMODScanner类 - 与统一扫描器集成
 * 这是实用性的关键：与现有扫描系统协作
 */
class CJMODScanner {
public:
    CJMODScanner();
    
    /**
     * @brief scanKeyword - 核心扫描接口
     * 使用场景：
     * scanner.scanKeyword(keyword.args[0], []() { 
     *     // 扫描到关键字时的处理逻辑
     * });
     */
    void scanKeyword(const Arg& arg, std::function<void()> handler);
    void scanKeyword(const std::string& keyword, std::function<void()> handler);
    
    /**
     * @brief peekKeyword - 相对位置关键字查看
     * 这是原生API的核心实用功能
     * 使用场景：keyword.args[0](scanner.peekKeyword(-1))
     */
    std::string peekKeyword(int offset) const;
    
    /**
     * @brief policy策略模式 - 扫描器行为控制
     * 使用场景：
     * scanner.policyChangeBegin(":", Policy::COLLECT);
     * const string& collected = scanner.policyChangeEnd("}", Policy::NORMAL);
     */
    void policyChangeBegin(const std::string& trigger, Policy policy);
    std::string policyChangeEnd(const std::string& trigger, Policy policy);
    
    /**
     * @brief 实用的判断函数
     */
    bool isObject(const std::string& content) const;
    bool isFunction(const std::string& content) const;
    
    /**
     * @brief 设置当前扫描上下文
     */
    void setTokens(const std::vector<std::string>& tokens, size_t position = 0);

private:
    std::unordered_map<std::string, std::function<void()>> keywordHandlers_;
    std::vector<std::string> currentTokens_;
    size_t currentPosition_;
    Policy currentPolicy_;
    std::string collectBuffer_;
    
    // 策略处理
    void processCollectPolicy(const std::string& content);
    void processSkipPolicy();
};

/**
 * @brief CHTLJSFunction类 - 实用的简化流程
 * 重点：不是万能工具，而是专门为CHTL JS设计的实用工具
 */
class CHTLJSFunction {
public:
    /**
     * @brief 构造函数
     * @param functionName JS函数名
     * @param paramNames 参数名列表（可选）
     */
    CHTLJSFunction(const std::string& functionName, 
                   const std::vector<std::string>& paramNames = {});
    
    /**
     * @brief bindKeyProcessor - 绑定键处理器
     * 专门处理CHTL JS函数的参数值
     */
    template<typename T>
    void bindKeyProcessor(const std::string& keyName, 
                         const std::function<std::string(T)>& processor) {
        if (!syntax_) createSyntax();
        syntax_->args.bind(keyName, processor);
    }
    
    /**
     * @brief transformKey - 设置JS转换模板
     * 专门为每个参数设置JS代码模板
     */
    void transformKey(const std::string& keyName, const std::string& jsTemplate);
    
    /**
     * @brief 标准CJMOD流程
     */
    std::unique_ptr<Syntax> createSyntax();
    void scanKeyword(CJMODScanner& scanner);
    void matchKeyword(const std::string& keyword, const std::string& value);
    std::string generateCode();
    
    /**
     * @brief 获取内部组件
     */
    const std::string& getFunctionName() const { return functionName_; }
    Syntax* getSyntax() const { return syntax_.get(); }

private:
    std::string functionName_;
    std::vector<std::string> paramNames_;
    std::unique_ptr<Syntax> syntax_;
    
    std::string generateSyntaxPattern();
};

/**
 * @brief 全局函数 - 保持原生API风格
 */

/**
 * @brief syntaxAnalys - 核心语法分析
 * 完全按照原生API设计
 */
std::unique_ptr<Syntax> syntaxAnalys(const std::string& pattern, 
                                    const std::string& ignoreChars = ",:{};()");

/**
 * @brief generateCode - JS代码生成
 */
std::string generateCode(const Syntax& syntax);

/**
 * @brief 获取全局扫描器实例
 */
CJMODScanner& getCJMODScanner();

/**
 * @brief 创建CHTLJSFunction的便捷函数
 */
std::unique_ptr<CHTLJSFunction> createCHTLJSFunction(
    const std::string& functionName,
    const std::vector<std::string>& paramNames = {});

} // namespace CJMOD
} // namespace CHTL