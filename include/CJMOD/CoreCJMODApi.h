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
class CJMODScanner;

/**
 * @brief 扫描策略枚举
 */
enum class Policy {
    NORMAL,     // 常规模式
    COLLECT,    // 收集模式 - 收集两个关键字之间的内容
    SKIP        // 跳过模式
};

/**
 * @brief 双指针扫描位置信息
 */
struct ScanPosition {
    size_t frontPointer;    // 前指针位置
    size_t backPointer;     // 后指针位置
    std::string content;    // 扫描到的内容
    bool hasKeyword;        // 是否检测到关键字
    
    ScanPosition() : frontPointer(0), backPointer(0), hasKeyword(false) {}
};

/**
 * @brief 前置截取结果
 */
struct ExtractResult {
    std::string extracted;      // 截取的内容
    std::string remaining;      // 剩余内容
    bool success;              // 是否成功
    
    ExtractResult() : success(false) {}
};

// ============================================================================
// 核心类 1: Arg - 参数处理核心
// ============================================================================

/**
 * @brief Arg类 - 参数处理的核心
 * 
 * 核心功能：
 * 1. operator() 重载 - 接收函数，处理peekKeyword结果
 * 2. bind() - 绑定值处理函数
 * 3. match() - 匹配并处理参数值
 * 4. transform() - 转换为JS代码
 */
class Arg {
public:
    /**
     * @brief 构造函数
     */
    Arg(const std::string& name = "", bool isPlaceholder = false);
    
    /**
     * @brief operator() 重载 - 您的核心设计
     * 用法：keyword.arg[0]([](int num) { return std::to_string(num); });
     * 或者：keyword.arg[0](peekKeyword(-1)); // 传递值给参数
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
            return processedValue_;
        }
    }
    
    /**
     * @brief bind方法 - 绑定值处理函数
     * 用法：keyword.args.bind("url", [](std::string str) { return str; });
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
     * @brief match方法 - 匹配参数值
     * 用法：keyword.args.match("url", peekKeyword(-1));
     */
    void match(const std::string& value);
    
    /**
     * @brief transform方法 - 转换为JS代码
     * 用法：keyword.args.transform("url", "new URL('${value}')");
     */
    void transform(const std::string& jsTemplate);
    
    /**
     * @brief 获取最终JS代码
     */
    const std::string& getJSCode() const { return jsCode_; }
    
    /**
     * @brief 获取处理后的值
     */
    const std::string& getValue() const { return processedValue_; }
    
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

private:
    std::string name_;
    bool isPlaceholder_;
    bool hasBind_;
    std::string rawValue_;
    std::string processedValue_;
    std::string jsTemplate_;
    std::string jsCode_;
    std::function<std::string(const std::string&)> bindFunction_;
    
    void applyTransform();
};

// ============================================================================
// 核心类 2: Syntax - 语法分析结果容器
// ============================================================================

/**
 * @brief Syntax类 - 语法分析结果的容器
 * 
 * 核心功能：
 * 1. 维护args向量 - 所有分析出的参数
 * 2. bind() - 按名称绑定参数函数
 * 3. result() - 组合所有参数的JS代码
 */
class Syntax {
public:
    Syntax() = default;
    
    /**
     * @brief args向量 - 您的核心设计
     * 用法：keyword.args[0], keyword.args[1]...
     */
    std::vector<Arg> args;
    
    /**
     * @brief bind方法 - 按名称绑定
     * 用法：keyword.args.bind("url", [](std::string str) { return str; });
     * 支持占位符自动计数：多次bind("$", func)会绑定到不同的$
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
     * @brief match方法 - 按名称匹配值
     * 用法：keyword.args.match("url", peekKeyword(-1));
     * 支持占位符自动计数：多次match("$", value)会匹配到不同的$
     */
    void match(const std::string& name, const std::string& value);
    
    /**
     * @brief transform方法 - 按名称转换
     * 用法：keyword.args.transform("url", "new URL('${value}')");
     */
    void transform(const std::string& name, const std::string& jsTemplate);
    
    /**
     * @brief result方法 - 组合所有JS代码
     * 用法：std::string result = keyword.result();
     */
    std::string result() const;
    
    /**
     * @brief 获取参数数量
     */
    size_t length() const { return args.size(); }
    
    /**
     * @brief 添加参数（内部使用）
     */
    void addArg(const std::string& name, bool isPlaceholder = false);

private:
    std::unordered_map<std::string, size_t> nameToIndex_;
    size_t placeholderCounter_ = 0;
    
    Arg* findArg(const std::string& name);
};

// ============================================================================
// 核心类 3: CHTLJSFunction - 高级封装
// ============================================================================

/**
 * @brief CHTLJSFunction类 - 高级函数封装
 * 
 * 提供更高级的接口，封装常见的使用模式
 */
class CHTLJSFunction {
public:
    CHTLJSFunction(const std::string& functionName, const std::vector<std::string>& paramNames = {});
    
    /**
     * @brief 添加参数
     */
    CHTLJSFunction& addParam(const std::string& name, bool isPlaceholder = false);
    
    /**
     * @brief 绑定参数处理函数
     */
    template<typename T>
    CHTLJSFunction& bindParam(const std::string& name, const std::function<std::string(T)>& func) {
        if (!syntax_) createSyntax();
        syntax_->bind(name, func);
        return *this;
    }
    
    /**
     * @brief 匹配参数值
     */
    CHTLJSFunction& matchParam(const std::string& name, const std::string& value);
    
    /**
     * @brief 转换参数为JS
     */
    CHTLJSFunction& transformParam(const std::string& name, const std::string& jsTemplate);
    
    /**
     * @brief 生成JS代码
     */
    std::string generateCode();
    
    /**
     * @brief 获取内部Syntax对象
     */
    Syntax* getSyntax() const { return syntax_.get(); }

private:
    std::string functionName_;
    std::vector<std::string> paramNames_;
    std::unique_ptr<Syntax> syntax_;
    
    void createSyntax();
};

// ============================================================================
// 双指针扫描器 - 您的核心架构
// ============================================================================

/**
 * @brief 双指针扫描器
 * 
 * 核心功能：
 * 1. 双指针同步扫描
 * 2. 关键字检测和通知
 * 3. 前置截取机制
 * 4. Policy策略支持
 */
class DualPointerScanner {
public:
    DualPointerScanner();
    
    /**
     * @brief 设置扫描内容
     */
    void setContent(const std::vector<std::string>& tokens);
    
    /**
     * @brief 扫描初始范围 - 确定不存在语法片段
     */
    bool scanInitialRange(size_t rangeSize = 5);
    
    /**
     * @brief 双指针同步扫描
     * 前指针和后指针从0开始，同步向前移动
     * 前指针遇到关键字时通知后指针收集
     */
    ScanPosition synchronousScan(const std::string& keyword);
    
    /**
     * @brief 前置截取机制
     * 处理如 "arg ** arg" 的情况，截取前面的arg避免发送给编译器
     */
    ExtractResult frontExtract(const std::string& keyword, int beforeOffset = -1);
    
    /**
     * @brief peekKeyword - 相对位置查看
     * 用法：peekKeyword(-1), peekKeyword(1)
     */
    std::string peekKeyword(int offset) const;
    
    /**
     * @brief 获取当前扫描位置
     */
    const ScanPosition& getCurrentPosition() const { return currentPosition_; }

private:
    std::vector<std::string> tokens_;
    ScanPosition currentPosition_;
    
    bool isValidPosition(size_t position) const;
    void movePointersForward();
};

/**
 * @brief CJMODScanner - 完整的扫描器实现
 * 
 * 整合双指针扫描和Policy策略
 */
class CJMODScanner {
public:
    CJMODScanner();
    
    /**
     * @brief scanKeyword - 核心扫描接口
     * 用法：scanner.scanKeyword(keyword.args[1], []() { ... });
     */
    void scanKeyword(const Arg& arg, std::function<void()> handler);
    void scanKeyword(const std::string& keyword, std::function<void()> handler);
    
    /**
     * @brief peekKeyword - 智能关键字查看
     * 用法：peekKeyword(-1), peekKeyword(1)
     */
    std::string peekKeyword(int offset) const;
    
    /**
     * @brief policy系列 - 策略模式
     * 用法：
     * scanner.policyChangeBegin(":", Policy::COLLECT);
     * const std::string& str = scanner.policyChangeEnd("}", Policy::NORMAL);
     */
    void policyChangeBegin(const std::string& trigger, Policy policy);
    std::string policyChangeEnd(const std::string& trigger, Policy policy);
    
    /**
     * @brief 辅助函数
     */
    bool isObject(const std::string& content);
    bool isFunction(const std::string& content);
    
    /**
     * @brief 设置扫描上下文
     */
    void setTokens(const std::vector<std::string>& tokens);
    void setPosition(size_t position);
    
    /**
     * @brief 获取双指针扫描器
     */
    DualPointerScanner& getDualPointerScanner() { return dualScanner_; }

private:
    DualPointerScanner dualScanner_;
    std::unordered_map<std::string, std::function<void()>> keywordHandlers_;
    std::vector<std::string> currentTokens_;
    size_t currentPosition_;
    Policy currentPolicy_;
    
    // Policy状态栈
    struct PolicyState {
        std::string trigger;
        Policy policy;
        std::string buffer;
    };
    std::vector<PolicyState> policyStack_;
    
    void processCollectPolicy(const std::string& content);
    void processSkipPolicy();
    bool matchesTrigger(const std::string& content, const std::string& trigger) const;
};

// ============================================================================
// 核心工厂函数
// ============================================================================

/**
 * @brief syntaxAnalys - 核心语法分析函数
 * 用法：
 * const keyword = syntaxAnalys("printMylove({ url: $, mode: $ });", ",:{};()");
 * 
 * 功能：
 * 1. 智能分词算法
 * 2. 占位符自动识别
 * 3. 忽略字符处理
 * 4. 注释过滤
 */
std::unique_ptr<Syntax> syntaxAnalys(const std::string& pattern, 
                                    const std::string& ignoreChars = "");

/**
 * @brief generateCode - 代码生成函数
 * 用法：generateCode(keyword);
 * 
 * 功能：
 * 1. 模板替换
 * 2. 代码格式化
 * 3. 错误恢复
 */
std::string generateCode(const Syntax& syntax);

/**
 * @brief 创建CHTLJSFunction
 */
std::unique_ptr<CHTLJSFunction> createCHTLJSFunction(const std::string& functionName,
                                                    const std::vector<std::string>& paramNames = {});

} // namespace CJMOD
} // namespace CHTL

/**
 * @brief 核心设计要点：
 * 
 * 1. 【原始API忠实实现】
 *    - Arg::operator() 支持函数绑定和值传递
 *    - Syntax::args 向量访问：keyword.args[0]
 *    - 占位符自动计数：多次bind("$", func)自动绑定不同的$
 * 
 * 2. 【双指针扫描架构】
 *    - 前指针和后指针从0开始
 *    - 先扫描初始范围确定无语法片段
 *    - 双指针同步移动，前指针遇到关键字通知后指针收集
 * 
 * 3. 【前置截取机制】
 *    - 处理 "arg ** arg" 类型的语法
 *    - 统一扫描器扫描到**时启动CJMOD收集
 *    - 截取前面的arg避免发送给编译器
 * 
 * 4. 【三类核心架构】
 *    - Arg: 参数处理核心，支持bind/match/transform
 *    - Syntax: 语法容器，维护args向量和整体操作
 *    - CHTLJSFunction: 高级封装，简化常见使用模式
 * 
 * 5. 【正确的完整流程】
 *    syntaxAnalys(拆分成多个arg) 
 *    -> bind(绑定获取值的函数) 
 *    -> transform(最终的JS代码) 
 *    -> scanKeyword(什么关键字作为统一扫描器的搜索) 
 *    -> match(捕获参数值，这个要拿到代码片段才行，否则无法使用peekKeyword) 
 *    -> result(组合所有参数的最终JS代码) 
 *    -> generateCode(把代码交给生成器，生成器将代码输出为标准js)
 * 
 * 6. 【关键理解】
 *    - scanKeyword 设置什么关键字让统一扫描器搜索
 *    - match 必须在拿到代码片段后才能使用peekKeyword获取参数值
 *    - transform 预先定义JS代码模板，match时填充实际值
 *    - Policy策略：NORMAL/COLLECT/SKIP
 *    - 辅助函数：isObject/isFunction
 */