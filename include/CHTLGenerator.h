#pragma once

#include "CHTLCommon.h"
#include "CHTLASTNodes.h"
#include <memory>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <sstream>

namespace chtl {

// 生成器错误类型
enum class GeneratorErrorType {
    AST_ERROR,              // AST节点错误
    TEMPLATE_ERROR,         // 模板处理错误
    VARIABLE_ERROR,         // 变量处理错误
    STYLE_ERROR,            // 样式处理错误
    NAMESPACE_ERROR,        // 命名空间错误
    OUTPUT_ERROR            // 输出错误
};

// 生成器错误信息
struct GeneratorError {
    GeneratorErrorType type;
    String message;
    String context;
    size_t line;
    size_t column;
    
    GeneratorError(GeneratorErrorType t, const String& msg, const String& ctx = "", size_t l = 0, size_t c = 0)
        : type(t), message(msg), context(ctx), line(l), column(c) {}
};

// 生成结果
struct GeneratorResult {
    bool success = false;
    String htmlOutput;
    String cssOutput;
    String jsOutput;
    std::vector<GeneratorError> errors;
    std::vector<String> warnings;
    
    void addError(GeneratorErrorType type, const String& message, const String& context = "", size_t line = 0, size_t column = 0) {
        errors.emplace_back(type, message, context, line, column);
        success = false;
    }
    
    void addWarning(const String& message) {
        warnings.push_back(message);
    }
};

// 样式上下文
struct StyleContext {
    StringMap inlineStyles;     // 内联样式
    StringMap classStyles;      // 类样式
    StringMap idStyles;         // ID样式
    StringMap pseudoStyles;     // 伪类样式
    StringSet autoClasses;      // 自动生成的类名
    StringSet autoIds;          // 自动生成的ID
    String currentSelector;     // 当前选择器
    bool inLocalStyle = false;  // 是否在局部样式块中
};

// 模板上下文
struct TemplateContext {
    std::unordered_map<String, std::shared_ptr<TemplateStyleNode>> styleTemplates;
    std::unordered_map<String, std::shared_ptr<TemplateElementNode>> elementTemplates;
    std::unordered_map<String, std::shared_ptr<TemplateVarNode>> varTemplates;
    std::unordered_map<String, std::shared_ptr<CustomStyleNode>> customStyles;
    std::unordered_map<String, std::shared_ptr<CustomElementNode>> customElements;
    std::unordered_map<String, std::shared_ptr<CustomVarNode>> customVars;
    StringMap variables;        // 变量值映射
    StringSet namespaces;       // 命名空间
};

// CHTL生成器主类
class CHTLGenerator {
public:
    explicit CHTLGenerator(const CompilerOptions& options = CompilerOptions{});
    ~CHTLGenerator() = default;
    
    // 主要生成接口
    GeneratorResult generate(std::shared_ptr<CHTLASTNode> ast);
    GeneratorResult generateFromFile(const String& filename);
    
    // 分段生成接口
    String generateHTML(std::shared_ptr<CHTLASTNode> ast);
    String generateCSS(std::shared_ptr<CHTLASTNode> ast);
    String generateJS(std::shared_ptr<CHTLASTNode> ast);
    
    // 节点生成方法
    String generateElement(std::shared_ptr<ElementNode> element);
    String generateText(std::shared_ptr<TextNode> text);
    String generateAttribute(std::shared_ptr<AttributeNode> attribute);
    String generateStyle(std::shared_ptr<StyleBlockNode> style);
    String generateScript(std::shared_ptr<ScriptBlockNode> script);
    
    // 样式相关生成
    String generateStyleProperty(std::shared_ptr<CSSPropertyNode> property);
    String generateClassSelector(const String& className);
    String generateIdSelector(const String& idName);
    String generatePseudoSelector(const String& pseudoSelector);
    String generateInlineStyles(const StringMap& styles);
    
    // 模板和自定义生成
    String generateTemplateStyle(std::shared_ptr<TemplateStyleNode> templateStyle);
    String generateTemplateElement(std::shared_ptr<TemplateElementNode> templateElement);
    String generateTemplateVar(std::shared_ptr<TemplateVarNode> templateVar);
    String generateCustomStyle(std::shared_ptr<CustomStyleNode> customStyle);
    String generateCustomElement(std::shared_ptr<CustomElementNode> customElement);
    String generateCustomVar(std::shared_ptr<CustomVarNode> customVar);
    
    // 模板使用和变量解析
    String generateTemplateUsage(const String& templateName, const String& templateType);
    String resolveVariable(const String& varName, const String& templateName = "");
    String resolveVariableGroup(const String& groupName, const String& varName);
    
    // 特殊语法生成
    String generateInherit(const String& inheritTarget);
    String generateDelete(const String& deleteTarget);
    String generateExcept(const StringList& exceptList);
    
    // 导入和命名空间
    String generateImport(std::shared_ptr<ImportNode> import);
    String generateNamespace(std::shared_ptr<NamespaceNode> namespaceNode);
    String generateConfiguration(const StringMap& configMap);
    String generateOrigin(std::shared_ptr<OriginNode> origin);
    
    // 工具方法
    void setDebugMode(bool debug) { debugMode_ = debug; }
    bool isDebugMode() const { return debugMode_; }
    
    // 样式管理
    void addInlineStyle(const String& property, const String& value);
    void addClassStyle(const String& className, const String& property, const String& value);
    void addIdStyle(const String& idName, const String& property, const String& value);
    void addPseudoStyle(const String& selector, const String& property, const String& value);
    String generateAutoClassName();
    String generateAutoIdName();
    
    // 模板管理
    void registerTemplate(const String& type, const String& name, std::shared_ptr<CHTLASTNode> node);
    void registerCustom(const String& type, const String& name, std::shared_ptr<CHTLASTNode> node);
    void registerVariable(const String& name, const String& value, const String& scope = "");
    
    // 命名空间管理
    void enterNamespace(const String& namespaceName);
    void exitNamespace();
    String getCurrentNamespace() const;
    String resolveNamespacedName(const String& name) const;
    
    // 错误处理
    void reportError(GeneratorErrorType type, const String& message, const String& context = "");
    void reportWarning(const String& message);

private:
    CompilerOptions options_;
    bool debugMode_;
    GeneratorResult result_;
    
    // 生成状态
    std::ostringstream htmlStream_;
    std::ostringstream cssStream_;
    std::ostringstream jsStream_;
    
    // 上下文管理
    StyleContext styleContext_;
    TemplateContext templateContext_;
    std::stack<String> namespaceStack_;
    String currentNamespace_;
    
    // 自动生成计数器
    size_t autoClassCounter_;
    size_t autoIdCounter_;
    
    // 样式收集
    StringMap globalStyles_;        // 全局样式
    StringMap localStyles_;         // 局部样式
    StringSet processedSelectors_;  // 已处理的选择器
    
    // 新增：全局CSS和JS管理
    std::unordered_map<String, StringMap> globalCSS_;  // 全局CSS规则
    String globalJS_;                                   // 全局JS代码
    
    // 内部生成方法
    void generateNode(std::shared_ptr<CHTLASTNode> node);
    void processChildren(std::shared_ptr<CHTLASTNode> parent);
    
    // HTML生成辅助
    String generateElementTag(const String& tagName, const StringMap& attributes, bool selfClosing = false);
    String generateElementContent(std::shared_ptr<ElementNode> element);
    String escapeHtml(const String& text) const;
    String generateAttributeString(const StringMap& attributes) const;
    
    // CSS生成辅助
    String generateCSSRule(const String& selector, const StringMap& properties);
    String generateCSSProperty(const String& property, const String& value);
    String optimizeCSS(const String& css) const;
    String mergeCSSRules(const StringMap& rules) const;
    
    // 模板处理辅助
    void collectTemplates(std::shared_ptr<CHTLASTNode> node);
    void collectCustoms(std::shared_ptr<CHTLASTNode> node);
    void collectVariables(std::shared_ptr<CHTLASTNode> node);
    void processTemplateInheritance();
    void processCustomSpecialization();
    
    // 变量替换
    String expandVariables(const String& text) const;
    
    // 样式处理
    void processLocalStyles(std::shared_ptr<StyleBlockNode> styleBlock, const String& elementContext);
    void processGlobalStyles();
    void generateAutoClasses(std::shared_ptr<ElementNode> element);
    void generateAutoIds(std::shared_ptr<ElementNode> element);
    
    // 约束处理
    void processExceptConstraints(const StringList& exceptList);
    bool isElementAllowed(const String& elementName) const;
    bool isTemplateAllowed(const String& templateName) const;
    
    // 输出格式化
    String formatHTML(const String& html) const;
    String formatCSS(const String& css) const;
    String formatJS(const String& js) const;
    
    // 调试和日志
    void debugLog(const String& message) const;
    void logGenerationStep(const String& step, const String& details = "") const;
    
    // 新增核心生成方法
    void generateHTMLRecursive(std::shared_ptr<CHTLASTNode> node, std::ostringstream& stream);
    void generateElementHTML(std::shared_ptr<ElementNode> element, std::ostringstream& stream);
    void generateTextHTML(std::shared_ptr<TextNode> text, std::ostringstream& stream);
    
    // 样式和脚本处理
    void processStyleBlock(std::shared_ptr<StyleBlockNode> styleBlock);
    void processCSSProperty(std::shared_ptr<CSSPropertyNode> property);
    void processCSSRule(std::shared_ptr<CSSRuleNode> rule);
    void processScriptBlock(std::shared_ptr<ScriptBlockNode> scriptBlock);
    void processTemplateUsage(std::shared_ptr<TemplateUsageNode> usage);
    
    // 全局样式和脚本管理
    void addToGlobalCSS(const String& selector, const String& property, const String& value);
    void generateGlobalCSS(std::ostringstream& stream);
    void generateGlobalJS(std::ostringstream& stream);
    
    // CHTL JS代码生成
    String generateCHTLJSCode(std::shared_ptr<CHTLASTNode> node);
    
    // 模板管理方法
    void registerTemplateStyle(std::shared_ptr<TemplateStyleNode> templateStyle);
    void registerTemplateElement(std::shared_ptr<TemplateElementNode> templateElement);
    void registerTemplateVar(std::shared_ptr<TemplateVarNode> templateVar);
    
    // 变量替换方法
    String expandTemplateVariables(const String& text, const String& templateName = "") const;
    String resolveVariableReference(std::shared_ptr<VariableReferenceNode> varRef);
    
    // 工具方法
    String escapeHTML(const String& text);
    String escapeHTMLAttribute(const String& text);
    
    // 辅助工具
    bool isValidHtmlTag(const String& tagName) const;
    bool isSelfClosingTag(const String& tagName) const;
    String sanitizeClassName(const String& name) const;
    String sanitizeIdName(const String& name) const;
    String generateUniqueId() const;
};

// CHTL生成器工厂
class CHTLGeneratorFactory {
public:
    static std::unique_ptr<CHTLGenerator> createGenerator(const CompilerOptions& options = CompilerOptions{});
    static std::unique_ptr<CHTLGenerator> createDebugGenerator();
    static std::unique_ptr<CHTLGenerator> createOptimizedGenerator();
};

// 生成器辅助类
class StyleCollector {
public:
    StyleCollector();
    
    void addStyle(const String& selector, const String& property, const String& value);
    void addInlineStyle(const String& property, const String& value);
    void addClassStyle(const String& className, const String& property, const String& value);
    void addIdStyle(const String& idName, const String& property, const String& value);
    
    String generateCSS() const;
    StringMap getInlineStyles() const { return inlineStyles_; }
    
private:
    std::unordered_map<String, StringMap> selectorStyles_;
    StringMap inlineStyles_;
    
    String generateCSSRule(const String& selector, const StringMap& properties) const;
};

// 模板解析器辅助类
class TemplateResolver {
public:
    TemplateResolver(const TemplateContext& context);
    
    String resolveTemplate(const String& type, const String& name);
    String resolveVariable(const String& varName, const String& scope = "");
    String resolveInheritance(const String& templateName);
    
    void processSpecialization(const String& templateName, const StringMap& specializations);
    void processDelete(const String& templateName, const StringSet& deletions);
    
private:
    const TemplateContext& templateContext_;
    std::unordered_map<String, String> resolvedCache_;
    
    String expandTemplate(std::shared_ptr<CHTLASTNode> templateNode);
};

} // namespace chtl