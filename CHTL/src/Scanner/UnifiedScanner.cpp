#include "Scanner/UnifiedScanner.h"
#include "Utils/FileUtils.h"
#include "Utils/Logger.h"
#include <regex>
#include <stack>
#include <algorithm>
#include <sstream>
#include <set>
#include <cctype>

namespace Scanner {

// 内部实现类
class UnifiedScanner::Impl {
public:
    struct ScanState {
        size_t line = 1;
        size_t column = 1;
        size_t position = 0;
        
        // 上下文栈
        std::stack<std::string> elementStack;
        std::stack<std::string> blockStack;
        
        // 当前上下文
        CodeSlice::Context currentContext;
        
        // 括号计数
        int braceCount = 0;
        int parenCount = 0;
        int bracketCount = 0;
        
        // 字符串状态
        bool inSingleQuote = false;
        bool inDoubleQuote = false;
        bool inTemplate = false;
        bool inComment = false;
        bool inMultiLineComment = false;
        
        void UpdatePosition(char ch) {
            if (ch == '\n') {
                ++line;
                column = 1;
            } else {
                ++column;
            }
            ++position;
        }
    };
    
    std::vector<CodeSlice> Scan(const std::string& content, 
                                const std::string& filename,
                                const ScanConfig& config) {
        std::vector<CodeSlice> slices;
        ScanState state;
        
        size_t sliceStart = 0;
        std::string currentSlice;
        currentSlice.reserve(config.initialSliceSize);
        
        for (size_t i = 0; i < content.length(); ++i) {
            char ch = content[i];
            char nextCh = (i + 1 < content.length()) ? content[i + 1] : '\0';
            
            // 处理注释
            if (!state.inSingleQuote && !state.inDoubleQuote && !state.inTemplate) {
                if (ch == '/' && nextCh == '/') {
                    state.inComment = true;
                } else if (ch == '/' && nextCh == '*') {
                    state.inMultiLineComment = true;
                    ++i; // 跳过 *
                    state.UpdatePosition(nextCh);
                } else if (state.inMultiLineComment && ch == '*' && nextCh == '/') {
                    state.inMultiLineComment = false;
                    ++i; // 跳过 /
                    state.UpdatePosition(nextCh);
                }
            }
            
            // 处理字符串
            if (!state.inComment && !state.inMultiLineComment) {
                if (ch == '\'' && !state.inDoubleQuote && !state.inTemplate) {
                    state.inSingleQuote = !state.inSingleQuote;
                } else if (ch == '"' && !state.inSingleQuote && !state.inTemplate) {
                    state.inDoubleQuote = !state.inDoubleQuote;
                } else if (ch == '`' && !state.inSingleQuote && !state.inDoubleQuote) {
                    state.inTemplate = !state.inTemplate;
                }
            }
            
            // 处理括号（不在字符串或注释中）
            if (!state.inSingleQuote && !state.inDoubleQuote && !state.inTemplate &&
                !state.inComment && !state.inMultiLineComment) {
                if (ch == '{') {
                    ++state.braceCount;
                    
                    // 检查是否进入特殊块
                    std::string precedingText = GetPrecedingIdentifier(content, i);
                    if (precedingText == "style") {
                        if (state.elementStack.empty()) {
                            state.currentContext.inGlobalStyle = true;
                        } else {
                            state.currentContext.inLocalStyle = true;
                        }
                        state.blockStack.push("style");
                    } else if (precedingText == "script") {
                        if (state.elementStack.empty()) {
                            state.currentContext.inGlobalScript = true;
                        } else {
                            state.currentContext.inLocalScript = true;
                        }
                        state.blockStack.push("script");
                    } else if (precedingText == "text") {
                        state.blockStack.push("text");
                    } else if (IsHTMLElement(precedingText) || IsCHTLElement(precedingText)) {
                        state.elementStack.push(precedingText);
                        state.currentContext.parentElement = precedingText;
                    }
                    
                    // 检查是否需要切片
                    if (ShouldSliceHere(state, config)) {
                        CreateSlice(slices, currentSlice, sliceStart, i, state, filename);
                        sliceStart = i;
                        currentSlice.clear();
                    }
                } else if (ch == '}') {
                    --state.braceCount;
                    
                    // 检查是否离开特殊块
                    if (!state.blockStack.empty()) {
                        std::string topBlock = state.blockStack.top();
                        if (topBlock == "style") {
                            state.currentContext.inLocalStyle = false;
                            state.currentContext.inGlobalStyle = false;
                            state.blockStack.pop();
                        } else if (topBlock == "script") {
                            state.currentContext.inLocalScript = false;
                            state.currentContext.inGlobalScript = false;
                            state.blockStack.pop();
                        } else if (topBlock == "text") {
                            state.blockStack.pop();
                        }
                    }
                    
                    if (!state.elementStack.empty() && state.braceCount < state.elementStack.size()) {
                        state.elementStack.pop();
                        state.currentContext.parentElement = state.elementStack.empty() ? 
                            "" : state.elementStack.top();
                    }
                } else if (ch == '(') {
                    ++state.parenCount;
                } else if (ch == ')') {
                    --state.parenCount;
                } else if (ch == '[') {
                    ++state.bracketCount;
                } else if (ch == ']') {
                    --state.bracketCount;
                }
            }
            
            // 处理行尾注释
            if (state.inComment && ch == '\n') {
                state.inComment = false;
            }
            
            currentSlice += ch;
            state.UpdatePosition(ch);
            
            // 检查切片大小
            if (currentSlice.size() >= config.maxSliceSize) {
                CreateSlice(slices, currentSlice, sliceStart, i, state, filename);
                sliceStart = i + 1;
                currentSlice.clear();
            }
        }
        
        // 处理最后的切片
        if (!currentSlice.empty()) {
            CreateSlice(slices, currentSlice, sliceStart, content.length() - 1, 
                       state, filename);
        }
        
        return slices;
    }
    
private:
    bool ShouldSliceHere(const ScanState& state, const ScanConfig& config) {
        // 在特定的语法边界进行切片
        if (state.braceCount == 0) return true;
        if (state.currentContext.inLocalStyle && state.braceCount == 1) return true;
        if (state.currentContext.inLocalScript && state.braceCount == 1) return true;
        return false;
    }
    
    void CreateSlice(std::vector<CodeSlice>& slices,
                    const std::string& content,
                    size_t startPos,
                    size_t endPos,
                    const ScanState& state,
                    const std::string& filename) {
        if (content.empty()) return;
        
        CodeSlice slice;
        slice.content = content;
        slice.filename = filename;
        slice.startLine = GetLineNumber(content, 0);
        slice.startColumn = GetColumnNumber(content, 0);
        slice.endLine = state.line;
        slice.endColumn = state.column;
        slice.context = state.currentContext;
        
        // 识别切片类型
        slice.type = IdentifyType(content, state.currentContext);
        
        slices.push_back(slice);
    }
    
    SliceType IdentifyType(const std::string& content, 
                          const CodeSlice::Context& context) {
        // 去除首尾空白
        std::string trimmed = Trim(content);
        
        // 检查是否为CHTL JS语法
        if (context.inLocalScript || context.inGlobalScript) {
            if (ContainsCHTLJSSyntax(trimmed)) {
                return SliceType::CHTLJS;
            }
            return SliceType::JavaScript;
        }
        
        // 检查是否为CSS
        if (context.inLocalStyle || context.inGlobalStyle) {
            if (ContainsCHTLStyleSyntax(trimmed)) {
                return SliceType::CHTL;
            }
            return SliceType::CSS;
        }
        
        // 检查是否为CHTL语法
        if (ContainsCHTLSyntax(trimmed)) {
            return SliceType::CHTL;
        }
        
        // 检查是否为纯文本
        if (IsPureText(trimmed)) {
            return SliceType::Text;
        }
        
        return SliceType::Unknown;
    }
    
    bool ContainsCHTLJSSyntax(const std::string& content) {
        // 检查增强选择器 {{...}}
        if (content.find("{{") != std::string::npos && 
            content.find("}}") != std::string::npos) {
            return true;
        }
        
        // 检查虚对象调用 ->
        if (content.find("->") != std::string::npos) {
            // 需要更精确的检查，避免误判
            std::regex virCallRegex(R"(\b\w+\s*->\s*\w+)");
            if (std::regex_search(content, virCallRegex)) {
                return true;
            }
        }
        
        // 检查CHTL JS特殊函数
        static const std::vector<std::string> chtljsFunctions = {
            "listen", "delegate", "animate", "iNeverAway"
        };
        
        for (const auto& func : chtljsFunctions) {
            if (content.find(func + "(") != std::string::npos ||
                content.find(func + " (") != std::string::npos) {
                return true;
            }
        }
        
        return false;
    }
    
    bool ContainsCHTLStyleSyntax(const std::string& content) {
        // 检查CHTL样式语法，如 @Style, @Var 等
        std::regex chtlStyleRegex(R"(@(Style|Var)\s+\w+)");
        return std::regex_search(content, chtlStyleRegex);
    }
    
    bool ContainsCHTLSyntax(const std::string& content) {
        // 检查CHTL核心语法
        static const std::vector<std::string> chtlKeywords = {
            "[Template]", "[Custom]", "[Origin]", "[Import]", 
            "[Namespace]", "[Configuration]", "[Info]",
            "@Element", "@Style", "@Var", "@Html", "@JavaScript",
            "inherit", "delete", "insert", "except"
        };
        
        for (const auto& keyword : chtlKeywords) {
            if (content.find(keyword) != std::string::npos) {
                return true;
            }
        }
        
        return false;
    }
    
    bool IsPureText(const std::string& content) {
        // 简单判断是否为纯文本
        return !ContainsCHTLSyntax(content) && 
               !ContainsCHTLJSSyntax(content) &&
               !ContainsCHTLStyleSyntax(content);
    }
    
    std::string GetPrecedingIdentifier(const std::string& content, size_t pos) {
        if (pos == 0) return "";
        
        size_t end = pos - 1;
        while (end > 0 && std::isspace(content[end])) --end;
        
        size_t start = end;
        while (start > 0 && (std::isalnum(content[start]) || content[start] == '_')) {
            --start;
        }
        
        if (start < end) {
            return content.substr(start + 1, end - start);
        }
        
        return "";
    }
    
    bool IsHTMLElement(const std::string& name) {
        static const std::set<std::string> htmlElements = {
            "html", "head", "body", "div", "span", "p", "a", "img",
            "ul", "ol", "li", "table", "tr", "td", "th", "form",
            "input", "button", "select", "option", "textarea",
            "h1", "h2", "h3", "h4", "h5", "h6", "header", "footer",
            "nav", "section", "article", "aside", "main"
        };
        
        return htmlElements.find(name) != htmlElements.end();
    }
    
    bool IsCHTLElement(const std::string& name) {
        return name == "text" || name == "style" || name == "script";
    }
    
    std::string Trim(const std::string& str) {
        size_t first = str.find_first_not_of(" \t\n\r");
        if (first == std::string::npos) return "";
        size_t last = str.find_last_not_of(" \t\n\r");
        return str.substr(first, last - first + 1);
    }
    
    size_t GetLineNumber(const std::string& content, size_t pos) {
        size_t line = 1;
        for (size_t i = 0; i < pos && i < content.length(); ++i) {
            if (content[i] == '\n') ++line;
        }
        return line;
    }
    
    size_t GetColumnNumber(const std::string& content, size_t pos) {
        size_t column = 1;
        for (size_t i = 0; i < pos && i < content.length(); ++i) {
            if (content[i] == '\n') {
                column = 1;
            } else {
                ++column;
            }
        }
        return column;
    }
};

// UnifiedScanner 实现
UnifiedScanner::UnifiedScanner() 
    : impl_(std::make_unique<Impl>()), config_() {}

UnifiedScanner::UnifiedScanner(const ScanConfig& config) 
    : impl_(std::make_unique<Impl>()), config_(config) {}

UnifiedScanner::~UnifiedScanner() = default;

std::vector<CodeSlice> UnifiedScanner::ScanFile(const std::string& filename) {
    try {
        std::string content = Utils::FileUtils::ReadFile(filename);
        return ScanString(content, filename);
    } catch (const std::exception& e) {
        ReportError("无法读取文件: " + std::string(e.what()), 0, 0);
        return {};
    }
}

std::vector<CodeSlice> UnifiedScanner::ScanString(const std::string& content, 
                                                  const std::string& sourceName) {
    std::vector<CodeSlice> slices = impl_->Scan(content, sourceName, config_);
    
    // 后处理
    ProcessCHTLSyntax(slices);
    ProcessCHTLJSSyntax(slices);
    AdjustSliceBoundaries(slices);
    MergeAdjacentSlices(slices);
    
    // 验证
    for (const auto& slice : slices) {
        if (!ValidateSlice(slice)) {
            ReportError("无效的代码片段", slice.startLine, slice.startColumn);
        }
    }
    
    return slices;
}

void UnifiedScanner::ProcessCHTLSyntax(std::vector<CodeSlice>& slices) {
    // 处理CHTL特殊语法，确保正确切割
    for (auto& slice : slices) {
        if (slice.type != SliceType::CHTL) continue;
        
        // TODO: 实现CHTL语法的特殊处理
    }
}

void UnifiedScanner::ProcessCHTLJSSyntax(std::vector<CodeSlice>& slices) {
    // 处理CHTL JS特殊语法
    for (size_t i = 0; i < slices.size(); ++i) {
        auto& slice = slices[i];
        if (slice.type != SliceType::CHTLJS && slice.type != SliceType::JavaScript) {
            continue;
        }
        
        // 查找并标记增强选择器
        size_t pos = 0;
        while ((pos = slice.content.find("{{", pos)) != std::string::npos) {
            size_t endPos = slice.content.find("}}", pos + 2);
            if (endPos != std::string::npos) {
                // 标记这部分为CHTL JS语法
                slice.type = SliceType::CHTLJS;
                pos = endPos + 2;
            } else {
                break;
            }
        }
        
        // 查找虚对象调用
        std::regex virCallRegex(R"(\b(\w+)\s*->\s*(\w+))");
        if (std::regex_search(slice.content, virCallRegex)) {
            slice.type = SliceType::CHTLJS;
        }
    }
}

void UnifiedScanner::AdjustSliceBoundaries(std::vector<CodeSlice>& slices) {
    // 调整切片边界，确保语法完整性
    for (size_t i = 0; i < slices.size(); ++i) {
        // TODO: 实现边界调整逻辑
    }
}

void UnifiedScanner::MergeAdjacentSlices(std::vector<CodeSlice>& slices) {
    if (slices.empty()) return;
    
    std::vector<CodeSlice> merged;
    merged.push_back(slices[0]);
    
    for (size_t i = 1; i < slices.size(); ++i) {
        auto& last = merged.back();
        auto& current = slices[i];
        
        // 合并相同类型且上下文相同的切片
        if (last.type == current.type &&
            last.context.inLocalStyle == current.context.inLocalStyle &&
            last.context.inLocalScript == current.context.inLocalScript &&
            last.context.inGlobalStyle == current.context.inGlobalStyle &&
            last.context.inGlobalScript == current.context.inGlobalScript) {
            
            last.content += current.content;
            last.endLine = current.endLine;
            last.endColumn = current.endColumn;
        } else {
            merged.push_back(current);
        }
    }
    
    slices = std::move(merged);
}

bool UnifiedScanner::ValidateSlice(const CodeSlice& slice) {
    // 基本验证
    if (slice.content.empty()) return false;
    if (slice.startLine > slice.endLine) return false;
    if (slice.startLine == slice.endLine && slice.startColumn > slice.endColumn) return false;
    
    return true;
}

void UnifiedScanner::ReportError(const std::string& message, size_t line, size_t column) {
    if (errorCallback_) {
        errorCallback_(message, line, column);
    } else {
        LOG_ERROR(message + " at " + std::to_string(line) + ":" + std::to_string(column));
    }
}

// 工具函数实现
namespace ScannerUtils {

bool IsCHTLKeyword(const std::string& word) {
    static const std::set<std::string> keywords = {
        "text", "style", "script", "inherit", "delete", "insert",
        "after", "before", "replace", "at", "top", "bottom",
        "from", "as", "except", "vir"
    };
    
    return keywords.find(word) != keywords.end();
}

bool IsCHTLJSSyntax(const std::string& text) {
    // 检查增强选择器
    if (text.find("{{") != std::string::npos && text.find("}}") != std::string::npos) {
        return true;
    }
    
    // 检查虚对象调用
    std::regex virCallRegex(R"(\b\w+\s*->\s*\w+)");
    if (std::regex_search(text, virCallRegex)) {
        return true;
    }
    
    return false;
}

std::string ExtractEnhancedSelector(const std::string& text, size_t& pos) {
    size_t start = text.find("{{", pos);
    if (start == std::string::npos) return "";
    
    size_t end = text.find("}}", start + 2);
    if (end == std::string::npos) return "";
    
    pos = end + 2;
    return text.substr(start, end - start + 2);
}

std::string ExtractVirtualObjectCall(const std::string& text, size_t& pos) {
    std::regex virCallRegex(R"(\b(\w+)\s*->\s*(\w+))");
    std::smatch match;
    
    std::string searchText = text.substr(pos);
    if (std::regex_search(searchText, match, virCallRegex)) {
        pos += match.position() + match.length();
        return match.str();
    }
    
    return "";
}

std::string GetSliceTypeName(SliceType type) {
    switch (type) {
        case SliceType::CHTL:       return "CHTL";
        case SliceType::CHTLJS:     return "CHTL JS";
        case SliceType::CSS:        return "CSS";
        case SliceType::JavaScript: return "JavaScript";
        case SliceType::Text:       return "Text";
        case SliceType::Comment:    return "Comment";
        case SliceType::Unknown:    return "Unknown";
        default:                    return "Unknown";
    }
}

} // namespace ScannerUtils

} // namespace Scanner