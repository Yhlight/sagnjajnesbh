#include "CommentGenerator.h"
#include <regex>
#include <sstream>
#include <algorithm>
#include <map>

namespace chtl {
namespace ast {

// CommentGenerator 实现
CommentGenerator::CommentGenerator() {
    // 设置默认格式选项
    format_options_.preserve_indentation = true;
    format_options_.add_blank_lines = false;
    format_options_.align_comments = false;
    format_options_.max_line_length = 80;
    format_options_.indent_string = "    ";
}

CommentGenerator::~CommentGenerator() = default;

std::string CommentGenerator::generateContextualComment(const CommentNode& comment, 
                                                       const std::string& surrounding_code) const {
    CodeBlockType block_type = inferBlockType(surrounding_code);
    return generateContextualComment(comment, block_type);
}

std::string CommentGenerator::generateContextualComment(const CommentNode& comment, 
                                                       CodeBlockType block_type) const {
    CommentContext target_context = mapBlockTypeToContext(block_type);
    return comment.generateComment(target_context);
}

std::vector<std::string> CommentGenerator::generateCommentsForBlock(
    const std::vector<std::unique_ptr<CommentNode>>& comments,
    CodeBlockType block_type) const {
    
    std::vector<std::string> result;
    for (const auto& comment : comments) {
        if (shouldPreserveComment(*comment, block_type)) {
            result.push_back(generateContextualComment(*comment, block_type));
        }
    }
    return result;
}

std::string CommentGenerator::processCodeWithComments(
    const std::string& code,
    const std::vector<std::unique_ptr<CommentNode>>& comments) const {
    
    if (comments.empty()) {
        return code;
    }
    
    CodeBlockType block_type = inferBlockType(code);
    std::ostringstream result;
    
    // 在代码前添加注释
    for (const auto& comment : comments) {
        if (shouldPreserveComment(*comment, block_type)) {
            std::string generated_comment = generateContextualComment(*comment, block_type);
            result << generated_comment << "\n";
        }
    }
    
    result << code;
    return result.str();
}

CodeBlockType CommentGenerator::inferBlockType(const std::string& code) const {
    // 首先检查CHTL语法，因为它可能包含CSS/JS/HTML语法
    if (containsCHTLSyntax(code)) {
        // 如果同时包含CHTL JS语法，优先判断为CHTL脚本
        if (containsCHTLJSSyntax(code)) {
            return CodeBlockType::CHTL_SCRIPT;
        }
        // 如果包含CSS特有语法（不只是花括号），判断为CHTL样式
        if (containsSpecificCSSSyntax(code)) {
            return CodeBlockType::CHTL_STYLE;
        }
        // 否则是CHTL模板
        return CodeBlockType::CHTL_TEMPLATE;
    }
    
    // 检查纯CSS语法
    if (containsCSSSyntax(code)) {
        return CodeBlockType::PURE_CSS;
    }
    
    // 检查JavaScript语法
    if (containsJSSyntax(code)) {
        return CodeBlockType::PURE_JS;
    }
    
    // 检查HTML语法
    if (containsHTMLSyntax(code)) {
        return CodeBlockType::HTML_OUTPUT;
    }
    
    return CodeBlockType::MIXED;
}

CommentContext CommentGenerator::mapBlockTypeToContext(CodeBlockType block_type) const {
    switch (block_type) {
        case CodeBlockType::CHTL_TEMPLATE:
            return CommentContext::CHTL;
        case CodeBlockType::CHTL_STYLE:
            return CommentContext::CSS;
        case CodeBlockType::CHTL_SCRIPT:
            return CommentContext::CHTL_JS;
        case CodeBlockType::PURE_CSS:
            return CommentContext::CSS;
        case CodeBlockType::PURE_JS:
            return CommentContext::JAVASCRIPT;
        case CodeBlockType::HTML_OUTPUT:
            return CommentContext::HTML;
        case CodeBlockType::MIXED:
        default:
            return CommentContext::CHTL;
    }
}

std::string CommentGenerator::insertCommentAtPosition(
    const std::string& code,
    const CommentNode& comment,
    const CommentPosition& position) const {
    
    std::vector<std::string> lines = splitCodeIntoLines(code);
    
    if (position.line >= lines.size()) {
        // 如果位置超出范围，在末尾添加
        std::string indentation = format_options_.preserve_indentation ? 
            format_options_.indent_string : "";
        std::string formatted_comment = formatComment(
            generateContextualComment(comment, inferBlockType(code)), 
            indentation);
        lines.push_back(formatted_comment);
    } else {
        // 在指定位置插入
        std::string indentation = format_options_.preserve_indentation ? 
            detectIndentation(code, position) : "";
        std::string formatted_comment = formatComment(
            generateContextualComment(comment, inferBlockType(code)), 
            indentation);
        lines.insert(lines.begin() + position.line, formatted_comment);
    }
    
    return joinLines(lines);
}

std::string CommentGenerator::insertCommentsInCode(
    const std::string& code,
    const std::vector<std::pair<CommentNode*, CommentPosition>>& positioned_comments) const {
    
    std::vector<std::string> lines = splitCodeIntoLines(code);
    CodeBlockType block_type = inferBlockType(code);
    
    // 按行号排序（从后往前插入以避免位置偏移）
    std::vector<std::pair<CommentNode*, CommentPosition>> sorted_comments = positioned_comments;
    std::sort(sorted_comments.begin(), sorted_comments.end(),
        [](const auto& a, const auto& b) {
            return a.second.line > b.second.line;
        });
    
    for (const auto& pair : sorted_comments) {
        const CommentNode* comment = pair.first;
        const CommentPosition& position = pair.second;
        
        if (comment && shouldPreserveComment(*comment, block_type)) {
            if (position.line <= lines.size()) {
                std::string indentation = format_options_.preserve_indentation ? 
                    detectIndentation(code, position) : "";
                std::string formatted_comment = formatComment(
                    generateContextualComment(*comment, block_type), 
                    indentation);
                lines.insert(lines.begin() + position.line, formatted_comment);
            }
        }
    }
    
    return joinLines(lines);
}

std::string CommentGenerator::generateCHTLComment(const CommentNode& comment) const {
    // CHTL注释通常转换为HTML注释
    return comment.generateComment(CommentContext::HTML);
}

std::string CommentGenerator::generateCHTLJSComment(const CommentNode& comment) const {
    return comment.generateComment(CommentContext::CHTL_JS);
}

std::string CommentGenerator::generateCSSComment(const CommentNode& comment) const {
    return comment.generateComment(CommentContext::CSS);
}

std::string CommentGenerator::generateJSComment(const CommentNode& comment) const {
    return comment.generateComment(CommentContext::JAVASCRIPT);
}

std::string CommentGenerator::generateHTMLComment(const CommentNode& comment) const {
    return comment.generateComment(CommentContext::HTML);
}

std::string CommentGenerator::getIndentation(const std::string& line) const {
    size_t first_non_space = line.find_first_not_of(" \t");
    if (first_non_space == std::string::npos) {
        return line; // 整行都是空白
    }
    return line.substr(0, first_non_space);
}

std::string CommentGenerator::formatComment(const std::string& comment, 
                                           const std::string& indentation) const {
    if (!format_options_.preserve_indentation) {
        return comment;
    }
    
    std::vector<std::string> comment_lines = splitCodeIntoLines(comment);
    std::vector<std::string> formatted_lines;
    
    for (const std::string& line : comment_lines) {
        if (line.empty()) {
            formatted_lines.push_back("");
        } else {
            formatted_lines.push_back(indentation + line);
        }
    }
    
    return joinLines(formatted_lines);
}

bool CommentGenerator::shouldPreserveComment(const CommentNode& comment, 
                                            CodeBlockType block_type) const {
    // 生成器注释总是保留
    if (comment.getType() == CommentType::GENERATOR) {
        return true;
    }
    
    // HTML注释在HTML输出中保留
    if (comment.getType() == CommentType::HTML && block_type == CodeBlockType::HTML_OUTPUT) {
        return true;
    }
    
    // 其他注释根据上下文决定
    switch (block_type) {
        case CodeBlockType::CHTL_TEMPLATE:
            // CHTL模板中保留生成器注释和HTML注释
            return comment.getType() == CommentType::GENERATOR || 
                   comment.getType() == CommentType::HTML;
        
        case CodeBlockType::HTML_OUTPUT:
            // HTML输出中保留HTML注释和生成器注释
            return comment.getType() == CommentType::HTML || 
                   comment.getType() == CommentType::GENERATOR;
        
        case CodeBlockType::CHTL_STYLE:
        case CodeBlockType::PURE_CSS:
        case CodeBlockType::CHTL_SCRIPT:
        case CodeBlockType::PURE_JS:
        case CodeBlockType::MIXED:
        default:
            // 其他情况下保留所有注释
            return true;
    }
}

std::string CommentGenerator::detectIndentation(const std::string& code, 
                                               const CommentPosition& position) const {
    std::vector<std::string> lines = splitCodeIntoLines(code);
    
    if (position.line < lines.size()) {
        return getIndentation(lines[position.line]);
    }
    
    // 如果位置超出范围，尝试找到最近的非空行的缩进
    for (int i = static_cast<int>(position.line) - 1; i >= 0; --i) {
        if (i < static_cast<int>(lines.size()) && !lines[i].empty()) {
            std::string trimmed = lines[i];
            trimmed.erase(0, trimmed.find_first_not_of(" \t"));
            if (!trimmed.empty()) {
                return getIndentation(lines[i]);
            }
        }
    }
    
    return "";
}

std::vector<std::string> CommentGenerator::splitCodeIntoLines(const std::string& code) const {
    std::vector<std::string> lines;
    std::istringstream iss(code);
    std::string line;
    
    while (std::getline(iss, line)) {
        lines.push_back(line);
    }
    
    // 如果最后没有换行符，确保最后一行也被包含
    if (!code.empty() && code.back() != '\n') {
        // 已经通过getline处理了
    }
    
    return lines;
}

std::string CommentGenerator::joinLines(const std::vector<std::string>& lines) const {
    std::ostringstream oss;
    for (size_t i = 0; i < lines.size(); ++i) {
        oss << lines[i];
        if (i < lines.size() - 1) {
            oss << "\n";
        }
    }
    return oss.str();
}

bool CommentGenerator::containsCHTLSyntax(const std::string& code) const {
    // 检查CHTL特有语法（包括CHTL JS语法）
    return code.find("@") != std::string::npos ||
           code.find("[Template]") != std::string::npos ||
           code.find("[Custom]") != std::string::npos ||
           code.find("[Origin]") != std::string::npos ||
           code.find("[Import]") != std::string::npos ||
           code.find("text {") != std::string::npos ||
           code.find("@Html") != std::string::npos ||
           // 包含CHTL JS语法也算作CHTL语法
           containsCHTLJSSyntax(code);
}

bool CommentGenerator::containsCHTLJSSyntax(const std::string& code) const {
    // 检查CHTL JS特有语法
    return code.find("{{") != std::string::npos ||
           code.find("}}") != std::string::npos ||
           code.find("->") != std::string::npos ||
           code.find("listen") != std::string::npos ||
           code.find("delegate") != std::string::npos ||
           code.find("animate") != std::string::npos ||
           code.find("vir ") != std::string::npos;
}

bool CommentGenerator::containsCSSSyntax(const std::string& code) const {
    // 检查CSS语法（包括通用符号）
    std::regex css_patterns(R"((color|background|margin|padding|border|font|width|height|display|position)\s*:)");
    return std::regex_search(code, css_patterns) ||
           code.find("{") != std::string::npos ||
           code.find("}") != std::string::npos ||
           code.find("px") != std::string::npos ||
           code.find("em") != std::string::npos ||
           code.find("rem") != std::string::npos ||
           code.find("#") != std::string::npos ||
           code.find(".") != std::string::npos;
}

bool CommentGenerator::containsSpecificCSSSyntax(const std::string& code) const {
    // 检查CSS特有语法（不包括可能与CHTL冲突的通用符号）
    std::regex css_patterns(R"((color|background|margin|padding|border|font|width|height|display|position|float|clear|overflow|z-index|opacity|visibility)\s*:)");
    return std::regex_search(code, css_patterns) ||
           code.find("px") != std::string::npos ||
           code.find("em") != std::string::npos ||
           code.find("rem") != std::string::npos ||
           code.find("vh") != std::string::npos ||
           code.find("vw") != std::string::npos ||
           code.find("rgba") != std::string::npos ||
           code.find("rgb") != std::string::npos ||
           code.find("@media") != std::string::npos ||
           code.find("@keyframes") != std::string::npos ||
           code.find(":hover") != std::string::npos ||
           code.find(":active") != std::string::npos ||
           code.find(":focus") != std::string::npos;
}

bool CommentGenerator::containsJSSyntax(const std::string& code) const {
    // 检查JavaScript语法
    return code.find("function") != std::string::npos ||
           code.find("var ") != std::string::npos ||
           code.find("let ") != std::string::npos ||
           code.find("const ") != std::string::npos ||
           code.find("=>") != std::string::npos ||
           code.find("console.log") != std::string::npos ||
           code.find("document.") != std::string::npos ||
           code.find("window.") != std::string::npos;
}

bool CommentGenerator::containsHTMLSyntax(const std::string& code) const {
    // 检查HTML语法
    return code.find("<") != std::string::npos ||
           code.find(">") != std::string::npos ||
           code.find("<!DOCTYPE") != std::string::npos ||
           code.find("<html") != std::string::npos ||
           code.find("<head") != std::string::npos ||
           code.find("<body") != std::string::npos ||
           code.find("<div") != std::string::npos ||
           code.find("<span") != std::string::npos;
}

std::string CommentGenerator::wrapLongComment(const std::string& comment, 
                                             size_t max_length,
                                             const std::string& prefix) const {
    if (comment.length() <= max_length) {
        return comment;
    }
    
    std::vector<std::string> lines;
    std::istringstream iss(comment);
    std::string word;
    std::string current_line = prefix;
    
    while (iss >> word) {
        if (current_line.length() + word.length() + 1 > max_length) {
            if (!current_line.empty()) {
                lines.push_back(current_line);
                current_line = prefix + word;
            } else {
                current_line = prefix + word;
            }
        } else {
            if (current_line != prefix) {
                current_line += " ";
            }
            current_line += word;
        }
    }
    
    if (!current_line.empty()) {
        lines.push_back(current_line);
    }
    
    return joinLines(lines);
}

// CommentParser 实现
CommentParser::CommentParser() = default;
CommentParser::~CommentParser() = default;

std::vector<std::pair<std::unique_ptr<CommentNode>, CommentPosition>> 
CommentParser::parseCommentsFromCode(const std::string& code) const {
    
    std::vector<std::pair<std::unique_ptr<CommentNode>, CommentPosition>> result;
    
    // 首先检测原始嵌入块，这样我们可以跳过它们内部的内容
    auto origin_blocks = origin_detector_.detectOriginEmbeds(code);
    
    // 解析不同类型的注释（跳过原始嵌入内部）
    auto single_line = parseSingleLineComments(code);
    auto multi_line = parseMultiLineComments(code);
    auto html_comments = parseHTMLComments(code);
    auto generator_comments = parseGeneratorComments(code);
    
    // 过滤掉原始嵌入内部的注释
    auto filter_origin_embeds = [&origin_blocks](auto& comments) {
        comments.erase(
            std::remove_if(comments.begin(), comments.end(),
                [&origin_blocks](const auto& comment_pair) {
                    size_t comment_pos = comment_pair.second.start_offset;
                    for (const auto& block : origin_blocks) {
                        if (!block.is_reference && 
                            comment_pos >= block.content_start && 
                            comment_pos < block.content_end) {
                            return true; // 在原始嵌入内部，应该移除
                        }
                    }
                    return false;
                }),
            comments.end());
    };
    
    filter_origin_embeds(single_line);
    filter_origin_embeds(multi_line);
    filter_origin_embeds(html_comments);
    filter_origin_embeds(generator_comments);
    
    // 合并所有注释
    result.insert(result.end(), 
                  std::make_move_iterator(single_line.begin()), 
                  std::make_move_iterator(single_line.end()));
    result.insert(result.end(), 
                  std::make_move_iterator(multi_line.begin()), 
                  std::make_move_iterator(multi_line.end()));
    result.insert(result.end(), 
                  std::make_move_iterator(html_comments.begin()), 
                  std::make_move_iterator(html_comments.end()));
    result.insert(result.end(), 
                  std::make_move_iterator(generator_comments.begin()), 
                  std::make_move_iterator(generator_comments.end()));
    
    // 按位置排序
    std::sort(result.begin(), result.end(),
        [](const auto& a, const auto& b) {
            return a.second.start_offset < b.second.start_offset;
        });
    
    return result;
}

std::vector<std::unique_ptr<CommentNode>> 
CommentParser::extractComments(const std::string& code) const {
    
    auto positioned_comments = parseCommentsFromCode(code);
    std::vector<std::unique_ptr<CommentNode>> result;
    
    for (auto& pair : positioned_comments) {
        result.push_back(std::move(pair.first));
    }
    
    return result;
}

std::string CommentParser::removeComments(const std::string& code) const {
    std::string result = code;
    
    // 移除HTML注释
    std::regex html_comment(R"(<!--[\s\S]*?-->)");
    result = std::regex_replace(result, html_comment, "");
    
    // 移除多行注释
    std::regex multi_line_comment(R"(/\*[\s\S]*?\*/)");
    result = std::regex_replace(result, multi_line_comment, "");
    
    // 移除单行注释
    std::regex single_line_comment(R"(//.*?$)", std::regex::multiline);
    result = std::regex_replace(result, single_line_comment, "");
    
    // 移除生成器注释
    std::regex generator_comment(R"(--.*?$)", std::regex::multiline);
    result = std::regex_replace(result, generator_comment, "");
    
    return result;
}

std::string CommentParser::removeComments(const std::string& code, CommentType type) const {
    std::string result = code;
    
    switch (type) {
        case CommentType::SINGLE_LINE:
            {
                std::regex pattern(R"(//.*?$)", std::regex::multiline);
                result = std::regex_replace(result, pattern, "");
            }
            break;
            
        case CommentType::MULTI_LINE:
            {
                std::regex pattern(R"(/\*[\s\S]*?\*/)");
                result = std::regex_replace(result, pattern, "");
            }
            break;
            
        case CommentType::HTML:
            {
                std::regex pattern(R"(<!--[\s\S]*?-->)");
                result = std::regex_replace(result, pattern, "");
            }
            break;
            
        case CommentType::GENERATOR:
            {
                std::regex pattern(R"(--.*?$)", std::regex::multiline);
                result = std::regex_replace(result, pattern, "");
            }
            break;
            
        case CommentType::CSS:
            {
                std::regex pattern(R"(/\*[\s\S]*?\*/)");
                result = std::regex_replace(result, pattern, "");
            }
            break;
    }
    
    return result;
}

std::vector<std::pair<std::unique_ptr<CommentNode>, CommentPosition>>
CommentParser::parseSingleLineComments(const std::string& code) const {
    
    std::vector<std::pair<std::unique_ptr<CommentNode>, CommentPosition>> result;
    std::regex pattern(R"(//(.*?)$)", std::regex::multiline);
    std::sregex_iterator iter(code.begin(), code.end(), pattern);
    std::sregex_iterator end;
    
    for (; iter != end; ++iter) {
        const std::smatch& match = *iter;
        if (!isInsideString(code, match.position()) && 
            !isInsideOriginEmbed(code, match.position())) {
            std::string content = match[1].str();
            size_t start_pos = match.position();
            size_t end_pos = start_pos + match.length();
            
            auto comment = CommentFactory::createSingleLine(content);
            CommentPosition position = calculatePosition(code, start_pos, end_pos);
            
            result.emplace_back(std::move(comment), position);
        }
    }
    
    return result;
}

std::vector<std::pair<std::unique_ptr<CommentNode>, CommentPosition>>
CommentParser::parseMultiLineComments(const std::string& code) const {
    
    std::vector<std::pair<std::unique_ptr<CommentNode>, CommentPosition>> result;
    std::regex pattern(R"(/\*([\s\S]*?)\*/)");
    std::sregex_iterator iter(code.begin(), code.end(), pattern);
    std::sregex_iterator end;
    
    for (; iter != end; ++iter) {
        const std::smatch& match = *iter;
        if (!isInsideString(code, match.position()) && 
            !isInsideOriginEmbed(code, match.position())) {
            std::string content = match[1].str();
            size_t start_pos = match.position();
            size_t end_pos = start_pos + match.length();
            
            auto comment = CommentFactory::createMultiLine(content);
            CommentPosition position = calculatePosition(code, start_pos, end_pos);
            
            result.emplace_back(std::move(comment), position);
        }
    }
    
    return result;
}

std::vector<std::pair<std::unique_ptr<CommentNode>, CommentPosition>>
CommentParser::parseHTMLComments(const std::string& code) const {
    
    std::vector<std::pair<std::unique_ptr<CommentNode>, CommentPosition>> result;
    std::regex pattern(R"(<!--([\s\S]*?)-->)");
    std::sregex_iterator iter(code.begin(), code.end(), pattern);
    std::sregex_iterator end;
    
    for (; iter != end; ++iter) {
        const std::smatch& match = *iter;
        std::string content = match[1].str();
        size_t start_pos = match.position();
        size_t end_pos = start_pos + match.length();
        
        auto comment = CommentFactory::createHTML(content);
        CommentPosition position = calculatePosition(code, start_pos, end_pos);
        
        result.emplace_back(std::move(comment), position);
    }
    
    return result;
}

std::vector<std::pair<std::unique_ptr<CommentNode>, CommentPosition>>
CommentParser::parseGeneratorComments(const std::string& code) const {
    
    std::vector<std::pair<std::unique_ptr<CommentNode>, CommentPosition>> result;
    std::regex pattern(R"(--(.*?)$)", std::regex::multiline);
    std::sregex_iterator iter(code.begin(), code.end(), pattern);
    std::sregex_iterator end;
    
    for (; iter != end; ++iter) {
        const std::smatch& match = *iter;
        if (!isInsideString(code, match.position()) && 
            !isInsideOriginEmbed(code, match.position())) {
            std::string content = match[1].str();
            size_t start_pos = match.position();
            size_t end_pos = start_pos + match.length();
            
            auto comment = CommentFactory::createGenerator(content);
            CommentPosition position = calculatePosition(code, start_pos, end_pos);
            
            result.emplace_back(std::move(comment), position);
        }
    }
    
    return result;
}

bool CommentParser::isInsideString(const std::string& code, size_t position) const {
    // 简化实现：检查位置前的引号数量
    size_t single_quotes = 0;
    size_t double_quotes = 0;
    
    for (size_t i = 0; i < position && i < code.length(); ++i) {
        if (code[i] == '\'' && !isEscaped(code, i)) {
            single_quotes++;
        } else if (code[i] == '"' && !isEscaped(code, i)) {
            double_quotes++;
        }
    }
    
    // 如果引号数量为奇数，说明在字符串内部
    return (single_quotes % 2 == 1) || (double_quotes % 2 == 1);
}

bool CommentParser::isInsideOriginEmbed(const std::string& code, size_t position) const {
    return origin_detector_.isInsideOriginEmbed(code, position);
}

bool CommentParser::isInsideComment(const std::string& code, size_t position) const {
    // 检查是否在注释内部
    // 这是一个简化实现，实际可能需要更复杂的逻辑
    
    // 检查单行注释
    size_t line_start = code.rfind('\n', position);
    if (line_start == std::string::npos) line_start = 0;
    size_t comment_pos = code.find("//", line_start);
    if (comment_pos != std::string::npos && comment_pos < position) {
        return true;
    }
    
    // 检查多行注释
    size_t multi_start = code.rfind("/*", position);
    size_t multi_end = code.rfind("*/", position);
    if (multi_start != std::string::npos && 
        (multi_end == std::string::npos || multi_start > multi_end)) {
        return true;
    }
    
    return false;
}

CommentPosition CommentParser::calculatePosition(const std::string& code, 
                                                 size_t start_offset, 
                                                 size_t end_offset) const {
    size_t line = 1;
    size_t column = 1;
    
    for (size_t i = 0; i < start_offset && i < code.length(); ++i) {
        if (code[i] == '\n') {
            line++;
            column = 1;
        } else {
            column++;
        }
    }
    
    return CommentPosition(line - 1, column - 1, start_offset, end_offset);
}

bool CommentParser::isEscaped(const std::string& code, size_t pos) const {
    if (pos == 0) return false;
    
    size_t backslash_count = 0;
    for (int i = static_cast<int>(pos) - 1; i >= 0 && code[i] == '\\'; --i) {
        backslash_count++;
    }
    
    return backslash_count % 2 == 1;
}

// CommentIntegrator 实现
CommentIntegrator::CommentIntegrator() {
    // 设置默认策略
    strategy_.preserve_generator_comments = true;
    strategy_.preserve_documentation_comments = true;
    strategy_.remove_debug_comments = false;
    strategy_.convert_comments_to_target_format = true;
}

CommentIntegrator::~CommentIntegrator() = default;

void CommentIntegrator::integrateWithCHTLCompiler(
    const std::string& chtl_code,
    std::vector<std::unique_ptr<CommentNode>>& comments) const {
    
    // 从CHTL代码中提取注释
    auto extracted_comments = parser_.extractComments(chtl_code);
    
    // 合并注释
    for (auto& comment : extracted_comments) {
        comments.push_back(std::move(comment));
    }
    
    // 应用策略
    filterCommentsByStrategy(comments);
    convertCommentsToTargetContext(comments, CommentContext::CHTL);
}

void CommentIntegrator::integrateWithCHTLJSCompiler(
    const std::string& chtl_js_code,
    std::vector<std::unique_ptr<CommentNode>>& comments) const {
    
    auto extracted_comments = parser_.extractComments(chtl_js_code);
    
    for (auto& comment : extracted_comments) {
        comments.push_back(std::move(comment));
    }
    
    filterCommentsByStrategy(comments);
    convertCommentsToTargetContext(comments, CommentContext::CHTL_JS);
}

void CommentIntegrator::integrateWithCSSCompiler(
    const std::string& css_code,
    std::vector<std::unique_ptr<CommentNode>>& comments) const {
    
    auto extracted_comments = parser_.extractComments(css_code);
    
    for (auto& comment : extracted_comments) {
        comments.push_back(std::move(comment));
    }
    
    filterCommentsByStrategy(comments);
    convertCommentsToTargetContext(comments, CommentContext::CSS);
}

void CommentIntegrator::integrateWithJSCompiler(
    const std::string& js_code,
    std::vector<std::unique_ptr<CommentNode>>& comments) const {
    
    auto extracted_comments = parser_.extractComments(js_code);
    
    for (auto& comment : extracted_comments) {
        comments.push_back(std::move(comment));
    }
    
    filterCommentsByStrategy(comments);
    convertCommentsToTargetContext(comments, CommentContext::JAVASCRIPT);
}

std::string CommentIntegrator::generateFinalOutput(
    const std::string& base_code,
    const std::vector<std::unique_ptr<CommentNode>>& comments,
    CodeBlockType output_type) const {
    
    return generator_.processCodeWithComments(base_code, comments);
}

void CommentIntegrator::filterCommentsByStrategy(
    std::vector<std::unique_ptr<CommentNode>>& comments) const {
    
    comments.erase(
        std::remove_if(comments.begin(), comments.end(),
            [this](const std::unique_ptr<CommentNode>& comment) {
                CommentType type = comment->getType();
                
                // 检查类型过滤器
                auto it = strategy_.type_filters.find(type);
                if (it != strategy_.type_filters.end() && !it->second) {
                    return true; // 移除此类型的注释
                }
                
                // 应用其他策略
                if (!strategy_.preserve_generator_comments && 
                    type == CommentType::GENERATOR) {
                    return true;
                }
                
                return false;
            }),
        comments.end());
}

void CommentIntegrator::convertCommentsToTargetContext(
    std::vector<std::unique_ptr<CommentNode>>& comments,
    CommentContext target_context) const {
    
    if (!strategy_.convert_comments_to_target_format) {
        return;
    }
    
    for (auto& comment : comments) {
        comment->setContext(target_context);
    }
}

} // namespace ast
} // namespace chtl