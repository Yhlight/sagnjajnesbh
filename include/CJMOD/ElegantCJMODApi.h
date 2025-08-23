#pragma once

#include <string>
#include <vector>
#include <functional>
#include <memory>
#include <unordered_map>
#include <variant>

namespace CHTL {
namespace CJMOD {

// ==========================================
// 优雅CJMOD API - 表面简单，内在精妙
// 严格按照原始API设计哲学：
// 1. 用户接口极简直观
// 2. 内部实现精妙高效
// 3. 自动化流程智能处理
// ==========================================

// 前向声明
class Keyword;
class ArgCollection;
class CJMODScanner;

// 参数值类型
using ArgValue = std::variant<std::string, int, double, bool>;

// 策略模式枚举
enum class Policy {
    NORMAL,     // 正常扫描
    COLLECT,    // 收集模式
    SKIP,       // 跳过模式
    DEEP        // 深度扫描
};

/**
 * @brief 参数类 - 表面简单的参数，内在智能的处理
 */
class Arg {
public:
    Arg(const std::string& name, bool isPlaceholder = false);
    
    // 表面简单的接口
    const std::string& getName() const { return name_; }
    bool isPlaceholder() const { return isPlaceholder_; }
    const std::string& getValue() const { return processedValue_; }
    
    // 内在精妙：智能绑定处理函数
    template<typename T>
    void bind(std::function<std::string(const T&)> processor);
    
    // 内在精妙：智能匹配和处理
    void match(const ArgValue& value);
    
private:
    std::string name_;
    bool isPlaceholder_;
    std::string rawValue_;
    std::string processedValue_;
    
    // 内在精妙：多类型处理器
    std::function<std::string(const std::string&)> stringProcessor_;
    std::function<std::string(int)> intProcessor_;
    std::function<std::string(double)> doubleProcessor_;
    std::function<std::string(bool)> boolProcessor_;
    
    void processValue();
};

/**
 * @brief 参数集合类 - 表面简单的集合，内在智能的管理
 */
class ArgCollection {
public:
    // 表面简单的访问接口
    size_t length() const { return args_.size(); }
    const Arg& operator[](size_t index) const;
    Arg& operator[](size_t index);
    
    // 表面简单的绑定接口，内在支持智能类型推导
    template<typename T>
    void bind(const std::string& name, std::function<std::string(const T&)> processor);
    
    // 表面简单的匹配接口
    void match(const std::string& name, const ArgValue& value);
    void match(size_t index, const ArgValue& value);
    
    // 表面简单的结果接口，内在智能组合
    std::string result() const;
    
    // 内部管理方法
    void addArg(const std::string& name, bool isPlaceholder = false);
    Arg* findArg(const std::string& name);
    
private:
    std::vector<std::unique_ptr<Arg>> args_;
    std::unordered_map<std::string, size_t> nameIndex_;
};

/**
 * @brief 关键字类 - 表面简单的关键字，内在精妙的语法结构
 */
class Keyword {
public:
    Keyword(const std::string& pattern);
    
    // 表面简单的访问接口
    ArgCollection args;
    const std::string& getPattern() const { return pattern_; }
    
    // 内在精妙：智能模式匹配
    bool matches(const std::string& code) const;
    
private:
    std::string pattern_;
    void parsePattern();
};

/**
 * @brief CJMOD扫描器 - 表面简单的扫描，内在精妙的状态机
 */
class CJMODScanner {
public:
    CJMODScanner();
    
    // 表面简单的扫描接口
    void scanKeyword(const Arg& keywordArg, std::function<void()> handler);
    void scanKeyword(const std::string& keyword, std::function<void()> handler);
    
    // 表面简单的查看接口，内在智能的位置计算
    std::string peekKeyword(int offset) const;
    std::string currentKeyword() const;
    
    // 表面简单的策略接口，内在精妙的状态栈
    void policyChangeBegin(const std::string& startMarker, Policy policy);
    std::string policyChangeEnd(const std::string& endMarker, Policy returnPolicy);
    
    // 内部状态管理
    void setSourceCode(const std::string& code);
    void reset();
    
private:
    std::string sourceCode_;
    size_t currentPosition_;
    std::vector<std::string> tokens_;
    std::vector<Policy> policyStack_;
    Policy currentPolicy_;
    
    // 内在精妙：智能词法分析
    void tokenize();
    void advancePosition();
    std::string extractToken(size_t position) const;
};

// ==========================================
// CHTL JS函数快速创建系统
// ==========================================

/**
 * @brief CHTL JS函数对象 - 托管CJMOD标准流程的部分步骤
 * 
 * 设计理念：
 * - 托管语法创建：自动生成syntaxAnalys所需的语法模式
 * - 托管参数绑定：简化bind步骤的重复代码
 * - 保持标准流程：仍需要手动执行scanKeyword、match、generateCode
 * - 完全兼容：返回标准Keyword对象，可无缝接入现有CJMOD代码
 */
class CHTLJSFunction {
public:
    CHTLJSFunction(const std::string& functionName, const std::vector<std::string>& keyNames);
    
    // 托管的步骤：简化语法创建和参数绑定
    void bindKeyProcessor(const std::string& keyName, std::function<std::string(const std::string&)> processor);
    void setDefaultValues(const std::unordered_map<std::string, std::string>& defaults);
    void enableCHTLJSFeatures(bool unordered = true, bool optional = true, bool undecoratedLiterals = true);
    
    // 标准流程接口：开发者仍需手动调用标准CJMOD步骤
    std::unique_ptr<Keyword>& getKeyword() { return keyword_; }
    const std::unique_ptr<Keyword>& getKeyword() const { return keyword_; }
    
    // 辅助方法：帮助处理配置对象（可选使用）
    std::string processConfigObject(const std::string& configStr);
    
    // 获取函数信息
    const std::string& getFunctionName() const { return functionName_; }
    const std::vector<std::string>& getKeyNames() const { return keyNames_; }
    
private:
    std::string functionName_;
    std::vector<std::string> keyNames_;
    std::unique_ptr<Keyword> keyword_;
    std::unordered_map<std::string, std::function<std::string(const std::string&)>> keyProcessors_;
    std::unordered_map<std::string, std::string> defaultValues_;
    bool supportUnordered_;
    bool supportOptional_;
    bool supportUndecoratedLiterals_;
    
    void initializeKeyword();
    std::string generateSyntaxPattern();
};

/**
 * @brief 快速创建CHTL JS函数 - 简化CJMOD开发流程
 * 
 * 使用示例：
 * ```cpp
 * // CHTL JS函数天然支持vir，无需区分类型
 * auto printFunc = createCHTLJSFunction("printMylove", {"url", "mode"});
 * auto virFunc = createCHTLJSFunction("iNeverAway", {"MyPromise"});
 * 
 * // 使用时可以选择是否使用vir：
 * // printMylove({...});              ← 直接调用
 * // const result = printMylove({...}); ← 赋值调用
 * // vir myVir = iNeverAway({...});   ← 虚对象调用（优化支持）
 * ```
 * 
 * @param functionName CHTL JS函数名称
 * @param keyNames 键名列表
 * @return CHTLJSFunction对象，天然支持vir，可以继续使用标准CJMOD流程
 */
std::unique_ptr<CHTLJSFunction> createCHTLJSFunction(const std::string& functionName, 
                                                   const std::vector<std::string>& keyNames);

// ==========================================
// vir函数注册表 - 管理哪些函数支持vir语法
// ==========================================

/**
 * @brief CHTL JS虚对象函数注册表
 * 
 * 管理哪些CHTL JS函数支持vir语法，解决统一扫描器识别问题：
 * - 使用createCHTLJSFunction创建的函数自动注册vir支持
 * - 使用标准CJMOD流程创建的函数默认不支持vir，需手动注册
 * - 统一扫描器在处理vir时查询此注册表进行验证
 */
class CHTLJSVirRegistry {
public:
    /**
     * @brief 注册函数的vir支持
     * @param functionName 函数名称
     * @param supportedKeys 支持的键名列表
     */
    static void registerFunction(const std::string& functionName, 
                               const std::vector<std::string>& supportedKeys);
    
    /**
     * @brief 检查函数是否支持vir语法
     * @param functionName 函数名称
     * @return true如果支持vir，false否则
     */
    static bool isSupported(const std::string& functionName);
    
    /**
     * @brief 获取函数支持的键名列表
     * @param functionName 函数名称
     * @return 支持的键名列表，如果函数不支持vir则返回空列表
     */
    static std::vector<std::string> getSupportedKeys(const std::string& functionName);
    
    /**
     * @brief 获取所有支持vir的函数列表
     * @return 支持vir的函数名称列表
     */
    static std::vector<std::string> getAllSupportedFunctions();
    
    /**
     * @brief 清空注册表（主要用于测试）
     */
    static void clear();

private:
    static std::unordered_map<std::string, std::vector<std::string>> virFunctions_;
};

/**
 * @brief 为标准CJMOD流程提供vir注册接口
 * 
 * 当使用标准CJMOD流程（syntaxAnalys + bind + scanKeyword + match + generateCode）
 * 创建CHTL JS函数时，可以调用此函数手动注册vir支持
 * 
 * @param functionName 函数名称
 * @param supportedKeys 支持的键名列表
 */
void registerCJMODFunctionForVir(const std::string& functionName, 
                                const std::vector<std::string>& supportedKeys);

// ==========================================
// CJMOD关键字处理系统 - 统一扫描器集成
// ==========================================

/**
 * @brief CJMOD关键字处理信息
 */
struct CJMODKeywordInfo {
    std::string keyword;                    // 关键字
    bool needsBacktrack;                   // 是否需要回退
    size_t backtrackDistance;             // 回退距离（单元数）
    bool needsForwardCollect;             // 是否需要向前收集
    size_t forwardCollectDistance;        // 向前收集距离
    std::function<void()> handler;        // 处理函数
    
    CJMODKeywordInfo() : needsBacktrack(false), backtrackDistance(0), 
                        needsForwardCollect(true), forwardCollectDistance(1) {}
};

/**
 * @brief CJMOD关键字处理器
 * 
 * 管理CJMOD关键字的识别和处理策略，支持：
 * - 关键字注册和查询
 * - 回退机制配置
 * - 向前收集配置
 * - 统一扫描器集成接口
 */
class CJMODKeywordHandler {
public:
    /**
     * @brief 注册CJMOD关键字
     * @param keyword 关键字
     * @param info 处理信息
     */
    static void registerKeyword(const std::string& keyword, const CJMODKeywordInfo& info);
    
    /**
     * @brief 检查是否为CJMOD关键字
     * @param keyword 待检查的关键字
     * @return true如果是CJMOD关键字，false否则
     */
    static bool isCJMODKeyword(const std::string& keyword);
    
    /**
     * @brief 获取关键字处理信息
     * @param keyword 关键字
     * @return 处理信息，如果关键字不存在则返回默认信息
     */
    static CJMODKeywordInfo getKeywordInfo(const std::string& keyword);
    
    /**
     * @brief 处理关键字
     * @param keyword 关键字
     * @return true如果处理成功，false否则
     */
    static bool handleKeyword(const std::string& keyword);
    
    /**
     * @brief 获取所有注册的关键字
     * @return 关键字列表
     */
    static std::vector<std::string> getAllKeywords();
    
    /**
     * @brief 清空所有注册的关键字（主要用于测试）
     */
    static void clear();

private:
    static std::unordered_map<std::string, CJMODKeywordInfo> keywordMap_;
};

/**
 * @brief 统一扫描器集成接口
 * 
 * 提供统一扫描器与CJMOD的集成接口，支持：
 * - 关键字识别和拦截
 * - 回退和向前收集
 * - 上下文管理
 */
class CJMODScannerIntegration {
public:
    /**
     * @brief 处理统一扫描器识别到的关键字
     * @param keyword 关键字
     * @param context 扫描器上下文
     * @return 处理结果
     */
    static bool processKeywordFromScanner(const std::string& keyword, void* context);
    
    /**
     * @brief 注册常用的CJMOD关键字
     */
    static void registerCommonKeywords();
    
    /**
     * @brief 初始化CJMOD扫描器集成
     */
    static void initialize();
};

// ==========================================
// vir机制限制系统 - 禁止直接vir语法
// ==========================================

/**
 * @brief vir函数绑定器
 * 
 * 替代直接vir语法，通过编程方式绑定函数的vir支持
 * 避免vir语法对统一扫描器造成负担
 */
class VirFunctionBinder {
public:
    /**
     * @brief 绑定函数的vir支持
     * @param functionName 函数名称
     * @param keyMappings 键映射表：键名 -> 全局函数名
     * @return 绑定是否成功
     */
    static bool bind(const std::string& functionName, 
                    const std::unordered_map<std::string, std::string>& keyMappings);
    
    /**
     * @brief 检查函数是否已绑定vir支持
     * @param functionName 函数名称
     * @return true如果已绑定，false否则
     */
    static bool isBound(const std::string& functionName);
    
    /**
     * @brief 获取函数的vir键映射
     * @param functionName 函数名称
     * @return 键映射表，如果未绑定则返回空
     */
    static std::unordered_map<std::string, std::string> getKeyMappings(const std::string& functionName);
    
    /**
     * @brief 生成vir对象访问代码
     * @param functionName 函数名称
     * @param objectName 虚对象名称
     * @param keyAccess 键访问路径（如 "MyPromise.Happy"）
     * @return 生成的JS访问代码
     */
    static std::string generateVirAccess(const std::string& functionName,
                                        const std::string& objectName,
                                        const std::string& keyAccess);
    
    /**
     * @brief 清空所有vir绑定（主要用于测试）
     */
    static void clear();

private:
    static std::unordered_map<std::string, std::unordered_map<std::string, std::string>> virBindings_;
};

/**
 * @brief vir语法检测器
 * 
 * 检测并拦截开发者直接使用vir语法的行为
 */
class VirSyntaxDetector {
public:
    /**
     * @brief 检测代码中是否包含直接vir语法
     * @param code 源代码
     * @return 检测结果
     */
    static bool detectDirectVirUsage(const std::string& code);
    
    /**
     * @brief 生成vir语法替代建议
     * @param virStatement vir语句
     * @return 替代建议
     */
    static std::string generateAlternativeSuggestion(const std::string& virStatement);
    
    /**
     * @brief 处理检测到的vir违规
     * @param code 源代码
     * @return 处理结果和建议
     */
    static std::string handleVirViolation(const std::string& code);
};

// ==========================================
// 扫描策略系统 - 智能选择扫描算法
// ==========================================

/**
 * @brief 扫描策略类型
 */
enum class ScanStrategy {
    DUAL_POINTER,      // 双指针扫描（默认）
    BACKTRACK,         // 回退重拼接
    HYBRID,            // 混合策略
    ADAPTIVE           // 自适应策略
};

/**
 * @brief 扫描上下文
 */
struct ScanContext {
    size_t currentPosition;
    size_t tokenCount;
    int nestingLevel;
    double memoryPressure;
    std::string currentKeyword;
    
    ScanContext() : currentPosition(0), tokenCount(0), nestingLevel(0), memoryPressure(0.0) {}
};

/**
 * @brief 扫描结果
 */
struct ScanResult {
    bool success;
    double executionTime;
    size_t memoryUsage;
    std::string processedCode;
    
    ScanResult() : success(false), executionTime(0.0), memoryUsage(0) {}
};

/**
 * @brief 扫描策略管理器
 * 
 * 管理不同的扫描策略，根据情况智能选择最优策略
 */
class ScanStrategyManager {
public:
    /**
     * @brief 选择扫描策略
     * @param keyword 关键字
     * @param context 上下文信息
     * @return 选择的策略
     */
    static ScanStrategy selectStrategy(const std::string& keyword, const ScanContext& context);
    
    /**
     * @brief 执行扫描策略
     * @param strategy 策略类型
     * @param keyword 关键字
     * @param context 上下文
     * @return 执行结果
     */
    static ScanResult executeStrategy(ScanStrategy strategy, 
                                    const std::string& keyword, 
                                    ScanContext& context);
    
    /**
     * @brief 设置默认策略
     * @param strategy 默认策略
     */
    static void setDefaultStrategy(ScanStrategy strategy);
    
    /**
     * @brief 获取当前默认策略
     * @return 默认策略
     */
    static ScanStrategy getDefaultStrategy();
    
    /**
     * @brief 初始化策略系统
     */
    static void initialize();

private:
    static ScanStrategy defaultStrategy_;
    static std::map<ScanStrategy, size_t> strategyUsageCount_;
    static std::map<ScanStrategy, double> strategyPerformance_;
};

/**
 * @brief 关键字复杂度分析器
 */
class KeywordComplexityAnalyzer {
public:
    /**
     * @brief 计算关键字处理复杂度
     * @param keyword 关键字
     * @return 复杂度评分（1-10）
     */
    static int calculateComplexity(const std::string& keyword);
    
    /**
     * @brief 推荐最优策略
     * @param keyword 关键字
     * @param context 上下文
     * @return 推荐的策略
     */
    static ScanStrategy recommendStrategy(const std::string& keyword, const ScanContext& context);
};

// ==========================================
// 核心API函数 - 表面简单，内在精妙
// ==========================================

/**
 * @brief 语法分析 - 表面简单的接口，内在精妙的模式解析
 * @param pattern 语法模式字符串
 * @param ignoreChars 忽略的字符
 * @return 智能解析的关键字对象
 */
std::unique_ptr<Keyword> syntaxAnalys(const std::string& pattern, const std::string& ignoreChars = "");

/**
 * @brief 获取全局扫描器 - 表面简单的访问，内在智能的单例管理
 */
CJMODScanner& getCJMODScanner();

/**
 * @brief 代码生成 - 表面简单的调用，内在智能的组合
 * @param keyword 关键字对象
 * @return 生成的JavaScript代码
 */
std::string generateCode(const Keyword& keyword);

// ==========================================
// 模板特化实现
// ==========================================

template<typename T>
void Arg::bind(std::function<std::string(const T&)> processor) {
    if constexpr (std::is_same_v<T, std::string>) {
        stringProcessor_ = processor;
    } else if constexpr (std::is_same_v<T, int>) {
        intProcessor_ = processor;
    } else if constexpr (std::is_same_v<T, double>) {
        doubleProcessor_ = processor;
    } else if constexpr (std::is_same_v<T, bool>) {
        boolProcessor_ = processor;
    }
}

template<typename T>
void ArgCollection::bind(const std::string& name, std::function<std::string(const T&)> processor) {
    Arg* arg = findArg(name);
    if (arg) {
        arg->bind<T>(processor);
    }
}

// ==========================================
// 珂朵莉模块专用扩展 - 修正版
// ==========================================

namespace Chtholly {

/**
 * @brief iNeverAway函数实现 - 支持任意自定义键的承诺函数系统
 * 
 * 设计理念：
 * 1. 虚对象(vir)是CHTL JS原生功能，不是CJMOD创造的
 * 2. 自定义键可以是任意符合命名规范的名称，不限于Void
 * 3. 状态标记<>是可选的，开发者可以自由决定
 * 4. CJMOD具有极高自由度，可以包含头文件、定义全局变量
 */
class INeverAwaySystem {
public:
    // 表面简单的接口
    static void registerPromiseFunction(const std::string& name, const std::string& jsCode);
    static std::string generatePromiseCall(const std::string& message, int duration = 60);
    
    // 自定义键支持 - 任意键名，任意状态
    static void defineCustomKey(const std::string& keyName, const std::string& state = "", const std::string& jsTemplate = "");
    static std::string processCustomKeys(const std::string& virObjectCode);
    
    // 全局状态管理 - 利用CJMOD的高自由度
    static void initializeGlobalState();
    static void registerGlobalFunction(const std::string& functionName, const std::string& jsCode);
    static std::string getGlobalFunctionName(const std::string& keyName, const std::string& state = "");
    
    // 虚对象处理 - CHTL JS原生功能的扩展支持
    static std::string processVirObject(const std::string& virName, const std::string& objectContent);
    
private:
    static std::unordered_map<std::string, std::string> promiseFunctions_;
    static std::unordered_map<std::string, std::string> customKeys_;
    
    // 全局状态管理 - CJMOD的高自由度特性
    static std::unordered_map<std::string, std::string> globalFunctions_;
    static std::unordered_map<std::string, std::string> keyStateMapping_;
    static int globalFunctionCounter_;
    static bool isInitialized_;
};

/**
 * @brief printMylove函数实现 - 图片转字符像素块系统
 */
class PrintMyloveSystem {
public:
    // 表面简单的接口 - 官方键值对版本（支持无序、可选、无修饰字面量）
    static std::string generateImageProcessor(const std::string& configObject);
    
    // 内在精妙：智能参数处理
    static std::string processImageUrl(const std::string& url);
    static std::string validateMode(const std::string& mode);
    static std::string processDimension(const std::string& dimension);
    static std::string processConfigObject(const std::string& configStr);
    
    // CHTL JS官方特性支持
    static std::string parseUnorderedKeyValue(const std::string& configStr);
    static std::string handleOptionalKeys(const std::string& configStr);
    static std::string processUndecoratedLiterals(const std::string& value);
    
private:
    static std::string generateASCIIConverter();
    static std::string generatePixelConverter();
};

} // namespace Chtholly

} // namespace CJMOD
} // namespace CHTL