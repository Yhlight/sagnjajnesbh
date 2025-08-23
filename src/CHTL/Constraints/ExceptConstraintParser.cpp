#include "CHTL/Constraints/ExceptConstraintParser.h"
#include "Utils/ErrorHandler.h"
#include <sstream>

namespace CHTL {
namespace Constraints {

ExceptConstraintParser::ExceptConstraintParser(const std::vector<Core::CHTLToken>& tokens)
    : tokens_(tokens) {}

std::vector<ConstraintRule> ExceptConstraintParser::ParseExceptConstraints(size_t& startPos) {
    std::vector<ConstraintRule> rules;
    
    if (!IsExceptKeyword(startPos)) {
        return rules;
    }
    
    // 跳过except关键字
    startPos++;
    SkipWhitespaceAndComments(startPos);
    
    // 解析约束列表（用逗号分隔）
    while (startPos < tokens_.size()) {
        ConstraintTarget target;
        std::string specificName;
        
        // 解析约束目标
        if (ParseConstraintTarget(startPos, target, specificName)) {
            ConstraintRule rule;
            
            // 根据解析结果确定约束类型
            if (!specificName.empty() && specificName.find("@") == std::string::npos && 
                specificName.find("[") == std::string::npos) {
                // 精确约束：具体的元素名称
                rule = ConstraintRuleBuilder::CreatePreciseRule(target, specificName, 
                    "精确约束: 禁止使用 " + specificName);
            } else if (specificName.find("[") != std::string::npos) {
                // 类型约束：[Custom]、[Template]等
                rule = ConstraintRuleBuilder::CreateTypeRule(target, 
                    "类型约束: 禁止使用 " + specificName);
            } else {
                // 类型约束：@Html等
                rule = ConstraintRuleBuilder::CreateTypeRule(target, 
                    "类型约束: 禁止使用 " + specificName);
            }
            
            rules.push_back(rule);
        }
        
        SkipWhitespaceAndComments(startPos);
        
        // 检查是否有逗号分隔符
        if (IsTokenType(startPos, Core::TokenType::COMMA)) {
            startPos++; // 跳过逗号
            SkipWhitespaceAndComments(startPos);
        } else if (IsTokenType(startPos, Core::TokenType::SEMICOLON)) {
            startPos++; // 跳过分号，结束解析
            break;
        } else {
            break; // 没有更多约束
        }
    }
    
    return rules;
}

bool ExceptConstraintParser::IsExceptKeyword(size_t pos) const {
    return pos < tokens_.size() && 
           tokens_[pos].GetType() == Core::TokenType::EXCEPT;
}

bool ExceptConstraintParser::ParseConstraintTarget(size_t& pos, ConstraintTarget& target, std::string& specificName) {
    SkipWhitespaceAndComments(pos);
    
    if (pos >= tokens_.size()) {
        return false;
    }
    
    const auto& token = tokens_[pos];
    
    // 检查类型标记
    if (token.GetType() == Core::TokenType::LEFT_BRACKET) {
        // 解析[Custom]、[Template]等类型约束
        return ParseTypeConstraintTarget(pos, target, specificName);
    } else if (token.GetType() == Core::TokenType::AT) {
        // 解析@Html、@Element等类型约束
        return ParseAtTypeConstraintTarget(pos, target, specificName);
    } else if (token.GetType() == Core::TokenType::IDENTIFIER) {
        // 解析HTML元素名称的精确约束
        specificName = token.GetValue();
        target = ConstraintTarget::HTML_ELEMENT;
        pos++;
        return true;
    }
    
    return false;
}

bool ExceptConstraintParser::ParseTypeConstraintTarget(size_t& pos, ConstraintTarget& target, std::string& specificName) {
    // 解析[Custom]、[Template]等
    if (!ConsumeToken(pos, Core::TokenType::LEFT_BRACKET)) {
        return false;
    }
    
    SkipWhitespaceAndComments(pos);
    
    if (pos >= tokens_.size()) {
        return false;
    }
    
    std::string typeKeyword = GetTokenValue(pos);
    pos++;
    
    SkipWhitespaceAndComments(pos);
    
    if (!ConsumeToken(pos, Core::TokenType::RIGHT_BRACKET)) {
        return false;
    }
    
    specificName = "[" + typeKeyword + "]";
    
    // 检查是否有后续的@Element、@Style、@Var等
    SkipWhitespaceAndComments(pos);
    
    if (pos < tokens_.size() && tokens_[pos].GetType() == Core::TokenType::AT) {
        std::string atType;
        if (ParseAtTypeConstraintTarget(pos, target, atType)) {
            specificName += " " + atType;
            
            // 根据组合确定目标类型
            if (atType.find("@Element") != std::string::npos) {
                if (typeKeyword == "Custom") {
                    target = ConstraintTarget::CUSTOM_ELEMENT;
                } else if (typeKeyword == "Template") {
                    target = ConstraintTarget::TEMPLATE_ELEMENT;
                }
            } else if (atType.find("@Style") != std::string::npos) {
                if (typeKeyword == "Custom") {
                    target = ConstraintTarget::CUSTOM_STYLE;
                } else if (typeKeyword == "Template") {
                    target = ConstraintTarget::TEMPLATE_STYLE;
                }
            } else if (atType.find("@Var") != std::string::npos) {
                if (typeKeyword == "Custom") {
                    target = ConstraintTarget::CUSTOM_VAR;
                } else if (typeKeyword == "Template") {
                    target = ConstraintTarget::TEMPLATE_VAR;
                }
            }
        }
    } else {
        // 纯类型约束
        if (typeKeyword == "Custom") {
            target = ConstraintTarget::CUSTOM_TYPE;
        } else if (typeKeyword == "Template") {
            target = ConstraintTarget::TEMPLATE_TYPE;
        }
    }
    
    return true;
}

bool ExceptConstraintParser::ParseAtTypeConstraintTarget(size_t& pos, ConstraintTarget& target, std::string& specificName) {
    // 解析@Html、@Element等
    if (!ConsumeToken(pos, Core::TokenType::AT)) {
        return false;
    }
    
    SkipWhitespaceAndComments(pos);
    
    if (pos >= tokens_.size()) {
        return false;
    }
    
    std::string atKeyword = GetTokenValue(pos);
    pos++;
    
    specificName = "@" + atKeyword;
    
    // 根据关键字确定目标类型
    if (atKeyword == "Html") {
        target = ConstraintTarget::HTML_TYPE;
    } else if (atKeyword == "Element") {
        target = ConstraintTarget::HTML_ELEMENT; // 默认为HTML元素
    } else if (atKeyword == "Style") {
        target = ConstraintTarget::CUSTOM_STYLE; // 默认为自定义样式
    } else if (atKeyword == "Var") {
        target = ConstraintTarget::CUSTOM_VAR; // 默认为自定义变量
    } else {
        target = ConstraintTarget::HTML_ELEMENT; // 默认
    }
    
    // 检查是否有具体的名称
    SkipWhitespaceAndComments(pos);
    
    if (pos < tokens_.size() && tokens_[pos].GetType() == Core::TokenType::IDENTIFIER) {
        std::string elementName = GetTokenValue(pos);
        pos++;
        specificName += " " + elementName;
    }
    
    return true;
}

void ExceptConstraintParser::SkipWhitespaceAndComments(size_t& pos) {
    while (pos < tokens_.size()) {
        const auto& token = tokens_[pos];
        if (token.GetType() == Core::TokenType::WHITESPACE || 
            token.GetType() == Core::TokenType::SINGLE_LINE_COMMENT ||
            token.GetType() == Core::TokenType::MULTI_LINE_COMMENT ||
            token.GetType() == Core::TokenType::GENERATOR_COMMENT) {
            pos++;
        } else {
            break;
        }
    }
}

bool ExceptConstraintParser::IsTokenType(size_t pos, Core::TokenType type) const {
    return pos < tokens_.size() && tokens_[pos].GetType() == type;
}

std::string ExceptConstraintParser::GetTokenValue(size_t pos) const {
    return (pos < tokens_.size()) ? tokens_[pos].GetValue() : "";
}

bool ExceptConstraintParser::ConsumeToken(size_t& pos, Core::TokenType type) {
    if (IsTokenType(pos, type)) {
        pos++;
        return true;
    }
    return false;
}

std::string ExceptConstraintParser::ParseIdentifier(size_t& pos) {
    SkipWhitespaceAndComments(pos);
    
    if (IsTokenType(pos, Core::TokenType::IDENTIFIER)) {
        std::string identifier = GetTokenValue(pos);
        pos++;
        return identifier;
    }
    
    return "";
}

// ExceptConstraintIntegrator实现
ExceptConstraintIntegrator::ExceptConstraintIntegrator(CHTLConstraintValidator& validator)
    : validator_(validator) {}

bool ExceptConstraintIntegrator::ProcessExceptConstraints(const std::vector<Core::CHTLToken>& tokens,
                                                         SyntaxContext context,
                                                         size_t startPos) {
    ExceptConstraintParser parser(tokens);
    size_t pos = startPos;
    
    try {
        auto rules = parser.ParseExceptConstraints(pos);
        ApplyConstraintRules(context, rules);
        
        Utils::ErrorHandler::GetInstance().LogInfo(
            "成功处理except约束，添加了 " + std::to_string(rules.size()) + " 个约束规则"
        );
        
        return true;
    } catch (const std::exception& e) {
        Utils::ErrorHandler::GetInstance().LogError(
            "except约束处理异常: " + std::string(e.what())
        );
        return false;
    }
}

void ExceptConstraintIntegrator::ApplyConstraintRules(SyntaxContext context, const std::vector<ConstraintRule>& rules) {
    for (const auto& rule : rules) {
        validator_.AddExceptRule(context, rule);
    }
}

} // namespace Constraints
} // namespace CHTL