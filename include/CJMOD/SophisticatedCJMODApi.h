#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <functional>
#include <memory>
#include <variant>
#include <type_traits>
#include <regex>

namespace CHTL {
namespace CJMOD {

class CJMODScanner;
class Args;
class Syntax;

// 扫描策略枚举
enum class Policy {
    NORMAL,   // 常规扫描
    COLLECT,  // 收集模式
    SKIP      // 跳过模式
};

// 参数值类型变体 - 支持多种类型的参数值
using ArgValue = std::variant<std::string, int, double, bool>;

// Arg类 - 代表语法中的一个参数
class Arg {
public:
    explicit Arg(const std::string& name, bool isPlaceholder = false);
    
    // 模板化绑定函数 - 支持不同类型的参数处理
    template<typename T>
    void bind(const std::function<std::string(const T&)>& func);
    
    // 重载操作符，支持直接函数调用语法
    template<typename T>
    std::string operator()(const T& value);
    
    // 匹配值并执行绑定的函数
    void match(const std::string& value);
    void match(const ArgValue& value);
    
    // 转换为JS代码
    const std::string& transform(const std::string& jsCode);
    
    // 获取器方法
    const std::string& getName() const { return name_; }
    bool isPlaceholder() const { return isPlaceholder_; }
    const std::string& getValue() const { return transformedValue_; }
    const std::string& getRawValue() const { return rawValue_; }
    
    // 类型检查辅助方法
    bool isObject() const;
    bool isFunction() const;
    
private:
    std::string name_;
    bool isPlaceholder_;
    std::string rawValue_;
    std::string transformedValue_;
    
    // 使用类型擦除技术存储不同类型的绑定函数
    std::function<std::string(const std::string&)> bindFunction_;
    
    // 内部辅助方法
    template<typename T>
    std::string convertToString(const T& value);
    void executeBinding();
};

// Args类 - 管理参数集合
class Args {
public:
    Args() = default;
    
    // 添加参数
    void addArg(const std::string& name, bool isPlaceholder = false);
    
    // 索引访问
    Arg& operator[](size_t index);
    const Arg& operator[](size_t index) const;
    
    // 名称查找
    Arg* find(const std::string& name);
    const Arg* find(const std::string& name) const;
    
    // 绑定函数 - 支持名称绑定和占位符绑定
    template<typename T>
    void bind(const std::string& name, const std::function<std::string(const T&)>& func);
    
    // 匹配值
    void match(const std::string& name, const std::string& value);
    void match(const std::string& name, const ArgValue& value);
    
    // 转换参数
    void transform(const std::string& name, const std::string& jsCode);
    
    // 生成结果
    std::string result() const;
    
    // 获取参数数量和占位符数量
    size_t length() const { return args_.size(); }
    size_t placeholderCount() const { return placeholderCount_; }
    
private:
    std::vector<Arg> args_;
    std::unordered_map<std::string, size_t> nameToIndex_;
    size_t placeholderCount_ = 0;
    size_t currentPlaceholderIndex_ = 0;
    
    // 内部辅助方法
    Arg* getNextPlaceholder();
};

// Syntax类 - 语法容器
class Syntax {
public:
    Syntax() = default;
    
    // 公共成员，支持直接访问
    Args args;
    
    // 便利方法
    Args& getArgs() { return args; }
    const Args& getArgs() const { return args; }
    
    // 生成结果
    std::string result() const { return args.result(); }
    
private:
    std::string originalPattern_;
    std::string ignoreChars_;
    
    friend std::unique_ptr<Syntax> syntaxAnalys(const std::string& pattern, const std::string& ignoreChars);
};

// CJMODScanner类 - 扫描器核心
class CJMODScanner {
public:
    CJMODScanner();
    
    // 扫描关键字 - 支持Arg对象和字符串
    void scanKeyword(const Arg& arg, std::function<void()> handler);
    void scanKeyword(const std::string& keyword, std::function<void()> handler);
    
    // 窥视关键字 - 获取相对位置的token
    std::string peekKeyword(int offset) const;
    
    // 策略控制
    void policyChangeBegin(const std::string& trigger, Policy policy);
    std::string policyChangeEnd(const std::string& trigger, Policy policy);
    
    // 辅助判断方法
    bool isObject(const std::string& content) const;
    bool isFunction(const std::string& content) const;
    
    // 设置当前扫描的tokens
    void setTokens(const std::vector<std::string>& tokens);
    void setCurrentPosition(size_t position) { currentPosition_ = position; }
    
    // 执行扫描
    void executeScan(const std::string& input);
    
private:
    std::unordered_map<std::string, std::function<void()>> keywordHandlers_;
    std::vector<std::string> currentTokens_;
    size_t currentPosition_;
    Policy currentPolicy_;
    std::string collectBuffer_;
    
    // 策略栈，支持嵌套策略
    struct PolicyState {
        Policy policy;
        std::string trigger;
        std::string buffer;
    };
    std::vector<PolicyState> policyStack_;
    
    // 内部方法
    void tokenize(const std::string& input);
    void processToken(const std::string& token);
    void handlePolicyChange(const std::string& token);
    bool matchesTrigger(const std::string& token, const std::string& trigger) const;
};

// 全局函数声明

// 语法分析函数 - 核心入口点
std::unique_ptr<Syntax> syntaxAnalys(const std::string& pattern, const std::string& ignoreChars = "");

// 代码生成函数
std::string generateCode(const Syntax& syntax);

// 辅助函数
bool isValidIdentifier(const std::string& str);
std::vector<std::string> tokenizePattern(const std::string& pattern, const std::string& ignoreChars);

// 模板实现部分
template<typename T>
void Arg::bind(const std::function<std::string(const T&)>& func) {
    // 使用类型擦除将模板函数转换为统一接口
    bindFunction_ = [func](const std::string& value) -> std::string {
        if constexpr (std::is_same_v<T, std::string>) {
            return func(value);
        } else if constexpr (std::is_same_v<T, int>) {
            try {
                int intValue = std::stoi(value);
                return func(intValue);
            } catch (const std::exception&) {
                return value; // 转换失败时返回原值
            }
        } else if constexpr (std::is_same_v<T, double>) {
            try {
                double doubleValue = std::stod(value);
                return func(doubleValue);
            } catch (const std::exception&) {
                return value;
            }
        } else if constexpr (std::is_same_v<T, bool>) {
            bool boolValue = (value == "true" || value == "1");
            return func(boolValue);
        } else {
            // 对于其他类型，尝试直接调用
            return func(T{});
        }
    };
}

template<typename T>
std::string Arg::operator()(const T& value) {
    std::string strValue = convertToString(value);
    match(strValue);
    return transformedValue_;
}

template<typename T>
std::string Arg::convertToString(const T& value) {
    if constexpr (std::is_same_v<T, std::string>) {
        return value;
    } else if constexpr (std::is_arithmetic_v<T>) {
        return std::to_string(value);
    } else {
        // 对于其他类型，尝试使用字符串流
        std::ostringstream oss;
        oss << value;
        return oss.str();
    }
}

template<typename T>
void Args::bind(const std::string& name, const std::function<std::string(const T&)>& func) {
    if (name == "$") {
        // 绑定到下一个未绑定的占位符
        Arg* placeholder = getNextPlaceholder();
        if (placeholder) {
            placeholder->bind(func);
        }
    } else {
        Arg* arg = find(name);
        if (arg) {
            arg->bind(func);
        }
    }
}

} // namespace CJMOD
} // namespace CHTL