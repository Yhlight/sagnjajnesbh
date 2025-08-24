/**
 * @file CHTLUnifiedScanner_v2.cpp
 * @brief 基于main分支语法文档的精准代码切割器实现
 */

#include "Scanner/CHTLUnifiedScanner_v2.h"
#include <iostream>
#include <sstream>
#include <algorithm>
#include <cctype>

namespace Scanner {

CHTLUnifiedScanner::CHTLUnifiedScanner() 
    : debugMode_(false) {
    
    // 初始化CHTL关键字（基于语法文档）
    chtlKeywords_ = {
        "html", "head", "body", "div", "span", "p", "h1", "h2", "h3", "h4", "h5", "h6",
        "ul", "ol", "li", "table", "tr", "td", "th", "form", "input", "button", "img", "a",
        "text", "style", "script", "title", "meta", "link"
    };
    
    // 初始化CHTL JS关键字（基于语法文档第1099-1311行）
    chtlJSKeywords_ = {
        "vir",          // 虚对象声明
        "listen",       // 增强监听器
        "delegate",     // 事件委托
        "animate"       // 动画
        // 注意：增强选择器{{}}和箭头操作符->通过模式匹配识别
    };
    
    // 初始化CHTL JS语法正则表达式（基于语法文档）
    
    // 虚对象模式: vir 对象名 = listen/delegate/animate({...});
    virPattern_ = std::regex(R"(vir\s+\w+\s*=\s*(listen|delegate|animate))");
    
    // 增强选择器模式: {{选择器}}
    enhancedSelectorPattern_ = std::regex(R"(\{\{[^}]+\}\})");
    
    // 箭头操作符模式: 对象->方法 或 {{选择器}}->方法
    arrowOperatorPattern_ = std::regex(R"((\w+|\{\{[^}]+\}\})\s*->\s*\w+(\s*\([^)]*\))?)");
    
    // 监听器模式: listen({...})
    listenPattern_ = std::regex(R"(listen\s*\(\s*\{[\s\S]*?\}\s*\))");
    
    // 事件委托模式: delegate({...})
    delegatePattern_ = std::regex(R"(delegate\s*\(\s*\{[\s\S]*?\}\s*\))");
    
    // 动画模式: animate({...})
    animatePattern_ = std::regex(R"(animate\s*\(\s*\{[\s\S]*?\}\s*\))");
}

std::vector<CodeFragment> CHTLUnifiedScanner::ScanSource(const std::string& source, 
                                                        const std::string& fileName) {
    LogDebug("开始扫描CHTL源代码: " + fileName);
    LogDebug("源代码大小: " + std::to_string(source.length()) + " 字节");
    
    // 重置上下文
    context_.reset();
    
    // 第一步：可变长度切片扫描
    LogDebug("开始可变长度切片扫描");
    auto fragments = VariableLengthSlicing(source, fileName);
    
    // 第二步：对CHTL和CHTL JS片段进行最小单元切割
    std::vector<CodeFragment> finalFragments;
    for (const auto& fragment : fragments) {
        if (fragment.type == FragmentType::CHTL || fragment.type == FragmentType::CHTL_JS) {
            std::string typeName = (fragment.type == FragmentType::CHTL ? "CHTL" : "CHTL JS");
            LogDebug("对" + typeName + "片段进行最小单元切割");
            auto minimalFragments = MinimalUnitSlicing(fragment);
            finalFragments.insert(finalFragments.end(), minimalFragments.begin(), minimalFragments.end());
        } else {
            finalFragments.push_back(fragment);
        }
    }
    
    LogDebug("扫描完成，发现 " + std::to_string(finalFragments.size()) + " 个代码片段");
    return finalFragments;
}

std::vector<CodeFragment> CHTLUnifiedScanner::VariableLengthSlicing(const std::string& source, 
                                                                   const std::string& fileName) {
    std::vector<CodeFragment> fragments;
    size_t currentPos = 0;
    
    while (currentPos < source.length()) {
        LogDebug("查找语法边界，起始位置: " + std::to_string(currentPos));
        
        // 查找下一个语法边界
        size_t boundaryPos = FindSyntaxBoundary(source, currentPos);
        
        if (boundaryPos == std::string::npos) {
            LogDebug("未找到完整语法边界，返回文件末尾");
            boundaryPos = source.length();
        } else {
            LogDebug("找到语法边界: " + std::to_string(boundaryPos));
        }
        
        // 检查片段完整性并可能扩展
        size_t expandedPos = CheckAndExpandFragment(source, currentPos, boundaryPos);
        
        if (expandedPos != boundaryPos) {
            LogDebug("片段扩展: " + std::to_string(boundaryPos) + " -> " + std::to_string(expandedPos));
            boundaryPos = expandedPos;
        }
        
        // 提取片段内容
        std::string fragmentContent = source.substr(currentPos, boundaryPos - currentPos);
        
        // 确定片段类型
        FragmentType type = DetermineFragmentType(fragmentContent);
        
        // 计算行列号
        auto [line, column] = CalculateLineColumn(source, currentPos);
        
        // 创建片段
        CodeFragment fragment(fragmentContent, type, currentPos, boundaryPos, fileName);
        fragment.lineNumber = line;
        fragment.columnNumber = column;
        
        LogDebug("推送片段，类型: " + std::to_string(static_cast<int>(type)) + 
                ", 内容: " + fragmentContent.substr(0, 20) + "...");
        
        fragments.push_back(fragment);
        currentPos = boundaryPos;
        
        // 更新上下文
        UpdateContext(fragmentContent, currentPos);
    }
    
    LogDebug("可变长度切片扫描完成，共生成 " + std::to_string(fragments.size()) + " 个片段");
    return fragments;
}

size_t CHTLUnifiedScanner::FindSyntaxBoundary(const std::string& source, size_t startPos) {
    // 关键字边界检测
    std::vector<std::string> boundaryKeywords = {
        // CHTL关键字
        "html", "head", "body", "div", "span", "p", "h1", "h2", "h3", "h4", "h5", "h6",
        "ul", "ol", "li", "table", "tr", "td", "th", "form", "input", "button", "img", "a",
        "text", "style", "script",
        
        // CHTL JS关键字
        "vir", "listen", "delegate", "animate"
    };
    
    size_t nearestBoundary = std::string::npos;
    
    // 检查关键字边界
    for (const auto& keyword : boundaryKeywords) {
        size_t pos = source.find(keyword, startPos + 1); // +1 避免找到当前位置
        if (pos != std::string::npos) {
            // 确保是完整单词
            if ((pos == 0 || !std::isalnum(source[pos - 1])) &&
                (pos + keyword.length() >= source.length() || 
                 !std::isalnum(source[pos + keyword.length()]))) {
                if (nearestBoundary == std::string::npos || pos < nearestBoundary) {
                    nearestBoundary = pos;
                }
            }
        }
    }
    
    // 检查特殊语法边界
    std::vector<std::string> specialPatterns = {
        "{{",  // 增强选择器开始
        "}}",  // 增强选择器结束
        "->",  // 箭头操作符
        "/*",  // 多行注释开始
        "*/",  // 多行注释结束
        "//",  // 单行注释
        "{",   // 块开始
        "}"    // 块结束
    };
    
    for (const auto& pattern : specialPatterns) {
        size_t pos = source.find(pattern, startPos + 1);
        if (pos != std::string::npos) {
            if (nearestBoundary == std::string::npos || pos < nearestBoundary) {
                nearestBoundary = pos;
            }
        }
    }
    
    return nearestBoundary;
}

size_t CHTLUnifiedScanner::CheckAndExpandFragment(const std::string& source, 
                                                 size_t startPos, size_t endPos) {
    // 提取当前片段
    std::string fragment = source.substr(startPos, endPos - startPos);
    
    // 检查是否为不完整的CHTL JS语法
    
    // 1. 检查虚对象声明是否完整
    if (fragment.find("vir ") != std::string::npos) {
        // 查找完整的虚对象声明结束
        size_t parenCount = 0;
        size_t braceCount = 0;
        bool foundListen = false;
        bool inParens = false;
        bool inBraces = false;
        
        // 从起始位置开始查找，确保包含完整语句
        for (size_t i = startPos; i < source.length(); ++i) {
            char c = source[i];
            
            // 检查是否找到listen函数
            if (!foundListen && i + 6 < source.length() && 
                source.substr(i, 6) == "listen") {
                foundListen = true;
            }
            
            if (foundListen) {
                if (c == '(') {
                    parenCount++;
                    inParens = true;
                } else if (c == ')') {
                    parenCount--;
                } else if (c == '{') {
                    braceCount++;
                    inBraces = true;
                } else if (c == '}') {
                    braceCount--;
                    if (inBraces && braceCount == 0 && parenCount == 0) {
                        // 找到完整的函数结束，查找分号
                        for (size_t j = i + 1; j < source.length(); ++j) {
                            if (source[j] == ';') {
                                return j + 1;
                            } else if (!std::isspace(source[j])) {
                                return i + 1;
                            }
                        }
                        return i + 1;
                    }
                }
            }
        }
    }
    
    // 2. 检查增强选择器是否完整
    if (fragment.find("{{") != std::string::npos && fragment.find("}}") == std::string::npos) {
        size_t closePos = source.find("}}", endPos);
        if (closePos != std::string::npos) {
            return closePos + 2;
        }
    }
    
    // 3. 检查函数调用是否完整
    std::vector<std::string> functions = {"listen", "delegate", "animate"};
    for (const auto& func : functions) {
        if (fragment.find(func) != std::string::npos) {
            size_t parenCount = 0;
            bool inParens = false;
            
            for (size_t i = endPos; i < source.length(); ++i) {
                char c = source[i];
                if (c == '(') {
                    parenCount++;
                    inParens = true;
                } else if (c == ')') {
                    parenCount--;
                    if (inParens && parenCount == 0) {
                        // 查找分号
                        for (size_t j = i + 1; j < source.length(); ++j) {
                            if (source[j] == ';') {
                                return j + 1;
                            } else if (!std::isspace(source[j])) {
                                break;
                            }
                        }
                        return i + 1;
                    }
                }
            }
        }
    }
    
    return endPos; // 无需扩展
}

std::vector<CodeFragment> CHTLUnifiedScanner::MinimalUnitSlicing(const CodeFragment& fragment) {
    std::vector<CodeFragment> result;
    std::string content = fragment.content;
    
    // 对于CHTL JS，需要识别最小单元
    if (fragment.type == FragmentType::CHTL_JS) {
        LogDebug("智能最小单元切割，内容长度: " + std::to_string(content.length()));
        
        // 识别关键的切割点
        std::vector<std::pair<size_t, std::string>> cutPoints;
        
        // 查找箭头操作符 ->
        std::sregex_iterator iter(content.begin(), content.end(), arrowOperatorPattern_);
        std::sregex_iterator end;
        
        for (; iter != end; ++iter) {
            const std::smatch& match = *iter;
            size_t pos = match.position();
            
            // 在->处切割
            size_t arrowPos = content.find("->", pos);
            if (arrowPos != std::string::npos) {
                cutPoints.push_back({arrowPos, "->"});
            }
        }
        
        // 查找增强选择器边界
        std::sregex_iterator selectorIter(content.begin(), content.end(), enhancedSelectorPattern_);
        for (; selectorIter != end; ++selectorIter) {
            const std::smatch& match = *selectorIter;
            size_t startPos = match.position();
            size_t endPos = startPos + match.length();
            
            cutPoints.push_back({endPos, "}}"});
        }
        
        // 根据切割点分割
        if (!cutPoints.empty()) {
            std::sort(cutPoints.begin(), cutPoints.end());
            
            size_t lastPos = 0;
            for (const auto& cutPoint : cutPoints) {
                size_t pos = cutPoint.first;
                
                if (pos > lastPos) {
                    std::string subContent = content.substr(lastPos, pos - lastPos);
                    if (!Trim(subContent).empty()) {
                        CodeFragment subFragment(subContent, FragmentType::CHTL_JS, 
                                               fragment.startPosition + lastPos,
                                               fragment.startPosition + pos,
                                               fragment.fileName);
                        result.push_back(subFragment);
                    }
                }
                
                lastPos = pos;
            }
            
            // 添加最后一部分
            if (lastPos < content.length()) {
                std::string subContent = content.substr(lastPos);
                if (!Trim(subContent).empty()) {
                    CodeFragment subFragment(subContent, FragmentType::CHTL_JS,
                                           fragment.startPosition + lastPos,
                                           fragment.endPosition,
                                           fragment.fileName);
                    result.push_back(subFragment);
                }
            }
            
            LogDebug("智能最小单元切割完成，生成 " + std::to_string(result.size()) + " 个片段");
        } else {
            result.push_back(fragment);
        }
    } else {
        // 对于CHTL片段，保持完整
        result.push_back(fragment);
    }
    
    return result;
}

bool CHTLUnifiedScanner::IsCHTLSyntax(const std::string& content) {
    std::string trimmed = Trim(content);
    
    // 检查CHTL HTML元素
    for (const auto& keyword : chtlKeywords_) {
        if (trimmed.find(keyword) == 0) {
            // 确保后面跟着空白或大括号
            size_t keywordEnd = keyword.length();
            if (keywordEnd < trimmed.length()) {
                char nextChar = trimmed[keywordEnd];
                if (std::isspace(nextChar) || nextChar == '{') {
                    return true;
                }
            } else {
                return true;
            }
        }
    }
    
    return false;
}

bool CHTLUnifiedScanner::IsCHTLJSSyntax(const std::string& content) {
    return DetectVirtualObject(content) ||
           DetectEnhancedSelector(content) ||
           DetectArrowOperator(content) ||
           DetectListenSyntax(content) ||
           DetectDelegateSyntax(content) ||
           DetectAnimateSyntax(content);
}

bool CHTLUnifiedScanner::DetectVirtualObject(const std::string& content) {
    return std::regex_search(content, virPattern_);
}

bool CHTLUnifiedScanner::DetectEnhancedSelector(const std::string& content) {
    return std::regex_search(content, enhancedSelectorPattern_);
}

bool CHTLUnifiedScanner::DetectArrowOperator(const std::string& content) {
    return std::regex_search(content, arrowOperatorPattern_);
}

bool CHTLUnifiedScanner::DetectListenSyntax(const std::string& content) {
    return std::regex_search(content, listenPattern_);
}

bool CHTLUnifiedScanner::DetectDelegateSyntax(const std::string& content) {
    return std::regex_search(content, delegatePattern_);
}

bool CHTLUnifiedScanner::DetectAnimateSyntax(const std::string& content) {
    return std::regex_search(content, animatePattern_);
}

bool CHTLUnifiedScanner::IsCSSSyntax(const std::string& content) {
    std::string trimmed = Trim(content);
    
    // CSS选择器模式
    std::regex cssPattern(R"(^\s*[\w\-\.#\[\],:]+\s*\{[\s\S]*\}\s*$)");
    return std::regex_match(trimmed, cssPattern);
}

bool CHTLUnifiedScanner::IsJavaScriptSyntax(const std::string& content) {
    std::string trimmed = Trim(content);
    
    // JavaScript关键字
    std::vector<std::string> jsKeywords = {
        "var", "let", "const", "function", "if", "else", "for", "while", 
        "do", "switch", "case", "break", "continue", "return", "try", 
        "catch", "finally", "throw", "new", "delete", "typeof", "instanceof"
    };
    
    for (const auto& keyword : jsKeywords) {
        if (trimmed.find(keyword) == 0) {
            return true;
        }
    }
    
    return false;
}

FragmentType CHTLUnifiedScanner::DetermineFragmentType(const std::string& content) {
    // 首先检查CHTL JS语法（更特殊）
    if (IsCHTLJSSyntax(content)) {
        return FragmentType::CHTL_JS;
    }
    
    // 然后检查CHTL语法
    if (IsCHTLSyntax(content)) {
        return FragmentType::CHTL;
    }
    
    // 检查CSS语法
    if (IsCSSSyntax(content)) {
        return FragmentType::CSS;
    }
    
    // 检查JavaScript语法
    if (IsJavaScriptSyntax(content)) {
        return FragmentType::JS;
    }
    
    // 默认识别为CHTL片段
    return FragmentType::CHTL;
}

void CHTLUnifiedScanner::UpdateContext(const std::string& content, size_t position) {
    if (content.find("script") != std::string::npos) {
        context_.inScriptBlock = true;
    }
    if (content.find("style") != std::string::npos) {
        context_.inStyleBlock = true;
    }
    
    // 更新大括号深度
    for (char c : content) {
        if (c == '{') context_.braceDepth++;
        else if (c == '}') context_.braceDepth--;
        else if (c == '(') context_.parenthesesDepth++;
        else if (c == ')') context_.parenthesesDepth--;
    }
}

std::string CHTLUnifiedScanner::Trim(const std::string& str) {
    size_t start = str.find_first_not_of(" \t\n\r");
    if (start == std::string::npos) return "";
    
    size_t end = str.find_last_not_of(" \t\n\r");
    return str.substr(start, end - start + 1);
}

void CHTLUnifiedScanner::LogDebug(const std::string& message) {
    if (debugMode_) {
        std::cout << "[Scanner] " << message << std::endl;
    }
}

std::pair<int, int> CHTLUnifiedScanner::CalculateLineColumn(const std::string& source, size_t position) {
    int line = 1;
    int column = 1;
    
    for (size_t i = 0; i < position && i < source.length(); ++i) {
        if (source[i] == '\n') {
            line++;
            column = 1;
        } else {
            column++;
        }
    }
    
    return {line, column};
}

} // namespace Scanner