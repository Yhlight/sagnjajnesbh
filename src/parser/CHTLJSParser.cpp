#include "../../include/CHTLJSParser.h"
#include "../../include/CHTLLexer.h"
#include <iostream>
#include <sstream>
#include <algorithm>
#include <regex>

namespace chtl {

CHTLJSParser::CHTLJSParser(const CompilerOptions& options) 
    : currentPos_(0), options_(options), debugMode_(options.debugMode),
      stateMachine_(std::make_unique<StateMachine>()),
      contextManager_(std::make_unique<ContextManager>()),
      functionCounter_(0) {
    
    // 初始化解析上下文
    currentContext_.nestingLevel = 0;
    
    if (debugMode_) {
        debugLog("CHTL JS解析器初始化完成");
    }
}

CHTLJSParseResult CHTLJSParser::parse(const String& source, const String& filename) {
    currentFilename_ = filename;
    result_ = CHTLJSParseResult{};
    
    if (debugMode_) {
        debugLog("开始解析CHTL JS文件: " + filename);
    }
    
    // 使用词法分析器生成Token
    CHTLLexer lexer(options_);
    auto lexResult = lexer.tokenize(source, filename);
    
    if (!lexResult.success) {
        result_.success = false;
        for (const auto& error : lexResult.errors) {
            result_.addError(CHTLJSParseErrorType::SYNTAX_ERROR, error.message, error.line, error.column, filename);
        }
        return result_;
    }
    
    return parseFromTokens(lexResult.tokens, filename);
}

CHTLJSParseResult CHTLJSParser::parseFromTokens(const std::vector<Token>& tokens, const String& filename) {
    tokens_ = tokens;
    currentPos_ = 0;
    currentFilename_ = filename;
    result_ = CHTLJSParseResult{};
    
    if (tokens_.empty()) {
        result_.addError(CHTLJSParseErrorType::SYNTAX_ERROR, "空的Token流", 0, 0, filename);
        return result_;
    }
    
    try {
        // 创建根节点
        auto root = std::make_shared<CHTLJSRootNode>(0, 0);
        
        // 解析语句
        while (!isAtEnd()) {
            skipWhitespace();
            skipComments();
            
            if (isAtEnd()) break;
            
            auto stmt = parseStatement();
            if (stmt) {
                root->addChild(stmt);
            }
        }
        
        result_.ast = root;
        result_.success = true;
        
        if (debugMode_) {
            debugLog("CHTL JS解析完成，生成AST节点: " + std::to_string(root->getChildren().size()) + " 个语句");
        }
        
    } catch (const std::exception& e) {
        result_.addError(CHTLJSParseErrorType::SYNTAX_ERROR, "解析异常: " + String(e.what()), 
                        currentToken().line, currentToken().column, filename);
    }
    
    return result_;
}

CHTLJSParseResult CHTLJSParser::parseScriptBlock(const String& scriptContent, const String& filename) {
    // 专门用于解析局部script块内容
    return parse(scriptContent, filename);
}

std::shared_ptr<CHTLJSASTNode> CHTLJSParser::parseStatement() {
    const auto& token = currentToken();
    
    switch (token.type) {
        case TokenType::VIR:
            return parseVirtualObject();
        case TokenType::DOUBLE_LEFT_BRACE:
            return parseEnhancedSelector();
        case TokenType::IDENTIFIER: {
            // 可能是变量赋值、函数调用或其他JS语句
            const auto& nextToken = peekToken();
            if (nextToken.type == TokenType::ARROW) {
                return parseArrowExpression();
            } else {
                return parseJSStatement();
            }
        }
        case TokenType::CONST:
        case TokenType::LET:
        case TokenType::VAR:
            return parseVariableDeclaration();
        default:
            return parseJSStatement();
    }
}

std::shared_ptr<CHTLJSASTNode> CHTLJSParser::parseExpression() {
    return parseJSExpression();
}

std::shared_ptr<EnhancedSelectorNode> CHTLJSParser::parseEnhancedSelector() {
    const auto& startToken = consumeToken(TokenType::DOUBLE_LEFT_BRACE); // {{
    
    String selectorContent;
    while (!matchToken(TokenType::DOUBLE_RIGHT_BRACE) && !isAtEnd()) {
        selectorContent += currentToken().value;
        skipToken();
    }
    
    if (matchToken(TokenType::DOUBLE_RIGHT_BRACE)) {
        consumeToken(TokenType::DOUBLE_RIGHT_BRACE); // }}
    } else {
        reportError(CHTLJSParseErrorType::ENHANCED_SELECTOR_ERROR, "增强选择器缺少 '}}'");
        return nullptr;
    }
    
    if (!isValidSelector(selectorContent)) {
        reportError(CHTLJSParseErrorType::INVALID_SELECTOR, "无效的选择器: " + selectorContent);
        return nullptr;
    }
    
    auto selector = std::make_shared<EnhancedSelectorNode>(selectorContent, startToken.line, startToken.column);
    
    // 解析选择器类型
    if (isClassSelector(selectorContent)) {
        selector->setSelectorType("class");
    } else if (isIdSelector(selectorContent)) {
        selector->setSelectorType("id");
    } else if (isTagSelector(selectorContent)) {
        selector->setSelectorType("tag");
    } else if (isComplexSelector(selectorContent)) {
        selector->setSelectorType("complex");
    } else {
        selector->setSelectorType("auto"); // 自动推导
    }
    
    // 检查是否有索引访问
    if (hasIndexAccess(selectorContent)) {
        String index = extractIndexFromSelector(selectorContent);
        selector->setIndex(index);
    }
    
    return selector;
}

std::shared_ptr<ArrowExpressionNode> CHTLJSParser::parseArrowExpression() {
    auto leftExpr = parseJSExpression(); // 左侧表达式
    
    if (!matchToken(TokenType::ARROW)) {
        reportError(CHTLJSParseErrorType::ARROW_SYNTAX_ERROR, "期望箭头符号 '->'");
        return nullptr;
    }
    
    const auto& arrowToken = consumeToken(TokenType::ARROW);
    
    enterArrowContext();
    
    auto rightExpr = parseJSExpression(); // 右侧表达式
    
    exitArrowContext();
    
    auto arrowExpr = std::make_shared<ArrowExpressionNode>(leftExpr, rightExpr, arrowToken.line, arrowToken.column);
    return arrowExpr;
}

std::shared_ptr<ListenerNode> CHTLJSParser::parseListener() {
    const auto& listenToken = consumeToken(TokenType::LISTEN);
    
    enterListenerContext();
    
    if (!matchToken(TokenType::LEFT_PAREN)) {
        reportError(CHTLJSParseErrorType::SYNTAX_ERROR, "listen函数缺少左括号");
        exitListenerContext();
        return nullptr;
    }
    
    consumeToken(TokenType::LEFT_PAREN);
    
    auto eventMap = parseEventMap();
    
    if (!matchToken(TokenType::RIGHT_PAREN)) {
        reportError(CHTLJSParseErrorType::SYNTAX_ERROR, "listen函数缺少右括号");
        exitListenerContext();
        return nullptr;
    }
    
    consumeToken(TokenType::RIGHT_PAREN);
    
    exitListenerContext();
    
    auto listener = std::make_shared<ListenerNode>(eventMap, listenToken.line, listenToken.column);
    return listener;
}

std::shared_ptr<CHTLJSASTNode> CHTLJSParser::parseEventMap() {
    if (!matchToken(TokenType::LEFT_BRACE)) {
        reportError(CHTLJSParseErrorType::SYNTAX_ERROR, "事件映射缺少左花括号");
        return nullptr;
    }
    
    consumeToken(TokenType::LEFT_BRACE);
    
    auto eventMap = std::make_shared<EventMapNode>(currentToken().line, currentToken().column);
    
    while (!matchToken(TokenType::RIGHT_BRACE) && !isAtEnd()) {
        skipWhitespace();
        skipComments();
        
        if (matchToken(TokenType::RIGHT_BRACE)) break;
        
        // 解析事件名
        const auto& eventToken = consumeToken(TokenType::IDENTIFIER);
        String eventName = eventToken.value;
        
        if (!isValidEventType(eventName)) {
            reportError(CHTLJSParseErrorType::INVALID_EVENT, "无效的事件类型: " + eventName);
            continue;
        }
        
        if (!matchToken(TokenType::COLON)) {
            reportError(CHTLJSParseErrorType::SYNTAX_ERROR, "事件映射缺少冒号");
            continue;
        }
        
        consumeToken(TokenType::COLON);
        
        // 解析事件处理函数
        auto handler = parseEventHandler();
        if (handler) {
            auto eventPair = std::make_shared<EventPairNode>(eventName, handler, eventToken.line, eventToken.column);
            eventMap->addChild(eventPair);
        }
        
        // 可选的逗号
        if (matchToken(TokenType::COMMA)) {
            skipToken();
        }
    }
    
    if (matchToken(TokenType::RIGHT_BRACE)) {
        consumeToken(TokenType::RIGHT_BRACE);
    } else {
        reportError(CHTLJSParseErrorType::SYNTAX_ERROR, "事件映射缺少右花括号");
    }
    
    return eventMap;
}

std::shared_ptr<CHTLJSASTNode> CHTLJSParser::parseEventHandler() {
    const auto& token = currentToken();
    
    if (token.type == TokenType::IDENTIFIER) {
        // 可能是函数名引用
        String functionName = token.value;
        skipToken();
        return std::make_shared<FunctionReferenceNode>(functionName, token.line, token.column);
    } else if (token.type == TokenType::FUNCTION) {
        // 匿名函数
        return parseJSFunction();
    } else if (token.type == TokenType::LEFT_PAREN) {
        // 箭头函数 () => {}
        return parseArrowFunction();
    } else {
        reportError(CHTLJSParseErrorType::SYNTAX_ERROR, "无效的事件处理器");
        return nullptr;
    }
}

std::shared_ptr<DelegateNode> CHTLJSParser::parseDelegate() {
    const auto& delegateToken = consumeToken(TokenType::DELEGATE);
    
    enterDelegateContext();
    
    if (!matchToken(TokenType::LEFT_PAREN)) {
        reportError(CHTLJSParseErrorType::DELEGATE_ERROR, "delegate函数缺少左括号");
        exitDelegateContext();
        return nullptr;
    }
    
    consumeToken(TokenType::LEFT_PAREN);
    
    // 解析委托配置对象
    auto config = parseDelegateConfig();
    
    if (!matchToken(TokenType::RIGHT_PAREN)) {
        reportError(CHTLJSParseErrorType::DELEGATE_ERROR, "delegate函数缺少右括号");
        exitDelegateContext();
        return nullptr;
    }
    
    consumeToken(TokenType::RIGHT_PAREN);
    
    exitDelegateContext();
    
    auto delegate = std::make_shared<DelegateNode>(config, delegateToken.line, delegateToken.column);
    return delegate;
}

std::shared_ptr<CHTLJSASTNode> CHTLJSParser::parseDelegateConfig() {
    if (!matchToken(TokenType::LEFT_BRACE)) {
        reportError(CHTLJSParseErrorType::SYNTAX_ERROR, "委托配置缺少左花括号");
        return nullptr;
    }
    
    consumeToken(TokenType::LEFT_BRACE);
    
    auto config = std::make_shared<DelegateConfigNode>(currentToken().line, currentToken().column);
    
    while (!matchToken(TokenType::RIGHT_BRACE) && !isAtEnd()) {
        skipWhitespace();
        skipComments();
        
        if (matchToken(TokenType::RIGHT_BRACE)) break;
        
        const auto& keyToken = consumeToken(TokenType::IDENTIFIER);
        String key = keyToken.value;
        
        if (!matchToken(TokenType::COLON)) {
            reportError(CHTLJSParseErrorType::SYNTAX_ERROR, "委托配置缺少冒号");
            continue;
        }
        
        consumeToken(TokenType::COLON);
        
        std::shared_ptr<CHTLJSASTNode> value;
        
        if (key == "target") {
            value = parseDelegateTarget();
        } else if (isValidEventType(key)) {
            value = parseEventHandler();
        } else {
            value = parseJSExpression();
        }
        
        if (value) {
            auto configPair = std::make_shared<ConfigPairNode>(key, value, keyToken.line, keyToken.column);
            config->addChild(configPair);
        }
        
        if (matchToken(TokenType::COMMA)) {
            skipToken();
        }
    }
    
    if (matchToken(TokenType::RIGHT_BRACE)) {
        consumeToken(TokenType::RIGHT_BRACE);
    } else {
        reportError(CHTLJSParseErrorType::SYNTAX_ERROR, "委托配置缺少右花括号");
    }
    
    return config;
}

std::shared_ptr<CHTLJSASTNode> CHTLJSParser::parseDelegateTarget() {
    if (matchToken(TokenType::DOUBLE_LEFT_BRACE)) {
        // 单个选择器 {{selector}}
        return parseEnhancedSelector();
    } else if (matchToken(TokenType::LEFT_BRACKET)) {
        // 选择器数组 [{{sel1}}, {{sel2}}]
        consumeToken(TokenType::LEFT_BRACKET);
        
        auto targetArray = std::make_shared<TargetArrayNode>(currentToken().line, currentToken().column);
        
        while (!matchToken(TokenType::RIGHT_BRACKET) && !isAtEnd()) {
            skipWhitespace();
            
            if (matchToken(TokenType::RIGHT_BRACKET)) break;
            
            if (matchToken(TokenType::DOUBLE_LEFT_BRACE)) {
                auto selector = parseEnhancedSelector();
                if (selector) {
                    targetArray->addChild(selector);
                }
            }
            
            if (matchToken(TokenType::COMMA)) {
                skipToken();
            }
        }
        
        if (matchToken(TokenType::RIGHT_BRACKET)) {
            consumeToken(TokenType::RIGHT_BRACKET);
        }
        
        return targetArray;
    } else {
        reportError(CHTLJSParseErrorType::DELEGATE_ERROR, "无效的委托目标");
        return nullptr;
    }
}

std::shared_ptr<AnimationNode> CHTLJSParser::parseAnimation() {
    const auto& animateToken = consumeToken(TokenType::ANIMATE);
    
    enterAnimationContext();
    
    if (!matchToken(TokenType::LEFT_PAREN)) {
        reportError(CHTLJSParseErrorType::INVALID_ANIMATION, "animate函数缺少左括号");
        exitAnimationContext();
        return nullptr;
    }
    
    consumeToken(TokenType::LEFT_PAREN);
    
    auto animationConfig = parseAnimationProperties();
    
    if (!matchToken(TokenType::RIGHT_PAREN)) {
        reportError(CHTLJSParseErrorType::INVALID_ANIMATION, "animate函数缺少右括号");
        exitAnimationContext();
        return nullptr;
    }
    
    consumeToken(TokenType::RIGHT_PAREN);
    
    exitAnimationContext();
    
    auto animation = std::make_shared<AnimationNode>(animationConfig, animateToken.line, animateToken.column);
    return animation;
}

std::shared_ptr<CHTLJSASTNode> CHTLJSParser::parseAnimationProperties() {
    if (!matchToken(TokenType::LEFT_BRACE)) {
        reportError(CHTLJSParseErrorType::SYNTAX_ERROR, "动画配置缺少左花括号");
        return nullptr;
    }
    
    consumeToken(TokenType::LEFT_BRACE);
    
    auto config = std::make_shared<AnimationConfigNode>(currentToken().line, currentToken().column);
    
    while (!matchToken(TokenType::RIGHT_BRACE) && !isAtEnd()) {
        skipWhitespace();
        skipComments();
        
        if (matchToken(TokenType::RIGHT_BRACE)) break;
        
        const auto& keyToken = consumeToken(TokenType::IDENTIFIER);
        String key = keyToken.value;
        
        if (!matchToken(TokenType::COLON)) {
            reportError(CHTLJSParseErrorType::SYNTAX_ERROR, "动画配置缺少冒号");
            continue;
        }
        
        consumeToken(TokenType::COLON);
        
        std::shared_ptr<CHTLJSASTNode> value;
        
        if (key == "target") {
            value = parseAnimationTarget();
        } else if (key == "when") {
            value = parseAnimationWhen();
        } else if (key == "begin" || key == "end") {
            value = parseAnimationKeyframe();
        } else if (key == "easing") {
            value = parseAnimationEasing();
        } else {
            value = parseJSExpression();
        }
        
        if (value) {
            auto animProp = std::make_shared<AnimationPropertyNode>(key, value, keyToken.line, keyToken.column);
            config->addChild(animProp);
        }
        
        if (matchToken(TokenType::COMMA)) {
            skipToken();
        }
    }
    
    if (matchToken(TokenType::RIGHT_BRACE)) {
        consumeToken(TokenType::RIGHT_BRACE);
    } else {
        reportError(CHTLJSParseErrorType::SYNTAX_ERROR, "动画配置缺少右花括号");
    }
    
    return config;
}

std::shared_ptr<CHTLJSASTNode> CHTLJSParser::parseAnimationTarget() {
    if (matchToken(TokenType::DOUBLE_LEFT_BRACE)) {
        return parseEnhancedSelector();
    } else if (matchToken(TokenType::LEFT_BRACKET)) {
        // 目标数组
        consumeToken(TokenType::LEFT_BRACKET);
        
        auto targetArray = std::make_shared<TargetArrayNode>(currentToken().line, currentToken().column);
        
        while (!matchToken(TokenType::RIGHT_BRACKET) && !isAtEnd()) {
            skipWhitespace();
            
            if (matchToken(TokenType::RIGHT_BRACKET)) break;
            
            if (matchToken(TokenType::DOUBLE_LEFT_BRACE)) {
                auto selector = parseEnhancedSelector();
                if (selector) {
                    targetArray->addChild(selector);
                }
            } else {
                auto expr = parseJSExpression();
                if (expr) {
                    targetArray->addChild(expr);
                }
            }
            
            if (matchToken(TokenType::COMMA)) {
                skipToken();
            }
        }
        
        if (matchToken(TokenType::RIGHT_BRACKET)) {
            consumeToken(TokenType::RIGHT_BRACKET);
        }
        
        return targetArray;
    } else {
        return parseJSExpression();
    }
}

std::shared_ptr<CHTLJSASTNode> CHTLJSParser::parseAnimationKeyframe() {
    if (!matchToken(TokenType::LEFT_BRACE)) {
        reportError(CHTLJSParseErrorType::SYNTAX_ERROR, "动画关键帧缺少左花括号");
        return nullptr;
    }
    
    consumeToken(TokenType::LEFT_BRACE);
    
    auto keyframe = std::make_shared<KeyframeNode>(currentToken().line, currentToken().column);
    
    while (!matchToken(TokenType::RIGHT_BRACE) && !isAtEnd()) {
        skipWhitespace();
        skipComments();
        
        if (matchToken(TokenType::RIGHT_BRACE)) break;
        
        // 解析CSS属性
        const auto& propToken = consumeToken(TokenType::IDENTIFIER);
        String property = propToken.value;
        
        if (!matchToken(TokenType::COLON)) {
            reportError(CHTLJSParseErrorType::SYNTAX_ERROR, "关键帧属性缺少冒号");
            continue;
        }
        
        consumeToken(TokenType::COLON);
        
        String value;
        if (matchToken(TokenType::STRING_LITERAL) || matchToken(TokenType::SINGLE_STRING)) {
            value = parseLiteral();
        } else {
            // 解析表达式
            auto valueExpr = parseJSExpression();
            if (valueExpr) {
                value = valueExpr->toString();
            }
        }
        
        auto cssProp = std::make_shared<CSSPropertyNode>(property, value, propToken.line, propToken.column);
        keyframe->addChild(cssProp);
        
        if (matchToken(TokenType::COMMA)) {
            skipToken();
        }
    }
    
    if (matchToken(TokenType::RIGHT_BRACE)) {
        consumeToken(TokenType::RIGHT_BRACE);
    } else {
        reportError(CHTLJSParseErrorType::SYNTAX_ERROR, "动画关键帧缺少右花括号");
    }
    
    return keyframe;
}

std::shared_ptr<CHTLJSASTNode> CHTLJSParser::parseAnimationWhen() {
    if (!matchToken(TokenType::LEFT_BRACKET)) {
        reportError(CHTLJSParseErrorType::SYNTAX_ERROR, "when数组缺少左方括号");
        return nullptr;
    }
    
    consumeToken(TokenType::LEFT_BRACKET);
    
    auto whenArray = std::make_shared<WhenArrayNode>(currentToken().line, currentToken().column);
    
    while (!matchToken(TokenType::RIGHT_BRACKET) && !isAtEnd()) {
        skipWhitespace();
        
        if (matchToken(TokenType::RIGHT_BRACKET)) break;
        
        if (matchToken(TokenType::LEFT_BRACE)) {
            auto whenItem = parseWhenItem();
            if (whenItem) {
                whenArray->addChild(whenItem);
            }
        }
        
        if (matchToken(TokenType::COMMA)) {
            skipToken();
        }
    }
    
    if (matchToken(TokenType::RIGHT_BRACKET)) {
        consumeToken(TokenType::RIGHT_BRACKET);
    }
    
    return whenArray;
}

std::shared_ptr<CHTLJSASTNode> CHTLJSParser::parseWhenItem() {
    if (!matchToken(TokenType::LEFT_BRACE)) {
        return nullptr;
    }
    
    consumeToken(TokenType::LEFT_BRACE);
    
    auto whenItem = std::make_shared<WhenItemNode>(currentToken().line, currentToken().column);
    
    while (!matchToken(TokenType::RIGHT_BRACE) && !isAtEnd()) {
        skipWhitespace();
        skipComments();
        
        if (matchToken(TokenType::RIGHT_BRACE)) break;
        
        const auto& keyToken = consumeToken(TokenType::IDENTIFIER);
        String key = keyToken.value;
        
        if (!matchToken(TokenType::COLON)) {
            reportError(CHTLJSParseErrorType::SYNTAX_ERROR, "when项缺少冒号");
            continue;
        }
        
        consumeToken(TokenType::COLON);
        
        std::shared_ptr<CHTLJSASTNode> value;
        
        if (key == "at") {
            // 时间点
            value = parseJSExpression();
        } else {
            // CSS属性
            String propValue;
            if (matchToken(TokenType::STRING_LITERAL) || matchToken(TokenType::SINGLE_STRING)) {
                propValue = parseLiteral();
            } else {
                auto valueExpr = parseJSExpression();
                if (valueExpr) {
                    propValue = valueExpr->toString();
                }
            }
            value = std::make_shared<CSSPropertyNode>(key, propValue, keyToken.line, keyToken.column);
        }
        
        if (value) {
            whenItem->addChild(value);
        }
        
        if (matchToken(TokenType::COMMA)) {
            skipToken();
        }
    }
    
    if (matchToken(TokenType::RIGHT_BRACE)) {
        consumeToken(TokenType::RIGHT_BRACE);
    }
    
    return whenItem;
}

std::shared_ptr<CHTLJSASTNode> CHTLJSParser::parseAnimationEasing() {
    String easingValue = parseLiteral();
    
    if (!isValidEasingFunction(easingValue)) {
        reportError(CHTLJSParseErrorType::INVALID_ANIMATION, "无效的缓动函数: " + easingValue);
        return nullptr;
    }
    
    return std::make_shared<EasingNode>(easingValue, currentToken().line, currentToken().column);
}

std::shared_ptr<VirtualObjectNode> CHTLJSParser::parseVirtualObject() {
    const auto& virToken = consumeToken(TokenType::VIR);
    
    const auto& nameToken = consumeToken(TokenType::IDENTIFIER);
    String virName = nameToken.value;
    
    if (!isValidVirtualObjectName(virName)) {
        reportError(CHTLJSParseErrorType::VIRTUAL_OBJECT_ERROR, "无效的虚对象名: " + virName);
        return nullptr;
    }
    
    if (!matchToken(TokenType::EQUALS)) {
        reportError(CHTLJSParseErrorType::VIRTUAL_OBJECT_ERROR, "虚对象声明缺少赋值符号");
        return nullptr;
    }
    
    consumeToken(TokenType::EQUALS);
    
    enterVirtualObjectContext(virName);
    
    // 解析虚对象赋值表达式
    auto assignment = parseVirtualObjectAssignment();
    
    exitVirtualObjectContext();
    
    if (!assignment) {
        return nullptr;
    }
    
    auto virObj = std::make_shared<VirtualObjectNode>(virName, assignment, virToken.line, virToken.column);
    
    // 注册虚对象
    VirtualObjectEntry entry;
    entry.name = virName;
    entry.functionPrefix = "chtl_vir_" + virName + "_";
    registerVirtualObject(virName, entry);
    
    return virObj;
}

std::shared_ptr<CHTLJSASTNode> CHTLJSParser::parseVirtualObjectAssignment() {
    const auto& token = currentToken();
    
    if (token.type == TokenType::LISTEN) {
        return parseListener();
    } else if (token.type == TokenType::IDENTIFIER) {
        // 可能是iNeverAway或其他函数调用
        String functionName = token.value;
        if (functionName == "iNeverAway") {
            return parseINeverAway();
        } else {
            return parseJSCall();
        }
    } else {
        return parseJSExpression();
    }
}

std::shared_ptr<CHTLJSASTNode> CHTLJSParser::parseINeverAway() {
    const auto& funcToken = consumeToken(TokenType::IDENTIFIER); // iNeverAway
    
    if (!matchToken(TokenType::LEFT_PAREN)) {
        reportError(CHTLJSParseErrorType::SYNTAX_ERROR, "iNeverAway函数缺少左括号");
        return nullptr;
    }
    
    consumeToken(TokenType::LEFT_PAREN);
    
    // 解析函数映射对象
    auto functionMap = parseINeverAwayMap();
    
    if (!matchToken(TokenType::RIGHT_PAREN)) {
        reportError(CHTLJSParseErrorType::SYNTAX_ERROR, "iNeverAway函数缺少右括号");
        return nullptr;
    }
    
    consumeToken(TokenType::RIGHT_PAREN);
    
    auto iNeverAway = std::make_shared<INeverAwayNode>(functionMap, funcToken.line, funcToken.column);
    return iNeverAway;
}

std::shared_ptr<CHTLJSASTNode> CHTLJSParser::parseINeverAwayMap() {
    if (!matchToken(TokenType::LEFT_BRACE)) {
        reportError(CHTLJSParseErrorType::SYNTAX_ERROR, "iNeverAway映射缺少左花括号");
        return nullptr;
    }
    
    consumeToken(TokenType::LEFT_BRACE);
    
    auto functionMap = std::make_shared<FunctionMapNode>(currentToken().line, currentToken().column);
    
    while (!matchToken(TokenType::RIGHT_BRACE) && !isAtEnd()) {
        skipWhitespace();
        skipComments();
        
        if (matchToken(TokenType::RIGHT_BRACE)) break;
        
        // 解析函数签名，如 "Void<A>": function(int, int) { ... }
        String signature;
        
        // 解析复杂签名
        while (!matchToken(TokenType::COLON) && !isAtEnd()) {
            signature += currentToken().value;
            skipToken();
        }
        
        if (!matchToken(TokenType::COLON)) {
            reportError(CHTLJSParseErrorType::SYNTAX_ERROR, "函数映射缺少冒号");
            continue;
        }
        
        consumeToken(TokenType::COLON);
        
        // 解析函数定义
        auto funcDef = parseJSFunction();
        if (funcDef) {
            auto funcPair = std::make_shared<FunctionPairNode>(signature, funcDef, currentToken().line, currentToken().column);
            functionMap->addChild(funcPair);
        }
        
        if (matchToken(TokenType::COMMA)) {
            skipToken();
        }
    }
    
    if (matchToken(TokenType::RIGHT_BRACE)) {
        consumeToken(TokenType::RIGHT_BRACE);
    } else {
        reportError(CHTLJSParseErrorType::SYNTAX_ERROR, "iNeverAway映射缺少右花括号");
    }
    
    return functionMap;
}

std::shared_ptr<CHTLJSASTNode> CHTLJSParser::parseVirtualObjectCall() {
    // 解析虚对象方法调用，如 Test->click()
    const auto& virToken = consumeToken(TokenType::IDENTIFIER); // 虚对象名
    String virName = virToken.value;
    
    if (!matchToken(TokenType::ARROW)) {
        reportError(CHTLJSParseErrorType::VIRTUAL_OBJECT_ERROR, "虚对象调用缺少箭头符号");
        return nullptr;
    }
    
    consumeToken(TokenType::ARROW);
    
    const auto& methodToken = consumeToken(TokenType::IDENTIFIER); // 方法名
    String methodName = methodToken.value;
    
    // 检查虚对象是否存在
    auto virObj = getVirtualObject(virName);
    if (!virObj) {
        reportError(CHTLJSParseErrorType::VIRTUAL_OBJECT_ERROR, "未定义的虚对象: " + virName);
        return nullptr;
    }
    
    // 生成实际的函数名
    String actualFunctionName = generateFunctionName(virName, methodName);
    
    auto virCall = std::make_shared<VirtualCallNode>(virName, methodName, actualFunctionName, 
                                                    virToken.line, virToken.column);
    
    // 解析参数列表（如果有）
    if (matchToken(TokenType::LEFT_PAREN)) {
        consumeToken(TokenType::LEFT_PAREN);
        
        while (!matchToken(TokenType::RIGHT_PAREN) && !isAtEnd()) {
            skipWhitespace();
            
            if (matchToken(TokenType::RIGHT_PAREN)) break;
            
            auto arg = parseJSExpression();
            if (arg) {
                virCall->addChild(arg);
            }
            
            if (matchToken(TokenType::COMMA)) {
                skipToken();
            }
        }
        
        if (matchToken(TokenType::RIGHT_PAREN)) {
            consumeToken(TokenType::RIGHT_PAREN);
        }
    }
    
    return virCall;
}

// JavaScript表达式解析
std::shared_ptr<CHTLJSASTNode> CHTLJSParser::parseJSStatement() {
    // 简化实现，解析基本的JavaScript语句
    String statement;
    
    while (!matchToken(TokenType::SEMICOLON) && !isAtEnd() && 
           !matchToken(TokenType::RIGHT_BRACE) && !matchToken(TokenType::EOF_TOKEN)) {
        statement += currentToken().value + " ";
        skipToken();
    }
    
    if (matchToken(TokenType::SEMICOLON)) {
        skipToken();
    }
    
    return std::make_shared<JSStatementNode>(statement, currentToken().line, currentToken().column);
}

std::shared_ptr<CHTLJSASTNode> CHTLJSParser::parseJSExpression() {
    // 简化实现，解析基本的JavaScript表达式
    String expression;
    int parenCount = 0;
    int braceCount = 0;
    
    while (!isAtEnd()) {
        const auto& token = currentToken();
        
        if (token.type == TokenType::LEFT_PAREN) {
            parenCount++;
        } else if (token.type == TokenType::RIGHT_PAREN) {
            parenCount--;
            if (parenCount < 0) break;
        } else if (token.type == TokenType::LEFT_BRACE) {
            braceCount++;
        } else if (token.type == TokenType::RIGHT_BRACE) {
            braceCount--;
            if (braceCount < 0) break;
        } else if ((token.type == TokenType::COMMA || token.type == TokenType::SEMICOLON) && 
                   parenCount == 0 && braceCount == 0) {
            break;
        }
        
        expression += token.value + " ";
        skipToken();
    }
    
    return std::make_shared<JSExpressionNode>(expression, currentToken().line, currentToken().column);
}

std::shared_ptr<CHTLJSASTNode> CHTLJSParser::parseJSFunction() {
    if (matchToken(TokenType::FUNCTION)) {
        consumeToken(TokenType::FUNCTION);
    }
    
    // 解析参数列表
    if (!matchToken(TokenType::LEFT_PAREN)) {
        reportError(CHTLJSParseErrorType::SYNTAX_ERROR, "函数缺少参数列表");
        return nullptr;
    }
    
    consumeToken(TokenType::LEFT_PAREN);
    
    String parameters;
    while (!matchToken(TokenType::RIGHT_PAREN) && !isAtEnd()) {
        parameters += currentToken().value + " ";
        skipToken();
    }
    
    if (matchToken(TokenType::RIGHT_PAREN)) {
        consumeToken(TokenType::RIGHT_PAREN);
    }
    
    // 解析函数体
    if (!matchToken(TokenType::LEFT_BRACE)) {
        reportError(CHTLJSParseErrorType::SYNTAX_ERROR, "函数缺少函数体");
        return nullptr;
    }
    
    String body;
    int braceCount = 1;
    consumeToken(TokenType::LEFT_BRACE);
    
    while (braceCount > 0 && !isAtEnd()) {
        const auto& token = currentToken();
        
        if (token.type == TokenType::LEFT_BRACE) {
            braceCount++;
        } else if (token.type == TokenType::RIGHT_BRACE) {
            braceCount--;
        }
        
        if (braceCount > 0) {
            body += token.value + " ";
        }
        
        skipToken();
    }
    
    auto function = std::make_shared<JSFunctionNode>(parameters, body, currentToken().line, currentToken().column);
    return function;
}

std::shared_ptr<CHTLJSASTNode> CHTLJSParser::parseArrowFunction() {
    // 解析箭头函数 () => { ... }
    consumeToken(TokenType::LEFT_PAREN);
    
    String parameters;
    while (!matchToken(TokenType::RIGHT_PAREN) && !isAtEnd()) {
        parameters += currentToken().value + " ";
        skipToken();
    }
    
    if (matchToken(TokenType::RIGHT_PAREN)) {
        consumeToken(TokenType::RIGHT_PAREN);
    }
    
    // 期望箭头符号
    if (!matchToken(TokenType::ARROW)) {
        reportError(CHTLJSParseErrorType::SYNTAX_ERROR, "箭头函数缺少箭头符号");
        return nullptr;
    }
    
    consumeToken(TokenType::ARROW);
    
    // 解析函数体
    String body;
    if (matchToken(TokenType::LEFT_BRACE)) {
        // 块级函数体
        int braceCount = 1;
        consumeToken(TokenType::LEFT_BRACE);
        
        while (braceCount > 0 && !isAtEnd()) {
            const auto& token = currentToken();
            
            if (token.type == TokenType::LEFT_BRACE) {
                braceCount++;
            } else if (token.type == TokenType::RIGHT_BRACE) {
                braceCount--;
            }
            
            if (braceCount > 0) {
                body += token.value + " ";
            }
            
            skipToken();
        }
    } else {
        // 表达式函数体
        auto expr = parseJSExpression();
        if (expr) {
            body = expr->toString();
        }
    }
    
    auto arrowFunc = std::make_shared<ArrowFunctionNode>(parameters, body, currentToken().line, currentToken().column);
    return arrowFunc;
}

std::shared_ptr<CHTLJSASTNode> CHTLJSParser::parseVariableDeclaration() {
    const auto& declToken = consumeToken(); // const, let, var
    String declType = declToken.value;
    
    const auto& nameToken = consumeToken(TokenType::IDENTIFIER);
    String varName = nameToken.value;
    
    if (!matchToken(TokenType::EQUALS)) {
        reportError(CHTLJSParseErrorType::SYNTAX_ERROR, "变量声明缺少赋值");
        return nullptr;
    }
    
    consumeToken(TokenType::EQUALS);
    
    auto value = parseJSExpression();
    
    if (matchToken(TokenType::SEMICOLON)) {
        skipToken();
    }
    
    auto varDecl = std::make_shared<VariableDeclarationNode>(declType, varName, value, 
                                                           declToken.line, declToken.column);
    return varDecl;
}

std::shared_ptr<CHTLJSASTNode> CHTLJSParser::parseJSCall() {
    const auto& nameToken = consumeToken(TokenType::IDENTIFIER);
    String functionName = nameToken.value;
    
    if (!matchToken(TokenType::LEFT_PAREN)) {
        // 不是函数调用，回退
        currentPos_--;
        return parseJSExpression();
    }
    
    consumeToken(TokenType::LEFT_PAREN);
    
    auto call = std::make_shared<JSCallNode>(functionName, nameToken.line, nameToken.column);
    
    while (!matchToken(TokenType::RIGHT_PAREN) && !isAtEnd()) {
        skipWhitespace();
        
        if (matchToken(TokenType::RIGHT_PAREN)) break;
        
        auto arg = parseJSExpression();
        if (arg) {
            call->addChild(arg);
        }
        
        if (matchToken(TokenType::COMMA)) {
            skipToken();
        }
    }
    
    if (matchToken(TokenType::RIGHT_PAREN)) {
        consumeToken(TokenType::RIGHT_PAREN);
    }
    
    return call;
}

String CHTLJSParser::parseLiteral() {
    const auto& token = currentToken();
    
    switch (token.type) {
        case TokenType::STRING_LITERAL:
        case TokenType::SINGLE_STRING:
            skipToken();
            // 移除引号
            return token.value.substr(1, token.value.length() - 2);
            
        case TokenType::UNQUOTED_LITERAL:
        case TokenType::IDENTIFIER:
        case TokenType::NUMBER:
            skipToken();
            return token.value;
            
        default:
            reportError(CHTLJSParseErrorType::SYNTAX_ERROR, "期望字面量，得到: " + token.value);
            return "";
    }
}

// Token操作实现
const Token& CHTLJSParser::currentToken() const {
    static Token eofToken{TokenType::EOF_TOKEN, "", 0, 0};
    return currentPos_ < tokens_.size() ? tokens_[currentPos_] : eofToken;
}

const Token& CHTLJSParser::peekToken(size_t offset) const {
    static Token eofToken{TokenType::EOF_TOKEN, "", 0, 0};
    size_t pos = currentPos_ + offset;
    return pos < tokens_.size() ? tokens_[pos] : eofToken;
}

Token CHTLJSParser::consumeToken() {
    if (currentPos_ < tokens_.size()) {
        return tokens_[currentPos_++];
    }
    return Token{TokenType::EOF_TOKEN, "", 0, 0};
}

Token CHTLJSParser::consumeToken(TokenType expectedType) {
    const auto& token = currentToken();
    if (token.type != expectedType) {
        reportError(CHTLJSParseErrorType::SYNTAX_ERROR, 
                   "期望 " + tokenTypeToString(expectedType) + "，得到 " + token.value);
    }
    return consumeToken();
}

bool CHTLJSParser::matchToken(TokenType type) const {
    return currentToken().type == type;
}

bool CHTLJSParser::matchAnyToken(const std::vector<TokenType>& types) const {
    const auto& currentType = currentToken().type;
    return std::find(types.begin(), types.end(), currentType) != types.end();
}

void CHTLJSParser::skipToken() {
    if (currentPos_ < tokens_.size()) {
        currentPos_++;
    }
}

bool CHTLJSParser::isAtEnd() const {
    return currentPos_ >= tokens_.size() || currentToken().type == TokenType::EOF_TOKEN;
}

void CHTLJSParser::skipWhitespace() {
    while (matchToken(TokenType::WHITESPACE) && !isAtEnd()) {
        skipToken();
    }
}

void CHTLJSParser::skipComments() {
    while ((matchToken(TokenType::COMMENT_SINGLE) || matchToken(TokenType::COMMENT_MULTI)) && !isAtEnd()) {
        skipToken();
    }
}

// 语法验证
bool CHTLJSParser::isValidSelector(const String& selector) const {
    if (selector.empty()) return false;
    
    // 基本选择器验证
    return isClassSelector(selector) || isIdSelector(selector) || 
           isTagSelector(selector) || isComplexSelector(selector);
}

bool CHTLJSParser::isClassSelector(const String& selector) const {
    return !selector.empty() && selector[0] == '.';
}

bool CHTLJSParser::isIdSelector(const String& selector) const {
    return !selector.empty() && selector[0] == '#';
}

bool CHTLJSParser::isTagSelector(const String& selector) const {
    if (selector.empty()) return false;
    
    // 检查是否为有效的HTML标签名
    std::regex tagRegex("^[a-zA-Z][a-zA-Z0-9-]*$");
    return std::regex_match(selector, tagRegex);
}

bool CHTLJSParser::isComplexSelector(const String& selector) const {
    // 检查是否包含空格（后代选择器）
    return selector.find(' ') != String::npos;
}

bool CHTLJSParser::hasIndexAccess(const String& selector) const {
    return selector.find('[') != String::npos && selector.find(']') != String::npos;
}

String CHTLJSParser::extractIndexFromSelector(const String& selector) const {
    size_t start = selector.find('[');
    size_t end = selector.find(']');
    
    if (start != String::npos && end != String::npos && end > start) {
        return selector.substr(start + 1, end - start - 1);
    }
    
    return "0";
}

bool CHTLJSParser::isValidEventType(const String& eventType) const {
    // 常见事件类型验证
    static const StringSet validEvents = {
        "click", "mouseenter", "mouseleave", "mouseover", "mouseout",
        "keydown", "keyup", "keypress", "focus", "blur", "change",
        "input", "submit", "load", "unload", "resize", "scroll"
    };
    
    return validEvents.find(eventType) != validEvents.end();
}

bool CHTLJSParser::isValidEasingFunction(const String& easing) const {
    static const StringSet validEasing = {
        "linear", "ease", "ease-in", "ease-out", "ease-in-out",
        "cubic-bezier"
    };
    
    return validEasing.find(easing) != validEasing.end() || 
           easing.find("cubic-bezier") == 0;
}

bool CHTLJSParser::isValidVirtualObjectName(const String& name) const {
    if (name.empty()) return false;
    
    // 变量名验证
    return std::all_of(name.begin(), name.end(), [](char c) {
        return std::isalnum(c) || c == '_' || c == '$';
    }) && !std::isdigit(name[0]);
}

// 虚对象管理
void CHTLJSParser::registerVirtualObject(const String& name, const VirtualObjectEntry& entry) {
    virtualObjects_[name] = entry;
    currentContext_.virtualObjectNames.insert(name);
    
    if (debugMode_) {
        debugLog("注册虚对象: " + name);
    }
}

VirtualObjectEntry* CHTLJSParser::getVirtualObject(const String& name) {
    auto it = virtualObjects_.find(name);
    return (it != virtualObjects_.end()) ? &it->second : nullptr;
}

String CHTLJSParser::generateFunctionName(const String& virName, const String& key) {
    return "chtl_vir_" + virName + "_" + key + "_" + std::to_string(functionCounter_++);
}

// 上下文管理
void CHTLJSParser::enterArrowContext() {
    currentContext_.inArrowExpression = true;
    contextStack_.push("arrow");
    
    if (debugMode_) {
        debugLog("进入箭头表达式上下文");
    }
}

void CHTLJSParser::exitArrowContext() {
    currentContext_.inArrowExpression = false;
    if (!contextStack_.empty()) {
        contextStack_.pop();
    }
    
    if (debugMode_) {
        debugLog("退出箭头表达式上下文");
    }
}

void CHTLJSParser::enterListenerContext() {
    currentContext_.inListener = true;
    contextStack_.push("listener");
    
    if (debugMode_) {
        debugLog("进入监听器上下文");
    }
}

void CHTLJSParser::exitListenerContext() {
    currentContext_.inListener = false;
    if (!contextStack_.empty()) {
        contextStack_.pop();
    }
    
    if (debugMode_) {
        debugLog("退出监听器上下文");
    }
}

void CHTLJSParser::enterDelegateContext() {
    currentContext_.inDelegate = true;
    contextStack_.push("delegate");
    
    if (debugMode_) {
        debugLog("进入委托上下文");
    }
}

void CHTLJSParser::exitDelegateContext() {
    currentContext_.inDelegate = false;
    if (!contextStack_.empty()) {
        contextStack_.pop();
    }
    
    if (debugMode_) {
        debugLog("退出委托上下文");
    }
}

void CHTLJSParser::enterAnimationContext() {
    currentContext_.inAnimation = true;
    contextStack_.push("animation");
    
    if (debugMode_) {
        debugLog("进入动画上下文");
    }
}

void CHTLJSParser::exitAnimationContext() {
    currentContext_.inAnimation = false;
    if (!contextStack_.empty()) {
        contextStack_.pop();
    }
    
    if (debugMode_) {
        debugLog("退出动画上下文");
    }
}

void CHTLJSParser::enterVirtualObjectContext(const String& virName) {
    currentContext_.inVirtualObject = true;
    contextStack_.push("virtual:" + virName);
    
    if (debugMode_) {
        debugLog("进入虚对象上下文: " + virName);
    }
}

void CHTLJSParser::exitVirtualObjectContext() {
    currentContext_.inVirtualObject = false;
    if (!contextStack_.empty()) {
        contextStack_.pop();
    }
    
    if (debugMode_) {
        debugLog("退出虚对象上下文");
    }
}

// 错误处理
void CHTLJSParser::reportError(CHTLJSParseErrorType type, const String& message, const String& context) {
    const auto& token = currentToken();
    result_.addError(type, message, token.line, token.column, context);
    
    if (debugMode_) {
        std::cerr << "CHTL JS解析错误: " << message << " (行:" << token.line << ", 列:" << token.column << ")" << std::endl;
    }
}

void CHTLJSParser::reportWarning(const String& message) {
    result_.addWarning(message);
    
    if (debugMode_) {
        std::cerr << "CHTL JS解析警告: " << message << std::endl;
    }
}

void CHTLJSParser::debugLog(const String& message) const {
    if (debugMode_) {
        std::cout << "[CHTL JS Parser] " << message << std::endl;
    }
}

// 工厂方法实现
std::unique_ptr<CHTLJSParser> CHTLJSParserFactory::createParser(const CompilerOptions& options) {
    return std::make_unique<CHTLJSParser>(options);
}

std::unique_ptr<CHTLJSParser> CHTLJSParserFactory::createDebugParser() {
    CompilerOptions debugOptions;
    debugOptions.debugMode = true;
    return std::make_unique<CHTLJSParser>(debugOptions);
}

} // namespace chtl