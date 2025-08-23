#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <functional>
#include <memory>
#include <variant>

namespace CHTL {
namespace CJMOD {

// 前向声明
class Syntax;
class CHTLJSFunction;

// 参数值类型
using ArgValue = std::variant<std::string, int, double, bool>;

/**
 * @brief 扫描策略枚举
 */
enum class ScanStrategy {
    SLIDING_WINDOW,     // 滑动窗口双指针扫描（正确实现）
    FRONT_EXTRACT      // 前置代码截取（不发送给编译器）
};

/**
 * @brief 策略模式枚举
 */
enum class Policy {
    NORMAL,     // 常规
    COLLECT,    // 收集
    SKIP        // 跳过
};

/**
 * @brief Arg类 - 核心参数处理类
 * 
 * 设计理念：
 * - 支持operator()重载进行值处理
 * - 智能bind函数处理不同类型
 * - 占位符自动计数和匹配
 * - 类型安全的值转换
 */
class Arg {
public:
    /**
     * @brief 构造函数
     */
    Arg(const std::string& name, bool isPlaceholder = false);
    
    /**
     * @brief bind方法 - 类型安全的函数绑定
     */
    template<typename T>
    void bind(const std::function<std::string(T)>& func) {
        static_assert(std::is_convertible_v<T, std::string> || 
                     std::is_arithmetic_v<T> || 
                     std::is_same_v<T, std::string>, 
                     "Unsupported bind function parameter type");
        
        // 类型擦除存储，内部精妙的类型处理
        bindFunction_ = [func](const std::string& value) -> std::string {
            return processTypedValue<T>(func, value);
        };
        
        hasBind_ = true;
    }
    
    /**
     * @brief operator()重载 - 直接调用绑定函数
     * 用法：keyword.args[0](peekKeyword(-1));
     */
    std::string operator()(const std::string& value);
    
    /**
     * @brief match方法 - 值匹配和处理
     */
    void match(const std::string& value);
    
    /**
     * @brief transform方法 - JS代码转换
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
};

/**
 * @brief Syntax类 - 核心语法分析和扫描类
 * 
 * 设计理念：
 * - 集成语法分析、扫描策略、代码生成
 * - 实现正确的滑动窗口双指针扫描
 * - 实现前置代码截取（不发送给编译器）
 * - 提供简洁的API接口
 */
class Syntax {
public:
    /**
     * @brief 构造函数
     */
    Syntax();
    
    /**
     * @brief 析构函数
     */
    ~Syntax();
    
    // ======== 语法分析功能 ========
    
    /**
     * @brief 分析CHTL JS语法模式
     * @param pattern 语法模式，如 "printMylove({ url: $, mode: $ })"
     * @param ignoreChars 忽略字符，如 ",:{};()"
     * @return 解析成功返回true
     */
    bool analyzePattern(const std::string& pattern, const std::string& ignoreChars = "");
    
    // ======== 参数操作 ========
    
    /**
     * @brief 索引访问参数 - syntax.args[0]
     */
    Arg& operator[](size_t index);
    const Arg& operator[](size_t index) const;
    
    /**
     * @brief bind方法 - 参数绑定
     * 支持占位符重复绑定：syntax.bind("$", func)
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
     * @brief match方法 - 参数匹配
     */
    void match(const std::string& name, const std::string& value);
    
    /**
     * @brief 获取参数数量
     */
    size_t length() const { return args_.size(); }
    
    // ======== 扫描功能 ========
    
    /**
     * @brief 设置扫描策略
     */
    void setScanStrategy(ScanStrategy strategy) { scanStrategy_ = strategy; }
    
    /**
     * @brief 扫描并处理源代码
     * @param source 源代码
     * @return 处理后的代码
     */
    std::string scanAndProcess(const std::string& source);
    
    /**
     * @brief 扫描关键字
     * @param keyword 关键字
     * @param handler 处理函数
     */
    void scanKeyword(const std::string& keyword, std::function<void()> handler);
    
    /**
     * @brief 向前/向后查看关键字
     * @param offset 偏移量（负数向后，正数向前）
     * @return 找到的关键字
     */
    std::string peekKeyword(int offset) const;
    
    /**
     * @brief 策略模式开始
     * @param trigger 触发字符
     * @param policy 策略类型
     */
    void policyChangeBegin(const std::string& trigger, Policy policy);
    
    /**
     * @brief 策略模式结束
     * @param trigger 结束字符
     * @param policy 返回策略
     * @return 收集到的内容
     */
    std::string policyChangeEnd(const std::string& trigger, Policy policy);
    
    // ======== 代码生成 ========
    
    /**
     * @brief 生成最终代码
     * @return 生成的JS代码
     */
    std::string generateCode() const;
    
    /**
     * @brief result方法 - 组合所有转换后的参数
     */
    std::string result() const;

private:
    // 参数列表
    std::vector<Arg> args_;
    std::unordered_map<std::string, size_t> nameToIndex_;
    size_t placeholderCounter_;
    
    // 扫描相关
    ScanStrategy scanStrategy_;
    std::vector<std::string> currentTokens_;
    size_t currentPosition_;
    
    // 策略状态
    Policy currentPolicy_;
    std::string collectBuffer_;
    struct PolicyState {
        std::string trigger;
        Policy policy;
        std::string buffer;
    };
    std::vector<PolicyState> policyStack_;
    
    // 关键字处理
    std::unordered_map<std::string, std::function<void()>> keywordHandlers_;
    
    // 内部方法
    void addArg(const std::string& name, bool isPlaceholder = false);
    Arg* findArg(const std::string& name);
    
    // 扫描算法实现
    std::string slidingWindowScan(const std::string& source);
    std::string frontExtractScan(const std::string& source);
    
    // 滑动窗口双指针扫描的辅助方法
    bool detectCHTLJSSyntaxAtPos(const std::string& source, size_t pos) const;
    size_t findSyntaxEnd(const std::string& source, size_t start) const;
    std::string processCHTLJSSyntax(const std::string& syntax);
    bool isSyntaxComplete(const std::string& syntax) const;
    void resetScannerState();
    
    // 前置截取扫描的辅助方法
    size_t findNextCHTLJSSyntax(const std::string& source, size_t start) const;
    
    // 策略处理
    void processCollectPolicy(const std::string& content);
    void processSkipPolicy();
    bool matchesTrigger(const std::string& content, const std::string& trigger) const;
};

/**
 * @brief CHTLJSFunction类 - CHTL JS函数封装
 * 
 * 设计理念：
 * - 管理CHTL JS函数的定义和调用
 * - 支持虚对象绑定
 * - 与Syntax类协同工作
 * - 提供函数级别的配置和优化
 */
class CHTLJSFunction {
public:
    /**
     * @brief 构造函数
     */
    CHTLJSFunction(const std::string& name);
    
    /**
     * @brief 添加参数定义
     */
    void addParam(const std::string& name, const std::string& defaultValue = "");
    
    /**
     * @brief 设置函数体模板
     */
    void setBodyTemplate(const std::string& bodyTemplate);
    
    /**
     * @brief 启用虚对象支持
     */
    void enableVirSupport(bool enable = true) { virSupported_ = enable; }
    
    /**
     * @brief 是否支持虚对象
     */
    bool isVirSupported() const { return virSupported_; }
    
    /**
     * @brief 绑定到Syntax对象
     * @param syntax 语法对象
     * @return 绑定成功返回true
     */
    bool bindToSyntax(Syntax& syntax);
    
    /**
     * @brief 生成JavaScript代码
     */
    std::string generateJS() const;
    
    /**
     * @brief 生成虚对象访问代码
     * @param objectName 虚对象名称
     * @param keyAccess 键访问路径
     */
    std::string generateVirAccess(const std::string& objectName, const std::string& keyAccess) const;
    
    /**
     * @brief 获取函数名
     */
    const std::string& getName() const { return name_; }
    
    /**
     * @brief 获取支持的键名列表
     */
    const std::vector<std::string>& getKeyNames() const { return keyNames_; }

private:
    std::string name_;
    std::vector<std::pair<std::string, std::string>> params_; // name, defaultValue
    std::vector<std::string> keyNames_;  // 函数支持的键名
    std::string bodyTemplate_;
    bool virSupported_;
    
    // 与Syntax的集成
    Syntax* boundSyntax_;
    
    // 生成辅助方法
    std::string generateGlobalFunctionName(const std::string& keyName) const;
    std::string generateVirFunctionMapping() const;
};

// ======== 全局函数 ========

/**
 * @brief 语法分析函数 - 兼容原API
 * @param pattern 语法模式
 * @param ignoreChars 忽略字符
 * @return Syntax对象指针
 */
std::unique_ptr<Syntax> syntaxAnalys(const std::string& pattern, 
                                    const std::string& ignoreChars = "");

/**
 * @brief 代码生成函数 - 兼容原API
 * @param syntax 语法对象
 * @return 生成的JS代码
 */
std::string generateCode(const Syntax& syntax);

/**
 * @brief 创建CHTL JS函数
 * @param functionName 函数名
 * @param keyNames 键名列表
 * @return CHTLJSFunction对象指针
 */
std::unique_ptr<CHTLJSFunction> createCHTLJSFunction(const std::string& functionName, 
                                                   const std::vector<std::string>& keyNames);

/**
 * @brief 设置全局扫描策略
 * @param strategy 扫描策略
 */
void setGlobalScanStrategy(ScanStrategy strategy);

/**
 * @brief 获取全局扫描策略
 * @return 当前全局扫描策略
 */
ScanStrategy getGlobalScanStrategy();

} // namespace CJMOD
} // namespace CHTL