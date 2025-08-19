#include "CHTLParser.h"
#include "../AST/CHTLNodes.h"
#include <iostream>
#include <sstream>

namespace chtl {
namespace parser {

CHTLParser::CHTLParser() : current_token_(0), debug_mode_(false) {
    owned_context_ = std::make_unique<CHTLContext>();
    context_ = owned_context_.get();
    
    // 初始化状态管理器和专门解析器
    state_manager_ = std::make_unique<StateManager>();
    // 安全初始化专门Parser，避免段错误
    template_parser_ = nullptr;
    custom_parser_ = nullptr;
    origin_parser_ = nullptr;
    constraint_parser_ = nullptr;
}

CHTLParser::CHTLParser(CHTLContext& context) 
    : current_token_(0), context_(&context), debug_mode_(false) {
    
    // 初始化状态管理器和专门解析器
    state_manager_ = std::make_unique<StateManager>();
    // 安全初始化专门Parser，避免段错误
    template_parser_ = nullptr;
    custom_parser_ = nullptr;
    origin_parser_ = nullptr;
    constraint_parser_ = nullptr;
}

void CHTLParser::setTokens(const std::vector<Token>& tokens) {
    tokens_ = tokens;
    current_token_ = 0;
    clearErrors();
}

void CHTLParser::setInput(const std::string& input) {
    // 简化实现：使用词法分析器
    // 这里应该调用词法分析器，但为了简化，我们跳过
    addError("setInput方法需要词法分析器支持");
}

std::unique_ptr<ast::ASTNode> CHTLParser::parseProgram() {
    if (debug_mode_) {
        std::cout << "开始解析程序" << std::endl;
    }
    
    auto program = std::make_unique<ast::ProgramNode>();
    
    while (!isAtEnd()) {
        try {
            auto stmt = parseStatement();
            if (stmt) {
                program->addChild(std::move(stmt));
            }
        } catch (const std::exception& e) {
            addError("解析语句时发生异常: " + std::string(e.what()));
            synchronize();
        }
    }
    
    if (debug_mode_) {
        std::cout << "程序解析完成，共 " << program->getChildCount() << " 个语句" << std::endl;
    }
    
    return std::move(program);
}

std::unique_ptr<ast::ASTNode> CHTLParser::parseStatement() {
    Token token = getCurrentToken();
    
    // 更新状态机
    if (!state_manager_->isInState(CompilerState::PARSING_CHTL)) {
        state_manager_->transition(CompilerState::PARSING_CHTL, SyntaxContext::GLOBAL);
    }
    
    switch (token.type) {
        case TokenType::LEFT_BRACKET: {
            // 检查括号内的关键字
            Token nextToken = peekToken(1);
            if (nextToken.type == TokenType::TEMPLATE) {
                return parseTemplate();
            } else if (nextToken.type == TokenType::CUSTOM) {
                return parseCustom();
            } else if (nextToken.type == TokenType::ORIGIN) {
                return parseOrigin();
            } else if (nextToken.type == TokenType::CONFIGURATION) {
                return parseConfiguration();
            } else if (nextToken.type == TokenType::IMPORT) {
                return parseImport();
            } else if (nextToken.type == TokenType::NAMESPACE) {
                return parseNamespace();
            } else {
                addError("未识别的括号内语句类型: " + nextToken.value);
                advance();
                return nullptr;
            }
        }
        case TokenType::EXCEPT:
            return parseConstraint();
        case TokenType::AT:
            // 可能是模板/自定义使用或变量使用
            return parseTemplateUsage();
        case TokenType::IDENTIFIER: {
            // 检查是否为变量调用格式 TemplateName(variableName)
            Token nextToken = peekToken(1);
            if (nextToken.type == TokenType::LEFT_PAREN) {
                return parseVariableUsage();
            } else {
                return parseElement();
            }
        }
        default:
            addError("未识别的语句类型: " + token.value);
            advance(); // 跳过未知token
            return nullptr;
    }
}

std::unique_ptr<ast::ASTNode> CHTLParser::parseTemplate() {
    if (debug_mode_) {
        std::cout << "使用专门的模板解析器解析模板定义" << std::endl;
    }
    
    // 委托给专门的模板解析器
    return template_parser_->parseTemplate(tokens_, current_token_);
}

std::unique_ptr<ast::ASTNode> CHTLParser::parseCustom() {
    if (debug_mode_) {
        std::cout << "使用专门的自定义解析器解析自定义定义" << std::endl;
    }
    
    // 委托给专门的自定义解析器
    return custom_parser_->parseCustom(tokens_, current_token_);
}

std::unique_ptr<ast::ASTNode> CHTLParser::parseOrigin() {
    if (debug_mode_) {
        std::cout << "使用专门的原始嵌入解析器解析原始嵌入" << std::endl;
    }
    
    // 委托给专门的原始嵌入解析器
    return origin_parser_->parseOrigin(tokens_, current_token_);
}

std::unique_ptr<ast::ASTNode> CHTLParser::parseConfiguration() {
    if (debug_mode_) {
        std::cout << "解析配置组定义" << std::endl;
    }
    
    // TODO: 实现配置组解析
    auto config_node = std::make_unique<ast::ConfigurationNode>();
    
    return std::move(config_node);
}

std::unique_ptr<ast::ASTNode> CHTLParser::parseNamespace() {
    if (debug_mode_) {
        std::cout << "解析命名空间定义" << std::endl;
    }
    
    // TODO: 实现命名空间解析
    auto namespace_node = std::make_unique<ast::NamespaceNode>();
    
    return std::move(namespace_node);
}

std::unique_ptr<ast::ASTNode> CHTLParser::parseImport() {
    if (debug_mode_) {
        std::cout << "解析导入语句" << std::endl;
    }
    
    // TODO: 实现导入解析
    auto import_node = std::make_unique<ast::ImportNode>();
    
    return std::move(import_node);
}

std::unique_ptr<ast::ASTNode> CHTLParser::parseConstraint() {
    if (debug_mode_) {
        std::cout << "使用专门的约束解析器解析约束" << std::endl;
    }
    
    // 委托给专门的约束解析器
    return constraint_parser_->parseConstraint(tokens_, current_token_);
}

std::unique_ptr<ast::ASTNode> CHTLParser::parseSpecialization() {
    if (debug_mode_) {
        std::cout << "解析特例化操作" << std::endl;
    }
    
    // TODO: 实现特例化操作解析
    auto specialization_node = std::make_unique<ast::SpecializationNode>();
    
    return std::move(specialization_node);
}

std::unique_ptr<ast::ASTNode> CHTLParser::parseIndexAccess() {
    if (debug_mode_) {
        std::cout << "解析索引访问" << std::endl;
    }
    
    // TODO: 实现索引访问解析
    auto index_node = std::make_unique<ast::IndexAccessNode>();
    
    return std::move(index_node);
}

std::unique_ptr<ast::ASTNode> CHTLParser::parseTemplateUsage() {
    if (debug_mode_) {
        std::cout << "解析模板使用" << std::endl;
    }
    
    // TODO: 实现模板使用解析
    auto template_node = std::make_unique<ast::TemplateNode>();
    
    return std::move(template_node);
}

std::unique_ptr<ast::ASTNode> CHTLParser::parseCustomUsage() {
    if (debug_mode_) {
        std::cout << "解析自定义使用" << std::endl;
    }
    
    // TODO: 实现自定义使用解析
    auto custom_node = std::make_unique<ast::CustomNode>();
    
    return std::move(custom_node);
}

std::unique_ptr<ast::ASTNode> CHTLParser::parseOriginUsage() {
    if (debug_mode_) {
        std::cout << "解析原始嵌入使用" << std::endl;
    }
    
    // TODO: 实现原始嵌入使用解析
    auto origin_node = std::make_unique<ast::OriginNode>();
    
    return std::move(origin_node);
}

std::unique_ptr<ast::ASTNode> CHTLParser::parseVariableUsage() {
    if (debug_mode_) {
        std::cout << "解析变量组使用" << std::endl;
    }
    
    // TODO: 实现变量组使用解析 ThemeColor(tableColor)
    auto variable_node = std::make_unique<ast::VariableGroupNode>();
    
    return std::move(variable_node);
}

std::unique_ptr<ast::ASTNode> CHTLParser::parseElement() {
    if (debug_mode_) {
        std::cout << "解析元素" << std::endl;
    }
    
    auto element_node = std::make_unique<ast::ElementNode>();
    
    // 1. 解析标签名
    if (check(TokenType::IDENTIFIER)) {
        element_node->tag = getCurrentToken().value;
        advance();
    } else {
        addError("期望元素标签名");
        return nullptr;
    }
    
    // 2. 解析元素体
    if (check(TokenType::LEFT_BRACE)) {
        advance(); // 消费 '{'
        
        // 解析元素内容（属性和子元素）
        while (!check(TokenType::RIGHT_BRACE) && !isAtEnd()) {
            if (check(TokenType::IDENTIFIER)) {
                Token current = getCurrentToken();
                Token next = peekToken(1);
                
                if (next.type == TokenType::COLON) {
                    // 这是属性：name: value;
                    std::string attrName = current.value;
                    advance(); // 消费属性名
                    advance(); // 消费 ':'
                    
                    if (check(TokenType::IDENTIFIER) || check(TokenType::STRING)) {
                        std::string attrValue = getCurrentToken().value;
                        element_node->attributes.emplace_back(attrName, attrValue);
                        advance(); // 消费属性值
                        
                        if (check(TokenType::SEMICOLON)) {
                            advance(); // 消费 ';'
                        }
                    }
                } else if (next.type == TokenType::LEFT_BRACE) {
                    // 这是子元素
                    auto child = parseElement();
                    if (child) {
                        element_node->addChild(std::move(child));
                    }
                } else if (current.value == "text" && next.type == TokenType::LEFT_BRACE) {
                    // 这是文本内容：text { content }
                    advance(); // 消费 'text'
                    advance(); // 消费 '{'
                    
                    if (check(TokenType::IDENTIFIER) || check(TokenType::STRING)) {
                        auto text_node = std::make_unique<ast::TextNode>();
                        text_node->content = getCurrentToken().value;
                        element_node->addChild(std::move(text_node));
                        advance(); // 消费文本内容
                    }
                    
                    if (check(TokenType::RIGHT_BRACE)) {
                        advance(); // 消费 '}'
                    }
                } else {
                    addError("未识别的元素内容: " + current.value);
                    advance();
                }
            } else {
                addError("期望标识符");
                advance();
            }
        }
        
        if (check(TokenType::RIGHT_BRACE)) {
            advance(); // 消费 '}'
        } else {
            addError("期望 '}'");
        }
    }
    
    return std::move(element_node);
}

bool CHTLParser::hasMoreTokens() const {
    return !isAtEnd();
}

Token CHTLParser::getCurrentToken() const {
    if (isAtEnd()) {
        return Token(TokenType::EOF_TOKEN, "", TokenPosition());
    }
    return tokens_[current_token_];
}

Token CHTLParser::peekToken(size_t offset) const {
    size_t pos = current_token_ + offset;
    if (pos >= tokens_.size()) {
        return Token(TokenType::EOF_TOKEN, "", TokenPosition());
    }
    return tokens_[pos];
}

std::vector<std::string> CHTLParser::getErrors() const {
    return errors_;
}

void CHTLParser::clearErrors() {
    errors_.clear();
}

void CHTLParser::advance() {
    if (!isAtEnd()) {
        current_token_++;
    }
}

bool CHTLParser::match(TokenType type) {
    if (check(type)) {
        advance();
        return true;
    }
    return false;
}

bool CHTLParser::check(TokenType type) const {
    if (isAtEnd()) return false;
    return getCurrentToken().type == type;
}

Token CHTLParser::consume(TokenType type, const std::string& message) {
    if (check(type)) {
        Token token = getCurrentToken();
        advance();
        return token;
    }
    
    addError(message + "，但得到了 " + getCurrentToken().value);
    return Token(TokenType::INVALID, "", TokenPosition());
}

void CHTLParser::addError(const std::string& message) {
    std::ostringstream oss;
    Token token = getCurrentToken();
    oss << "行 " << token.position.line << ", 列 " << token.position.column 
        << ": " << message;
    
    errors_.push_back(oss.str());
    
    if (debug_mode_) {
        std::cerr << "解析错误: " << oss.str() << std::endl;
    }
}

void CHTLParser::synchronize() {
    advance();
    
    while (!isAtEnd()) {
        if (getCurrentToken().type == TokenType::SEMICOLON) {
            advance();
            return;
        }
        
        switch (getCurrentToken().type) {
            case TokenType::TEMPLATE:
            case TokenType::CUSTOM:
            case TokenType::IMPORT:
            case TokenType::NAMESPACE:
                return;
            default:
                break;
        }
        
        advance();
    }
}

bool CHTLParser::isAtEnd() const {
    return current_token_ >= tokens_.size() || getCurrentToken().type == TokenType::EOF_TOKEN;
}

std::string CHTLParser::tokenTypeToString(TokenType type) const {
    return Token::typeToString(type);
}

} // namespace parser
} // namespace chtl