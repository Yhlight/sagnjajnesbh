#include "OriginEmbedDetector.h"
#include <regex>
#include <algorithm>
#include <sstream>

namespace chtl {
namespace ast {

OriginEmbedDetector::OriginEmbedDetector() 
    : allow_custom_types_(true), case_sensitive_(false) {
    // 初始化标准原始嵌入类型
    standard_types_ = {
        "@Html", "@Style", "@JavaScript", "@JS", "@CSS"
    };
}

OriginEmbedDetector::~OriginEmbedDetector() = default;

std::vector<OriginEmbedBlock> OriginEmbedDetector::detectOriginEmbeds(const std::string& code) const {
    std::vector<OriginEmbedBlock> blocks;
    
    // 解析原始嵌入定义（带大括号的）
    auto definitions = parseOriginDefinitions(code);
    blocks.insert(blocks.end(), definitions.begin(), definitions.end());
    
    // 解析原始嵌入引用（以分号结尾的）
    auto references = parseOriginReferences(code);
    blocks.insert(blocks.end(), references.begin(), references.end());
    
    // 按位置排序
    std::sort(blocks.begin(), blocks.end(), 
        [](const OriginEmbedBlock& a, const OriginEmbedBlock& b) {
            return a.start_pos < b.start_pos;
        });
    
    return blocks;
}

std::vector<OriginEmbedBlock> OriginEmbedDetector::parseOriginDefinitions(const std::string& code) const {
    std::vector<OriginEmbedBlock> blocks;
    
    // 查找 [Origin] 模式
    std::regex origin_pattern(R"(\[Origin\]\s*(@\w+)(?:\s+(\w+))?\s*\{)");
    std::sregex_iterator iter(code.begin(), code.end(), origin_pattern);
    std::sregex_iterator end;
    
    for (; iter != end; ++iter) {
        const std::smatch& match = *iter;
        
        OriginEmbedBlock block;
        block.start_pos = match.position();
        block.type = match[1].str();
        block.name = match[2].matched ? match[2].str() : "";
        block.is_reference = false;
        
        // 找到开始大括号的位置
        size_t brace_pos = match.position() + match.length() - 1;
        
        // 找到匹配的结束大括号
        size_t end_brace = findMatchingBrace(code, brace_pos);
        if (end_brace != std::string::npos) {
            block.end_pos = end_brace + 1;
            block.content_start = brace_pos + 1;
            block.content_end = end_brace;
            
            // 验证类型
            if (isValidOriginType(block.type)) {
                blocks.push_back(block);
            }
        }
    }
    
    return blocks;
}

std::vector<OriginEmbedBlock> OriginEmbedDetector::parseOriginReferences(const std::string& code) const {
    std::vector<OriginEmbedBlock> blocks;
    
    // 查找 [Origin] @Type [name]; 模式
    std::regex ref_pattern(R"(\[Origin\]\s*(@\w+)(?:\s+(\w+))?\s*;)");
    std::sregex_iterator iter(code.begin(), code.end(), ref_pattern);
    std::sregex_iterator end;
    
    for (; iter != end; ++iter) {
        const std::smatch& match = *iter;
        
        OriginEmbedBlock block;
        block.start_pos = match.position();
        block.end_pos = match.position() + match.length();
        block.type = match[1].str();
        block.name = match[2].matched ? match[2].str() : "";
        block.is_reference = true;
        block.content_start = block.end_pos;
        block.content_end = block.end_pos;
        
        // 验证类型
        if (isValidOriginType(block.type)) {
            blocks.push_back(block);
        }
    }
    
    return blocks;
}

bool OriginEmbedDetector::isInsideOriginEmbed(const std::string& code, size_t position) const {
    auto blocks = detectOriginEmbeds(code);
    return isInsideOriginEmbed(blocks, position);
}

bool OriginEmbedDetector::isInsideOriginEmbed(const std::vector<OriginEmbedBlock>& blocks, size_t position) const {
    for (const auto& block : blocks) {
        // 检查是否在原始嵌入内容区域内（不包括引用）
        if (!block.is_reference && 
            position >= block.content_start && 
            position < block.content_end) {
            return true;
        }
    }
    return false;
}

std::string OriginEmbedDetector::extractOriginContent(const std::string& code, const OriginEmbedBlock& block) const {
    if (block.is_reference || block.content_start >= block.content_end) {
        return "";
    }
    
    return code.substr(block.content_start, block.content_end - block.content_start);
}

std::string OriginEmbedDetector::removeOriginEmbeds(const std::string& code) const {
    auto blocks = detectOriginEmbeds(code);
    return removeOriginEmbeds(code, blocks);
}

std::string OriginEmbedDetector::removeOriginEmbeds(const std::string& code, const std::vector<OriginEmbedBlock>& blocks) const {
    if (blocks.empty()) {
        return code;
    }
    
    std::string result;
    size_t last_pos = 0;
    
    for (const auto& block : blocks) {
        // 添加块之前的内容
        if (block.start_pos > last_pos) {
            result += code.substr(last_pos, block.start_pos - last_pos);
        }
        
        // 跳过整个原始嵌入块
        last_pos = block.end_pos;
    }
    
    // 添加最后剩余的内容
    if (last_pos < code.length()) {
        result += code.substr(last_pos);
    }
    
    return result;
}

bool OriginEmbedDetector::isValidOriginType(const std::string& type) const {
    // 必须以@开头
    if (type.empty() || type[0] != '@') {
        return false;
    }
    
    // 检查是否是标准类型
    auto it = std::find_if(standard_types_.begin(), standard_types_.end(),
        [&](const std::string& std_type) {
            return case_sensitive_ ? (type == std_type) : 
                   (std::tolower(type[0]) == std::tolower(std_type[0]) &&
                    std::equal(type.begin() + 1, type.end(), std_type.begin() + 1,
                              [](char a, char b) { return std::tolower(a) == std::tolower(b); }));
        });
    
    if (it != standard_types_.end()) {
        return true;
    }
    
    // 如果允许自定义类型，检查格式是否有效
    if (allow_custom_types_) {
        // @后面必须是有效的标识符
        std::regex identifier_pattern(R"(@[a-zA-Z_][a-zA-Z0-9_]*)");
        return std::regex_match(type, identifier_pattern);
    }
    
    return false;
}

bool OriginEmbedDetector::isValidOriginName(const std::string& name) const {
    if (name.empty()) {
        return true; // 名称是可选的
    }
    
    // 检查是否是有效的标识符
    std::regex identifier_pattern(R"([a-zA-Z_][a-zA-Z0-9_]*)");
    return std::regex_match(name, identifier_pattern);
}

size_t OriginEmbedDetector::findMatchingBrace(const std::string& code, size_t start_pos) const {
    if (start_pos >= code.length() || code[start_pos] != '{') {
        return std::string::npos;
    }
    
    int brace_count = 1;
    size_t pos = start_pos + 1;
    bool in_string = false;
    char string_char = 0;
    bool escaped = false;
    
    while (pos < code.length() && brace_count > 0) {
        char c = code[pos];
        
        if (escaped) {
            escaped = false;
        } else if (c == '\\') {
            escaped = true;
        } else if (!in_string && (c == '"' || c == '\'')) {
            in_string = true;
            string_char = c;
        } else if (in_string && c == string_char) {
            in_string = false;
        } else if (!in_string) {
            if (c == '{') {
                brace_count++;
            } else if (c == '}') {
                brace_count--;
            }
        }
        
        pos++;
    }
    
    return (brace_count == 0) ? pos - 1 : std::string::npos;
}

size_t OriginEmbedDetector::skipWhitespace(const std::string& code, size_t pos) const {
    while (pos < code.length() && std::isspace(code[pos])) {
        pos++;
    }
    return pos;
}

size_t OriginEmbedDetector::skipWhitespaceReverse(const std::string& code, size_t pos) const {
    while (pos > 0 && std::isspace(code[pos - 1])) {
        pos--;
    }
    return pos;
}

std::string OriginEmbedDetector::debugOriginBlocks(const std::vector<OriginEmbedBlock>& blocks) const {
    std::ostringstream oss;
    oss << "OriginEmbed Blocks (" << blocks.size() << "):\n";
    
    for (size_t i = 0; i < blocks.size(); ++i) {
        const auto& block = blocks[i];
        oss << "  Block " << i << ":\n";
        oss << "    Type: " << block.type << "\n";
        oss << "    Name: " << (block.name.empty() ? "(none)" : block.name) << "\n";
        oss << "    Range: [" << block.start_pos << ", " << block.end_pos << ")\n";
        oss << "    Content: [" << block.content_start << ", " << block.content_end << ")\n";
        oss << "    Reference: " << (block.is_reference ? "true" : "false") << "\n";
    }
    
    return oss.str();
}

// OriginEmbedUtils 实现
namespace OriginEmbedUtils {

bool isStandardOriginType(const std::string& type) {
    static const std::vector<std::string> standard_types = {
        "@Html", "@Style", "@JavaScript", "@JS", "@CSS"
    };
    
    return std::find(standard_types.begin(), standard_types.end(), type) != standard_types.end();
}

std::vector<std::string> getStandardOriginTypes() {
    return {"@Html", "@Style", "@JavaScript", "@JS", "@CSS"};
}

bool isValidIdentifier(const std::string& name) {
    if (name.empty()) {
        return false;
    }
    
    if (!std::isalpha(name[0]) && name[0] != '_') {
        return false;
    }
    
    for (size_t i = 1; i < name.length(); ++i) {
        if (!std::isalnum(name[i]) && name[i] != '_') {
            return false;
        }
    }
    
    return true;
}

std::string normalizeOriginContent(const std::string& content) {
    std::string result = content;
    
    // 规范化换行符
    size_t pos = 0;
    while ((pos = result.find("\r\n", pos)) != std::string::npos) {
        result.replace(pos, 2, "\n");
        pos += 1;
    }
    
    return result;
}

std::string trimOriginContent(const std::string& content) {
    std::string result = content;
    
    // 移除前后空白
    result.erase(result.begin(), std::find_if(result.begin(), result.end(), [](unsigned char ch) {
        return !std::isspace(ch);
    }));
    result.erase(std::find_if(result.rbegin(), result.rend(), [](unsigned char ch) {
        return !std::isspace(ch);
    }).base(), result.end());
    
    return result;
}

std::pair<size_t, size_t> calculateContentBounds(const std::string& code, 
                                                 size_t origin_start, 
                                                 size_t origin_end) {
    // 找到 { 的位置
    size_t content_start = code.find('{', origin_start);
    if (content_start == std::string::npos || content_start >= origin_end) {
        return {origin_end, origin_end};
    }
    
    content_start++; // 跳过 {
    
    // 找到对应的 } 的位置
    size_t content_end = origin_end;
    if (content_end > 0 && code[content_end - 1] == '}') {
        content_end--;
    }
    
    return {content_start, content_end};
}

} // namespace OriginEmbedUtils

} // namespace ast
} // namespace chtl