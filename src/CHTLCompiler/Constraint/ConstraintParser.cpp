#include "ConstraintParser.h"
#include "../Lexer/CHTLToken.h"
#include <iostream>
#include <sstream>

namespace chtl {
namespace constraint_system {

ConstraintParser::ConstraintParser() : debugMode_(false) {
    ownedContext_ = std::make_unique<CHTLContext>();
    ownedStateManager_ = std::make_unique<StateManager>();
    context_ = ownedContext_.get();
    stateManager_ = ownedStateManager_.get();
    constraintManager_ = g_constraintManager;
}

ConstraintParser::ConstraintParser(CHTLContext& context, StateManager& stateManager) 
    : context_(&context), stateManager_(&stateManager), debugMode_(false) {
    constraintManager_ = g_constraintManager;
}

ConstraintParser::~ConstraintParser() = default;

std::unique_ptr<ast::ConstraintNode> ConstraintParser::parseConstraint(const std::vector<Token>& tokens, size_t& position) {
    // 按语法文档解析约束语句：except span, [Custom] @Element Box;
    
    if (!expectKeyword(tokens, position, "except")) {
        addError("期望 'except' 关键字");
        return nullptr;
    }
    
    auto constraintNode = std::make_unique<ast::ConstraintNode>();
    constraintNode->type = "except";
    
    // 解析约束目标列表
    std::vector<std::string> targets;
    
    do {
        std::string target = parseConstraintTarget(tokens, position);
        if (target.empty()) {
            addError("无效的约束目标");
            return nullptr;
        }
        
        targets.push_back(target);
        constraintNode->targets.push_back(target);
        
        // 检查是否有逗号继续
        if (position < tokens.size() && tokens[position].type == TokenType::COMMA) {
            position++; // 跳过 ','
        } else {
            break;
        }
    } while (position < tokens.size());
    
    if (!expectToken(tokens, position, TokenType::SEMICOLON)) {
        addError("期望 ';' 结束约束语句");
        return nullptr;
    }
    
    // 确定约束类型并创建约束规则
    ConstraintType constraintType = determineConstraintType(targets);
    
    ConstraintRule rule;
    rule.type = constraintType;
    rule.scope = getCurrentScope();
    rule.targets = targets;
    rule.isActive = true;
    
    // 解析目标类型
    for (const auto& target : targets) {
        ConstraintTargetType targetType = parseTargetType(target);
        rule.targetTypes.push_back(targetType);
    }
    
    // 注册到约束管理器
    constraintManager_->addConstraintRule(rule);
    
    return constraintNode;
}

std::string ConstraintParser::parseConstraintTarget(const std::vector<Token>& tokens, size_t& position) {
    // 解析单个约束目标
    
    if (position >= tokens.size()) {
        return "";
    }
    
    Token currentToken = tokens[position];
    
    if (currentToken.type == TokenType::LEFT_BRACKET) {
        // 解析类型约束：[Custom] @Element Box, [Template] @Style Text
        return parseTypedConstraintTarget(tokens, position);
    } else if (currentToken.type == TokenType::AT) {
        // 解析类型标识符约束：@Html, @Style
        return parseTypeIdentifierTarget(tokens, position);
    } else if (currentToken.type == TokenType::IDENTIFIER) {
        // 解析元素约束：span, div
        return parseElementTarget(tokens, position);
    } else {
        addError("无效的约束目标格式");
        return "";
    }
}

std::string ConstraintParser::parseTypedConstraintTarget(const std::vector<Token>& tokens, size_t& position) {
    // 解析 [Custom] @Element Box 或 [Template] @Style Text 格式
    
    if (!expectToken(tokens, position, TokenType::LEFT_BRACKET)) {
        return "";
    }
    
    Token typeToken = getCurrentToken(tokens, position);
    if (typeToken.type != TokenType::IDENTIFIER) {
        addError("期望约束类型");
        return "";
    }
    
    std::string constraintType = typeToken.value;
    position++;
    
    if (!expectToken(tokens, position, TokenType::RIGHT_BRACKET)) {
        addError("期望 ']' 结束约束类型");
        return "";
    }
    
    if (!expectToken(tokens, position, TokenType::AT)) {
        addError("期望 '@' 类型标识符");
        return "";
    }
    
    Token subTypeToken = getCurrentToken(tokens, position);
    if (subTypeToken.type != TokenType::IDENTIFIER) {
        addError("期望子类型标识符");
        return "";
    }
    
    std::string subType = subTypeToken.value;
    position++;
    
    // 检查是否有具体名称
    std::string targetName;
    if (position < tokens.size() && tokens[position].type == TokenType::IDENTIFIER) {
        targetName = tokens[position].value;
        position++;
    }
    
    // 构建完整的约束目标字符串
    std::string fullTarget = "[" + constraintType + "] @" + subType;
    if (!targetName.empty()) {
        fullTarget += " " + targetName;
    }
    
    return fullTarget;
}

std::string ConstraintParser::parseTypeIdentifierTarget(const std::vector<Token>& tokens, size_t& position) {
    // 解析 @Html, @Style, @JavaScript 格式
    
    if (!expectToken(tokens, position, TokenType::AT)) {
        return "";
    }
    
    Token typeToken = getCurrentToken(tokens, position);
    if (typeToken.type != TokenType::IDENTIFIER) {
        addError("期望类型标识符");
        return "";
    }
    
    std::string typeName = typeToken.value;
    position++;
    
    return "@" + typeName;
}

std::string ConstraintParser::parseElementTarget(const std::vector<Token>& tokens, size_t& position) {
    // 解析 span, div 等HTML元素格式
    
    Token elementToken = getCurrentToken(tokens, position);
    if (elementToken.type != TokenType::IDENTIFIER) {
        addError("期望元素名称");
        return "";
    }
    
    std::string elementName = elementToken.value;
    position++;
    
    // 检查是否有索引：div[0]
    if (position < tokens.size() && tokens[position].type == TokenType::LEFT_BRACKET) {
        position++; // 跳过 '['
        
        if (position < tokens.size() && tokens[position].type == TokenType::NUMBER) {
            elementName += "[" + tokens[position].value + "]";
            position++;
            
            if (!expectToken(tokens, position, TokenType::RIGHT_BRACKET)) {
                addError("期望 ']' 结束索引");
                return "";
            }
        } else {
            addError("期望索引数字");
            return "";
        }
    }
    
    return elementName;
}

// === 约束类型分析 ===

ConstraintType ConstraintParser::determineConstraintType(const std::vector<std::string>& targets) {
    // 根据约束目标确定约束类型
    
    bool hasTypeConstraint = false;
    bool hasPreciseConstraint = false;
    
    for (const auto& target : targets) {
        if (target.find("@") != std::string::npos || 
            target.find("[Custom]") != std::string::npos || 
            target.find("[Template]") != std::string::npos ||
            target.find("[Origin]") != std::string::npos) {
            hasTypeConstraint = true;
        } else {
            hasPreciseConstraint = true;
        }
    }
    
    if (hasTypeConstraint && hasPreciseConstraint) {
        // 混合约束，按语法文档处理为精确约束
        return ConstraintType::PRECISE;
    } else if (hasTypeConstraint) {
        // 纯类型约束
        return ConstraintType::TYPE;
    } else {
        // 纯精确约束
        return ConstraintType::PRECISE;
    }
}

ConstraintTargetType ConstraintParser::parseTargetType(const std::string& target) {
    // 解析约束目标的具体类型
    
    if (target.find("[Custom] @Element") != std::string::npos) {
        return ConstraintTargetType::CUSTOM_ELEMENT;
    } else if (target.find("[Custom] @Style") != std::string::npos) {
        return ConstraintTargetType::CUSTOM_STYLE;
    } else if (target.find("[Custom] @Var") != std::string::npos) {
        return ConstraintTargetType::CUSTOM_VAR;
    } else if (target.find("[Template] @Element") != std::string::npos) {
        return ConstraintTargetType::TEMPLATE_ELEMENT;
    } else if (target.find("[Template] @Style") != std::string::npos) {
        return ConstraintTargetType::TEMPLATE_STYLE;
    } else if (target.find("[Template] @Var") != std::string::npos) {
        return ConstraintTargetType::TEMPLATE_VAR;
    } else if (target.find("@Html") != std::string::npos) {
        return ConstraintTargetType::HTML_TYPE;
    } else if (target.find("[Custom]") != std::string::npos) {
        return ConstraintTargetType::CUSTOM_TYPE;
    } else if (target.find("[Template]") != std::string::npos) {
        return ConstraintTargetType::TEMPLATE_TYPE;
    } else if (target.find("[Origin]") != std::string::npos) {
        return ConstraintTargetType::ORIGIN_TYPE;
    } else if (isHTMLElement(target)) {
        return ConstraintTargetType::HTML_ELEMENT;
    }
    
    return ConstraintTargetType::HTML_ELEMENT; // 默认
}

bool ConstraintParser::isHTMLElement(const std::string& element) {
    // 检查是否为HTML元素（去除索引部分）
    std::string cleanElement = element;
    
    // 移除索引部分 [0], [1] 等
    size_t bracketPos = cleanElement.find('[');
    if (bracketPos != std::string::npos) {
        cleanElement = cleanElement.substr(0, bracketPos);
    }
    
    // 检查是否为标准HTML元素
    std::vector<std::string> htmlElements = {
        "div", "span", "p", "a", "img", "input", "button", "form", "table", "tr", "td",
        "ul", "ol", "li", "h1", "h2", "h3", "h4", "h5", "h6", "section", "article",
        "header", "footer", "nav", "main", "aside", "figure", "figcaption",
        "html", "head", "body", "title", "meta", "link", "script", "style"
    };
    
    return std::find(htmlElements.begin(), htmlElements.end(), cleanElement) != htmlElements.end();
}

// === 约束验证 ===

bool ConstraintParser::validateConstraintSyntax(const std::string& constraintStatement) {
    // 验证约束语句的语法正确性
    
    // 基本格式检查：必须以 "except" 开头，以 ";" 结尾
    if (constraintStatement.find("except") != 0) {
        addError("约束语句必须以 'except' 开头");
        return false;
    }
    
    if (constraintStatement.back() != ';') {
        addError("约束语句必须以 ';' 结尾");
        return false;
    }
    
    // 检查是否有有效的约束目标
    std::string targets = constraintStatement.substr(6); // 跳过 "except"
    targets = targets.substr(0, targets.length() - 1);   // 去掉结尾的 ';'
    
    if (targets.empty() || targets.find_first_not_of(" \t") == std::string::npos) {
        addError("约束语句必须包含约束目标");
        return false;
    }
    
    return true;
}

std::vector<std::string> ConstraintParser::extractConstraintTargets(const std::string& constraintStatement) {
    // 从约束语句中提取约束目标列表
    
    std::vector<std::string> targets;
    
    // 提取 "except" 和 ";" 之间的内容
    size_t startPos = constraintStatement.find("except") + 6;
    size_t endPos = constraintStatement.find_last_of(';');
    
    if (startPos >= endPos) {
        return targets;
    }
    
    std::string targetString = constraintStatement.substr(startPos, endPos - startPos);
    
    // 按逗号分割
    std::stringstream ss(targetString);
    std::string target;
    
    while (std::getline(ss, target, ',')) {
        // 去除前后空白
        target.erase(0, target.find_first_not_of(" \t"));
        target.erase(target.find_last_not_of(" \t") + 1);
        
        if (!target.empty()) {
            targets.push_back(target);
        }
    }
    
    return targets;
}

// === 作用域管理 ===

std::string ConstraintParser::getCurrentScope() {
    // 获取当前约束作用域
    return constraintManager_->getCurrentScope();
}

void ConstraintParser::enterConstraintScope(const std::string& scope) {
    // 进入约束作用域
    constraintManager_->enterScope(scope);
}

void ConstraintParser::exitConstraintScope(const std::string& scope) {
    // 退出约束作用域
    constraintManager_->exitScope(scope);
}

// === 约束应用 ===

bool ConstraintParser::applyConstraintToContext(const std::string& constraintStatement, const std::string& contextScope) {
    // 将约束应用到指定上下文
    
    if (!validateConstraintSyntax(constraintStatement)) {
        return false;
    }
    
    auto targets = extractConstraintTargets(constraintStatement);
    if (targets.empty()) {
        addError("约束语句没有有效目标");
        return false;
    }
    
    // 创建约束规则
    ConstraintRule rule;
    rule.type = determineConstraintType(targets);
    rule.scope = contextScope;
    rule.targets = targets;
    rule.isActive = true;
    
    // 解析目标类型
    for (const auto& target : targets) {
        ConstraintTargetType targetType = parseTargetType(target);
        rule.targetTypes.push_back(targetType);
    }
    
    // 添加到约束管理器
    constraintManager_->addConstraintRule(rule);
    
    return true;
}

// === 基础方法 ===

bool ConstraintParser::expectToken(const std::vector<Token>& tokens, size_t& position, TokenType expectedType, const std::string& errorMessage) {
    if (position >= tokens.size() || tokens[position].type != expectedType) {
        if (!errorMessage.empty()) addError(errorMessage); return false;
    }
    position++;
    return true;
}

bool ConstraintParser::expectKeyword(const std::vector<Token>& tokens, size_t& position, const std::string& keyword) {
    if (position >= tokens.size() || 
        tokens[position].type != TokenType::IDENTIFIER ||
        tokens[position].value != keyword) {
        return false;
    }
    position++;
    return true;
}

Token ConstraintParser::getCurrentToken(const std::vector<Token>& tokens, size_t position) const {
    if (position < tokens.size()) {
        return tokens[position];
    }
    return Token(TokenType::END_OF_FILE, "", TokenPosition(0, 0));
}

void ConstraintParser::addError(const std::string& message) {
    errors_.push_back(message);
}

} // namespace constraint_system
} // namespace chtl