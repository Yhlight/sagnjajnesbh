#pragma once
#include "../ast/ASTNode.h"
#include "../ast/CHTLNodes.h"
#include "../ast/SpecializedNodes.h"
#include "../common/Context.h"
#include <string>
#include <sstream>
#include <unordered_map>
#include <unordered_set>

namespace chtl {
namespace generator {

// HTML生成器配置
struct GeneratorConfig {
    bool pretty_print = true;           // 美化输出
    int indent_size = 2;                // 缩进大小
    bool generate_comments = true;      // 生成注释
    bool minify_css = false;           // 压缩CSS
    bool minify_js = false;            // 压缩JS
    std::string css_class_prefix = "";  // CSS类名前缀
    bool auto_generate_ids = true;      // 自动生成ID
};

// HTML生成器
class HTMLGenerator : public ast::ASTVisitor {
public:
    explicit HTMLGenerator(CompilerContext& context, const GeneratorConfig& config = GeneratorConfig());
    ~HTMLGenerator() = default;
    
    // 生成HTML
    std::string generate(const ast::ASTNode* root);
    std::string generateToFile(const ast::ASTNode* root, const std::string& output_path);
    
    // 访问者模式实现
    void visitProgram(ast::ProgramNode& node) override;
    void visitElement(ast::ElementNode& node) override;
    void visitText(ast::TextNode& node) override;
    void visitAttribute(ast::AttributeNode& node) override;
    void visitStyleBlock(ast::StyleBlockNode& node) override;
    void visitScriptBlock(ast::ScriptBlockNode& node) override;
    void visitTemplateDefinition(ast::TemplateDefinitionNode& node) override;
    void visitCustomDefinition(ast::CustomDefinitionNode& node) override;
    void visitOriginBlock(ast::OriginBlockNode& node) override;
    void visitImportStatement(ast::ImportStatementNode& node) override;
    void visitNamespaceDefinition(ast::NamespaceDefinitionNode& node) override;
    void visitCHTLJSSelector(ast::CHTLJSSelectorNode& node) override;
    void visitComment(ast::CommentNode& node) override;
    void visitLiteral(ast::LiteralNode& node) override;
    void visitIdentifier(ast::IdentifierNode& node) override;
    
    // 获取生成的CSS和JS
    std::string getGeneratedCSS() const;
    std::string getGeneratedJS() const;
    
    // 错误处理
    bool hasError() const;
    std::vector<std::string> getErrors() const;
    void clearErrors();
    
private:
    CompilerContext& context_;          // 编译上下文
    GeneratorConfig config_;            // 生成器配置
    std::ostringstream html_output_;    // HTML输出流
    std::ostringstream css_output_;     // CSS输出流
    std::ostringstream js_output_;      // JS输出流
    int current_indent_;                // 当前缩进级别
    std::vector<std::string> errors_;   // 错误列表
    
    // 自动生成的类名和ID管理
    std::unordered_set<std::string> generated_classes_;
    std::unordered_set<std::string> generated_ids_;
    std::unordered_map<std::string, std::string> class_mappings_;
    std::unordered_map<std::string, std::string> id_mappings_;
    
    // 模板和自定义实例管理
    std::unordered_map<std::string, const ast::TemplateDefinitionNode*> templates_;
    std::unordered_map<std::string, const ast::CustomDefinitionNode*> customs_;
    
    // CHTL JS处理
    std::unordered_map<std::string, std::string> chtl_js_selectors_;
    std::vector<std::string> chtl_js_functions_;
    
    // 输出辅助方法
    void writeIndent();
    void writeLine(const std::string& content = "");
    void write(const std::string& content);
    void increaseIndent();
    void decreaseIndent();
    
    // HTML生成辅助
    void generateElementStart(const std::string& tag_name, const std::vector<std::pair<std::string, std::string>>& attributes);
    void generateElementEnd(const std::string& tag_name);
    void generateSelfClosingElement(const std::string& tag_name, const std::vector<std::pair<std::string, std::string>>& attributes);
    
    // CSS生成辅助
    void generateCSS(const ast::ASTNode* style_node);
    void generateCSSSelector(const ast::CSSSelectorNode* selector_node);
    void generateCSSProperty(const ast::CSSPropertyNode* property_node);
    void processStyleInheritance(const ast::StyleInheritanceNode* inherit_node);
    void processStyleDelete(const ast::StyleDeleteNode* delete_node);
    
    // JS生成辅助
    void generateJS(const ast::ASTNode* script_node);
    void generateCHTLJSSelector(const ast::CHTLJSSelectorNode* selector_node);
    void generateCHTLJSListen(const ast::CHTLJSListenNode* listen_node);
    void generateCHTLJSDelegate(const ast::CHTLJSDelegateNode* delegate_node);
    void generateCHTLJSVir(const ast::CHTLJSVirNode* vir_node);
    
    // 模板处理
    void processTemplate(const ast::TemplateDefinitionNode* template_node);
    void expandTemplate(const std::string& template_name, const std::string& template_type);
    
    // 自定义处理
    void processCustom(const ast::CustomDefinitionNode* custom_node);
    void expandCustom(const std::string& custom_name, const std::string& custom_type);
    
    // 变量处理
    std::string resolveVariable(const ast::VariableReferenceNode* var_ref);
    void processVariableSpecialization(const ast::VariableSpecializationNode* var_spec);
    
    // 类名和ID管理
    std::string generateClassName(const std::string& base_name);
    std::string generateElementId(const std::string& base_name);
    std::string resolveCSSSelector(const std::string& selector);
    
    // 原始嵌入处理
    void processOriginBlock(const ast::OriginBlockNode* origin_node);
    
    // 约束处理
    void processExceptConstraint(const ast::ExceptConstraintNode* constraint_node);
    
    // 错误报告
    void reportError(const std::string& message);
    
    // 工具方法
    std::string escapeHTML(const std::string& text);
    std::string unquoteString(const std::string& quoted_string);
    bool isSelfClosingTag(const std::string& tag_name);
    std::string formatCSS(const std::string& css);
    std::string formatJS(const std::string& js);
    
    // HTML文档结构生成
    void generateDocumentHead();
    void generateDocumentBody();
    void generateInlineStyles();
    void generateInlineScripts();
};

} // namespace generator
} // namespace chtl