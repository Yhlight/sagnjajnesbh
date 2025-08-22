#pragma once

#include <string>
#include <vector>
#include <memory>
#include <unordered_set>
#include "Utils/ErrorHandler.h"

namespace CHTL {
namespace Scanner {

/**
 * @brief 代码片段类型枚举
 */
enum class FragmentType {
    CHTL,       // CHTL语法片段
    CHTLJS,     // CHTL JS语法片段
    CSS,        // CSS样式片段
    JavaScript, // JavaScript代码片段
    Unknown     // 未知类型
};

/**
 * @brief 代码片段结构体
 */
struct CodeFragment {
    FragmentType type;          // 片段类型
    std::string content;        // 片段内容
    size_t startPos;           // 起始位置
    size_t endPos;             // 结束位置
    int startLine;             // 起始行号
    int startColumn;           // 起始列号
    int endLine;               // 结束行号
    int endColumn;             // 结束列号
    std::string context;       // 上下文信息
    
    CodeFragment(FragmentType t, const std::string& c, size_t start, size_t end)
        : type(t), content(c), startPos(start), endPos(end), 
          startLine(1), startColumn(1), endLine(1), endColumn(1) {}
};

/**
 * @brief 扫描配置结构体
 */
struct ScanConfig {
    size_t maxFragmentSize;     // 最大片段大小
    bool enableContextCheck;    // 是否启用上下文检查
    bool enableMinimalUnit;     // 是否启用最小单元切割
    bool preserveWhitespace;    // 是否保留空白字符
    
    ScanConfig() : maxFragmentSize(1024 * 1024), enableContextCheck(true),
                   enableMinimalUnit(true), preserveWhitespace(false) {}
};

/**
 * @brief CHTL统一扫描器类
 * 
 * 负责将CHTL源代码精准切割成不同类型的代码片段，
 * 支持CHTL、CHTL JS、CSS、JavaScript四种类型的代码识别和分离
 */
class CHTLUnifiedScanner {
public:
    /**
     * @brief 构造函数
     * @param config 扫描配置
     */
    explicit CHTLUnifiedScanner(const ScanConfig& config = ScanConfig());
    
    /**
     * @brief 析构函数
     */
    ~CHTLUnifiedScanner() = default;
    
    /**
     * @brief 扫描CHTL源代码
     * @param source CHTL源代码
     * @param fileName 文件名（用于错误报告）
     * @return 代码片段列表
     */
    std::vector<CodeFragment> Scan(const std::string& source, const std::string& fileName = "");
    
    /**
     * @brief 设置扫描配置
     * @param config 新的扫描配置
     */
    void SetConfig(const ScanConfig& config) { config_ = config; }
    
    /**
     * @brief 获取扫描配置
     * @return 当前扫描配置
     */
    const ScanConfig& GetConfig() const { return config_; }
    
    /**
     * @brief 清空扫描器状态
     */
    void Clear();
    
    /**
     * @brief 获取错误数量
     * @return 错误数量
     */
    size_t GetErrorCount() const;

private:
    /**
     * @brief 初始化CHTL关键字集合
     */
    void InitializeKeywords();
    
    /**
     * @brief 检测代码片段类型
     * @param content 代码内容
     * @param context 上下文信息
     * @return 片段类型
     */
    FragmentType DetectFragmentType(const std::string& content, const std::string& context = "");
    
    /**
     * @brief 查找下一个代码块的边界
     * @param source 源代码
     * @param startPos 起始位置
     * @param blockType 块类型标识符
     * @return 块的结束位置，如果未找到返回string::npos
     */
    size_t FindBlockEnd(const std::string& source, size_t startPos, const std::string& blockType);
    
    /**
     * @brief 处理CHTL代码块
     * @param source 源代码
     * @param startPos 起始位置
     * @param endPos 结束位置
     * @param fragments 片段列表
     */
    void ProcessCHTLBlock(const std::string& source, size_t startPos, size_t endPos,
                         std::vector<CodeFragment>& fragments);
    
    /**
     * @brief 处理样式块（style{}）
     * @param source 源代码
     * @param startPos 起始位置
     * @param endPos 结束位置
     * @param fragments 片段列表
     */
    void ProcessStyleBlock(const std::string& source, size_t startPos, size_t endPos,
                          std::vector<CodeFragment>& fragments);
    
    /**
     * @brief 处理脚本块（script{}）
     * @param source 源代码
     * @param startPos 起始位置
     * @param endPos 结束位置
     * @param fragments 片段列表
     */
    void ProcessScriptBlock(const std::string& source, size_t startPos, size_t endPos,
                           std::vector<CodeFragment>& fragments);
    
    /**
     * @brief 处理全局样式块
     * @param source 源代码
     * @param startPos 起始位置
     * @param endPos 结束位置
     * @param fragments 片段列表
     */
    void ProcessGlobalStyleBlock(const std::string& source, size_t startPos, size_t endPos,
                               std::vector<CodeFragment>& fragments);
    
    /**
     * @brief 处理原始嵌入块（[Origin]）
     * @param source 源代码
     * @param startPos 起始位置
     * @param endPos 结束位置
     * @param fragments 片段列表
     */
    void ProcessOriginBlock(const std::string& source, size_t startPos, size_t endPos,
                           std::vector<CodeFragment>& fragments);
    
    /**
     * @brief 进行最小单元切割
     * @param fragment 代码片段
     * @return 切割后的片段列表
     */
    std::vector<CodeFragment> PerformMinimalUnitSplit(const CodeFragment& fragment);
    
    /**
     * @brief 检查上下文合理性
     * @param fragment 代码片段
     * @param prevFragment 前一个片段
     * @param nextFragment 后一个片段
     * @return 是否需要扩展片段
     */
    bool CheckContextValidity(const CodeFragment& fragment, 
                             const CodeFragment* prevFragment = nullptr,
                             const CodeFragment* nextFragment = nullptr);
    
    /**
     * @brief 扩展片段边界
     * @param source 源代码
     * @param fragment 要扩展的片段
     * @param expandSize 扩展大小
     * @return 扩展后的片段
     */
    CodeFragment ExpandFragment(const std::string& source, const CodeFragment& fragment, size_t expandSize);
    
    /**
     * @brief 跳过空白字符
     * @param source 源代码
     * @param pos 当前位置
     * @return 跳过空白字符后的位置
     */
    size_t SkipWhitespace(const std::string& source, size_t pos);
    
    /**
     * @brief 跳过注释
     * @param source 源代码
     * @param pos 当前位置
     * @return 跳过注释后的位置
     */
    size_t SkipComments(const std::string& source, size_t pos);
    
    /**
     * @brief 计算位置的行号和列号
     * @param source 源代码
     * @param pos 位置
     * @param line 输出行号
     * @param column 输出列号
     */
    void CalculateLineColumn(const std::string& source, size_t pos, int& line, int& column);
    
    /**
     * @brief 检查是否为CHTL关键字
     * @param word 单词
     * @return 是否为CHTL关键字
     */
    bool IsCHTLKeyword(const std::string& word);
    
    /**
     * @brief 检查是否为CHTL JS特征
     * @param content 代码内容
     * @return 是否包含CHTL JS特征
     */
    bool HasCHTLJSFeatures(const std::string& content);

private:
    ScanConfig config_;                                 // 扫描配置
    std::string currentFileName_;                       // 当前文件名
    std::unordered_set<std::string> chtlKeywords_;     // CHTL关键字集合
    std::unordered_set<std::string> chtlJSKeywords_;   // CHTL JS关键字集合
};

} // namespace Scanner
} // namespace CHTL