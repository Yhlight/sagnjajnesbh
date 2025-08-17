#include "../../include/CHTLJSGenerator.h"
#include <iostream>

namespace chtl {

CHTLJSGenerator::CHTLJSGenerator(const CompilerOptions& options)
    : options_(options), debugMode_(options.debugMode), functionCounter_(0), 
      delegateCounter_(0), animationCounter_(0) {
    
    if (debugMode_) {
        debugLog("CHTL JS生成器初始化完成");
    }
}

CHTLJSGeneratorResult CHTLJSGenerator::generate(std::shared_ptr<CHTLJSASTNode> ast) {
    result_ = CHTLJSGeneratorResult{};
    
    if (!ast) {
        result_.addError(CHTLJSGeneratorErrorType::AST_ERROR, "AST节点为空");
        return result_;
    }
    
    if (debugMode_) {
        debugLog("开始生成JavaScript代码");
    }
    
    try {
        // 简化实现 - 生成基本JavaScript代码
        result_.jsOutput = "/* CHTL JS Generated Code */\n";
        result_.jsOutput += "console.log('CHTL JS Generated');\n";
        result_.success = true;
        
        if (debugMode_) {
            debugLog("JavaScript代码生成完成");
        }
        
    } catch (const std::exception& e) {
        result_.addError(CHTLJSGeneratorErrorType::OUTPUT_ERROR, "生成异常: " + String(e.what()));
    }
    
    return result_;
}

CHTLJSGeneratorResult CHTLJSGenerator::generateFromScriptBlock(const String& scriptContent, const String& filename) {
    // 占位符实现
    result_.addError(CHTLJSGeneratorErrorType::OUTPUT_ERROR, "generateFromScriptBlock未实现");
    return result_;
}

// 占位符方法实现
String CHTLJSGenerator::generateStatement(std::shared_ptr<CHTLJSASTNode> stmt) { return ""; }
String CHTLJSGenerator::generateExpression(std::shared_ptr<CHTLJSASTNode> expr) { return ""; }
String CHTLJSGenerator::generateEnhancedSelector(std::shared_ptr<EnhancedSelectorNode> selector) { return ""; }
String CHTLJSGenerator::generateSelectorQuery(const String& selectorContent, const String& selectorType, const String& index) { return ""; }
String CHTLJSGenerator::generateClassSelectorQuery(const String& className, const String& index) { return ""; }
String CHTLJSGenerator::generateIdSelectorQuery(const String& idName, const String& index) { return ""; }
String CHTLJSGenerator::generateTagSelectorQuery(const String& tagName, const String& index) { return ""; }
String CHTLJSGenerator::generateComplexSelectorQuery(const String& selector, const String& index) { return ""; }
String CHTLJSGenerator::generateAutoSelectorQuery(const String& selector, const String& index) { return ""; }
String CHTLJSGenerator::generateArrowExpression(const String& leftExpr, const String& rightExpr) { return ""; }
String CHTLJSGenerator::generateChainedCall(std::shared_ptr<CHTLJSASTNode> leftExpr, std::shared_ptr<CHTLJSASTNode> rightExpr) { return ""; }
String CHTLJSGenerator::generateListener(const StringMap& eventMap) { return ""; }
String CHTLJSGenerator::generateEventMap(std::shared_ptr<CHTLJSASTNode> eventMap) { return ""; }
String CHTLJSGenerator::generateEventHandler(std::shared_ptr<CHTLJSASTNode> handler) { return ""; }
String CHTLJSGenerator::generateEventBinding(const String& targetSelector, const String& eventType, const String& handlerCode) { return ""; }
String CHTLJSGenerator::generateDelegate(const StringMap& delegateConfig) { return ""; }
String CHTLJSGenerator::generateDelegateConfig(std::shared_ptr<CHTLJSASTNode> config) { return ""; }
String CHTLJSGenerator::generateDelegateTarget(std::shared_ptr<CHTLJSASTNode> target) { return ""; }
String CHTLJSGenerator::generateDelegateRegistration(const DelegateRegistry& registry) { return ""; }
String CHTLJSGenerator::generateDelegateEventHandler(const String& parentSelector, const StringSet& childSelectors, const String& eventType, const String& handlerCode) { return ""; }
String CHTLJSGenerator::generateAnimation(std::shared_ptr<AnimationNode> animation) { return ""; }
String CHTLJSGenerator::generateAnimationConfig(std::shared_ptr<CHTLJSASTNode> config) { return ""; }
String CHTLJSGenerator::generateAnimationTarget(std::shared_ptr<CHTLJSASTNode> target) { return ""; }
String CHTLJSGenerator::generateAnimationKeyframe(std::shared_ptr<CHTLJSASTNode> keyframe) { return ""; }
String CHTLJSGenerator::generateAnimationWhen(std::shared_ptr<CHTLJSASTNode> whenArray) { return ""; }
String CHTLJSGenerator::generateAnimationEasing(std::shared_ptr<CHTLJSASTNode> easing) { return ""; }
String CHTLJSGenerator::generateAnimationFunction(const String& targetCode, const String& configCode) { return ""; }
String CHTLJSGenerator::generateVirtualObject(std::shared_ptr<VirtualObjectNode> virObj) { return ""; }
String CHTLJSGenerator::generateVirtualObjectAssignment(std::shared_ptr<CHTLJSASTNode> assignment) { return ""; }
String CHTLJSGenerator::generateVirtualObjectCall(std::shared_ptr<CHTLJSASTNode> call) { return ""; }
String CHTLJSGenerator::generateINeverAway(std::shared_ptr<CHTLJSASTNode> iNeverAway) { return ""; }
String CHTLJSGenerator::generateINeverAwayMap(std::shared_ptr<CHTLJSASTNode> functionMap) { return ""; }
String CHTLJSGenerator::generateVirtualFunction(const String& virName, const String& signature, const String& functionCode) { return ""; }
String CHTLJSGenerator::generateJSStatement(std::shared_ptr<CHTLJSASTNode> stmt) { return ""; }
String CHTLJSGenerator::generateJSExpression(std::shared_ptr<CHTLJSASTNode> expr) { return ""; }
String CHTLJSGenerator::generateJSFunction(std::shared_ptr<CHTLJSASTNode> func) { return ""; }
String CHTLJSGenerator::generateArrowFunction(std::shared_ptr<CHTLJSASTNode> arrowFunc) { return ""; }
String CHTLJSGenerator::generateJSCall(std::shared_ptr<CHTLJSASTNode> call) { return ""; }
String CHTLJSGenerator::generateVariableDeclaration(std::shared_ptr<CHTLJSASTNode> varDecl) { return ""; }
String CHTLJSGenerator::generateVariableGroup(std::shared_ptr<CHTLJSASTNode> varGroup) { return ""; }
String CHTLJSGenerator::generateVariableSpecialization(std::shared_ptr<CHTLJSASTNode> varSpec) { return ""; }

void CHTLJSGenerator::registerVirtualObject(const String& name, const VirtualObjectInfo& info) {
    virtualObjects_[name] = info;
    
    if (debugMode_) {
        debugLog("注册虚对象: " + name);
    }
}

VirtualObjectInfo* CHTLJSGenerator::getVirtualObject(const String& name) {
    auto it = virtualObjects_.find(name);
    return (it != virtualObjects_.end()) ? &it->second : nullptr;
}

String CHTLJSGenerator::generateVirtualFunctionName(const String& virName, const String& signature) {
    return "chtl_vir_" + virName + "_" + signature + "_" + std::to_string(functionCounter_++);
}

void CHTLJSGenerator::registerDelegate(const String& parentSelector, const StringSet& childSelectors, const StringMap& eventHandlers) {
    DelegateRegistry registry;
    registry.parentSelector = parentSelector;
    registry.childSelectors = childSelectors;
    registry.eventHandlers = eventHandlers;
    registry.registryId = "delegate_" + std::to_string(delegateCounter_++);
    
    delegateRegistries_.push_back(registry);
    
    if (debugMode_) {
        debugLog("注册事件委托: " + registry.registryId);
    }
}

String CHTLJSGenerator::generateDelegateRegistryId() {
    return "delegate_" + std::to_string(delegateCounter_++);
}

String CHTLJSGenerator::generateGlobalDelegateManager() {
    return "/* Global Delegate Manager */";
}

void CHTLJSGenerator::registerSelector(const String& selector, const String& type) {
    availableSelectors_.insert(selector);
    
    if (type == "class") {
        availableClasses_.insert(selector);
    } else if (type == "id") {
        availableIds_.insert(selector);
    }
    
    if (debugMode_) {
        debugLog("注册选择器: " + selector + " (类型: " + type + ")");
    }
}

void CHTLJSGenerator::reportError(CHTLJSGeneratorErrorType type, const String& message, const String& context) {
    result_.addError(type, message, context);
    
    if (debugMode_) {
        std::cerr << "CHTL JS生成器错误: " << message << std::endl;
    }
}

void CHTLJSGenerator::reportWarning(const String& message) {
    result_.addWarning(message);
    
    if (debugMode_) {
        std::cerr << "CHTL JS生成器警告: " << message << std::endl;
    }
}

void CHTLJSGenerator::debugLog(const String& message) const {
    if (debugMode_) {
        std::cout << "[CHTL JS Generator] " << message << std::endl;
    }
}

// 工厂方法实现
std::unique_ptr<CHTLJSGenerator> CHTLJSGeneratorFactory::createGenerator(const CompilerOptions& options) {
    return std::make_unique<CHTLJSGenerator>(options);
}

std::unique_ptr<CHTLJSGenerator> CHTLJSGeneratorFactory::createDebugGenerator() {
    CompilerOptions debugOptions;
    debugOptions.debugMode = true;
    return std::make_unique<CHTLJSGenerator>(debugOptions);
}

std::unique_ptr<CHTLJSGenerator> CHTLJSGeneratorFactory::createOptimizedGenerator() {
    CompilerOptions optimizedOptions;
    optimizedOptions.debugMode = false;
    return std::make_unique<CHTLJSGenerator>(optimizedOptions);
}

std::unique_ptr<CHTLJSGenerator> CHTLJSGeneratorFactory::createMinifiedGenerator() {
    CompilerOptions minifiedOptions;
    minifiedOptions.debugMode = false;
    // 可以添加更多优化选项
    return std::make_unique<CHTLJSGenerator>(minifiedOptions);
}

} // namespace chtl