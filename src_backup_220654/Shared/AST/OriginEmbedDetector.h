#pragma once
#include <string>
#include <vector>
#include <utility>

namespace chtl {
namespace ast {

// 原始嵌入块信息
struct OriginEmbedBlock {
    size_t start_pos;           // 开始位置（包含[Origin]）
    size_t end_pos;             // 结束位置（包含结束的}）
    size_t content_start;       // 内容开始位置（{之后）
    size_t content_end;         // 内容结束位置（}之前）
    std::string type;           // 类型（如@Html, @Style等）
    std::string name;           // 名称（可选）
    bool is_reference;          // 是否是引用（以;结尾）
};

// 原始嵌入检测器
class OriginEmbedDetector {
public:
    OriginEmbedDetector();
    ~OriginEmbedDetector();
    
    // 主要检测方法
    std::vector<OriginEmbedBlock> detectOriginEmbeds(const std::string& code) const;
    
    // 位置检查方法
    bool isInsideOriginEmbed(const std::string& code, size_t position) const;
    bool isInsideOriginEmbed(const std::vector<OriginEmbedBlock>& blocks, size_t position) const;
    
    // 工具方法
    std::string extractOriginContent(const std::string& code, const OriginEmbedBlock& block) const;
    std::string removeOriginEmbeds(const std::string& code) const;
    std::string removeOriginEmbeds(const std::string& code, const std::vector<OriginEmbedBlock>& blocks) const;
    
    // 验证方法
    bool isValidOriginType(const std::string& type) const;
    bool isValidOriginName(const std::string& name) const;
    
    // 调试方法
    std::string debugOriginBlocks(const std::vector<OriginEmbedBlock>& blocks) const;
    
private:
    // 内部解析方法
    std::vector<OriginEmbedBlock> parseOriginDefinitions(const std::string& code) const;
    std::vector<OriginEmbedBlock> parseOriginReferences(const std::string& code) const;
    
    // 辅助方法
    std::pair<std::string, std::string> parseOriginHeader(const std::string& header) const;
    size_t findMatchingBrace(const std::string& code, size_t start_pos) const;
    size_t skipWhitespace(const std::string& code, size_t pos) const;
    size_t skipWhitespaceReverse(const std::string& code, size_t pos) const;
    
    // 正则表达式模式
    bool matchesOriginPattern(const std::string& text, size_t pos) const;
    
    // 预定义的原始嵌入类型
    std::vector<std::string> standard_types_;
    
    // 配置选项
    bool allow_custom_types_;
    bool case_sensitive_;
};

// 原始嵌入工具函数
namespace OriginEmbedUtils {
    // 类型检查
    bool isStandardOriginType(const std::string& type);
    std::vector<std::string> getStandardOriginTypes();
    
    // 名称验证
    bool isValidIdentifier(const std::string& name);
    
    // 内容处理
    std::string normalizeOriginContent(const std::string& content);
    std::string trimOriginContent(const std::string& content);
    
    // 位置计算
    std::pair<size_t, size_t> calculateContentBounds(const std::string& code, 
                                                     size_t origin_start, 
                                                     size_t origin_end);
}

} // namespace ast
} // namespace chtl