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
    // 表面简单的接口 - 官方键值对版本
    static std::string generateImageProcessor(const std::string& configObject);
    
    // 内在精妙：智能参数处理
    static std::string processImageUrl(const std::string& url);
    static std::string validateMode(const std::string& mode);
    static std::string processDimension(const std::string& dimension);
    static std::string processConfigObject(const std::string& configStr);
    
private:
    static std::string generateASCIIConverter();
    static std::string generatePixelConverter();
};

} // namespace Chtholly

} // namespace CJMOD
} // namespace CHTL