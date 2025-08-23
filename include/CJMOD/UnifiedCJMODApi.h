#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <functional>
#include <variant>
#include <memory>

namespace CHTL {
namespace CJMOD {

// 参数值类型
using ArgValue = std::variant<std::string, int, double, bool>;

/**
 * @brief 扫描策略枚举
 */
enum class ScanStrategy {
    SLIDING_WINDOW,  // 双指针滑动窗口策略
    FRONT_EXTRACT    // 前置代码截取策略
};

/**
 * @brief Arg类 - 简化的参数处理
 * 核心理念：只保留最基本的参数绑定和转换功能
 */
class Arg {
public:
    Arg(const std::string& name = "");
    
    // 设置参数值
    Arg& operator=(const ArgValue& value);
    template<typename T>
    Arg& operator=(T value) {
        value_ = ArgValue(value);
        return *this;
    }
    
    // 获取参数值
    const ArgValue& GetValue() const { return value_; }
    std::string ToString() const;
    
    // 参数名称
    const std::string& GetName() const { return name_; }
    void SetName(const std::string& name) { name_ = name; }

private:
    std::string name_;
    ArgValue value_;
};

/**
 * @brief CHTLJSFunction类 - CHTL JS函数封装
 * 管理虚对象绑定和函数注册
 */
class CHTLJSFunction {
public:
    CHTLJSFunction(const std::string& name);
    
    // 添加参数定义
    void AddParam(const std::string& name, const std::string& defaultValue = "");
    
    // 设置函数体模板
    void SetBodyTemplate(const std::string& bodyTemplate);
    
    // 生成最终JavaScript代码
    std::string GenerateJS() const;
    
    // 虚对象支持
    void EnableVirSupport(bool enable = true) { virSupported_ = enable; }
    bool IsVirSupported() const { return virSupported_; }
    
    // 获取函数名
    const std::string& GetName() const { return name_; }

private:
    std::string name_;
    std::vector<std::pair<std::string, std::string>> params_; // name, defaultValue
    std::string bodyTemplate_;
    bool virSupported_;
};

/**
 * @brief Syntax类 - 统一管理核心
 * 这是整个CJMOD API的中心，管理所有语法处理、扫描、代码生成
 */
class Syntax {
public:
    Syntax();
    ~Syntax();
    
    // === 核心功能：语法分析和代码生成 ===
    
    /**
     * @brief 分析CHTL JS语法模式
     * @param pattern 语法模式，如 "functionName({key1: value1, key2: value2})"
     * @return 解析成功返回true
     */
    bool AnalyzePattern(const std::string& pattern);
    
    /**
     * @brief 设置参数值
     * @param name 参数名
     * @param value 参数值
     */
    void SetArg(const std::string& name, const ArgValue& value);
    
    /**
     * @brief 获取参数
     * @param name 参数名
     * @return 参数引用
     */
    Arg& GetArg(const std::string& name);
    
    /**
     * @brief 生成最终JavaScript代码
     * @return 生成的JS代码
     */
    std::string GenerateCode() const;
    
    // === 高级功能：自定义处理 ===
    
    /**
     * @brief 设置自定义转换规则
     * @param func 转换函数
     */
    void SetTransformRule(const std::function<std::string(const std::unordered_map<std::string, Arg>&)>& func);
    
    /**
     * @brief 注册CHTL JS函数
     * @param func 函数对象
     */
    void RegisterFunction(const CHTLJSFunction& func);
    
    // === 扫描器管理（统一在Syntax中） ===
    
    /**
     * @brief 设置扫描策略
     * @param strategy 扫描策略
     */
    void SetScanStrategy(ScanStrategy strategy);
    
    /**
     * @brief 扫描源代码并处理语法
     * @param source 源代码
     * @return 处理后的代码
     */
    std::string ScanAndProcess(const std::string& source);
    
    /**
     * @brief 双指针滑动窗口扫描
     * 前指针和后指针从0开始，一同向前移动
     * 前指针遇到关键字时通知后指针进入收集状态
     */
    std::string SlidingWindowScan(const std::string& source);
    
    /**
     * @brief 前置代码截取扫描
     * 从前面截取代码，避免错误送入编译器
     */
    std::string FrontExtractScan(const std::string& source);
    
    // === 虚对象（vir）管理 ===
    
    /**
     * @brief 绑定函数到虚对象支持列表
     * @param functionName 函数名
     */
    void BindVirFunction(const std::string& functionName);
    
    /**
     * @brief 检查函数是否支持vir
     * @param functionName 函数名
     * @return 是否支持
     */
    bool IsVirSupported(const std::string& functionName) const;
    
    // === 调试和实用功能 ===
    
    /**
     * @brief 获取已注册的函数列表
     */
    std::vector<std::string> GetRegisteredFunctions() const;
    
    /**
     * @brief 获取当前参数列表
     */
    std::vector<std::string> GetArgNames() const;
    
    /**
     * @brief 清空当前状态
     */
    void Clear();

private:
    // 内部状态
    std::unordered_map<std::string, Arg> args_;
    std::unordered_map<std::string, CHTLJSFunction> functions_;
    std::unordered_set<std::string> virSupportedFunctions_;
    
    // 扫描器状态
    ScanStrategy scanStrategy_;
    std::string currentPattern_;
    std::function<std::string(const std::unordered_map<std::string, Arg>&)> customTransform_;
    
    // 内部方法
    bool ParseCHTLJSSyntax(const std::string& pattern);
    std::string DefaultTransform() const;
    void InitializeScanner();
    
    // 滑动窗口扫描内部状态
    struct ScannerState {
        size_t frontPointer;
        size_t backPointer;
        bool collectMode;
        std::string collectBuffer;
        std::vector<std::string> keywords;
    };
    
    ScannerState scannerState_;
    
    // 扫描器辅助方法
    void ResetScannerState();
    bool DetectKeywordAt(const std::string& source, size_t position, std::string& keyword);
    std::string ExtractAndProcess(const std::string& source, size_t start, size_t end);
    void InitialScan(const std::string& source); // 起始扫描避免语法片段位于开头
};

// === 便捷函数（全局接口） ===

/**
 * @brief 快速创建CHTL JS函数
 * @param name 函数名
 * @param keyNames 键名列表
 * @return CHTLJSFunction对象
 */
CHTLJSFunction CreateCHTLJSFunction(const std::string& name, const std::vector<std::string>& keyNames = {});

/**
 * @brief 快速语法处理
 * @param pattern 语法模式
 * @param args 参数映射
 * @return 生成的代码
 */
std::string QuickProcess(const std::string& pattern, const std::unordered_map<std::string, ArgValue>& args = {});

/**
 * @brief 绑定函数到vir支持
 * @param functionName 函数名
 */
void VirFunctionBind(const std::string& functionName);

/**
 * @brief 设置全局扫描策略
 * @param strategy 扫描策略
 */
void SetGlobalScanStrategy(ScanStrategy strategy);

} // namespace CJMOD
} // namespace CHTL