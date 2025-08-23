#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <functional>
#include <memory>

namespace CHTL {
namespace CJMOD {

// 前向声明
class Arg;
class Syntax;
class CHTLJSFunction;
class CJMODScanner;
class UnifiedScanner;  // 统一扫描器
class CodeGenerator;   // 代码生成器

/**
 * @brief 扫描策略枚举
 */
enum class Policy {
    NORMAL,     // 常规模式
    COLLECT,    // 收集模式 - 收集两个关键字之间的内容
    SKIP        // 跳过模式
};

/**
 * @brief 代码片段结构 - 统一扫描器返回的结果
 */
struct CodeFragment {
    std::string originalCode;       // 原始代码片段
    std::vector<std::string> tokens; // 分词结果
    std::string triggerKeyword;     // 触发的关键字
    size_t keywordPosition;         // 关键字在片段中的位置
    
    CodeFragment() : keywordPosition(0) {}
};

/**
 * @brief 双指针扫描位置信息
 */
struct ScanPosition {
    size_t frontPointer;    // 前指针位置
    size_t backPointer;     // 后指针位置
    std::string content;    // 扫描到的内容
    bool hasKeyword;        // 是否检测到关键字
    
    ScanPosition() : frontPointer(0), backPointer(0), hasKeyword(false) {}
};

/**
 * @brief 前置截取结果
 */
struct ExtractResult {
    std::string extracted;      // 截取的内容
    std::string remaining;      // 剩余内容
    bool success;              // 是否成功
    
    ExtractResult() : success(false) {}
};

// ============================================================================
// 核心类 1: Arg - 参数处理核心
// ============================================================================

/**
 * @brief Arg类 - 参数处理的核心
 * 
 * 核心功能：
 * 1. operator() 重载 - 接收函数，处理peekKeyword结果
 * 2. bind() - 绑定值处理函数
 * 3. match() - 匹配并处理参数值（需要代码片段才能使用peekKeyword）
 * 4. transform() - 转换为JS代码模板
 */
class Arg {
public:
    /**
     * @brief 构造函数
     */
    Arg(const std::string& name = "", bool isPlaceholder = false);
    
    /**
     * @brief operator() 重载 - 您的核心设计
     * 用法：keyword.arg[0]([](int num) { return std::to_string(num); });
     * 或者：keyword.arg[0](peekKeyword(-1)); // 传递值给参数
     */
    template<typename T>
    std::string operator()(T value) {
        if constexpr (std::is_invocable_v<T>) {
            // 如果T是函数，绑定该函数
            bindFunction_ = [value](const std::string& input) -> std::string {
                if constexpr (std::is_same_v<std::invoke_result_t<T>, std::string>) {
                    return value();
                } else {
                    return std::to_string(value());
                }
            };
            hasBind_ = true;
            return "";
        } else {
            // 如果T是值，直接处理
            if (hasBind_) {
                processedValue_ = bindFunction_(std::to_string(value));
            } else {
                processedValue_ = std::to_string(value);
            }
            return processedValue_;
        }
    }
    
    /**
     * @brief bind方法 - 绑定值处理函数
     * 用法：keyword.args.bind("url", [](std::string str) { return str; });
     * 🔧 流程位置：第2步 - 在拿到代码片段前预先绑定处理函数
     */
    template<typename T>
    void bind(const std::function<std::string(T)>& func) {
        bindFunction_ = [func](const std::string& rawValue) -> std::string {
            if constexpr (std::is_same_v<T, std::string>) {
                return func(rawValue);
            } else if constexpr (std::is_same_v<T, int>) {
                try {
                    return func(std::stoi(rawValue));
                } catch (...) {
                    return func(0);
                }
            } else if constexpr (std::is_same_v<T, double>) {
                try {
                    return func(std::stod(rawValue));
                } catch (...) {
                    return func(0.0);
                }
            }
            return rawValue;
        };
        hasBind_ = true;
    }
    
    /**
     * @brief transform方法 - 预先定义JS代码模板
     * 用法：keyword.args.transform("url", "loadImage('${value}')");
     * 🔧 流程位置：第3步 - 预先定义最终的JS代码模板
     */
    void transform(const std::string& jsTemplate);
    
    /**
     * @brief match方法 - 捕获参数值（需要代码片段）
     * 用法：keyword.args.match("url", peekKeyword(-1));
     * 🔧 流程位置：第5步 - 拿到代码片段后才能使用peekKeyword获取参数值
     * ⚠️ 重要：必须在有代码片段的情况下才能调用，否则peekKeyword无法工作
     */
    void match(const std::string& value);
    
    /**
     * @brief 应用JS模板转换，填充实际值
     */
    void applyTransform();
    
    /**
     * @brief 获取最终JS代码
     */
    const std::string& getJSCode() const { return jsCode_; }
    
    /**
     * @brief 获取处理后的值
     */
    const std::string& getValue() const { return processedValue_; }
    
    /**
     * @brief 获取参数名称
     */
    const std::string& getName() const { return name_; }
    
    /**
     * @brief 是否为占位符
     */
    bool isPlaceholder() const { return isPlaceholder_; }
    
    /**
     * @brief 是否已绑定函数
     */
    bool hasBind() const { return hasBind_; }
    
    /**
     * @brief 是否已定义JS模板
     */
    bool hasTransform() const { return !jsTemplate_.empty(); }

private:
    std::string name_;
    bool isPlaceholder_;
    bool hasBind_;
    std::string rawValue_;
    std::string processedValue_;
    std::string jsTemplate_;
    std::string jsCode_;
    std::function<std::string(const std::string&)> bindFunction_;
};

// ============================================================================
// 核心类 2: Syntax - 语法分析结果容器
// ============================================================================

/**
 * @brief Syntax类 - 语法分析结果的容器
 * 
 * 核心功能：
 * 1. 维护args向量 - 所有分析出的参数
 * 2. bind() - 按名称绑定参数函数
 * 3. transform() - 预先定义JS代码模板
 * 4. 处理代码片段 - 从统一扫描器获取的代码片段中提取参数值
 * 5. result() - 组合所有参数的JS代码
 */
class Syntax {
public:
    Syntax() = default;
    
    /**
     * @brief args向量 - 您的核心设计
     * 用法：keyword.args[0], keyword.args[1]...
     */
    std::vector<Arg> args;
    
    /**
     * @brief bind方法 - 按名称绑定
     * 用法：keyword.args.bind("url", [](std::string str) { return str; });
     * 支持占位符自动计数：多次bind("$", func)会绑定到不同的$
     * 🔧 流程位置：第2步 - 预先绑定获取值的函数
     */
    template<typename T>
    void bind(const std::string& name, const std::function<std::string(T)>& func) {
        if (name == "$") {
            // 占位符绑定：自动找到下一个未绑定的占位符
            for (auto& arg : args) {
                if (arg.isPlaceholder() && !arg.hasBind()) {
                    arg.bind(func);
                    placeholderCounter_++;
                    break;
                }
            }
        } else {
            // 命名参数绑定
            Arg* targetArg = findArg(name);
            if (targetArg) {
                targetArg->bind(func);
            }
        }
    }
    
    /**
     * @brief transform方法 - 预先定义JS代码模板
     * 用法：keyword.args.transform("url", "loadImage('${value}')");
     * 🔧 流程位置：第3步 - 预先定义最终的JS代码模板
     */
    void transform(const std::string& name, const std::string& jsTemplate);
    
    /**
     * @brief 处理代码片段 - 核心：从统一扫描器的结果中提取参数值
     * 🔧 流程位置：第5步 - 拿到代码片段后，使用peekKeyword技术获取实际参数值
     * ⚠️ 关键：只有在这一步才能使用peekKeyword，因为需要代码片段上下文
     */
    void processCodeFragment(const CodeFragment& fragment, CJMODScanner& scanner);
    
    /**
     * @brief match方法 - 按名称匹配值
     * 用法：keyword.args.match("url", peekKeyword(-1));
     * 支持占位符自动计数：多次match("$", value)会匹配到不同的$
     * 🔧 流程位置：第5步 - 在processCodeFragment内部调用
     */
    void match(const std::string& name, const std::string& value);
    
    /**
     * @brief result方法 - 组合所有参数的最终JS代码
     * 用法：std::string result = keyword.result();
     * 🔧 流程位置：第6步 - 组合所有参数的最终JS代码
     */
    std::string result() const;
    
    /**
     * @brief 获取参数数量
     */
    size_t length() const { return args.size(); }
    
    /**
     * @brief 添加参数（内部使用）
     */
    void addArg(const std::string& name, bool isPlaceholder = false);
    
    /**
     * @brief 获取触发关键字（用于统一扫描器）
     */
    std::string getTriggerKeyword() const { return triggerKeyword_; }
    
    /**
     * @brief 设置触发关键字
     */
    void setTriggerKeyword(const std::string& keyword) { triggerKeyword_ = keyword; }

private:
    std::unordered_map<std::string, size_t> nameToIndex_;
    size_t placeholderCounter_ = 0;
    std::string triggerKeyword_;  // 用于统一扫描器的关键字
    
    Arg* findArg(const std::string& name);
};

// ============================================================================
// 统一扫描器 - 负责扫描代码并返回代码片段
// ============================================================================

/**
 * @brief 统一扫描器
 * 
 * 🎯 核心职责：
 * 1. 接收关键字列表（从scanKeyword获得）
 * 2. 扫描代码，寻找这些关键字
 * 3. 找到关键字后，返回包含上下文的代码片段
 * 4. CJMOD API再根据代码片段使用peekKeyword技术提取参数值
 */
class UnifiedScanner {
public:
    UnifiedScanner();
    
    /**
     * @brief 注册需要扫描的关键字
     * 🔧 流程位置：第4步 - scanKeyword阶段设置什么关键字让统一扫描器搜索
     */
    void registerKeyword(const std::string& keyword, const std::string& syntaxId);
    
    /**
     * @brief 扫描代码，寻找注册的关键字
     * @param sourceCode 源代码
     * @return 找到的代码片段列表
     */
    std::vector<CodeFragment> scanCode(const std::string& sourceCode);
    
    /**
     * @brief 扫描单个关键字
     * @param sourceCode 源代码  
     * @param keyword 要搜索的关键字
     * @return 包含该关键字的代码片段
     */
    CodeFragment scanForKeyword(const std::string& sourceCode, const std::string& keyword);
    
    /**
     * @brief 清除所有注册的关键字
     */
    void clearKeywords();

private:
    std::unordered_map<std::string, std::string> registeredKeywords_;  // keyword -> syntaxId
    
    /**
     * @brief 提取关键字周围的代码片段
     */
    CodeFragment extractCodeFragment(const std::string& sourceCode, 
                                   size_t keywordPos, 
                                   const std::string& keyword);
    
    /**
     * @brief 智能分词
     */
    std::vector<std::string> tokenize(const std::string& code);
};

// ============================================================================
// 代码生成器 - 负责将JS代码输出为标准JS
// ============================================================================

/**
 * @brief 代码生成器
 * 
 * 🎯 核心职责：
 * 1. 接收CJMOD处理后的JS代码
 * 2. 进行代码优化、格式化
 * 3. 输出标准的JS代码
 */
class CodeGenerator {
public:
    CodeGenerator();
    
    /**
     * @brief 生成标准JS代码
     * 🔧 流程位置：第7步 - 把代码交给生成器，生成器将代码输出为标准JS
     * @param jsCode CJMOD处理后的JS代码
     * @return 标准格式的JS代码
     */
    std::string generateStandardJS(const std::string& jsCode);
    
    /**
     * @brief 优化JS代码
     */
    std::string optimizeCode(const std::string& jsCode);
    
    /**
     * @brief 格式化JS代码
     */
    std::string formatCode(const std::string& jsCode);
    
    /**
     * @brief 验证JS语法
     */
    bool validateSyntax(const std::string& jsCode);

private:
    /**
     * @brief 移除多余的空白
     */
    std::string removeExtraWhitespace(const std::string& code);
    
    /**
     * @brief 标准化变量名
     */
    std::string standardizeNames(const std::string& code);
    
    /**
     * @brief 添加必要的分号
     */
    std::string addSemicolons(const std::string& code);
};

// ============================================================================
// 双指针扫描器 - 您的核心架构
// ============================================================================

/**
 * @brief 双指针扫描器
 * 
 * 核心功能：
 * 1. 双指针同步扫描
 * 2. 关键字检测和通知
 * 3. 前置截取机制
 * 4. Policy策略支持
 */
class DualPointerScanner {
public:
    DualPointerScanner();
    
    /**
     * @brief 设置扫描内容
     */
    void setContent(const std::vector<std::string>& tokens);
    
    /**
     * @brief 扫描初始范围 - 确定不存在语法片段
     */
    bool scanInitialRange(size_t rangeSize = 5);
    
    /**
     * @brief 双指针同步扫描
     * 前指针和后指针从0开始，同步向前移动
     * 前指针遇到关键字时通知后指针收集
     */
    ScanPosition synchronousScan(const std::string& keyword);
    
    /**
     * @brief 前置截取机制
     * 处理如 "arg ** arg" 的情况，截取前面的arg避免发送给编译器
     */
    ExtractResult frontExtract(const std::string& keyword, int beforeOffset = -1);
    
    /**
     * @brief peekKeyword - 相对位置查看
     * 用法：peekKeyword(-1), peekKeyword(1)
     * ⚠️ 重要：只有在有代码片段上下文时才能正确工作
     */
    std::string peekKeyword(int offset) const;
    
    /**
     * @brief 获取当前扫描位置
     */
    const ScanPosition& getCurrentPosition() const { return currentPosition_; }

private:
    std::vector<std::string> tokens_;
    ScanPosition currentPosition_;
    
    bool isValidPosition(size_t position) const;
    void movePointersForward();
};

/**
 * @brief CJMODScanner - 完整的扫描器实现
 * 
 * 整合双指针扫描和Policy策略，处理代码片段
 */
class CJMODScanner {
public:
    CJMODScanner();
    
    /**
     * @brief scanKeyword - 设置关键字让统一扫描器搜索
     * 用法：scanner.scanKeyword(keyword.args[1], []() { ... });
     * 🔧 流程位置：第4步 - 设置什么关键字作为统一扫描器的搜索目标
     */
    void scanKeyword(const Arg& arg, std::function<void()> handler);
    void scanKeyword(const std::string& keyword, std::function<void()> handler);
    
    /**
     * @brief 设置代码片段上下文 - 来自统一扫描器
     * ⚠️ 关键：有了代码片段才能使用peekKeyword
     */
    void setCodeFragment(const CodeFragment& fragment);
    
    /**
     * @brief peekKeyword - 智能关键字查看
     * 用法：peekKeyword(-1), peekKeyword(1)
     * ⚠️ 重要：必须先调用setCodeFragment设置代码片段上下文
     */
    std::string peekKeyword(int offset) const;
    
    /**
     * @brief policy系列 - 策略模式
     * 用法：
     * scanner.policyChangeBegin(":", Policy::COLLECT);
     * const std::string& str = scanner.policyChangeEnd("}", Policy::NORMAL);
     */
    void policyChangeBegin(const std::string& trigger, Policy policy);
    std::string policyChangeEnd(const std::string& trigger, Policy policy);
    
    /**
     * @brief 辅助函数
     */
    bool isObject(const std::string& content);
    bool isFunction(const std::string& content);
    
    /**
     * @brief 设置扫描上下文
     */
    void setTokens(const std::vector<std::string>& tokens);
    void setPosition(size_t position);
    
    /**
     * @brief 获取双指针扫描器
     */
    DualPointerScanner& getDualPointerScanner() { return dualScanner_; }
    
    /**
     * @brief 获取统一扫描器
     */
    UnifiedScanner& getUnifiedScanner() { return unifiedScanner_; }

private:
    DualPointerScanner dualScanner_;
    UnifiedScanner unifiedScanner_;
    std::unordered_map<std::string, std::function<void()>> keywordHandlers_;
    std::vector<std::string> currentTokens_;
    size_t currentPosition_;
    Policy currentPolicy_;
    CodeFragment currentFragment_;  // 当前代码片段上下文
    bool hasFragment_;              // 是否有代码片段上下文
    
    // Policy状态栈
    struct PolicyState {
        std::string trigger;
        Policy policy;
        std::string buffer;
    };
    std::vector<PolicyState> policyStack_;
    
    void processCollectPolicy(const std::string& content);
    void processSkipPolicy();
    bool matchesTrigger(const std::string& content, const std::string& trigger) const;
};

// ============================================================================
// 核心类 3: CHTLJSFunction - 高级封装
// ============================================================================

/**
 * @brief CHTLJSFunction类 - 高级函数封装
 * 
 * 提供更高级的接口，封装常见的使用模式
 * 自动处理完整的工作流程
 */
class CHTLJSFunction {
public:
    CHTLJSFunction(const std::string& functionName, const std::vector<std::string>& paramNames = {});
    
    /**
     * @brief 添加参数
     */
    CHTLJSFunction& addParam(const std::string& name, bool isPlaceholder = false);
    
    /**
     * @brief 绑定参数处理函数
     * 🔧 流程位置：第2步
     */
    template<typename T>
    CHTLJSFunction& bindParam(const std::string& name, const std::function<std::string(T)>& func) {
        if (!syntax_) createSyntax();
        syntax_->bind(name, func);
        return *this;
    }
    
    /**
     * @brief 转换参数为JS模板
     * 🔧 流程位置：第3步
     */
    CHTLJSFunction& transformParam(const std::string& name, const std::string& jsTemplate);
    
    /**
     * @brief 设置触发关键字
     * 🔧 流程位置：第4步
     */
    CHTLJSFunction& setTriggerKeyword(const std::string& keyword);
    
    /**
     * @brief 处理源代码，执行完整流程
     * 🔧 执行第4-7步的完整流程
     */
    std::string processSourceCode(const std::string& sourceCode);
    
    /**
     * @brief 生成JS代码（简化接口）
     */
    std::string generateCode();
    
    /**
     * @brief 获取内部Syntax对象
     */
    Syntax* getSyntax() const { return syntax_.get(); }

private:
    std::string functionName_;
    std::vector<std::string> paramNames_;
    std::unique_ptr<Syntax> syntax_;
    std::unique_ptr<CJMODScanner> scanner_;
    std::unique_ptr<CodeGenerator> generator_;
    
    void createSyntax();
    void createScanner();
    void createGenerator();
};

// ============================================================================
// 核心工厂函数
// ============================================================================

/**
 * @brief syntaxAnalys - 核心语法分析函数
 * 用法：
 * const keyword = syntaxAnalys("printMylove({ url: $, mode: $ });", ",:{};()");
 * 
 * 🔧 流程位置：第1步 - 拆分成多个arg
 * 
 * 功能：
 * 1. 智能分词算法
 * 2. 占位符自动识别
 * 3. 忽略字符处理
 * 4. 注释过滤
 */
std::unique_ptr<Syntax> syntaxAnalys(const std::string& pattern, 
                                    const std::string& ignoreChars = "");

/**
 * @brief generateCode - 代码生成函数
 * 用法：generateCode(keyword);
 * 
 * 🔧 流程位置：第7步 - 把代码交给生成器，生成器将代码输出为标准JS
 * 
 * 功能：
 * 1. 模板替换
 * 2. 代码格式化
 * 3. 错误恢复
 */
std::string generateCode(const Syntax& syntax);

/**
 * @brief 创建CHTLJSFunction
 */
std::unique_ptr<CHTLJSFunction> createCHTLJSFunction(const std::string& functionName,
                                                    const std::vector<std::string>& paramNames = {});

/**
 * @brief 创建统一扫描器
 */
std::unique_ptr<UnifiedScanner> createUnifiedScanner();

/**
 * @brief 创建代码生成器
 */
std::unique_ptr<CodeGenerator> createCodeGenerator();

} // namespace CJMOD
} // namespace CHTL

/**
 * @brief 🚀 完整工作流程详解：
 * 
 * 【第1步】syntaxAnalys(拆分成多个arg)
 *   - 解析语法模式，拆分成多个Arg对象
 *   - 识别占位符和具名参数
 *   - 示例：syntaxAnalys("printMylove({ url: $, mode: $ });")
 * 
 * 【第2步】bind(绑定获取值的函数) 
 *   - 为每个参数预先绑定值处理函数
 *   - 示例：keyword.bind("url", [](const std::string& s) { return "\"" + s + "\""; });
 * 
 * 【第3步】transform(最终的JS代码)
 *   - 预先定义每个参数的JS代码模板
 *   - 示例：keyword.transform("url", "loadImage('${value}')");
 * 
 * 【第4步】scanKeyword(什么关键字作为统一扫描器的搜索)
 *   - 设置关键字，让统一扫描器知道要搜索什么
 *   - 示例：scanner.scanKeyword("printMylove", handler);
 *   - 统一扫描器开始在源代码中寻找这个关键字
 * 
 * 【第5步】match(捕获参数值，这个要拿到代码片段才行，否则无法使用peekKeyword)
 *   - ⚠️ 关键点：统一扫描器找到关键字后，返回包含上下文的代码片段
 *   - 只有拿到代码片段，才能使用peekKeyword(-1), peekKeyword(1)获取参数值
 *   - CJMOD API根据代码片段分解为多个keyword，传入实际参数值
 *   - 示例：keyword.match("url", scanner.peekKeyword(-1));
 * 
 * 【第6步】result(组合所有参数的最终JS代码)
 *   - 将所有参数的JS代码组合成完整的JS代码
 *   - 示例：std::string jsCode = keyword.result();
 * 
 * 【第7步】generateCode(把代码交给生成器，生成器将代码输出为标准js)
 *   - 代码生成器对JS代码进行优化、格式化
 *   - 输出符合标准的JS代码
 *   - 示例：std::string finalCode = generateCode(keyword);
 * 
 * 💡 核心理解：
 * - 统一扫描器是桥梁：接收关键字 -> 扫描源代码 -> 返回代码片段
 * - 代码片段是关键：只有有了代码片段，peekKeyword才能工作
 * - 确实很复杂：需要统一扫描器、CJMOD API、代码生成器三方协作
 * - 前置截取：避免参数被编译器误处理
 * - 双指针扫描：您的创新扫描机制，提高效率和准确性
 */