#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <functional>
#include <memory>

namespace CHTL {
namespace CJMOD {

// 前向声明
class Scanner;
struct CodeFragment;

/**
 * @brief 扫描策略枚举 - 完善的Policy系统
 */
enum class Policy {
    NORMAL,     // 常规扫描
    COLLECT,    // 收集模式 - 收集内容直到遇到结束标记
    SKIP        // 跳过模式 - 跳过指定内容
};

/**
 * @brief Arg类 - 完善的参数处理设计
 * 
 * 修正原始API缺陷：
 * 1. 分离bind（值处理）和transform（JS转换）职责
 * 2. 支持占位符自动管理
 * 3. 与双指针扫描机制集成
 */
class Arg {
public:
    Arg(const std::string& name = "", bool isPlaceholder = false);
    
    // 修正后的bind设计 - 只负责值处理
    template<typename T>
    void bind(const std::function<std::string(T)>& processor) {
        valueProcessor_ = [processor](const std::string& raw) -> std::string {
            if constexpr (std::is_same_v<T, std::string>) {
                return processor(raw);
            } else if constexpr (std::is_same_v<T, int>) {
                try {
                    return processor(std::stoi(raw));
                } catch (...) {
                    return processor(0);
                }
            } else if constexpr (std::is_same_v<T, double>) {
                try {
                    return processor(std::stod(raw));
                } catch (...) {
                    return processor(0.0);
                }
            } else {
                return processor(T{});
            }
        };
        hasBind_ = true;
    }
    
    // 无参数bind - 用于固定值
    void bind(const std::function<std::string()>& generator) {
        valueProcessor_ = [generator](const std::string&) -> std::string {
            return generator();
        };
        hasBind_ = true;
    }
    
    // match方法 - 接收原始值并应用处理器
    void match(const std::string& rawValue);
    
    // transform方法 - 只负责JS代码转换
    void transform(const std::string& jsTemplate);
    
    // 获取器
    const std::string& getName() const { return name_; }
    bool isPlaceholder() const { return isPlaceholder_; }
    bool hasBind() const { return hasBind_; }
    bool hasValue() const { return hasValue_; }
    const std::string& getRawValue() const { return rawValue_; }
    const std::string& getProcessedValue() const { return processedValue_; }
    const std::string& getJSCode() const { return jsCode_; }
    
    // 新增：支持可选参数、无修饰字面量和可变参数
    bool isOptional() const { return isOptional_; }
    bool hasLiteralSupport() const { return literalSupport_; }
    bool isVariadic() const { return isVariadic_; }
    void SetOptional(bool optional) { isOptional_ = optional; }
    void SetLiteralSupport(bool support) { literalSupport_ = support; }
    void SetVariadic(bool variadic) { isVariadic_ = variadic; }
    
    // 可变参数专用方法
    void addVariadicValue(const std::string& value);
    const std::vector<std::string>& getVariadicValues() const { return variadicValues_; }
    size_t getVariadicCount() const { return variadicValues_.size(); }
    
    // 输出重载
    friend std::ostream& operator<<(std::ostream& os, const Arg& arg);

private:
    std::string name_;
    bool isPlaceholder_;
    bool hasBind_;
    bool hasValue_;
    bool isOptional_;        // 新增：是否为可选参数
    bool literalSupport_;    // 新增：是否支持无修饰字面量
    bool isVariadic_;        // 新增：是否为可变参数
    std::string rawValue_;
    std::string processedValue_;
    std::string jsTemplate_;
    std::string jsCode_;
    std::vector<std::string> variadicValues_;  // 新增：可变参数值列表
    std::function<std::string(const std::string&)> valueProcessor_;
    
    void applyTransform();
};

/**
 * @brief Syntax类 - 完善的语法容器设计
 * 
 * 修正原始API缺陷：
 * 1. 改进占位符管理机制
 * 2. 支持链式操作（Enhanced版本特性）
 * 3. 更好的参数查找和绑定
 */
class Syntax {
public:
    Syntax() = default;
    
    // 核心：args向量
    std::vector<Arg> args;
    
    // 改进的bind方法 - 支持占位符自动计数
    template<typename T>
    Syntax& bind(const std::string& name, const std::function<std::string(T)>& processor) {
        auto* arg = findArg(name);
        if (arg) {
            arg->bind(processor);
        }
        return *this; // 支持链式调用
    }
    
    Syntax& bind(const std::string& name, const std::function<std::string()>& generator) {
        auto* arg = findArg(name);
        if (arg) {
            arg->bind(generator);
        }
        return *this;
    }
    
    // 批量操作支持
    Syntax& match(const std::string& name, const std::string& value);
    Syntax& transform(const std::string& name, const std::string& jsTemplate);
    
    // result方法 - 组合所有参数的JS代码
    std::string result() const;
    
    // 工具方法
    size_t length() const { return args.size(); }
    void addArg(const std::string& name, bool isPlaceholder = false);
    void setTriggerKeyword(const std::string& keyword);
    const std::string& getTriggerKeyword() const { return triggerKeyword_; }
    
    // 占位符管理
    size_t getPlaceholderCount() const { return placeholderCounter_; }
    void resetPlaceholderIndex() { currentPlaceholderIndex_ = 0; }
    
    // 新增：无序和字面量支持
    void SetUnorderedSupport(bool support) { unorderedSupport_ = support; }
    void SetLiteralSupport(bool support) { literalSupport_ = support; }
    bool hasUnorderedSupport() const { return unorderedSupport_; }
    bool hasLiteralSupport() const { return literalSupport_; }

private:
    std::unordered_map<std::string, size_t> nameToIndex_;
    size_t placeholderCounter_ = 0;
    mutable size_t currentPlaceholderIndex_ = 0; // 用于占位符自动计数
    std::string triggerKeyword_;
    bool unorderedSupport_ = false;  // 新增：是否支持无序参数
    bool literalSupport_ = false;    // 新增：是否支持无修饰字面量
    
    Arg* findArg(const std::string& name);
};

/**
 * @brief CHTLJSFunction类 - 完善的CJMOD处理器
 * 
 * 集成所有讨论的特性：
 * 1. 双指针扫描机制
 * 2. 前置提取机制  
 * 3. 完整的Policy系统
 * 4. 与统一扫描器集成
 * 5. 虚对象支持
 */
class CHTLJSFunction {
public:
    CHTLJSFunction();
    
    // 修正后的核心流程
    std::unique_ptr<Syntax> syntaxAnalys(const std::string& pattern, const std::string& ignoreChars = "");
    
    // 扫描器集成 - 双指针扫描机制
    void scanKeyword(const Arg& arg, std::function<void()> handler);
    void scanKeyword(const std::string& keyword, std::function<void()> handler);
    
    // 辅助函数 - 完善的工具集
    std::string peekKeyword(int offset) const;
    bool isObject(const std::string& content) const;
    bool isFunction(const std::string& content) const;
    std::string slice(const std::string& content, size_t start, size_t end = std::string::npos) const;
    
    // 完善的Policy系统
    void policyChangeBegin(const std::string& trigger, Policy policy);
    std::string policyChangeEnd(const std::string& trigger, Policy policy);
    Policy getCurrentPolicy() const { return currentPolicy_; }
    
    // 代码生成和优化
    std::string generateCode(const Syntax& syntax);
    
    // 统一扫描器集成
    void setCodeFragment(const CodeFragment& fragment);
    std::string processSourceCode(const std::string& sourceCode, const std::string& pattern);
    
    // 虚对象支持 - 委托给CHTL JS
    void virBind(const std::string& functionName);
    bool hasVirtualObject(const std::string& name) const;

private:
    CodeFragment* currentFragment_;
    bool hasFragment_;
    Policy currentPolicy_;
    std::unordered_map<std::string, std::function<void()>> keywordHandlers_;
    
    // Policy栈管理
    struct PolicyState {
        std::string trigger;
        Policy policy;
        size_t position;
    };
    std::vector<PolicyState> policyStack_;
    
    // 双指针扫描状态
    struct ScannerState {
        size_t frontPointer;
        size_t backPointer;
        bool isScanning;
        std::string collectedContent;
    };
    ScannerState scannerState_;
    
    // 虚对象注册表
    std::unordered_map<std::string, std::string> virtualObjects_;
    
    // 内部方法
    std::vector<std::string> tokenize(const std::string& input, const std::string& ignoreChars);
    void processPolicyChange(const std::string& trigger, Policy policy);
    std::string optimizeJSCode(const std::string& jsCode);
    void initializeScannerState();
    void updateScannerPointers();
    std::string extractContent(size_t start, size_t end);
};

// ============================================================================
// 全局函数 - 完善的API入口点
// ============================================================================

/**
 * @brief syntaxAnalys全局函数 - 支持无序、可选、无修饰字面量、可变参数的语法分析
 * @param pattern 语法模式字符串，使用$作为占位符，$?表示可选参数，...表示可变参数
 * @param ignoreChars 忽略的字符集（可选）
 * @param unorderedSupport 是否支持无序参数（默认true）
 * @param literalSupport 是否支持无修饰字面量（默认true）
 */
std::unique_ptr<Syntax> syntaxAnalys(const std::string& pattern, 
                                    const std::string& ignoreChars = "",
                                    bool unorderedSupport = true,
                                    bool literalSupport = true);

/**
 * @brief generateCode全局函数 - 优化的代码生成
 */
std::string generateCode(const Syntax& syntax);

/**
 * @brief createCHTLJSFunction - 完善的简化流程
 * 
 * 自动化完整的原始API流程：
 * 1. syntaxAnalys - 自动分析占位符
 * 2. bind - 自动绑定默认处理器
 * 3. transform - 自动设置JS转换
 * 4. scanKeyword - 自动注册扫描器
 * 5. match - 自动匹配参数值
 * 6. result - 自动组合结果
 * 7. generateCode - 自动生成优化的JS代码
 */
std::string createCHTLJSFunction(const std::string& chtlJsCode);

// ============================================================================
// Enhanced版本特性 - 可选的高级功能
// ============================================================================

/**
 * @brief EnhancedSyntax - 支持链式操作的增强版本
 */
class EnhancedSyntax : public Syntax {
public:
    // 链式bind操作
    template<typename T>
    EnhancedSyntax& chainBind(const std::string& name, const std::function<std::string(T)>& processor) {
        bind(name, processor);
        return *this;
    }
    
    // 链式transform操作
    EnhancedSyntax& chainTransform(const std::string& name, const std::string& jsTemplate) {
        transform(name, jsTemplate);
        return *this;
    }
    
    // 批量绑定
    EnhancedSyntax& bindAll(const std::function<std::string(const std::string&)>& defaultProcessor);
};

/**
 * @brief AutoFillProcessor - 智能填充处理器
 */
class AutoFillProcessor {
public:
    static std::string autoFill(const std::string& chtlJsCode, const std::vector<std::string>& values);
    static std::unique_ptr<Syntax> smartAnalysis(const std::string& pattern);
    static void applyIntelligentBinding(Syntax& syntax);
};

} // namespace CJMOD
} // namespace CHTL

/**
 * @brief 🎯 完善的CJMOD API设计总结
 * 
 * 【修正的核心流程】
 * syntaxAnalys -> bind -> transform -> scanKeyword -> match -> result -> generateCode
 * 
 * 【三个核心类 - 完善版】
 * ✅ Arg：分离bind和transform职责，完善占位符处理
 * ✅ Syntax：改进参数管理，支持链式操作
 * ✅ CHTLJSFunction：集成双指针扫描、Policy系统、虚对象支持
 * 
 * 【集成的高级特性】
 * ✅ 双指针扫描和前置提取机制
 * ✅ 完整的Policy系统（NORMAL, COLLECT, SKIP）
 * ✅ 与统一扫描器深度集成
 * ✅ 虚对象委托支持
 * ✅ 智能的占位符管理
 * ✅ 优化的JS代码生成
 * ✅ Enhanced版本的链式操作
 * ✅ AutoFill智能填充
 * 
 * 【简化流程】
 * ✅ createCHTLJSFunction：一键自动化所有步骤
 * ✅ 智能默认处理器
 * ✅ 自动优化和错误处理
 */