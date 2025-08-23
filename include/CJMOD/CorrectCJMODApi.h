#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <functional>
#include <memory>

namespace CHTL {
namespace CJMOD {

/**
 * @brief 双指针扫描机制的正确实现
 * 
 * 关键特点：
 * 1. 前指针和后指针都从位置0开始
 * 2. 先扫描初始范围确定不存在语法片段
 * 3. 前指针移动到合适位置后，前后指针同步向前
 * 4. 前指针遇到扫描关键字时通知后指针收集
 * 5. 能够收集到完整的代码片段
 */
class DualPointerScanner {
public:
    /**
     * @brief 构造函数
     * @param source 源代码
     * @param keywords CJMOD设置的扫描关键字列表
     */
    DualPointerScanner(const std::string& source, const std::vector<std::string>& keywords);
    
    /**
     * @brief 执行双指针扫描
     * @return 扫描结果：普通代码片段和CJMOD片段的分离
     */
    struct ScanResult {
        std::vector<std::string> normalCodeSegments;  // 普通代码片段
        std::vector<std::string> cjmodCodeSegments;   // CJMOD代码片段
        std::vector<size_t> cjmodPositions;           // CJMOD片段在原代码中的位置
    };
    
    ScanResult scan();
    
private:
    std::string source_;
    std::vector<std::string> keywords_;
    size_t frontPtr_;   // 前指针：寻找CJMOD语法
    size_t backPtr_;    // 后指针：标记已处理位置
    bool isCollecting_; // 是否正在收集模式
    
    // 扫描初始范围，确定不存在语法片段
    bool scanInitialRange(size_t start, size_t end);
    
    // 检测前指针位置是否有CJMOD关键字
    bool detectKeywordAtFrontPtr();
    
    // 同步向前移动双指针
    void movePtrsSynchronously();
    
    // 前指针通知后指针进行收集
    void startCollection();
    
    // 完成收集
    std::string endCollection();
};

/**
 * @brief 前置截取机制的正确实现
 * 
 * 解决问题：
 * 对于 "arg ** arg" 这样的语法，统一扫描器扫描到"**"时才启动CJMOD收集，
 * 但第一个"arg"已经被包括在统一扫描器的代码片段中，
 * 需要截取这部分避免发送给编译器
 */
class FrontExtractMechanism {
public:
    /**
     * @brief 构造函数
     * @param triggerKeyword 触发关键字（如"**"）
     * @param extractPattern 需要前置截取的模式（如"arg"）
     */
    FrontExtractMechanism(const std::string& triggerKeyword, 
                         const std::string& extractPattern);
    
    /**
     * @brief 执行前置截取
     * @param codeSegment 统一扫描器收集的代码片段
     * @return 截取结果
     */
    struct ExtractResult {
        std::string frontPart;      // 前置部分（需要截取的）
        std::string remainingPart;  // 剩余部分（发送给编译器）
        bool hasExtraction;         // 是否有截取内容
    };
    
    ExtractResult extract(const std::string& codeSegment);
    
private:
    std::string triggerKeyword_;
    std::string extractPattern_;
    
    // 在代码片段中寻找触发关键字的位置
    size_t findTriggerPosition(const std::string& segment);
    
    // 从触发位置向前截取匹配模式
    std::string extractFrontPattern(const std::string& segment, size_t triggerPos);
};

// 前向声明
class Arg;
class Args;
class Syntax;

/**
 * @brief Arg类 - 完全按照原生API设计
 */
class Arg {
public:
    Arg(const std::string& name, bool isPlaceholder = false);
    
    // 核心特性：operator()重载
    std::string operator()(const std::string& value);
    
    // 智能bind函数处理
    template<typename T>
    void bind(const std::function<std::string(T)>& func) {
        bindFunction_ = [func](const std::string& value) -> std::string {
            if constexpr (std::is_same_v<T, std::string>) {
                return func(value);
            } else if constexpr (std::is_arithmetic_v<T>) {
                try {
                    if constexpr (std::is_integral_v<T>) {
                        T num = static_cast<T>(std::stoll(value));
                        return func(num);
                    } else {
                        T num = static_cast<T>(std::stod(value));
                        return func(num);
                    }
                } catch (...) {
                    return func(T{});
                }
            }
            return value;
        };
        hasBind_ = true;
    }
    
    void match(const std::string& value);
    const std::string& transform(const std::string& jsCode);
    
    // 访问器
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
};

/**
 * @brief Args类 - 完全按照原生API设计
 */
class Args {
public:
    Args() = default;
    
    void addArg(const std::string& name, bool isPlaceholder = false);
    
    // 索引访问：keyword.args[0]
    Arg& operator[](size_t index);
    const Arg& operator[](size_t index) const;
    
    // 占位符重复绑定：args.bind("$", func)
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
    
    void match(const std::string& name, const std::string& value);
    void transform(const std::string& name, const std::string& jsTemplate);
    std::string result() const;
    size_t length() const { return args_.size(); }

private:
    std::vector<Arg> args_;
    std::unordered_map<std::string, size_t> nameToIndex_;
    size_t placeholderCounter_ = 0;
    
    Arg* findArg(const std::string& name);
};

/**
 * @brief Syntax类 - 完全按照原生API设计
 */
class Syntax {
public:
    Syntax() = default;
    
    Args args;  // 便捷访问：keyword.args
    std::string result() const { return args.result(); }

private:
    std::string pattern_;
};

/**
 * @brief CHTLJSFunction类 - 重构版本
 * 基于正确的双指针扫描和前置截取机制
 */
class CHTLJSFunction {
public:
    /**
     * @brief 构造函数
     * @param functionName CHTL JS函数名
     * @param keyNames 参数键名列表
     * @param useFrontExtract 是否启用前置截取机制
     */
    CHTLJSFunction(const std::string& functionName, 
                   const std::vector<std::string>& keyNames,
                   bool useFrontExtract = false);
    
    /**
     * @brief 绑定键处理器（支持前置截取）
     */
    template<typename T>
    void bindKeyProcessor(const std::string& keyName, 
                         const std::function<std::string(T)>& processor) {
        if (keyProcessors_.find(keyName) == keyProcessors_.end()) {
            keyProcessors_[keyName] = [processor](const std::string& value) -> std::string {
                if constexpr (std::is_same_v<T, std::string>) {
                    return processor(value);
                } else if constexpr (std::is_arithmetic_v<T>) {
                    try {
                        if constexpr (std::is_integral_v<T>) {
                            T num = static_cast<T>(std::stoll(value));
                            return processor(num);
                        } else {
                            T num = static_cast<T>(std::stod(value));
                            return processor(num);
                        }
                    } catch (...) {
                        return processor(T{});
                    }
                }
                return value;
            };
        }
    }
    
    /**
     * @brief 设置前置截取规则
     * @param triggerKeyword 触发关键字
     * @param extractPattern 截取模式
     */
    void setFrontExtractRule(const std::string& triggerKeyword, 
                            const std::string& extractPattern);
    
    /**
     * @brief 执行标准CJMOD流程（带双指针扫描和前置截取）
     */
    std::unique_ptr<Syntax> createSyntax();
    std::string processWithDualPointer(const std::string& sourceCode);
    std::string generateCode(const Syntax& syntax);

private:
    std::string functionName_;
    std::vector<std::string> keyNames_;
    bool useFrontExtract_;
    std::unordered_map<std::string, std::function<std::string(const std::string&)>> keyProcessors_;
    
    // 双指针扫描器
    std::unique_ptr<DualPointerScanner> scanner_;
    
    // 前置截取机制
    std::unique_ptr<FrontExtractMechanism> frontExtractor_;
    
    // 生成语法模式字符串
    std::string generateSyntaxPattern();
};

/**
 * @brief 全局函数 - 完全按照原生API设计
 */

// 核心语法分析函数
std::unique_ptr<Syntax> syntaxAnalys(const std::string& pattern, 
                                    const std::string& ignoreChars = "");

// 智能代码生成
std::string generateCode(const Syntax& syntax);

/**
 * @brief 扫描策略配置
 */
void configureDualPointerScanning(const std::vector<std::string>& keywords);
void enableFrontExtractMechanism(const std::string& triggerKeyword, 
                                const std::string& extractPattern);

} // namespace CJMOD
} // namespace CHTL