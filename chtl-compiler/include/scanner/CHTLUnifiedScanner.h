#pragma once

#include <string>
#include <vector>
#include <memory>
#include "CodeFragment.h"
#include "ScannerContext.h"

namespace chtl {

/**
 * CHTL 统一扫描器
 * 负责将CHTL源代码精准切割成不同类型的代码片段
 * 
 * 核心职责：
 * 1. 基于可变长度切片进行工作
 * 2. 智能识别代码片段边界
 * 3. 确保CHTL/CHTL JS代码的最小单元切割
 * 4. 上下文感知，避免过度细分
 */
class CHTLUnifiedScanner {
public:
    CHTLUnifiedScanner();
    ~CHTLUnifiedScanner();

    /**
     * 扫描CHTL源代码
     * @param source 源代码内容
     * @param filename 文件名（用于错误报告）
     * @return 代码片段列表
     */
    std::vector<CodeFragment> Scan(const std::string& source, const std::string& filename = "");

    /**
     * 设置切片大小
     * @param size 初始切片大小（字符数）
     */
    void SetSliceSize(size_t size) { m_SliceSize = size; }

    /**
     * 获取扫描错误信息
     */
    const std::vector<std::string>& GetErrors() const { return m_Errors; }

    /**
     * 清除错误信息
     */
    void ClearErrors() { m_Errors.clear(); }

private:
    // 切片相关
    size_t m_SliceSize;
    std::string m_Buffer;
    size_t m_BufferPosition;
    
    // 上下文
    std::unique_ptr<ScannerContext> m_Context;
    
    // 错误信息
    std::vector<std::string> m_Errors;
    std::string m_CurrentFile;

    // 核心扫描方法
    void ProcessSlice(const std::string& slice, size_t startPos, 
                     std::vector<CodeFragment>& fragments);
    
    // 片段识别方法
    bool IsFragmentBoundary(const std::string& buffer, size_t pos);
    FragmentType IdentifyFragmentType(const std::string& content);
    
    // CHTL特定识别
    bool IsCHTLKeyword(const std::string& token);
    bool IsCHTLJSStart(const std::string& buffer, size_t pos);
    bool IsCHTLJSEnd(const std::string& buffer, size_t pos);
    
    // 最小单元切割
    std::vector<CodeFragment> SplitToMinimalUnits(const CodeFragment& fragment);
    
    // 上下文相关切割
    bool ShouldMergeFragments(const CodeFragment& prev, const CodeFragment& current);
    void MergeConsecutiveFragments(std::vector<CodeFragment>& fragments);
    
    // 边界检测
    bool IsCompleteCHTLStatement(const std::string& content);
    bool IsCompleteCHTLJSStatement(const std::string& content);
    bool CheckBracketBalance(const std::string& content);
    
    // 特殊处理
    void ProcessLocalStyleBlock(const std::string& content, size_t startPos,
                               std::vector<CodeFragment>& fragments);
    void ProcessLocalScriptBlock(const std::string& content, size_t startPos,
                                std::vector<CodeFragment>& fragments);
    void ProcessTemplateBlock(const std::string& content, size_t startPos,
                             std::vector<CodeFragment>& fragments);
    void ProcessOriginBlock(const std::string& content, size_t startPos,
                           std::vector<CodeFragment>& fragments);
    
    // 工具方法
    std::string ExtractToken(const std::string& buffer, size_t& pos);
    void SkipWhitespace(const std::string& buffer, size_t& pos);
    void SkipComment(const std::string& buffer, size_t& pos);
    bool IsWhitespace(char ch) const;
    bool IsIdentifierChar(char ch) const;
    
    // 错误处理
    void ReportError(const std::string& message, size_t line, size_t column);
    
    // CHTL JS 特殊语法识别
    bool IsEnhancedSelector(const std::string& content);  // {{selector}}
    bool IsVirtualObject(const std::string& content);     // vir 对象
    bool IsCHTLJSFunction(const std::string& content);    // listen, delegate等
    
    // 辅助方法
    size_t FindMatchingBrace(const std::string& content, size_t startPos);
    size_t FindBlockEnd(const std::string& content, size_t startPos);
    size_t FindElementEnd(const std::string& content, size_t startPos);
    size_t FindStatementEnd(const std::string& content, size_t startPos);
};

} // namespace chtl