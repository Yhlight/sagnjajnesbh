#include "CustomParser.h"
#include "../Lexer/CHTLToken.h"
#include <iostream>

namespace chtl {
namespace custom_system {

CustomParser::CustomParser() : debugMode_(false) {
    ownedContext_ = std::make_unique<CHTLContext>();
    ownedStateManager_ = std::make_unique<StateManager>();
    context_ = ownedContext_.get();
    stateManager_ = ownedStateManager_.get();
    // 延迟初始化Manager
    customManager_ = nullptr;
}

CustomParser::CustomParser(CHTLContext& context, StateManager& stateManager) 
    : context_(&context), stateManager_(&stateManager), debugMode_(false) {
    // 延迟初始化Manager
    customManager_ = nullptr;
}

CustomParser::~CustomParser() = default;

std::shared_ptr<CustomManager> CustomParser::getCustomManager() {
    if (!customManager_) {
        customManager_ = g_customManager;
    }
    return customManager_;
}

std::unique_ptr<ast::CustomNode> CustomParser::parseCustom(const std::vector<Token>& tokens, size_t& position) {
    // 按语法文档解析 [Custom] @Style/@Element/@Var 块
    
    if (!expectToken(tokens, position, TokenType::LEFT_BRACKET)) {
        addError("期望 '[' 开始自定义块");
        return nullptr;
    }
    
    if (!expectKeyword(tokens, position, "Custom")) {
        addError("期望 'Custom' 关键字");
        return nullptr;
    }
    
    if (!expectToken(tokens, position, TokenType::RIGHT_BRACKET)) {
        addError("期望 ']' 结束自定义标识");
        return nullptr;
    }
    
    // 解析类型标识符：@Style, @Element, @Var
    if (!expectToken(tokens, position, TokenType::AT)) {
        addError("期望 '@' 类型标识符");
        return nullptr;
    }
    
    Token typeToken = getCurrentToken(tokens, position);
    if (typeToken.type != TokenType::IDENTIFIER) {
        addError("期望自定义类型标识符");
        return nullptr;
    }
    
    std::string customType = typeToken.value;
    position++;
    
    // 解析自定义名称
    Token nameToken = getCurrentToken(tokens, position);
    if (nameToken.type != TokenType::IDENTIFIER) {
        addError("期望自定义名称");
        return nullptr;
    }
    
    std::string customName = nameToken.value;
    position++;
    
    // 创建AST节点
    auto customNode = std::make_unique<ast::CustomNode>();
    customNode->name = customName;
    customNode->customType = customType;
    
    // 根据类型解析内容
    if (customType == "Style") {
        return parseCustomStyle(tokens, position, std::move(customNode));
    } else if (customType == "Element") {
        return parseCustomElement(tokens, position, std::move(customNode));
    } else if (customType == "Var") {
        return parseCustomVariable(tokens, position, std::move(customNode));
    } else {
        addError("未知的自定义类型: " + customType);
        return nullptr;
    }
}

std::unique_ptr<ast::CustomNode> CustomParser::parseCustomStyle(const std::vector<Token>& tokens, size_t& position, 
                                                               std::unique_ptr<ast::CustomNode> customNode) {
    // 按语法文档解析自定义样式组
    
    if (!expectToken(tokens, position, TokenType::LEFT_BRACE)) {
        addError("期望 '{' 开始自定义样式块");
        return nullptr;
    }
    
    CustomStyle customStyle;
    customStyle.name = customNode->name;
    
    // 解析继承语句
    parseCustomInheritance(tokens, position, customStyle);
    
    // 解析属性和无值属性
    while (position < tokens.size() && tokens[position].type != TokenType::RIGHT_BRACE) {
        if (tokens[position].type == TokenType::IDENTIFIER) {
            std::string propName = tokens[position].value;
            position++;
            
            if (position < tokens.size() && tokens[position].type == TokenType::COLON) {
                // 有值属性
                position++; // 跳过 ':'
                
                if (position < tokens.size()) {
                    std::string propValue = tokens[position].value;
                    customStyle.properties[propName] = propValue;
                    position++;
                }
            } else if (position < tokens.size() && tokens[position].type == TokenType::SEMICOLON) {
                // 无值属性
                customStyle.noValueProperties.insert(propName);
            }
            
            // 跳过分号
            if (position < tokens.size() && tokens[position].type == TokenType::SEMICOLON) {
                position++;
            }
        } else {
            position++; // 跳过未识别的token
        }
    }
    
    if (!expectToken(tokens, position, TokenType::RIGHT_BRACE)) {
        addError("期望 '}' 结束自定义样式块");
        return nullptr;
    }
    
    // 注册到管理器
    customManager_->addCustomStyle(customStyle.name, customStyle);
    
    return customNode;
}

std::unique_ptr<ast::CustomNode> CustomParser::parseCustomElement(const std::vector<Token>& tokens, size_t& position,
                                                                 std::unique_ptr<ast::CustomNode> customNode) {
    // 按语法文档解析自定义元素
    
    if (!expectToken(tokens, position, TokenType::LEFT_BRACE)) {
        addError("期望 '{' 开始自定义元素块");
        return nullptr;
    }
    
    CustomElement customElement;
    customElement.name = customNode->name;
    
    // 解析继承语句
    parseCustomInheritance(tokens, position, customElement);
    
    // 解析HTML元素内容
    while (position < tokens.size() && tokens[position].type != TokenType::RIGHT_BRACE) {
        // 解析HTML元素（简化实现）
        auto elementNode = parseHTMLElement(tokens, position);
        if (elementNode) {
            customElement.elements.push_back(std::move(elementNode));
        } else {
            position++; // 跳过无法解析的token
        }
    }
    
    if (!expectToken(tokens, position, TokenType::RIGHT_BRACE)) {
        addError("期望 '}' 结束自定义元素块");
        return nullptr;
    }
    
    // 注册到管理器
    customManager_->addCustomElement(customElement.name, std::move(customElement));
    
    return customNode;
}

std::unique_ptr<ast::CustomNode> CustomParser::parseCustomVariable(const std::vector<Token>& tokens, size_t& position,
                                                                  std::unique_ptr<ast::CustomNode> customNode) {
    // 按语法文档解析自定义变量组
    
    if (!expectToken(tokens, position, TokenType::LEFT_BRACE)) {
        addError("期望 '{' 开始自定义变量块");
        return nullptr;
    }
    
    CustomVariable customVariable;
    customVariable.name = customNode->name;
    
    // 解析继承语句
    parseCustomInheritance(tokens, position, customVariable);
    
    // 解析变量定义
    while (position < tokens.size() && tokens[position].type != TokenType::RIGHT_BRACE) {
        if (tokens[position].type == TokenType::IDENTIFIER) {
            std::string varName = tokens[position].value;
            position++;
            
            if (position < tokens.size() && tokens[position].type == TokenType::COLON) {
                // 有值变量
                position++; // 跳过 ':'
                
                if (position < tokens.size()) {
                    std::string varValue = tokens[position].value;
                    customVariable.variables[varName] = varValue;
                    position++;
                }
            } else if (position < tokens.size() && tokens[position].type == TokenType::SEMICOLON) {
                // 无值变量
                customVariable.noValueVariables.insert(varName);
            }
            
            // 跳过分号
            if (position < tokens.size() && tokens[position].type == TokenType::SEMICOLON) {
                position++;
            }
        } else {
            position++; // 跳过未识别的token
        }
    }
    
    if (!expectToken(tokens, position, TokenType::RIGHT_BRACE)) {
        addError("期望 '}' 结束自定义变量块");
        return nullptr;
    }
    
    // 注册到管理器
    customManager_->addCustomVariable(customVariable.name, customVariable);
    
    return customNode;
}

// === 特例化操作解析 ===

std::unique_ptr<ast::SpecializationNode> CustomParser::parseSpecialization(const std::vector<Token>& tokens, size_t& position) {
    // 按语法文档解析特例化操作：inherit, delete, insert, replace
    
    Token operationToken = getCurrentToken(tokens, position);
    if (operationToken.type != TokenType::IDENTIFIER) {
        addError("期望特例化操作关键字");
        return nullptr;
    }
    
    std::string operation = operationToken.value;
    position++;
    
    auto specializationNode = std::make_unique<ast::SpecializationNode>();
    specializationNode->operation = operation;
    
    if (operation == "inherit") {
        return parseInheritSpecialization(tokens, position, std::move(specializationNode));
    } else if (operation == "delete") {
        return parseDeleteSpecialization(tokens, position, std::move(specializationNode));
    } else if (operation == "insert") {
        return parseInsertSpecialization(tokens, position, std::move(specializationNode));
    } else if (operation == "replace") {
        return parseReplaceSpecialization(tokens, position, std::move(specializationNode));
    } else {
        addError("未知的特例化操作: " + operation);
        return nullptr;
    }
}

std::unique_ptr<ast::SpecializationNode> CustomParser::parseInheritSpecialization(const std::vector<Token>& tokens, size_t& position,
                                                                                  std::unique_ptr<ast::SpecializationNode> node) {
    // 解析 inherit @Style ThemeColor;
    
    if (!expectToken(tokens, position, TokenType::AT)) {
        addError("期望 '@' 继承类型标识");
        return nullptr;
    }
    
    Token typeToken = getCurrentToken(tokens, position);
    if (typeToken.type != TokenType::IDENTIFIER) {
        addError("期望继承类型");
        return nullptr;
    }
    
    std::string inheritType = typeToken.value;
    position++;
    
    Token nameToken = getCurrentToken(tokens, position);
    if (nameToken.type != TokenType::IDENTIFIER) {
        addError("期望继承目标名称");
        return nullptr;
    }
    
    std::string inheritName = nameToken.value;
    position++;
    
    node->target = "@" + inheritType + " " + inheritName;
    
    if (!expectToken(tokens, position, TokenType::SEMICOLON)) {
        addError("期望 ';' 结束继承语句");
        return nullptr;
    }
    
    return node;
}

std::unique_ptr<ast::SpecializationNode> CustomParser::parseDeleteSpecialization(const std::vector<Token>& tokens, size_t& position,
                                                                                 std::unique_ptr<ast::SpecializationNode> node) {
    // 解析 delete span, div[0], @Style WhiteText;
    
    std::vector<std::string> targets;
    
    do {
        Token targetToken = getCurrentToken(tokens, position);
        if (targetToken.type == TokenType::IDENTIFIER) {
            std::string target = targetToken.value;
            position++;
            
            // 检查是否有索引：div[0]
            if (position < tokens.size() && tokens[position].type == TokenType::LEFT_BRACKET) {
                position++; // 跳过 '['
                
                if (position < tokens.size() && tokens[position].type == TokenType::NUMBER) {
                    target += "[" + tokens[position].value + "]";
                    position++;
                    
                    if (!expectToken(tokens, position, TokenType::RIGHT_BRACKET)) {
                        addError("期望 ']' 结束索引");
                        return nullptr;
                    }
                }
            }
            
            targets.push_back(target);
        } else if (targetToken.type == TokenType::AT) {
            // @Style WhiteText 格式
            position++; // 跳过 '@'
            
            Token typeToken = getCurrentToken(tokens, position);
            if (typeToken.type != TokenType::IDENTIFIER) {
                addError("期望删除类型");
                return nullptr;
            }
            
            std::string deleteType = typeToken.value;
            position++;
            
            Token nameToken = getCurrentToken(tokens, position);
            if (nameToken.type != TokenType::IDENTIFIER) {
                addError("期望删除目标名称");
                return nullptr;
            }
            
            std::string deleteName = nameToken.value;
            position++;
            
            targets.push_back("@" + deleteType + " " + deleteName);
        }
        
        // 检查是否有逗号继续
        if (position < tokens.size() && tokens[position].type == TokenType::COMMA) {
            position++; // 跳过 ','
        } else {
            break;
        }
    } while (position < tokens.size());
    
    // 将所有目标合并为一个字符串
    std::string allTargets;
    for (size_t i = 0; i < targets.size(); ++i) {
        if (i > 0) allTargets += ", ";
        allTargets += targets[i];
    }
    
    node->target = allTargets;
    
    if (!expectToken(tokens, position, TokenType::SEMICOLON)) {
        addError("期望 ';' 结束删除语句");
        return nullptr;
    }
    
    return node;
}

std::unique_ptr<ast::SpecializationNode> CustomParser::parseInsertSpecialization(const std::vector<Token>& tokens, size_t& position,
                                                                                 std::unique_ptr<ast::SpecializationNode> node) {
    // 解析 insert after div[0] { ... }
    
    // 解析位置：after, before, at top, at bottom
    Token positionToken = getCurrentToken(tokens, position);
    if (positionToken.type != TokenType::IDENTIFIER) {
        addError("期望插入位置");
        return nullptr;
    }
    
    std::string insertPosition = positionToken.value;
    position++;
    
    // 处理复合位置：at top, at bottom
    if (insertPosition == "at") {
        Token nextToken = getCurrentToken(tokens, position);
        if (nextToken.type == TokenType::IDENTIFIER && 
            (nextToken.value == "top" || nextToken.value == "bottom")) {
            insertPosition += " " + nextToken.value;
            position++;
        }
    }
    
    node->position = insertPosition;
    
    // 解析目标选择器
    Token targetToken = getCurrentToken(tokens, position);
    if (targetToken.type == TokenType::IDENTIFIER) {
        std::string target = targetToken.value;
        position++;
        
        // 检查索引
        if (position < tokens.size() && tokens[position].type == TokenType::LEFT_BRACKET) {
            position++; // 跳过 '['
            
            if (position < tokens.size() && tokens[position].type == TokenType::NUMBER) {
                target += "[" + tokens[position].value + "]";
                position++;
                
                if (!expectToken(tokens, position, TokenType::RIGHT_BRACKET)) {
                    addError("期望 ']' 结束索引");
                    return nullptr;
                }
            }
        }
        
        node->target = target;
    }
    
    // 解析插入内容
    if (!expectToken(tokens, position, TokenType::LEFT_BRACE)) {
        addError("期望 '{' 开始插入内容");
        return nullptr;
    }
    
    // 解析插入的HTML内容（简化实现）
    while (position < tokens.size() && tokens[position].type != TokenType::RIGHT_BRACE) {
        auto elementNode = parseHTMLElement(tokens, position);
        if (elementNode) {
            node->content.push_back(std::move(elementNode));
        } else {
            position++;
        }
    }
    
    if (!expectToken(tokens, position, TokenType::RIGHT_BRACE)) {
        addError("期望 '}' 结束插入内容");
        return nullptr;
    }
    
    return node;
}

std::unique_ptr<ast::SpecializationNode> CustomParser::parseReplaceSpecialization(const std::vector<Token>& tokens, size_t& position,
                                                                                  std::unique_ptr<ast::SpecializationNode> node) {
    // 解析 replace span { ... }
    
    Token targetToken = getCurrentToken(tokens, position);
    if (targetToken.type != TokenType::IDENTIFIER) {
        addError("期望替换目标");
        return nullptr;
    }
    
    std::string target = targetToken.value;
    position++;
    
    // 检查索引
    if (position < tokens.size() && tokens[position].type == TokenType::LEFT_BRACKET) {
        position++; // 跳过 '['
        
        if (position < tokens.size() && tokens[position].type == TokenType::NUMBER) {
            target += "[" + tokens[position].value + "]";
            position++;
            
            if (!expectToken(tokens, position, TokenType::RIGHT_BRACKET)) {
                addError("期望 ']' 结束索引");
                return nullptr;
            }
        }
    }
    
    node->target = target;
    
    // 解析替换内容
    if (!expectToken(tokens, position, TokenType::LEFT_BRACE)) {
        addError("期望 '{' 开始替换内容");
        return nullptr;
    }
    
    // 解析替换的HTML内容（简化实现）
    while (position < tokens.size() && tokens[position].type != TokenType::RIGHT_BRACE) {
        auto elementNode = parseHTMLElement(tokens, position);
        if (elementNode) {
            node->content.push_back(std::move(elementNode));
        } else {
            position++;
        }
    }
    
    if (!expectToken(tokens, position, TokenType::RIGHT_BRACE)) {
        addError("期望 '}' 结束替换内容");
        return nullptr;
    }
    
    return node;
}

// === 辅助方法 ===

bool CustomParser::parseCustomInheritance(const std::vector<Token>& tokens, size_t& position, CustomStyle& customStyle) {
    // 解析自定义样式组的继承语句
    while (position < tokens.size() && 
           (tokens[position].type == TokenType::IDENTIFIER && tokens[position].value == "inherit") ||
           tokens[position].type == TokenType::AT) {
        
        if (tokens[position].value == "inherit") {
            position++; // 跳过 'inherit'
            
            if (position < tokens.size() && tokens[position].type == TokenType::AT) {
                position++; // 跳过 '@'
                
                Token typeToken = getCurrentToken(tokens, position);
                if (typeToken.type == TokenType::IDENTIFIER && typeToken.value == "Style") {
                    position++; // 跳过 'Style'
                    
                    Token nameToken = getCurrentToken(tokens, position);
                    if (nameToken.type == TokenType::IDENTIFIER) {
                        customStyle.inheritedTemplates.push_back(nameToken.value);
                        position++;
                    }
                }
            }
            
            // 跳过分号
            if (position < tokens.size() && tokens[position].type == TokenType::SEMICOLON) {
                position++;
            }
        } else {
            break;
        }
    }
    return true;
}

bool CustomParser::parseCustomInheritance(const std::vector<Token>& tokens, size_t& position, CustomElement& customElement) {
    // 解析自定义元素的继承语句（类似样式组）
    while (position < tokens.size() && 
           (tokens[position].type == TokenType::IDENTIFIER && tokens[position].value == "inherit") ||
           tokens[position].type == TokenType::AT) {
        
        if (tokens[position].value == "inherit") {
            position++; // 跳过 'inherit'
            
            if (position < tokens.size() && tokens[position].type == TokenType::AT) {
                position++; // 跳过 '@'
                
                Token typeToken = getCurrentToken(tokens, position);
                if (typeToken.type == TokenType::IDENTIFIER && typeToken.value == "Element") {
                    position++; // 跳过 'Element'
                    
                    Token nameToken = getCurrentToken(tokens, position);
                    if (nameToken.type == TokenType::IDENTIFIER) {
                        customElement.inheritedTemplates.push_back(nameToken.value);
                        position++;
                    }
                }
            }
            
            // 跳过分号
            if (position < tokens.size() && tokens[position].type == TokenType::SEMICOLON) {
                position++;
            }
        } else {
            break;
        }
    }
    return true;
}

bool CustomParser::parseCustomInheritance(const std::vector<Token>& tokens, size_t& position, CustomVariable& customVariable) {
    // 解析自定义变量组的继承语句（类似样式组）
    while (position < tokens.size() && 
           (tokens[position].type == TokenType::IDENTIFIER && tokens[position].value == "inherit") ||
           tokens[position].type == TokenType::AT) {
        
        if (tokens[position].value == "inherit") {
            position++; // 跳过 'inherit'
            
            if (position < tokens.size() && tokens[position].type == TokenType::AT) {
                position++; // 跳过 '@'
                
                Token typeToken = getCurrentToken(tokens, position);
                if (typeToken.type == TokenType::IDENTIFIER && typeToken.value == "Var") {
                    position++; // 跳过 'Var'
                    
                    Token nameToken = getCurrentToken(tokens, position);
                    if (nameToken.type == TokenType::IDENTIFIER) {
                        customVariable.inheritedTemplates.push_back(nameToken.value);
                        position++;
                    }
                }
            }
            
            // 跳过分号
            if (position < tokens.size() && tokens[position].type == TokenType::SEMICOLON) {
                position++;
            }
        } else {
            break;
        }
    }
    return true;
}

std::unique_ptr<ast::ASTNode> CustomParser::parseHTMLElement(const std::vector<Token>& tokens, size_t& position) {
    // 简化的HTML元素解析（需要更完整的实现）
    
    if (position >= tokens.size() || tokens[position].type != TokenType::IDENTIFIER) {
        return nullptr;
    }
    
    auto elementNode = std::make_unique<ast::ElementNode>();
    elementNode->tagName = tokens[position].value;
    position++;
    
    // TODO: 解析属性、子元素等
    
    return std::move(elementNode);
}

// === 基础方法 ===

bool CustomParser::expectToken(const std::vector<Token>& tokens, size_t& position, TokenType expectedType, const std::string& errorMessage) {
    if (position >= tokens.size() || tokens[position].type != expectedType) {
        if (!errorMessage.empty()) {
            addError(errorMessage);
        }
        return false;
    }
    position++;
    return true;
}

bool CustomParser::expectKeyword(const std::vector<Token>& tokens, size_t& position, const std::string& keyword) {
    if (position >= tokens.size() || 
        tokens[position].type != TokenType::IDENTIFIER ||
        tokens[position].value != keyword) {
        return false;
    }
    position++;
    return true;
}

Token CustomParser::getCurrentToken(const std::vector<Token>& tokens, size_t position) const {
    if (position < tokens.size()) {
        return tokens[position];
    }
    return Token(TokenType::END_OF_FILE, "", TokenPosition(0, 0));
}

void CustomParser::addError(const std::string& message) {
    errors_.push_back(message);
}

} // namespace custom_system
} // namespace chtl