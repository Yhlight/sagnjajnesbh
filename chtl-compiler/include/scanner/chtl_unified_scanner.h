#ifndef CHTL_UNIFIED_SCANNER_H
#define CHTL_UNIFIED_SCANNER_H

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <stack>
#include "scanner/token.h"
#include "scanner/scanner_context.h"
#include "dispatcher/code_fragment.h"

namespace chtl {

/**
 * CHTLUnifiedScanner - 精准代码切割器
 * 负责将CHTL源代码切割成不同类型的代码片段
 */
class CHTLUnifiedScanner {
public:
    CHTLUnifiedScanner();
    ~CHTLUnifiedScanner();

    /**
     * 扫描CHTL源代码并将其切割成不同类型的片段
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

private:
    // 扫描器上下文
    std::unique_ptr<ScannerContext> context_;
    
    // 错误信息
    std::vector<std::string> errors_;
    
    // 当前扫描位置
    size_t currentPos_;
    
    // 源代码
    std::string source_;
    
    // 上下文栈，用于追踪嵌套结构
    std::stack<FragmentType> contextStack_;
    
    // 扫描方法
    void scanCHTL();
    void scanLocalStyle();
    void scanGlobalStyle();
    void scanScript();
    void scanCHTLJS();
    
    // 辅助方法
    char peek(size_t offset = 0) const;
    char advance();
    bool match(const std::string& expected);
    bool isAtEnd() const;
    void skipWhitespace();
    void skipComment();
    void skipSingleLineComment();
    void skipMultiLineComment();
    void skipGeneratorComment();
    
    // 提取特定类型的代码块
    std::string extractBlock(char openDelim, char closeDelim);
    std::string extractUntil(const std::string& delimiter);
    
    // 判断当前位置的代码类型
    FragmentType detectFragmentType();
    
    // 创建代码片段
    std::unique_ptr<CodeFragment> createFragment(
        FragmentType type, 
        const std::string& content,
        size_t line,
        size_t column
    );
    
    // 辅助判断方法
    std::string extractCHTLBlock();
    bool isInElementContext();
    bool containsCHTLJSSyntax();
    
    // 错误处理
    void addError(const std::string& message, size_t line, size_t column);
};

} // namespace chtl

#endif // CHTL_UNIFIED_SCANNER_H