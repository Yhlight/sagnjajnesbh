#pragma once

#include "CHTLCommon.h"
#include "CHTLJSASTNodes.h"
#include <memory>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <sstream>

namespace chtl {

// CHTL JS生成器错误类型
enum class CHTLJSGeneratorErrorType {
    AST_ERROR,                  // AST节点错误
    SELECTOR_ERROR,             // 选择器错误
    VIRTUAL_OBJECT_ERROR,       // 虚对象错误
    ANIMATION_ERROR,            // 动画错误
    DELEGATE_ERROR,             // 事件委托错误
    FUNCTION_ERROR,             // 函数生成错误
    OUTPUT_ERROR                // 输出错误
};

// CHTL JS生成器错误信息
struct CHTLJSGeneratorError {
    CHTLJSGeneratorErrorType type;
    String message;
    String context;
    size_t line;
    size_t column;
    
    CHTLJSGeneratorError(CHTLJSGeneratorErrorType t, const String& msg, const String& ctx = "", size_t l = 0, size_t c = 0)
        : type(t), message(msg), context(ctx), line(l), column(c) {}
};

// CHTL JS生成结果
struct CHTLJSGeneratorResult {
    bool success = false;
    String jsOutput;
    std::vector<CHTLJSGeneratorError> errors;
    std::vector<String> warnings;
    
    void addError(CHTLJSGeneratorErrorType type, const String& message, const String& context = "", size_t line = 0, size_t column = 0) {
        errors.emplace_back(type, message, context, line, column);
        success = false;
    }
    
    void addWarning(const String& message) {
        warnings.push_back(message);
    }
};

// 虚对象生成信息
struct VirtualObjectInfo {
    String name;
    String functionPrefix;
    StringMap functionMap;      // 虚对象方法名到实际函数名的映射
    std::vector<String> generatedFunctions;  // 生成的函数代码
};

// 事件委托注册表
struct DelegateRegistry {
    String parentSelector;
    StringSet childSelectors;
    StringMap eventHandlers;    // 事件类型到处理函数的映射
    String registryId;          // 委托注册表ID
};

// CHTL JS生成器主类
class CHTLJSGenerator {
public:
    explicit CHTLJSGenerator(const CompilerOptions& options = CompilerOptions{});
    ~CHTLJSGenerator() = default;
    
    // 主要生成接口
    CHTLJSGeneratorResult generate(std::shared_ptr<CHTLJSASTNode> ast);
    CHTLJSGeneratorResult generateFromScriptBlock(const String& scriptContent, const String& filename = "");
    
    // 节点生成方法
    String generateStatement(std::shared_ptr<CHTLJSASTNode> stmt);
    String generateExpression(std::shared_ptr<CHTLJSASTNode> expr);
    
    // 增强选择器生成
    String generateEnhancedSelector(std::shared_ptr<EnhancedSelectorNode> selector);
    String generateSelectorQuery(const String& selectorContent, const String& selectorType, const String& index = "");
    String generateClassSelectorQuery(const String& className, const String& index = "");
    String generateIdSelectorQuery(const String& idName, const String& index = "");
    String generateTagSelectorQuery(const String& tagName, const String& index = "");
    String generateComplexSelectorQuery(const String& selector, const String& index = "");
    String generateAutoSelectorQuery(const String& selector, const String& index = "");
    
    // 箭头语法生成
    String generateArrowExpression(std::shared_ptr<ArrowExpressionNode> arrow);
    String generateChainedCall(std::shared_ptr<CHTLJSASTNode> leftExpr, std::shared_ptr<CHTLJSASTNode> rightExpr);
    
    // 事件监听器生成
    String generateListener(std::shared_ptr<ListenerNode> listener);
    String generateEventMap(std::shared_ptr<CHTLJSASTNode> eventMap);
    String generateEventHandler(std::shared_ptr<CHTLJSASTNode> handler);
    String generateEventBinding(const String& targetSelector, const String& eventType, const String& handlerCode);
    
    // 事件委托生成
    String generateDelegate(std::shared_ptr<DelegateNode> delegate);
    String generateDelegateConfig(std::shared_ptr<CHTLJSASTNode> config);
    String generateDelegateTarget(std::shared_ptr<CHTLJSASTNode> target);
    String generateDelegateRegistration(const DelegateRegistry& registry);
    String generateDelegateEventHandler(const String& parentSelector, const StringSet& childSelectors, 
                                       const String& eventType, const String& handlerCode);
    
    // 动画生成
    String generateAnimation(std::shared_ptr<AnimationNode> animation);
    String generateAnimationConfig(std::shared_ptr<CHTLJSASTNode> config);
    String generateAnimationTarget(std::shared_ptr<CHTLJSASTNode> target);
    String generateAnimationKeyframe(std::shared_ptr<CHTLJSASTNode> keyframe);
    String generateAnimationWhen(std::shared_ptr<CHTLJSASTNode> whenArray);
    String generateAnimationEasing(std::shared_ptr<CHTLJSASTNode> easing);
    String generateAnimationFunction(const String& targetCode, const String& configCode);
    
    // 虚对象生成
    String generateVirtualObject(std::shared_ptr<VirtualObjectNode> virObj);
    String generateVirtualObjectAssignment(std::shared_ptr<CHTLJSASTNode> assignment);
    String generateVirtualObjectCall(std::shared_ptr<CHTLJSASTNode> call);
    String generateINeverAway(std::shared_ptr<CHTLJSASTNode> iNeverAway);
    String generateINeverAwayMap(std::shared_ptr<CHTLJSASTNode> functionMap);
    String generateVirtualFunction(const String& virName, const String& signature, const String& functionCode);
    
    // JavaScript表达式生成
    String generateJSStatement(std::shared_ptr<CHTLJSASTNode> stmt);
    String generateJSExpression(std::shared_ptr<CHTLJSASTNode> expr);
    String generateJSFunction(std::shared_ptr<CHTLJSASTNode> func);
    String generateArrowFunction(std::shared_ptr<CHTLJSASTNode> arrowFunc);
    String generateJSCall(std::shared_ptr<CHTLJSASTNode> call);
    String generateVariableDeclaration(std::shared_ptr<CHTLJSASTNode> varDecl);
    
    // 变量组生成
    String generateVariableGroup(std::shared_ptr<CHTLJSASTNode> varGroup);
    String generateVariableSpecialization(std::shared_ptr<CHTLJSASTNode> varSpec);
    
    // 工具方法
    void setDebugMode(bool debug) { debugMode_ = debug; }
    bool isDebugMode() const { return debugMode_; }
    
    // 虚对象管理
    void registerVirtualObject(const String& name, const VirtualObjectInfo& info);
    VirtualObjectInfo* getVirtualObject(const String& name);
    String generateVirtualFunctionName(const String& virName, const String& signature);
    
    // 事件委托管理
    void registerDelegate(const String& parentSelector, const StringSet& childSelectors, 
                         const StringMap& eventHandlers);
    String generateDelegateRegistryId();
    String generateGlobalDelegateManager();
    
    // 选择器管理
    void registerSelector(const String& selector, const String& type);
    StringSet getAvailableSelectors() const { return availableSelectors_; }
    StringSet getAvailableClasses() const { return availableClasses_; }
    StringSet getAvailableIds() const { return availableIds_; }
    
    // 错误处理
    void reportError(CHTLJSGeneratorErrorType type, const String& message, const String& context = "");
    void reportWarning(const String& message);

private:
    CompilerOptions options_;
    bool debugMode_;
    CHTLJSGeneratorResult result_;
    
    // 生成状态
    std::ostringstream jsStream_;
    std::ostringstream helperStream_;   // 辅助函数流
    std::ostringstream initStream_;     // 初始化代码流
    
    // 虚对象管理
    std::unordered_map<String, VirtualObjectInfo> virtualObjects_;
    size_t functionCounter_;
    
    // 事件委托管理
    std::vector<DelegateRegistry> delegateRegistries_;
    size_t delegateCounter_;
    
    // 选择器管理
    StringSet availableSelectors_;
    StringSet availableClasses_;
    StringSet availableIds_;
    
    // 动画管理
    std::vector<String> animationFunctions_;
    size_t animationCounter_;
    
    // 内部生成方法
    void generateNode(std::shared_ptr<CHTLJSASTNode> node);
    void processChildren(std::shared_ptr<CHTLJSASTNode> parent);
    
    // 选择器处理
    String processSelectorContent(const String& content) const;
    String generateSelectorFunction(const String& selectorType, const String& content, const String& index = "");
    String optimizeSelectorQuery(const String& query) const;
    
    // 事件处理
    String generateEventListenerCode(const String& targetCode, const String& eventType, const String& handlerCode);
    String generateEventDelegationCode(const String& parentSelector, const String& childSelector, 
                                      const String& eventType, const String& handlerCode);
    String wrapEventHandler(const String& handlerCode) const;
    
    // 动画处理
    String generateKeyframeCSS(const StringMap& properties) const;
    String generateEasingFunction(const String& easingType) const;
    String generateAnimationTimeline(const std::vector<String>& keyframes, const std::vector<String>& timings) const;
    
    // 虚对象处理
    String processVirtualSignature(const String& signature) const;
    String generateVirtualMethodCall(const String& virName, const String& methodName, const std::vector<String>& args);
    String generateVirtualObjectRegistry() const;
    
    // JavaScript代码处理
    String sanitizeJSCode(const String& code) const;
    String wrapInIIFE(const String& code) const;
    String generateStrictMode() const;
    
    // 输出格式化和优化
    String formatJavaScript(const String& js) const;
    String optimizeJavaScript(const String& js) const;
    String minifyJavaScript(const String& js) const;
    
    // 辅助函数生成
    String generateSelectorHelpers() const;
    String generateAnimationHelpers() const;
    String generateDelegateHelpers() const;
    String generateVirtualObjectHelpers() const;
    String generateUtilityFunctions() const;
    
    // 调试和日志
    void debugLog(const String& message) const;
    void logGenerationStep(const String& step, const String& details = "") const;
    
    // 验证和检查
    bool isValidJavaScript(const String& code) const;
    bool isValidSelector(const String& selector) const;
    bool isValidEventType(const String& eventType) const;
    bool isValidAnimationProperty(const String& property) const;
    
    // 代码生成辅助
    String escapeJSString(const String& str) const;
    String generateJSComment(const String& comment) const;
    String generateJSBlock(const String& content, int indentLevel = 0) const;
    String indentCode(const String& code, int level) const;
};

// CHTL JS生成器工厂
class CHTLJSGeneratorFactory {
public:
    static std::unique_ptr<CHTLJSGenerator> createGenerator(const CompilerOptions& options = CompilerOptions{});
    static std::unique_ptr<CHTLJSGenerator> createDebugGenerator();
    static std::unique_ptr<CHTLJSGenerator> createOptimizedGenerator();
    static std::unique_ptr<CHTLJSGenerator> createMinifiedGenerator();
};

// 选择器查询生成器辅助类
class SelectorQueryGenerator {
public:
    SelectorQueryGenerator();
    
    String generateQuery(const String& selector, const String& index = "");
    String generateClassQuery(const String& className, const String& index = "");
    String generateIdQuery(const String& idName, const String& index = "");
    String generateTagQuery(const String& tagName, const String& index = "");
    String generateComplexQuery(const String& selector, const String& index = "");
    
    void setOptimizationLevel(int level) { optimizationLevel_ = level; }
    
private:
    int optimizationLevel_;
    
    String optimizeQuery(const String& query) const;
    String generateIndexedQuery(const String& baseQuery, const String& index) const;
};

// 动画生成器辅助类
class AnimationGenerator {
public:
    AnimationGenerator();
    
    String generateAnimation(const String& targetCode, const StringMap& config);
    String generateKeyframe(const StringMap& properties);
    String generateEasing(const String& easingFunction);
    String generateTimeline(const std::vector<std::pair<double, StringMap>>& keyframes);
    
    void setAnimationFramework(const String& framework) { framework_ = framework; }
    
private:
    String framework_;  // 动画框架 (requestAnimationFrame, GSAP, etc.)
    
    String generateRequestAnimationFrame(const String& animationCode) const;
    String generateCSSAnimation(const String& animationName, const StringMap& keyframes) const;
    String generateJSAnimation(const String& targetCode, const String& animationCode) const;
};

// 事件委托管理器辅助类
class DelegateManager {
public:
    DelegateManager();
    
    String registerDelegate(const String& parentSelector, const StringSet& childSelectors, 
                           const StringMap& eventHandlers);
    String generateGlobalManager();
    String generateDelegateCode(const String& registryId);
    
    void setDelegationStrategy(const String& strategy) { strategy_ = strategy; }
    
private:
    String strategy_;  // 委托策略 (bubble, capture, etc.)
    std::unordered_map<String, DelegateRegistry> registries_;
    size_t registryCounter_;
    
    String generateEventDelegation(const DelegateRegistry& registry) const;
    String optimizeDelegateCode(const String& code) const;
};

} // namespace chtl