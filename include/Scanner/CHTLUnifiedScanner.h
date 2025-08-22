#pragma once

#include <string>
#include <vector>
#include <memory>

namespace CHTL {
namespace Scanner {

/**
 * @brief 代码片段类型
 */
enum class FragmentType {
    CHTL,        // CHTL代码块
    CHTL_JS,     // CHTL JS语法片段（最小单元）
    CSS,         // CSS代码块
    JS,          // JavaScript代码块
    Unknown
};

/**
 * @brief 代码片段结构
 */
struct CodeFragment {
    FragmentType type;
    std::string content;
    size_t startPos;
    size_t endPos;
    size_t startLine;
    size_t startColumn;
    size_t endLine;
    size_t endColumn;
    
    CodeFragment() : type(FragmentType::Unknown), startPos(0), endPos(0), 
                    startLine(1), startColumn(1), endLine(1), endColumn(1) {}
};

/**
 * @brief 扫描器状态
 */
enum class ScannerState {
    GLOBAL,           // 全局状态（扫描CHTL块）
    IN_CHTL_BLOCK,    // 在CHTL块内
    IN_GLOBAL_STYLE,  // 在全局style块内
    IN_LOCAL_STYLE,   // 在局部style块内
    IN_GLOBAL_SCRIPT, // 在全局script块内
    IN_LOCAL_SCRIPT,  // 在局部script块内
    COLLECTING_CSS,   // 收集CSS代码状态
    COLLECTING_JS     // 收集JS代码状态
};

/**
 * @brief CHTL统一扫描器
 * 实现基于CHTL/CHTL JS切割点位的状态改变机制
 */
class CHTLUnifiedScanner {
public:
    CHTLUnifiedScanner();
    ~CHTLUnifiedScanner() = default;

    /**
     * @brief 扫描CHTL源代码，产生代码片段流
     * @param source 源代码
     * @param fileName 文件名
     * @return 代码片段列表
     */
    std::vector<CodeFragment> ScanSource(const std::string& source, const std::string& fileName = "");

    /**
     * @brief 设置详细输出模式
     */
    void SetVerbose(bool verbose) { verbose_ = verbose; }

private:
    /**
     * @brief 扫描器状态
     */
    ScannerState currentState_;
    
    /**
     * @brief 当前位置
     */
    size_t currentPos_;
    size_t currentLine_;
    size_t currentColumn_;
    
    /**
     * @brief 源代码
     */
    std::string source_;
    std::string fileName_;
    
    /**
     * @brief 代码片段列表
     */
    std::vector<CodeFragment> fragments_;
    
    /**
     * @brief 当前收集的代码缓冲区
     */
    std::string currentBuffer_;
    size_t bufferStartPos_;
    size_t bufferStartLine_;
    size_t bufferStartColumn_;
    
    /**
     * @brief 详细输出模式
     */
    bool verbose_;

    // ============ 核心扫描逻辑 ============
    
    /**
     * @brief 主扫描循环
     */
    void ScanLoop();
    
    /**
     * @brief 处理全局状态
     */
    void HandleGlobalState();
    
    /**
     * @brief 处理CHTL块内状态
     */
    void HandleCHTLBlockState();
    
    /**
     * @brief 处理style块状态
     */
    void HandleStyleBlockState();
    
    /**
     * @brief 处理script块状态
     */
    void HandleScriptBlockState();
    
    // ============ 切割点位检测 ============
    
    /**
     * @brief 检测CHTL语法切割点位
     * @return 如果检测到CHTL语法，返回语法长度；否则返回0
     */
    size_t DetectCHTLSyntax();
    
    /**
     * @brief 检测CHTL JS语法切割点位
     * @return 如果检测到CHTL JS语法，返回语法长度；否则返回0
     */
    size_t DetectCHTLJSSyntax();
    
    /**
     * @brief 检测变量模板语法：ThemeColor(), Colors()等
     */
    size_t DetectVariableTemplate();
    
    /**
     * @brief 检测增强选择器语法：{{.selector}}
     */
    size_t DetectEnhancedSelector();
    
    /**
     * @brief 检测虚对象语法：obj->method(), listen(), delegate()等
     */
    size_t DetectVirtualObjectSyntax();
    
    // ============ 状态转换 ============
    
    /**
     * @brief 进入CHTL块
     */
    void EnterCHTLBlock();
    
    /**
     * @brief 退出CHTL块
     */
    void ExitCHTLBlock();
    
    /**
     * @brief 进入style块
     * @param isGlobal 是否为全局style
     */
    void EnterStyleBlock(bool isGlobal);
    
    /**
     * @brief 退出style块
     */
    void ExitStyleBlock();
    
    /**
     * @brief 进入script块
     * @param isGlobal 是否为全局script
     */
    void EnterScriptBlock(bool isGlobal);
    
    /**
     * @brief 退出script块
     */
    void ExitScriptBlock();
    
    // ============ 片段管理 ============
    
    /**
     * @brief 开始收集代码到缓冲区
     */
    void StartCollecting();
    
    /**
     * @brief 推送收集的代码作为片段
     * @param type 片段类型
     */
    void PushCollectedFragment(FragmentType type);
    
    /**
     * @brief 推送CHTL/CHTL JS语法片段
     * @param type 片段类型
     * @param content 语法内容
     * @param length 语法长度
     */
    void PushSyntaxFragment(FragmentType type, const std::string& content, size_t length);
    
    // ============ 工具方法 ============
    
    /**
     * @brief 获取当前字符
     */
    char CurrentChar();
    
    /**
     * @brief 向前移动一个字符
     */
    void AdvanceChar();
    
    /**
     * @brief 向前移动指定长度
     */
    void Advance(size_t length);
    
    /**
     * @brief 检查是否到达文件末尾
     */
    bool IsAtEnd();
    
    /**
     * @brief 跳过空白字符
     */
    void SkipWhitespace();
    
    /**
     * @brief 匹配字符串
     */
    bool Match(const std::string& str);
    
    /**
     * @brief 查找字符串
     */
    size_t Find(const std::string& str);
    
    /**
     * @brief 输出调试信息
     */
    void LogDebug(const std::string& message);
};

} // namespace Scanner
} // namespace CHTL