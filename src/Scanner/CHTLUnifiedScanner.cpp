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
    
    // 只注册基础的CHTL JS语法关键字，不注册CJMOD扩展
    RegisterKeyword("vir");          // 虚对象声明关键字
    RegisterKeyword("listen");       // CHTL JS原生监听器
    RegisterKeyword("delegate");     // CHTL JS原生事件委托
    RegisterKeyword("animate");      // CHTL JS原生动画
    
    // CJMOD扩展关键字将通过Import动态注册
    // RegisterKeyword("printMylove");   // ❌ 移除：这是CJMOD扩展
    // RegisterKeyword("iNeverAway");    // ❌ 移除：这是CJMOD扩展
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
    
    // 构建片段索引
    if (!fragments_.empty()) {
        BuildFragmentIndex(fragments_);
        
        if (verbose_) {
            PrintIndexStatistics(fragments_);
        }
    }
    
    return fragments_;
}

void CHTLUnifiedScanner::ScanLoop() {
    LogDebug("开始可变长度切片扫描");
    
    // 实现可变长度切片机制
    size_t fragmentStart = 0;
    
    while (fragmentStart < source_.length()) {
        // 1. 读取一个片段
        size_t fragmentEnd = FindNextSyntaxBoundary(fragmentStart);
        
        if (fragmentEnd == std::string::npos || fragmentEnd <= fragmentStart) {
            // 处理剩余内容
            if (fragmentStart < source_.length()) {
                std::string remainingContent = source_.substr(fragmentStart);
                if (!Utils::StringUtils::Trim(remainingContent).empty()) {
                    PushFragment(DetermineFragmentType(remainingContent), remainingContent, 
                               fragmentStart, source_.length());
                }
            }
            break;
        }
        
        // 2. 检查片段完整性
        if (!IsCompleteCodeFragment(fragmentStart, fragmentEnd)) {
            LogDebug("片段不完整，尝试扩展边界: " + std::to_string(fragmentStart) + "-" + std::to_string(fragmentEnd));
            
            // 3. 动态扩展边界
            if (!ExpandFragmentBoundary(fragmentStart, fragmentEnd)) {
                LogDebug("无法扩展边界，使用当前位置");
            }
        }
        
        // 4. 提取完整片段
        std::string fragmentContent = source_.substr(fragmentStart, fragmentEnd - fragmentStart);
        fragmentContent = Utils::StringUtils::Trim(fragmentContent);
        
        if (!fragmentContent.empty()) {
            // 5. 检查是否需要最小单元切割
            if (NeedsMinimalUnitSlicing(fragmentContent)) {
                LogDebug("执行最小单元切割");
                std::vector<CodeFragment> subFragments;
                PerformMinimalUnitSlicing(fragmentContent, subFragments);
                
                // 添加子片段
                for (auto& subFragment : subFragments) {
                    subFragment.startPos += fragmentStart;
                    subFragment.endPos += fragmentStart;
                    fragments_.push_back(subFragment);
                }
            } else {
                // 6. 直接添加完整片段
                FragmentType type = DetermineFragmentType(fragmentContent);
                PushFragment(type, fragmentContent, fragmentStart, fragmentEnd);
            }
        }
        
        // 7. 移动到下一个片段
        fragmentStart = fragmentEnd;
        
        // 跳过空白字符
        while (fragmentStart < source_.length() && std::isspace(source_[fragmentStart])) {
            fragmentStart++;
        }
    }
    
    LogDebug("可变长度切片扫描完成，共生成 " + std::to_string(fragments_.size()) + " 个片段");
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
            std::cout << "[DEBUG] 检测到CHTL元素: " << word << std::endl;
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
                    ExtractAndProcessFromWindow(slidingState_.backPointer, slidingState_.frontPointer, FragmentType::CHTL);
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
            ExtractAndProcessFromWindow(slidingState_.backPointer, source_.length(), FragmentType::CHTL);
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
                ProcessExtractedSegment(frontSegment, FragmentType::CHTL);
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
                ProcessExtractedSegment(remainingSegment, FragmentType::CHTL);
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
                    PushFragment(FragmentType::CHTL, source_.substr(0, pos), 0, pos);
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
    std::cout << "[DEBUG] 开始传统扫描" << std::endl;
    
    // 使用可变长度切片机制进行智能扫描
    size_t currentPos = 0;
    
    while (currentPos < source_.length()) {
        // 1. 确定初始片段边界
        size_t fragmentStart = currentPos;
        size_t fragmentEnd = std::min(currentPos + 200, source_.length()); // 初始片段大小200字符
        
        LogDebug("初始片段范围: " + std::to_string(fragmentStart) + "-" + std::to_string(fragmentEnd));
        
        // 2. 检查片段完整性并动态扩增
        if (!IsCompleteCodeFragment(fragmentStart, fragmentEnd)) {
            LogDebug("片段不完整，开始动态扩增");
            if (!ExpandFragmentBoundary(fragmentStart, fragmentEnd)) {
                // 如果无法扩增到完整片段，使用语法边界
                fragmentEnd = FindNextSyntaxBoundary(fragmentStart);
            }
        }
        
        // 3. 提取片段内容
        std::string fragmentContent = source_.substr(fragmentStart, fragmentEnd - fragmentStart);
        
        // 4. 进行最小单元切割（如果需要）
        std::vector<CodeFragment> subFragments;
        if (NeedsMinimalUnitSlicing(fragmentContent)) {
            PerformMinimalUnitSlicing(fragmentContent, subFragments);
            
            // 推送所有子片段
            for (auto& subFragment : subFragments) {
                subFragment.startPos += fragmentStart; // 调整位置
                fragments_.push_back(subFragment);
                LogDebug("推送子片段，类型: " + std::to_string(static_cast<int>(subFragment.type)) + 
                        ", 长度: " + std::to_string(subFragment.content.length()));
            }
        } else {
            // 5. 直接推送完整片段
            CodeFragment fragment;
            fragment.content = fragmentContent;
            fragment.startPos = fragmentStart;
            fragment.type = DetermineFragmentType(fragmentContent);
            
            fragments_.push_back(fragment);
            LogDebug("推送完整片段，类型: " + std::to_string(static_cast<int>(fragment.type)) + 
                    ", 长度: " + std::to_string(fragment.content.length()));
        }
        
        // 6. 移动到下一个片段
        currentPos = fragmentEnd;
    }
    
    LogDebug("可变长度切片扫描完成，共生成 " + std::to_string(fragments_.size()) + " 个片段");
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

// ============ 可变长度切片机制实现 ============

bool CHTLUnifiedScanner::IsCompleteCodeFragment(size_t startPos, size_t endPos) {
    if (startPos >= endPos || endPos > source_.length()) {
        return false;
    }
    
    std::string content = source_.substr(startPos, endPos - startPos);
    content = Utils::StringUtils::Trim(content);
    
    if (content.empty()) {
        return false;
    }
    
    // 根据内容类型进行完整性检测
    if (IsHTMLElement(content)) {
        // CHTL元素完整性检测
        return IsCHTLBlockComplete(content);
    }
    
    if (IsStyleBlock(content)) {
        // 样式块完整性检测
        return IsStyleBlockComplete(content);
    }
    
    if (IsScriptBlock(content)) {
        // 脚本块完整性检测
        return IsScriptBlockComplete(content);
    }
    
    if (IsVirObject(content) || HasCHTLJSSyntax(content)) {
        // CHTL JS片段完整性检测
        return IsCHTLJSFragmentComplete(content);
    }
    
    // 检查基本的语法平衡
    return CalculateBraceBalance(content) == 0;
}

bool CHTLUnifiedScanner::ExpandFragmentBoundary(size_t startPos, size_t& endPos) {
    LogDebug("开始动态扩增片段边界，当前范围: " + std::to_string(startPos) + "-" + std::to_string(endPos));
    
    size_t originalEndPos = endPos;
    size_t maxExpansion = std::min(source_.length(), endPos + 500); // 最大扩增500字符
    
    // 逐步扩增，直到找到完整的代码片段
    for (size_t newEndPos = endPos + 1; newEndPos <= maxExpansion; ++newEndPos) {
        if (IsCompleteCodeFragment(startPos, newEndPos)) {
            endPos = newEndPos;
            LogDebug("成功扩增片段边界到: " + std::to_string(endPos) + 
                    " (扩增了 " + std::to_string(endPos - originalEndPos) + " 字符)");
            return true;
        }
    }
    
    LogDebug("无法找到完整的片段边界，保持原始位置");
    return false;
}

bool CHTLUnifiedScanner::IsCHTLBlockComplete(const std::string& content) const {
    // 检查CHTL块的完整性
    int braceBalance = CalculateBraceBalance(content);
    
    // CHTL块必须有平衡的大括号
    if (braceBalance != 0) {
        return false;
    }
    
    // 检查是否有基本的CHTL元素结构
    bool hasOpeningElement = false;
    bool hasClosingBrace = false;
    
    // 简单的结构检查
    size_t openBrace = content.find('{');
    size_t closeBrace = content.rfind('}');
    
    if (openBrace != std::string::npos && closeBrace != std::string::npos && 
        openBrace < closeBrace) {
        hasOpeningElement = true;
        hasClosingBrace = true;
    }
    
    return hasOpeningElement && hasClosingBrace;
}

bool CHTLUnifiedScanner::IsCHTLJSFragmentComplete(const std::string& content) const {
    // 检查CHTL JS片段的完整性
    
    // 检查vir语法的完整性
    if (content.find("vir") == 0) {
        // vir对象必须有完整的结构：vir name { ... }
        size_t openBrace = content.find('{');
        size_t closeBrace = content.rfind('}');
        
        if (openBrace == std::string::npos || closeBrace == std::string::npos) {
            return false;
        }
        
        return CalculateBraceBalance(content) == 0;
    }
    
    // 检查{{}}语法的完整性
    if (content.find("{{") != std::string::npos) {
        size_t openCount = 0;
        size_t closeCount = 0;
        
        for (size_t i = 0; i < content.length() - 1; ++i) {
            if (content[i] == '{' && content[i + 1] == '{') {
                openCount++;
                i++; // 跳过下一个字符
            } else if (content[i] == '}' && content[i + 1] == '}') {
                closeCount++;
                i++; // 跳过下一个字符
            }
        }
        
        return openCount == closeCount;
    }
    
    return true;
}

void CHTLUnifiedScanner::PerformMinimalUnitSlicing(const std::string& content, 
                                                  std::vector<CodeFragment>& fragments) {
    LogDebug("开始智能最小单元切割，内容长度: " + std::to_string(content.length()));
    
    // 检测上下文环境
    bool inScriptBlock = content.find("script") != std::string::npos;
    bool inStyleBlock = content.find("style") != std::string::npos;
    
    size_t pos = 0;
    size_t startPos = 0;
    
    while (pos < content.length()) {
        bool foundCutPoint = false;
        size_t cutEndPos = pos;
        FragmentType cutType = FragmentType::CHTL;
        
        // 1. 检测虚对象语法 vir objectName = CHTL JS函数
        if (pos + 3 < content.length() && content.substr(pos, 3) == "vir") {
            size_t endPos = pos + 3;
            // 跳过空白和对象名
            while (endPos < content.length() && (std::isspace(content[endPos]) || std::isalnum(content[endPos]) || content[endPos] == '_')) {
                endPos++;
            }
            // 找到对象体的结束
            if (endPos < content.length() && content[endPos] == '{') {
                int braceCount = 1;
                endPos++;
                while (endPos < content.length() && braceCount > 0) {
                    if (content[endPos] == '{') braceCount++;
                    else if (content[endPos] == '}') braceCount--;
                    endPos++;
                }
            }
            cutEndPos = endPos;
            cutType = FragmentType::CHTL_JS;
            foundCutPoint = true;
        }
        
        // 2. 检测增强选择器语法 {{selector}}
        else if (pos + 2 < content.length() && content.substr(pos, 2) == "{{") {
            size_t endPos = content.find("}}", pos + 2);
            if (endPos != std::string::npos) {
                endPos += 2;
                
                // 检查是否跟着箭头操作符
                if (endPos + 2 < content.length() && content.substr(endPos, 2) == "->") {
                    endPos += 2;
                    // 继续查找方法名
                    while (endPos < content.length() && (std::isalnum(content[endPos]) || content[endPos] == '_')) {
                        endPos++;
                    }
                }
                
                cutEndPos = endPos;
                cutType = FragmentType::CHTL_JS;
                foundCutPoint = true;
            }
        }
        
        // 3. 检测箭头操作符 object->method
        else if (pos + 2 < content.length() && content.substr(pos, 2) == "->") {
            // 向前查找对象名
            size_t objectStart = pos;
            while (objectStart > 0 && (std::isalnum(content[objectStart - 1]) || content[objectStart - 1] == '_')) {
                objectStart--;
            }
            
            // 向后查找方法名
            size_t methodEnd = pos + 2;
            while (methodEnd < content.length() && (std::isalnum(content[methodEnd]) || content[methodEnd] == '_')) {
                methodEnd++;
            }
            
            // 检查是否有参数
            if (methodEnd < content.length() && content[methodEnd] == '(') {
                int parenCount = 1;
                methodEnd++;
                while (methodEnd < content.length() && parenCount > 0) {
                    if (content[methodEnd] == '(') parenCount++;
                    else if (content[methodEnd] == ')') parenCount--;
                    methodEnd++;
                }
            }
            
            cutEndPos = methodEnd;
            cutType = FragmentType::CHTL_JS;
            foundCutPoint = true;
        }
        
        // 4. 检测CJMOD关键字（动态注册的）
        if (!foundCutPoint) {
            for (const auto& pair : cjmodKeywords_) {
                const std::string& keyword = pair.first;
                if (pos + keyword.length() <= content.length() && 
                    content.substr(pos, keyword.length()) == keyword) {
                    
                    size_t endPos = pos + keyword.length();
                    
                    // 跳过空白
                    while (endPos < content.length() && std::isspace(content[endPos])) {
                        endPos++;
                    }
                    
                    // 查找参数块
                    if (endPos < content.length() && content[endPos] == '(') {
                        int parenCount = 1;
                        endPos++;
                        while (endPos < content.length() && parenCount > 0) {
                            if (content[endPos] == '(') parenCount++;
                            else if (content[endPos] == ')') parenCount--;
                            endPos++;
                        }
                    }
                    
                    cutEndPos = endPos;
                    cutType = FragmentType::CHTL_JS;
                    foundCutPoint = true;
                    break;
                }
            }
        }
        
        // 5. 在script块中检测纯JavaScript语法
        if (!foundCutPoint && inScriptBlock) {
            // 检测console.log, let, const, var等JavaScript关键字
            std::vector<std::string> jsKeywords = {"console.", "let ", "const ", "var ", "function ", "document."};
            for (const auto& keyword : jsKeywords) {
                if (pos + keyword.length() <= content.length() && 
                    content.substr(pos, keyword.length()) == keyword) {
                    // 找到语句结束（分号或换行）
                    size_t endPos = pos + keyword.length();
                    while (endPos < content.length() && content[endPos] != ';' && content[endPos] != '\n') {
                        endPos++;
                    }
                    if (endPos < content.length()) endPos++; // 包含分号或换行
                    
                    cutEndPos = endPos;
                    cutType = FragmentType::JS;
                    foundCutPoint = true;
                    break;
                }
            }
        }
        
        // 4. 在style块中检测CHTL变量模板
        else if (inStyleBlock) {
            // 检测ThemeColor(), Colors()等CHTL变量模板
            std::vector<std::string> chtlVars = {"ThemeColor()", "Colors()", "Spacing()"};
            for (const auto& var : chtlVars) {
                if (pos + var.length() <= content.length() && 
                    content.substr(pos, var.length()) == var) {
                    cutEndPos = pos + var.length();
                    cutType = FragmentType::CHTL;
                    foundCutPoint = true;
                    break;
                }
            }
        }
        
        if (foundCutPoint) {
            // 推送前面的内容（如果有）
            if (pos > startPos) {
                std::string prevContent = content.substr(startPos, pos - startPos);
                if (!prevContent.empty()) {
                    CodeFragment fragment;
                    fragment.content = prevContent;
                    fragment.type = DetermineFragmentTypeInContext(prevContent, inScriptBlock, inStyleBlock);
                    fragment.startPos = startPos;
                    fragments.push_back(fragment);
                }
            }
            
            // 推送切割出的片段
            std::string cutContent = content.substr(pos, cutEndPos - pos);
            CodeFragment fragment;
            fragment.content = cutContent;
            fragment.type = cutType;
            fragment.startPos = pos;
            fragments.push_back(fragment);
            
            pos = cutEndPos;
            startPos = pos;
        } else {
            pos++;
        }
    }
    
    // 推送剩余内容
    if (startPos < content.length()) {
        std::string remainingContent = content.substr(startPos);
        if (!remainingContent.empty()) {
            CodeFragment fragment;
            fragment.content = remainingContent;
            fragment.type = DetermineFragmentTypeInContext(remainingContent, inScriptBlock, inStyleBlock);
            fragment.startPos = startPos;
            fragments.push_back(fragment);
        }
    }
    
    LogDebug("智能最小单元切割完成，生成 " + std::to_string(fragments.size()) + " 个片段");
}

bool CHTLUnifiedScanner::IsValidCHTLJSSyntax(const std::string& content) const {
    std::string trimmed = content;
    
    // 移除前后空白
    size_t start = trimmed.find_first_not_of(" \t\n\r");
    if (start == std::string::npos) return false;
    size_t end = trimmed.find_last_not_of(" \t\n\r");
    trimmed = trimmed.substr(start, end - start + 1);
    
    // 1. 检查虚对象声明: vir objectName = {...}
    std::regex virPattern(R"(^\s*vir\s+\w+\s*=\s*\{[\s\S]*\}\s*$)");
    if (std::regex_match(trimmed, virPattern)) {
        return true;
    }
    
    // 2. 检查增强选择器调用: {{selector}}->method(...)
    std::regex selectorPattern(R"(^\s*\{\{[^}]+\}\}\s*->\s*\w+\s*(\([^)]*\))?\s*;?\s*$)");
    if (std::regex_match(trimmed, selectorPattern)) {
        return true;
    }
    
    // 3. 检查虚对象方法调用: object->method(...) (CHTL JS专属特征)
    std::regex methodPattern(R"(^\s*\w+\s*->\s*\w+\s*(\([^)]*\))?\s*;?\s*$)");
    if (std::regex_match(trimmed, methodPattern)) {
        return true;
    }
    
    // 4. 检查CHTL JS特定方法: listen(...), delegate(...), animate(...)
    std::regex chtlJSFuncPattern(R"(^\s*(listen|delegate|animate)\s*\([^)]*\)\s*;?\s*$)");
    if (std::regex_match(trimmed, chtlJSFuncPattern)) {
        return true;
    }
    
    // 5. 检查CJMOD关键字调用
    for (const auto& pair : cjmodKeywords_) {
        const std::string& keyword = pair.first;
        std::regex cjmodPattern("^\\s*" + keyword + "\\s*\\([^)]*\\)\\s*;?\\s*$");
        if (std::regex_match(trimmed, cjmodPattern)) {
            return true;
        }
    }
    
    // 6. 检查包含Import语句的片段
    if (trimmed.find("[Import]") != std::string::npos) {
        return true;
    }
    
    return false;
}

FragmentType CHTLUnifiedScanner::DetermineFragmentTypeInContext(const std::string& content, 
                                                               bool inScriptBlock, 
                                                               bool inStyleBlock) {
    std::string trimmed = content;
    // 移除前后空白
    size_t start = trimmed.find_first_not_of(" \t\n\r");
    if (start == std::string::npos) return FragmentType::Unknown;
    
    size_t end = trimmed.find_last_not_of(" \t\n\r");
    trimmed = trimmed.substr(start, end - start + 1);
    
    // 在script块内的上下文判断
    if (inScriptBlock) {
        // 更严格的CHTL JS语法检测
        if (IsValidCHTLJSSyntax(trimmed)) {
            LogDebug("Script块内识别为CHTL JS片段: " + trimmed.substr(0, 20) + "...");
            return FragmentType::CHTL_JS;
        }
        
        // 检测纯JavaScript语法
        if (trimmed.find("console.") != std::string::npos ||
            trimmed.find("let ") == 0 || trimmed.find("const ") == 0 || 
            trimmed.find("var ") == 0 || trimmed.find("function ") == 0 ||
            trimmed.find("document.") != std::string::npos) {
            LogDebug("Script块内识别为JavaScript片段: " + trimmed.substr(0, 20) + "...");
            return FragmentType::JS;
        }
        
        // 默认为CHTL JS（在script块内）
        LogDebug("Script块内默认识别为CHTL JS片段: " + trimmed.substr(0, 20) + "...");
        return FragmentType::CHTL_JS;
    }
    
    // 在style块内的上下文判断
    if (inStyleBlock) {
        // 检测CHTL变量模板
        if (trimmed.find("ThemeColor()") != std::string::npos ||
            trimmed.find("Colors()") != std::string::npos ||
            trimmed.find("Spacing()") != std::string::npos) {
            LogDebug("Style块内识别为CHTL片段: " + trimmed.substr(0, 20) + "...");
            return FragmentType::CHTL;
        }
        
        // 检测CSS语法
        if (trimmed.find(":") != std::string::npos && 
            (trimmed.find("background") != std::string::npos ||
             trimmed.find("color") != std::string::npos ||
             trimmed.find("margin") != std::string::npos ||
             trimmed.find("padding") != std::string::npos ||
             trimmed.find("font") != std::string::npos)) {
            LogDebug("Style块内识别为CSS片段: " + trimmed.substr(0, 20) + "...");
            return FragmentType::CSS;
        }
        
        // 默认为CSS（在style块内）
        LogDebug("Style块内默认识别为CSS片段: " + trimmed.substr(0, 20) + "...");
        return FragmentType::CSS;
    }
    
    // 普通上下文，使用原有的判断逻辑
    return DetermineFragmentType(content);
}

int CHTLUnifiedScanner::CalculateBraceBalance(const std::string& content) const {
    int balance = 0;
    bool inString = false;
    char stringChar = 0;
    
    for (size_t i = 0; i < content.length(); ++i) {
        char c = content[i];
        
        // 处理字符串内的字符
        if (inString) {
            if (c == stringChar && (i == 0 || content[i-1] != '\\')) {
                inString = false;
            }
            continue;
        }
        
        // 检测字符串开始
        if (c == '"' || c == '\'') {
            inString = true;
            stringChar = c;
            continue;
        }
        
        // 计算大括号平衡
        if (c == '{') {
            balance++;
        } else if (c == '}') {
            balance--;
        }
    }
    
    return balance;
}

size_t CHTLUnifiedScanner::FindNextSyntaxBoundary(size_t startPos) {
    if (startPos >= source_.length()) {
        return std::string::npos;
    }
    
    LogDebug("查找语法边界，起始位置: " + std::to_string(startPos));
    
    // 从startPos开始查找下一个语法边界
    size_t pos = startPos;
    int braceLevel = 0;
    bool inString = false;
    char stringDelimiter = '\0';
    bool inComment = false;
    
    while (pos < source_.length()) {
        char current = source_[pos];
        char next = (pos + 1 < source_.length()) ? source_[pos + 1] : '\0';
        
        // 处理字符串
        if (!inComment && (current == '"' || current == '\'' || current == '`')) {
            if (!inString) {
                inString = true;
                stringDelimiter = current;
            } else if (current == stringDelimiter && (pos == 0 || source_[pos - 1] != '\\')) {
                inString = false;
                stringDelimiter = '\0';
            }
        }
        
        // 处理注释
        if (!inString) {
            if (current == '/' && next == '/') {
                inComment = true;
                pos += 2;
                continue;
            } else if (current == '/' && next == '*') {
                // 多行注释开始
                pos += 2;
                while (pos + 1 < source_.length()) {
                    if (source_[pos] == '*' && source_[pos + 1] == '/') {
                        pos += 2;
                        break;
                    }
                    pos++;
                }
                continue;
            } else if (inComment && current == '\n') {
                inComment = false;
            }
        }
        
        if (inComment || inString) {
            pos++;
            continue;
        }
        
        // 处理大括号
        if (current == '{') {
            braceLevel++;
        } else if (current == '}') {
            braceLevel--;
            
            // 如果大括号平衡，这里可能是一个语法边界
            if (braceLevel == 0) {
                size_t endPos = pos + 1;
                
                // 检查这是否构成一个完整的语法单元
                std::string content = source_.substr(startPos, endPos - startPos);
                if (IsValidSyntaxUnit(content)) {
                    LogDebug("找到语法边界: " + std::to_string(endPos));
                    return endPos;
                }
            }
        }
        
        pos++;
    }
    
    // 如果没有找到完整的语法边界，返回文件末尾
    LogDebug("未找到完整语法边界，返回文件末尾");
    return source_.length();
}

bool CHTLUnifiedScanner::NeedsMinimalUnitSlicing(const std::string& content) {
    // 检查是否包含需要切割的CHTL JS语法
    if (content.find("{{") != std::string::npos || 
        content.find("}}") != std::string::npos ||
        content.find("->") != std::string::npos) {
        return true;
    }
    
    // 检查是否包含CJMOD关键字
    for (const auto& pair : cjmodKeywords_) {
        if (content.find(pair.first) != std::string::npos) {
            return true;
        }
    }
    
    // 检查是否包含基础CHTL JS关键字
    for (const auto& keyword : registeredKeywords_) {
        if (content.find(keyword) != std::string::npos) {
            return true;
        }
    }
    
    return false;
}

FragmentType CHTLUnifiedScanner::DetermineFragmentType(const std::string& content) {
    std::string trimmed = content;
    // 移除前后空白
    size_t start = trimmed.find_first_not_of(" \t\n\r");
    if (start == std::string::npos) return FragmentType::Unknown;
    
    size_t end = trimmed.find_last_not_of(" \t\n\r");
    trimmed = trimmed.substr(start, end - start + 1);
    
    // 优先检测CHTL语法（HTML元素开头）
    if (trimmed.find("html") == 0 || trimmed.find("body") == 0 || 
        trimmed.find("div") == 0 || trimmed.find("text") == 0 ||
        trimmed.find("head") == 0 || trimmed.find("title") == 0 ||
        trimmed.find("meta") == 0 || trimmed.find("link") == 0) {
        LogDebug("识别为CHTL片段: " + trimmed.substr(0, 20) + "...");
        return FragmentType::CHTL;
    }
    
    // 检测CHTL JS语法
    if (trimmed.find("vir") == 0 || trimmed.find("{{") != std::string::npos) {
        LogDebug("识别为CHTL JS片段: " + trimmed.substr(0, 20) + "...");
        return FragmentType::CHTL_JS;
    }
    
    // 检测CJMOD扩展语法
    for (const auto& pair : cjmodKeywords_) {
        if (trimmed.find(pair.first) != std::string::npos) {
            LogDebug("识别为CHTL JS片段(CJMOD扩展): " + trimmed.substr(0, 20) + "...");
            return FragmentType::CHTL_JS;
        }
    }
    
    // 检测基础CHTL JS关键字
    for (const auto& keyword : registeredKeywords_) {
        if (trimmed.find(keyword) != std::string::npos) {
            LogDebug("识别为CHTL JS片段(基础语法): " + trimmed.substr(0, 20) + "...");
            return FragmentType::CHTL_JS;
        }
    }
    
    // 检测纯CSS语法（不包含CHTL元素的CSS）
    if (trimmed.find("style") == 0 && trimmed.find("html") == std::string::npos) {
        LogDebug("识别为CSS片段: " + trimmed.substr(0, 20) + "...");
        return FragmentType::CSS;
    }
    
    // 检测CSS属性语法
    if (trimmed.find(":") != std::string::npos && 
        (trimmed.find("background") != std::string::npos ||
         trimmed.find("color") != std::string::npos ||
         trimmed.find("margin") != std::string::npos ||
         trimmed.find("padding") != std::string::npos)) {
        LogDebug("识别为CSS片段: " + trimmed.substr(0, 20) + "...");
        return FragmentType::CSS;
    }
    
    // 检测JavaScript语法
    if (trimmed.find("console.log") != std::string::npos ||
        trimmed.find("function") != std::string::npos ||
        trimmed.find("document.") != std::string::npos ||
        trimmed.find("let ") != std::string::npos ||
        trimmed.find("const ") != std::string::npos ||
        trimmed.find("var ") != std::string::npos) {
        LogDebug("识别为JavaScript片段: " + trimmed.substr(0, 20) + "...");
        return FragmentType::JS;
    }
    
    // 默认为CHTL（因为CHTL是主要语言）
    LogDebug("默认识别为CHTL片段: " + trimmed.substr(0, 20) + "...");
    return FragmentType::CHTL;
}

// ============ 片段索引接口实现 ============

void CHTLUnifiedScanner::BuildFragmentIndex(std::vector<CodeFragment>& fragments) {
    LogDebug("开始构建片段索引");
    indexManager_.BuildIndex(fragments);
    LogDebug("片段索引构建完成");
}

std::vector<size_t> CHTLUnifiedScanner::GetOptimalMergeOrder(const std::vector<CodeFragment>& fragments) const {
    return indexManager_.GetOptimalMergeOrder(fragments);
}

std::vector<size_t> CHTLUnifiedScanner::FindIncompleteFragments(const std::vector<CodeFragment>& fragments) const {
    return indexManager_.FindIncompleteFragments(fragments);
}

std::vector<size_t> CHTLUnifiedScanner::FindFragmentsByType(const std::vector<CodeFragment>& fragments, 
                                                           FragmentType type) const {
    return indexManager_.FindFragmentsByType(fragments, type);
}

std::vector<size_t> CHTLUnifiedScanner::FindFragmentsByContext(const std::vector<CodeFragment>& fragments, 
                                                             FragmentContext context) const {
    return indexManager_.FindFragmentsByContext(fragments, context);
}

std::vector<size_t> CHTLUnifiedScanner::GetFragmentDependencies(size_t fragmentId) const {
    return indexManager_.GetDependencies(fragmentId);
}

void CHTLUnifiedScanner::PrintIndexStatistics(const std::vector<CodeFragment>& fragments) const {
    indexManager_.PrintIndexStatistics(fragments);
}

// ============================================================================
// CJMOD动态注册机制实现
// ============================================================================

void CHTLUnifiedScanner::RegisterCJMODKeyword(const std::string& keyword, const std::string& moduleName, 
                                             std::function<std::string(const std::string&)> handler) {
    LogDebug("注册CJMOD关键字: " + keyword + " (模块: " + moduleName + ")");
    
    // 检查关键字是否已被其他模块注册
    if (cjmodKeywords_.find(keyword) != cjmodKeywords_.end()) {
        std::string existingModule = cjmodKeywords_[keyword].first;
        if (existingModule != moduleName) {
            Utils::ErrorHandler::GetInstance().LogWarning(
                "关键字 '" + keyword + "' 已被模块 '" + existingModule + "' 注册，模块 '" + moduleName + "' 的注册被忽略"
            );
            return;
        }
    }
    
    // 注册关键字
    cjmodKeywords_[keyword] = std::make_pair(moduleName, handler);
    
    // 添加到基础关键字集合
    registeredKeywords_.insert(keyword);
    
    // 记录模块的关键字
    if (cjmodModules_.find(moduleName) == cjmodModules_.end()) {
        cjmodModules_[moduleName] = std::vector<std::string>();
    }
    
    auto& moduleKeywords = cjmodModules_[moduleName];
    if (std::find(moduleKeywords.begin(), moduleKeywords.end(), keyword) == moduleKeywords.end()) {
        moduleKeywords.push_back(keyword);
    }
    
    LogDebug("CJMOD关键字注册成功: " + keyword);
}

void CHTLUnifiedScanner::UnregisterCJMODModule(const std::string& moduleName) {
    LogDebug("注销CJMOD模块: " + moduleName);
    
    auto moduleIt = cjmodModules_.find(moduleName);
    if (moduleIt != cjmodModules_.end()) {
        // 移除该模块的所有关键字
        for (const auto& keyword : moduleIt->second) {
            cjmodKeywords_.erase(keyword);
            registeredKeywords_.erase(keyword);
        }
        
        // 移除模块记录
        cjmodModules_.erase(moduleIt);
        
        LogDebug("CJMOD模块注销成功: " + moduleName);
    }
}

bool CHTLUnifiedScanner::IsKeywordRegistered(const std::string& keyword) const {
    return registeredKeywords_.find(keyword) != registeredKeywords_.end();
}

std::function<std::string(const std::string&)> CHTLUnifiedScanner::GetKeywordHandler(const std::string& keyword) const {
    auto it = cjmodKeywords_.find(keyword);
    if (it != cjmodKeywords_.end()) {
        return it->second.second;
    }
    return nullptr;
}

std::vector<std::string> CHTLUnifiedScanner::GetRegisteredCJMODModules() const {
    std::vector<std::string> modules;
    for (const auto& pair : cjmodModules_) {
        modules.push_back(pair.first);
    }
    return modules;
}

// ============================================================================
// 内容类型检测辅助方法实现
// ============================================================================

bool CHTLUnifiedScanner::IsHTMLElement(const std::string& content) const {
    // 检测CHTL HTML元素语法
    std::regex htmlElementRegex(R"(^\s*(html|body|div|span|p|h[1-6]|ul|ol|li|table|tr|td|th|form|input|button|img|a)\s*\{)");
    return std::regex_search(content, htmlElementRegex);
}

bool CHTLUnifiedScanner::IsStyleBlock(const std::string& content) const {
    // 检测样式块
    return content.find("style") != std::string::npos && content.find("{") != std::string::npos;
}

bool CHTLUnifiedScanner::IsScriptBlock(const std::string& content) const {
    // 检测脚本块
    return content.find("script") != std::string::npos && content.find("{") != std::string::npos;
}

bool CHTLUnifiedScanner::IsVirObject(const std::string& content) const {
    // 检测虚对象语法：vir objectName = ...
    std::regex virRegex(R"(\bvir\s+\w+\s*=)");
    return std::regex_search(content, virRegex);
}

bool CHTLUnifiedScanner::HasCHTLJSSyntax(const std::string& content) const {
    // 更精确的CHTL JS语法检测
    
    // 1. 检查完整的增强选择器模式：{{...}}->...
    std::regex selectorPattern(R"(\{\{[^}]+\}\}\s*->\s*\w+)");
    if (std::regex_search(content, selectorPattern)) {
        return true;
    }
    
    // 2. 检查虚对象声明：vir identifier =
    std::regex virPattern(R"(\bvir\s+\w+\s*=)");
    if (std::regex_search(content, virPattern)) {
        return true;
    }
    
    // 3. 检查虚对象方法调用：object->method (CHTL JS专属特征)
    std::regex virMethodPattern(R"(\w+\s*->\s*\w+\s*\()");
    if (std::regex_search(content, virMethodPattern)) {
        return true;
    }
    
    // 4. 检查CHTL JS特定方法调用
    std::regex chtlJSMethodPattern(R"(\b(listen|delegate|animate)\s*\()");
    if (std::regex_search(content, chtlJSMethodPattern)) {
        return true;
    }
    
    // 5. 检查注册的CJMOD关键字（更严格的匹配）
    for (const auto& pair : cjmodKeywords_) {
        const std::string& keyword = pair.first;
        std::regex keywordPattern("\\b" + keyword + "\\s*\\(");
        if (std::regex_search(content, keywordPattern)) {
            return true;
        }
    }
    
    return false;
}

bool CHTLUnifiedScanner::IsStyleBlockComplete(const std::string& content) const {
    // 检查样式块的完整性
    size_t stylePos = content.find("style");
    if (stylePos == std::string::npos) {
        return false;
    }
    
    // 找到style后面的第一个{
    size_t openBrace = content.find("{", stylePos);
    if (openBrace == std::string::npos) {
        return false;
    }
    
    // 检查大括号平衡
    int braceCount = 0;
    for (size_t i = openBrace; i < content.length(); ++i) {
        if (content[i] == '{') {
            braceCount++;
        } else if (content[i] == '}') {
            braceCount--;
            if (braceCount == 0) {
                return true; // 找到匹配的闭合括号
            }
        }
    }
    
    return braceCount == 0;
}

bool CHTLUnifiedScanner::IsScriptBlockComplete(const std::string& content) const {
    // 检查脚本块的完整性
    size_t scriptPos = content.find("script");
    if (scriptPos == std::string::npos) {
        return false;
    }
    
    // 找到script后面的第一个{
    size_t openBrace = content.find("{", scriptPos);
    if (openBrace == std::string::npos) {
        return false;
    }
    
    // 检查大括号平衡
    int braceCount = 0;
    for (size_t i = openBrace; i < content.length(); ++i) {
        if (content[i] == '{') {
            braceCount++;
        } else if (content[i] == '}') {
            braceCount--;
            if (braceCount == 0) {
                return true; // 找到匹配的闭合括号
            }
        }
    }
    
    return braceCount == 0;
}

bool CHTLUnifiedScanner::IsValidSyntaxUnit(const std::string& content) const {
    std::string trimmed = Utils::StringUtils::Trim(content);
    
    if (trimmed.empty()) {
        return false;
    }
    
    // 检查基本的语法完整性
    
    // 1. 检查括号平衡
    if (CalculateBraceBalance(trimmed) != 0) {
        return false;
    }
    
    // 2. 检查是否为完整的CHTL元素
    if (IsHTMLElement(trimmed)) {
        return IsCHTLBlockComplete(trimmed);
    }
    
    // 3. 检查是否为样式块或脚本块
    if (IsStyleBlock(trimmed)) {
        return IsStyleBlockComplete(trimmed);
    }
    
    if (IsScriptBlock(trimmed)) {
        return IsScriptBlockComplete(trimmed);
    }
    
    // 4. 检查是否为虚对象或CHTL JS语法
    if (IsVirObject(trimmed) || HasCHTLJSSyntax(trimmed)) {
        return IsCHTLJSFragmentComplete(trimmed);
    }
    
    // 5. 对于其他情况，检查是否包含基本的结构
    if (trimmed.find("{") != std::string::npos && trimmed.find("}") != std::string::npos) {
        return true;
    }
    
    // 6. 单行语句（如属性声明、简单表达式）
    if (trimmed.find(";") != std::string::npos || 
        trimmed.find(":") != std::string::npos ||
        trimmed.find("=") != std::string::npos) {
        return true;
    }
    
    return false;
}
} // namespace Scanner
} // namespace CHTL