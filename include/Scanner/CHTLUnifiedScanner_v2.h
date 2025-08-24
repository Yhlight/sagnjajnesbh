/**
 * @file CHTLUnifiedScanner_v2.h
 * @brief 基于main分支语法文档的精准代码切割器
 * @details 实现可变长度切片扫描，精确识别CHTL、CHTL JS、CSS、JS代码片段
 */

#ifndef CHTL_UNIFIED_SCANNER_V2_H
#define CHTL_UNIFIED_SCANNER_V2_H

#include <string>
#include <vector>
#include <memory>
#include <unordered_set>
#include <regex>

namespace Scanner {

/**
 * @brief 代码片段类型
 */
enum class FragmentType {
    CHTL = 0,      // CHTL代码片段
    CHTL_JS = 1,   // CHTL JS代码片段  
    CSS = 2,       // CSS代码片段
    JS = 3,        // JavaScript代码片段
    Unknown = 4    // 未知类型
};

/**
 * @brief 片段完整性状态
 */
enum class FragmentCompleteness {
    COMPLETE,      // 完整片段
    PARTIAL,       // 部分片段
    INCOMPLETE,    // 不完整片段
    MERGED         // 已合并片段
};

/**
 * @brief 代码片段
 */
struct CodeFragment {
    std::string content;                    // 片段内容
    FragmentType type;                      // 片段类型
    FragmentCompleteness completeness;      // 完整性状态
    size_t startPosition;                   // 在源码中的起始位置
    size_t endPosition;                     // 在源码中的结束位置
    int lineNumber;                         // 行号
    int columnNumber;                       // 列号
    std::string fileName;                   // 源文件名
    
    CodeFragment(const std::string& content, FragmentType type, 
                size_t start, size_t end, const std::string& fileName = "")
        : content(content), type(type), completeness(FragmentCompleteness::COMPLETE),
          startPosition(start), endPosition(end), lineNumber(1), 
          columnNumber(1), fileName(fileName) {}
};

/**
 * @brief 上下文状态
 */
struct ScannerContext {
    bool inScriptBlock = false;         // 是否在script块中
    bool inStyleBlock = false;          // 是否在style块中  
    bool inGlobalScript = false;        // 是否在全局script中
    bool inLocalScript = false;         // 是否在局部script中
    bool inGlobalStyle = false;         // 是否在全局style中
    bool inLocalStyle = false;          // 是否在局部style中
    int braceDepth = 0;                 // 大括号嵌套深度
    int parenthesesDepth = 0;           // 小括号嵌套深度
    
    void reset() {
        inScriptBlock = inStyleBlock = inGlobalScript = 
        inLocalScript = inGlobalStyle = inLocalStyle = false;
        braceDepth = parenthesesDepth = 0;
    }
};

/**
 * @brief CHTL统一扫描器 - 基于main分支语法文档的精准实现
 */
class CHTLUnifiedScanner {
public:
    CHTLUnifiedScanner();
    ~CHTLUnifiedScanner() = default;

    /**
     * @brief 扫描源代码并进行精准切割
     * @param source 源代码
     * @param fileName 文件名
     * @return 代码片段列表
     */
    std::vector<CodeFragment> ScanSource(const std::string& source, 
                                        const std::string& fileName = "");

    /**
     * @brief 启用调试模式
     */
    void EnableDebugMode(bool enable = true) { debugMode_ = enable; }

private:
    // ============ 可变长度切片核心算法 ============
    
    /**
     * @brief 可变长度切片扫描 - 核心算法
     * @param source 源代码
     * @param fileName 文件名
     * @return 初步切片结果
     */
    std::vector<CodeFragment> VariableLengthSlicing(const std::string& source, 
                                                   const std::string& fileName);
    
    /**
     * @brief 查找语法边界
     * @param source 源代码
     * @param startPos 起始位置
     * @return 边界位置，如果未找到返回string::npos
     */
    size_t FindSyntaxBoundary(const std::string& source, size_t startPos);
    
    /**
     * @brief 检查片段完整性并扩展
     * @param source 源代码
     * @param startPos 起始位置
     * @param endPos 结束位置
     * @return 扩展后的结束位置
     */
    size_t CheckAndExpandFragment(const std::string& source, 
                                 size_t startPos, size_t endPos);
    
    /**
     * @brief 最小单元切割 - 精确切割CHTL和CHTL JS
     * @param fragment 代码片段
     * @return 切割后的片段列表
     */
    std::vector<CodeFragment> MinimalUnitSlicing(const CodeFragment& fragment);

    // ============ 语法识别引擎 ============
    
    /**
     * @brief 判断是否为CHTL语法
     * @param content 内容
     * @return 是否为CHTL语法
     */
    bool IsCHTLSyntax(const std::string& content);
    
    /**
     * @brief 判断是否为CHTL JS语法
     * @param content 内容
     * @return 是否为CHTL JS语法
     */
    bool IsCHTLJSSyntax(const std::string& content);
    
    /**
     * @brief 判断是否为CSS语法
     * @param content 内容
     * @return 是否为CSS语法
     */
    bool IsCSSSyntax(const std::string& content);
    
    /**
     * @brief 判断是否为JavaScript语法
     * @param content 内容
     * @return 是否为JavaScript语法
     */
    bool IsJavaScriptSyntax(const std::string& content);

    // ============ CHTL JS特定语法检测 ============
    
    /**
     * @brief 检测虚对象语法: vir 对象名 = listen/delegate/animate({...});
     */
    bool DetectVirtualObject(const std::string& content);
    
    /**
     * @brief 检测增强选择器语法: {{选择器}}
     */
    bool DetectEnhancedSelector(const std::string& content);
    
    /**
     * @brief 检测箭头操作符语法: 对象->方法
     */
    bool DetectArrowOperator(const std::string& content);
    
    /**
     * @brief 检测监听器语法: listen({...})
     */
    bool DetectListenSyntax(const std::string& content);
    
    /**
     * @brief 检测事件委托语法: delegate({...})
     */
    bool DetectDelegateSyntax(const std::string& content);
    
    /**
     * @brief 检测动画语法: animate({...})
     */
    bool DetectAnimateSyntax(const std::string& content);

    // ============ 上下文管理 ============
    
    /**
     * @brief 更新上下文状态
     * @param content 当前内容
     * @param position 当前位置
     */
    void UpdateContext(const std::string& content, size_t position);
    
    /**
     * @brief 确定片段类型（基于上下文）
     * @param content 内容
     * @return 片段类型
     */
    FragmentType DetermineFragmentType(const std::string& content);

    // ============ 工具方法 ============
    
    /**
     * @brief 去除前后空白字符
     */
    std::string Trim(const std::string& str);
    
    /**
     * @brief 输出调试信息
     */
    void LogDebug(const std::string& message);
    
    /**
     * @brief 计算行列号
     */
    std::pair<int, int> CalculateLineColumn(const std::string& source, size_t position);

private:
    // ============ 成员变量 ============
    
    bool debugMode_;                        // 调试模式
    ScannerContext context_;                // 扫描上下文
    std::unordered_set<std::string> chtlKeywords_;     // CHTL关键字集合
    std::unordered_set<std::string> chtlJSKeywords_;   // CHTL JS关键字集合
    
    // CHTL JS语法的正则表达式模式
    std::regex virPattern_;                 // 虚对象模式
    std::regex enhancedSelectorPattern_;    // 增强选择器模式
    std::regex arrowOperatorPattern_;       // 箭头操作符模式
    std::regex listenPattern_;              // 监听器模式
    std::regex delegatePattern_;            // 事件委托模式
    std::regex animatePattern_;             // 动画模式
};

} // namespace Scanner

#endif // CHTL_UNIFIED_SCANNER_V2_H