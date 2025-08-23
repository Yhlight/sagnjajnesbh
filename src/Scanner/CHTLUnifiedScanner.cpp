#include "Scanner/CHTLUnifiedScanner.h"
#include "CHTL/Constraints/CHTLConstraintValidator.h"
#include "Utils/ErrorHandler.h"
#include "Utils/StringUtils.h"
#include <iostream>
#include <regex>

namespace CHTL {
namespace Scanner {

CHTLUnifiedScanner::CHTLUnifiedScanner() 
    : currentState_(ScannerState::GLOBAL), currentPos_(0), currentLine_(1), 
      currentColumn_(1), bufferStartPos_(0), bufferStartLine_(1), 
      bufferStartColumn_(1), verbose_(false) {
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

} // namespace Scanner
} // namespace CHTL