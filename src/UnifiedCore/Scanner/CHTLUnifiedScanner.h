#pragma once
#include "Token.h"
#include <string>
#include <vector>
#include <memory>
#include <unordered_set>

namespace chtl {

// 代码片段类型
enum class FragmentType {
    CHTL,           // CHTL代码片段
    CHTL_JS,        // CHTL JS代码片段
    CSS,            // CSS代码片段
    JAVASCRIPT,     // JavaScript代码片段
    MIXED,          // 混合类型片段
    UNKNOWN         // 未知类型片段
};

// 代码切片信息
struct CodeSlice {
    std::string content;        // 切片内容
    FragmentType type;          // 片段类型
    size_t start_pos;          // 起始位置
    size_t end_pos;            // 结束位置
    size_t line;               // 行号
    size_t column;             // 列号
    bool is_complete;          // 是否为完整片段
    bool needs_expansion;      // 是否需要扩增
    
    CodeSlice(const std::string& content = "", FragmentType type = FragmentType::UNKNOWN,
              size_t start = 0, size_t end = 0, size_t line = 1, size_t column = 1)
        : content(content), type(type), start_pos(start), end_pos(end),
          line(line), column(column), is_complete(false), needs_expansion(false) {}
};

// 最小语法单元
struct MinimalUnit {
    std::string content;        // 单元内容
    FragmentType type;          // 类型
    size_t start_pos;          // 起始位置
    size_t end_pos;            // 结束位置
    bool is_boundary;          // 是否为边界单元
    bool can_aggregate;        // 是否可以聚合
    
    MinimalUnit(const std::string& content = "", FragmentType type = FragmentType::UNKNOWN,
                size_t start = 0, size_t end = 0)
        : content(content), type(type), start_pos(start), end_pos(end),
          is_boundary(false), can_aggregate(true) {}
};

// 扫描器配置
struct ScannerConfig {
    size_t initial_slice_size;     // 初始切片大小
    size_t max_slice_size;         // 最大切片大小
    size_t expansion_step;         // 扩增步长
    bool enable_aggregation;       // 是否启用聚合
    bool enable_context_analysis;  // 是否启用上下文分析
    size_t max_aggregation_size;   // 最大聚合大小
    
    ScannerConfig()
        : initial_slice_size(1024), max_slice_size(8192), expansion_step(512),
          enable_aggregation(true), enable_context_analysis(true), max_aggregation_size(2048) {}
};

// CHTL统一扫描器
class CHTLUnifiedScanner {
public:
    CHTLUnifiedScanner(const ScannerConfig& config = ScannerConfig());
    ~CHTLUnifiedScanner();
    
    // 主扫描接口
    std::vector<CodeSlice> scanSource(const std::string& source_code);
    std::vector<CodeSlice> scanSourceIncremental(const std::string& source_code, size_t start_pos = 0);
    
    // 切片处理
    CodeSlice readNextSlice(const std::string& source, size_t& position);
    bool isSliceComplete(const CodeSlice& current_slice, const std::string& next_preview);
    CodeSlice expandSlice(const CodeSlice& slice, const std::string& source, size_t expansion_size);
    
    // 完整性检查
    bool checkFragmentCompleteness(const std::string& fragment, FragmentType type);
    bool canFormCompleteFragment(const std::string& current, const std::string& next, FragmentType type);
    std::string getCompletionPreview(const std::string& source, size_t position, size_t preview_size = 100);
    
    // 最小单元切割
    std::vector<MinimalUnit> performMinimalUnitCutting(const CodeSlice& slice);
    std::vector<MinimalUnit> cutCHTLUnits(const std::string& content, size_t base_pos);
    std::vector<MinimalUnit> cutCHTLJSUnits(const std::string& content, size_t base_pos);
    std::vector<MinimalUnit> cutCSSUnits(const std::string& content, size_t base_pos);
    std::vector<MinimalUnit> cutJavaScriptUnits(const std::string& content, size_t base_pos);
    
    // 上下文感知聚合
    std::vector<CodeSlice> performContextAwareAggregation(const std::vector<MinimalUnit>& units);
    bool shouldAggregateUnits(const MinimalUnit& unit1, const MinimalUnit& unit2);
    CodeSlice aggregateUnits(const std::vector<MinimalUnit>& units, FragmentType target_type);
    
    // 片段类型识别
    FragmentType identifyFragmentType(const std::string& content);
    FragmentType analyzeContentType(const std::string& content);
    bool containsCHTLSyntax(const std::string& content);
    bool containsCHTLJSSyntax(const std::string& content);
    bool containsCSSSyntax(const std::string& content);
    bool containsJavaScriptSyntax(const std::string& content);
    
    // CHTL特定检测
    bool isCHTLElement(const std::string& content);
    bool isCHTLTemplate(const std::string& content);
    bool isCHTLCustom(const std::string& content);
    bool isCHTLNamespace(const std::string& content);
    bool isCHTLImport(const std::string& content);
    
    // CHTL JS特定检测
    bool isCHTLJSSelector(const std::string& content);
    bool isCHTLJSFunction(const std::string& content);
    bool isCHTLJSVirtualObject(const std::string& content);
    bool hasCHTLJSArrow(const std::string& content);
    
    // 边界检测
    bool isAtCHTLBoundary(const std::string& content, size_t position);
    bool isAtCHTLJSBoundary(const std::string& content, size_t position);
    bool isAtStringBoundary(const std::string& content, size_t position);
    bool isAtCommentBoundary(const std::string& content, size_t position);
    
    // 嵌套处理
    int calculateNestingLevel(const std::string& content, size_t position);
    std::vector<std::pair<size_t, size_t>> findNestedBlocks(const std::string& content);
    bool isInsideNestedBlock(size_t position, const std::vector<std::pair<size_t, size_t>>& blocks);
    
    // 配置管理
    void setConfig(const ScannerConfig& config);
    ScannerConfig getConfig() const;
    
    // 统计和调试
    struct ScannerStatistics {
        size_t total_slices;
        size_t expanded_slices;
        size_t aggregated_units;
        size_t chtl_fragments;
        size_t chtl_js_fragments;
        size_t css_fragments;
        size_t js_fragments;
        double average_slice_size;
        double expansion_ratio;
    };
    
    ScannerStatistics getStatistics() const;
    void resetStatistics();
    void printScannerInfo() const;
    
    // 错误处理
    std::string getLastError() const { return last_error_; }
    bool hasErrors() const { return !last_error_.empty(); }
    void clearErrors();

private:
    ScannerConfig config_;
    mutable ScannerStatistics stats_;
    std::string last_error_;
    
    // 内部状态
    std::vector<CodeSlice> current_slices_;
    std::unordered_set<std::string> chtl_keywords_;
    std::unordered_set<std::string> chtl_js_keywords_;
    
    // 初始化
    void initializeKeywords();
    void initializeCHTLKeywords();
    void initializeCHTLJSKeywords();
    
    // 切片分析
    bool analyzeSliceCompleteness(const CodeSlice& slice, const std::string& source, size_t next_pos);
    size_t calculateOptimalExpansion(const CodeSlice& slice, const std::string& source, size_t next_pos);
    
    // 语法分析辅助
    std::vector<size_t> findCHTLBoundaries(const std::string& content);
    std::vector<size_t> findCHTLJSBoundaries(const std::string& content);
    std::vector<size_t> findStringBoundaries(const std::string& content);
    std::vector<size_t> findCommentBoundaries(const std::string& content);
    
    // 最小单元识别
    bool isMinimalCHTLUnit(const std::string& content);
    bool isMinimalCHTLJSUnit(const std::string& content);
    bool isMinimalCSSUnit(const std::string& content);
    bool isMinimalJSUnit(const std::string& content);
    
    // 聚合策略
    bool canAggregateCHTLUnits(const MinimalUnit& unit1, const MinimalUnit& unit2);
    bool canAggregateCHTLJSUnits(const MinimalUnit& unit1, const MinimalUnit& unit2);
    size_t calculateAggregationScore(const std::vector<MinimalUnit>& units);
    
    // 错误处理
    void setError(const std::string& error);
    void updateStatistics(const CodeSlice& slice, bool was_expanded, bool was_aggregated);
};

// 扫描器工具函数
namespace ScannerUtils {
    // 语法模式匹配
    bool matchesCHTLPattern(const std::string& content);
    bool matchesCHTLJSPattern(const std::string& content);
    bool matchesCSSPattern(const std::string& content);
    bool matchesJSPattern(const std::string& content);
    
    // 边界检测工具
    size_t findNextSignificantChar(const std::string& content, size_t start_pos);
    size_t findPreviousSignificantChar(const std::string& content, size_t start_pos);
    bool isWhitespace(char c);
    bool isSignificantChar(char c);
    
    // 嵌套分析工具
    std::vector<std::pair<char, char>> getBracePairs();
    int countNestedLevel(const std::string& content, size_t position, char open_char, char close_char);
    bool isBalanced(const std::string& content, char open_char, char close_char);
    
    // 语法单元识别
    bool isCHTLKeyword(const std::string& word);
    bool isCHTLJSKeyword(const std::string& word);
    bool isCSSProperty(const std::string& word);
    bool isJSKeyword(const std::string& word);
    
    // 字符串和注释处理
    std::pair<size_t, size_t> findStringSpan(const std::string& content, size_t start_pos);
    std::pair<size_t, size_t> findCommentSpan(const std::string& content, size_t start_pos);
    bool isInsideString(const std::string& content, size_t position);
    bool isInsideComment(const std::string& content, size_t position);
    
    // 切片优化
    std::string optimizeSliceContent(const std::string& content);
    std::string trimSliceContent(const std::string& content);
    bool isEmptySlice(const std::string& content);
}

} // namespace chtl