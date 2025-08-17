#pragma once

#include "CHTLCommon.h"
#include "CHTLTokens.h"
#include "CHTLJSASTNodes.h"
#include "CHTLLexer.h"
#include "StateMachine.h"
#include "ContextManager.h"
#include <memory>
#include <vector>
#include <unordered_map>

namespace chtl {

// CHTL JS解析器错误类型
enum class CHTLJSParseErrorType {
    SYNTAX_ERROR,               // 语法错误
    INVALID_SELECTOR,           // 无效选择器
    INVALID_EVENT,              // 无效事件
    INVALID_ANIMATION,          // 无效动画
    VIRTUAL_OBJECT_ERROR,       // 虚对象错误
    DELEGATE_ERROR,             // 事件委托错误
    ARROW_SYNTAX_ERROR,         // 箭头语法错误
    ENHANCED_SELECTOR_ERROR     // 增强选择器错误
};

// CHTL JS解析器错误信息
struct CHTLJSParseError {
    CHTLJSParseErrorType type;
    String message;
    size_t line;
    size_t column;
    String context;
    
    CHTLJSParseError(CHTLJSParseErrorType t, const String& msg, size_t l = 0, size_t c = 0, const String& ctx = "")
        : type(t), message(msg), line(l), column(c), context(ctx) {}
};

// CHTL JS解析结果
struct CHTLJSParseResult {
    bool success = false;
    std::shared_ptr<CHTLJSASTNode> ast = nullptr;
    std::vector<CHTLJSParseError> errors;
    std::vector<String> warnings;
    
    void addError(CHTLJSParseErrorType type, const String& message, size_t line = 0, size_t column = 0, const String& context = "") {
        errors.emplace_back(type, message, line, column, context);
        success = false;
    }
    
    void addWarning(const String& message) {
        warnings.push_back(message);
    }
};

// 虚对象注册表项
struct VirtualObjectEntry {
    String name;
    String functionPrefix;
    StringMap functionMap;  // 键名到实际函数名的映射
    std::vector<String> functionKeys;
};

// CHTL JS解析器主类
class CHTLJSParser {
public:
    explicit CHTLJSParser(const CompilerOptions& options = CompilerOptions{});
    ~CHTLJSParser() = default;
    
    // 主要解析接口
    CHTLJSParseResult parse(const String& source, const String& filename = "");
    CHTLJSParseResult parseFromTokens(const std::vector<Token>& tokens, const String& filename = "");
    CHTLJSParseResult parseScriptBlock(const String& scriptContent, const String& filename = "");
    
    // 核心CHTL JS语法解析
    std::shared_ptr<CHTLJSASTNode> parseStatement();
    std::shared_ptr<CHTLJSASTNode> parseExpression();
    
    // 增强选择器解析 {{selector}}
    std::shared_ptr<EnhancedSelectorNode> parseEnhancedSelector();
    std::shared_ptr<CHTLJSASTNode> parseSelectorContent();
    std::shared_ptr<CHTLJSASTNode> parseClassSelector();
    std::shared_ptr<CHTLJSASTNode> parseIdSelector();
    std::shared_ptr<CHTLJSASTNode> parseTagSelector();
    std::shared_ptr<CHTLJSASTNode> parseComplexSelector();
    std::shared_ptr<CHTLJSASTNode> parseIndexedSelector();
    
    // 箭头语法解析 ->
    std::shared_ptr<ArrowExpressionNode> parseArrowExpression();
    std::shared_ptr<CHTLJSASTNode> parseChainedCall();
    
    // 事件监听器解析 listen
    std::shared_ptr<ListenerNode> parseListener();
    std::shared_ptr<CHTLJSASTNode> parseEventMap();
    std::shared_ptr<CHTLJSASTNode> parseEventHandler();
    
    // 事件委托解析 delegate
    std::shared_ptr<DelegateNode> parseDelegate();
    std::shared_ptr<CHTLJSASTNode> parseDelegateTarget();
    std::shared_ptr<CHTLJSASTNode> parseDelegateEventMap();
    
    // 动画解析 animate
    std::shared_ptr<AnimationNode> parseAnimation();
    std::shared_ptr<CHTLJSASTNode> parseAnimationTarget();
    std::shared_ptr<CHTLJSASTNode> parseAnimationProperties();
    std::shared_ptr<CHTLJSASTNode> parseAnimationKeyframe();
    std::shared_ptr<CHTLJSASTNode> parseAnimationWhen();
    std::shared_ptr<CHTLJSASTNode> parseAnimationEasing();
    
    // 虚对象解析 vir
    std::shared_ptr<VirtualObjectNode> parseVirtualObject();
    std::shared_ptr<CHTLJSASTNode> parseVirtualObjectAssignment();
    std::shared_ptr<CHTLJSASTNode> parseVirtualObjectCall();
    
    // 变量组解析
    std::shared_ptr<CHTLJSASTNode> parseVariableGroup();
    std::shared_ptr<CHTLJSASTNode> parseVariableSpecialization();
    
    // JavaScript表达式解析
    std::shared_ptr<CHTLJSASTNode> parseJSFunction();
    std::shared_ptr<CHTLJSASTNode> parseJSObject();
    std::shared_ptr<CHTLJSASTNode> parseJSArray();
    std::shared_ptr<CHTLJSASTNode> parseJSProperty();
    std::shared_ptr<CHTLJSASTNode> parseJSCall();
    
    // 字面量和标识符解析
    String parseIdentifier();
    String parseStringLiteral();
    String parseNumberLiteral();
    std::shared_ptr<CHTLJSASTNode> parseLiteral();
    
    // 工具方法
    void setDebugMode(bool debug) { debugMode_ = debug; }
    bool isDebugMode() const { return debugMode_; }
    
    // 虚对象管理
    void registerVirtualObject(const String& name, const VirtualObjectEntry& entry);
    VirtualObjectEntry* getVirtualObject(const String& name);
    String generateFunctionName(const String& virName, const String& key);
    
    // 错误处理
    void reportError(CHTLJSParseErrorType type, const String& message, const String& context = "");
    void reportWarning(const String& message);
    
    // 状态管理
    void pushContext(const String& contextName);
    void popContext();
    String getCurrentContext() const;

private:
    // 核心解析状态
    std::vector<Token> tokens_;
    size_t currentPos_;
    String currentFilename_;
    CHTLJSParseResult result_;
    CompilerOptions options_;
    bool debugMode_;
    
    // 状态管理
    std::unique_ptr<StateMachine> stateMachine_;
    std::unique_ptr<ContextManager> contextManager_;
    std::stack<String> contextStack_;
    
    // 虚对象注册表
    std::unordered_map<String, VirtualObjectEntry> virtualObjects_;
    size_t functionCounter_;
    
    // 解析上下文
    struct CHTLJSParseContext {
        StringSet availableSelectors;
        StringSet availableClasses;
        StringSet availableIds;
        StringSet virtualObjectNames;
        bool inArrowExpression = false;
        bool inListener = false;
        bool inDelegate = false;
        bool inAnimation = false;
        bool inVirtualObject = false;
        int nestingLevel = 0;
    };
    
    CHTLJSParseContext currentContext_;
    std::stack<CHTLJSParseContext> contextStack_backup_;
    
    // Token操作
    const Token& currentToken() const;
    const Token& peekToken(size_t offset = 1) const;
    Token consumeToken();
    Token consumeToken(TokenType expectedType);
    bool matchToken(TokenType type) const;
    bool matchAnyToken(const std::vector<TokenType>& types) const;
    void skipToken();
    void skipToToken(TokenType type);
    void skipToMatchingBrace();
    void skipToMatchingParen();
    
    // 语法验证
    bool isValidSelector(const String& selector) const;
    bool isValidEventType(const String& eventType) const;
    bool isValidAnimationProperty(const String& property) const;
    bool isValidEasingFunction(const String& easing) const;
    bool isValidVirtualObjectName(const String& name) const;
    
    // 选择器相关验证
    bool isClassSelector(const String& selector) const;
    bool isIdSelector(const String& selector) const;
    bool isTagSelector(const String& selector) const;
    bool isComplexSelector(const String& selector) const;
    bool hasIndexAccess(const String& selector) const;
    
    // 上下文相关解析
    void enterArrowContext();
    void exitArrowContext();
    void enterListenerContext();
    void exitListenerContext();
    void enterDelegateContext();
    void exitDelegateContext();
    void enterAnimationContext();
    void exitAnimationContext();
    void enterVirtualObjectContext(const String& virName);
    void exitVirtualObjectContext();
    
    // 选择器解析辅助
    String extractSelectorText(const String& fullSelector) const;
    String extractIndexFromSelector(const String& selector) const;
    std::vector<String> parseSelectorParts(const String& selector) const;
    
    // 动画属性解析辅助
    std::unordered_map<String, String> parseAnimationKeyframeProperties(const String& properties) const;
    
    // 辅助方法
    bool isAtEnd() const;
    void synchronize();
    void skipWhitespace();
    void skipComments();
    
    // 调试和日志
    void debugLog(const String& message) const;
    void logParseStep(const String& step, const String& details = "") const;
};

// CHTL JS解析器工厂
class CHTLJSParserFactory {
public:
    static std::unique_ptr<CHTLJSParser> createParser(const CompilerOptions& options = CompilerOptions{});
    static std::unique_ptr<CHTLJSParser> createDebugParser();
};

} // namespace chtl