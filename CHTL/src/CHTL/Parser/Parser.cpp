#include "CHTL/Parser/Parser.h"
#include "CHTL/AST/TemplateNodes.h"
#include "CHTL/AST/SpecialNodes.h"
#include "CHTL/AST/StyleNodes.h"
#include "Utils/FileUtils.h"
#include "Utils/Logger.h"
#include <sstream>
#include <algorithm>

namespace CHTL {

// Parser构造函数
Parser::Parser() : Parser(ParserConfig{}) {}

Parser::Parser(const ParserConfig& config) 
    : config_(config),
      lexer_(std::make_unique<Lexer>()),
      globalMap_(std::make_shared<GlobalMap>()),
      stateMachine_(std::make_shared<StateMachine>()),
      contextManager_(std::make_shared<ContextManager>()) {
    
    // 设置组件之间的关联
    contextManager_->SetGlobalMap(globalMap_);
    contextManager_->SetStateMachine(stateMachine_);
    
    if (config_.debugMode) {
        LexerConfig lexerConfig;
        lexerConfig.debugMode = true;
        lexer_->SetConfig(lexerConfig);
    }
}

Parser::~Parser() = default;

// 解析字符串输入
std::shared_ptr<AST::ProgramNode> Parser::Parse(const std::string& input, const std::string& filename) {
    currentFilename_ = filename;
    errors_.clear();
    
    // 设置词法分析器输入
    lexer_->SetInput(input, filename);
    
    // 获取第一个Token
    currentToken_ = lexer_->NextToken();
    
    // 解析程序
    auto program = ParseProgram();
    
    // 验证是否到达文件末尾
    if (!IsAtEnd()) {
        ReportError("意外的Token，期望文件结束");
    }
    
    return program;
}

// 解析文件
std::shared_ptr<AST::ProgramNode> Parser::ParseFile(const std::string& filename) {
    std::string content = Utils::FileUtils::ReadFile(filename);
    return Parse(content, filename);
}

// 解析程序
std::shared_ptr<AST::ProgramNode> Parser::ParseProgram() {
    auto program = std::make_shared<AST::ProgramNode>(currentToken_);
    program->SetSourceFile(currentFilename_);
    
    // 解析顶层语句
    while (!IsAtEnd()) {
        try {
            auto statement = ParseTopLevelStatement();
            if (statement) {
                program->AddChild(statement);
            }
        } catch (const std::exception& e) {
            if (config_.recoverFromErrors) {
                ReportError(e.what());
                Synchronize();
            } else {
                throw;
            }
        }
    }
    
    return program;
}

// 解析顶层语句
std::shared_ptr<AST::ASTNode> Parser::ParseTopLevelStatement() {
    // 跳过注释
    while (Match(TokenType::SingleLineComment) || 
           Match(TokenType::MultiLineComment) || 
           Match(TokenType::GeneratorComment)) {
        // 注释已被跳过
    }
    
    // 检查特殊标记
    if (Check(TokenType::Template)) {
        return ParseTemplateDeclaration();
    }
    
    if (Check(TokenType::Custom)) {
        return ParseCustomDeclaration();
    }
    
    if (Check(TokenType::Origin)) {
        return ParseOriginBlock();
    }
    
    if (Check(TokenType::Import)) {
        return ParseImportStatement();
    }
    
    if (Check(TokenType::Namespace)) {
        return ParseNamespaceDeclaration();
    }
    
    if (Check(TokenType::Configuration)) {
        return ParseConfigurationBlock();
    }
    
    // 检查HTML元素或标识符
    if (Check(TokenType::HtmlElement) || Check(TokenType::Identifier)) {
        return ParseElement();
    }
    
    ReportError("期望顶层语句");
    Advance();  // 跳过错误的Token
    return nullptr;
}

// 解析元素
std::shared_ptr<AST::ASTNode> Parser::ParseElement() {
    Token elementToken = currentToken_;
    std::string tagName;
    
    if (Check(TokenType::HtmlElement)) {
        tagName = Advance().value;
    } else if (Check(TokenType::Text)) {
        Advance();  // 消耗 'text' token
        return ParseTextBlock();
    } else if (Check(TokenType::Style)) {
        Advance();  // 消耗 'style' token
        return ParseStyleBlock(contextManager_->GetCurrentContextType() == ContextType::Element);
    } else if (Check(TokenType::Script)) {
        Advance();  // 消耗 'script' token
        return ParseScriptBlock(contextManager_->GetCurrentContextType() == ContextType::Element);
    } else if (Check(TokenType::Identifier)) {
        tagName = Advance().value;
        
        // 检查是否为特殊元素名（如果不是关键字）
        if (tagName == "text") {
            return ParseTextBlock();
        } else if (tagName == "style") {
            return ParseStyleBlock(contextManager_->GetCurrentContextType() == ContextType::Element);
        } else if (tagName == "script") {
            return ParseScriptBlock(contextManager_->GetCurrentContextType() == ContextType::Element);
        }
    } else {
        ReportError("期望元素名称");
        return nullptr;
    }
    
    return ParseElementNode(tagName);
}

// 解析元素节点
std::shared_ptr<AST::ElementNode> Parser::ParseElementNode(const std::string& tagName) {
    auto element = std::make_shared<AST::ElementNode>(tagName, previousToken_);
    
    // 进入元素上下文
    auto elementContext = ContextUtils::CreateElementContext(tagName);
    contextManager_->PushContext(elementContext);
    stateMachine_->PushState(ParseState::InElement, tagName);
    
    // 检查是否有属性块或内容块
    if (Match(TokenType::LeftBrace)) {
        ParseElementContent(element);
        Consume(TokenType::RightBrace, "期望 '}'");
    }
    
    // 退出元素上下文
    contextManager_->PopContext();
    stateMachine_->PopState();
    
    // 验证元素
    if (config_.validateSemantics) {
        ValidateElement(element);
    }
    
    return element;
}

// 解析元素内容
void Parser::ParseElementContent(std::shared_ptr<AST::ElementNode> element) {
    // 首先解析属性
    ParseAttributes(element);
    
    // 然后解析子元素
    while (!Check(TokenType::RightBrace) && !IsAtEnd()) {
        // 跳过注释
        if (Match(TokenType::SingleLineComment) || 
            Match(TokenType::MultiLineComment) || 
            Match(TokenType::GeneratorComment)) {
            continue;
        }
        
        // 在元素内部，可以有元素、text、style、script等
        if (Check(TokenType::HtmlElement) || Check(TokenType::Identifier) || 
            Check(TokenType::Text) || Check(TokenType::Style) || Check(TokenType::Script)) {
            auto child = ParseElement();
            if (child) {
                element->AddChild(child);
            }
        } else {
            ReportError("期望元素或标识符");
            Advance();  // 跳过错误的Token
        }
    }
}

// 解析属性
void Parser::ParseAttributes(std::shared_ptr<AST::ElementNode> element) {
    // 检查是否有属性
    while (Check(TokenType::Identifier) || Check(TokenType::HtmlElement)) {
        // 保存当前位置
        Token savedToken = currentToken_;
        std::string attrName = Advance().value;
        
        // 检查下一个是否是冒号
        if (!Match(TokenType::Colon)) {
            // 不是属性，回退
            currentToken_ = savedToken;
            break;
        }
        
        std::string attrValue = ParseStringLiteral();
        
        // 添加属性到元素
        element->AddAttribute(attrName, attrValue);
        
        // 可选的分号
        Match(TokenType::Semicolon);
    }
}

// 解析字符串字面量
std::string Parser::ParseStringLiteral() {
    if (Check(TokenType::StringLiteral)) {
        return Advance().value;
    } else if (Check(TokenType::SingleQuoteLiteral)) {
        return Advance().value;
    } else if (Check(TokenType::Identifier) || Check(TokenType::UnquotedLiteral)) {
        // 无引号字面量
        return Advance().value;
    } else if (Check(TokenType::Number)) {
        return Advance().value;
    }
    
    ReportError("期望字面量值");
    return "";
}

// 解析文本块
std::shared_ptr<AST::TextBlockNode> Parser::ParseTextBlock() {
    auto textNode = std::make_shared<AST::TextBlockNode>(previousToken_);
    
    Consume(TokenType::LeftBrace, "期望 '{'");
    
    // 解析文本内容
    std::string content = ParseStringLiteral();
    textNode->SetContent(content);
    
    Consume(TokenType::RightBrace, "期望 '}'");
    
    return textNode;
}

// 解析样式块
std::shared_ptr<AST::StyleBlockNode> Parser::ParseStyleBlock(bool isLocal) {
    auto styleBlock = std::make_shared<AST::StyleBlockNode>(isLocal, previousToken_);
    
    // 进入样式上下文
    contextManager_->EnterStyleContext(isLocal);
    stateMachine_->PushState(ParseState::InStyle, "style");
    
    Consume(TokenType::LeftBrace, "期望 '{'");
    
    // 解析样式规则
    while (!Check(TokenType::RightBrace) && !IsAtEnd()) {
        // 跳过注释
        if (Match(TokenType::SingleLineComment) || 
            Match(TokenType::MultiLineComment)) {
            continue;
        }
        
        auto rule = ParseStyleRule();
        if (rule) {
            styleBlock->AddChild(rule);
        }
    }
    
    Consume(TokenType::RightBrace, "期望 '}'");
    
    // 退出样式上下文
    contextManager_->ExitStyleContext();
    stateMachine_->PopState();
    
    return styleBlock;
}

// 解析样式规则
std::shared_ptr<AST::ASTNode> Parser::ParseStyleRule() {
    auto rule = std::make_shared<AST::StyleRuleNode>(currentToken_);
    
    // 检查是否有选择器
    if (Check(TokenType::Dot) || Check(TokenType::Identifier) || 
        Check(TokenType::Ampersand) || currentToken_.value == "#") {
        
        auto selector = ParseStyleSelector();
        rule->SetSelector(selector);
        
        // 解析样式块
        Consume(TokenType::LeftBrace, "期望 '{'");
        
        while (!Check(TokenType::RightBrace) && !IsAtEnd()) {
            auto property = ParseStyleProperty();
            if (property) {
                rule->AddChild(property);
            }
        }
        
        Consume(TokenType::RightBrace, "期望 '}'");
    } else {
        // 内联样式（无选择器）
        auto property = ParseStyleProperty();
        if (property) {
            rule->AddChild(property);
        }
    }
    
    return rule;
}

// 解析样式选择器
std::shared_ptr<AST::ASTNode> Parser::ParseStyleSelector() {
    if (Match(TokenType::Dot)) {
        // 类选择器
        std::string className = Consume(TokenType::Identifier, "期望类名").value;
        return std::make_shared<AST::StyleSelectorNode>(
            AST::StyleSelectorNode::SelectorType::Class, className, previousToken_);
    }
    
    if (Match(TokenType::Ampersand)) {
        // 上下文选择器 &
        if (Match(TokenType::Colon)) {
            // 伪类选择器 &:hover
            std::string pseudo = Consume(TokenType::Identifier, "期望伪类名").value;
            return std::make_shared<AST::StyleSelectorNode>(
                AST::StyleSelectorNode::SelectorType::PseudoClass, pseudo, previousToken_);
        }
        
        return std::make_shared<AST::StyleSelectorNode>(
            AST::StyleSelectorNode::SelectorType::Ampersand, "&", previousToken_);
    }
    
    if (currentToken_.value == "#") {
        // ID选择器
        Advance();
        std::string idName = Consume(TokenType::Identifier, "期望ID名").value;
        return std::make_shared<AST::StyleSelectorNode>(
            AST::StyleSelectorNode::SelectorType::Id, idName, previousToken_);
    }
    
    // 元素选择器
    if (Check(TokenType::Identifier) || Check(TokenType::HtmlElement)) {
        std::string elementName = Advance().value;
        return std::make_shared<AST::StyleSelectorNode>(
            AST::StyleSelectorNode::SelectorType::Element, elementName, previousToken_);
    }
    
    ReportError("期望样式选择器");
    return nullptr;
}

// 解析样式属性
std::shared_ptr<AST::ASTNode> Parser::ParseStyleProperty() {
    if (!Check(TokenType::Identifier)) {
        ReportError("期望属性名");
        return nullptr;
    }
    
    std::string propertyName = Advance().value;
    auto property = std::make_shared<AST::StylePropertyNode>(propertyName, previousToken_);
    
    Consume(TokenType::Colon, "期望 ':'");
    
    // 解析属性值
    while (!Check(TokenType::Semicolon) && !Check(TokenType::RightBrace) && !IsAtEnd()) {
        // 检查是否为变量使用
        if (Check(TokenType::Identifier) && PeekNext().type == TokenType::LeftParen) {
            auto varUse = ParseVariableUse();
            if (varUse) {
                property->AddChild(varUse);
            }
        } else {
            // 普通值
            std::string value = ParseStringLiteral();
            auto valueNode = std::make_shared<AST::StringLiteralNode>(value, previousToken_);
            property->AddChild(valueNode);
        }
        
        // 检查是否有更多值（空格分隔）
        if (!Check(TokenType::Semicolon) && !Check(TokenType::RightBrace)) {
            // 继续解析下一个值
            continue;
        }
        break;
    }
    
    // 可选的分号
    Match(TokenType::Semicolon);
    
    return property;
}

// 解析脚本块
std::shared_ptr<AST::ScriptBlockNode> Parser::ParseScriptBlock(bool isLocal) {
    auto scriptBlock = std::make_shared<AST::ScriptBlockNode>(isLocal, previousToken_);
    
    Consume(TokenType::LeftBrace, "期望 '{'");
    
    // 收集脚本内容直到找到匹配的右大括号
    std::stringstream content;
    int braceCount = 1;
    
    while (braceCount > 0 && !IsAtEnd()) {
        Token token = Advance();
        
        if (token.type == TokenType::LeftBrace) {
            braceCount++;
        } else if (token.type == TokenType::RightBrace) {
            braceCount--;
            if (braceCount == 0) break;
        }
        
        content << token.value << " ";
    }
    
    scriptBlock->SetContent(content.str());
    
    return scriptBlock;
}

// 解析模板声明
std::shared_ptr<AST::ASTNode> Parser::ParseTemplateDeclaration() {
    Consume(TokenType::Template, "期望 '[Template]'");
    
    // 解析模板类型
    AST::TemplateType templateType;
    std::string typeName;
    
    if (Match(TokenType::AtStyle)) {
        templateType = AST::TemplateType::Style;
        typeName = Consume(TokenType::Identifier, "期望模板名称").value;
    } else if (Match(TokenType::AtElement)) {
        templateType = AST::TemplateType::Element;
        typeName = Consume(TokenType::Identifier, "期望模板名称").value;
    } else if (Match(TokenType::AtVar)) {
        templateType = AST::TemplateType::Var;
        typeName = Consume(TokenType::Identifier, "期望模板名称").value;
    } else {
        ReportError("期望模板类型 (@Style, @Element, @Var)");
        return nullptr;
    }
    
    auto templateNode = std::make_shared<AST::TemplateDeclarationNode>(
        templateType, typeName, previousToken_);
    
    // 添加到符号表
    Symbol symbol(typeName, SymbolType::TemplateStyle, currentFilename_, 
                  previousToken_.line, previousToken_.column);
    globalMap_->AddSymbol(symbol);
    
    // 解析模板内容
    Consume(TokenType::LeftBrace, "期望 '{'");
    
    // 进入模板上下文
    auto templateContext = ContextUtils::CreateTemplateContext(GetTemplateTypeName(templateType));
    contextManager_->PushContext(templateContext);
    stateMachine_->PushState(ParseState::InTemplate, typeName);
    
    // 解析模板内容
    ParseTemplateContent(templateType);
    
    Consume(TokenType::RightBrace, "期望 '}'");
    
    // 退出模板上下文
    contextManager_->PopContext();
    stateMachine_->PopState();
    
    return templateNode;
}

// 解析模板内容
void Parser::ParseTemplateContent(AST::TemplateType type) {
    // TODO: 实现模板内容解析
}

// 解析自定义声明
std::shared_ptr<AST::ASTNode> Parser::ParseCustomDeclaration() {
    Consume(TokenType::Custom, "期望 '[Custom]'");
    
    // 解析自定义类型
    AST::TemplateType customType;
    std::string typeName;
    
    if (Match(TokenType::AtStyle)) {
        customType = AST::TemplateType::Style;
        typeName = Consume(TokenType::Identifier, "期望自定义名称").value;
    } else if (Match(TokenType::AtElement)) {
        customType = AST::TemplateType::Element;
        typeName = Consume(TokenType::Identifier, "期望自定义名称").value;
    } else if (Match(TokenType::AtVar)) {
        customType = AST::TemplateType::Var;
        typeName = Consume(TokenType::Identifier, "期望自定义名称").value;
    } else {
        ReportError("期望自定义类型 (@Style, @Element, @Var)");
        return nullptr;
    }
    
    auto customNode = std::make_shared<AST::CustomDeclarationNode>(
        customType, typeName, previousToken_);
    
    // 添加到符号表
    Symbol symbol(typeName, SymbolType::CustomStyle, currentFilename_, 
                  previousToken_.line, previousToken_.column);
    globalMap_->AddSymbol(symbol);
    
    // 解析自定义内容
    Consume(TokenType::LeftBrace, "期望 '{'");
    
    // 进入自定义上下文
    auto customContext = ContextUtils::CreateCustomContext(GetTemplateTypeName(customType));
    contextManager_->PushContext(customContext);
    stateMachine_->PushState(ParseState::InCustom, typeName);
    
    // 解析自定义内容
    // TODO: 实现自定义内容解析
    
    Consume(TokenType::RightBrace, "期望 '}'");
    
    // 退出自定义上下文
    contextManager_->PopContext();
    stateMachine_->PopState();
    
    return customNode;
}

// 解析变量使用
std::shared_ptr<AST::ASTNode> Parser::ParseVariableUse() {
    std::string varGroupName = Advance().value;
    
    Consume(TokenType::LeftParen, "期望 '('");
    std::string varName = Consume(TokenType::Identifier, "期望变量名").value;
    Consume(TokenType::RightParen, "期望 ')'");
    
    return std::make_shared<AST::StyleVariableUseNode>(
        varGroupName, varName, previousToken_);
}

// 辅助方法实现
bool Parser::Match(TokenType type) {
    if (Check(type)) {
        Advance();
        return true;
    }
    return false;
}

bool Parser::Check(TokenType type) const {
    if (IsAtEnd()) return false;
    return currentToken_.type == type;
}

Token Parser::Advance() {
    if (!IsAtEnd()) {
        previousToken_ = currentToken_;
        currentToken_ = lexer_->NextToken();
    }
    return previousToken_;
}

Token Parser::Consume(TokenType type, const std::string& message) {
    if (Check(type)) return Advance();
    
    ReportError(message);
    throw std::runtime_error(message);
}

Token Parser::Peek() const {
    return currentToken_;
}

Token Parser::PeekNext() const {
    return lexer_->PeekToken();
}

bool Parser::IsAtEnd() const {
    return currentToken_.type == TokenType::Eof;
}

// 错误处理
void Parser::ReportError(const std::string& message) {
    ReportError(message, currentToken_);
}

void Parser::ReportError(const std::string& message, const Token& token) {
    ParseError error = CreateError(message, token);
    errors_.push_back(error);
    
    if (errorCallback_) {
        errorCallback_(error);
    }
    
    LOG_ERROR(message + " at " + std::to_string(token.line) + ":" + std::to_string(token.column));
}

ParseError Parser::CreateError(const std::string& message, const Token& token) {
    ParseError error;
    error.message = message;
    error.line = token.line;
    error.column = token.column;
    error.filename = currentFilename_;
    error.actualToken = token.type;
    error.expectedToken = TokenType::Unknown;
    return error;
}

void Parser::Synchronize() {
    Advance();
    
    while (!IsAtEnd()) {
        if (previousToken_.type == TokenType::Semicolon) return;
        
        switch (currentToken_.type) {
            case TokenType::Template:
            case TokenType::Custom:
            case TokenType::Origin:
            case TokenType::Import:
            case TokenType::Namespace:
            case TokenType::Configuration:
            case TokenType::HtmlElement:
                return;
            default:
                break;
        }
        
        Advance();
    }
}

// 验证方法
bool Parser::ValidateElement(const std::shared_ptr<AST::ElementNode>& element) {
    // TODO: 实现元素验证
    (void)element;  // 抑制未使用参数警告
    return true;
}

// 工具方法
bool Parser::IsHTMLElement(const std::string& name) const {
    return TokenUtils::IsHtmlElement(name);
}

bool Parser::IsKeyword(const std::string& name) const {
    return TokenUtils::IsKeyword(name);
}

// 其余方法的占位实现
std::shared_ptr<AST::ASTNode> Parser::ParseOriginBlock() {
    // TODO: 实现原始块解析
    return nullptr;
}

std::shared_ptr<AST::ASTNode> Parser::ParseImportStatement() {
    // TODO: 实现导入语句解析
    return nullptr;
}

std::shared_ptr<AST::ASTNode> Parser::ParseNamespaceDeclaration() {
    // TODO: 实现命名空间声明解析
    return nullptr;
}

std::shared_ptr<AST::ASTNode> Parser::ParseConfigurationBlock() {
    // TODO: 实现配置块解析
    return nullptr;
}

} // namespace CHTL