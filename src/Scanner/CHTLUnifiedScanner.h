#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <memory>

namespace chtl {
namespace scanner {

/**
 * CHTL统一扫描器
 * 基于上下文的精确扫描，不将无修饰字面量作为CHTL JS判断条件
 * 严格按照CHTL语法文档实现
 */
class CHTLUnifiedScanner {
public:
    enum class CodeContext {
        CHTL_GLOBAL,        // CHTL全局上下文
        CHTL_ELEMENT,       // CHTL元素内部
        CHTL_STYLE,         // CHTL样式块内部
        CHTL_SCRIPT,        // CHTL脚本块内部（这里才是CHTL JS）
        CHTL_TEXT,          // CHTL文本块内部
        CHTL_TEMPLATE,      // CHTL模板定义内部
        CHTL_CUSTOM,        // CHTL自定义定义内部
        CHTL_ORIGIN,        // CHTL原始嵌入内部
        CHTL_CONFIGURATION, // CHTL配置块内部
        CHTL_IMPORT,        // CHTL导入语句内部
        CHTL_NAMESPACE,     // CHTL命名空间内部
        HTML_RAW,           // 原始HTML内容
        CSS_RAW,            // 原始CSS内容
        JS_RAW              // 原始JavaScript内容
    };
    
    struct CodeFragment {
        std::string content;        // 代码片段内容
        CodeContext context;        // 上下文类型
        size_t startPosition;       // 起始位置
        size_t endPosition;         // 结束位置
        int nestingLevel;           // 嵌套层级
        std::string parentContext;  // 父上下文信息
        
        // 无修饰字面量相关
        bool containsUndecoratedLiterals = false;  // 是否包含无修饰字面量
        std::vector<std::pair<size_t, std::string>> undecoratedLiterals; // 无修饰字面量位置和内容
        
        // CHTL JS特征（仅在CHTL_SCRIPT上下文中有效）
        bool containsCHTLJSFeatures = false;      // 是否包含CHTL JS特征
        bool hasEnhancedSelectors = false;        // 是否有增强选择器{{}}
        bool hasArrowOperators = false;           // 是否有箭头操作符->
        bool hasListenCalls = false;              // 是否有listen调用
        bool hasDelegateCalls = false;            // 是否有delegate调用
        bool hasAnimateCalls = false;             // 是否有animate调用
        bool hasVirtualObjects = false;           // 是否有vir虚对象
    };
    
    struct ScanResult {
        std::vector<CodeFragment> fragments;      // 扫描到的代码片段
        std::unordered_map<CodeContext, int> contextCounts; // 各上下文的片段数量
        bool hasErrors = false;                   // 是否有扫描错误
        std::vector<std::string> errors;          // 错误信息
        std::vector<std::string> warnings;        // 警告信息
    };
    
    CHTLUnifiedScanner();
    ~CHTLUnifiedScanner();
    
    // === 主扫描接口 ===
    
    /**
     * 扫描CHTL代码
     * 基于上下文精确识别不同类型的代码片段
     */
    ScanResult scanCode(const std::string& code);
    
    /**
     * 基于上下文的精确扫描
     * 不将无修饰字面量作为CHTL JS判断条件
     */
    std::vector<CodeFragment> contextAwareScan(const std::string& code);
    
    // === 上下文识别 ===
    
    /**
     * 识别代码上下文
     * 基于当前位置和嵌套结构确定上下文类型
     */
    CodeContext identifyContext(const std::string& code, size_t position, const std::vector<std::string>& contextStack);
    
    /**
     * 检测CHTL JS上下文
     * 仅在script{}块内部才可能是CHTL JS
     */
    bool isCHTLJSContext(const std::string& code, size_t position, const std::vector<std::string>& contextStack);
    
    /**
     * 检测CHTL JS特征
     * 基于特定语法特征（而非无修饰字面量）判断CHTL JS
     */
    bool detectCHTLJSFeatures(const std::string& scriptContent);
    
    // === 无修饰字面量处理 ===
    
    /**
     * 基于上下文扫描无修饰字面量
     * 只在适当的上下文中识别无修饰字面量
     */
    std::vector<std::pair<size_t, std::string>> scanUndecoratedLiterals(const std::string& content, CodeContext context);
    
    /**
     * 验证无修饰字面量的上下文合法性
     * 确保无修饰字面量在正确的上下文中使用
     */
    bool isValidUndecoratedLiteralContext(CodeContext context);
    
    /**
     * 区分无修饰字面量和标识符
     * 避免将变量名误认为是无修饰字面量
     */
    bool isUndecoratedLiteralNotIdentifier(const std::string& value, CodeContext context);
    
    // === CHTL JS特征检测 ===
    
    /**
     * 检测增强选择器
     * {{选择器}} 格式
     */
    bool hasEnhancedSelectors(const std::string& content);
    
    /**
     * 检测箭头操作符
     * -> 操作符（但要排除注释中的箭头）
     */
    bool hasArrowOperators(const std::string& content);
    
    /**
     * 检测CHTL JS函数调用
     * listen, delegate, animate, vir等
     */
    bool hasCHTLJSFunctionCalls(const std::string& content);
    
    /**
     * 检测虚对象语法
     * vir 关键字
     */
    bool hasVirtualObjectSyntax(const std::string& content);
    
    // === 上下文栈管理 ===
    
    /**
     * 更新上下文栈
     * 根据当前扫描位置更新上下文栈
     */
    void updateContextStack(const std::string& code, size_t position, std::vector<std::string>& contextStack);
    
    /**
     * 进入新上下文
     */
    void enterContext(const std::string& contextName, std::vector<std::string>& contextStack);
    
    /**
     * 退出当前上下文
     */
    void exitContext(std::vector<std::string>& contextStack);
    
    /**
     * 获取当前上下文
     */
    std::string getCurrentContext(const std::vector<std::string>& contextStack);
    
    // === 可变长度切片机制 ===
    
    /**
     * 检查片段完整性
     * 检查下一个片段的起始部分是否可能与当前片段组成完整的CHTL或CHTL JS代码片段
     */
    bool checkFragmentCompleteness(const std::string& code, size_t currentEnd, size_t nextStart);
    
    /**
     * 向前扩增切片范围
     * 若判定为可组成完整片段，则向前扩增指定长度的切片范围
     */
    size_t expandSliceRange(const std::string& code, size_t currentEnd, size_t expandLength);
    
    /**
     * 按最小语法单元二次切割
     * 例如，{{box}}->click 需被切割为 {{box}}-> 与 click 两个单元
     */
    std::vector<std::string> performMinimalUnitSlicing(const std::string& fragment);
    
    /**
     * 适当聚合连续片段
     * 确保多个连续的CHTL/CHTL JS代码片段保持适当聚合，而非全部拆解为最小单元
     */
    std::vector<CodeFragment> aggregateConsecutiveFragments(const std::vector<CodeFragment>& fragments);
    
    // === 嵌套结构处理 ===
    
    /**
     * 扫描嵌套结构
     * 处理CHTL的复杂嵌套语法
     */
    std::vector<CodeFragment> scanNestedStructures(const std::string& code);
    
    /**
     * 匹配大括号
     * 找到匹配的开始和结束大括号
     */
    size_t findMatchingBrace(const std::string& code, size_t startPos);
    
    /**
     * 提取块内容
     * 提取{}内的完整内容
     */
    std::string extractBlockContent(const std::string& code, size_t startPos, size_t endPos);
    
    // === 智能分析 ===
    
    /**
     * 分析代码特征
     * 为每个代码片段分析其特征
     */
    void analyzeCodeFeatures(CodeFragment& fragment);
    
    /**
     * 上下文相关的特征分析
     * 根据不同上下文分析不同的特征
     */
    void analyzeContextSpecificFeatures(CodeFragment& fragment);
    
    /**
     * 验证上下文一致性
     * 确保代码片段的上下文标识正确
     */
    bool validateContextConsistency(const CodeFragment& fragment);
    
    // === 配置和选项 ===
    
    void setDebugMode(bool enabled);
    void setStrictMode(bool enabled);           // 严格模式：更严格的上下文检查
    void setContextSensitivity(int level);     // 上下文敏感度 (1-5)
    
    // === 错误处理 ===
    void addError(const std::string& message);
    void addWarning(const std::string& message);
    void clearErrors();
    bool hasErrors() const;
    std::vector<std::string> getErrors() const;
    std::vector<std::string> getWarnings() const;

private:
    std::vector<std::string> errors_;
    std::vector<std::string> warnings_;
    
    // 配置选项
    bool debugMode_ = false;
    bool strictMode_ = true;
    int contextSensitivity_ = 3;
    
    // 上下文识别规则
    std::unordered_map<std::string, CodeContext> contextKeywords_;
    std::unordered_set<std::string> chtljsKeywords_;
    std::unordered_set<std::string> validUndecoratedContexts_;
    
    // CHTL JS特征模式
    std::vector<std::string> enhancedSelectorPatterns_;
    std::vector<std::string> chtljsFunctionPatterns_;
    
    void initializeContextRules();
    void initializeCHTLJSPatterns();
    void initializeUndecoratedLiteralRules();
};

} // namespace scanner
} // namespace chtl