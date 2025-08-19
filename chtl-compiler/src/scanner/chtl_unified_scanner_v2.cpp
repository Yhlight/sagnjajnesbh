#include "scanner/chtl_unified_scanner_v2.h"
#include <algorithm>
#include <sstream>
#include <regex>

namespace chtl {

CHTLUnifiedScannerV2::CHTLUnifiedScannerV2() 
    : initialSliceSize_(1024), 
      expandStep_(512) {
    
    // 初始化CHTL关键字
    chtlKeywords_ = {
        "text", "style", "script", "inherit", "delete", "insert",
        "except", "from", "as", "vir", "html", "head", "body",
        "div", "span", "p", "h1", "h2", "h3", "h4", "h5", "h6",
        "a", "img", "ul", "ol", "li", "table", "tr", "td", "th",
        "form", "input", "button", "select", "option", "textarea"
    };
    
    // 初始化CHTL JS函数
    chtlJSFunctions_ = {
        "listen", "delegate", "animate", "iNeverAway", "printMylove"
    };
}

CHTLUnifiedScannerV2::~CHTLUnifiedScannerV2() = default;

std::vector<std::unique_ptr<CodeFragment>> CHTLUnifiedScannerV2::scan(const std::string& source) {
    reset();
    
    buffer_ = std::make_unique<SliceBuffer>(source, initialSliceSize_);
    
    while (!buffer_->isAtEnd()) {
        scanSlice();
    }
    
    return std::move(fragments_);
}

void CHTLUnifiedScannerV2::reset() {
    buffer_.reset();
    errors_.clear();
    fragments_.clear();
}

void CHTLUnifiedScannerV2::scanSlice() {
    std::string slice = buffer_->getCurrentSlice();
    
    // 检查是否需要扩展切片
    while (!isReasonableCutoff() && buffer_->expandSlice(expandStep_)) {
        slice = buffer_->getCurrentSlice();
    }
    
    // 将切片按最小语法单元切割
    size_t sliceStart = buffer_->getSliceStart();
    auto units = splitIntoSyntaxUnits(slice, sliceStart);
    
    // 将语法单元组合成代码片段
    combineUnitsIntoFragments(units);
    
    // 移动到下一个切片
    if (!units.empty()) {
        size_t processedLength = units.back().endPos - sliceStart;
        buffer_->moveToNext(processedLength);
    } else {
        // 如果没有识别到任何语法单元，移动一个默认步长
        buffer_->moveToNext(std::min(slice.length(), initialSliceSize_));
    }
}

bool CHTLUnifiedScannerV2::isReasonableCutoff() {
    std::string currentSlice = buffer_->getCurrentSlice();
    if (currentSlice.empty()) {
        return true;
    }
    
    // 检查是否在字符串内部
    bool inString = false;
    char stringChar = 0;
    for (size_t i = 0; i < currentSlice.length(); ++i) {
        if (!inString && (currentSlice[i] == '"' || currentSlice[i] == '\'')) {
            inString = true;
            stringChar = currentSlice[i];
        } else if (inString && currentSlice[i] == stringChar && 
                   (i == 0 || currentSlice[i-1] != '\\')) {
            inString = false;
        }
    }
    if (inString) {
        return false;
    }
    
    // 检查花括号是否匹配
    int braceCount = 0;
    for (char ch : currentSlice) {
        if (ch == '{') braceCount++;
        else if (ch == '}') braceCount--;
    }
    if (braceCount != 0) {
        return false;
    }
    
    // 检查是否可能与下一个片段组成完整的语法单元
    return !mightFormCompleteFragment();
}

bool CHTLUnifiedScannerV2::mightFormCompleteFragment() {
    std::string currentSlice = buffer_->getCurrentSlice();
    if (currentSlice.empty()) {
        return false;
    }
    
    // 获取当前切片的最后几个字符
    size_t checkLen = std::min(size_t(20), currentSlice.length());
    std::string tail = currentSlice.substr(currentSlice.length() - checkLen);
    
    // 预览下一个切片的开始部分
    std::string nextPreview = buffer_->peekNextSlice(20);
    
    // 检查是否在CHTL元素边界
    if (tail.find_last_of(" \t\n\r") != std::string::npos) {
        size_t lastNonSpace = tail.find_last_not_of(" \t\n\r");
        if (lastNonSpace != std::string::npos) {
            std::string lastWord = tail.substr(lastNonSpace + 1);
            if (isCHTLKeyword(lastWord) && nextPreview.find('{') == 0) {
                return true;
            }
        }
    }
    
    // 检查是否在增强选择器中间
    if (tail.find("{{") != std::string::npos && 
        tail.find("}}") == std::string::npos) {
        return true;
    }
    
    // 检查是否在箭头操作符前
    if (tail.length() >= 2 && tail.substr(tail.length() - 2) == "}}" &&
        nextPreview.length() >= 2 && nextPreview.substr(0, 2) == "->") {
        return true;
    }
    
    // 检查是否在vir声明中间
    if (tail.find("vir ") != std::string::npos && 
        tail.find('=') == std::string::npos) {
        return true;
    }
    
    return false;
}

std::vector<SyntaxUnit> CHTLUnifiedScannerV2::splitIntoSyntaxUnits(
    const std::string& slice, size_t startPos) {
    
    std::vector<SyntaxUnit> units;
    size_t pos = 0;
    
    while (pos < slice.length()) {
        // 跳过空白
        while (pos < slice.length() && isWhitespace(slice[pos])) {
            pos++;
        }
        
        if (pos >= slice.length()) {
            break;
        }
        
        SyntaxUnitType type;
        std::string content = extractCompleteSyntaxUnit(slice, pos, type);
        
        if (!content.empty()) {
            auto [line, column] = buffer_->getLineColumn(startPos + pos - content.length());
            
            units.push_back({
                type,
                content,
                startPos + pos - content.length(),
                startPos + pos,
                line,
                column
            });
        }
    }
    
    return units;
}

std::string CHTLUnifiedScannerV2::extractCompleteSyntaxUnit(
    const std::string& content, size_t& pos, SyntaxUnitType& type) {
    
    size_t startPos = pos;
    
    // 检查增强选择器 {{
    if (pos + 1 < content.length() && 
        content[pos] == '{' && content[pos + 1] == '{') {
        pos += 2;
        while (pos + 1 < content.length()) {
            if (content[pos] == '}' && content[pos + 1] == '}') {
                pos += 2;
                type = SyntaxUnitType::ENHANCED_SELECTOR;
                return content.substr(startPos, pos - startPos);
            }
            pos++;
        }
        // 未找到结束符，回退
        pos = startPos;
    }
    
    // 检查箭头操作符 ->
    if (pos + 1 < content.length() && 
        content[pos] == '-' && content[pos + 1] == '>') {
        pos += 2;
        type = SyntaxUnitType::ARROW_OPERATOR;
        return "->";
    }
    
    // 检查vir声明
    if (matchString(content, pos, "vir ")) {
        pos += 4;
        // 提取vir名称
        while (pos < content.length() && isIdentifierChar(content[pos])) {
            pos++;
        }
        type = SyntaxUnitType::VIR_DECLARATION;
        return content.substr(startPos, pos - startPos);
    }
    
    // 检查标识符（可能是CHTL元素或关键字）
    if (isIdentifierChar(content[pos])) {
        while (pos < content.length() && isIdentifierChar(content[pos])) {
            pos++;
        }
        
        std::string word = content.substr(startPos, pos - startPos);
        
        // 跳过后续空白
        while (pos < content.length() && isWhitespace(content[pos])) {
            pos++;
        }
        
        // 检查是否跟随花括号（CHTL元素）
        if (pos < content.length() && content[pos] == '{') {
            // 提取整个块
            int braceCount = 0;
            do {
                if (content[pos] == '{') braceCount++;
                else if (content[pos] == '}') braceCount--;
                pos++;
            } while (pos < content.length() && braceCount > 0);
            
            if (isCHTLKeyword(word)) {
                if (word == "text") {
                    type = SyntaxUnitType::CHTL_TEXT;
                } else if (word == "style") {
                    type = SyntaxUnitType::CHTL_STYLE;
                } else if (word == "script") {
                    type = SyntaxUnitType::CHTL_SCRIPT;
                } else {
                    type = SyntaxUnitType::CHTL_ELEMENT;
                }
            } else {
                type = SyntaxUnitType::CHTL_ELEMENT;
            }
            
            return content.substr(startPos, pos - startPos);
        }
        
        // 检查是否是函数调用
        if (pos < content.length() && content[pos] == '(') {
            // 提取到函数调用结束
            int parenCount = 0;
            do {
                if (content[pos] == '(') parenCount++;
                else if (content[pos] == ')') parenCount--;
                pos++;
            } while (pos < content.length() && parenCount > 0);
            
            type = SyntaxUnitType::JS_FUNCTION_CALL;
            return content.substr(startPos, pos - startPos);
        }
        
        // 否则只是一个标识符
        type = SyntaxUnitType::UNKNOWN;
        return word;
    }
    
    // 其他字符
    pos++;
    type = SyntaxUnitType::DELIMITER;
    return content.substr(startPos, 1);
}

void CHTLUnifiedScannerV2::combineUnitsIntoFragments(const std::vector<SyntaxUnit>& units) {
    if (units.empty()) {
        return;
    }
    
    size_t i = 0;
    while (i < units.size()) {
        const auto& unit = units[i];
        
        // 聚合连续的CHTL代码片段
        if (isCHTLUnit(unit.type)) {
            size_t start = i;
            
            // 收集所有连续的CHTL相关单元
            while (i < units.size() && (isCHTLUnit(units[i].type) || 
                   units[i].type == SyntaxUnitType::DELIMITER)) {
                
                // 如果遇到特殊块（style/script），检查是否应该单独处理
                if (units[i].type == SyntaxUnitType::CHTL_STYLE) {
                    // 检查是否是局部样式（在元素内部）
                    if (isLocalStyle(units, i)) {
                        // 如果之前有CHTL片段，先创建它
                        if (i > start) {
                            fragments_.push_back(createFragment(units, start, i, FragmentType::CHTL));
                        }
                        // 创建局部样式片段
                        fragments_.push_back(createFragment(units, i, i + 1, FragmentType::LOCAL_STYLE));
                        i++;
                        start = i;
                        continue;
                    }
                } else if (units[i].type == SyntaxUnitType::CHTL_SCRIPT) {
                    // 检查是否包含CHTL JS语法
                    if (containsCHTLJSSyntax(units, i)) {
                        // 如果之前有CHTL片段，先创建它
                        if (i > start) {
                            fragments_.push_back(createFragment(units, start, i, FragmentType::CHTL));
                        }
                        // 创建CHTL JS片段
                        size_t scriptEnd = findScriptEnd(units, i);
                        fragments_.push_back(createFragment(units, i, scriptEnd, FragmentType::CHTL_JS));
                        i = scriptEnd;
                        start = i;
                        continue;
                    }
                }
                i++;
            }
            
            // 创建聚合的CHTL片段
            if (i > start) {
                fragments_.push_back(createFragment(units, start, i, FragmentType::CHTL));
            }
        }
        // 处理全局CSS
        else if (unit.type == SyntaxUnitType::CSS_RULE) {
            size_t start = i;
            // 聚合所有CSS规则
            while (i < units.size() && 
                   (units[i].type == SyntaxUnitType::CSS_RULE || 
                    units[i].type == SyntaxUnitType::DELIMITER)) {
                i++;
            }
            fragments_.push_back(createFragment(units, start, i, FragmentType::CSS));
        }
        // 处理纯JavaScript
        else if (unit.type == SyntaxUnitType::JS_STATEMENT) {
            size_t start = i;
            // 聚合所有JS语句
            while (i < units.size() && 
                   (units[i].type == SyntaxUnitType::JS_STATEMENT || 
                    units[i].type == SyntaxUnitType::JS_FUNCTION_CALL ||
                    units[i].type == SyntaxUnitType::DELIMITER)) {
                i++;
            }
            fragments_.push_back(createFragment(units, start, i, FragmentType::JAVASCRIPT));
        }
        else {
            // 跳过其他单元
            i++;
        }
    }
}

bool CHTLUnifiedScannerV2::isCHTLUnit(SyntaxUnitType type) const {
    return type == SyntaxUnitType::CHTL_ELEMENT ||
           type == SyntaxUnitType::CHTL_TEXT ||
           type == SyntaxUnitType::CHTL_STYLE ||
           type == SyntaxUnitType::CHTL_SCRIPT ||
           type == SyntaxUnitType::CHTL_TEMPLATE ||
           type == SyntaxUnitType::CHTL_CUSTOM ||
           type == SyntaxUnitType::CHTL_IMPORT;
}

bool CHTLUnifiedScannerV2::isLocalStyle(const std::vector<SyntaxUnit>& units, size_t index) const {
    // 检查style块是否在元素内部
    // 简化实现：如果前面有未闭合的元素，则认为是局部样式
    int elementDepth = 0;
    for (size_t i = 0; i < index; ++i) {
        if (units[i].type == SyntaxUnitType::CHTL_ELEMENT) {
            // 计算花括号深度
            for (char ch : units[i].content) {
                if (ch == '{') elementDepth++;
                else if (ch == '}') elementDepth--;
            }
        }
    }
    return elementDepth > 0;
}

bool CHTLUnifiedScannerV2::containsCHTLJSSyntax(const std::vector<SyntaxUnit>& units, size_t startIndex) const {
    // 查找script块的内容，检查是否包含CHTL JS语法
    if (startIndex >= units.size()) return false;
    
    const std::string& content = units[startIndex].content;
    return content.find("{{") != std::string::npos ||
           content.find("->") != std::string::npos ||
           content.find("vir ") != std::string::npos ||
           content.find("listen(") != std::string::npos ||
           content.find("delegate(") != std::string::npos ||
           content.find("animate(") != std::string::npos;
}

size_t CHTLUnifiedScannerV2::findScriptEnd(const std::vector<SyntaxUnit>& units, size_t startIndex) const {
    // 找到script块的结束位置
    size_t i = startIndex + 1;
    int braceDepth = 0;
    
    // 计算起始块的花括号
    for (char ch : units[startIndex].content) {
        if (ch == '{') braceDepth++;
        else if (ch == '}') braceDepth--;
    }
    
    // 继续查找直到花括号匹配
    while (i < units.size() && braceDepth > 0) {
        for (char ch : units[i].content) {
            if (ch == '{') braceDepth++;
            else if (ch == '}') braceDepth--;
        }
        i++;
    }
    
    return i;
}

std::unique_ptr<CodeFragment> CHTLUnifiedScannerV2::createFragment(
    const std::vector<SyntaxUnit>& units,
    size_t startIdx,
    size_t endIdx,
    FragmentType type) {
    
    if (startIdx >= endIdx || startIdx >= units.size()) {
        return nullptr;
    }
    
    // 组合内容
    std::stringstream content;
    for (size_t i = startIdx; i < endIdx && i < units.size(); ++i) {
        content << units[i].content;
        if (i < endIdx - 1) {
            content << " ";
        }
    }
    
    size_t line = units[startIdx].line;
    size_t column = units[startIdx].column;
    
    switch (type) {
        case FragmentType::CHTL:
            return std::make_unique<CHTLFragment>(content.str(), line, column);
        case FragmentType::CHTL_JS:
            return std::make_unique<CHTLJSFragment>(content.str(), line, column);
        case FragmentType::CSS:
            return std::make_unique<CSSFragment>(content.str(), line, column);
        case FragmentType::JAVASCRIPT:
            return std::make_unique<JavaScriptFragment>(content.str(), line, column);
        case FragmentType::LOCAL_STYLE:
            return std::make_unique<LocalStyleFragment>(content.str(), line, column);
        default:
            return std::make_unique<CHTLFragment>(content.str(), line, column);
    }
}

// 辅助方法实现
bool CHTLUnifiedScannerV2::isWhitespace(char ch) const {
    return ch == ' ' || ch == '\t' || ch == '\n' || ch == '\r';
}

bool CHTLUnifiedScannerV2::isIdentifierChar(char ch) const {
    return (ch >= 'a' && ch <= 'z') || 
           (ch >= 'A' && ch <= 'Z') || 
           (ch >= '0' && ch <= '9') || 
           ch == '_' || ch == '-';
}

bool CHTLUnifiedScannerV2::matchString(const std::string& content, size_t pos, 
                                      const std::string& str) const {
    if (pos + str.length() > content.length()) {
        return false;
    }
    return content.substr(pos, str.length()) == str;
}

void CHTLUnifiedScannerV2::addError(const std::string& message, size_t line, size_t column) {
    std::stringstream ss;
    ss << "Line " << line << ", Column " << column << ": " << message;
    errors_.push_back(ss.str());
}

bool CHTLUnifiedScannerV2::isCHTLKeyword(const std::string& word) const {
    return chtlKeywords_.find(word) != chtlKeywords_.end();
}

SyntaxUnitType CHTLUnifiedScannerV2::identifySyntaxUnit(const std::string& content, size_t pos) {
    // 这个方法可以用于更复杂的语法单元识别
    return SyntaxUnitType::UNKNOWN;
}

bool CHTLUnifiedScannerV2::isCHTLElementBoundary(const std::string& content, size_t pos) {
    // 检查是否在CHTL元素的边界
    if (pos >= content.length()) {
        return false;
    }
    
    // 检查是否是花括号
    if (content[pos] == '{' || content[pos] == '}') {
        return true;
    }
    
    // 检查是否是分号（属性结束）
    if (content[pos] == ';') {
        return true;
    }
    
    return false;
}

bool CHTLUnifiedScannerV2::isCHTLJSBoundary(const std::string& content, size_t pos) {
    // 检查是否在CHTL JS语法边界
    if (pos + 1 >= content.length()) {
        return false;
    }
    
    // 检查增强选择器边界
    if ((content[pos] == '{' && content[pos + 1] == '{') ||
        (content[pos] == '}' && content[pos + 1] == '}')) {
        return true;
    }
    
    // 检查箭头操作符
    if (content[pos] == '-' && content[pos + 1] == '>') {
        return true;
    }
    
    return false;
}

} // namespace chtl