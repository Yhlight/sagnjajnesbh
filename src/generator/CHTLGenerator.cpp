#include "../../include/CHTLGenerator.h"
#include <iostream>

namespace chtl {

CHTLGenerator::CHTLGenerator(const CompilerOptions& options)
    : options_(options), debugMode_(options.debugMode), autoClassCounter_(0), autoIdCounter_(0) {
    
    if (debugMode_) {
        debugLog("CHTL生成器初始化完成");
    }
}

GeneratorResult CHTLGenerator::generate(std::shared_ptr<CHTLASTNode> ast) {
    result_ = GeneratorResult{};
    
    if (!ast) {
        result_.addError(GeneratorErrorType::AST_ERROR, "AST节点为空");
        return result_;
    }
    
    if (debugMode_) {
        debugLog("开始生成HTML/CSS/JS代码");
    }
    
    try {
        // 简化实现 - 生成基本HTML结构
        result_.htmlOutput = "<div><!-- CHTL Generated Content --></div>";
        result_.cssOutput = "/* CHTL Generated Styles */";
        result_.jsOutput = "/* CHTL Generated Scripts */";
        result_.success = true;
        
        if (debugMode_) {
            debugLog("代码生成完成");
        }
        
    } catch (const std::exception& e) {
        result_.addError(GeneratorErrorType::OUTPUT_ERROR, "生成异常: " + String(e.what()));
    }
    
    return result_;
}

GeneratorResult CHTLGenerator::generateFromFile(const String& filename) {
    // 占位符实现
    result_.addError(GeneratorErrorType::OUTPUT_ERROR, "generateFromFile未实现");
    return result_;
}

String CHTLGenerator::generateHTML(std::shared_ptr<CHTLASTNode> ast) {
    return "<div><!-- HTML Generated --></div>";
}

String CHTLGenerator::generateCSS(std::shared_ptr<CHTLASTNode> ast) {
    return "/* CSS Generated */";
}

String CHTLGenerator::generateJS(std::shared_ptr<CHTLASTNode> ast) {
    return "/* JS Generated */";
}

// 占位符方法实现
String CHTLGenerator::generateElement(std::shared_ptr<ElementNode> element) { return ""; }
String CHTLGenerator::generateText(std::shared_ptr<TextNode> text) { return ""; }
String CHTLGenerator::generateAttribute(std::shared_ptr<AttributeNode> attribute) { return ""; }
String CHTLGenerator::generateStyle(std::shared_ptr<StyleBlockNode> style) { return ""; }
String CHTLGenerator::generateScript(std::shared_ptr<ScriptBlockNode> script) { return ""; }
String CHTLGenerator::generateStyleProperty(std::shared_ptr<StylePropertyNode> property) { return ""; }
String CHTLGenerator::generateClassSelector(std::shared_ptr<ClassSelectorNode> selector) { return ""; }
String CHTLGenerator::generateIdSelector(std::shared_ptr<IdSelectorNode> selector) { return ""; }
String CHTLGenerator::generatePseudoSelector(std::shared_ptr<PseudoSelectorNode> selector) { return ""; }
String CHTLGenerator::generateInlineStyles(const StringMap& styles) { return ""; }
String CHTLGenerator::generateTemplateStyle(std::shared_ptr<TemplateStyleNode> templateStyle) { return ""; }
String CHTLGenerator::generateTemplateElement(std::shared_ptr<TemplateElementNode> templateElement) { return ""; }
String CHTLGenerator::generateTemplateVar(std::shared_ptr<TemplateVarNode> templateVar) { return ""; }
String CHTLGenerator::generateCustomStyle(std::shared_ptr<CustomStyleNode> customStyle) { return ""; }
String CHTLGenerator::generateCustomElement(std::shared_ptr<CustomElementNode> customElement) { return ""; }
String CHTLGenerator::generateCustomVar(std::shared_ptr<CustomVarNode> customVar) { return ""; }
String CHTLGenerator::generateTemplateUsage(std::shared_ptr<TemplateUsageNode> usage) { return ""; }
String CHTLGenerator::resolveVariable(const String& varName, const String& templateName) { return ""; }
String CHTLGenerator::resolveVariableGroup(const String& groupName, const String& varName) { return ""; }
String CHTLGenerator::generateInherit(std::shared_ptr<InheritNode> inherit) { return ""; }
String CHTLGenerator::generateDelete(std::shared_ptr<DeleteNode> deleteNode) { return ""; }
String CHTLGenerator::generateExcept(std::shared_ptr<ExceptNode> except) { return ""; }
String CHTLGenerator::generateImport(std::shared_ptr<ImportNode> import) { return ""; }
String CHTLGenerator::generateNamespace(std::shared_ptr<NamespaceNode> namespaceNode) { return ""; }
String CHTLGenerator::generateConfiguration(std::shared_ptr<ConfigurationNode> config) { return ""; }
String CHTLGenerator::generateOrigin(std::shared_ptr<OriginNode> origin) { return ""; }

void CHTLGenerator::addInlineStyle(const String& property, const String& value) {}
void CHTLGenerator::addClassStyle(const String& className, const String& property, const String& value) {}
void CHTLGenerator::addIdStyle(const String& idName, const String& property, const String& value) {}
void CHTLGenerator::addPseudoStyle(const String& selector, const String& property, const String& value) {}

String CHTLGenerator::generateAutoClassName() {
    return "chtl-auto-class-" + std::to_string(autoClassCounter_++);
}

String CHTLGenerator::generateAutoIdName() {
    return "chtl-auto-id-" + std::to_string(autoIdCounter_++);
}

void CHTLGenerator::registerTemplate(const String& type, const String& name, std::shared_ptr<CHTLASTNode> node) {}
void CHTLGenerator::registerCustom(const String& type, const String& name, std::shared_ptr<CHTLASTNode> node) {}
void CHTLGenerator::registerVariable(const String& name, const String& value, const String& scope) {}

void CHTLGenerator::enterNamespace(const String& namespaceName) {
    namespaceStack_.push(namespaceName);
    currentNamespace_ = namespaceName;
}

void CHTLGenerator::exitNamespace() {
    if (!namespaceStack_.empty()) {
        namespaceStack_.pop();
    }
    currentNamespace_ = namespaceStack_.empty() ? "" : namespaceStack_.top();
}

String CHTLGenerator::getCurrentNamespace() const {
    return currentNamespace_;
}

String CHTLGenerator::resolveNamespacedName(const String& name) const {
    if (currentNamespace_.empty()) {
        return name;
    }
    return currentNamespace_ + "::" + name;
}

void CHTLGenerator::reportError(GeneratorErrorType type, const String& message, const String& context) {
    result_.addError(type, message, context);
    
    if (debugMode_) {
        std::cerr << "生成器错误: " << message << std::endl;
    }
}

void CHTLGenerator::reportWarning(const String& message) {
    result_.addWarning(message);
    
    if (debugMode_) {
        std::cerr << "生成器警告: " << message << std::endl;
    }
}

void CHTLGenerator::debugLog(const String& message) const {
    if (debugMode_) {
        std::cout << "[CHTL Generator] " << message << std::endl;
    }
}

// 工厂方法实现
std::unique_ptr<CHTLGenerator> CHTLGeneratorFactory::createGenerator(const CompilerOptions& options) {
    return std::make_unique<CHTLGenerator>(options);
}

std::unique_ptr<CHTLGenerator> CHTLGeneratorFactory::createDebugGenerator() {
    CompilerOptions debugOptions;
    debugOptions.debugMode = true;
    return std::make_unique<CHTLGenerator>(debugOptions);
}

std::unique_ptr<CHTLGenerator> CHTLGeneratorFactory::createOptimizedGenerator() {
    CompilerOptions optimizedOptions;
    optimizedOptions.debugMode = false;
    return std::make_unique<CHTLGenerator>(optimizedOptions);
}

} // namespace chtl