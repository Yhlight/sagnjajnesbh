#pragma once
#include <string>
#include <memory>
#include <vector>

namespace chtl {
namespace ast {

// 注释类型枚举
enum class CommentType {
    SINGLE_LINE,        // // 单行注释
    MULTI_LINE,         // /* */ 多行注释
    GENERATOR,          // -- 生成器注释
    HTML,               // <!-- --> HTML注释
    CSS                 // /* */ CSS注释
};

// 注释上下文枚举
enum class CommentContext {
    CHTL,               // CHTL上下文
    CHTL_JS,            // CHTL JS上下文
    CSS,                // CSS上下文
    JAVASCRIPT,         // JavaScript上下文
    HTML,               // HTML上下文
    UNKNOWN             // 未知上下文
};

// 注释节点类
class CommentNode {
public:
    CommentNode(const std::string& content, CommentType type, CommentContext context);
    CommentNode(const std::string& content, CommentType type);
    ~CommentNode() = default;

    // 基本属性访问
    const std::string& getContent() const { return content_; }
    CommentType getType() const { return type_; }
    CommentContext getContext() const { return context_; }
    
    void setContent(const std::string& content) { content_ = content; }
    void setType(CommentType type) { type_ = type; }
    void setContext(CommentContext context) { context_ = context; }

    // 注释生成
    std::string generateComment() const;
    std::string generateComment(CommentContext target_context) const;
    
    // 上下文推断
    void inferContext(const std::string& surrounding_code);
    static CommentContext inferContextFromCode(const std::string& code);
    
    // 注释转换
    std::string convertToContext(CommentContext target_context) const;
    bool canConvertToContext(CommentContext target_context) const;
    
    // 工具方法
    bool isMultiLine() const;
    bool shouldAppearInOutput() const;
    std::string getFormattedContent() const;
    
    // 调试信息
    std::string toString() const;
    std::string getTypeString() const;
    std::string getContextString() const;

private:
    std::string content_;           // 注释内容
    CommentType type_;             // 注释类型
    CommentContext context_;       // 注释上下文
    
    // 内部生成方法
    std::string generateSingleLineComment() const;
    std::string generateMultiLineComment() const;
    std::string generateGeneratorComment() const;
    std::string generateHTMLComment() const;
    std::string generateCSSComment() const;
    
    // 内容处理
    std::string cleanContent() const;
    std::string escapeContent(CommentContext target_context) const;
};

// 注释工厂类
class CommentFactory {
public:
    // 从原始文本创建注释
    static std::unique_ptr<CommentNode> createFromText(const std::string& text, 
                                                      CommentContext context = CommentContext::UNKNOWN);
    
    // 从特定格式创建注释
    static std::unique_ptr<CommentNode> createSingleLine(const std::string& content, 
                                                        CommentContext context = CommentContext::CHTL_JS);
    static std::unique_ptr<CommentNode> createMultiLine(const std::string& content, 
                                                       CommentContext context = CommentContext::CHTL_JS);
    static std::unique_ptr<CommentNode> createGenerator(const std::string& content);
    static std::unique_ptr<CommentNode> createHTML(const std::string& content);
    static std::unique_ptr<CommentNode> createCSS(const std::string& content);
    
    // 解析现有注释
    static std::unique_ptr<CommentNode> parseComment(const std::string& comment_text);
    
public:
    static CommentType detectCommentType(const std::string& text);
    static std::string extractCommentContent(const std::string& text, CommentType type);
};

// 注释管理器
class CommentManager {
public:
    CommentManager();
    ~CommentManager();
    
    // 注释注册和管理
    void addComment(std::unique_ptr<CommentNode> comment);
    void addComment(const std::string& content, CommentType type, CommentContext context);
    
    // 上下文切换
    void setGlobalContext(CommentContext context);
    CommentContext getGlobalContext() const { return global_context_; }
    
    // 批量处理
    std::vector<std::string> generateAllComments() const;
    std::vector<std::string> generateCommentsForContext(CommentContext context) const;
    
    // 过滤和查询
    std::vector<CommentNode*> getCommentsByType(CommentType type) const;
    std::vector<CommentNode*> getCommentsByContext(CommentContext context) const;
    std::vector<CommentNode*> getOutputComments() const; // 只返回应该出现在输出中的注释
    
    // 清理
    void clear();
    void removeCommentsByType(CommentType type);
    void removeCommentsByContext(CommentContext context);
    
    // 统计信息
    size_t getCommentCount() const { return comments_.size(); }
    std::string getStatistics() const;

private:
    std::vector<std::unique_ptr<CommentNode>> comments_;
    CommentContext global_context_;
};

// 注释工具函数
namespace CommentUtils {
    // 类型转换
    std::string commentTypeToString(CommentType type);
    std::string commentContextToString(CommentContext context);
    CommentType stringToCommentType(const std::string& type_str);
    CommentContext stringToCommentContext(const std::string& context_str);
    
    // 格式检测
    bool isValidCommentFormat(const std::string& text, CommentType type);
    bool isCommentLine(const std::string& line);
    CommentType detectCommentTypeFromLine(const std::string& line);
    
    // 内容处理
    std::string trimCommentWhitespace(const std::string& content);
    std::string normalizeCommentContent(const std::string& content);
    std::vector<std::string> splitMultiLineComment(const std::string& content);
    
    // 上下文推断
    CommentContext inferContextFromFileExtension(const std::string& filename);
    CommentContext inferContextFromSurroundingCode(const std::string& code);
}

} // namespace ast
} // namespace chtl