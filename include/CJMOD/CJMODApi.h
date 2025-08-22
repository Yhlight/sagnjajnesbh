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
 * @brief 参数值类型 - 支持多种类型的参数
 */
using ArgValue = std::variant<std::string, int, double, bool>;

/**
 * @brief 参数绑定函数类型
 */
template<typename T>
using ArgBindFunction = std::function<std::string(T)>;

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
 * 按照用户的原始API设计：支持bind、match、transform、operator()
 */
class Arg {
public:
    /**
     * @brief 构造函数
     * @param name 参数名称
     * @param isPlaceholder 是否为占位符($)
     */
    Arg(const std::string& name, bool isPlaceholder = false);
    
    /**
     * @brief 模板化bind方法 - 绑定处理函数
     * @param func 处理函数
     */
    template<typename T>
    void Bind(const std::function<std::string(T)>& func) {
        // 存储类型安全的绑定函数
        if constexpr (std::is_same_v<T, std::string>) {
            stringBindFunction_ = func;
        } else if constexpr (std::is_same_v<T, int>) {
            intBindFunction_ = func;
        } else if constexpr (std::is_same_v<T, double>) {
            doubleBindFunction_ = func;
        } else if constexpr (std::is_same_v<T, bool>) {
            boolBindFunction_ = func;
        }
        
        hasBoundFunction_ = true;
    }
    
    /**
     * @brief 重载operator() - 接受参数并调用绑定函数
     * @param value 参数值
     * @return 处理后的字符串
     */
    template<typename T>
    std::string operator()(T value) {
        if constexpr (std::is_same_v<T, std::string>) {
            if (stringBindFunction_) {
                currentValue_ = stringBindFunction_(value);
                return currentValue_;
            }
        } else if constexpr (std::is_same_v<T, int>) {
            if (intBindFunction_) {
                currentValue_ = intBindFunction_(value);
                return currentValue_;
            }
        } else if constexpr (std::is_same_v<T, double>) {
            if (doubleBindFunction_) {
                currentValue_ = doubleBindFunction_(value);
                return currentValue_;
            }
        } else if constexpr (std::is_same_v<T, bool>) {
            if (boolBindFunction_) {
                currentValue_ = boolBindFunction_(value);
                return currentValue_;
            }
        }
        
        // 如果没有绑定函数，直接转换为字符串
        if constexpr (std::is_same_v<T, std::string>) {
            currentValue_ = value;
        } else {
            currentValue_ = std::to_string(value);
        }
        
        return currentValue_;
    }
    
    /**
     * @brief match方法 - 匹配参数值并调用绑定函数
     * @param value 参数值
     */
    void Match(const ArgValue& value);
    
    /**
     * @brief transform方法 - 将参数转换为JS代码
     * @param jsTemplate JS代码模板
     * @return 转换后的JS代码
     */
    std::string Transform(const std::string& jsTemplate);
    
    /**
     * @brief 获取参数名称
     */
    const std::string& GetName() const { return name_; }
    
    /**
     * @brief 是否为占位符
     */
    bool IsPlaceholder() const { return isPlaceholder_; }
    
    /**
     * @brief 是否已绑定函数
     */
    bool IsBound() const { return hasBoundFunction_; }
    
    /**
     * @brief 获取当前值
     */
    const std::string& GetCurrentValue() const { return currentValue_; }
    
    /**
     * @brief 重置参数状态
     */
    void Reset();

private:
    std::string name_;                          // 参数名称
    bool isPlaceholder_;                        // 是否为占位符
    std::string currentValue_;                  // 当前值
    bool hasBoundFunction_;                     // 是否已绑定函数
    
    // 类型安全的绑定函数
    std::function<std::string(std::string)> stringBindFunction_;
    std::function<std::string(int)> intBindFunction_;
    std::function<std::string(double)> doubleBindFunction_;
    std::function<std::string(bool)> boolBindFunction_;
};

/**
 * @brief ArgCollection类 - 参数集合管理
 */
class ArgCollection {
public:
    /**
     * @brief 添加参数
     * @param name 参数名称
     * @param isPlaceholder 是否为占位符
     */
    void AddArg(const std::string& name, bool isPlaceholder = false);
    
    /**
     * @brief 模板化bind方法 - 按名称绑定
     * @param name 参数名称
     * @param func 处理函数
     */
    template<typename T>
    void Bind(const std::string& name, const std::function<std::string(T)>& func) {
        auto it = nameToIndex_.find(name);
        if (it != nameToIndex_.end()) {
            args_[it->second].Bind<T>(func);
        } else {
            // 对于占位符，绑定第一个未绑定的占位符
            for (auto& arg : args_) {
                if (arg.IsPlaceholder() && !arg.IsBound()) {
                    arg.Bind<T>(func);
                    break;
                }
            }
        }
    }
    
    /**
     * @brief 按索引绑定
     * @param index 参数索引
     * @param func 处理函数
     */
    template<typename T>
    void Bind(size_t index, const std::function<std::string(T)>& func) {
        if (index < args_.size()) {
            args_[index].Bind<T>(func);
        }
    }
    
    /**
     * @brief 匹配参数值
     * @param name 参数名称
     * @param value 参数值
     */
    void Match(const std::string& name, const ArgValue& value);
    
    /**
     * @brief 按索引匹配参数值
     * @param index 参数索引
     * @param value 参数值
     */
    void Match(size_t index, const ArgValue& value);
    
    /**
     * @brief transform方法 - 转换指定参数为JS代码
     * @param name 参数名称
     * @param jsTemplate JS代码模板
     */
    void Transform(const std::string& name, const std::string& jsTemplate);
    
    /**
     * @brief 索引访问操作符
     */
    Arg& operator[](size_t index);
    const Arg& operator[](size_t index) const;
    
    /**
     * @brief result方法 - 组合所有参数的结果
     * @return 组合后的字符串
     */
    std::string Result() const;
    
    /**
     * @brief 获取参数数量
     */
    size_t Length() const { return args_.size(); }
    
    /**
     * @brief 重置所有参数
     */
    void Reset();

private:
    std::vector<Arg> args_;                                     // 参数列表
    std::unordered_map<std::string, size_t> nameToIndex_;       // 名称到索引的映射
};

/**
 * @brief Syntax类 - 语法分析结果
 * 按照用户的原始API设计：维护args和result
 */
class Syntax {
public:
    /**
     * @brief 构造函数
     * @param pattern 语法模式
     * @param ignoreChars 忽略的字符
     */
    Syntax(const std::string& pattern, const std::string& ignoreChars = "");
    
    /**
     * @brief 获取参数集合
     */
    ArgCollection& GetArgs() { return args_; }
    const ArgCollection& GetArgs() const { return args_; }
    
    /**
     * @brief 模板化bind方法 - 便捷绑定
     * @param name 参数名称
     * @param func 处理函数
     */
    template<typename T>
    void Bind(const std::string& name, const std::function<std::string(T)>& func) {
        args_.Bind<T>(name, func);
    }
    
    /**
     * @brief result方法 - 生成最终代码
     * @return 生成的代码
     */
    std::string Result() const;
    
    /**
     * @brief 生成代码（兼容性方法）
     */
    std::string GenerateCode() const { return Result(); }

private:
    std::string pattern_;           // 原始语法模式
    std::string ignoreChars_;       // 忽略的字符
    ArgCollection args_;            // 参数集合
    std::string result_;            // 结果缓存
    
    /**
     * @brief 解析语法模式
     */
    void ParsePattern();
};

/**
 * @brief CJMODScanner类 - 扫描器核心
 * 按照用户的原始API设计：scanKeyword、policyChange、peekKeyword
 */
class CJMODScanner {
public:
    CJMODScanner();
    
    /**
     * @brief scanKeyword - 扫描关键字的核心接口
     * @param keyword 关键字
     * @param handler 处理函数
     */
    void ScanKeyword(const std::string& keyword, std::function<void()> handler);
    
    /**
     * @brief scanKeyword - 扫描Arg关键字
     * @param arg 参数对象
     * @param handler 处理函数
     */
    void ScanKeyword(const Arg& arg, std::function<void()> handler);
    
    /**
     * @brief peekKeyword - 查看指定偏移的关键字
     * @param offset 偏移量（负数向前，正数向后）
     * @return 关键字字符串
     */
    std::string PeekKeyword(int offset) const;
    
    /**
     * @brief policyChangeBegin - 开始策略改变
     * @param trigger 触发字符
     * @param policy 新策略
     */
    void PolicyChangeBegin(const std::string& trigger, ScanPolicy policy);
    
    /**
     * @brief policyChangeEnd - 结束策略改变
     * @param trigger 结束字符
     * @param policy 恢复的策略
     * @return 收集的内容（COLLECT模式下）
     */
    std::string PolicyChangeEnd(const std::string& trigger, ScanPolicy policy);
    
    /**
     * @brief 辅助函数：判断是否为对象
     * @param content 内容字符串
     * @return 是否为对象
     */
    bool IsObject(const std::string& content);
    
    /**
     * @brief 辅助函数：判断是否为函数
     * @param content 内容字符串
     * @return 是否为函数
     */
    bool IsFunction(const std::string& content);

private:
    std::unordered_map<std::string, std::function<void()>> keywordHandlers_;   // 关键字处理器
    std::vector<std::string> currentTokens_;                                   // 当前Token流
    size_t currentPosition_;                                                   // 当前位置
    ScanPolicy currentPolicy_;                                                 // 当前策略
    std::string collectBuffer_;                                                // 收集缓冲区
};

/**
 * @brief CJMODExtension基类 - 扩展接口
 */
class CJMODExtension {
public:
    virtual ~CJMODExtension() = default;
    
    /**
     * @brief 初始化扩展 - 注册语法处理器
     * @param scanner 扫描器引用
     * @return 是否初始化成功
     */
    virtual bool Initialize(CJMODScanner& scanner) = 0;
    
    /**
     * @brief 获取扩展名称
     */
    virtual std::string GetName() const = 0;
    
    /**
     * @brief 获取扩展版本
     */
    virtual std::string GetVersion() const = 0;
    
    /**
     * @brief 获取支持的语法列表
     */
    virtual std::vector<std::string> GetSupportedSyntax() const = 0;
};

/**
 * @brief 全局函数：syntaxAnalys - 语法分析核心
 * @param pattern 语法模式字符串
 * @param ignoreChars 需要忽略的字符
 * @return Syntax对象智能指针
 */
std::unique_ptr<Syntax> SyntaxAnalys(const std::string& pattern, const std::string& ignoreChars = "");

/**
 * @brief 全局函数：generateCode - 代码生成
 * @param syntax 语法对象
 * @return 生成的JavaScript代码
 */
std::string GenerateCode(const Syntax& syntax);

/**
 * @brief CJMODManager类 - CJMOD管理器
 * 纯API管理，不包含具体扩展实现
 */
class CJMODManager {
public:
    CJMODManager();
    ~CJMODManager() = default;
    
    /**
     * @brief 注册扩展
     * @param extension 扩展对象
     * @return 是否注册成功
     */
    bool RegisterExtension(std::unique_ptr<CJMODExtension> extension);
    
    /**
     * @brief 获取扫描器引用
     */
    CJMODScanner& GetScanner() { return scanner_; }
    
    /**
     * @brief 处理CHTL JS代码（纯API驱动）
     * @param source CHTL JS源代码
     * @return 处理后的JavaScript代码
     */
    std::string ProcessCHTLJS(const std::string& source);

private:
    CJMODScanner scanner_;                                          // 扫描器
    std::vector<std::unique_ptr<CJMODExtension>> extensions_;       // 已注册的扩展
};

} // namespace CJMOD
} // namespace CHTL