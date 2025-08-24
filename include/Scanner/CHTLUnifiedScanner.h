#pragma once

#include <string>
#include <vector>
#include <memory>
#include <unordered_set>
#include "CHTL/Comments/ContextualCommentSystem.h"

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
 * @brief 扫描策略枚举
 */
enum class ScanStrategy {
    SLIDING_WINDOW,  // 双指针滑动窗口策略（修正版）
    FRONT_EXTRACT    // 前置代码截取策略
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
 * 支持两种扫描策略：双指针滑动窗口和前置代码截取
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
     * @brief 设置扫描策略
     * @param strategy 扫描策略
     */
    void SetScanStrategy(ScanStrategy strategy) { scanStrategy_ = strategy; }

    /**
     * @brief 获取当前扫描策略
     */
    ScanStrategy GetScanStrategy() const { return scanStrategy_; }

    /**
     * @brief 设置详细输出模式
     */
    void SetVerbose(bool verbose) { verbose_ = verbose; }

    /**
     * @brief 注册CHTL JS关键字
     * @param keyword 关键字
     */
    void RegisterKeyword(const std::string& keyword);

    /**
     * @brief 清空已注册的关键字
     */
    void ClearKeywords();

private:
    /**
     * @brief 扫描器状态
     */
    ScannerState currentState_;
    ScanStrategy scanStrategy_;
    
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

    /**
     * @brief 已注册的CHTL JS关键字
     */
    std::unordered_set<std::string> registeredKeywords_;

    /**
     * @brief 上下文注释生成器
     */
    Comments::ContextualCommentGenerator commentGenerator_;

    // ============ 双指针滑动窗口扫描状态 ============
    struct SlidingWindowState {
        size_t frontPointer;     // 前指针
        size_t backPointer;      // 后指针
        bool collectMode;        // 收集模式
        std::string collectBuffer; // 收集缓冲区
        bool initialScanComplete; // 初始扫描是否完成
    };
    
    SlidingWindowState slidingState_;

    // ============ 前置代码截取扫描状态 ============
    struct FrontExtractState {
        std::vector<std::string> extractedSegments; // 已截取的代码段
        size_t currentSegmentPos; // 当前段的位置
        bool extractionActive;   // 是否正在截取
    };
    
    FrontExtractState extractState_;

    // ============ 核心扫描逻辑 ============
    
    /**
     * @brief 主扫描循环（根据策略选择扫描方法）
     */
    void ScanLoop();
    
    /**
     * @brief 双指针滑动窗口扫描实现
     * 前指针和后指针从0开始，一同向前移动
     * 前指针遇到关键字时通知后指针进入收集状态
     */
    void SlidingWindowScan();
    
    /**
     * @brief 前置代码截取扫描实现
     * 从前面截取代码，避免错误送入编译器
     */
    void FrontExtractScan();
    
    /**
     * @brief 初始扫描：避免语法片段位于开头被前指针错过
     * @param searchRange 搜索范围（从开头搜索多少字符）
     */
    void InitialScan(size_t searchRange = 1000);
    
    /**
     * @brief 重置双指针扫描状态
     */
    void ResetSlidingWindowState();
    
    /**
     * @brief 重置前置截取扫描状态
     */
    void ResetFrontExtractState();

    // ============ 关键字检测 ============
    
    /**
     * @brief 在指定位置检测关键字
     * @param position 检测位置
     * @param keyword 检测到的关键字（输出参数）
     * @return 是否检测到关键字
     */
    bool DetectKeywordAt(size_t position, std::string& keyword);
    
    /**
     * @brief 检测位置是否为CHTL JS语法起始
     * @param position 检测位置
     * @return 语法长度，0表示未检测到
     */
    size_t DetectCHTLJSSyntaxAt(size_t position);
    
    /**
     * @brief 查找下一个关键字位置
     * @param startPos 开始搜索的位置
     * @param keyword 找到的关键字（输出参数）
     * @return 关键字位置，string::npos表示未找到
     */
    size_t FindNextKeyword(size_t startPos, std::string& keyword);

    /**
     * @brief 检测"--"注释
     * @param position 检测位置
     * @return 注释长度，0表示未检测到
     */
    size_t DetectDashComment(size_t position);
    
    /**
     * @brief 处理"--"注释，根据上下文生成相应的注释
     * @param comment 注释内容
     * @param surroundingCode 周围的代码上下文
     * @return 处理后的注释
     */
    std::string ProcessDashComment(const std::string& comment, const std::string& surroundingCode);

    // ============ 代码片段提取和处理 ============
    
    /**
     * @brief 从滑动窗口中提取并处理代码片段
     * @param start 起始位置
     * @param end 结束位置
     * @param type 片段类型
     */
    void ExtractAndProcessFromWindow(size_t start, size_t end, FragmentType type);
    
    /**
     * @brief 截取前置代码段
     * @param endPos 截取结束位置
     * @return 截取的代码内容
     */
    std::string ExtractFrontSegment(size_t endPos);
    
    /**
     * @brief 处理截取的代码段，确保不发送给编译器
     * @param segment 代码段
     * @param segmentType 段类型
     */
    void ProcessExtractedSegment(const std::string& segment, FragmentType segmentType);

    // ============ 传统扫描逻辑（兼容性保留） ============
    
    /**
     * @brief 传统扫描方式（已有的实现）
     */
    void TraditionalScan();
    
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
    
    /**
     * @brief 推送指定内容的片段
     * @param type 片段类型
     * @param content 内容
     * @param startPos 起始位置
     * @param endPos 结束位置
     */
    void PushFragment(FragmentType type, const std::string& content, size_t startPos, size_t endPos);
    
    // ============ 工具方法 ============
    
    /**
     * @brief 获取当前字符
     */
    char CurrentChar();
    
    /**
     * @brief 获取指定位置的字符
     */
    char CharAt(size_t pos);
    
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
     * @brief 检查指定位置是否到达文件末尾
     */
    bool IsAtEnd(size_t pos);
    
    /**
     * @brief 跳过空白字符
     */
    void SkipWhitespace();
    
    /**
     * @brief 匹配字符串
     */
    bool Match(const std::string& str);
    
    /**
     * @brief 在指定位置匹配字符串
     */
    bool MatchAt(size_t pos, const std::string& str);
    
    /**
     * @brief 查找字符串
     */
    size_t Find(const std::string& str);
    
    /**
     * @brief 输出调试信息
     */
    void LogDebug(const std::string& message);
    
    // ============ 可变长度切片机制 ============
    
    /**
     * @brief 智能片段完整性检测
     * @param startPos 片段开始位置
     * @param endPos 片段结束位置
     * @return 是否为完整的代码片段
     */
    bool IsCompleteCodeFragment(size_t startPos, size_t endPos);
    
    /**
     * @brief 动态扩增片段边界
     * @param startPos 当前开始位置
     * @param endPos 当前结束位置（引用，会被修改）
     * @return 是否成功扩增
     */
    bool ExpandFragmentBoundary(size_t startPos, size_t& endPos);
    
    /**
     * @brief 检测CHTL块的完整性
     * @param content 代码内容
     * @return 是否为完整的CHTL块
     */
    bool IsCHTLBlockComplete(const std::string& content);
    
    /**
     * @brief 检测CHTL JS片段的完整性
     * @param content 代码内容
     * @return 是否为完整的CHTL JS片段
     */
    bool IsCHTLJSFragmentComplete(const std::string& content);
    
    /**
     * @brief 最小单元切割
     * @param content 完整代码内容
     * @param fragments 输出的片段列表
     */
    void PerformMinimalUnitSlicing(const std::string& content, std::vector<CodeFragment>& fragments);
    
    /**
     * @brief 计算大括号平衡
     * @param content 代码内容
     * @return 大括号平衡数（正数表示未闭合的左括号）
     */
    int CalculateBraceBalance(const std::string& content);
    
    /**
     * @brief 查找下一个语法边界
     * @param startPos 开始位置
     * @return 语法边界位置
     */
    size_t FindNextSyntaxBoundary(size_t startPos);
    
    /**
     * @brief 判断是否需要最小单元切割
     * @param content 代码内容
     * @return 是否需要切割
     */
    bool NeedsMinimalUnitSlicing(const std::string& content);
    
    /**
     * @brief 确定片段类型
     * @param content 代码内容
     * @return 片段类型
     */
    FragmentType DetermineFragmentType(const std::string& content);
};

} // namespace Scanner
} // namespace CHTL