#include "Scanner/CHTLUnifiedScanner.h"
#include "CHTL/Constraints/CHTLConstraintValidator.h"
#include "Utils/ErrorHandler.h"
#include "Utils/StringUtils.h"
#include <iostream>
#include <regex>

namespace CHTL {
namespace Scanner {

CHTLUnifiedScanner::CHTLUnifiedScanner() 
    : currentState_(ScannerState::GLOBAL), scanStrategy_(ScanStrategy::SLIDING_WINDOW),
      currentPos_(0), currentLine_(1), currentColumn_(1), bufferStartPos_(0), 
      bufferStartLine_(1), bufferStartColumn_(1), verbose_(false) {
    
    // 初始化双指针滑动窗口状态
    ResetSlidingWindowState();
    
    // 初始化前置代码截取状态
    ResetFrontExtractState();
    
    // 注册默认的CHTL JS关键字
    RegisterKeyword("vir");
    RegisterKeyword("printMylove");
    RegisterKeyword("iNeverAway");
    RegisterKeyword("listen");
    RegisterKeyword("delegate");
}

std::vector<CodeFragment> CHTLUnifiedScanner::ScanSource(const std::string& source, const std::string& fileName) {
    // 初始化扫描状态
    source_ = source;
    fileName_ = fileName;
    currentPos_ = 0;
    currentLine_ = 1;
    currentColumn_ = 1;
    currentState_ = ScannerState::GLOBAL;
    fragments_.clear();
    currentBuffer_.clear();
    
    LogDebug("开始扫描CHTL源代码: " + fileName);
    LogDebug("源代码大小: " + std::to_string(source.length()) + " 字节");
    
    // 开始主扫描循环
    ScanLoop();
    
    // 处理剩余缓冲区内容
    if (!currentBuffer_.empty()) {
        if (currentState_ == ScannerState::COLLECTING_CSS) {
            PushCollectedFragment(FragmentType::CSS);
        } else if (currentState_ == ScannerState::COLLECTING_JS) {
            PushCollectedFragment(FragmentType::JS);
        } else {
            PushCollectedFragment(FragmentType::CHTL);
        }
    }
    
    LogDebug("扫描完成，发现 " + std::to_string(fragments_.size()) + " 个代码片段");
    return fragments_;
}

void CHTLUnifiedScanner::ScanLoop() {
    // 根据扫描策略选择扫描方法
    switch (scanStrategy_) {
        case ScanStrategy::SLIDING_WINDOW:
            SlidingWindowScan();
            break;
        case ScanStrategy::FRONT_EXTRACT:
            FrontExtractScan();
            break;
        default:
            // 后备方案：使用传统扫描
            TraditionalScan();
            break;
    }
}

void CHTLUnifiedScanner::HandleGlobalState() {
    SkipWhitespace();
    if (IsAtEnd()) return;
    
    // 检测全局CHTL语法
    
    // 检测命名空间：[Namespace] 空间名 {
    if (Match("[Namespace]")) {
        StartCollecting();
        // 收集整个命名空间块
        size_t braceCount = 0;
        while (!IsAtEnd()) {
            if (CurrentChar() == '{') {
                braceCount++;
                currentBuffer_ += CurrentChar();
                AdvanceChar();
            } else if (CurrentChar() == '}') {
                currentBuffer_ += CurrentChar();
                AdvanceChar();
                braceCount--;
                if (braceCount == 0) break;
            } else {
                currentBuffer_ += CurrentChar();
                AdvanceChar();
            }
        }
        PushCollectedFragment(FragmentType::CHTL);
        return;
    }
    
    // 检测模板：[Template] @Element/Style/Var 名称 {
    if (Match("[Template]")) {
        StartCollecting();
        // 收集整个模板块
        size_t braceCount = 0;
        while (!IsAtEnd()) {
            if (CurrentChar() == '{') {
                braceCount++;
                currentBuffer_ += CurrentChar();
                AdvanceChar();
            } else if (CurrentChar() == '}') {
                currentBuffer_ += CurrentChar();
                AdvanceChar();
                braceCount--;
                if (braceCount == 0) break;
            } else {
                currentBuffer_ += CurrentChar();
                AdvanceChar();
            }
        }
        PushCollectedFragment(FragmentType::CHTL);
        return;
    }
    
    // 检测自定义：[Custom] @Element/Style/Var 名称 {
    if (Match("[Custom]")) {
        StartCollecting();
        // 收集整个自定义块
        size_t braceCount = 0;
        while (!IsAtEnd()) {
            if (CurrentChar() == '{') {
                braceCount++;
                currentBuffer_ += CurrentChar();
                AdvanceChar();
            } else if (CurrentChar() == '}') {
                currentBuffer_ += CurrentChar();
                AdvanceChar();
                braceCount--;
                if (braceCount == 0) break;
            } else {
                currentBuffer_ += CurrentChar();
                AdvanceChar();
            }
        }
        PushCollectedFragment(FragmentType::CHTL);
        return;
    }
    
    // 检测全局样式：style {
    if (Match("style") && currentPos_ < source_.length() && 
        (source_[currentPos_] == ' ' || source_[currentPos_] == '\t' || 
         source_[currentPos_] == '\n' || source_[currentPos_] == '{')) {
        EnterStyleBlock(true); // 全局样式
        return;
    }
    
    // 检测全局脚本：script {
    if (Match("script") && currentPos_ < source_.length() && 
        (source_[currentPos_] == ' ' || source_[currentPos_] == '\t' || 
         source_[currentPos_] == '\n' || source_[currentPos_] == '{')) {
        EnterScriptBlock(true); // 全局脚本
        return;
    }
    
    // 检测HTML元素（CHTL块）
    std::string word = "";
    size_t startPos = currentPos_;
    while (!IsAtEnd() && (std::isalnum(CurrentChar()) || CurrentChar() == '-' || CurrentChar() == '_')) {
        word += CurrentChar();
        AdvanceChar();
    }
    
    if (!word.empty()) {
        SkipWhitespace();
        if (!IsAtEnd() && CurrentChar() == '{') {
            // 这是一个CHTL元素块
            currentPos_ = startPos; // 回退
            EnterCHTLBlock();
            return;
        } else {
            // 不是CHTL块，可能是其他内容
            currentPos_ = startPos; // 回退
        }
    }
    
    // 默认：当作CHTL内容收集
    if (currentBuffer_.empty()) {
        StartCollecting();
    }
    currentBuffer_ += CurrentChar();
    AdvanceChar();
}

void CHTLUnifiedScanner::HandleCHTLBlockState() {
    // 在CHTL块内，收集内容直到遇到块结束或嵌套的style/script
    
    // 检测嵌套的局部样式：style {
    size_t stylePos = Find("style");
    if (stylePos != std::string::npos && stylePos == currentPos_) {
        // 推送当前收集的CHTL内容
        if (!currentBuffer_.empty()) {
            PushCollectedFragment(FragmentType::CHTL);
        }
        EnterStyleBlock(false); // 局部样式
        return;
    }
    
    // 检测嵌套的局部脚本：script {
    size_t scriptPos = Find("script");
    if (scriptPos != std::string::npos && scriptPos == currentPos_) {
        // 推送当前收集的CHTL内容
        if (!currentBuffer_.empty()) {
            PushCollectedFragment(FragmentType::CHTL);
        }
        EnterScriptBlock(false); // 局部脚本
        return;
    }
    
    // 检测块结束
    if (CurrentChar() == '}') {
        // 检查是否是CHTL块的结束
        // 简化实现：假设遇到}就是块结束
        if (!currentBuffer_.empty()) {
            currentBuffer_ += CurrentChar();
            AdvanceChar();
            PushCollectedFragment(FragmentType::CHTL);
        } else {
            AdvanceChar();
        }
        ExitCHTLBlock();
        return;
    }
    
    // 继续收集CHTL内容
    if (currentBuffer_.empty()) {
        StartCollecting();
    }
    currentBuffer_ += CurrentChar();
    AdvanceChar();
}

void CHTLUnifiedScanner::HandleStyleBlockState() {
    // 在样式块内，检测CHTL变量模板等切割点位
    
    // 检测变量模板语法：ThemeColor(), Colors()等
    size_t chtlVarLength = DetectVariableTemplate();
    if (chtlVarLength > 0) {
        // 推送前面收集的CSS代码
        if (!currentBuffer_.empty()) {
            PushCollectedFragment(FragmentType::CSS);
        }
        
        // 推送CHTL变量片段
        std::string chtlVar = source_.substr(currentPos_, chtlVarLength);
        PushSyntaxFragment(FragmentType::CHTL, chtlVar, chtlVarLength);
        
        // 继续收集CSS
        StartCollecting();
        return;
    }
    
    // 检测样式块结束
    if (CurrentChar() == '}') {
        // 推送收集的CSS内容
        if (!currentBuffer_.empty()) {
            PushCollectedFragment(FragmentType::CSS);
        }
        AdvanceChar(); // 跳过 }
        ExitStyleBlock();
        return;
    }
    
    // 继续收集CSS内容
    if (currentBuffer_.empty()) {
        StartCollecting();
    }
    currentBuffer_ += CurrentChar();
    AdvanceChar();
}

void CHTLUnifiedScanner::HandleScriptBlockState() {
    // 在脚本块内，检测CHTL JS语法切割点位
    
    // 检测CHTL JS语法
    size_t chtlJSLength = DetectCHTLJSSyntax();
    if (chtlJSLength > 0) {
        // 推送前面收集的JS代码
        if (!currentBuffer_.empty()) {
            PushCollectedFragment(FragmentType::JS);
        }
        
        // 推送CHTL JS片段
        std::string chtlJS = source_.substr(currentPos_, chtlJSLength);
        PushSyntaxFragment(FragmentType::CHTL_JS, chtlJS, chtlJSLength);
        
        // 继续收集JS
        StartCollecting();
        return;
    }
    
    // 检测脚本块结束
    if (CurrentChar() == '}') {
        // 推送收集的JS内容
        if (!currentBuffer_.empty()) {
            PushCollectedFragment(FragmentType::JS);
        }
        AdvanceChar(); // 跳过 }
        ExitScriptBlock();
        return;
    }
    
    // 继续收集JS内容
    if (currentBuffer_.empty()) {
        StartCollecting();
    }
    currentBuffer_ += CurrentChar();
    AdvanceChar();
}

// ============ 切割点位检测实现 ============

size_t CHTLUnifiedScanner::DetectCHTLSyntax() {
    // 检测基本CHTL语法，如元素名、属性等
    if (std::isalpha(CurrentChar())) {
        size_t start = currentPos_;
        while (!IsAtEnd() && (std::isalnum(CurrentChar()) || CurrentChar() == '-' || CurrentChar() == '_')) {
            AdvanceChar();
        }
        size_t length = currentPos_ - start;
        currentPos_ = start; // 回退
        return length;
    }
    return 0;
}

size_t CHTLUnifiedScanner::DetectCHTLJSSyntax() {
    // 检测增强选择器
    size_t selectorLength = DetectEnhancedSelector();
    if (selectorLength > 0) return selectorLength;
    
    // 检测虚对象语法
    size_t virtualLength = DetectVirtualObjectSyntax();
    if (virtualLength > 0) return virtualLength;
    
    return 0;
}

size_t CHTLUnifiedScanner::DetectVariableTemplate() {
    // 检测变量模板语法：ThemeColor(), Colors()等
    if (std::isalpha(CurrentChar())) {
        size_t start = currentPos_;
        
        // 读取标识符
        while (!IsAtEnd() && (std::isalnum(CurrentChar()) || CurrentChar() == '_')) {
            AdvanceChar();
        }
        
        // 检查是否跟着括号
        if (!IsAtEnd() && CurrentChar() == '(') {
            AdvanceChar(); // 跳过 (
            
            // 查找匹配的 )
            int parenCount = 1;
            while (!IsAtEnd() && parenCount > 0) {
                if (CurrentChar() == '(') {
                    parenCount++;
                } else if (CurrentChar() == ')') {
                    parenCount--;
                }
                AdvanceChar();
            }
            
            size_t length = currentPos_ - start;
            currentPos_ = start; // 回退
            return length;
        }
        
        currentPos_ = start; // 回退
    }
    return 0;
}

size_t CHTLUnifiedScanner::DetectEnhancedSelector() {
    // 检测增强选择器：{{selector}}
    if (Match("{{")) {
        size_t start = currentPos_ - 2; // 包含{{
        
        // 查找匹配的}}
        while (!IsAtEnd()) {
            if (Match("}}")) {
                size_t length = currentPos_ - start;
                currentPos_ = start; // 回退
                return length;
            }
            AdvanceChar();
        }
        
        currentPos_ = start; // 回退
    }
    return 0;
}

size_t CHTLUnifiedScanner::DetectVirtualObjectSyntax() {
    // 检测箭头操作符：->
    if (Match("->")) {
        currentPos_ -= 2; // 回退
        return 2;
    }
    
    // 检测监听器语法：listen(
    if (Match("listen")) {
        size_t start = currentPos_ - 6;
        if (!IsAtEnd() && CurrentChar() == '(') {
            AdvanceChar(); // 跳过 (
            
            // 查找匹配的括号
            int parenCount = 1;
            while (!IsAtEnd() && parenCount > 0) {
                if (CurrentChar() == '(') {
                    parenCount++;
                } else if (CurrentChar() == ')') {
                    parenCount--;
                }
                AdvanceChar();
            }
            
            size_t length = currentPos_ - start;
            currentPos_ = start; // 回退
            return length;
        }
        currentPos_ = start; // 回退
    }
    
    // 检测vir声明：vir 对象名
    if (Match("vir ")) {
        size_t start = currentPos_ - 4;
        
        // 读取对象名
        while (!IsAtEnd() && (std::isalnum(CurrentChar()) || CurrentChar() == '_')) {
            AdvanceChar();
        }
        
        size_t length = currentPos_ - start;
        currentPos_ = start; // 回退
        return length;
    }
    
    return 0;
}

// ============ 状态转换实现 ============

void CHTLUnifiedScanner::EnterCHTLBlock() {
    LogDebug("进入CHTL块");
    currentState_ = ScannerState::IN_CHTL_BLOCK;
    StartCollecting();
}

void CHTLUnifiedScanner::ExitCHTLBlock() {
    LogDebug("退出CHTL块");
    currentState_ = ScannerState::GLOBAL;
}

void CHTLUnifiedScanner::EnterStyleBlock(bool isGlobal) {
    LogDebug(isGlobal ? "进入全局样式块" : "进入局部样式块");
    currentState_ = isGlobal ? ScannerState::IN_GLOBAL_STYLE : ScannerState::IN_LOCAL_STYLE;
    
    // 跳过 "style" 和空白字符，找到 {
    while (!IsAtEnd() && CurrentChar() != '{') {
        AdvanceChar();
    }
    if (!IsAtEnd() && CurrentChar() == '{') {
        AdvanceChar(); // 跳过 {
    }
    
    StartCollecting();
}

void CHTLUnifiedScanner::ExitStyleBlock() {
    LogDebug("退出样式块");
    if (currentState_ == ScannerState::IN_GLOBAL_STYLE) {
        currentState_ = ScannerState::GLOBAL;
    } else {
        currentState_ = ScannerState::IN_CHTL_BLOCK;
    }
}

void CHTLUnifiedScanner::EnterScriptBlock(bool isGlobal) {
    LogDebug(isGlobal ? "进入全局脚本块" : "进入局部脚本块");
    currentState_ = isGlobal ? ScannerState::IN_GLOBAL_SCRIPT : ScannerState::IN_LOCAL_SCRIPT;
    
    // 跳过 "script" 和空白字符，找到 {
    while (!IsAtEnd() && CurrentChar() != '{') {
        AdvanceChar();
    }
    if (!IsAtEnd() && CurrentChar() == '{') {
        AdvanceChar(); // 跳过 {
    }
    
    StartCollecting();
}

void CHTLUnifiedScanner::ExitScriptBlock() {
    LogDebug("退出脚本块");
    if (currentState_ == ScannerState::IN_GLOBAL_SCRIPT) {
        currentState_ = ScannerState::GLOBAL;
    } else {
        currentState_ = ScannerState::IN_CHTL_BLOCK;
    }
}

// ============ 片段管理实现 ============

void CHTLUnifiedScanner::StartCollecting() {
    currentBuffer_.clear();
    bufferStartPos_ = currentPos_;
    bufferStartLine_ = currentLine_;
    bufferStartColumn_ = currentColumn_;
}

void CHTLUnifiedScanner::PushCollectedFragment(FragmentType type) {
    if (currentBuffer_.empty()) return;
    
    CodeFragment fragment;
    fragment.type = type;
    fragment.content = currentBuffer_;
    fragment.startPos = bufferStartPos_;
    fragment.endPos = currentPos_;
    fragment.startLine = bufferStartLine_;
    fragment.startColumn = bufferStartColumn_;
    fragment.endLine = currentLine_;
    fragment.endColumn = currentColumn_;
    
    fragments_.push_back(fragment);
    
    LogDebug("推送" + std::to_string(static_cast<int>(type)) + "类型片段: " + 
             currentBuffer_.substr(0, std::min(currentBuffer_.length(), size_t(50))));
    
    currentBuffer_.clear();
}

void CHTLUnifiedScanner::PushSyntaxFragment(FragmentType type, const std::string& content, size_t length) {
    CodeFragment fragment;
    fragment.type = type;
    fragment.content = content;
    fragment.startPos = currentPos_;
    fragment.endPos = currentPos_ + length;
    fragment.startLine = currentLine_;
    fragment.startColumn = currentColumn_;
    fragment.endLine = currentLine_;
    fragment.endColumn = currentColumn_ + length;
    
    fragments_.push_back(fragment);
    
    LogDebug("推送" + std::to_string(static_cast<int>(type)) + "语法片段: " + content);
    
    Advance(length);
}

// ============ 工具方法实现 ============

char CHTLUnifiedScanner::CurrentChar() {
    if (currentPos_ >= source_.length()) return '\0';
    return source_[currentPos_];
}

void CHTLUnifiedScanner::AdvanceChar() {
    if (currentPos_ < source_.length()) {
        if (source_[currentPos_] == '\n') {
            currentLine_++;
            currentColumn_ = 1;
        } else {
            currentColumn_++;
        }
        currentPos_++;
    }
}

void CHTLUnifiedScanner::Advance(size_t length) {
    for (size_t i = 0; i < length && !IsAtEnd(); ++i) {
        AdvanceChar();
    }
}

bool CHTLUnifiedScanner::IsAtEnd() {
    return currentPos_ >= source_.length();
}

void CHTLUnifiedScanner::SkipWhitespace() {
    while (!IsAtEnd() && std::isspace(CurrentChar())) {
        AdvanceChar();
    }
}

bool CHTLUnifiedScanner::Match(const std::string& str) {
    if (currentPos_ + str.length() > source_.length()) {
        return false;
    }
    
    if (source_.substr(currentPos_, str.length()) == str) {
        Advance(str.length());
        return true;
    }
    
    return false;
}

size_t CHTLUnifiedScanner::Find(const std::string& str) {
    size_t pos = source_.find(str, currentPos_);
    if (pos == std::string::npos) {
        return std::string::npos;
    }
    return pos;
}

void CHTLUnifiedScanner::LogDebug(const std::string& message) {
    if (verbose_) {
        std::cout << "[Scanner] " << message << std::endl;
    }
}

// ============ 新的扫描策略实现 ============

void CHTLUnifiedScanner::SlidingWindowScan() {
    LogDebug("开始双指针滑动窗口扫描");
    
    // 1. 第一步：初始扫描，避免语法片段位于开头被前指针错过
    InitialScan();
    
    // 2. 第二步：移动前指针到初始扫描结束位置
    slidingState_.frontPointer = (slidingState_.initialScanComplete) ? 
        std::min(size_t(1000), source_.length()) : 0;
    slidingState_.backPointer = 0;
    
    LogDebug("初始扫描完成，前指针位置: " + std::to_string(slidingState_.frontPointer));
    
    // 3. 主扫描循环：前指针和后指针一同向前移动
    while (slidingState_.frontPointer < source_.length()) {
        std::string keyword;
        
        // 前指针检测关键字
        if (DetectKeywordAt(slidingState_.frontPointer, keyword)) {
            LogDebug("前指针在位置 " + std::to_string(slidingState_.frontPointer) + 
                    " 检测到关键字: " + keyword);
            
            // 通知后指针进入收集状态
            if (!slidingState_.collectMode) {
                // 推送前面的普通代码片段
                if (slidingState_.backPointer < slidingState_.frontPointer) {
                    ExtractAndProcessFromWindow(slidingState_.backPointer, slidingState_.frontPointer, FragmentType::JS);
                }
                
                // 进入收集模式
                slidingState_.collectMode = true;
                slidingState_.collectBuffer.clear();
                LogDebug("后指针进入收集状态");
            }
        }
        
        // 如果在收集模式中，收集CHTL JS语法片段
        if (slidingState_.collectMode) {
            size_t syntaxLength = DetectCHTLJSSyntaxAt(slidingState_.frontPointer);
            if (syntaxLength > 0) {
                // 收集CHTL JS语法片段
                std::string syntaxFragment = source_.substr(slidingState_.frontPointer, syntaxLength);
                slidingState_.collectBuffer += syntaxFragment;
                
                LogDebug("收集CHTL JS语法片段: " + syntaxFragment);
                
                // 移动前指针跳过这个语法片段
                slidingState_.frontPointer += syntaxLength;
                
                // 检查是否为语法结束
                if (syntaxFragment.find('}') != std::string::npos || 
                    syntaxFragment.find(';') != std::string::npos) {
                    // 推送收集的CHTL JS片段
                    PushFragment(FragmentType::CHTL_JS, slidingState_.collectBuffer, 
                               slidingState_.backPointer, slidingState_.frontPointer);
                    
                    // 退出收集模式，更新后指针
                    slidingState_.collectMode = false;
                    slidingState_.backPointer = slidingState_.frontPointer;
                    slidingState_.collectBuffer.clear();
                    
                    LogDebug("完成CHTL JS片段收集，后指针更新到位置: " + 
                            std::to_string(slidingState_.backPointer));
                }
                continue;
            }
        }
        
        // 前指针和后指针一同向前移动（滑动窗口特征）
        slidingState_.frontPointer++;
        
        // 如果不在收集模式，后指针也同步移动
        if (!slidingState_.collectMode) {
            slidingState_.backPointer = slidingState_.frontPointer;
        }
    }
    
    // 处理剩余内容
    if (slidingState_.backPointer < source_.length()) {
        if (slidingState_.collectMode && !slidingState_.collectBuffer.empty()) {
            // 推送未完成的CHTL JS片段
            PushFragment(FragmentType::CHTL_JS, slidingState_.collectBuffer, 
                       slidingState_.backPointer, source_.length());
        } else {
            // 推送普通代码片段
            ExtractAndProcessFromWindow(slidingState_.backPointer, source_.length(), FragmentType::JS);
        }
    }
    
    LogDebug("双指针滑动窗口扫描完成");
}

void CHTLUnifiedScanner::FrontExtractScan() {
    LogDebug("开始前置代码截取扫描");
    
    size_t currentPos = 0;
    
    while (currentPos < source_.length()) {
        std::string keyword;
        size_t keywordPos = FindNextKeyword(currentPos, keyword);
        
        if (keywordPos != std::string::npos) {
            LogDebug("在位置 " + std::to_string(keywordPos) + " 发现关键字: " + keyword);
            
            // 截取前置代码段（避免发送给编译器）
            if (keywordPos > currentPos) {
                std::string frontSegment = ExtractFrontSegment(keywordPos);
                ProcessExtractedSegment(frontSegment, FragmentType::JS);
                LogDebug("截取前置代码段，长度: " + std::to_string(frontSegment.length()));
            }
            
            // 处理CHTL JS语法片段
            size_t syntaxLength = DetectCHTLJSSyntaxAt(keywordPos);
            if (syntaxLength > 0) {
                std::string syntaxFragment = source_.substr(keywordPos, syntaxLength);
                PushFragment(FragmentType::CHTL_JS, syntaxFragment, keywordPos, keywordPos + syntaxLength);
                
                LogDebug("处理CHTL JS语法片段: " + syntaxFragment);
                currentPos = keywordPos + syntaxLength;
            } else {
                // 不是有效的CHTL JS语法，作为普通代码处理
                currentPos = keywordPos + keyword.length();
            }
        } else {
            // 没有更多关键字，处理剩余代码
            if (currentPos < source_.length()) {
                std::string remainingSegment = source_.substr(currentPos);
                ProcessExtractedSegment(remainingSegment, FragmentType::JS);
                LogDebug("处理剩余代码段，长度: " + std::to_string(remainingSegment.length()));
            }
            break;
        }
    }
    
    LogDebug("前置代码截取扫描完成");
}

void CHTLUnifiedScanner::InitialScan(size_t searchRange) {
    LogDebug("开始初始扫描，搜索范围: " + std::to_string(searchRange));
    
    size_t endPos = std::min(searchRange, source_.length());
    
    for (size_t pos = 0; pos < endPos; ++pos) {
        std::string keyword;
        if (DetectKeywordAt(pos, keyword)) {
            LogDebug("初始扫描在位置 " + std::to_string(pos) + " 发现关键字: " + keyword);
            
            // 检测并处理CHTL JS语法
            size_t syntaxLength = DetectCHTLJSSyntaxAt(pos);
            if (syntaxLength > 0) {
                // 推送前面的代码（如果有）
                if (pos > 0) {
                    PushFragment(FragmentType::JS, source_.substr(0, pos), 0, pos);
                }
                
                // 推送CHTL JS语法片段
                std::string syntaxFragment = source_.substr(pos, syntaxLength);
                PushFragment(FragmentType::CHTL_JS, syntaxFragment, pos, pos + syntaxLength);
                
                LogDebug("初始扫描处理CHTL JS片段: " + syntaxFragment);
            }
        }
    }
    
    slidingState_.initialScanComplete = true;
    LogDebug("初始扫描完成");
}

void CHTLUnifiedScanner::TraditionalScan() {
    LogDebug("使用传统扫描方式");
    
    // 原有的扫描逻辑
    while (!IsAtEnd()) {
        switch (currentState_) {
            case ScannerState::GLOBAL:
                HandleGlobalState();
                break;
            case ScannerState::IN_CHTL_BLOCK:
                HandleCHTLBlockState();
                break;
            case ScannerState::IN_GLOBAL_STYLE:
            case ScannerState::IN_LOCAL_STYLE:
                HandleStyleBlockState();
                break;
            case ScannerState::IN_GLOBAL_SCRIPT:
            case ScannerState::IN_LOCAL_SCRIPT:
                HandleScriptBlockState();
                break;
            case ScannerState::COLLECTING_CSS:
            case ScannerState::COLLECTING_JS:
                // 在CSS/JS收集状态下，继续检测CHTL切割点位
                if (currentState_ == ScannerState::COLLECTING_CSS) {
                    HandleStyleBlockState();
                } else {
                    HandleScriptBlockState();
                }
                break;
        }
    }
}

// ============ 状态重置方法 ============

void CHTLUnifiedScanner::ResetSlidingWindowState() {
    slidingState_.frontPointer = 0;
    slidingState_.backPointer = 0;
    slidingState_.collectMode = false;
    slidingState_.collectBuffer.clear();
    slidingState_.initialScanComplete = false;
}

void CHTLUnifiedScanner::ResetFrontExtractState() {
    extractState_.extractedSegments.clear();
    extractState_.currentSegmentPos = 0;
    extractState_.extractionActive = false;
}

// ============ 关键字检测方法 ============

bool CHTLUnifiedScanner::DetectKeywordAt(size_t position, std::string& keyword) {
    for (const auto& kw : registeredKeywords_) {
        if (MatchAt(position, kw)) {
            keyword = kw;
            return true;
        }
    }
    return false;
}

size_t CHTLUnifiedScanner::DetectCHTLJSSyntaxAt(size_t position) {
    // 首先检测是否为"--"注释
    size_t commentLength = DetectDashComment(position);
    if (commentLength > 0) {
        return commentLength;
    }
    
    // 保存当前位置
    size_t savedPos = currentPos_;
    currentPos_ = position;
    
    // 使用现有的CHTL JS语法检测
    size_t length = DetectCHTLJSSyntax();
    
    // 恢复位置
    currentPos_ = savedPos;
    
    return length;
}

size_t CHTLUnifiedScanner::FindNextKeyword(size_t startPos, std::string& keyword) {
    size_t nearestPos = std::string::npos;
    std::string nearestKeyword;
    
    for (const auto& kw : registeredKeywords_) {
        size_t pos = source_.find(kw, startPos);
        if (pos != std::string::npos && (nearestPos == std::string::npos || pos < nearestPos)) {
            nearestPos = pos;
            nearestKeyword = kw;
        }
    }
    
    if (nearestPos != std::string::npos) {
        keyword = nearestKeyword;
    }
    
    return nearestPos;
}

// ============ 代码片段处理方法 ============

void CHTLUnifiedScanner::ExtractAndProcessFromWindow(size_t start, size_t end, FragmentType type) {
    if (start >= end || start >= source_.length()) return;
    
    end = std::min(end, source_.length());
    std::string content = source_.substr(start, end - start);
    
    if (!content.empty()) {
        PushFragment(type, content, start, end);
        LogDebug("从滑动窗口提取片段，类型: " + std::to_string(static_cast<int>(type)) + 
                ", 长度: " + std::to_string(content.length()));
    }
}

std::string CHTLUnifiedScanner::ExtractFrontSegment(size_t endPos) {
    if (endPos > extractState_.currentSegmentPos && extractState_.currentSegmentPos < source_.length()) {
        endPos = std::min(endPos, source_.length());
        std::string segment = source_.substr(extractState_.currentSegmentPos, endPos - extractState_.currentSegmentPos);
        extractState_.currentSegmentPos = endPos;
        return segment;
    }
    return "";
}

void CHTLUnifiedScanner::ProcessExtractedSegment(const std::string& segment, FragmentType segmentType) {
    if (segment.empty()) return;
    
    // 存储截取的代码段，确保不发送给编译器
    extractState_.extractedSegments.push_back(segment);
    
    // 创建片段记录，但标记为已截取，不会被发送到编译器
    PushFragment(segmentType, segment, extractState_.currentSegmentPos - segment.length(), extractState_.currentSegmentPos);
    
    LogDebug("处理截取的代码段（不发送给编译器），长度: " + std::to_string(segment.length()));
}

// ============ 新增工具方法 ============

char CHTLUnifiedScanner::CharAt(size_t pos) {
    if (pos >= source_.length()) return '\0';
    return source_[pos];
}

bool CHTLUnifiedScanner::IsAtEnd(size_t pos) {
    return pos >= source_.length();
}

bool CHTLUnifiedScanner::MatchAt(size_t pos, const std::string& str) {
    if (pos + str.length() > source_.length()) {
        return false;
    }
    
    return source_.substr(pos, str.length()) == str;
}

void CHTLUnifiedScanner::PushFragment(FragmentType type, const std::string& content, size_t startPos, size_t endPos) {
    CodeFragment fragment;
    fragment.type = type;
    fragment.content = content;
    fragment.startPos = startPos;
    fragment.endPos = endPos;
    fragment.startLine = 1; // 简化实现，可以后续改进
    fragment.startColumn = startPos + 1;
    fragment.endLine = 1;
    fragment.endColumn = endPos + 1;
    
    fragments_.push_back(fragment);
    
    LogDebug("推送片段，类型: " + std::to_string(static_cast<int>(type)) + 
            ", 内容: " + content.substr(0, std::min(content.length(), size_t(50))));
}

void CHTLUnifiedScanner::RegisterKeyword(const std::string& keyword) {
    registeredKeywords_.insert(keyword);
    LogDebug("注册关键字: " + keyword);
}

void CHTLUnifiedScanner::ClearKeywords() {
    registeredKeywords_.clear();
    LogDebug("清空所有关键字");
}

// ============ "--"注释处理方法 ============

size_t CHTLUnifiedScanner::DetectDashComment(size_t position) {
    if (position + 2 > source_.length()) {
        return 0;
    }
    
    // 检查是否为"--"开头
    if (source_.substr(position, 2) != "--") {
        return 0;
    }
    
    // 查找注释结束位置（行尾或文件尾）
    size_t endPos = position + 2;
    while (endPos < source_.length() && source_[endPos] != '\n' && source_[endPos] != '\r') {
        endPos++;
    }
    
    return endPos - position;
}

std::string CHTLUnifiedScanner::ProcessDashComment(const std::string& comment, const std::string& surroundingCode) {
    // 去掉"--"前缀
    std::string commentContent = comment.substr(2);
    
    // 去掉首尾空白
    size_t start = commentContent.find_first_not_of(" \t");
    if (start == std::string::npos) {
        commentContent = "";
    } else {
        size_t end = commentContent.find_last_not_of(" \t");
        commentContent = commentContent.substr(start, end - start + 1);
    }
    
    // 检测上下文
    Comments::ProgrammingContext context = commentGenerator_.DetectContext(surroundingCode);
    
    // 根据上下文生成注释
    std::string processedComment = commentGenerator_.ProcessDashComment(commentContent, context);
    
    LogDebug("处理\"--\"注释，上下文: " + std::to_string(static_cast<int>(context)) + 
            ", 原始: " + comment + ", 处理后: " + processedComment);
    
    return processedComment;
}

} // namespace Scanner
} // namespace CHTL