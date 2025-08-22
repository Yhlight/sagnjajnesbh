#include "Scanner/CHTLUnifiedScanner.h"
#include "Utils/StringUtils.h"
#include <algorithm>
#include <regex>
#include <stack>

namespace CHTL {
namespace Scanner {

CHTLUnifiedScanner::CHTLUnifiedScanner(const ScanConfig& config) 
    : config_(config) {
    InitializeKeywords();
}

void CHTLUnifiedScanner::InitializeKeywords() {
    // CHTL关键字（基于语法文档）
    chtlKeywords_.insert({
        "text", "style", "script", "inherit", "delete", "insert",
        "after", "before", "replace", "at", "top", "bottom",
        "from", "as", "except", "vir"
    });
    
    // CHTL标记关键字
    chtlKeywords_.insert({
        "[Template]", "[Custom]", "[Origin]", "[Import]", "[Namespace]",
        "[Configuration]", "[Info]", "[Export]", "[Name]", "[OriginType]"
    });
    
    // CHTL类型关键字
    chtlKeywords_.insert({
        "@Style", "@Element", "@Var", "@Html", "@JavaScript", "@Chtl", 
        "@CJmod", "@Config", "@Vue", "@CSS", "@Css", "@css"
    });
    
    // CHTL JS关键字（基于语法文档的CHTL JS特征）
    chtlJSKeywords_.insert({
        "listen", "delegate", "animate", "vir", "iNeverAway", "printMylove"
    });
}

std::vector<CodeFragment> CHTLUnifiedScanner::Scan(const std::string& source, const std::string& fileName) {
    currentFileName_ = fileName;
    std::vector<CodeFragment> fragments;
    
    if (source.empty()) {
        return fragments;
    }
    
    // 第一阶段：基于可变长度切片的精准扫描 - 严格按照目标规划第48-55行
    auto rawFragments = PerformVariableLengthSlicing(source);
    
    // 第二阶段：向前扩增检查，确保切片完整性
    auto expandedFragments = PerformForwardExpansion(rawFragments, source);
    
    // 第三阶段：基于最小单元的二次切割，确保结果绝对精确
    auto preciseFragments = PerformMinimalUnitSlicing(expandedFragments, source);
    
    // 第四阶段：上下文检查，确保代码片段不会过小
    fragments = PerformContextualOptimization(preciseFragments, source);
    
    return fragments;
}

FragmentType CHTLUnifiedScanner::DetectFragmentType(const std::string& content, const std::string& context) {
    std::string trimmed = Utils::StringUtils::Trim(content);
    if (trimmed.empty()) {
        return FragmentType::Unknown;
    }
    
    // 检查CHTL JS特征
    if (HasCHTLJSFeatures(content)) {
        return FragmentType::CHTLJS;
    }
    
    // 检查CSS特征
    if (std::regex_search(content, std::regex(R"([a-zA-Z-]+\s*:\s*[^;]+;)"))) {
        return FragmentType::CSS;
    }
    
    // 检查JavaScript特征
    if (std::regex_search(content, std::regex(R"(\b(function|var|let|const|if|for|while)\b)"))) {
        return FragmentType::JavaScript;
    }
    
    // 检查CHTL关键字
    std::vector<std::string> words = Utils::StringUtils::Split(trimmed, " ");
    for (const auto& word : words) {
        if (IsCHTLKeyword(Utils::StringUtils::Trim(word))) {
            return FragmentType::CHTL;
        }
    }
    
    // 检查CHTL语法模式
    if (std::regex_search(content, std::regex(R"(\[[A-Z][a-zA-Z]*\])"))) {
        return FragmentType::CHTL;
    }
    
    if (std::regex_search(content, std::regex(R"(@[A-Z][a-zA-Z]*)"))) {
        return FragmentType::CHTL;
    }
    
    // 默认为CHTL类型
    return FragmentType::CHTL;
}

size_t CHTLUnifiedScanner::FindBlockEnd(const std::string& source, size_t startPos, const std::string& blockType) {
    if (startPos >= source.length()) {
        return std::string::npos;
    }
    
    char startChar = blockType == "}" ? '{' : blockType[0];
    char endChar = blockType[0];
    
    std::stack<char> brackets;
    size_t pos = startPos;
    
    // 找到起始字符
    while (pos < source.length() && source[pos] != startChar) {
        pos++;
    }
    
    if (pos >= source.length()) {
        return std::string::npos;
    }
    
    brackets.push(startChar);
    pos++;
    
    while (pos < source.length() && !brackets.empty()) {
        char ch = source[pos];
        
        // 跳过字符串字面量
        if (ch == '"' || ch == '\'') {
            char quote = ch;
            pos++;
            while (pos < source.length() && source[pos] != quote) {
                if (source[pos] == '\\' && pos + 1 < source.length()) {
                    pos += 2; // 跳过转义字符
                } else {
                    pos++;
                }
            }
            if (pos < source.length()) pos++; // 跳过结束引号
            continue;
        }
        
        // 跳过注释
        if (ch == '/' && pos + 1 < source.length()) {
            if (source[pos + 1] == '/') {
                // 单行注释
                while (pos < source.length() && source[pos] != '\n') {
                    pos++;
                }
                continue;
            } else if (source[pos + 1] == '*') {
                // 多行注释
                pos += 2;
                while (pos + 1 < source.length() && 
                       !(source[pos] == '*' && source[pos + 1] == '/')) {
                    pos++;
                }
                if (pos + 1 < source.length()) pos += 2;
                continue;
            }
        }
        
        if (ch == startChar) {
            brackets.push(ch);
        } else if (ch == endChar) {
            brackets.pop();
        }
        
        pos++;
    }
    
    return brackets.empty() ? pos - 1 : std::string::npos;
}

void CHTLUnifiedScanner::ProcessCHTLBlock(const std::string& source, size_t startPos, size_t endPos,
                                         std::vector<CodeFragment>& fragments) {
    std::string content = source.substr(startPos, endPos - startPos);
    CodeFragment fragment(FragmentType::CHTL, content, startPos, endPos);
    CalculateLineColumn(source, startPos, fragment.startLine, fragment.startColumn);
    CalculateLineColumn(source, endPos, fragment.endLine, fragment.endColumn);
    fragments.push_back(fragment);
}

void CHTLUnifiedScanner::ProcessStyleBlock(const std::string& source, size_t startPos, size_t endPos,
                                          std::vector<CodeFragment>& fragments) {
    std::string content = source.substr(startPos, endPos - startPos);
    
    // 检查是否包含CHTL语法（如变量、选择器等）
    FragmentType type = FragmentType::CSS;
    if (HasCHTLJSFeatures(content) || 
        std::regex_search(content, std::regex(R"(@[A-Z][a-zA-Z]*|&|\.[\w-]+|#[\w-]+)"))) {
        type = FragmentType::CHTL;
    }
    
    CodeFragment fragment(type, content, startPos, endPos);
    CalculateLineColumn(source, startPos, fragment.startLine, fragment.startColumn);
    CalculateLineColumn(source, endPos, fragment.endLine, fragment.endColumn);
    fragment.context = "style_block";
    fragments.push_back(fragment);
}

void CHTLUnifiedScanner::ProcessScriptBlock(const std::string& source, size_t startPos, size_t endPos,
                                           std::vector<CodeFragment>& fragments) {
    std::string content = source.substr(startPos, endPos - startPos);
    
    // 检查是否包含CHTL JS语法
    FragmentType type = HasCHTLJSFeatures(content) ? FragmentType::CHTLJS : FragmentType::JavaScript;
    
    CodeFragment fragment(type, content, startPos, endPos);
    CalculateLineColumn(source, startPos, fragment.startLine, fragment.startColumn);
    CalculateLineColumn(source, endPos, fragment.endLine, fragment.endColumn);
    fragment.context = "script_block";
    fragments.push_back(fragment);
}

void CHTLUnifiedScanner::ProcessGlobalStyleBlock(const std::string& source, size_t startPos, size_t endPos,
                                               std::vector<CodeFragment>& fragments) {
    std::string content = source.substr(startPos, endPos - startPos);
    CodeFragment fragment(FragmentType::CSS, content, startPos, endPos);
    CalculateLineColumn(source, startPos, fragment.startLine, fragment.startColumn);
    CalculateLineColumn(source, endPos, fragment.endLine, fragment.endColumn);
    fragment.context = "global_style";
    fragments.push_back(fragment);
}

void CHTLUnifiedScanner::ProcessOriginBlock(const std::string& source, size_t startPos, size_t endPos,
                                           std::vector<CodeFragment>& fragments) {
    std::string content = source.substr(startPos, endPos - startPos);
    
    // 根据Origin类型确定片段类型
    FragmentType type = FragmentType::Unknown;
    if (content.find("@Html") != std::string::npos) {
        type = FragmentType::CHTL;
    } else if (content.find("@Style") != std::string::npos || content.find("@CSS") != std::string::npos) {
        type = FragmentType::CSS;
    } else if (content.find("@JavaScript") != std::string::npos) {
        type = FragmentType::JavaScript;
    } else {
        type = FragmentType::CHTL; // 自定义类型默认为CHTL
    }
    
    CodeFragment fragment(type, content, startPos, endPos);
    CalculateLineColumn(source, startPos, fragment.startLine, fragment.startColumn);
    CalculateLineColumn(source, endPos, fragment.endLine, fragment.endColumn);
    fragment.context = "origin_block";
    fragments.push_back(fragment);
}

std::vector<CodeFragment> CHTLUnifiedScanner::PerformMinimalUnitSplit(const CodeFragment& fragment) {
    std::vector<CodeFragment> result;
    
    if (fragment.type != FragmentType::CHTL && fragment.type != FragmentType::CHTLJS) {
        result.push_back(fragment);
        return result;
    }
    
    // 对于CHTL和CHTL JS，按照最小单元进行切割
    // 例如：{{box}}->click 切割为 {{box}}-> 和 click
    std::string content = fragment.content;
    size_t pos = 0;
    size_t startPos = fragment.startPos;
    
    std::regex chtlPattern(R"(\{\{[^}]+\}\}->?|@[A-Z][a-zA-Z]*|[A-Za-z_][A-Za-z0-9_]*\([^)]*\))");
    std::sregex_iterator iter(content.begin(), content.end(), chtlPattern);
    std::sregex_iterator end;
    
    size_t lastEnd = 0;
    for (; iter != end; ++iter) {
        const std::smatch& match = *iter;
        
        // 添加匹配前的内容
        if (match.position() > lastEnd) {
            std::string beforeMatch = content.substr(lastEnd, match.position() - lastEnd);
            if (!Utils::StringUtils::Trim(beforeMatch).empty()) {
                CodeFragment beforeFragment(fragment.type, beforeMatch, 
                                          startPos + lastEnd, startPos + match.position());
                result.push_back(beforeFragment);
            }
        }
        
        // 添加匹配的内容
        std::string matchStr = match.str();
        CodeFragment matchFragment(fragment.type, matchStr, 
                                 startPos + match.position(), 
                                 startPos + match.position() + match.length());
        result.push_back(matchFragment);
        
        lastEnd = match.position() + match.length();
    }
    
    // 添加剩余内容
    if (lastEnd < content.length()) {
        std::string remaining = content.substr(lastEnd);
        if (!Utils::StringUtils::Trim(remaining).empty()) {
            CodeFragment remainingFragment(fragment.type, remaining, 
                                         startPos + lastEnd, fragment.endPos);
            result.push_back(remainingFragment);
        }
    }
    
    // 如果没有找到任何匹配，返回原片段
    if (result.empty()) {
        result.push_back(fragment);
    }
    
    return result;
}

bool CHTLUnifiedScanner::CheckContextValidity(const CodeFragment& fragment, 
                                              const CodeFragment* prevFragment,
                                              const CodeFragment* nextFragment) {
    // 检查片段是否在合理的上下文中
    // 例如：检查CHTL语法是否被意外截断
    
    if (fragment.type == FragmentType::CHTL || fragment.type == FragmentType::CHTLJS) {
        std::string content = Utils::StringUtils::Trim(fragment.content);
        
        // 检查是否有未闭合的括号
        int braceCount = 0, bracketCount = 0, parenCount = 0;
        for (char ch : content) {
            switch (ch) {
                case '{': braceCount++; break;
                case '}': braceCount--; break;
                case '[': bracketCount++; break;
                case ']': bracketCount--; break;
                case '(': parenCount++; break;
                case ')': parenCount--; break;
            }
        }
        
        if (braceCount != 0 || bracketCount != 0 || parenCount != 0) {
            return false; // 需要扩展
        }
        
        // 检查是否以不完整的语法结尾
        if (Utils::StringUtils::EndsWith(content, "->") || 
            Utils::StringUtils::EndsWith(content, "@") ||
            Utils::StringUtils::EndsWith(content, ".")) {
            return false; // 需要扩展
        }
    }
    
    return true; // 上下文合理
}

CodeFragment CHTLUnifiedScanner::ExpandFragment(const std::string& source, 
                                               const CodeFragment& fragment, 
                                               size_t expandSize) {
    size_t newStart = (fragment.startPos > expandSize) ? 
                      fragment.startPos - expandSize : 0;
    size_t newEnd = std::min(fragment.endPos + expandSize, source.length());
    
    std::string newContent = source.substr(newStart, newEnd - newStart);
    CodeFragment expanded(fragment.type, newContent, newStart, newEnd);
    expanded.context = fragment.context + "_expanded";
    
    return expanded;
}

size_t CHTLUnifiedScanner::SkipWhitespace(const std::string& source, size_t pos) {
    while (pos < source.length() && Utils::StringUtils::IsWhitespace(source[pos])) {
        pos++;
    }
    return pos;
}

size_t CHTLUnifiedScanner::SkipComments(const std::string& source, size_t pos) {
    if (pos + 1 >= source.length()) {
        return pos;
    }
    
    // 跳过单行注释 //
    if (source[pos] == '/' && source[pos + 1] == '/') {
        while (pos < source.length() && source[pos] != '\n') {
            pos++;
        }
        return pos;
    }
    
    // 跳过多行注释 /* */
    if (source[pos] == '/' && source[pos + 1] == '*') {
        pos += 2;
        while (pos + 1 < source.length() && 
               !(source[pos] == '*' && source[pos + 1] == '/')) {
            pos++;
        }
        if (pos + 1 < source.length()) {
            pos += 2;
        }
        return pos;
    }
    
    // 跳过生成器注释 --
    if (source[pos] == '-' && source[pos + 1] == '-') {
        while (pos < source.length() && source[pos] != '\n') {
            pos++;
        }
        return pos;
    }
    
    return pos;
}

void CHTLUnifiedScanner::CalculateLineColumn(const std::string& source, size_t pos, int& line, int& column) {
    line = 1;
    column = 1;
    
    for (size_t i = 0; i < pos && i < source.length(); ++i) {
        if (source[i] == '\n') {
            line++;
            column = 1;
        } else {
            column++;
        }
    }
}

bool CHTLUnifiedScanner::IsCHTLKeyword(const std::string& word) {
    return chtlKeywords_.find(word) != chtlKeywords_.end();
}

bool CHTLUnifiedScanner::HasCHTLJSFeatures(const std::string& content) {
    // 检查CHTL JS特征语法
    
    // 检查增强选择器 {{}}
    if (std::regex_search(content, std::regex(R"(\{\{[^}]+\}\})"))) {
        return true;
    }
    
    // 检查CHTL JS关键字
    for (const auto& keyword : chtlJSKeywords_) {
        if (content.find(keyword) != std::string::npos) {
            return true;
        }
    }
    
    // 检查箭头操作符 ->
    if (content.find("->") != std::string::npos) {
        return true;
    }
    
    // 检查虚对象语法 vir
    if (std::regex_search(content, std::regex(R"(\bvir\s+\w+\s*=)"))) {
        return true;
    }
    
    return false;
}

void CHTLUnifiedScanner::Clear() {
    currentFileName_.clear();
}

size_t CHTLUnifiedScanner::GetErrorCount() const {
    return Utils::ErrorHandler::GetInstance().GetErrorCount();
}

// 精准代码切割的四个核心阶段实现 - 严格按照目标规划第48-55行

std::vector<CodeFragment> CHTLUnifiedScanner::PerformVariableLengthSlicing(const std::string& source) {
    // 第一阶段：基于可变长度切片的精准扫描
    std::vector<CodeFragment> fragments;
    
    size_t pos = 0;
    size_t sourceLen = source.length();
    
    while (pos < sourceLen) {
        // 改进的空白字符处理 - 减少空白片段
        if (!config_.preserveWhitespace) {
            size_t oldPos = pos;
            pos = SkipWhitespace(source, pos);
            if (pos >= sourceLen) break;
            
            // 如果跳过了大量空白字符，创建一个空白片段而不是多个
            if (pos - oldPos > 1) {
                // 跳过大量空白，不创建片段
                continue;
            }
        }
        
        // 识别代码块边界
        size_t blockStart = pos;
        size_t blockEnd = FindNextBlockBoundary(source, pos);
        
        if (blockEnd > blockStart) {
            std::string content = source.substr(blockStart, blockEnd - blockStart);
            FragmentType type = DetectFragmentType(content);
            
            CodeFragment fragment(type, content, blockStart, blockEnd);
            CalculateLineColumn(source, blockStart, fragment.startLine, fragment.startColumn);
            CalculateLineColumn(source, blockEnd, fragment.endLine, fragment.endColumn);
            
            fragments.push_back(fragment);
            pos = blockEnd;
        } else {
            pos++;
        }
    }
    
    return fragments;
}

std::vector<CodeFragment> CHTLUnifiedScanner::PerformForwardExpansion(const std::vector<CodeFragment>& fragments, const std::string& source) {
    // 第二阶段：向前扩增检查，确保切片完整性
    // 目标规划第50行：检查是否下一个片段的开头是否可能组成了一个完整的CHTL或CHTL JS代码片段
    
    std::vector<CodeFragment> expandedFragments;
    
    for (size_t i = 0; i < fragments.size(); ++i) {
        CodeFragment fragment = fragments[i];
        
        // 检查当前片段是否需要向前扩增
        if (i + 1 < fragments.size()) {
            const auto& nextFragment = fragments[i + 1];
            
            // 检查是否能组成完整的CHTL/CHTL JS代码片段
            if (ShouldExpandForward(fragment, nextFragment, source)) {
                // 向前扩增一定的长度
                size_t expansionLength = CalculateExpansionLength(fragment, nextFragment, source);
                
                if (expansionLength > 0) {
                    size_t newEnd = std::min(fragment.endPos + expansionLength, source.length());
                    std::string expandedContent = source.substr(fragment.startPos, newEnd - fragment.startPos);
                    
                    fragment.content = expandedContent;
                    fragment.endPos = newEnd;
                    CalculateLineColumn(source, newEnd, fragment.endLine, fragment.endColumn);
                }
            }
        }
        
        expandedFragments.push_back(fragment);
    }
    
    return expandedFragments;
}

std::vector<CodeFragment> CHTLUnifiedScanner::PerformMinimalUnitSlicing(const std::vector<CodeFragment>& fragments, const std::string& source) {
    // 第三阶段：基于最小单元的二次切割，确保结果绝对精确
    // 目标规划第52行：例如{{box}}->click将被切割为{{box}}->和click
    
    std::vector<CodeFragment> preciseFragments;
    
    for (const auto& fragment : fragments) {
        if (fragment.type == FragmentType::CHTL || fragment.type == FragmentType::CHTLJS) {
            // 对CHTL和CHTL JS片段进行最小单元切割
            auto minimalUnits = SplitIntoMinimalUnits(fragment, source);
            preciseFragments.insert(preciseFragments.end(), minimalUnits.begin(), minimalUnits.end());
        } else {
            // 其他类型的片段保持不变
            preciseFragments.push_back(fragment);
        }
    }
    
    return preciseFragments;
}

std::vector<CodeFragment> CHTLUnifiedScanner::PerformContextualOptimization(const std::vector<CodeFragment>& fragments, const std::string& source) {
    // 第四阶段：上下文检查，确保代码片段不会过小
    // 目标规划第53行：让多个CHTL和CHTL JS代码片段连续而非全部细分为最小单元
    
    std::vector<CodeFragment> optimizedFragments;
    
    for (size_t i = 0; i < fragments.size(); ++i) {
        const auto& fragment = fragments[i];
        
        // 检查是否应该与相邻片段合并
        if (ShouldMergeWithNext(fragment, i + 1 < fragments.size() ? &fragments[i + 1] : nullptr)) {
            // 合并连续的CHTL/CHTL JS片段
            CodeFragment mergedFragment = fragment;
            size_t j = i + 1;
            
            while (j < fragments.size() && ShouldMergeWithNext(mergedFragment, &fragments[j])) {
                const auto& nextFragment = fragments[j];
                
                // 扩展合并片段
                std::string mergedContent = source.substr(mergedFragment.startPos, 
                                                         nextFragment.endPos - mergedFragment.startPos);
                mergedFragment.content = mergedContent;
                mergedFragment.endPos = nextFragment.endPos;
                mergedFragment.endLine = nextFragment.endLine;
                mergedFragment.endColumn = nextFragment.endColumn;
                
                j++;
            }
            
            optimizedFragments.push_back(mergedFragment);
            i = j - 1; // 跳过已合并的片段
        } else {
            optimizedFragments.push_back(fragment);
        }
    }
    
    return optimizedFragments;
}

// 辅助方法实现

size_t CHTLUnifiedScanner::FindNextBlockBoundary(const std::string& source, size_t pos) {
    // 查找下一个代码块边界
    size_t len = source.length();
    
    // 寻找关键的边界字符
    std::vector<size_t> boundaries;
    
    // 查找各种块的开始
    for (size_t i = pos; i < len; ++i) {
        if (source[i] == '{' || source[i] == '}' || source[i] == '[' || source[i] == ']') {
            boundaries.push_back(i + 1);
            break;
        }
        
        // 查找关键字边界
        if (i > pos && (source[i] == ' ' || source[i] == '\n' || source[i] == '\t')) {
            std::string word = source.substr(pos, i - pos);
            if (chtlKeywords_.count(word) || chtlJSKeywords_.count(word)) {
                boundaries.push_back(i);
                break;
            }
        }
    }
    
    if (boundaries.empty()) {
        return len;
    }
    
    return *std::min_element(boundaries.begin(), boundaries.end());
}

bool CHTLUnifiedScanner::ShouldExpandForward(const CodeFragment& current, const CodeFragment& next, const std::string& source) {
    // 检查是否需要向前扩增
    
    // 如果当前片段以不完整的CHTL/CHTL JS语法结束，需要扩增
    std::string currentEnd = current.content.substr(std::max(0, (int)current.content.length() - 10));
    std::string nextStart = next.content.substr(0, std::min(10, (int)next.content.length()));
    
    // 检查CHTL JS的增强选择器语法：{{box}}->click
    if (currentEnd.find("{{") != std::string::npos && nextStart.find("->") == 0) {
        return true;
    }
    
    // 检查其他可能的不完整语法
    if (currentEnd.find("@") != std::string::npos && nextStart.find("Element") == 0) {
        return true;
    }
    
    return false;
}

size_t CHTLUnifiedScanner::CalculateExpansionLength(const CodeFragment& current, const CodeFragment& next, const std::string& source) {
    // 计算需要扩增的长度
    
    // 基础扩增策略：扩增到下一个完整的语法单元
    std::string nextContent = next.content;
    
    // 查找下一个完整的分隔符
    size_t expansionEnd = nextContent.find_first_of(" \n\t;{}[]");
    if (expansionEnd == std::string::npos) {
        expansionEnd = nextContent.length();
    }
    
    return expansionEnd;
}

std::vector<CodeFragment> CHTLUnifiedScanner::SplitIntoMinimalUnits(const CodeFragment& fragment, const std::string& source) {
    // 将片段分割为最小单元
    std::vector<CodeFragment> units;
    
    std::string content = fragment.content;
    size_t pos = 0;
    size_t contentLen = content.length();
    
    while (pos < contentLen) {
        // 查找下一个最小单元边界
        size_t unitEnd = FindMinimalUnitBoundary(content, pos);
        
        if (unitEnd > pos) {
            std::string unitContent = content.substr(pos, unitEnd - pos);
            FragmentType unitType = DetectFragmentType(unitContent);
            
            CodeFragment unit(unitType, unitContent, 
                            fragment.startPos + pos, fragment.startPos + unitEnd);
            
            // 计算行列信息
            CalculateLineColumn(source, unit.startPos, unit.startLine, unit.startColumn);
            CalculateLineColumn(source, unit.endPos, unit.endLine, unit.endColumn);
            
            units.push_back(unit);
            pos = unitEnd;
        } else {
            pos++;
        }
    }
    
    return units;
}

size_t CHTLUnifiedScanner::FindMinimalUnitBoundary(const std::string& content, size_t pos) {
    // 查找最小单元边界
    size_t len = content.length();
    
    // CHTL JS增强选择器的特殊处理
    if (pos < len && content.substr(pos, 2) == "{{") {
        // 查找}}的结束
        size_t end = content.find("}}", pos + 2);
        if (end != std::string::npos) {
            // 检查是否有->操作符
            if (end + 2 < len && content.substr(end + 2, 2) == "->") {
                return end + 4; // 包含->操作符
            }
            return end + 2; // 只包含{{}}
        }
    }
    
    // 其他最小单元边界
    for (size_t i = pos + 1; i < len; ++i) {
        if (content[i] == ' ' || content[i] == '\n' || content[i] == '\t' ||
            content[i] == '{' || content[i] == '}' || content[i] == ';' ||
            content[i] == '(' || content[i] == ')') {
            return i;
        }
    }
    
    return len;
}

bool CHTLUnifiedScanner::ShouldMergeWithNext(const CodeFragment& current, const CodeFragment* next) {
    // 检查是否应该与下一个片段合并
    
    if (!next) {
        return false;
    }
    
    // 如果两个片段都是CHTL或CHTL JS类型，且在同一行或相邻行，考虑合并
    if ((current.type == FragmentType::CHTL || current.type == FragmentType::CHTLJS) &&
        (next->type == FragmentType::CHTL || next->type == FragmentType::CHTLJS)) {
        
        // 检查是否在相邻位置
        if (next->startPos - current.endPos <= 2) { // 允许少量空白字符
            return true;
        }
    }
    
    return false;
}

} // namespace Scanner
} // namespace CHTL