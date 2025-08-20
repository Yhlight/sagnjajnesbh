#include "scanner/CHTLUnifiedScanner.h"
#include "utils/StringUtils.h"
#include "utils/Logger.h"
#include <algorithm>
#include <sstream>

namespace chtl {

CHTLUnifiedScanner::CHTLUnifiedScanner()
    : m_SliceSize(4096)  // 默认4KB切片
    , m_BufferPosition(0)
    , m_Context(std::make_unique<ScannerContext>()) {
}

CHTLUnifiedScanner::~CHTLUnifiedScanner() = default;

std::vector<CodeFragment> CHTLUnifiedScanner::Scan(const std::string& source, 
                                                  const std::string& filename) {
    m_CurrentFile = filename;
    m_Buffer = source;
    m_BufferPosition = 0;
    m_Context->Reset();
    m_Errors.clear();
    
    std::vector<CodeFragment> fragments;
    
    // 处理整个源代码
    while (m_BufferPosition < m_Buffer.length()) {
        // 计算切片大小
        size_t sliceEnd = std::min(m_BufferPosition + m_SliceSize, m_Buffer.length());
        
        // 检查是否需要扩展切片以避免截断
        while (sliceEnd < m_Buffer.length() && !IsFragmentBoundary(m_Buffer, sliceEnd)) {
            sliceEnd = std::min(sliceEnd + 256, m_Buffer.length());  // 每次扩展256字符
        }
        
        // 提取切片
        std::string slice = m_Buffer.substr(m_BufferPosition, sliceEnd - m_BufferPosition);
        
        // 处理切片
        ProcessSlice(slice, m_BufferPosition, fragments);
        
        m_BufferPosition = sliceEnd;
    }
    
    // 合并连续的相同类型片段（上下文感知）
    MergeConsecutiveFragments(fragments);
    
    return fragments;
}

void CHTLUnifiedScanner::ProcessSlice(const std::string& slice, size_t startPos,
                                     std::vector<CodeFragment>& fragments) {
    size_t pos = 0;
    size_t fragmentStart = 0;
    
    while (pos < slice.length()) {
        char ch = slice[pos];
        m_Context->UpdatePosition(ch);
        
        // 处理注释
        if (pos + 1 < slice.length()) {
            if (ch == '/' && slice[pos + 1] == '/') {
                // 单行注释
                size_t commentEnd = slice.find('\n', pos);
                if (commentEnd == std::string::npos) {
                    commentEnd = slice.length();
                }
                pos = commentEnd;
                continue;
            } else if (ch == '/' && slice[pos + 1] == '*') {
                // 多行注释
                m_Context->PushState(ScannerState::IN_MULTILINE_COMMENT);
                size_t commentEnd = slice.find("*/", pos + 2);
                if (commentEnd != std::string::npos) {
                    pos = commentEnd + 2;
                    m_Context->PopState();
                } else {
                    pos = slice.length();
                }
                continue;
            } else if (ch == '-' && slice[pos + 1] == '-') {
                // CHTL生成器注释
                size_t commentEnd = slice.find('\n', pos);
                if (commentEnd == std::string::npos) {
                    commentEnd = slice.length();
                }
                std::string comment = slice.substr(pos, commentEnd - pos);
                fragments.push_back(CodeFragment(
                    FragmentType::CHTL,
                    comment,
                    m_Context->GetCurrentLine(),
                    m_Context->GetCurrentColumn(),
                    m_Context->GetCurrentLine(),
                    m_Context->GetCurrentColumn() + comment.length()
                ));
                pos = commentEnd;
                continue;
            }
        }
        
        // 处理字符串字面量
        if (ch == '"' || ch == '\'') {
            if (m_Context->GetCurrentState() != ScannerState::IN_STRING_LITERAL) {
                m_Context->PushState(ScannerState::IN_STRING_LITERAL);
                m_Context->SetStringDelimiter(ch);
            } else if (ch == m_Context->GetStringDelimiter()) {
                m_Context->PopState();
                m_Context->SetStringDelimiter('\0');
            }
            pos++;
            continue;
        }
        
        // 在字符串中，跳过所有内容
        if (m_Context->GetCurrentState() == ScannerState::IN_STRING_LITERAL) {
            pos++;
            continue;
        }
        
        // 处理括号
        if (ch == '{') {
            m_Context->PushBrace(BraceType::CURLY, m_Context->GetCurrentLine(), 
                               m_Context->GetCurrentColumn());
        } else if (ch == '}') {
            m_Context->PopBrace(BraceType::CURLY);
        } else if (ch == '[') {
            m_Context->PushBrace(BraceType::SQUARE, m_Context->GetCurrentLine(),
                               m_Context->GetCurrentColumn());
        } else if (ch == ']') {
            m_Context->PopBrace(BraceType::SQUARE);
        } else if (ch == '(') {
            m_Context->PushBrace(BraceType::PARENTHESIS, m_Context->GetCurrentLine(),
                               m_Context->GetCurrentColumn());
        } else if (ch == ')') {
            m_Context->PopBrace(BraceType::PARENTHESIS);
        }
        
        // 识别CHTL关键结构
        if (fragmentStart == pos) {
            std::string lookahead = slice.substr(pos, std::min<size_t>(20, slice.length() - pos));
            
            // 检查CHTL关键字
            if (utils::StringUtils::StartsWith(lookahead, "[Template]") ||
                utils::StringUtils::StartsWith(lookahead, "[Custom]") ||
                utils::StringUtils::StartsWith(lookahead, "[Configuration]") ||
                utils::StringUtils::StartsWith(lookahead, "[Namespace]") ||
                utils::StringUtils::StartsWith(lookahead, "[Import]") ||
                utils::StringUtils::StartsWith(lookahead, "[Origin]")) {
                
                // 找到完整的块
                size_t blockEnd = FindBlockEnd(slice, pos);
                std::string block = slice.substr(pos, blockEnd - pos);
                
                fragments.push_back(CodeFragment(
                    FragmentType::CHTL,
                    block,
                    m_Context->GetCurrentLine(),
                    m_Context->GetCurrentColumn(),
                    m_Context->GetCurrentLine(),
                    m_Context->GetCurrentColumn() + block.length()
                ));
                
                pos = blockEnd;
                fragmentStart = pos;
                continue;
            }
            
            // 检查元素名称
            if (std::isalpha(ch) || ch == '_') {
                size_t tokenEnd = pos;
                while (tokenEnd < slice.length() && 
                       (std::isalnum(slice[tokenEnd]) || slice[tokenEnd] == '_')) {
                    tokenEnd++;
                }
                
                std::string token = slice.substr(pos, tokenEnd - pos);
                
                // 检查是否是特殊的CHTL元素
                if (token == "text" || token == "style" || token == "script") {
                    // 找到对应的块
                    size_t blockStart = slice.find('{', tokenEnd);
                    if (blockStart != std::string::npos) {
                        size_t blockEnd = FindMatchingBrace(slice, blockStart);
                        if (blockEnd != std::string::npos) {
                            std::string block = slice.substr(pos, blockEnd - pos + 1);
                            
                            FragmentType type = FragmentType::CHTL;
                            if (token == "style" && m_Context->IsInCHTLContext()) {
                                // 局部style块
                                ProcessLocalStyleBlock(block, startPos + pos, fragments);
                            } else if (token == "script" && m_Context->IsInCHTLContext()) {
                                // 局部script块
                                ProcessLocalScriptBlock(block, startPos + pos, fragments);
                            } else {
                                fragments.push_back(CodeFragment(
                                    type,
                                    block,
                                    m_Context->GetCurrentLine(),
                                    m_Context->GetCurrentColumn(),
                                    m_Context->GetCurrentLine(),
                                    m_Context->GetCurrentColumn() + block.length()
                                ));
                            }
                            
                            pos = blockEnd + 1;
                            fragmentStart = pos;
                            continue;
                        }
                    }
                }
                
                // 普通元素
                size_t elementEnd = FindElementEnd(slice, pos);
                std::string element = slice.substr(pos, elementEnd - pos);
                
                fragments.push_back(CodeFragment(
                    FragmentType::CHTL,
                    element,
                    m_Context->GetCurrentLine(),
                    m_Context->GetCurrentColumn(),
                    m_Context->GetCurrentLine(),
                    m_Context->GetCurrentColumn() + element.length()
                ));
                
                pos = elementEnd;
                fragmentStart = pos;
                continue;
            }
        }
        
        pos++;
    }
}

bool CHTLUnifiedScanner::IsFragmentBoundary(const std::string& buffer, size_t pos) {
    if (pos >= buffer.length()) return true;
    
    // 检查是否在字符串中
    if (m_Context->GetCurrentState() == ScannerState::IN_STRING_LITERAL) {
        return false;
    }
    
    // 检查括号是否平衡
    if (!m_Context->IsBalanced()) {
        return false;
    }
    
    // 检查是否在注释中
    if (m_Context->GetCurrentState() == ScannerState::IN_MULTILINE_COMMENT) {
        return false;
    }
    
    // 检查CHTL JS特殊语法 {{}}
    if (pos >= 2 && buffer[pos-2] == '{' && buffer[pos-1] == '{') {
        size_t endPos = buffer.find("}}", pos);
        return endPos == std::string::npos || endPos <= pos;
    }
    
    // 检查是否在CHTL元素边界
    if (pos > 0 && buffer[pos-1] == '}' && m_Context->GetBraceDepth(BraceType::CURLY) == 0) {
        return true;
    }
    
    // 检查是否在语句边界
    if (pos > 0 && buffer[pos-1] == ';') {
        return true;
    }
    
    return false;
}

void CHTLUnifiedScanner::ProcessLocalStyleBlock(const std::string& content, size_t startPos,
                                               std::vector<CodeFragment>& fragments) {
    // style { ... } 块的处理
    size_t braceStart = content.find('{');
    if (braceStart == std::string::npos) return;
    
    size_t braceEnd = FindMatchingBrace(content, braceStart);
    if (braceEnd == std::string::npos) return;
    
    // 提取style块内容
    std::string styleContent = content.substr(braceStart + 1, braceEnd - braceStart - 1);
    
    // 将整个style块作为CHTL片段
    fragments.push_back(CodeFragment(
        FragmentType::CHTL,
        "style",
        m_Context->GetCurrentLine(),
        m_Context->GetCurrentColumn(),
        m_Context->GetCurrentLine(),
        m_Context->GetCurrentColumn() + 5
    ));
    
    // 分析style块内容，分离CHTL特性和纯CSS
    size_t pos = 0;
    while (pos < styleContent.length()) {
        SkipWhitespace(styleContent, pos);
        if (pos >= styleContent.length()) break;
        
        // 检查CHTL特性
        if (styleContent[pos] == '@' || styleContent[pos] == '&' ||
            (pos + 6 < styleContent.length() && 
             styleContent.substr(pos, 6) == "delete") ||
            (pos + 7 < styleContent.length() && 
             styleContent.substr(pos, 7) == "inherit")) {
            
            // CHTL样式特性
            size_t featureEnd = FindStatementEnd(styleContent, pos);
            std::string feature = styleContent.substr(pos, featureEnd - pos);
            
            fragments.push_back(CodeFragment(
                FragmentType::CHTL,
                feature,
                m_Context->GetCurrentLine(),
                m_Context->GetCurrentColumn(),
                m_Context->GetCurrentLine(),
                m_Context->GetCurrentColumn() + feature.length()
            ));
            
            pos = featureEnd;
        } else {
            // 纯CSS内容
            size_t cssEnd = pos;
            while (cssEnd < styleContent.length()) {
                if (styleContent[cssEnd] == '@' || styleContent[cssEnd] == '&' ||
                    (cssEnd + 6 < styleContent.length() && 
                     styleContent.substr(cssEnd, 6) == "delete") ||
                    (cssEnd + 7 < styleContent.length() && 
                     styleContent.substr(cssEnd, 7) == "inherit")) {
                    break;
                }
                cssEnd++;
            }
            
            if (cssEnd > pos) {
                std::string css = styleContent.substr(pos, cssEnd - pos);
                fragments.push_back(CodeFragment(
                    FragmentType::CSS,
                    css,
                    m_Context->GetCurrentLine(),
                    m_Context->GetCurrentColumn(),
                    m_Context->GetCurrentLine(),
                    m_Context->GetCurrentColumn() + css.length()
                ));
                pos = cssEnd;
            }
        }
    }
}

void CHTLUnifiedScanner::ProcessLocalScriptBlock(const std::string& content, size_t startPos,
                                                std::vector<CodeFragment>& fragments) {
    // script { ... } 块的处理
    size_t braceStart = content.find('{');
    if (braceStart == std::string::npos) return;
    
    size_t braceEnd = FindMatchingBrace(content, braceStart);
    if (braceEnd == std::string::npos) return;
    
    // 提取script块内容
    std::string scriptContent = content.substr(braceStart + 1, braceEnd - braceStart - 1);
    
    // 将整个script块作为CHTL片段
    fragments.push_back(CodeFragment(
        FragmentType::CHTL,
        "script",
        m_Context->GetCurrentLine(),
        m_Context->GetCurrentColumn(),
        m_Context->GetCurrentLine(),
        m_Context->GetCurrentColumn() + 6
    ));
    
    // 分析script块内容，识别CHTL JS特性
    size_t pos = 0;
    while (pos < scriptContent.length()) {
        SkipWhitespace(scriptContent, pos);
        if (pos >= scriptContent.length()) break;
        
        // 检查CHTL JS增强选择器 {{}}
        if (pos + 1 < scriptContent.length() && 
            scriptContent[pos] == '{' && scriptContent[pos + 1] == '{') {
            
            size_t selectorEnd = scriptContent.find("}}", pos + 2);
            if (selectorEnd != std::string::npos) {
                std::string selector = scriptContent.substr(pos, selectorEnd - pos + 2);
                fragments.push_back(CodeFragment(
                    FragmentType::CHTL_JS,
                    selector,
                    m_Context->GetCurrentLine(),
                    m_Context->GetCurrentColumn(),
                    m_Context->GetCurrentLine(),
                    m_Context->GetCurrentColumn() + selector.length()
                ));
                pos = selectorEnd + 2;
                continue;
            }
        }
        
        // 检查vir关键字
        if (pos + 3 < scriptContent.length() &&
            scriptContent.substr(pos, 3) == "vir" &&
            (pos + 3 == scriptContent.length() || IsWhitespace(scriptContent[pos + 3]))) {
            
            // 找到整个vir声明
            size_t virEnd = FindStatementEnd(scriptContent, pos);
            std::string virDecl = scriptContent.substr(pos, virEnd - pos);
            fragments.push_back(CodeFragment(
                FragmentType::CHTL_JS,
                virDecl,
                m_Context->GetCurrentLine(),
                m_Context->GetCurrentColumn(),
                m_Context->GetCurrentLine(),
                m_Context->GetCurrentColumn() + virDecl.length()
            ));
            pos = virEnd;
            continue;
        }
        
        // 检查->操作符
        if (pos + 1 < scriptContent.length() &&
            scriptContent[pos] == '-' && scriptContent[pos + 1] == '>') {
            
            // 找到整个CHTL JS调用链
            size_t chainStart = pos;
            while (chainStart > 0 && !IsWhitespace(scriptContent[chainStart - 1])) {
                chainStart--;
            }
            
            size_t chainEnd = pos + 2;
            while (chainEnd < scriptContent.length()) {
                if (scriptContent[chainEnd] == ';' || 
                    scriptContent[chainEnd] == ',' ||
                    scriptContent[chainEnd] == ')' ||
                    IsWhitespace(scriptContent[chainEnd])) {
                    break;
                }
                chainEnd++;
            }
            
            std::string chain = scriptContent.substr(chainStart, chainEnd - chainStart);
            fragments.push_back(CodeFragment(
                FragmentType::CHTL_JS,
                chain,
                m_Context->GetCurrentLine(),
                m_Context->GetCurrentColumn(),
                m_Context->GetCurrentLine(),
                m_Context->GetCurrentColumn() + chain.length()
            ));
            pos = chainEnd;
            continue;
        }
        
        // 其他JavaScript代码
        size_t jsEnd = pos;
        while (jsEnd < scriptContent.length()) {
            if ((jsEnd + 1 < scriptContent.length() && 
                 scriptContent[jsEnd] == '{' && scriptContent[jsEnd + 1] == '{') ||
                (jsEnd + 3 < scriptContent.length() &&
                 scriptContent.substr(jsEnd, 3) == "vir" &&
                 (jsEnd + 3 == scriptContent.length() || IsWhitespace(scriptContent[jsEnd + 3]))) ||
                (jsEnd + 1 < scriptContent.length() &&
                 scriptContent[jsEnd] == '-' && scriptContent[jsEnd + 1] == '>')) {
                break;
            }
            jsEnd++;
        }
        
        if (jsEnd > pos) {
            std::string js = scriptContent.substr(pos, jsEnd - pos);
            fragments.push_back(CodeFragment(
                FragmentType::JAVASCRIPT,
                js,
                m_Context->GetCurrentLine(),
                m_Context->GetCurrentColumn(),
                m_Context->GetCurrentLine(),
                m_Context->GetCurrentColumn() + js.length()
            ));
            pos = jsEnd;
        }
    }
}

void CHTLUnifiedScanner::MergeConsecutiveFragments(std::vector<CodeFragment>& fragments) {
    if (fragments.size() < 2) return;
    
    std::vector<CodeFragment> merged;
    merged.push_back(fragments[0]);
    
    for (size_t i = 1; i < fragments.size(); ++i) {
        CodeFragment& prev = merged.back();
        const CodeFragment& current = fragments[i];
        
        // 合并相同类型的连续片段（但保持CHTL和CHTL JS的最小单元）
        if (prev.GetType() == current.GetType() &&
            prev.GetEndLine() == current.GetStartLine() &&
            ShouldMergeFragments(prev, current)) {
            
            // 合并内容
            std::string mergedContent = prev.GetContent() + current.GetContent();
            
            merged.back() = CodeFragment(
                prev.GetType(),
                mergedContent,
                prev.GetStartLine(),
                prev.GetStartColumn(),
                current.GetEndLine(),
                current.GetEndColumn()
            );
        } else {
            merged.push_back(current);
        }
    }
    
    fragments = std::move(merged);
}

bool CHTLUnifiedScanner::ShouldMergeFragments(const CodeFragment& prev, 
                                              const CodeFragment& current) {
    // CHTL和CHTL JS片段需要保持最小单元，不轻易合并
    if (prev.GetType() == FragmentType::CHTL || 
        prev.GetType() == FragmentType::CHTL_JS) {
        
        // 检查是否是完整的语句
        if (IsCompleteCHTLStatement(prev.GetContent())) {
            return false;
        }
        
        // 检查是否是不同的CHTL结构
        if (prev.IsCHTLTemplate() || prev.IsCHTLCustom() ||
            prev.IsCHTLImport() || prev.IsCHTLConfiguration() ||
            prev.IsCHTLNamespace() || prev.IsCHTLOrigin()) {
            return false;
        }
    }
    
    // CSS和JavaScript可以合并
    return true;
}

bool CHTLUnifiedScanner::IsWhitespace(char ch) const {
    return ch == ' ' || ch == '\t' || ch == '\n' || ch == '\r';
}

void CHTLUnifiedScanner::SkipWhitespace(const std::string& buffer, size_t& pos) {
    while (pos < buffer.length() && IsWhitespace(buffer[pos])) {
        pos++;
    }
}

size_t CHTLUnifiedScanner::FindMatchingBrace(const std::string& content, size_t startPos) {
    if (startPos >= content.length() || content[startPos] != '{') {
        return std::string::npos;
    }
    
    int depth = 0;
    bool inString = false;
    char stringDelimiter = '\0';
    
    for (size_t i = startPos; i < content.length(); ++i) {
        char ch = content[i];
        
        // 处理字符串
        if (!inString && (ch == '"' || ch == '\'')) {
            inString = true;
            stringDelimiter = ch;
        } else if (inString && ch == stringDelimiter) {
            // 检查转义
            if (i > 0 && content[i-1] != '\\') {
                inString = false;
                stringDelimiter = '\0';
            }
        }
        
        if (!inString) {
            if (ch == '{') {
                depth++;
            } else if (ch == '}') {
                depth--;
                if (depth == 0) {
                    return i;
                }
            }
        }
    }
    
    return std::string::npos;
}

size_t CHTLUnifiedScanner::FindBlockEnd(const std::string& content, size_t startPos) {
    // 找到块的开始括号
    size_t braceStart = content.find('{', startPos);
    if (braceStart == std::string::npos) {
        // 没有括号的情况，找到语句结束
        return FindStatementEnd(content, startPos);
    }
    
    // 找到匹配的结束括号
    size_t braceEnd = FindMatchingBrace(content, braceStart);
    if (braceEnd == std::string::npos) {
        return content.length();
    }
    
    return braceEnd + 1;
}

size_t CHTLUnifiedScanner::FindElementEnd(const std::string& content, size_t startPos) {
    // 找到元素的结束位置
    size_t pos = startPos;
    
    // 跳过元素名
    while (pos < content.length() && 
           (std::isalnum(content[pos]) || content[pos] == '_')) {
        pos++;
    }
    
    // 跳过空白
    while (pos < content.length() && IsWhitespace(content[pos])) {
        pos++;
    }
    
    // 如果有属性或内容块
    if (pos < content.length() && content[pos] == '{') {
        size_t blockEnd = FindMatchingBrace(content, pos);
        if (blockEnd != std::string::npos) {
            return blockEnd + 1;
        }
    }
    
    return pos;
}

size_t CHTLUnifiedScanner::FindStatementEnd(const std::string& content, size_t startPos) {
    size_t pos = startPos;
    int braceDepth = 0;
    bool inString = false;
    char stringDelimiter = '\0';
    
    while (pos < content.length()) {
        char ch = content[pos];
        
        // 处理字符串
        if (!inString && (ch == '"' || ch == '\'')) {
            inString = true;
            stringDelimiter = ch;
        } else if (inString && ch == stringDelimiter) {
            if (pos > 0 && content[pos-1] != '\\') {
                inString = false;
                stringDelimiter = '\0';
            }
        }
        
        if (!inString) {
            if (ch == '{') {
                braceDepth++;
            } else if (ch == '}') {
                braceDepth--;
                if (braceDepth < 0) {
                    return pos;
                }
            } else if (ch == ';' && braceDepth == 0) {
                return pos + 1;
            }
        }
        
        pos++;
    }
    
    return pos;
}

bool CHTLUnifiedScanner::IsCompleteCHTLStatement(const std::string& content) {
    std::string trimmed = utils::StringUtils::Trim(content);
    
    // 检查是否是完整的CHTL声明
    if (utils::StringUtils::StartsWith(trimmed, "[") && 
        utils::StringUtils::EndsWith(trimmed, "]")) {
        return true;
    }
    
    // 检查是否是完整的元素
    if (CheckBracketBalance(content)) {
        // 检查是否以}或;结尾
        if (!trimmed.empty() && 
            (trimmed.back() == '}' || trimmed.back() == ';')) {
            return true;
        }
    }
    
    return false;
}

bool CHTLUnifiedScanner::CheckBracketBalance(const std::string& content) {
    int curly = 0, square = 0, paren = 0;
    bool inString = false;
    char stringDelimiter = '\0';
    
    for (size_t i = 0; i < content.length(); ++i) {
        char ch = content[i];
        
        if (!inString && (ch == '"' || ch == '\'')) {
            inString = true;
            stringDelimiter = ch;
        } else if (inString && ch == stringDelimiter) {
            if (i > 0 && content[i-1] != '\\') {
                inString = false;
            }
        }
        
        if (!inString) {
            switch (ch) {
                case '{': curly++; break;
                case '}': curly--; break;
                case '[': square++; break;
                case ']': square--; break;
                case '(': paren++; break;
                case ')': paren--; break;
            }
        }
    }
    
    return curly == 0 && square == 0 && paren == 0;
}

void CHTLUnifiedScanner::ReportError(const std::string& message, size_t line, size_t column) {
    std::stringstream ss;
    ss << m_CurrentFile << ":" << line << ":" << column << " - " << message;
    m_Errors.push_back(ss.str());
    
    utils::Logger::GetInstance().Error(message, m_CurrentFile, line);
}

} // namespace chtl