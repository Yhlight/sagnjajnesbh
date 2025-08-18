#pragma once
#include "CommentNode.h"
#include "OriginEmbedDetector.h"
#include <string>
#include <vector>
#include <memory>
#include <unordered_map>

namespace chtl {
namespace ast {

// 代码块类型枚举
enum class CodeBlockType {
    CHTL_TEMPLATE,      // CHTL模板代码
    CHTL_STYLE,         // CHTL样式块
    CHTL_SCRIPT,        // CHTL脚本块
    PURE_CSS,           // 纯CSS代码
    PURE_JS,            // 纯JavaScript代码
    HTML_OUTPUT,        // HTML输出
    MIXED               // 混合代码
};

// 注释位置信息
struct CommentPosition {
    size_t line;
    size_t column;
    size_t start_offset;
    size_t end_offset;
    
    CommentPosition(size_t l = 0, size_t c = 0, size_t start = 0, size_t end = 0)
        : line(l), column(c), start_offset(start), end_offset(end) {}
};

// 上下文感知的注释生成器
class CommentGenerator {
public:
    CommentGenerator();
    ~CommentGenerator();
    
    // 主要生成方法
    std::string generateContextualComment(const CommentNode& comment, 
                                        const std::string& surrounding_code) const;
    std::string generateContextualComment(const CommentNode& comment, 
                                        CodeBlockType block_type) const;
    
    // 批量处理
    std::vector<std::string> generateCommentsForBlock(
        const std::vector<std::unique_ptr<CommentNode>>& comments,
        CodeBlockType block_type) const;
    
    std::string processCodeWithComments(const std::string& code,
                                       const std::vector<std::unique_ptr<CommentNode>>& comments) const;
    
    // 上下文推断
    CodeBlockType inferBlockType(const std::string& code) const;
    CommentContext mapBlockTypeToContext(CodeBlockType block_type) const;
    
    // 注释插入
    std::string insertCommentAtPosition(const std::string& code,
                                       const CommentNode& comment,
                                       const CommentPosition& position) const;
    
    std::string insertCommentsInCode(const std::string& code,
                                    const std::vector<std::pair<CommentNode*, CommentPosition>>& positioned_comments) const;
    
    // 格式化选项
    struct FormatOptions {
        bool preserve_indentation = true;
        bool add_blank_lines = false;
        bool align_comments = false;
        size_t max_line_length = 80;
        std::string indent_string = "    ";
    };
    
    void setFormatOptions(const FormatOptions& options) { format_options_ = options; }
    const FormatOptions& getFormatOptions() const { return format_options_; }
    
    // 特殊处理方法
    std::string generateCHTLComment(const CommentNode& comment) const;
    std::string generateCHTLJSComment(const CommentNode& comment) const;
    std::string generateCSSComment(const CommentNode& comment) const;
    std::string generateJSComment(const CommentNode& comment) const;
    std::string generateHTMLComment(const CommentNode& comment) const;
    
    // 工具方法
    std::string getIndentation(const std::string& line) const;
    std::string formatComment(const std::string& comment, 
                            const std::string& indentation) const;
    bool shouldPreserveComment(const CommentNode& comment, 
                              CodeBlockType block_type) const;
    
    // 上下文分析（调试用）
    bool containsCHTLSyntax(const std::string& code) const;
    bool containsCHTLJSSyntax(const std::string& code) const;
    bool containsCSSSyntax(const std::string& code) const;
    bool containsSpecificCSSSyntax(const std::string& code) const;
    bool containsJSSyntax(const std::string& code) const;
    bool containsHTMLSyntax(const std::string& code) const;
    
private:
    FormatOptions format_options_;
    
    // 内部辅助方法
    std::string detectIndentation(const std::string& code, 
                                 const CommentPosition& position) const;
    
    std::vector<std::string> splitCodeIntoLines(const std::string& code) const;
    std::string joinLines(const std::vector<std::string>& lines) const;
    

    
    // 格式化辅助
    std::string wrapLongComment(const std::string& comment, 
                               size_t max_length,
                               const std::string& prefix) const;
};

// 注释解析器 - 从代码中提取注释
class CommentParser {
public:
    CommentParser();
    ~CommentParser();
    
    // 解析方法
    std::vector<std::pair<std::unique_ptr<CommentNode>, CommentPosition>> 
        parseCommentsFromCode(const std::string& code) const;
    
    std::vector<std::unique_ptr<CommentNode>> 
        extractComments(const std::string& code) const;
    
    std::string removeComments(const std::string& code) const;
    std::string removeComments(const std::string& code, CommentType type) const;
    
    // 特定格式解析
    std::vector<std::pair<std::unique_ptr<CommentNode>, CommentPosition>>
        parseSingleLineComments(const std::string& code) const;
    
    std::vector<std::pair<std::unique_ptr<CommentNode>, CommentPosition>>
        parseMultiLineComments(const std::string& code) const;
    
    std::vector<std::pair<std::unique_ptr<CommentNode>, CommentPosition>>
        parseHTMLComments(const std::string& code) const;
    
    std::vector<std::pair<std::unique_ptr<CommentNode>, CommentPosition>>
        parseGeneratorComments(const std::string& code) const;
    
    // 工具方法
    bool isInsideString(const std::string& code, size_t position) const;
    bool isInsideComment(const std::string& code, size_t position) const;
    bool isInsideOriginEmbed(const std::string& code, size_t position) const;
    CommentPosition calculatePosition(const std::string& code, 
                                    size_t start_offset, 
                                    size_t end_offset) const;
    
private:
    OriginEmbedDetector origin_detector_;
    
    // 内部解析辅助方法
    std::pair<size_t, size_t> findCommentBoundaries(const std::string& code,
                                                    size_t start_pos,
                                                    CommentType type) const;
    
    bool isValidCommentStart(const std::string& code, size_t pos, CommentType type) const;
    bool isEscaped(const std::string& code, size_t pos) const;
};

// 注释集成器 - 将注释系统集成到编译器中
class CommentIntegrator {
public:
    CommentIntegrator();
    ~CommentIntegrator();
    
    // 编译器集成
    void integrateWithCHTLCompiler(const std::string& chtl_code,
                                  std::vector<std::unique_ptr<CommentNode>>& comments) const;
    
    void integrateWithCHTLJSCompiler(const std::string& chtl_js_code,
                                    std::vector<std::unique_ptr<CommentNode>>& comments) const;
    
    void integrateWithCSSCompiler(const std::string& css_code,
                                 std::vector<std::unique_ptr<CommentNode>>& comments) const;
    
    void integrateWithJSCompiler(const std::string& js_code,
                                std::vector<std::unique_ptr<CommentNode>>& comments) const;
    
    // 输出生成
    std::string generateFinalOutput(const std::string& base_code,
                                   const std::vector<std::unique_ptr<CommentNode>>& comments,
                                   CodeBlockType output_type) const;
    
    // 注释策略配置
    struct CommentStrategy {
        bool preserve_generator_comments = true;
        bool preserve_documentation_comments = true;
        bool remove_debug_comments = false;
        bool convert_comments_to_target_format = true;
        std::unordered_map<CommentType, bool> type_filters;
    };
    
    void setCommentStrategy(const CommentStrategy& strategy) { strategy_ = strategy; }
    const CommentStrategy& getCommentStrategy() const { return strategy_; }
    
private:
    CommentGenerator generator_;
    CommentParser parser_;
    CommentStrategy strategy_;
    
    // 内部处理方法
    void filterCommentsByStrategy(std::vector<std::unique_ptr<CommentNode>>& comments) const;
    void convertCommentsToTargetContext(std::vector<std::unique_ptr<CommentNode>>& comments,
                                       CommentContext target_context) const;
};

} // namespace ast
} // namespace chtl