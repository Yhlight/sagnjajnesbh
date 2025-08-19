#include "OriginParser.h"
#include "../Lexer/CHTLToken.h"
#include <iostream>

namespace chtl {
namespace origin_system {

OriginParser::OriginParser() : debugMode_(false) {
    ownedContext_ = std::make_unique<CHTLContext>();
    ownedStateManager_ = std::make_unique<StateManager>();
    context_ = ownedContext_.get();
    stateManager_ = ownedStateManager_.get();
    // 延迟初始化Manager
    originManager_ = nullptr;
}

OriginParser::OriginParser(CHTLContext& context, StateManager& stateManager) 
    : context_(&context), stateManager_(&stateManager), debugMode_(false) {
    // 延迟初始化Manager
    originManager_ = nullptr;
}

OriginParser::~OriginParser() = default;

std::shared_ptr<OriginManager> OriginParser::getOriginManager() {
    if (!originManager_) {
        originManager_ = g_originManager;
    }
    return originManager_;
}

std::unique_ptr<ast::OriginNode> OriginParser::parseOrigin(const std::vector<Token>& tokens, size_t& position) {
    // 按语法文档解析 [Origin] @Html/@Style/@JavaScript 块
    
    if (!expectToken(tokens, position, TokenType::LEFT_BRACKET)) {
        addError("期望 '[' 开始原始嵌入块");
        return nullptr;
    }
    
    if (!expectKeyword(tokens, position, "Origin")) {
        addError("期望 'Origin' 关键字");
        return nullptr;
    }
    
    if (!expectToken(tokens, position, TokenType::RIGHT_BRACKET)) {
        addError("期望 ']' 结束原始嵌入标识");
        return nullptr;
    }
    
    // 解析类型标识符：@Html, @Style, @JavaScript, 或自定义类型
    if (!expectToken(tokens, position, TokenType::AT)) {
        addError("期望 '@' 类型标识符");
        return nullptr;
    }
    
    Token typeToken = getCurrentToken(tokens, position);
    if (typeToken.type != TokenType::IDENTIFIER) {
        addError("期望原始嵌入类型标识符");
        return nullptr;
    }
    
    std::string originType = "@" + typeToken.value;
    position++;
    
    // 创建AST节点
    auto originNode = std::make_unique<ast::OriginNode>();
    originNode->originType = originType;
    
    // 检查是否有名称（带名原始嵌入）
    if (position < tokens.size() && tokens[position].type == TokenType::IDENTIFIER) {
        originNode->name = tokens[position].value;
        originNode->isNamed = true;
        position++;
    } else {
        originNode->isNamed = false;
    }
    
    // 解析原始内容块
    if (!expectToken(tokens, position, TokenType::LEFT_BRACE)) {
        addError("期望 '{' 开始原始内容块");
        return nullptr;
    }
    
    // 根据类型解析内容
    if (originType == "@Html") {
        return parseHTMLOrigin(tokens, position, std::move(originNode));
    } else if (originType == "@Style") {
        return parseStyleOrigin(tokens, position, std::move(originNode));
    } else if (originType == "@JavaScript") {
        return parseJavaScriptOrigin(tokens, position, std::move(originNode));
    } else {
        // 自定义原始类型
        return parseCustomOrigin(tokens, position, std::move(originNode));
    }
}

std::unique_ptr<ast::OriginNode> OriginParser::parseHTMLOrigin(const std::vector<Token>& tokens, size_t& position,
                                                              std::unique_ptr<ast::OriginNode> originNode) {
    // 按语法文档解析HTML原始嵌入内容
    
    std::string htmlContent;
    int braceLevel = 1; // 已经读取了开始的 '{'
    
    while (position < tokens.size() && braceLevel > 0) {
        Token token = tokens[position];
        
        if (token.type == TokenType::LEFT_BRACE) {
            braceLevel++;
            htmlContent += "{";
        } else if (token.type == TokenType::RIGHT_BRACE) {
            braceLevel--;
            if (braceLevel > 0) {
                htmlContent += "}";
            }
        } else {
            // 保留原始token内容
            htmlContent += token.value;
            
            // 在token之间添加适当的空格
            if (position + 1 < tokens.size() && 
                needsSpaceBetween(token, tokens[position + 1])) {
                htmlContent += " ";
            }
        }
        
        position++;
    }
    
    // 验证HTML内容
    if (!originManager_->validateOriginContent(htmlContent, originNode->originType)) {
        addError("HTML原始内容验证失败");
        return nullptr;
    }
    
    originNode->content = htmlContent;
    
    // 注册到管理器
    if (originNode->isNamed) {
        originManager_->addNamedOrigin(originNode->originType, originNode->name, htmlContent);
    } else {
        OriginItem item;
        item.type = OriginType::HTML;
        item.typeName = originNode->originType;
        item.content = htmlContent;
        item.isNamed = false;
        originManager_->addOriginItem(item);
    }
    
    return originNode;
}

std::unique_ptr<ast::OriginNode> OriginParser::parseStyleOrigin(const std::vector<Token>& tokens, size_t& position,
                                                               std::unique_ptr<ast::OriginNode> originNode) {
    // 按语法文档解析CSS原始嵌入内容
    
    std::string cssContent;
    int braceLevel = 1; // 已经读取了开始的 '{'
    
    while (position < tokens.size() && braceLevel > 0) {
        Token token = tokens[position];
        
        if (token.type == TokenType::LEFT_BRACE) {
            braceLevel++;
            cssContent += "{";
        } else if (token.type == TokenType::RIGHT_BRACE) {
            braceLevel--;
            if (braceLevel > 0) {
                cssContent += "}";
            }
        } else {
            // 保留原始token内容
            cssContent += token.value;
            
            // 在token之间添加适当的空格
            if (position + 1 < tokens.size() && 
                needsSpaceBetween(token, tokens[position + 1])) {
                cssContent += " ";
            }
        }
        
        position++;
    }
    
    // 验证CSS内容
    if (!originManager_->validateOriginContent(cssContent, originNode->originType)) {
        addError("CSS原始内容验证失败");
        return nullptr;
    }
    
    originNode->content = cssContent;
    
    // 注册到管理器
    if (originNode->isNamed) {
        originManager_->addNamedOrigin(originNode->originType, originNode->name, cssContent);
    } else {
        OriginItem item;
        item.type = OriginType::STYLE;
        item.typeName = originNode->originType;
        item.content = cssContent;
        item.isNamed = false;
        originManager_->addOriginItem(item);
    }
    
    return originNode;
}

std::unique_ptr<ast::OriginNode> OriginParser::parseJavaScriptOrigin(const std::vector<Token>& tokens, size_t& position,
                                                                    std::unique_ptr<ast::OriginNode> originNode) {
    // 按语法文档解析JavaScript原始嵌入内容
    
    std::string jsContent;
    int braceLevel = 1; // 已经读取了开始的 '{'
    
    while (position < tokens.size() && braceLevel > 0) {
        Token token = tokens[position];
        
        if (token.type == TokenType::LEFT_BRACE) {
            braceLevel++;
            jsContent += "{";
        } else if (token.type == TokenType::RIGHT_BRACE) {
            braceLevel--;
            if (braceLevel > 0) {
                jsContent += "}";
            }
        } else {
            // 保留原始token内容
            jsContent += token.value;
            
            // 在token之间添加适当的空格
            if (position + 1 < tokens.size() && 
                needsSpaceBetween(token, tokens[position + 1])) {
                jsContent += " ";
            }
        }
        
        position++;
    }
    
    // 验证JavaScript内容
    if (!originManager_->validateOriginContent(jsContent, originNode->originType)) {
        addError("JavaScript原始内容验证失败");
        return nullptr;
    }
    
    originNode->content = jsContent;
    
    // 注册到管理器
    if (originNode->isNamed) {
        originManager_->addNamedOrigin(originNode->originType, originNode->name, jsContent);
    } else {
        OriginItem item;
        item.type = OriginType::JAVASCRIPT;
        item.typeName = originNode->originType;
        item.content = jsContent;
        item.isNamed = false;
        originManager_->addOriginItem(item);
    }
    
    return originNode;
}

std::unique_ptr<ast::OriginNode> OriginParser::parseCustomOrigin(const std::vector<Token>& tokens, size_t& position,
                                                                std::unique_ptr<ast::OriginNode> originNode) {
    // 按语法文档解析自定义原始嵌入内容（如@Vue, @React等）
    
    // 首先验证自定义类型是否已注册
    if (!originManager_->isCustomOriginTypeRegistered(originNode->originType)) {
        addError("未注册的自定义原始类型: " + originNode->originType);
        return nullptr;
    }
    
    std::string customContent;
    int braceLevel = 1; // 已经读取了开始的 '{'
    
    while (position < tokens.size() && braceLevel > 0) {
        Token token = tokens[position];
        
        if (token.type == TokenType::LEFT_BRACE) {
            braceLevel++;
            customContent += "{";
        } else if (token.type == TokenType::RIGHT_BRACE) {
            braceLevel--;
            if (braceLevel > 0) {
                customContent += "}";
            }
        } else {
            // 保留原始token内容
            customContent += token.value;
            
            // 在token之间添加适当的空格
            if (position + 1 < tokens.size() && 
                needsSpaceBetween(token, tokens[position + 1])) {
                customContent += " ";
            }
        }
        
        position++;
    }
    
    // 验证自定义内容
    if (!originManager_->validateOriginContent(customContent, originNode->originType)) {
        addError("自定义原始内容验证失败");
        return nullptr;
    }
    
    originNode->content = customContent;
    
    // 注册到管理器
    if (originNode->isNamed) {
        originManager_->addNamedOrigin(originNode->originType, originNode->name, customContent);
    } else {
        OriginItem item;
        item.type = OriginType::CUSTOM;
        item.typeName = originNode->originType;
        item.content = customContent;
        item.isNamed = false;
        originManager_->addOriginItem(item);
    }
    
    return originNode;
}

// === 原始嵌入使用解析 ===

std::unique_ptr<ast::ASTNode> OriginParser::parseOriginUsage(const std::vector<Token>& tokens, size_t& position) {
    // 解析原始嵌入的使用：使用已定义的原始嵌入
    
    // 这里应该解析对已定义原始嵌入的引用
    // 例如：使用之前定义的 [Origin] @Html myComponent
    
    Token typeToken = getCurrentToken(tokens, position);
    if (typeToken.type != TokenType::AT) {
        addError("期望 '@' 原始类型标识符");
        return nullptr;
    }
    position++;
    
    Token nameToken = getCurrentToken(tokens, position);
    if (nameToken.type != TokenType::IDENTIFIER) {
        addError("期望原始嵌入类型名");
        return nullptr;
    }
    
    std::string originType = "@" + nameToken.value;
    position++;
    
    // 检查是否指定了名称
    std::string originName;
    if (position < tokens.size() && tokens[position].type == TokenType::IDENTIFIER) {
        originName = tokens[position].value;
        position++;
    }
    
    // 验证原始嵌入是否存在
    if (!originName.empty()) {
        if (!originManager_->hasNamedOrigin(originType, originName)) {
            addError("带名原始嵌入不存在: " + originType + " " + originName);
            return nullptr;
        }
    } else {
        auto items = originManager_->getOriginItemsByTypeName(originType);
        if (items.empty()) {
            addError("原始嵌入类型不存在: " + originType);
            return nullptr;
        }
    }
    
    // 创建使用节点
    auto usageNode = std::make_unique<ast::OriginNode>();
    usageNode->originType = originType;
    usageNode->name = originName;
    usageNode->isNamed = !originName.empty();
    usageNode->isUsage = true; // 标记为使用而非定义
    
    return std::move(usageNode);
}

// === 辅助方法 ===

bool OriginParser::needsSpaceBetween(const Token& current, const Token& next) {
    // 判断两个token之间是否需要空格
    
    // 标识符和标识符之间需要空格
    if (current.type == TokenType::IDENTIFIER && next.type == TokenType::IDENTIFIER) {
        return true;
    }
    
    // 数字和标识符之间需要空格
    if (current.type == TokenType::NUMBER && next.type == TokenType::IDENTIFIER) {
        return true;
    }
    
    // 标识符和数字之间需要空格
    if (current.type == TokenType::IDENTIFIER && next.type == TokenType::NUMBER) {
        return true;
    }
    
    // 字符串后面通常需要空格
    if (current.type == TokenType::STRING) {
        return true;
    }
    
    // 冒号后面需要空格
    if (current.type == TokenType::COLON) {
        return true;
    }
    
    // 分号后面需要空格
    if (current.type == TokenType::SEMICOLON) {
        return true;
    }
    
    // 逗号后面需要空格
    if (current.type == TokenType::COMMA) {
        return true;
    }
    
    return false;
}

bool OriginParser::isOriginTypeToken(const Token& token) {
    // 检查token是否为原始嵌入类型
    if (token.type != TokenType::IDENTIFIER) {
        return false;
    }
    
    return token.value == "Html" || 
           token.value == "Style" || 
           token.value == "JavaScript" ||
           originManager_->isCustomOriginTypeRegistered("@" + token.value);
}

std::string OriginParser::extractRawContent(const std::vector<Token>& tokens, size_t start, size_t end) {
    // 从token序列中提取原始内容
    
    std::string content;
    for (size_t i = start; i < end && i < tokens.size(); ++i) {
        content += tokens[i].value;
        
        // 在需要的地方添加空格
        if (i + 1 < end && i + 1 < tokens.size() && 
            needsSpaceBetween(tokens[i], tokens[i + 1])) {
            content += " ";
        }
    }
    
    return content;
}

// === 基础方法 ===

bool OriginParser::expectToken(const std::vector<Token>& tokens, size_t& position, TokenType expectedType, const std::string& errorMessage) {
    if (position >= tokens.size() || tokens[position].type != expectedType) {
        if (!errorMessage.empty()) {
            addError(errorMessage);
        }
        return false;
    }
    position++;
    return true;
}

bool OriginParser::expectKeyword(const std::vector<Token>& tokens, size_t& position, const std::string& keyword) {
    if (position >= tokens.size() || 
        tokens[position].type != TokenType::IDENTIFIER ||
        tokens[position].value != keyword) {
        return false;
    }
    position++;
    return true;
}

Token OriginParser::getCurrentToken(const std::vector<Token>& tokens, size_t position) const {
    if (position < tokens.size()) {
        return tokens[position];
    }
    return Token(TokenType::END_OF_FILE, "", TokenPosition(0, 0));
}

void OriginParser::addError(const std::string& message) {
    errors_.push_back(message);
}

} // namespace origin_system
} // namespace chtl