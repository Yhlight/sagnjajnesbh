#pragma once
#include <string>
#include <vector>
#include <memory>

namespace CHTL {
namespace Scanner {

// 代码片段类型
enum class FragmentType {
    CHTL,       // CHTL语法片段
    CHTLJS,     // CHTL JS语法片段  
    CSS,        // CSS片段
    JavaScript, // JavaScript片段
    HTML,       // 原始HTML片段
    Text        // 纯文本片段
};

// 代码片段
struct CodeFragment {
    FragmentType type;
    std::string content;
    size_t startLine;
    size_t startColumn;
    size_t endLine;
    size_t endColumn;
    
    CodeFragment(FragmentType t, const std::string& c, 
                 size_t sl, size_t sc, size_t el, size_t ec)
        : type(t), content(c), startLine(sl), startColumn(sc), 
          endLine(el), endColumn(ec) {}
};

// 扫描器配置
struct ScannerConfig {
    size_t initialSliceSize = 4096;      // 初始切片大小
    size_t maxSliceSize = 65536;         // 最大切片大小
    bool enableDebug = false;            // 是否启用调试
};

// CHTLUnifiedScanner - 统一扫描器
class UnifiedScanner {
public:
    explicit UnifiedScanner(const ScannerConfig& config = ScannerConfig());
    ~UnifiedScanner();
    
    // 扫描源代码，返回代码片段列表
    std::vector<CodeFragment> Scan(const std::string& sourceCode);
    
    // 扫描文件
    std::vector<CodeFragment> ScanFile(const std::string& filePath);
    
private:
    class Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace Scanner
} // namespace CHTL