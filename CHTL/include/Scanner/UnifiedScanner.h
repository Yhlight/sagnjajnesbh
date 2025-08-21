#ifndef CHTL_SCANNER_UNIFIEDSCANNER_H
#define CHTL_SCANNER_UNIFIEDSCANNER_H

#include <string>
#include <vector>
#include <memory>
#include <functional>

namespace Scanner {

// 代码片段类型
enum class SliceType {
    CHTL,           // CHTL代码片段
    CHTLJS,         // CHTL JS代码片段
    CSS,            // CSS代码片段
    JavaScript,     // JavaScript代码片段
    Text,           // 纯文本片段
    Comment,        // 注释片段
    Unknown         // 未知类型
};

// 代码片段结构
struct CodeSlice {
    SliceType type;                // 片段类型
    std::string content;            // 片段内容
    size_t startLine;              // 起始行号
    size_t startColumn;            // 起始列号
    size_t endLine;                // 结束行号
    size_t endColumn;              // 结束列号
    std::string filename;          // 所属文件名
    
    // 额外的上下文信息
    struct Context {
        bool inLocalStyle = false;      // 是否在局部style块中
        bool inLocalScript = false;     // 是否在局部script块中
        bool inGlobalStyle = false;     // 是否在全局style块中
        bool inGlobalScript = false;    // 是否在全局script块中
        bool inTemplate = false;        // 是否在模板中
        bool inCustom = false;          // 是否在自定义块中
        bool inOrigin = false;          // 是否在原始嵌入块中
        std::string parentElement;      // 父元素名称
        int nestingLevel = 0;           // 嵌套层级
    } context;
};

// 扫描配置
struct ScanConfig {
    size_t initialSliceSize = 4096;     // 初始切片大小
    size_t maxSliceSize = 65536;        // 最大切片大小
    bool preserveComments = true;       // 是否保留注释
    bool strictMode = false;            // 严格模式（报告所有警告）
    bool debugMode = false;             // 调试模式
};

// 统一扫描器类
class UnifiedScanner {
public:
    UnifiedScanner();
    explicit UnifiedScanner(const ScanConfig& config);
    ~UnifiedScanner();
    
    // 扫描文件
    std::vector<CodeSlice> ScanFile(const std::string& filename);
    
    // 扫描字符串
    std::vector<CodeSlice> ScanString(const std::string& content, 
                                      const std::string& sourceName = "<string>");
    
    // 设置配置
    void SetConfig(const ScanConfig& config) { config_ = config; }
    
    // 获取配置
    const ScanConfig& GetConfig() const { return config_; }
    
    // 设置错误回调
    using ErrorCallback = std::function<void(const std::string& message, 
                                           size_t line, size_t column)>;
    void SetErrorCallback(ErrorCallback callback) { errorCallback_ = callback; }
    
private:
    // 内部扫描实现
    class Impl;
    std::unique_ptr<Impl> impl_;
    
    ScanConfig config_;
    ErrorCallback errorCallback_;
    
    // 内部辅助方法
    std::vector<CodeSlice> DoScan(const std::string& content, 
                                  const std::string& filename);
    
    // 切片验证和调整
    bool ValidateSlice(const CodeSlice& slice);
    void AdjustSliceBoundaries(std::vector<CodeSlice>& slices);
    
    // 识别切片类型
    SliceType IdentifySliceType(const std::string& content, 
                               const CodeSlice::Context& context);
    
    // 处理特殊语法
    void ProcessCHTLSyntax(std::vector<CodeSlice>& slices);
    void ProcessCHTLJSSyntax(std::vector<CodeSlice>& slices);
    
    // 合并相邻的同类型切片
    void MergeAdjacentSlices(std::vector<CodeSlice>& slices);
    
    // 错误处理
    void ReportError(const std::string& message, size_t line, size_t column);
};

// 扫描器工具函数
namespace ScannerUtils {
    // 判断是否为CHTL关键字
    bool IsCHTLKeyword(const std::string& word);
    
    // 判断是否为CHTL JS特殊语法
    bool IsCHTLJSSyntax(const std::string& text);
    
    // 提取增强选择器
    std::string ExtractEnhancedSelector(const std::string& text, size_t& pos);
    
    // 提取虚对象调用
    std::string ExtractVirtualObjectCall(const std::string& text, size_t& pos);
    
    // 获取切片类型名称
    std::string GetSliceTypeName(SliceType type);
}

} // namespace Scanner

#endif // CHTL_SCANNER_UNIFIEDSCANNER_H