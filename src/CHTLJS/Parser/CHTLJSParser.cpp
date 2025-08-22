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
            
        case Core::TokenType::CONST:
        case Core::TokenType::LET:
        case Core::TokenType::VAR:
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
            
        case Core::TokenType::I_NEVER_AWAY:
            return ParseINeverAwayBlock();
            
        case Core::TokenType::FUNCTION:
            return ParseFunctionDefinition();
            
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
            
        case Core::TokenType::LEFT_PAREN:
            {
                Advance(); // 消费 (
                auto expr = ParseExpression();
                if (!Consume(Core::TokenType::RIGHT_PAREN, "期望 ')'")) {
                    return nullptr;
                }
                return expr;
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
            auto keyframes = ParseArrayLiteral();
            if (keyframes) {
                auto arrayNode = std::dynamic_pointer_cast<AST::ArrayLiteralNode>(keyframes);
                if (arrayNode) {
                    for (const auto& element : arrayNode->GetElements()) {
                        animateNode->AddKeyframe(element);
                    }
                }
            }
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
    if (!Consume(Core::TokenType::I_NEVER_AWAY, "期望 'iNeverAway'")) {
        return nullptr;
    }
    
    // 使用状态保护
    Core::CHTLJSStateGuard guard(stateManager_, Core::CompileState::PARSING_I_NEVER_AWAY);
    
    auto iNeverAwayNode = std::make_shared<AST::INeverAwayBlockNode>(Current());
    
    // 解析iNeverAway对象
    if (!Consume(Core::TokenType::LEFT_PAREN, "期望 '('")) {
        return nullptr;
    }
    
    if (!Consume(Core::TokenType::LEFT_BRACE, "期望 '{'")) {
        return nullptr;
    }
    
    // 解析方法定义
    while (!IsAtEnd() && !Check(Core::TokenType::RIGHT_BRACE)) {
        SkipWhitespaceAndComments();
        
        if (IsAtEnd() || Check(Core::TokenType::RIGHT_BRACE)) {
            break;
        }
        
        std::string methodName = ParseIdentifier();
        if (methodName.empty()) {
            ReportError("期望方法名");
            break;
        }
        
        if (!Consume(Core::TokenType::COLON, "期望 ':'")) {
            break;
        }
        
        auto methodDef = ParseExpression();
        if (methodDef) {
            // 检查是否为Void<State>方法
            if (methodName.find("Void<") == 0 && methodName.back() == '>') {
                std::string state = methodName.substr(5, methodName.length() - 6);
                iNeverAwayNode->AddVoidMethod(state, methodDef);
            } else if (methodName == "Void") {
                iNeverAwayNode->AddVoidMethod("", methodDef); // 默认Void
            } else {
                iNeverAwayNode->AddCustomMethod(methodName, methodDef);
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
    
    return iNeverAwayNode;
}

AST::ASTNodePtr CHTLJSParser::ParseVariableDeclaration() {
    const auto& token = Current();
    
    AST::VariableDeclarationNode::DeclarationType declType;
    if (token.GetType() == Core::TokenType::CONST) {
        declType = AST::VariableDeclarationNode::DeclarationType::CONST;
    } else if (token.GetType() == Core::TokenType::LET) {
        declType = AST::VariableDeclarationNode::DeclarationType::LET;
    } else if (token.GetType() == Core::TokenType::VAR) {
        declType = AST::VariableDeclarationNode::DeclarationType::VAR;
    } else {
        ReportError("期望变量声明关键字");
        return nullptr;
    }
    
    Advance(); // 消费关键字
    
    std::string varName = ParseIdentifier();
    if (varName.empty()) {
        ReportError("期望变量名");
        return nullptr;
    }
    
    auto varDecl = std::make_shared<AST::VariableDeclarationNode>(declType, varName, token);
    
    // 解析初始化器
    if (Consume(Core::TokenType::EQUAL, "")) {
        auto initializer = ParseExpression();
        if (initializer) {
            varDecl->SetInitializer(initializer);
        }
    }
    
    // 消费可选的分号
    if (Check(Core::TokenType::SEMICOLON)) {
        Advance();
    }
    
    // 更新上下文
    context_.variables[varName] = "declared";
    
    return varDecl;
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
            auto methodCall = std::make_shared<AST::MethodCallNode>(expr, methodName, Current());
            methodCall->SetIsArrowCall(isArrow);
            
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
            expr = std::make_shared<AST::AssignmentExpressionNode>(expr, right, Current());
        }
    }
    
    return expr;
}

AST::ASTNodePtr CHTLJSParser::ParseObjectLiteral() {
    if (!Consume(Core::TokenType::LEFT_BRACE, "期望 '{'")) {
        return nullptr;
    }
    
    auto objectNode = std::make_shared<AST::ObjectLiteralNode>(Current());
    
    while (!IsAtEnd() && !Check(Core::TokenType::RIGHT_BRACE)) {
        SkipWhitespaceAndComments();
        
        if (Check(Core::TokenType::RIGHT_BRACE)) {
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
        
        auto value = ParseExpression();
        if (value) {
            objectNode->AddProperty(key, value);
        }
        
        if (Check(Core::TokenType::COMMA)) {
            Advance();
        } else {
            break;
        }
    }
    
    if (!Consume(Core::TokenType::RIGHT_BRACE, "期望 '}'")) {
        return nullptr;
    }
    
    return objectNode;
}

AST::ASTNodePtr CHTLJSParser::ParseArrayLiteral() {
    if (!Consume(Core::TokenType::LEFT_BRACKET, "期望 '['")) {
        return nullptr;
    }
    
    auto arrayNode = std::make_shared<AST::ArrayLiteralNode>(Current());
    
    while (!IsAtEnd() && !Check(Core::TokenType::RIGHT_BRACKET)) {
        SkipWhitespaceAndComments();
        
        if (Check(Core::TokenType::RIGHT_BRACKET)) {
            break;
        }
        
        auto element = ParseExpression();
        if (element) {
            arrayNode->AddElement(element);
        }
        
        if (Check(Core::TokenType::COMMA)) {
            Advance();
        } else {
            break;
        }
    }
    
    if (!Consume(Core::TokenType::RIGHT_BRACKET, "期望 ']'")) {
        return nullptr;
    }
    
    return arrayNode;
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
            case Core::TokenType::CONST:
            case Core::TokenType::LET:
            case Core::TokenType::VAR:
            case Core::TokenType::FUNCTION:
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
    if (!Consume(Core::TokenType::FUNCTION, "期望 'function'")) {
        return nullptr;
    }
    
    std::string functionName = ParseIdentifier();
    if (functionName.empty()) {
        ReportError("期望函数名");
        return nullptr;
    }
    
    auto funcNode = std::make_shared<AST::FunctionDefinitionNode>(functionName, Current());
    
    // 解析参数
    if (!Consume(Core::TokenType::LEFT_PAREN, "期望 '('")) {
        return nullptr;
    }
    
    while (!IsAtEnd() && !Check(Core::TokenType::RIGHT_PAREN)) {
        std::string param = ParseIdentifier();
        if (!param.empty()) {
            funcNode->AddParameter(param);
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
    
    // 解析函数体
    auto body = ParseObjectLiteral();
    if (body) {
        funcNode->SetBody(body);
    }
    
    return funcNode;
}

AST::ASTNodePtr CHTLJSParser::ParseMethodCall(AST::ASTNodePtr object) {
    // 这个方法在ParseAssignmentExpression中已经内联实现
    return nullptr;
}

AST::ASTNodePtr CHTLJSParser::ParseVirtualMethodCall() {
    // 解析虚对象方法调用 object->method()
    std::string objectName = ParseIdentifier();
    if (objectName.empty()) {
        return nullptr;
    }
    
    if (!Consume(Core::TokenType::ARROW, "期望 '->'")) {
        return nullptr;
    }
    
    std::string methodName = ParseIdentifier();
    if (methodName.empty()) {
        return nullptr;
    }
    
    auto virtualCall = std::make_shared<AST::VirtualMethodCallNode>(objectName, methodName, Current());
    
    // 检查是否为Void<State>调用
    if (methodName.find("Void<") == 0 && methodName.back() == '>') {
        std::string state = methodName.substr(5, methodName.length() - 6);
        virtualCall->SetIsVoidStateCall(true);
        virtualCall->SetVoidState(state);
    }
    
    // 解析参数
    if (Check(Core::TokenType::LEFT_PAREN)) {
        Advance(); // 消费 (
        
        while (!IsAtEnd() && !Check(Core::TokenType::RIGHT_PAREN)) {
            auto arg = ParseExpression();
            if (arg) {
                virtualCall->AddArgument(arg);
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
    
    return virtualCall;
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
    if (!Consume(Core::TokenType::DOT, "期望 '.'")) {
        return nullptr;
    }
    
    std::string property = ParseIdentifier();
    if (property.empty()) {
        return nullptr;
    }
    
    return std::make_shared<AST::PropertyAccessNode>(object, property, Current());
}

} // namespace Parser
} // namespace CHTLJS
} // namespace CHTL
