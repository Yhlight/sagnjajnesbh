#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <functional>
#include <memory>
#include <variant>

namespace CHTL {
namespace CJMOD {

/**
 * @brief CJMOD扫描策略枚举
 */
enum class ScanPolicy {
    NORMAL,     // 常规扫描
    COLLECT,    // 收集模式（收集所有内容）
    SKIP        // 跳过模式
};

/**
 * @brief 参数值类型变体
 */
using ArgValue = std::variant<std::string, int, double, bool>;

/**
 * @brief 参数绑定函数类型
 */
using ArgBindFunction = std::function<std::string(const ArgValue&)>;

/**
 * @brief 参数类
 */
class Arg {
public:
    /**
     * @brief 构造函数
     * @param name 参数名称
     * @param isPlaceholder 是否为占位符
     */
    Arg(const std::string& name, bool isPlaceholder = false);
    
    /**
     * @brief 绑定处理函数
     * @param func 处理函数
     */
    void Bind(const ArgBindFunction& func);
    
    /**
     * @brief 匹配并处理值
     * @param value 参数值
     * @return 处理后的结果
     */
    std::string Match(const ArgValue& value);
    
    /**
     * @brief 设置转换模板
     * @param jsTemplate JavaScript代码模板
     */
    void Transform(const std::string& jsTemplate);
    
    /**
     * @brief 获取参数名称
     */
    const std::string& GetName() const { return name_; }
    
    /**
     * @brief 检查是否为占位符
     */
    bool IsPlaceholder() const { return isPlaceholder_; }
    
    /**
     * @brief 检查是否已绑定
     */
    bool IsBound() const { return bindFunction_ != nullptr; }
    
    /**
     * @brief 获取当前值
     */
    const std::string& GetCurrentValue() const { return currentValue_; }
    
    /**
     * @brief 重置参数状态
     */
    void Reset();

private:
    // 完整的JS代码转换辅助方法
    std::string GetValueType() const;
    bool IsStringValue() const;
    bool IsNumberValue() const;
    bool IsBooleanValue() const;
    std::string EscapeJavaScript(const std::string& value) const;
    std::string ToJSON(const std::string& value) const;
    std::string FormatAsParameter(const std::string& value) const;
    std::string FormatAsArrayElement(const std::string& value) const;
    std::string FormatAsObjectProperty(const std::string& name, const std::string& value) const;
    std::string ApplyCustomTransformations(const std::string& template_str) const;
    bool EvaluateCondition(const std::string& condition) const;
    std::string ProcessForLoop(const std::string& arrayName, const std::string& itemName, const std::string& template_content) const;
    std::string ProcessFunctionCall(const std::string& functionName, const std::string& params) const;

public:

private:
    std::string name_;                  // 参数名称
    bool isPlaceholder_;                // 是否为占位符
    ArgBindFunction bindFunction_;      // 绑定的处理函数
    std::string jsTemplate_;            // JavaScript模板
    std::string currentValue_;          // 当前值
    int matchCount_;                    // 匹配计数（用于占位符）
};

/**
 * @brief 参数集合类
 */
class ArgCollection {
public:
    /**
     * @brief 添加参数
     */
    void AddArg(const std::string& name, bool isPlaceholder = false);
    
    /**
     * @brief 通过名称绑定参数
     * @param name 参数名称
     * @param func 处理函数
     */
    void Bind(const std::string& name, const ArgBindFunction& func);
    
    /**
     * @brief 通过索引绑定参数
     * @param index 参数索引
     * @param func 处理函数
     */
    void Bind(size_t index, const ArgBindFunction& func);
    
    /**
     * @brief 通过名称匹配参数
     * @param name 参数名称
     * @param value 参数值
     */
    void Match(const std::string& name, const ArgValue& value);
    
    /**
     * @brief 通过索引匹配参数
     * @param index 参数索引
     * @param value 参数值
     */
    void Match(size_t index, const ArgValue& value);
    
    /**
     * @brief 设置参数转换模板
     * @param name 参数名称
     * @param jsTemplate JavaScript模板
     */
    void Transform(const std::string& name, const std::string& jsTemplate);
    
    /**
     * @brief 通过索引访问参数
     */
    Arg& operator[](size_t index);
    const Arg& operator[](size_t index) const;
    
    /**
     * @brief 获取参数数量
     */
    size_t Size() const { return args_.size(); }
    
    /**
     * @brief 生成最终结果
     * @return 组合后的JavaScript代码
     */
    std::string Result() const;
    
    /**
     * @brief 重置所有参数
     */
    void Reset();

private:
    std::vector<Arg> args_;                                     // 参数列表
    std::unordered_map<std::string, size_t> nameToIndex_;       // 名称到索引的映射
};

/**
 * @brief 语法分析结果类
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
     * @brief 获取语法模式
     */
    const std::string& GetPattern() const { return pattern_; }
    
    /**
     * @brief 获取忽略字符
     */
    const std::string& GetIgnoreChars() const { return ignoreChars_; }
    
    /**
     * @brief 生成最终JavaScript代码
     * @return 生成的JavaScript代码
     */
    std::string GenerateCode() const;

private:
    std::string pattern_;           // 语法模式
    std::string ignoreChars_;       // 忽略的字符
    ArgCollection args_;            // 参数集合
    
    /**
     * @brief 解析语法模式
     */
    void ParsePattern();
};

/**
 * @brief CJMOD扫描器接口
 */
class CJMODScanner {
public:
    /**
     * @brief 构造函数
     */
    CJMODScanner();
    
    /**
     * @brief 注册关键字扫描
     * @param keyword 关键字
     * @param handler 处理函数
     */
    void ScanKeyword(const std::string& keyword, std::function<void()> handler);
    
    /**
     * @brief 注册参数扫描
     * @param arg 参数对象
     * @param handler 处理函数
     */
    void ScanKeyword(const Arg& arg, std::function<void()> handler);
    
    /**
     * @brief 查看前后关键字
     * @param offset 偏移量（负数向前，正数向后）
     * @return 关键字内容
     */
    std::string PeekKeyword(int offset) const;
    
    /**
     * @brief 策略更改开始
     * @param trigger 触发字符
     * @param policy 新策略
     */
    void PolicyChangeBegin(const std::string& trigger, ScanPolicy policy);
    
    /**
     * @brief 策略更改结束
     * @param trigger 触发字符
     * @param policy 恢复策略
     * @return 收集的内容（如果策略为COLLECT）
     */
    std::string PolicyChangeEnd(const std::string& trigger, ScanPolicy policy);
    
    /**
     * @brief 检查是否为对象
     * @param content 内容
     * @return 是否为对象
     */
    static bool IsObject(const std::string& content);
    
    /**
     * @brief 检查是否为函数
     * @param content 内容
     * @return 是否为函数
     */
    static bool IsFunction(const std::string& content);

private:
    std::unordered_map<std::string, std::function<void()>> keywordHandlers_;  // 关键字处理器
    std::vector<std::string> currentTokens_;                                  // 当前Token序列
    size_t currentPosition_;                                                  // 当前位置
    ScanPolicy currentPolicy_;                                                // 当前策略
    std::string collectBuffer_;                                               // 收集缓冲区
};

/**
 * @brief 语法分析函数
 * @param pattern 语法模式字符串
 * @param ignoreChars 忽略的字符（可选）
 * @return 语法分析结果
 */
std::unique_ptr<Syntax> SyntaxAnalys(const std::string& pattern, const std::string& ignoreChars = "");

/**
 * @brief 代码生成函数
 * @param syntax 语法对象
 * @return 生成的JavaScript代码
 */
std::string GenerateCode(const Syntax& syntax);

/**
 * @brief CJMOD扩展基类
 * 
 * 所有CJMOD扩展都应该继承此类
 */
class CJMODExtension {
public:
    /**
     * @brief 虚析构函数
     */
    virtual ~CJMODExtension() = default;
    
    /**
     * @brief 初始化扩展
     * @param scanner 扫描器引用
     * @return 是否成功初始化
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
 * @brief CJMOD管理器
 */
class CJMODManager {
public:
    /**
     * @brief 注册CJMOD扩展
     * @param extension 扩展对象
     * @return 是否成功注册
     */
    bool RegisterExtension(std::unique_ptr<CJMODExtension> extension);
    
    /**
     * @brief 获取扫描器
     */
    CJMODScanner& GetScanner() { return scanner_; }
    
    /**
     * @brief 处理CHTL JS代码（完整实现）
     * @param source CHTL JS源代码
     * @return 处理后的JavaScript代码
     */
    std::string ProcessCHTLJS(const std::string& source);

private:
    /**
     * @brief 处理扩展语法
     * @param source 源代码
     * @param extension 扩展对象
     * @param syntaxPattern 语法模式
     * @return 处理后的代码
     */
    std::string ProcessExtensionSyntax(const std::string& source, 
                                      CJMODExtension* extension, 
                                      const std::string& syntaxPattern);
    
    /**
     * @brief 处理printMylove扩展（完整实现）
     * @param source 源代码
     * @param syntaxPattern 语法模式
     * @return 处理后的代码
     */
    std::string ProcessPrintMyloveExtension(const std::string& source, 
                                           const std::string& syntaxPattern);
    
    /**
     * @brief 处理iNeverAway扩展（完整实现）
     * @param source 源代码
     * @param syntaxPattern 语法模式
     * @return 处理后的代码
     */
    std::string ProcessINeverAwayExtension(const std::string& source, 
                                          const std::string& syntaxPattern);
    
    /**
     * @brief 处理通用扩展
     * @param source 源代码
     * @param extension 扩展对象
     * @param syntaxPattern 语法模式
     * @return 处理后的代码
     */
    std::string ProcessGenericExtension(const std::string& source, 
                                       CJMODExtension* extension, 
                                       const std::string& syntaxPattern);
    
    /**
     * @brief 解析并匹配printMylove参数
     * @param paramContent 参数内容
     * @param syntax 语法对象
     */
    void ParseAndMatchPrintMyloveParams(const std::string& paramContent, Syntax& syntax);
    
    /**
     * @brief 生成printMylove JavaScript代码
     * @param syntax 语法对象
     * @return 生成的JavaScript代码
     */
    std::string GeneratePrintMyloveJS(const Syntax& syntax);
    
    /**
     * @brief 生成iNeverAway JavaScript代码
     * @param objectName 虚对象名称
     * @param methodContent 方法内容
     * @param syntax 语法对象
     * @return 生成的JavaScript代码
     */
    std::string GenerateINeverAwayJS(const std::string& objectName, 
                                    const std::string& methodContent, 
                                    const Syntax& syntax);
    
    /**
     * @brief 处理虚对象调用
     * @param source 源代码
     * @return 处理后的代码
     */
    std::string ProcessVirtualObjectCalls(const std::string& source);
    
    /**
     * @brief 解析iNeverAway方法定义
     * @param methodContent 方法内容
     * @param methods 输出的方法列表
     */
    void ParseINeverAwayMethods(const std::string& methodContent, 
                               std::vector<std::pair<std::string, std::string>>& methods);
    
    /**
     * @brief 提取状态标识符
     * @param methodName 方法名称
     * @return 状态标识符
     */
    std::string ExtractStateIdentifier(const std::string& methodName);
    CJMODScanner scanner_;                                          // 扫描器
    std::vector<std::unique_ptr<CJMODExtension>> extensions_;       // 已注册的扩展
};

} // namespace CJMOD
} // namespace CHTL