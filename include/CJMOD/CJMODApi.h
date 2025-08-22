#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <functional>
#include <variant>
#include <memory>

namespace CHTL {
namespace CJMOD {

// 前向声明
class Syntax;
class CJMODScanner;

/**
 * @brief 参数值类型
 */
using ArgValue = std::variant<std::string, int, double, bool>;

/**
 * @brief 扫描策略枚举
 */
enum class ScanPolicy {
    NORMAL,     // 常规扫描
    COLLECT,    // 收集模式
    SKIP        // 跳过模式
};

/**
 * @brief Arg类 - 参数处理核心
 * 正确流程：bind设置获取值规则，transform设置JS转换模板
 */
class Arg {
public:
    /**
     * @brief 构造函数
     */
    Arg(const std::string& name, bool isPlaceholder = false);
    
    /**
     * @brief bind方法 - 设置如何处理输入值的规则
     * @param func 值处理函数（接受输入，返回处理后的值）
     */
    template<typename T>
    void Bind(const std::function<std::string(T)>& func) {
        // bind负责设置如何处理输入值，transform负责JS代码转换
        if constexpr (std::is_same_v<T, ArgValue> || std::is_same_v<T, const ArgValue&>) {
            argValueProcessor_ = [func](const ArgValue& value) -> std::string {
                if constexpr (std::is_same_v<T, const ArgValue&>) {
                    return func(value);
                } else {
                    return func(value);
                }
            };
        } else if constexpr (std::is_same_v<T, std::string>) {
            stringProcessor_ = func;
        } else if constexpr (std::is_same_v<T, int>) {
            intProcessor_ = func;
        } else if constexpr (std::is_same_v<T, double>) {
            doubleProcessor_ = func;
        } else if constexpr (std::is_same_v<T, bool>) {
            boolProcessor_ = func;
        }
        hasProcessor_ = true;
    }
    
    /**
     * @brief operator() - 使用bind的处理器处理输入值
     * @param value 输入值
     */
    template<typename T>
    std::string operator()(T value) {
        // 使用bind设置的处理器处理输入值
        if constexpr (std::is_same_v<T, std::string>) {
            if (stringProcessor_) {
                rawValue_ = stringProcessor_(value);
                return rawValue_;
            }
        } else if constexpr (std::is_same_v<T, int>) {
            if (intProcessor_) {
                rawValue_ = intProcessor_(value);
                return rawValue_;
            }
        } else if constexpr (std::is_same_v<T, double>) {
            if (doubleProcessor_) {
                rawValue_ = doubleProcessor_(value);
                return rawValue_;
            }
        } else if constexpr (std::is_same_v<T, bool>) {
            if (boolProcessor_) {
                rawValue_ = boolProcessor_(value);
                return rawValue_;
            }
        }
        
        // 没有处理器，直接转换为字符串
        if constexpr (std::is_same_v<T, std::string>) {
            rawValue_ = value;
        } else {
            rawValue_ = std::to_string(value);
        }
        return rawValue_;
    }
    
    /**
     * @brief match方法 - 直接设置参数值
     * @param value 参数值
     */
    void Match(const ArgValue& value);
    
    /**
     * @brief transform方法 - 设置JS代码转换模板
     * @param jsTemplate JS代码模板
     * @return 转换后的JS代码
     */
    std::string Transform(const std::string& jsTemplate);
    
    /**
     * @brief 获取原始值
     */
    const std::string& GetRawValue() const { return rawValue_; }
    
    /**
     * @brief 获取转换后的JS代码
     */
    const std::string& GetTransformedValue() const { return transformedValue_; }
    
    /**
     * @brief 获取参数名称
     */
    const std::string& GetName() const { return name_; }
    
    /**
     * @brief 是否为占位符
     */
    bool IsPlaceholder() const { return isPlaceholder_; }

private:
    std::string name_;                          // 参数名称
    bool isPlaceholder_;                        // 是否为占位符
    std::string rawValue_;                      // 原始值（match设置的值）
    std::string transformedValue_;              // 转换后的值（transform处理的JS代码）
    std::string jsTemplate_;                    // JS转换模板
    bool hasProcessor_;                         // 是否有值处理器
    
    // 值处理函数（bind设置的规则）
    std::function<std::string(std::string)> stringProcessor_;
    std::function<std::string(int)> intProcessor_;
    std::function<std::string(double)> doubleProcessor_;
    std::function<std::string(bool)> boolProcessor_;
    std::function<std::string(ArgValue)> argValueProcessor_;
    
    // 辅助方法
    void SetRawValueDirectly(const ArgValue& value);
    std::string DetectValueType(const std::string& value) const;
    std::string EscapeForJavaScript(const std::string& value) const;
    std::string FormatAsJSON(const std::string& value, const std::string& type) const;
    std::string FormatAsParameter(const std::string& value, const std::string& type) const;
};

/**
 * @brief ArgCollection类 - 参数集合管理
 */
class ArgCollection {
public:
    void AddArg(const std::string& name, bool isPlaceholder = false);
    
    /**
     * @brief bind方法 - 为指定参数设置值处理规则
     */
    template<typename T>
    void Bind(const std::string& name, const std::function<std::string(T)>& func) {
        auto it = nameToIndex_.find(name);
        if (it != nameToIndex_.end()) {
            args_[it->second].Bind<T>(func);
        }
    }
    
    template<typename T>
    void Bind(size_t index, const std::function<std::string(T)>& func) {
        if (index < args_.size()) {
            args_[index].Bind<T>(func);
        }
    }
    
    // 兼容性重载 - 支持const ArgValue&类型
    void Bind(const std::string& name, const std::function<std::string(const ArgValue&)>& func) {
        Bind<const ArgValue&>(name, func);
    }
    
    void Bind(size_t index, const std::function<std::string(const ArgValue&)>& func) {
        Bind<const ArgValue&>(index, func);
    }
    
    /**
     * @brief match方法 - 直接设置参数值
     */
    void Match(const std::string& name, const ArgValue& value);
    void Match(size_t index, const ArgValue& value);
    
    /**
     * @brief transform方法 - 为指定参数设置JS转换模板
     */
    void Transform(const std::string& name, const std::string& jsTemplate);
    
    /**
     * @brief result方法 - 组合所有转换后的JS代码
     */
    std::string Result() const;
    
    /**
     * @brief 索引访问
     */
    Arg& operator[](size_t index);
    const Arg& operator[](size_t index) const;
    
    size_t Length() const { return args_.size(); }
    size_t Size() const { return args_.size(); } // 兼容性方法

private:
    std::vector<Arg> args_;
    std::unordered_map<std::string, size_t> nameToIndex_;
};

/**
 * @brief Syntax类 - 语法分析结果
 */
class Syntax {
public:
    Syntax(const std::string& pattern, const std::string& ignoreChars = "");
    
    ArgCollection& GetArgs() { return args_; }
    const ArgCollection& GetArgs() const { return args_; }
    
    /**
     * @brief 便捷bind方法
     */
    template<typename T>
    void Bind(const std::string& name, const std::function<std::string(T)>& func) {
        args_.Bind<T>(name, func);
    }
    
    /**
     * @brief result方法 - 生成最终JS代码
     */
    std::string Result() const;

private:
    std::string pattern_;
    std::string ignoreChars_;
    ArgCollection args_;
    
    void ParsePattern();
};

/**
 * @brief CJMODScanner类 - 扫描器
 */
class CJMODScanner {
public:
    CJMODScanner();
    
    /**
     * @brief scanKeyword - 核心扫描接口
     */
    void ScanKeyword(const std::string& keyword, std::function<void()> handler);
    void ScanKeyword(const Arg& arg, std::function<void()> handler);
    
    /**
     * @brief peekKeyword - 查看偏移关键字
     */
    std::string PeekKeyword(int offset) const;
    
    /**
     * @brief policy系列 - 策略改变
     */
    void PolicyChangeBegin(const std::string& trigger, ScanPolicy policy);
    std::string PolicyChangeEnd(const std::string& trigger, ScanPolicy policy);
    
    /**
     * @brief 辅助函数
     */
    bool IsObject(const std::string& content);
    bool IsFunction(const std::string& content);

public:
    std::unordered_map<std::string, std::function<void()>> keywordHandlers_;
    std::vector<std::string> currentTokens_;
    size_t currentPosition_;
    ScanPolicy currentPolicy_;
    std::string collectBuffer_;
};

/**
 * @brief CJMODExtension基类
 */
class CJMODExtension {
public:
    virtual ~CJMODExtension() = default;
    virtual bool Initialize(CJMODScanner& scanner) = 0;
    virtual std::string GetName() const = 0;
    virtual std::string GetVersion() const = 0;
    virtual std::vector<std::string> GetSupportedSyntax() const = 0;
};

/**
 * @brief 全局函数
 */
std::unique_ptr<Syntax> SyntaxAnalys(const std::string& pattern, const std::string& ignoreChars = "");
std::string GenerateCode(const Syntax& syntax);

/**
 * @brief CJMODManager类 - 纯API管理
 */
class CJMODManager {
public:
    CJMODManager();
    
    bool RegisterExtension(std::unique_ptr<CJMODExtension> extension);
    CJMODScanner& GetScanner() { return scanner_; }
    std::string ProcessCHTLJS(const std::string& source);

private:
    CJMODScanner scanner_;
    std::vector<std::unique_ptr<CJMODExtension>> extensions_;
    std::unordered_map<std::string, std::function<void()>> keywordHandlers_;  // 关键字处理器映射
    std::string currentSource_;                                               // 当前处理的源代码
    
    // 内部方法
    void SetCurrentSource(const std::string& source);
    std::string GetProcessedSource() const;
};

} // namespace CJMOD
} // namespace CHTL