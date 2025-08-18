#include "TemplateParser.h"
#include <iostream>
#include <regex>
#include <algorithm>

namespace chtl {
namespace template_system {

TemplateParser::TemplateParser() : debugMode_(false) {
    ownedContext_ = std::make_unique<CHTLContext>();
    ownedStateManager_ = std::make_unique<StateManager>();
    context_ = ownedContext_.get();
    stateManager_ = ownedStateManager_.get();
    templateManager_ = g_templateManager;
}

TemplateParser::TemplateParser(CHTLContext& context, StateManager& stateManager) 
    : context_(&context), stateManager_(&stateManager), debugMode_(false) {
    templateManager_ = g_templateManager;
}

TemplateParser::~TemplateParser() = default;

std::unique_ptr<ast::TemplateNode> TemplateParser::parseTemplate(const std::vector<Token>& tokens, size_t& position) {
    if (debugMode_) {
        std::cout << "开始解析模板定义" << std::endl;
    }
    
    // 更新状态机
    if (!stateManager_->transition(CompilerState::PARSING_TEMPLATE, SyntaxContext::TEMPLATE_DEFINITION)) {
        addError("无法转换到模板解析状态");
        return nullptr;
    }
    
    // 重置解析上下文
    parseContext_ = TemplateParseContext();
    
    // 解析模板头部
    if (!parseTemplateHeader(tokens, position)) {
        return nullptr;
    }
    
    // 创建AST节点
    auto templateNode = std::make_unique<ast::TemplateNode>();
    templateNode->name = parseContext_.templateName;
    templateNode->type = parseContext_.templateType;
    
    // 进入模板上下文
    enterTemplateContext(parseContext_.templateType, parseContext_.templateName);
    
    // 解析模板体
    if (!parseTemplateBody(tokens, position)) {
        exitTemplateContext();
        return nullptr;
    }
    
    // 根据类型分别处理
    if (parseContext_.templateType == "Style") {
        return parseStyleTemplate(tokens, position);
    } else if (parseContext_.templateType == "Element") {
        return parseElementTemplate(tokens, position);
    } else if (parseContext_.templateType == "Var") {
        return parseVariableTemplate(tokens, position);
    }
    
    exitTemplateContext();
    
    if (debugMode_) {
        std::cout << "模板解析完成: " << parseContext_.templateName << std::endl;
    }
    
    return templateNode;
}

std::unique_ptr<ast::TemplateNode> TemplateParser::parseStyleTemplate(const std::vector<Token>& tokens, size_t& position) {
    auto templateNode = std::make_unique<ast::TemplateNode>();
    templateNode->name = parseContext_.templateName;
    templateNode->type = "Style";
    
    // 创建样式模板结构
    StyleTemplate styleTemplate;
    styleTemplate.name = parseContext_.templateName;
    styleTemplate.properties = parseContext_.currentProperties;
    styleTemplate.inheritedTemplates = parseContext_.inheritedTemplates;
    
    // 注册到模板管理器
    templateManager_->addStyleTemplate(parseContext_.templateName, styleTemplate);
    
    // 验证模板定义
    if (!validateTemplateDefinition("Style", parseContext_.templateName)) {
        addError("样式模板定义验证失败: " + parseContext_.templateName);
        return nullptr;
    }
    
    return templateNode;
}

std::unique_ptr<ast::TemplateNode> TemplateParser::parseElementTemplate(const std::vector<Token>& tokens, size_t& position) {
    auto templateNode = std::make_unique<ast::TemplateNode>();
    templateNode->name = parseContext_.templateName;
    templateNode->type = "Element";
    
    // 创建元素模板结构
    ElementTemplate elementTemplate;
    elementTemplate.name = parseContext_.templateName;
    elementTemplate.elements = std::move(parseContext_.currentElements);
    elementTemplate.inheritedTemplates = parseContext_.inheritedTemplates;
    
    // 注册到模板管理器
    templateManager_->addElementTemplate(parseContext_.templateName, std::move(elementTemplate));
    
    // 验证模板定义
    if (!validateTemplateDefinition("Element", parseContext_.templateName)) {
        addError("元素模板定义验证失败: " + parseContext_.templateName);
        return nullptr;
    }
    
    return templateNode;
}

std::unique_ptr<ast::TemplateNode> TemplateParser::parseVariableTemplate(const std::vector<Token>& tokens, size_t& position) {
    auto templateNode = std::make_unique<ast::TemplateNode>();
    templateNode->name = parseContext_.templateName;
    templateNode->type = "Var";
    
    // 创建变量模板结构
    VariableTemplate variableTemplate;
    variableTemplate.name = parseContext_.templateName;
    variableTemplate.variables = parseContext_.currentProperties; // 复用properties作为variables
    variableTemplate.inheritedTemplates = parseContext_.inheritedTemplates;
    
    // 注册到模板管理器
    templateManager_->addVariableTemplate(parseContext_.templateName, variableTemplate);
    
    // 验证模板定义
    if (!validateTemplateDefinition("Var", parseContext_.templateName)) {
        addError("变量模板定义验证失败: " + parseContext_.templateName);
        return nullptr;
    }
    
    return templateNode;
}

bool TemplateParser::parseTemplateHeader(const std::vector<Token>& tokens, size_t& position) {
    // 期望 [Template] @Type Name
    
    // 1. 消费 [Template] 关键字
    if (!expectToken(tokens, position, TokenType::LEFT_BRACKET, "期望 '['")) return false;
    if (!expectToken(tokens, position, TokenType::TEMPLATE, "期望 'Template'")) return false;
    if (!expectToken(tokens, position, TokenType::RIGHT_BRACKET, "期望 ']'")) return false;
    
    // 2. 消费类型说明符 @Style/@Element/@Var
    if (!expectToken(tokens, position, TokenType::AT, "期望 '@'")) return false;
    
    Token typeToken = getCurrentToken(tokens, position);
    if (typeToken.type != TokenType::IDENTIFIER) {
        addError("期望模板类型标识符");
        return false;
    }
    
    parseContext_.templateType = typeToken.value;
    position++;
    
    // 验证模板类型
    if (!isValidTemplateType(parseContext_.templateType)) {
        addError("无效的模板类型: " + parseContext_.templateType);
        return false;
    }
    
    // 3. 消费模板名称
    Token nameToken = getCurrentToken(tokens, position);
    if (nameToken.type != TokenType::IDENTIFIER) {
        addError("期望模板名称标识符");
        return false;
    }
    
    parseContext_.templateName = nameToken.value;
    position++;
    
    // 验证模板名称
    if (!isValidIdentifier(parseContext_.templateName)) {
        addError("无效的模板名称: " + parseContext_.templateName);
        return false;
    }
    
    // 更新解析状态
    updateParseState(TemplateParseState::EXPECTING_TEMPLATE_NAME, "解析模板头部完成");
    
    return true;
}

bool TemplateParser::parseTemplateBody(const std::vector<Token>& tokens, size_t& position) {
    // 期望 { ... }
    if (!expectToken(tokens, position, TokenType::LEFT_BRACE, "期望 '{'")) return false;
    
    parseContext_.braceDepth = 1;
    updateParseState(TemplateParseState::PARSING_TEMPLATE_BODY, "开始解析模板体");
    
    while (position < tokens.size() && parseContext_.braceDepth > 0) {
        Token currentToken = getCurrentToken(tokens, position);
        
        switch (currentToken.type) {
            case TokenType::LEFT_BRACE:
                parseContext_.braceDepth++;
                position++;
                break;
                
            case TokenType::RIGHT_BRACE:
                parseContext_.braceDepth--;
                position++;
                if (parseContext_.braceDepth == 0) {
                    updateParseState(TemplateParseState::TEMPLATE_COMPLETE, "模板体解析完成");
                }
                break;
                
            case TokenType::AT:
                // 可能是继承语句 @Style TemplateName;
                if (!parseInheritanceStatement(tokens, position, parseContext_.inheritedTemplates)) {
                    return false;
                }
                break;
                
            case TokenType::INHERIT:
                // 显式继承 inherit @Style TemplateName;
                if (!parseExplicitInheritance(tokens, position, parseContext_.inheritedTemplates)) {
                    return false;
                }
                break;
                
            case TokenType::IDENTIFIER:
                // 属性定义或元素定义
                if (parseContext_.templateType == "Style") {
                    std::string property, value;
                    if (!parseStyleProperty(tokens, position, property, value)) {
                        return false;
                    }
                    parseContext_.currentProperties[property] = value;
                } else if (parseContext_.templateType == "Var") {
                    std::string varName, varValue;
                    if (!parseVariableDefinition(tokens, position, varName, varValue)) {
                        return false;
                    }
                    parseContext_.currentProperties[varName] = varValue; // 复用properties
                } else if (parseContext_.templateType == "Element") {
                    auto element = parseTemplateElement(tokens, position);
                    if (element) {
                        parseContext_.currentElements.push_back(std::move(element));
                    }
                }
                break;
                
            default:
                position++;
                break;
        }
    }
    
    return parseContext_.braceDepth == 0;
}

bool TemplateParser::parseStyleProperty(const std::vector<Token>& tokens, size_t& position,
                                       std::string& property, std::string& value) {
    // 解析 property: value; 或 property = value;
    
    Token propToken = getCurrentToken(tokens, position);
    if (propToken.type != TokenType::IDENTIFIER) {
        addError("期望属性名称");
        return false;
    }
    
    property = propToken.value;
    position++;
    
    // 消费 : 或 =
    Token separatorToken = getCurrentToken(tokens, position);
    if (separatorToken.type != TokenType::COLON && separatorToken.type != TokenType::ASSIGN) {
        addError("期望 ':' 或 '='");
        return false;
    }
    position++;
    
    // 消费值
    Token valueToken = getCurrentToken(tokens, position);
    if (valueToken.type == TokenType::STRING || valueToken.type == TokenType::IDENTIFIER || valueToken.type == TokenType::NUMBER) {
        value = extractStringLiteral(valueToken.value);
        position++;
    } else {
        addError("期望属性值");
        return false;
    }
    
    // 消费分号（可选）
    Token nextToken = getCurrentToken(tokens, position);
    if (nextToken.type == TokenType::SEMICOLON) {
        position++;
    }
    
    return true;
}

bool TemplateParser::parseVariableDefinition(const std::vector<Token>& tokens, size_t& position,
                                            std::string& varName, std::string& varValue) {
    // 按语法文档：变量定义格式 variableName: "value";
    return parseStyleProperty(tokens, position, varName, varValue);
}

bool TemplateParser::parseCompositeInheritance(const std::vector<Token>& tokens, size_t& position,
                                              std::vector<std::string>& inheritedTemplates) {
    // 按语法文档：组合式继承 @Style ThemeColor;
    return parseInheritanceStatement(tokens, position, inheritedTemplates);
}

std::unique_ptr<ast::ElementNode> TemplateParser::parseTemplateElement(const std::vector<Token>& tokens, size_t& position) {
    Token elementToken = getCurrentToken(tokens, position);
    if (elementToken.type != TokenType::IDENTIFIER) {
        addError("期望元素标签名");
        return nullptr;
    }
    
    auto elementNode = std::make_unique<ast::ElementNode>();
    elementNode->tag = elementToken.value;
    position++;
    
    // TODO: 解析元素属性和子元素
    // 这需要与主解析器集成
    
    return elementNode;
}

bool TemplateParser::parseInheritanceStatement(const std::vector<Token>& tokens, size_t& position, 
                                              std::vector<std::string>& inheritedTemplates) {
    // 解析 @Style TemplateName; 格式的继承
    
    if (!expectToken(tokens, position, TokenType::AT, "期望 '@'")) return false;
    
    Token typeToken = getCurrentToken(tokens, position);
    if (typeToken.type != TokenType::IDENTIFIER) {
        addError("期望继承类型");
        return false;
    }
    
    std::string inheritType = typeToken.value;
    position++;
    
    // 验证继承类型与当前模板类型匹配
    if (inheritType != parseContext_.templateType) {
        addError("继承类型不匹配: 期望 " + parseContext_.templateType + "，得到 " + inheritType);
        return false;
    }
    
    Token nameToken = getCurrentToken(tokens, position);
    if (nameToken.type != TokenType::IDENTIFIER) {
        addError("期望继承模板名称");
        return false;
    }
    
    std::string inheritedName = nameToken.value;
    position++;
    
    // 消费分号
    if (!expectToken(tokens, position, TokenType::SEMICOLON, "期望 ';'")) return false;
    
    inheritedTemplates.push_back(inheritedName);
    
    if (debugMode_) {
        std::cout << "解析继承: " << inheritType << " " << inheritedName << std::endl;
    }
    
    return true;
}

bool TemplateParser::parseExplicitInheritance(const std::vector<Token>& tokens, size_t& position,
                                             std::vector<std::string>& inheritedTemplates) {
    // 解析 inherit @Style TemplateName; 格式的显式继承
    
    if (!expectToken(tokens, position, TokenType::INHERIT, "期望 'inherit'")) return false;
    
    return parseInheritanceStatement(tokens, position, inheritedTemplates);
}

bool TemplateParser::updateParseState(TemplateParseState newState, const std::string& reason) {
    if (debugMode_) {
        std::cout << "模板解析状态转换: " << static_cast<int>(parseContext_.state) 
                  << " -> " << static_cast<int>(newState) << " (" << reason << ")" << std::endl;
    }
    
    parseContext_.state = newState;
    return true;
}

void TemplateParser::enterTemplateContext(const std::string& templateType, const std::string& templateName) {
    context_->enterScope("template:" + templateName);
    context_->addTemplate(templateName, templateType);
    
    if (debugMode_) {
        std::cout << "进入模板上下文: " << templateType << " " << templateName << std::endl;
    }
}

void TemplateParser::exitTemplateContext() {
    context_->exitScope();
    
    if (debugMode_) {
        std::cout << "退出模板上下文" << std::endl;
    }
}

bool TemplateParser::validateTemplateDefinition(const std::string& templateType, const std::string& templateName) {
    // 检查模板名称是否已存在
    if (templateType == "Style" && templateManager_->hasStyleTemplate(templateName)) {
        addError("样式模板已存在: " + templateName);
        return false;
    }
    
    if (templateType == "Element" && templateManager_->hasElementTemplate(templateName)) {
        addError("元素模板已存在: " + templateName);
        return false;
    }
    
    if (templateType == "Var" && templateManager_->hasVariableTemplate(templateName)) {
        addError("变量模板已存在: " + templateName);
        return false;
    }
    
    return true;
}

bool TemplateParser::validateTemplateUsage(const std::string& templateType, const std::string& templateName) {
    // 验证模板使用的有效性
    if (templateType == "Style") {
        return templateManager_->hasStyleTemplate(templateName);
    } else if (templateType == "Element") {
        return templateManager_->hasElementTemplate(templateName);
    } else if (templateType == "Var") {
        return templateManager_->hasVariableTemplate(templateName);
    }
    
    return false;
}

bool TemplateParser::validateInheritance(const std::string& templateType, const std::vector<std::string>& inherited) {
    for (const auto& inheritedName : inherited) {
        if (!validateTemplateUsage(templateType, inheritedName)) {
            addError("继承的模板不存在: " + templateType + " " + inheritedName);
            return false;
        }
    }
    return true;
}

bool TemplateParser::expectToken(const std::vector<Token>& tokens, size_t& position, TokenType expectedType,
                                const std::string& errorMessage) {
    if (isAtEnd(tokens, position)) {
        addError("意外的文件结束");
        return false;
    }
    
    Token token = getCurrentToken(tokens, position);
    if (token.type != expectedType) {
        addError(errorMessage.empty() ? "Token类型不匹配" : errorMessage);
        return false;
    }
    
    position++;
    return true;
}

bool TemplateParser::consumeToken(const std::vector<Token>& tokens, size_t& position, TokenType tokenType) {
    if (isAtEnd(tokens, position)) return false;
    
    Token token = getCurrentToken(tokens, position);
    if (token.type == tokenType) {
        position++;
        return true;
    }
    
    return false;
}

Token TemplateParser::getCurrentToken(const std::vector<Token>& tokens, size_t position) const {
    if (position >= tokens.size()) {
        return Token(TokenType::EOF_TOKEN, "", TokenPosition());
    }
    return tokens[position];
}

Token TemplateParser::peekToken(const std::vector<Token>& tokens, size_t position, size_t offset) const {
    size_t targetPos = position + offset;
    if (targetPos >= tokens.size()) {
        return Token(TokenType::EOF_TOKEN, "", TokenPosition());
    }
    return tokens[targetPos];
}

bool TemplateParser::isAtEnd(const std::vector<Token>& tokens, size_t position) const {
    return position >= tokens.size() || getCurrentToken(tokens, position).type == TokenType::EOF_TOKEN;
}

void TemplateParser::addError(const std::string& error) {
    errors_.push_back("模板解析错误: " + error);
    
    if (debugMode_) {
        std::cout << "模板解析错误: " << error << std::endl;
    }
}

void TemplateParser::addContextualError(const std::string& error, const Token& token) {
    std::string contextualError = error + " (行:" + std::to_string(token.position.line) + 
                                 ", 列:" + std::to_string(token.position.column) + ")";
    addError(contextualError);
}

bool TemplateParser::isValidTemplateState() const {
    return parseContext_.state != TemplateParseState::EXPECTING_TEMPLATE_KEYWORD;
}

bool TemplateParser::canParseTemplateType(const std::string& templateType) const {
    return templateType == "Style" || templateType == "Element" || templateType == "Var";
}

std::string TemplateParser::extractStringLiteral(const std::string& tokenValue) {
    if (tokenValue.length() >= 2 && 
        ((tokenValue.front() == '"' && tokenValue.back() == '"') ||
         (tokenValue.front() == '\'' && tokenValue.back() == '\''))) {
        return tokenValue.substr(1, tokenValue.length() - 2);
    }
    return tokenValue;
}

bool TemplateParser::isValidIdentifier(const std::string& identifier) {
    if (identifier.empty()) return false;
    
    // 检查是否以字母或下划线开头
    if (!std::isalpha(identifier[0]) && identifier[0] != '_') {
        return false;
    }
    
    // 检查其余字符是否为字母、数字或下划线
    for (size_t i = 1; i < identifier.length(); ++i) {
        if (!std::isalnum(identifier[i]) && identifier[i] != '_') {
            return false;
        }
    }
    
    return true;
}

bool TemplateParser::isValidTemplateType(const std::string& type) {
    return type == "Style" || type == "Element" || type == "Var";
}

} // namespace template_system
} // namespace chtl