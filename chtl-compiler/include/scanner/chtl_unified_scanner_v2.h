#ifndef CHTL_UNIFIED_SCANNER_V2_H
#define CHTL_UNIFIED_SCANNER_V2_H

#include <string>
#include <vector>
#include <memory>
#include <unordered_set>
#include "scanner/slice_buffer.h"
#include "scanner/token.h"
#include "dispatcher/code_fragment.h"

namespace chtl {

/**
 * 语法单元类型
 */
enum class SyntaxUnitType {
    CHTL_ELEMENT,           // CHTL元素
    CHTL_TEXT,              // text块
    CHTL_STYLE,             // style块
    CHTL_SCRIPT,            // script块
    CHTL_TEMPLATE,          // 模板定义
    CHTL_CUSTOM,            // 自定义定义
    CHTL_IMPORT,            // 导入语句
    ENHANCED_SELECTOR,      // {{selector}}
    ARROW_OPERATOR,         // ->
    VIR_DECLARATION,        // vir声明
    JS_FUNCTION_CALL,       // 函数调用
    CSS_RULE,               // CSS规则
    JS_STATEMENT,           // JS语句
    DELIMITER,              // 分隔符
    UNKNOWN                 // 未知
};

/**
 * 语法单元
 */
struct SyntaxUnit {
    SyntaxUnitType type;
    std::string content;
    size_t startPos;
    size_t endPos;
    size_t line;
    size_t column;
};

/**
 * CHTLUnifiedScanner V2 - 基于可变长度切片的精准代码切割器
 */
class CHTLUnifiedScannerV2 {
public:
    CHTLUnifiedScannerV2();
    ~CHTLUnifiedScannerV2();
    
    /**
     * 扫描CHTL源代码并将其切割成代码片段
     * @param source CHTL源代码
     * @return 切割后的代码片段列表
     */
    std::vector<std::unique_ptr<CodeFragment>> scan(const std::string& source);
    
    /**
     * 重置扫描器状态
     */
    void reset();
    
    /**
     * 获取扫描错误信息
     */
    const std::vector<std::string>& getErrors() const { return errors_; }
    
    /**
     * 设置初始切片大小
     */
    void setInitialSliceSize(size_t size) { initialSliceSize_ = size; }
    
    /**
     * 设置切片扩展步长
     */
    void setExpandStep(size_t step) { expandStep_ = step; }
    
private:
    // 配置参数
    size_t initialSliceSize_;
    size_t expandStep_;
    
    // 扫描状态
    std::unique_ptr<SliceBuffer> buffer_;
    std::vector<std::string> errors_;
    
    // 当前处理的代码片段
    std::vector<std::unique_ptr<CodeFragment>> fragments_;
    
    // 扫描方法
    void scanSlice();
    
    /**
     * 检查当前切片是否在合理的截断位置
     * @return true表示截断位置合理，false表示需要扩展切片
     */
    bool isReasonableCutoff();
    
    /**
     * 检查是否可能与下一个片段组成完整的CHTL/CHTL JS代码
     */
    bool mightFormCompleteFragment();
    
    /**
     * 将切片按最小语法单元进行切割
     * @param slice 要切割的切片内容
     * @param startPos 切片在源代码中的起始位置
     * @return 切割后的语法单元列表
     */
    std::vector<SyntaxUnit> splitIntoSyntaxUnits(const std::string& slice, size_t startPos);
    
    /**
     * 识别语法单元类型
     */
    SyntaxUnitType identifySyntaxUnit(const std::string& content, size_t pos);
    
    /**
     * 检查是否是CHTL元素边界
     */
    bool isCHTLElementBoundary(const std::string& content, size_t pos);
    
    /**
     * 检查是否是CHTL JS语法边界
     */
    bool isCHTLJSBoundary(const std::string& content, size_t pos);
    
    /**
     * 提取完整的语法单元
     */
    std::string extractCompleteSyntaxUnit(const std::string& content, size_t& pos, SyntaxUnitType& type);
    
    /**
     * 将语法单元组合成代码片段
     */
    void combineUnitsIntoFragments(const std::vector<SyntaxUnit>& units);
    
    /**
     * 创建代码片段
     */
    std::unique_ptr<CodeFragment> createFragment(
        const std::vector<SyntaxUnit>& units,
        size_t startIdx,
        size_t endIdx,
        FragmentType type
    );
    
    // 辅助方法
    bool isWhitespace(char ch) const;
    bool isIdentifierChar(char ch) const;
    bool matchString(const std::string& content, size_t pos, const std::string& str) const;
    void addError(const std::string& message, size_t line, size_t column);
    
    // CHTL语法检测
    bool isCHTLKeyword(const std::string& word) const;
    bool isCHTLBlockStart(const std::string& content, size_t pos) const;
    bool isCHTLBlockEnd(const std::string& content, size_t pos) const;
    
    // CHTL JS语法检测
    bool isEnhancedSelectorStart(const std::string& content, size_t pos) const;
    bool isArrowOperator(const std::string& content, size_t pos) const;
    bool isVirDeclaration(const std::string& content, size_t pos) const;
    
    // 语法边界检测缓存
    std::unordered_set<std::string> chtlKeywords_;
    std::unordered_set<std::string> chtlJSFunctions_;
};

} // namespace chtl

#endif // CHTL_UNIFIED_SCANNER_V2_H