#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <functional>
#include <memory>
#include <regex>
#include <queue>

namespace CHTL {
namespace CJMOD {

// 前向声明
class Args;
class Syntax;
class AutoFillScanner;
class DualPointerScanner;
class FrontExtractMechanism;

/**
 * @brief 参数填充策略枚举
 */
enum class FillStrategy {
    AUTO,           // 自动策略
    SEQUENTIAL,     // 顺序填充
    PATTERN_MATCH,  // 模式匹配
    TYPE_AWARE,     // 类型感知
    CONTEXT_AWARE   // 上下文感知
};

/**
 * @brief 参数类型识别枚举
 */
enum class ParamType {
    STRING,
    NUMBER,
    BOOLEAN,
    OBJECT,
    FUNCTION,
    ARRAY,
    URL,
    COLOR,
    SIZE,
    UNKNOWN
};

/**
 * @brief 扫描位置信息
 */
struct ScanPosition {
    size_t frontPointer;
    size_t backPointer;
    std::string scannedContent;
    bool hasKeyword;
    std::string detectedKeyword;
    
    ScanPosition() : frontPointer(0), backPointer(0), hasKeyword(false) {}
};

/**
 * @brief 参数候选项
 */
struct ParamCandidate {
    std::string value;
    ParamType type;
    double confidence;  // 置信度 0.0-1.0
    size_t sourcePosition;
    std::string context;
    
    ParamCandidate(const std::string& val = "", ParamType t = ParamType::UNKNOWN, 
                   double conf = 0.0, size_t pos = 0, const std::string& ctx = "")
        : value(val), type(t), confidence(conf), sourcePosition(pos), context(ctx) {}
};

/**
 * @brief 自动填充结果
 */
struct AutoFillResult {
    bool success;
    std::vector<std::string> filledValues;
    std::vector<ParamCandidate> candidates;
    std::string errorMessage;
    double overallConfidence;
    
    AutoFillResult() : success(false), overallConfidence(0.0) {}
};

/**
 * @brief Arg类 - 增强自动填充能力
 */
class Arg {
public:
    Arg(const std::string& name = "", bool isPlaceholder = false);
    
    // 保持原有功能
    std::string operator()(const std::string& peekedValue);
    
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
    
    void transform(const std::string& jsTemplate);
    void match(const std::string& rawValue);
    
    // 新增：自动填充相关功能
    void setExpectedType(ParamType type) { expectedType_ = type; }
    ParamType getExpectedType() const { return expectedType_; }
    
    void setPattern(const std::string& pattern) { pattern_ = pattern; }
    const std::string& getPattern() const { return pattern_; }
    
    void setAutoFillCandidate(const ParamCandidate& candidate);
    const ParamCandidate& getAutoFillCandidate() const { return autoFillCandidate_; }
    
    bool hasAutoFillCandidate() const { return autoFillCandidate_.confidence > 0.0; }
    bool acceptAutoFill(double minConfidence = 0.7);
    
    // 访问器
    const std::string& getJSCode() const { return jsCode_; }
    const std::string& getValue() const { return processedValue_; }
    const std::string& getName() const { return name_; }
    bool isPlaceholder() const { return isPlaceholder_; }
    bool hasBind() const { return hasBind_; }
    bool hasTransform() const { return !jsTemplate_.empty(); }

private:
    std::string name_;
    bool isPlaceholder_;
    bool hasBind_;
    std::string rawValue_;
    std::string processedValue_;
    std::string jsTemplate_;
    std::string jsCode_;
    std::function<std::string(const std::string&)> valueProcessor_;
    
    // 自动填充相关
    ParamType expectedType_;
    std::string pattern_;
    ParamCandidate autoFillCandidate_;
    
    void applyJSTransform();
};

/**
 * @brief Args类 - 增强自动填充支持
 */
class Args {
public:
    Args() = default;
    
    void addArg(const std::string& name, bool isPlaceholder = false);
    
    Arg& operator[](size_t index);
    const Arg& operator[](size_t index) const;
    
    // 保持链式调用
    template<typename T>
    Args& bind(const std::string& name, const std::function<std::string(T)>& processor) {
        if (name == "$") {
            for (auto& arg : args_) {
                if (arg.isPlaceholder() && !arg.hasBind()) {
                    arg.bind(processor);
                    placeholderCounter_++;
                    break;
                }
            }
        } else {
            Arg* arg = findArg(name);
            if (arg) {
                arg->bind(processor);
            }
        }
        return *this;
    }
    
    Args& transform(const std::string& name, const std::string& jsTemplate);
    Args& match(const std::string& name, const std::string& value);
    
    // 新增：自动填充功能
    Args& expectTypes(const std::vector<ParamType>& types);
    Args& setPatterns(const std::vector<std::string>& patterns);
    
    AutoFillResult autoFill(const std::vector<std::string>& sourceTokens, 
                           FillStrategy strategy = FillStrategy::AUTO);
    
    Args& acceptAutoFill(double minConfidence = 0.7);
    
    std::string result() const;
    size_t length() const { return args_.size(); }

private:
    std::vector<Arg> args_;
    std::unordered_map<std::string, size_t> nameToIndex_;
    size_t placeholderCounter_ = 0;
    
    Arg* findArg(const std::string& name);
};

/**
 * @brief 双指针扫描器 - 深度实现
 */
class DualPointerScanner {
public:
    DualPointerScanner();
    
    void setTokens(const std::vector<std::string>& tokens);
    void setKeywords(const std::vector<std::string>& keywords);
    
    ScanPosition scanForKeyword(const std::string& keyword);
    std::vector<ScanPosition> scanAllKeywords();
    
    std::vector<std::string> extractBetweenPointers(const ScanPosition& pos);
    std::string extractRange(size_t start, size_t end);
    
    void reset();
    
private:
    std::vector<std::string> tokens_;
    std::vector<std::string> keywords_;
    size_t frontPointer_;
    size_t backPointer_;
    size_t initialScanRange_;
    
    bool isKeyword(const std::string& token);
    void synchronizedMove();
    void collectFragment(const ScanPosition& pos);
};

/**
 * @brief 前置截取机制 - 深度实现
 */
class FrontExtractMechanism {
public:
    FrontExtractMechanism();
    
    void setExtractPatterns(const std::vector<std::string>& patterns);
    
    std::vector<std::string> extractFrontParameters(const std::string& fragment, 
                                                   const std::string& keyword);
    
    std::vector<std::string> extractByPattern(const std::string& fragment, 
                                             const std::string& pattern);
    
    bool needsFrontExtraction(const std::string& keyword);
    
private:
    std::vector<std::string> extractPatterns_;
    std::unordered_map<std::string, std::string> keywordPatterns_;
    
    void initializeDefaultPatterns();
};

/**
 * @brief 参数类型检测器
 */
class ParamTypeDetector {
public:
    static ParamType detectType(const std::string& value);
    static double calculateTypeConfidence(const std::string& value, ParamType expectedType);
    static bool isValidForType(const std::string& value, ParamType type);
    
private:
    static bool isNumber(const std::string& value);
    static bool isURL(const std::string& value);
    static bool isColor(const std::string& value);
    static bool isSize(const std::string& value);
    static bool isObject(const std::string& value);
    static bool isFunction(const std::string& value);
    static bool isArray(const std::string& value);
};

/**
 * @brief 智能参数匹配器
 */
class SmartParameterMatcher {
public:
    SmartParameterMatcher();
    
    std::vector<ParamCandidate> findCandidates(const std::vector<std::string>& tokens,
                                              const std::vector<ParamType>& expectedTypes,
                                              const std::vector<std::string>& patterns);
    
    std::vector<ParamCandidate> contextAwareMatch(const std::vector<std::string>& tokens,
                                                 const std::string& functionName,
                                                 const std::vector<std::string>& paramNames);
    
    void setStrategy(FillStrategy strategy) { strategy_ = strategy; }
    
private:
    FillStrategy strategy_;
    std::unordered_map<std::string, std::vector<ParamType>> functionTypeHints_;
    
    void initializeFunctionHints();
    double calculateContextConfidence(const ParamCandidate& candidate, 
                                    const std::string& context);
};

/**
 * @brief 自动填充扫描器 - 核心类
 */
class AutoFillScanner {
public:
    AutoFillScanner();
    
    void setTokens(const std::vector<std::string>& tokens);
    void setKeywords(const std::vector<std::string>& keywords);
    
    AutoFillResult performAutoFill(Args& args, const std::string& functionName = "");
    
    void setFillStrategy(FillStrategy strategy) { strategy_ = strategy; }
    void setMinConfidence(double confidence) { minConfidence_ = confidence; }
    
    // 深度集成双指针扫描和前置截取
    void enableDualPointerScanning(bool enable = true) { useDualPointer_ = enable; }
    void enableFrontExtraction(bool enable = true) { useFrontExtract_ = enable; }
    
private:
    std::unique_ptr<DualPointerScanner> dualPointer_;
    std::unique_ptr<FrontExtractMechanism> frontExtract_;
    std::unique_ptr<SmartParameterMatcher> matcher_;
    
    std::vector<std::string> tokens_;
    std::vector<std::string> keywords_;
    FillStrategy strategy_;
    double minConfidence_;
    bool useDualPointer_;
    bool useFrontExtract_;
    
    AutoFillResult performSequentialFill(Args& args);
    AutoFillResult performPatternMatchFill(Args& args);
    AutoFillResult performTypeAwareFill(Args& args);
    AutoFillResult performContextAwareFill(Args& args, const std::string& functionName);
    
    std::vector<std::string> extractRelevantTokens();
    void optimizeResults(AutoFillResult& result);
};

/**
 * @brief Syntax类 - 增强自动填充
 */
class Syntax {
public:
    Syntax() = default;
    
    Args args;
    
    // 保持原有链式调用
    template<typename T>
    Syntax& bind(const std::function<std::string(T)>& processor) {
        args.bind<T>("$", processor);
        return *this;
    }
    
    template<typename T>
    Syntax& bind(const std::string& name, const std::function<std::string(T)>& processor) {
        args.bind<T>(name, processor);
        return *this;
    }
    
    Syntax& transform(const std::string& jsTemplate) {
        args.transform("$", jsTemplate);
        return *this;
    }
    
    Syntax& transform(const std::string& name, const std::string& jsTemplate) {
        args.transform(name, jsTemplate);
        return *this;
    }
    
    Syntax& match(const std::string& value) {
        args.match("$", value);
        return *this;
    }
    
    Syntax& match(const std::string& name, const std::string& value) {
        args.match(name, value);
        return *this;
    }
    
    // 新增：自动填充链式调用
    Syntax& expectTypes(const std::vector<ParamType>& types) {
        args.expectTypes(types);
        return *this;
    }
    
    Syntax& setPatterns(const std::vector<std::string>& patterns) {
        args.setPatterns(patterns);
        return *this;
    }
    
    Syntax& autoFill(const std::vector<std::string>& sourceTokens, 
                    FillStrategy strategy = FillStrategy::AUTO) {
        AutoFillResult result = args.autoFill(sourceTokens, strategy);
        if (result.success) {
            std::cout << "✅ 自动填充成功，置信度: " << result.overallConfidence << std::endl;
        } else {
            std::cout << "❌ 自动填充失败: " << result.errorMessage << std::endl;
        }
        return *this;
    }
    
    Syntax& autoFillWithScanner(AutoFillScanner& scanner, const std::string& functionName = "") {
        AutoFillResult result = scanner.performAutoFill(args, functionName);
        if (result.success) {
            std::cout << "🚀 智能自动填充成功，置信度: " << result.overallConfidence << std::endl;
            for (size_t i = 0; i < result.filledValues.size(); ++i) {
                std::cout << "  参数[" << i << "]: " << result.filledValues[i] << std::endl;
            }
        } else {
            std::cout << "⚠️ 智能自动填充失败: " << result.errorMessage << std::endl;
        }
        return *this;
    }
    
    Syntax& acceptAutoFill(double minConfidence = 0.7) {
        args.acceptAutoFill(minConfidence);
        return *this;
    }
    
    std::string result() const { return args.result(); }
    
    Syntax& debug(const std::string& message = "") const;
};

/**
 * @brief CHTLJSFunction类 - 深度自动填充集成
 */
class CHTLJSFunction {
public:
    CHTLJSFunction(const std::string& functionName, 
                   const std::vector<std::string>& paramNames = {});
    
    // 保持原有功能
    template<typename T>
    CHTLJSFunction& bindKeyProcessor(const std::string& keyName, 
                                    const std::function<std::string(T)>& processor) {
        if (!syntax_) createSyntax();
        syntax_->args.bind(keyName, processor);
        return *this;
    }
    
    CHTLJSFunction& transformKey(const std::string& keyName, const std::string& jsTemplate);
    CHTLJSFunction& matchKeyword(const std::string& keyword, const std::string& value);
    
    // 新增：深度自动填充
    CHTLJSFunction& enableAutoFill(const std::vector<ParamType>& expectedTypes = {},
                                  const std::vector<std::string>& patterns = {},
                                  FillStrategy strategy = FillStrategy::AUTO);
    
    CHTLJSFunction& performAutoFill(const std::vector<std::string>& sourceTokens);
    
    CHTLJSFunction& smartFill(AutoFillScanner& scanner) {
        if (!syntax_) createSyntax();
        syntax_->autoFillWithScanner(scanner, functionName_);
        return *this;
    }
    
    std::unique_ptr<Syntax> createSyntax();
    std::string generateCode();
    
    const std::string& getFunctionName() const { return functionName_; }
    Syntax* getSyntax() const { return syntax_.get(); }

private:
    std::string functionName_;
    std::vector<std::string> paramNames_;
    std::unique_ptr<Syntax> syntax_;
    
    std::vector<ParamType> expectedTypes_;
    std::vector<std::string> patterns_;
    FillStrategy fillStrategy_;
    bool autoFillEnabled_;
};

// 全局函数
std::unique_ptr<Syntax> syntaxAnalys(const std::string& pattern, 
                                     const std::string& ignoreChars = ",:{};()");

std::unique_ptr<AutoFillScanner> createAutoFillScanner();
std::unique_ptr<CHTLJSFunction> createSmartCHTLJSFunction(
    const std::string& functionName,
    const std::vector<std::string>& paramNames = {},
    const std::vector<ParamType>& expectedTypes = {},
    FillStrategy strategy = FillStrategy::AUTO);

} // namespace CJMOD
} // namespace CHTL