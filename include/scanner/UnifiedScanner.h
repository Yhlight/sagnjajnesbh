#ifndef CHTL_SCANNER_UNIFIED_SCANNER_H
#define CHTL_SCANNER_UNIFIED_SCANNER_H

#include <string>
#include <vector>
#include <memory>
#include <queue>
#include "../common/Token.h"
#include "../common/SourceLocation.h"
#include "../common/Error.h"

namespace chtl {

// 代码片段类型
enum class FragmentType {
    CHTL,           // CHTL代码
    CHTL_JS,        // CHTL JS代码
    CSS,            // 纯CSS代码
    JAVASCRIPT,     // 纯JavaScript代码
    HTML,           // 原始HTML代码
    COMMENT,        // 注释
    UNKNOWN
};

// 代码片段
struct CodeFragment {
    FragmentType type;
    std::string content;
    SourceLocation location;
    std::vector<TokenPtr> tokens;  // 该片段的token列表
    
    CodeFragment(FragmentType t, const std::string& c, const SourceLocation& loc)
        : type(t), content(c), location(loc) {}
};

using CodeFragmentPtr = std::shared_ptr<CodeFragment>;

// 扫描器配置
struct ScannerConfig {
    size_t initialBufferSize = 4096;      // 初始缓冲区大小
    size_t maxBufferSize = 1048576;       // 最大缓冲区大小（1MB）
    size_t lookAheadSize = 256;           // 向前查看大小
    bool preserveComments = true;         // 是否保留注释
    bool enableIncrementalParsing = true; // 是否启用增量解析
};

// 统一扫描器
class UnifiedScanner {
private:
    // 输入管理
    std::string filename_;
    std::string content_;
    size_t position_ = 0;
    size_t line_ = 1;
    size_t column_ = 1;
    
    // 配置
    ScannerConfig config_;
    
    // 诊断收集器
    DiagnosticCollector* diagnostics_;
    
    // 缓冲区管理
    std::vector<char> buffer_;
    size_t bufferStart_ = 0;
    size_t bufferEnd_ = 0;
    
    // 片段队列
    std::queue<CodeFragmentPtr> fragmentQueue_;
    
    // 当前处理的片段类型栈
    std::vector<FragmentType> typeStack_;
    
    // 辅助方法
    char peek(size_t offset = 0) const;
    char advance();
    void skipWhitespace();
    bool match(const std::string& text) const;
    bool matchRegex(const std::string& pattern) const;
    bool isAtEnd() const { return position_ >= content_.size(); }
    
    // 位置管理
    SourceLocation getCurrentLocation() const;
    void updatePosition(char ch);
    
    // 片段识别
    FragmentType identifyFragmentType();
    bool isCHTLStart() const;
    bool isCHTLJSStart() const;
    bool isCSSStart() const;
    bool isJavaScriptStart() const;
    bool isCommentStart() const;
    
    // 片段提取
    CodeFragmentPtr extractCHTLFragment();
    CodeFragmentPtr extractCHTLJSFragment();
    CodeFragmentPtr extractCSSFragment();
    CodeFragmentPtr extractJavaScriptFragment();
    CodeFragmentPtr extractCommentFragment();
    CodeFragmentPtr extractHTMLFragment();
    
    // 边界检测
    size_t findFragmentEnd(FragmentType type);
    bool isFragmentBoundary(FragmentType type) const;
    
    // 最小单元切割
    void splitIntoMinimalUnits(CodeFragmentPtr fragment);
    std::vector<std::string> splitCHTLUnits(const std::string& content);
    std::vector<std::string> splitCHTLJSUnits(const std::string& content);
    
    // 上下文验证
    bool validateFragmentContext(const CodeFragmentPtr& fragment);
    
public:
    UnifiedScanner(const std::string& filename, const std::string& content,
                  DiagnosticCollector* diag = nullptr);
    
    // 配置
    void setConfig(const ScannerConfig& cfg) { config_ = cfg; }
    const ScannerConfig& getConfig() const { return config_; }
    
    // 主扫描方法
    std::vector<CodeFragmentPtr> scan();
    
    // 增量扫描
    CodeFragmentPtr scanNext();
    bool hasMore() const;
    
    // 重置
    void reset();
    
    // 获取所有片段
    std::vector<CodeFragmentPtr> getAllFragments();
    
    // 调试信息
    void dumpFragments(const std::vector<CodeFragmentPtr>& fragments) const;
};

// 片段分析器 - 用于二次处理
class FragmentAnalyzer {
private:
    DiagnosticCollector* diagnostics_;
    
public:
    explicit FragmentAnalyzer(DiagnosticCollector* diag = nullptr)
        : diagnostics_(diag) {}
    
    // 分析片段依赖关系
    void analyzeDependencies(const std::vector<CodeFragmentPtr>& fragments);
    
    // 验证片段完整性
    bool validateFragments(const std::vector<CodeFragmentPtr>& fragments);
    
    // 优化片段顺序
    std::vector<CodeFragmentPtr> optimizeFragmentOrder(
        const std::vector<CodeFragmentPtr>& fragments);
    
    // 合并相邻的相同类型片段
    std::vector<CodeFragmentPtr> mergeAdjacentFragments(
        const std::vector<CodeFragmentPtr>& fragments);
};

// 扫描器工具函数
namespace ScannerUtils {
    // 判断字符类型
    bool isIdentifierStart(char ch);
    bool isIdentifierPart(char ch);
    bool isWhitespace(char ch);
    bool isNewline(char ch);
    
    // 字符串处理
    std::string unescapeString(const std::string& str);
    std::string escapeString(const std::string& str);
    
    // 片段类型转换
    std::string fragmentTypeToString(FragmentType type);
    FragmentType stringToFragmentType(const std::string& str);
}

} // namespace chtl

#endif // CHTL_SCANNER_UNIFIED_SCANNER_H