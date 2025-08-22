#include "Core/CHTLUnifiedScanner.hpp"
#include <algorithm>
#include <sstream>
#include <iostream>

namespace CHTL {

CHTLUnifiedScanner::CHTLUnifiedScanner() 
    : debugMode_(false) {
    InitializePatterns();
    statistics_ = {};
}

void CHTLUnifiedScanner::InitializePatterns() {
    // CHTL元素模式
    elementPattern_ = std::regex(R"(\b[a-zA-Z][a-zA-Z0-9]*\s*\{)");
    
    // 样式块模式
    stylePattern_ = std::regex(R"(\bstyle\s*\{)");
    
    // 脚本块模式（暂时保留，但主要处理CHTL语法）
    scriptPattern_ = std::regex(R"(\bscript\s*\{)");
    
    // 注释模式
    commentPattern_ = std::regex(R"((//.*$)|(--.*$)|(/\*.*?\*/))");
    
    // 字符串模式
    stringPattern_ = std::regex(R"(("[^"]*")|('[^']*'))");
    
    // 模板模式
    templatePattern_ = std::regex(R"(\[Template\]\s*@(Style|Element|Var)\s+\w+)");
    
    // 自定义模式
    customPattern_ = std::regex(R"(\[Custom\]\s*@(Style|Element|Var)\s+\w+)");
    
    // 原始嵌入模式
    originPattern_ = std::regex(R"(\[Origin\]\s*@(Html|Style|JavaScript)(\s+\w+)?)");
    
    // 导入模式
    importPattern_ = std::regex(R"(\[Import\]\s*@\w+\s+from\s+[\w/.]+)");
    
    // 配置模式
    configPattern_ = std::regex(R"(\[Configuration\](\s*@Config\s+\w+)?)");
    
    // 命名空间模式
    namespacePattern_ = std::regex(R"(\[Namespace\]\s+\w+)");
}

std::vector<CodeFragment> CHTLUnifiedScanner::ScanSource(const String& source, const String& filename) {
    std::vector<CodeFragment> fragments;
    
    if (source.empty()) {
        return fragments;
    }
    
    ScanContext context;
    context.location = SourceLocation(filename, 1, 1, 0);
    
    size_t position = 0;
    size_t line = 1;
    size_t column = 1;
    
    String currentFragment;
    FragmentType currentType = FragmentType::CHTL;
    SourceLocation fragmentStart(filename, line, column, position);
    
    while (position < source.length()) {
        char ch = source[position];
        
        // 更新位置信息
        if (ch == '\n') {
            line++;
            column = 1;
        } else {
            column++;
        }
        
        // 检查状态转换
        ScanState oldState = context.state;
        UpdateScanState(context, source, position);
        
        // 如果状态发生变化，可能需要切分片段
        if (oldState != context.state || ShouldSplitFragment(source, position, currentType)) {
            // 完成当前片段
            if (!currentFragment.empty()) {
                fragments.emplace_back(currentType, currentFragment, fragmentStart);
                UpdateStatistics(currentType);
                
                if (debugMode_) {
                    LogDebugInfo("片段: " + FragmentTypeToString(currentType) + 
                               " [" + std::to_string(fragmentStart.line) + ":" + 
                               std::to_string(fragmentStart.column) + "] " + 
                               currentFragment.substr(0, 50) + "...");
                }
            }
            
            // 开始新片段
            currentFragment.clear();
            currentType = DetermineFragmentType(source, position, context);
            fragmentStart = SourceLocation(filename, line, column, position);
        }
        
        currentFragment += ch;
        position++;
    }
    
    // 处理最后一个片段
    if (!currentFragment.empty()) {
        fragments.emplace_back(currentType, currentFragment, fragmentStart);
        UpdateStatistics(currentType);
    }
    
    // 后处理：合并相邻的同类型片段
    fragments = PostProcessFragments(fragments);
    
    statistics_.totalFragments = fragments.size();
    
    return fragments;
}

void CHTLUnifiedScanner::UpdateScanState(ScanContext& context, const String& source, size_t position) {
    // 检查注释
    if (IsStartOfComment(source, position)) {
        context.state = ScanState::InComment;
        return;
    }
    
    // 检查字符串
    if (IsStartOfString(source, position)) {
        context.state = ScanState::InString;
        return;
    }
    
    // 检查各种CHTL语法块
    if (IsStartOfTemplate(source, position)) {
        context.state = ScanState::InTemplate;
        return;
    }
    
    if (IsStartOfCustom(source, position)) {
        context.state = ScanState::InCustom;
        return;
    }
    
    if (IsStartOfOrigin(source, position)) {
        context.state = ScanState::InOrigin;
        return;
    }
    
    if (IsStartOfConfiguration(source, position)) {
        context.state = ScanState::InConfiguration;
        return;
    }
    
    if (IsStartOfNamespace(source, position)) {
        context.state = ScanState::InNamespace;
        return;
    }
    
    if (IsStartOfImport(source, position)) {
        context.state = ScanState::InImport;
        return;
    }
    
    // 检查样式块
    if (IsStartOfStyle(source, position)) {
        context.state = ScanState::InStyle;
        return;
    }
    
    // 检查脚本块
    if (IsStartOfScript(source, position)) {
        context.state = ScanState::InScript;
        return;
    }
    
    // 检查元素
    if (IsStartOfElement(source, position)) {
        context.state = ScanState::InElement;
        return;
    }
    
    // 默认为全局状态
    context.state = ScanState::Global;
}

FragmentType CHTLUnifiedScanner::DetermineFragmentType(const String& source, size_t position, const ScanContext& context) {
    // 根据当前扫描状态确定片段类型
    switch (context.state) {
        case ScanState::InStyle:
            // 检查是否为局部样式块（CHTL语法）还是全局样式块（CSS语法）
            if (IsInLocalStyleBlock(source, position)) {
                return FragmentType::CHTL;  // 局部样式块包含CHTL语法
            } else {
                return FragmentType::CSS;   // 全局样式块是纯CSS
            }
            
        case ScanState::InScript:
            // 脚本块暂时都作为CHTL处理（后续可能包含CHTL JS语法）
            return FragmentType::CHTL;
            
        case ScanState::InTemplate:
        case ScanState::InCustom:
        case ScanState::InConfiguration:
        case ScanState::InNamespace:
        case ScanState::InImport:
        case ScanState::InElement:
        case ScanState::Global:
            return FragmentType::CHTL;
            
        case ScanState::InOrigin:
            // 原始嵌入根据类型确定
            return DetermineOriginType(source, position);
            
        default:
            return FragmentType::CHTL;
    }
}

FragmentType CHTLUnifiedScanner::DetermineOriginType(const String& source, size_t position) {
    // 查找原始嵌入的类型标识
    size_t searchStart = position;
    while (searchStart > 0 && source[searchStart] != '[') {
        searchStart--;
    }
    
    String originBlock = source.substr(searchStart, std::min(size_t(100), source.length() - searchStart));
    
    if (originBlock.find("@Html") != String::npos) {
        return FragmentType::CHTL;  // HTML原始嵌入作为CHTL处理
    } else if (originBlock.find("@Style") != String::npos) {
        return FragmentType::CSS;
    } else if (originBlock.find("@JavaScript") != String::npos) {
        return FragmentType::JavaScript;
    }
    
    return FragmentType::CHTL;  // 默认
}

bool CHTLUnifiedScanner::ShouldSplitFragment(const String& source, size_t position, FragmentType currentType) {
    // 检查是否需要在当前位置分割片段
    (void)currentType; // 避免未使用参数警告
    
    // 在块结束时分割
    if (position > 0 && source[position - 1] == '}') {
        return true;
    }
    
    // 在新的语法块开始时分割
    if (IsStartOfNewSyntaxBlock(source, position)) {
        return true;
    }
    
    return false;
}

bool CHTLUnifiedScanner::IsStartOfComment(const String& source, size_t position) {
    if (position + 1 >= source.length()) return false;
    
    // 检查 //
    if (source[position] == '/' && source[position + 1] == '/') {
        return true;
    }
    
    // 检查 --
    if (source[position] == '-' && source[position + 1] == '-') {
        return true;
    }
    
    // 检查 /*
    if (source[position] == '/' && source[position + 1] == '*') {
        return true;
    }
    
    return false;
}

bool CHTLUnifiedScanner::IsStartOfString(const String& source, size_t position) {
    char ch = source[position];
    return ch == '"' || ch == '\'';
}

bool CHTLUnifiedScanner::IsStartOfTemplate(const String& source, size_t position) {
    return MatchesPattern(source, position, R"(\[Template\])");
}

bool CHTLUnifiedScanner::IsStartOfCustom(const String& source, size_t position) {
    return MatchesPattern(source, position, R"(\[Custom\])");
}

bool CHTLUnifiedScanner::IsStartOfOrigin(const String& source, size_t position) {
    return MatchesPattern(source, position, R"(\[Origin\])");
}

bool CHTLUnifiedScanner::IsStartOfConfiguration(const String& source, size_t position) {
    return MatchesPattern(source, position, R"(\[Configuration\])");
}

bool CHTLUnifiedScanner::IsStartOfNamespace(const String& source, size_t position) {
    return MatchesPattern(source, position, R"(\[Namespace\])");
}

bool CHTLUnifiedScanner::IsStartOfImport(const String& source, size_t position) {
    return MatchesPattern(source, position, R"(\[Import\])");
}

bool CHTLUnifiedScanner::IsStartOfStyle(const String& source, size_t position) {
    return MatchesPattern(source, position, R"(\bstyle\s*\{)");
}

bool CHTLUnifiedScanner::IsStartOfScript(const String& source, size_t position) {
    return MatchesPattern(source, position, R"(\bscript\s*\{)");
}

bool CHTLUnifiedScanner::IsStartOfElement(const String& source, size_t position) {
    // 检查是否为HTML元素开始
    if (position >= source.length()) return false;
    
    char ch = source[position];
    if (!std::isalpha(ch)) return false;
    
    // 提取标识符
    size_t end = position;
    while (end < source.length() && (std::isalnum(source[end]) || source[end] == '-')) {
        end++;
    }
    
    String identifier = source.substr(position, end - position);
    
    // 跳过空白
    while (end < source.length() && std::isspace(source[end])) {
        end++;
    }
    
    // 检查是否跟着 { 或属性
    if (end < source.length() && source[end] == '{') {
        return IsHtmlElement(identifier);
    }
    
    return false;
}

bool CHTLUnifiedScanner::IsStartOfNewSyntaxBlock(const String& source, size_t position) {
    return IsStartOfTemplate(source, position) ||
           IsStartOfCustom(source, position) ||
           IsStartOfOrigin(source, position) ||
           IsStartOfConfiguration(source, position) ||
           IsStartOfNamespace(source, position) ||
           IsStartOfImport(source, position);
}

bool CHTLUnifiedScanner::IsInLocalStyleBlock(const String& source, size_t position) {
    // 检查当前样式块是否为局部样式块
    // 局部样式块通常在元素内部
    
    size_t searchStart = position;
    int braceLevel = 0;
    
    // 向前搜索，找到包含的上下文
    while (searchStart > 0) {
        searchStart--;
        char ch = source[searchStart];
        
        if (ch == '}') {
            braceLevel++;
        } else if (ch == '{') {
            braceLevel--;
            if (braceLevel < 0) {
                // 找到了包含的块，检查是否为元素
                size_t blockStart = searchStart;
                while (blockStart > 0 && source[blockStart - 1] != '\n' && source[blockStart - 1] != '}') {
                    blockStart--;
                }
                
                String blockHeader = source.substr(blockStart, searchStart - blockStart);
                
                // 如果包含HTML元素名称，则为局部样式块
                return std::regex_search(blockHeader, std::regex(R"(\b[a-zA-Z][a-zA-Z0-9-]*\s*$)"));
            }
        }
    }
    
    return false;  // 默认为全局样式块
}

bool CHTLUnifiedScanner::IsHtmlElement(const String& identifier) {
    // HTML元素列表
    static const StringSet htmlElements = {
        "html", "head", "title", "meta", "link", "style", "script",
        "body", "div", "span", "p", "h1", "h2", "h3", "h4", "h5", "h6",
        "a", "img", "ul", "ol", "li", "table", "tr", "td", "th",
        "form", "input", "button", "select", "option", "textarea",
        "header", "footer", "nav", "section", "article", "aside",
        "main", "figure", "figcaption", "blockquote", "pre", "code",
        "strong", "em", "b", "i", "u", "br", "hr"
    };
    
    return htmlElements.find(identifier) != htmlElements.end();
}

bool CHTLUnifiedScanner::MatchesPattern(const String& source, size_t position, const String& pattern) {
    if (position >= source.length()) return false;
    
    std::regex regex(pattern);
    std::smatch match;
    String substring = source.substr(position, std::min(size_t(100), source.length() - position));
    
    return std::regex_search(substring, match, regex) && match.position() == 0;
}

std::vector<CodeFragment> CHTLUnifiedScanner::PostProcessFragments(const std::vector<CodeFragment>& fragments) {
    std::vector<CodeFragment> processed;
    
    for (size_t i = 0; i < fragments.size(); ++i) {
        const auto& current = fragments[i];
        
        // 检查是否可以与前一个片段合并
        if (!processed.empty() && 
            processed.back().type == current.type &&
            ShouldMergeFragments(processed.back(), current)) {
            
            // 合并片段
            processed.back().content += current.content;
        } else {
            processed.push_back(current);
        }
    }
    
    return processed;
}

bool CHTLUnifiedScanner::ShouldMergeFragments(const CodeFragment& first, const CodeFragment& second) {
    // 只合并相同类型的片段
    if (first.type != second.type) {
        return false;
    }
    
    // 不合并过大的片段
    if (first.content.length() + second.content.length() > 10000) {
        return false;
    }
    
    // CHTL片段可以合并
    if (first.type == FragmentType::CHTL) {
        return true;
    }
    
    return false;
}

void CHTLUnifiedScanner::UpdateStatistics(FragmentType type) {
    switch (type) {
        case FragmentType::CHTL:
            statistics_.chtlFragments++;
            break;
        case FragmentType::CSS:
            statistics_.cssFragments++;
            break;
        case FragmentType::JavaScript:
            statistics_.jsFragments++;
            break;
        default:
            break;
    }
}

void CHTLUnifiedScanner::SetOptions(bool enableDebug) {
    debugMode_ = enableDebug;
}

void CHTLUnifiedScanner::LogDebugInfo(const String& message) const {
    if (debugMode_) {
        std::cout << "[CHTLUnifiedScanner] " << message << std::endl;
    }
}

String CHTLUnifiedScanner::FragmentTypeToString(FragmentType type) {
    switch (type) {
        case FragmentType::CHTL: return "CHTL";
        case FragmentType::CSS: return "CSS";
        case FragmentType::JavaScript: return "JavaScript";
        default: return "Unknown";
    }
}

} // namespace CHTL