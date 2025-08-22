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
    
    size_t pos = 0;
    size_t sourceLen = source.length();
    
    while (pos < sourceLen) {
        // 跳过空白字符（如果不保留空白）
        if (!config_.preserveWhitespace) {
            pos = SkipWhitespace(source, pos);
            if (pos >= sourceLen) break;
        }
        
        // 跳过注释
        size_t afterComments = SkipComments(source, pos);
        if (afterComments > pos) {
            pos = afterComments;
            continue;
        }
        
        // 检查各种代码块类型
        if (pos + 1 < sourceLen && source[pos] == '[') {
            // 处理标记块：[Template], [Custom], [Origin], [Import], [Namespace], [Configuration]
            size_t blockEnd = FindBlockEnd(source, pos, "]");
            if (blockEnd != std::string::npos) {
                // 查找对应的花括号块
                size_t braceStart = source.find('{', blockEnd);
                if (braceStart != std::string::npos) {
                    size_t braceEnd = FindBlockEnd(source, braceStart, "}");
                    if (braceEnd != std::string::npos) {
                        ProcessCHTLBlock(source, pos, braceEnd + 1, fragments);
                        pos = braceEnd + 1;
                        continue;
                    }
                }
            }
        }
        
        // 检查style块
        if (source.substr(pos, 5) == "style" && 
            (pos + 5 >= sourceLen || !Utils::StringUtils::IsAlphaNumeric(source[pos + 5]))) {
            size_t braceStart = source.find('{', pos + 5);
            if (braceStart != std::string::npos) {
                size_t braceEnd = FindBlockEnd(source, braceStart, "}");
                if (braceEnd != std::string::npos) {
                    ProcessStyleBlock(source, pos, braceEnd + 1, fragments);
                    pos = braceEnd + 1;
                    continue;
                }
            }
        }
        
        // 检查script块
        if (source.substr(pos, 6) == "script" && 
            (pos + 6 >= sourceLen || !Utils::StringUtils::IsAlphaNumeric(source[pos + 6]))) {
            size_t braceStart = source.find('{', pos + 6);
            if (braceStart != std::string::npos) {
                size_t braceEnd = FindBlockEnd(source, braceStart, "}");
                if (braceEnd != std::string::npos) {
                    ProcessScriptBlock(source, pos, braceEnd + 1, fragments);
                    pos = braceEnd + 1;
                    continue;
                }
            }
        }
        
        // 检查HTML元素或CHTL语法
        size_t nextBlock = std::min({
            source.find('{', pos + 1),
            source.find('[', pos + 1),
            source.find("style", pos + 1),
            source.find("script", pos + 1)
        });
        
        if (nextBlock == std::string::npos) {
            nextBlock = sourceLen;
        }
        
        // 处理当前位置到下一个块之间的内容
        if (nextBlock > pos) {
            std::string content = source.substr(pos, nextBlock - pos);
            if (!Utils::StringUtils::Trim(content).empty()) {
                FragmentType type = DetectFragmentType(content);
                CodeFragment fragment(type, content, pos, nextBlock);
                CalculateLineColumn(source, pos, fragment.startLine, fragment.startColumn);
                CalculateLineColumn(source, nextBlock, fragment.endLine, fragment.endColumn);
                fragments.push_back(fragment);
            }
            pos = nextBlock;
        } else {
            // 如果没有找到下一个块或者nextBlock == pos，前进一个字符避免无限循环
            pos++;
        }
    }
    
    // 进行后处理：最小单元切割和上下文检查
    if (config_.enableMinimalUnit || config_.enableContextCheck) {
        std::vector<CodeFragment> processedFragments;
        
        for (size_t i = 0; i < fragments.size(); ++i) {
            const auto& fragment = fragments[i];
            
            // 上下文检查
            if (config_.enableContextCheck) {
                const CodeFragment* prev = (i > 0) ? &fragments[i - 1] : nullptr;
                const CodeFragment* next = (i + 1 < fragments.size()) ? &fragments[i + 1] : nullptr;
                
                if (!CheckContextValidity(fragment, prev, next)) {
                    // 需要扩展片段
                    CodeFragment expanded = ExpandFragment(source, fragment, 64);
                    processedFragments.push_back(expanded);
                    continue;
                }
            }
            
            // 最小单元切割
            if (config_.enableMinimalUnit && 
                (fragment.type == FragmentType::CHTL || fragment.type == FragmentType::CHTLJS)) {
                auto splitFragments = PerformMinimalUnitSplit(fragment);
                processedFragments.insert(processedFragments.end(), 
                                        splitFragments.begin(), splitFragments.end());
            } else {
                processedFragments.push_back(fragment);
            }
        }
        
        fragments = std::move(processedFragments);
    }
    
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

} // namespace Scanner
} // namespace CHTL