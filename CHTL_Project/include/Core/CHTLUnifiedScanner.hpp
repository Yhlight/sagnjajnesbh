#pragma once

#include "CHTLTypes.hpp"
#include <regex>

namespace CHTL {

/**
 * @brief CHTL统一扫描器
 * 
 * 负责精准代码切割，支持可变长度切片处理
 * 将CHTL源代码切割为CHTL、CHTL JS、CSS、JavaScript片段
 */
class CHTLUnifiedScanner {
public:
    CHTLUnifiedScanner();
    ~CHTLUnifiedScanner() = default;

    // 禁用拷贝构造和赋值
    CHTLUnifiedScanner(const CHTLUnifiedScanner&) = delete;
    CHTLUnifiedScanner& operator=(const CHTLUnifiedScanner&) = delete;

    /**
     * @brief 扫描源代码并切割为片段
     * @param source 源代码
     * @param filename 文件名
     * @return 代码片段列表
     */
    std::vector<CodeFragment> ScanSource(const String& source, const String& filename = "");

    /**
     * @brief 设置扫描选项
     * @param enableDebug 是否启用调试模式
     */
    void SetOptions(bool enableDebug = false);

    /**
     * @brief 获取扫描统计信息
     */
    struct ScanStatistics {
        size_t totalFragments;
        size_t chtlFragments;
        size_t chtljsFragments;
        size_t cssFragments;
        size_t jsFragments;
    };

    ScanStatistics GetStatistics() const { return statistics_; }

private:
    // 扫描状态
    enum class ScanState {
        Global,          // 全局状态
        InElement,       // 在元素内部
        InStyle,         // 在样式块内部
        InScript,        // 在脚本块内部
        InComment,       // 在注释内部
        InString,        // 在字符串内部
        InTemplate,      // 在模板内部
        InCustom,        // 在自定义内部
        InOrigin,        // 在原始嵌入内部
        InImport,        // 在导入内部
        InConfiguration, // 在配置内部
        InNamespace      // 在命名空间内部
    };

    // 扫描上下文
    struct ScanContext {
        ScanState state;
        size_t nestingLevel;
        String currentBlock;
        SourceLocation location;
        
        ScanContext() : state(ScanState::Global), nestingLevel(0) {}
    };

    // 内部方法
    void InitializePatterns();
    FragmentType DetermineFragmentType(const String& content, const ScanContext& context);
    bool IsCompleteFragment(const String& content, FragmentType type);
    std::vector<String> SplitIntoMinimalUnits(const String& content, FragmentType type);
    bool ShouldMergeFragments(const CodeFragment& current, const CodeFragment& next);
    void UpdateStatistics(FragmentType type);
    SourceLocation CalculateLocation(const String& source, size_t offset, const String& filename);

    // 正则表达式模式
    std::regex elementPattern_;
    std::regex stylePattern_;
    std::regex scriptPattern_;
    std::regex commentPattern_;
    std::regex stringPattern_;
    std::regex templatePattern_;
    std::regex customPattern_;
    std::regex originPattern_;
    std::regex importPattern_;
    std::regex configPattern_;
    std::regex namespacePattern_;
    std::regex chtljsPattern_;

    // 扫描选项
    bool debugMode_;
    
    // 统计信息
    ScanStatistics statistics_;
};

} // namespace CHTL