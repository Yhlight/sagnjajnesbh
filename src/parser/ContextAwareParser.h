#pragma once
#include "Parser.h"
#include "../common/ContextManager.h"
#include "../ast/NodeStateTracker.h"
#include <memory>
#include <unordered_map>
#include <functional>

namespace chtl {
namespace parser {

// 解析策略类型
enum class ParseStrategy {
    STRICT,             // 严格模式，严格按照CHTL语法规范
    TOLERANT,           // 容错模式，尽可能解析
    RECOVERY,           // 恢复模式，错误恢复后继续解析
    TEMPLATE_AWARE,     // 模板感知模式
    CUSTOM_AWARE,       // 自定义感知模式
    IMPORT_AWARE,       // 导入感知模式
    NAMESPACE_AWARE     // 命名空间感知模式
};

// 解析上下文信息
struct ParseContext {
    ContextScope current_scope;
    CompilationPhase current_phase;
    ParseStrategy strategy;
    bool in_template;
    bool in_custom;
    bool in_namespace;
    std::string current_namespace;
    std::string current_template;
    std::string current_custom;
    std::unordered_map<std::string, std::string> context_metadata;
    
    ParseContext()
        : current_scope(ContextScope::GLOBAL),
          current_phase(CompilationPhase::SYNTAX_PARSING),
          strategy(ParseStrategy::STRICT),
          in_template(false), in_custom(false), in_namespace(false) {}
};

// 解析决策
struct ParseDecision {
    bool should_parse;
    ParseStrategy recommended_strategy;
    std::string reason;
    std::vector<std::string> alternatives;
    
    ParseDecision(bool parse = true, ParseStrategy strategy = ParseStrategy::STRICT, 
                 const std::string& r = "")
        : should_parse(parse), recommended_strategy(strategy), reason(r) {}
};

// 上下文感知解析器
class ContextAwareParser : public CHTLParser {
public:
    explicit ContextAwareParser(ContextManager& context_manager);
    ~ContextAwareParser() = default;
    
    // 设置解析策略
    void setParseStrategy(ParseStrategy strategy);
    ParseStrategy getParseStrategy() const;
    
    // 上下文感知的解析方法（重写基类方法）
    std::unique_ptr<ast::ProgramNode> parseProgram() override;
    std::unique_ptr<ast::ElementNode> parseElement() override;
    std::unique_ptr<ast::TemplateDefinitionNode> parseTemplateDefinition() override;
    std::unique_ptr<ast::CustomDefinitionNode> parseCustomDefinition() override;
    std::unique_ptr<ast::ImportStatementNode> parseImportStatement() override;
    std::unique_ptr<ast::NamespaceDefinitionNode> parseNamespaceDefinition() override;
    std::unique_ptr<ast::StyleBlockNode> parseStyleBlock() override;
    std::unique_ptr<ast::ScriptBlockNode> parseScriptBlock() override;
    
    // 上下文感知的特殊解析方法
    std::unique_ptr<ast::ASTNode> parseInTemplateContext();
    std::unique_ptr<ast::ASTNode> parseInCustomContext();
    std::unique_ptr<ast::ASTNode> parseInNamespaceContext();
    std::unique_ptr<ast::ASTNode> parseInStyleContext();
    std::unique_ptr<ast::ASTNode> parseInScriptContext();
    
    // 解析决策方法
    ParseDecision shouldParseElement(const std::string& element_name) const;
    ParseDecision shouldParseTemplate(const std::string& template_name) const;
    ParseDecision shouldParseCustom(const std::string& custom_name) const;
    ParseDecision shouldParseImport(const std::string& import_path) const;
    ParseDecision shouldParseNamespace(const std::string& namespace_name) const;
    
    // 上下文验证
    bool validateInCurrentContext(ast::NodeType node_type) const;
    bool validateTemplateUsage(const std::string& template_name) const;
    bool validateCustomUsage(const std::string& custom_name) const;
    bool validateImportUsage(const std::string& import_path) const;
    
    // 错误恢复
    bool recoverFromError(ParseError error_type);
    void enterErrorRecoveryMode();
    void exitErrorRecoveryMode();
    bool isInErrorRecoveryMode() const;
    
    // 状态跟踪集成
    void setStateTracker(std::shared_ptr<ast::NodeStateTracker> tracker);
    ast::NodeStateTracker* getStateTracker() const;
    
    // 解析统计
    std::string getParsingStatistics() const;
    void printParsingReport() const;
    
private:
    ContextManager& context_manager_;
    std::shared_ptr<ast::NodeStateTracker> state_tracker_;
    ParseStrategy current_strategy_;
    bool error_recovery_mode_;
    
    // 解析上下文栈
    std::stack<ParseContext> context_stack_;
    
    // 解析统计
    struct ParsingStats {
        size_t elements_parsed;
        size_t templates_parsed;
        size_t customs_parsed;
        size_t imports_parsed;
        size_t namespaces_parsed;
        size_t errors_recovered;
        size_t context_switches;
        
        ParsingStats() : elements_parsed(0), templates_parsed(0), customs_parsed(0),
                        imports_parsed(0), namespaces_parsed(0), 
                        errors_recovered(0), context_switches(0) {}
    } parsing_stats_;
    
    // 内部辅助方法
    void pushParseContext(ContextScope scope, ParseStrategy strategy = ParseStrategy::STRICT);
    void popParseContext();
    ParseContext getCurrentParseContext() const;
    
    // 上下文感知的解析辅助方法
    bool isValidInTemplate(ast::NodeType node_type) const;
    bool isValidInCustom(ast::NodeType node_type) const;
    bool isValidInNamespace(ast::NodeType node_type) const;
    bool isValidInStyleBlock(ast::NodeType node_type) const;
    bool isValidInScriptBlock(ast::NodeType node_type) const;
    
    // CHTL语法规范验证
    bool validateCHTLTemplateDefinition(const std::string& name, const std::vector<std::string>& params) const;
    bool validateCHTLCustomDefinition(const std::string& name, const std::vector<std::string>& params) const;
    bool validateCHTLImportStatement(const std::string& type, const std::string& path) const;
    bool validateCHTLNamespaceDefinition(const std::string& name) const;
    bool validateCHTLElementUsage(const std::string& element_name) const;
    
    // 特殊语法处理
    bool handleCEEquivalence() const;  // 处理 ':' 与 '=' 的等价性
    bool handleUnquotedLiterals() const;
    bool handleTemplateInheritance(const std::string& parent_template) const;
    bool handleCustomSpecialization(const std::string& custom_name) const;
    
    // 错误处理和恢复
    void reportContextAwareError(ParseError error_type, const std::string& message);
    void reportContextAwareWarning(const std::string& message);
    bool attemptErrorRecovery(ParseError error_type);
    
    // 状态同步
    void syncWithContextManager();
    void updateNodeStates(ast::ASTNode* node);
    
    // 解析策略实现
    std::unique_ptr<ast::ASTNode> parseWithStrategy(ParseStrategy strategy, 
                                                   std::function<std::unique_ptr<ast::ASTNode>()> parse_func);
    
    // 模板和自定义解析的特殊处理
    std::unique_ptr<ast::ASTNode> parseTemplateStyleGroup();
    std::unique_ptr<ast::ASTNode> parseTemplateElementGroup();
    std::unique_ptr<ast::ASTNode> parseTemplateVariableGroup();
    std::unique_ptr<ast::ASTNode> parseCustomStyleGroup();
    std::unique_ptr<ast::ASTNode> parseCustomElementGroup();
    std::unique_ptr<ast::ASTNode> parseCustomVariableGroup();
    
    // 样式块特殊解析
    std::unique_ptr<ast::ASTNode> parseInlineStyle();
    std::unique_ptr<ast::ASTNode> parseClassSelector();
    std::unique_ptr<ast::ASTNode> parseIdSelector();
    std::unique_ptr<ast::ASTNode> parsePseudoSelector();
    std::unique_ptr<ast::ASTNode> parseContextualSelector(); // & 上下文推导
    
    // CHTL JS 特殊解析
    std::unique_ptr<ast::ASTNode> parseCHTLJSEnhancedSelector();
    std::unique_ptr<ast::ASTNode> parseCHTLJSListenFunction();
    std::unique_ptr<ast::ASTNode> parseCHTLJSDelegateFunction();
    std::unique_ptr<ast::ASTNode> parseCHTLJSAnimateFunction();
    std::unique_ptr<ast::ASTNode> parseCHTLJSVirObject();
    
    // 导入语句特殊处理
    bool validateImportType(const std::string& import_type) const;
    bool validateImportPath(const std::string& import_path) const;
    std::unique_ptr<ast::ASTNode> parseImportWithType(const std::string& import_type);
    
    // 命名空间处理
    bool enterNamespaceContext(const std::string& namespace_name);
    bool exitNamespaceContext();
    std::string resolveNamespacedIdentifier(const std::string& identifier) const;
};

// 解析器工厂扩展
class ContextAwareParserFactory {
public:
    // 创建标准上下文感知解析器
    static std::unique_ptr<ContextAwareParser> createStandardParser(ContextManager& context_manager);
    
    // 创建模板感知解析器
    static std::unique_ptr<ContextAwareParser> createTemplateAwareParser(ContextManager& context_manager);
    
    // 创建自定义感知解析器
    static std::unique_ptr<ContextAwareParser> createCustomAwareParser(ContextManager& context_manager);
    
    // 创建容错解析器
    static std::unique_ptr<ContextAwareParser> createTolerantParser(ContextManager& context_manager);
    
    // 创建调试解析器
    static std::unique_ptr<ContextAwareParser> createDebugParser(ContextManager& context_manager);
};

// 便利宏定义
#define CHTL_PARSE_WITH_CONTEXT(parser, scope, strategy) \
    do { \
        (parser).pushParseContext(scope, strategy); \
        auto context_guard = std::unique_ptr<void, std::function<void(void*)>>( \
            nullptr, [&](void*) { (parser).popParseContext(); }); \
    } while(0)

#define CHTL_VALIDATE_PARSE_CONTEXT(parser, node_type) \
    ((parser).validateInCurrentContext(node_type))

#define CHTL_PARSE_WITH_TRACKING(parser, node) \
    do { \
        if (auto tracker = (parser).getStateTracker()) { \
            CHTL_SNAPSHOT_NODE(*tracker, node); \
        } \
    } while(0)

} // namespace parser
} // namespace chtl