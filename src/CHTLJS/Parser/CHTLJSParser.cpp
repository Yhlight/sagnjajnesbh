#include "CHTLJS/Parser/CHTLJSParser.h"
#include "Utils/ErrorHandler.h"
#include <algorithm>

namespace CHTL {
namespace CHTLJS {
namespace Parser {

CHTLJSParser::CHTLJSParser(Core::CHTLJSState& stateManager, const ParserConfig& config)
    : config_(config), stateManager_(stateManager), tokens_(nullptr), nodeCount_(0) {}

AST::ASTNodePtr CHTLJSParser::Parse(Core::CHTLJSTokenStream& tokens, const std::string& fileName) {
    tokens_ = &tokens;
    fileName_ = fileName;
    nodeCount_ = 0;
    context_ = ParseContext();
    
    // 重置状态管理器
    stateManager_.Reset();
    
    // 创建根节点
    auto root = std::make_shared<AST::RootNode>();
    
    // 解析所有语句
    while (!IsAtEnd()) {
        SkipWhitespaceAndComments();
        
        if (IsAtEnd()) {
            break;
        }
        
        auto statement = ParseStatement();
        if (statement) {
            root->AddChild(statement);
            nodeCount_++;
        }
    }
    
    if (config_.enableDebug) {
        Utils::ErrorHandler::GetInstance().LogInfo(
            "CHTL JS解析完成，生成了 " + std::to_string(nodeCount_) + " 个AST节点"
        );
    }
    
    return root;
}

AST::ASTNodePtr CHTLJSParser::ParseStatement() {
    const auto& token = Current();
    
    switch (token.GetType()) {
        case Core::TokenType::VIR:
            return ParseVirtualObject();
            
        // 错误的Token已移除：CONST、LET、VAR不是CHTL JS的内容
            return ParseVariableDeclaration();
            
        case Core::TokenType::ENHANCED_SELECTOR:
            return ParseExpression(); // 增强选择器作为表达式处理
            
        case Core::TokenType::COMMENT:
            {
                std::string content = token.GetValue();
                Advance();
                return std::make_shared<AST::CommentNode>(content, token);
            }
            
        default:
            return ParseExpression();
    }
}

AST::ASTNodePtr CHTLJSParser::ParseExpression() {
    return ParseAssignmentExpression();
}

AST::ASTNodePtr CHTLJSParser::ParsePrimaryExpression() {
    const auto& token = Current();
    
    switch (token.GetType()) {
        case Core::TokenType::ENHANCED_SELECTOR:
            return ParseEnhancedSelector();
            
        case Core::TokenType::LISTEN:
            return ParseListenBlock();
            
        case Core::TokenType::DELEGATE:
            return ParseDelegateBlock();
            
        case Core::TokenType::ANIMATE:
            return ParseAnimateBlock();
            
        // 错误的Token已移除：I_NEVER_AWAY、FUNCTION不是CHTL JS的内容
            // return ParseINeverAwayBlock();  // 方法已移除
            // return ParseFunctionDefinition(); // 方法已移除
            
        case Core::TokenType::LEFT_PAREN:
            // 检查是否为箭头函数的参数
            if (IsArrowFunction()) {
                return ParseArrowFunction();
            }
            {
                Advance(); // 消费 (
                auto expr = ParseExpression();
                if (!Consume(Core::TokenType::RIGHT_PAREN, "期望 ')'")) {
                    return nullptr;
                }
                return expr;
            }
            
        case Core::TokenType::LEFT_BRACE:
            return ParseObjectLiteral();
            
        case Core::TokenType::LEFT_BRACKET:
            return ParseArrayLiteral();
            
        case Core::TokenType::IDENTIFIER:
            {
                std::string name = token.GetValue();
                Advance();
                return std::make_shared<AST::IdentifierNode>(name, token);
            }
            
        case Core::TokenType::STRING:
            {
                std::string value = token.GetValue();
                Advance();
                return std::make_shared<AST::LiteralNode>(AST::LiteralNode::LiteralType::STRING, value, token);
            }
            
        case Core::TokenType::NUMBER:
            {
                std::string value = token.GetValue();
                Advance();
                return std::make_shared<AST::LiteralNode>(AST::LiteralNode::LiteralType::NUMBER, value, token);
            }
            

            
        default:
            ReportError("意外的Token: " + token.GetValue());
            return nullptr;
    }
}

AST::ASTNodePtr CHTLJSParser::ParseEnhancedSelector() {
    const auto& token = Current();
    
    if (!Consume(Core::TokenType::ENHANCED_SELECTOR, "期望增强选择器")) {
        return nullptr;
    }
    
    // 使用状态保护
    Core::CHTLJSStateGuard guard(stateManager_, Core::CompileState::PARSING_ENHANCED_SELECTOR);
    
    auto selectorNode = std::make_shared<AST::EnhancedSelectorNode>(token.GetValue(), token);
    
    // 更新上下文
    context_.currentSelector = token.GetValue();
    stateManager_.EnterSelector(token.GetValue());
    
    return selectorNode;
}

AST::ASTNodePtr CHTLJSParser::ParseVirtualObject() {
    if (!Consume(Core::TokenType::VIR, "期望 'vir'")) {
        return nullptr;
    }
    
    // 使用状态保护
    Core::CHTLJSStateGuard guard(stateManager_, Core::CompileState::PARSING_VIRTUAL_OBJECT);
    
    // 解析虚对象名称
    std::string objectName = ParseIdentifier();
    if (objectName.empty()) {
        ReportError("期望虚对象名称");
        return nullptr;
    }
    
    auto virtualObjectNode = std::make_shared<AST::VirtualObjectNode>(objectName, Current());
    
    // 更新上下文
    context_.currentVirtualObject = objectName;
    context_.virtualObjects.push_back(objectName);
    stateManager_.EnterVirtualObject(objectName);
    
    // 解析赋值
    if (Consume(Core::TokenType::EQUAL, "期望 '='")) {
        auto assignment = ParseExpression();
        if (assignment) {
            virtualObjectNode->SetAssignment(assignment);
        }
    }
    
    // 消费可选的分号
    if (Check(Core::TokenType::SEMICOLON)) {
        Advance();
    }
    
    return virtualObjectNode;
}

AST::ASTNodePtr CHTLJSParser::ParseListenBlock() {
    if (!Consume(Core::TokenType::LISTEN, "期望 'listen'")) {
        return nullptr;
    }
    
    // 使用状态保护
    Core::CHTLJSStateGuard guard(stateManager_, Core::CompileState::PARSING_LISTEN_BLOCK);
    
    auto listenNode = std::make_shared<AST::ListenBlockNode>(Current());
    
    // 解析监听器对象
    if (!Consume(Core::TokenType::LEFT_PAREN, "期望 '('")) {
        return nullptr;
    }
    
    if (!Consume(Core::TokenType::LEFT_BRACE, "期望 '{'")) {
        return nullptr;
    }
    
    // 解析事件处理器
    while (!IsAtEnd() && !Check(Core::TokenType::RIGHT_BRACE)) {
        SkipWhitespaceAndComments();
        
        if (IsAtEnd() || Check(Core::TokenType::RIGHT_BRACE)) {
            break;
        }
        
        // 解析事件类型
        std::string eventType = ParseIdentifier();
        if (eventType.empty()) {
            ReportError("期望事件类型");
            break;
        }
        
        if (!Consume(Core::TokenType::COLON, "期望 ':'")) {
            break;
        }
        
        // 解析事件处理器
        auto handler = ParseExpression();
        if (handler) {
            listenNode->AddEventHandler(eventType, handler);
        }
        
        // 消费可选的逗号
        if (Check(Core::TokenType::COMMA)) {
            Advance();
        }
    }
    
    if (!Consume(Core::TokenType::RIGHT_BRACE, "期望 '}'")) {
        return nullptr;
    }
    
    if (!Consume(Core::TokenType::RIGHT_PAREN, "期望 ')'")) {
        return nullptr;
    }
    
    // 消费可选的分号
    if (Check(Core::TokenType::SEMICOLON)) {
        Advance();
    }
    
    return listenNode;
}

AST::ASTNodePtr CHTLJSParser::ParseDelegateBlock() {
    if (!Consume(Core::TokenType::DELEGATE, "期望 'delegate'")) {
        return nullptr;
    }
    
    // 使用状态保护
    Core::CHTLJSStateGuard guard(stateManager_, Core::CompileState::PARSING_DELEGATE_BLOCK);
    
    auto delegateNode = std::make_shared<AST::DelegateBlockNode>(Current());
    
    // 解析委托对象
    if (!Consume(Core::TokenType::LEFT_PAREN, "期望 '('")) {
        return nullptr;
    }
    
    if (!Consume(Core::TokenType::LEFT_BRACE, "期望 '{'")) {
        return nullptr;
    }
    
    // 解析委托配置
    while (!IsAtEnd() && !Check(Core::TokenType::RIGHT_BRACE)) {
        SkipWhitespaceAndComments();
        
        if (IsAtEnd() || Check(Core::TokenType::RIGHT_BRACE)) {
            break;
        }
        
        std::string key = ParseIdentifier();
        if (key.empty()) {
            ReportError("期望属性名");
            break;
        }
        
        if (!Consume(Core::TokenType::COLON, "期望 ':'")) {
            break;
        }
        
        if (key == "target") {
            // 解析目标选择器
            auto target = ParseExpression();
            if (target) {
                delegateNode->SetTarget(target);
            }
        } else {
            // 解析事件处理器
            auto handler = ParseExpression();
            if (handler) {
                delegateNode->AddEventHandler(key, handler);
            }
        }
        
        // 消费可选的逗号
        if (Check(Core::TokenType::COMMA)) {
            Advance();
        }
    }
    
    if (!Consume(Core::TokenType::RIGHT_BRACE, "期望 '}'")) {
        return nullptr;
    }
    
    if (!Consume(Core::TokenType::RIGHT_PAREN, "期望 ')'")) {
        return nullptr;
    }
    
    // 消费可选的分号
    if (Check(Core::TokenType::SEMICOLON)) {
        Advance();
    }
    
    return delegateNode;
}

AST::ASTNodePtr CHTLJSParser::ParseAnimateBlock() {
    if (!Consume(Core::TokenType::ANIMATE, "期望 'animate'")) {
        return nullptr;
    }
    
    // 使用状态保护
    Core::CHTLJSStateGuard guard(stateManager_, Core::CompileState::PARSING_ANIMATE_BLOCK);
    
    auto animateNode = std::make_shared<AST::AnimateBlockNode>(Current());
    
    // 解析动画对象
    if (!Consume(Core::TokenType::LEFT_PAREN, "期望 '('")) {
        return nullptr;
    }
    
    if (!Consume(Core::TokenType::LEFT_BRACE, "期望 '{'")) {
        return nullptr;
    }
    
    // 解析动画配置
    while (!IsAtEnd() && !Check(Core::TokenType::RIGHT_BRACE)) {
        SkipWhitespaceAndComments();
        
        if (IsAtEnd() || Check(Core::TokenType::RIGHT_BRACE)) {
            break;
        }
        
        std::string key = ParseIdentifier();
        if (key.empty()) {
            ReportError("期望动画属性名");
            break;
        }
        
        if (!Consume(Core::TokenType::COLON, "期望 ':'")) {
            break;
        }
        
        if (key == "target") {
            auto target = ParseExpression();
            if (target) {
                animateNode->SetTarget(target);
            }
        } else if (key == "duration") {
            std::string durationStr = ParseNumberValue();
            if (!durationStr.empty()) {
                animateNode->SetDuration(std::stoi(durationStr));
            }
        } else if (key == "easing") {
            std::string easing = ParseStringValue();
            if (!easing.empty()) {
                animateNode->SetEasing(easing);
            }
        } else if (key == "begin") {
            auto begin = ParseObjectLiteral();
            if (begin) {
                animateNode->SetBegin(begin);
            }
        } else if (key == "end") {
            auto end = ParseObjectLiteral();
            if (end) {
                animateNode->SetEnd(end);
            }
        } else if (key == "when") {
            // when关键帧解析 - 应该解析CHTL JS动画关键帧语法，不是JavaScript数组
            // 语法文档第1247-1261行描述了when关键帧的语法
            
            // 简化实现：跳过when解析，因为需要实现专门的CHTL JS关键帧解析
            // 跳过到下一个逗号或右大括号
            while (!IsAtEnd() && !Check(Core::TokenType::COMMA) && !Check(Core::TokenType::RIGHT_BRACE)) {
                Advance();
            }
            if (Check(Core::TokenType::COMMA)) {
                Advance();
            }
            Utils::ErrorHandler::GetInstance().LogWarning(
                "when关键帧解析暂时跳过，需要实现CHTL JS关键帧语法解析"
            );
        } else if (key == "loop") {
            std::string loopStr = ParseNumberValue();
            if (!loopStr.empty()) {
                animateNode->SetLoop(std::stoi(loopStr));
            }
        } else if (key == "delay") {
            std::string delayStr = ParseNumberValue();
            if (!delayStr.empty()) {
                animateNode->SetDelay(std::stoi(delayStr));
            }
        } else if (key == "callback") {
            auto callback = ParseExpression();
            if (callback) {
                animateNode->SetCallback(callback);
            }
        }
        
        // 消费可选的逗号
        if (Check(Core::TokenType::COMMA)) {
            Advance();
        }
    }
    
    if (!Consume(Core::TokenType::RIGHT_BRACE, "期望 '}'")) {
        return nullptr;
    }
    
    if (!Consume(Core::TokenType::RIGHT_PAREN, "期望 ')'")) {
        return nullptr;
    }
    
    // 消费可选的分号
    if (Check(Core::TokenType::SEMICOLON)) {
        Advance();
    }
    
    return animateNode;
}

AST::ASTNodePtr CHTLJSParser::ParseINeverAwayBlock() {
    // 错误的Token已移除：I_NEVER_AWAY不是CHTL JS的内容
    // CHTL JS不直接支持iNeverAway关键字，应该使用CJMOD扩展
    
    ReportError("CHTL JS不支持iNeverAway关键字，请使用CJMOD扩展");
    return nullptr;
}

AST::ASTNodePtr CHTLJSParser::ParseVariableDeclaration() {
    const auto& token = Current();
    
    // 错误的Token已移除：CONST、LET、VAR不是CHTL JS的内容
    // CHTL JS不直接支持这些JavaScript关键字
    // 变量声明应该通过其他CHTL JS语法实现
    
    ReportError("CHTL JS不支持直接的变量声明关键字，请使用CHTL JS语法");
    return nullptr;
}

AST::ASTNodePtr CHTLJSParser::ParseAssignmentExpression() {
    auto expr = ParsePrimaryExpression();
    if (!expr) {
        return nullptr;
    }
    
    // 检查方法调用
    while (Check(Core::TokenType::DOT) || Check(Core::TokenType::ARROW)) {
        bool isArrow = Current().GetType() == Core::TokenType::ARROW;
        Advance(); // 消费操作符
        
        if (Check(Core::TokenType::IDENTIFIER)) {
            std::string methodName = ParseIdentifier();
            // MethodCallNode已移除 - 方法调用是JavaScript语法，不属于CHTL JS核心
            ReportError("方法调用语法不属于CHTL JS核心，属于JavaScript语法");
            return nullptr;
            
            // 解析参数
            if (Check(Core::TokenType::LEFT_PAREN)) {
                Advance(); // 消费 (
                
                while (!IsAtEnd() && !Check(Core::TokenType::RIGHT_PAREN)) {
                    SkipWhitespaceAndComments();
                    
                    if (Check(Core::TokenType::RIGHT_PAREN)) {
                        break;
                    }
                    
                    auto arg = ParseExpression();
                    if (arg) {
                        methodCall->AddArgument(arg);
                    }
                    
                    if (Check(Core::TokenType::COMMA)) {
                        Advance();
                    } else {
                        break;
                    }
                }
                
                if (!Consume(Core::TokenType::RIGHT_PAREN, "期望 ')'")) {
                    return nullptr;
                }
            }
            
            expr = methodCall;
        }
    }
    
    // 检查赋值
    if (Check(Core::TokenType::EQUAL)) {
        Advance(); // 消费 =
        auto right = ParseExpression();
        if (right) {
            // AssignmentExpressionNode已移除 - 赋值表达式是JavaScript语法，不属于CHTL JS
            // expr = std::make_shared<AST::AssignmentExpressionNode>(expr, right, Current());
        }
    }
    
    return expr;
}

AST::ASTNodePtr CHTLJSParser::ParseObjectLiteral() {
    // ParseObjectLiteral已移除 - 对象字面量是JavaScript语法，不属于CHTL JS
    // 语法文档第1100行明确说明"CHTL JS不支持JS的语法"
    
    ReportError("CHTL JS不支持对象字面量语法，请使用CHTL JS语法");
    return nullptr;
}

AST::ASTNodePtr CHTLJSParser::ParseArrayLiteral() {
    // ParseArrayLiteral已移除 - 数组字面量是JavaScript语法，不属于CHTL JS
    // 语法文档第1100行明确说明"CHTL JS不支持JS的语法"
    
    ReportError("CHTL JS不支持数组字面量语法，请使用CHTL JS语法");
    return nullptr;
}

AST::ASTNodePtr CHTLJSParser::ParseArrowFunction() {
    auto arrowFunc = std::make_shared<AST::ArrowFunctionNode>(Current());
    
    // 解析参数
    if (Check(Core::TokenType::LEFT_PAREN)) {
        Advance(); // 消费 (
        
        while (!IsAtEnd() && !Check(Core::TokenType::RIGHT_PAREN)) {
            std::string param = ParseIdentifier();
            if (!param.empty()) {
                arrowFunc->AddParameter(param);
            }
            
            if (Check(Core::TokenType::COMMA)) {
                Advance();
            } else {
                break;
            }
        }
        
        if (!Consume(Core::TokenType::RIGHT_PAREN, "期望 ')'")) {
            return nullptr;
        }
    } else {
        // 单参数箭头函数
        std::string param = ParseIdentifier();
        if (!param.empty()) {
            arrowFunc->AddParameter(param);
        }
    }
    
    if (!Consume(Core::TokenType::ARROW, "期望 '=>'")) {
        return nullptr;
    }
    
    // 解析函数体
    auto body = ParseExpression();
    if (body) {
        arrowFunc->SetBody(body);
    }
    
    return arrowFunc;
}

// 辅助方法实现
bool CHTLJSParser::Check(Core::TokenType type) const {
    if (IsAtEnd()) return false;
    return Current().GetType() == type;
}

bool CHTLJSParser::Match(Core::TokenType type) {
    if (Check(type)) {
        Advance();
        return true;
    }
    return false;
}

bool CHTLJSParser::Consume(Core::TokenType type, const std::string& message) {
    if (Check(type)) {
        Advance();
        return true;
    }
    
    if (!message.empty()) {
        ReportError(message);
    }
    return false;
}

void CHTLJSParser::Advance() {
    if (!IsAtEnd()) {
        tokens_->Advance();
    }
}

const Core::CHTLJSToken& CHTLJSParser::Current() const {
    return tokens_->Current();
}

const Core::CHTLJSToken& CHTLJSParser::Previous() const {
    // 简化实现，实际应该维护previous token
    return tokens_->Current();
}

const Core::CHTLJSToken& CHTLJSParser::Peek() const {
    return tokens_->Peek();
}

bool CHTLJSParser::IsAtEnd() const {
    return tokens_->IsAtEnd() || Current().GetType() == Core::TokenType::END_OF_FILE;
}

void CHTLJSParser::ReportError(const std::string& message) {
    const auto& token = Current();
    Utils::ErrorHandler::GetInstance().LogError(
        "CHTL JS语法错误: " + message + " 在 " + fileName_ + ":" + 
        std::to_string(token.GetLine()) + ":" + std::to_string(token.GetColumn()) +
        " (Token: " + token.GetValue() + ")"
    );
}

void CHTLJSParser::Synchronize() {
    Advance();
    
    while (!IsAtEnd()) {
        if (Previous().GetType() == Core::TokenType::SEMICOLON) return;
        
        switch (Current().GetType()) {
            case Core::TokenType::VIR:
            // 错误的Token已移除：CONST、LET、VAR、FUNCTION不是CHTL JS的内容
                return;
            default:
                break;
        }
        
        Advance();
    }
}

std::string CHTLJSParser::ParseIdentifier() {
    if (Check(Core::TokenType::IDENTIFIER)) {
        std::string value = Current().GetValue();
        Advance();
        return value;
    }
    return "";
}

std::string CHTLJSParser::ParseStringValue() {
    if (Check(Core::TokenType::STRING)) {
        std::string value = Current().GetValue();
        Advance();
        return value;
    }
    return "";
}

std::string CHTLJSParser::ParseNumberValue() {
    if (Check(Core::TokenType::NUMBER)) {
        std::string value = Current().GetValue();
        Advance();
        return value;
    }
    return "";
}

void CHTLJSParser::SkipWhitespaceAndComments() {
    while (!IsAtEnd() && 
           (Current().GetType() == Core::TokenType::WHITESPACE || 
            Current().GetType() == Core::TokenType::NEWLINE ||
            Current().GetType() == Core::TokenType::COMMENT)) {
        Advance();
    }
}

AST::ASTNodePtr CHTLJSParser::ParseFunctionDefinition() {
    // 错误的Token已移除：FUNCTION不是CHTL JS的内容
    // CHTL JS不直接支持function关键字，应该使用箭头函数或其他CHTL JS语法
    
    ReportError("CHTL JS不支持function关键字，请使用箭头函数语法");
    return nullptr;
    
    std::string functionName = ParseIdentifier();
    if (functionName.empty()) {
        ReportError("期望函数名");
        return nullptr;
    }
    
    // 整个方法体已重写 - function是JavaScript语法，不属于CHTL JS
    // 语法文档第1100行明确说明"CHTL JS不支持JS的语法"
    
    ReportError("CHTL JS不支持function语法，请使用箭头函数语法");
    return nullptr;
}

AST::ASTNodePtr CHTLJSParser::ParseMethodCall(AST::ASTNodePtr object) {
    // 这个方法在ParseAssignmentExpression中已经内联实现
    return nullptr;
}

AST::ASTNodePtr CHTLJSParser::ParseVirtualMethodCall() {
    // ParseVirtualMethodCall已移除 - 虚对象方法调用属于CJMOD扩展，不属于CHTL JS核心
    // 语法文档第1485行明确说明虚对象调用属于CJMOD扩展
    
    ReportError("虚对象方法调用属于CJMOD扩展，不属于CHTL JS核心语法");
    return nullptr;
}

AST::ASTNodePtr CHTLJSParser::ParseAnimationKeyframe() {
    // 解析动画关键帧 { at: 0.5, opacity: 0.5 }
    if (!Consume(Core::TokenType::LEFT_BRACE, "期望 '{'")) {
        return nullptr;
    }
    
    double time = 0.0;
    auto keyframe = std::make_shared<AST::AnimationKeyframeNode>(time, Current());
    
    while (!IsAtEnd() && !Check(Core::TokenType::RIGHT_BRACE)) {
        SkipWhitespaceAndComments();
        
        if (Check(Core::TokenType::RIGHT_BRACE)) {
            break;
        }
        
        std::string key = ParseIdentifier();
        if (key.empty()) {
            break;
        }
        
        if (!Consume(Core::TokenType::COLON, "期望 ':'")) {
            break;
        }
        
        if (key == "at") {
            std::string timeStr = ParseNumberValue();
            if (!timeStr.empty()) {
                time = std::stod(timeStr);
                // 更新关键帧时间（需要重新创建节点）
                keyframe = std::make_shared<AST::AnimationKeyframeNode>(time, Current());
            }
        } else {
            // CSS属性
            std::string value = ParseStringValue();
            if (!value.empty()) {
                keyframe->AddCSSProperty(key, value);
            }
        }
        
        if (Check(Core::TokenType::COMMA)) {
            Advance();
        }
    }
    
    if (!Consume(Core::TokenType::RIGHT_BRACE, "期望 '}'")) {
        return nullptr;
    }
    
    return keyframe;
}

AST::ASTNodePtr CHTLJSParser::ParseAnimationProperty() {
    // 简化实现，返回标识符节点
    return ParsePrimaryExpression();
}

AST::ASTNodePtr CHTLJSParser::ParsePropertyAccess(AST::ASTNodePtr object) {
    // ParsePropertyAccess已移除 - 属性访问是JavaScript语法，不属于CHTL JS
    // 语法文档第1100行明确说明"CHTL JS不支持JS的语法"
    // 应该使用->箭头操作符，这是CHTL JS的语法
    
    ReportError("CHTL JS不支持.属性访问语法，请使用->箭头操作符");
    return nullptr;
}

bool CHTLJSParser::IsArrowFunction() const {
    // 简单检查：查看是否有 ) => 模式
    size_t currentPos = tokens_->GetPosition();
    
    // 向前扫描寻找 =>
    for (int i = 1; i < 10 && currentPos + i < tokens_->Size(); ++i) {
        const auto& token = tokens_->Peek(i);
        if (token.GetType() == Core::TokenType::ARROW) {
            return true;
        }
        if (token.GetType() == Core::TokenType::LEFT_BRACE || 
            token.GetType() == Core::TokenType::SEMICOLON) {
            break;
        }
    }
    
    return false;
}

} // namespace Parser
} // namespace CHTLJS
} // namespace CHTL
