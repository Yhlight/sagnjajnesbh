#include "Parser.h"
#include <iostream>
#include <sstream>

namespace chtl {
namespace parser {

CHTLParser::CHTLParser(CompilerContext& context)
    : context_(context), current_token_(TokenType::INVALID, ""), peek_token_(TokenType::INVALID, "") {
}

void CHTLParser::setInput(const std::string& input) {
    lexer_ = std::make_unique<Lexer>(context_);
    lexer_->setInput(input);
    advance(); // 初始化current_token_
}

void CHTLParser::setInputFile(const std::string& file_path) {
    lexer_ = std::make_unique<Lexer>(context_);
    lexer_->setInputFile(file_path);
    advance(); // 初始化current_token_
}

void CHTLParser::setLexer(std::unique_ptr<Lexer> lexer) {
    lexer_ = std::move(lexer);
    advance(); // 初始化current_token_
}

std::unique_ptr<ast::ProgramNode> CHTLParser::parseProgram() {
    CHTL_STATE_GUARD(context_, CompilerState::PARSING_CHTL, SyntaxContext::GLOBAL);
    
    auto program = std::make_unique<ast::ProgramNode>(current_token_.position);
    
    while (current_token_.type != TokenType::EOF_TOKEN) {
        try {
            if (isImportStart()) {
                program->addChild(parseImportStatement());
            } else if (isNamespaceStart()) {
                program->addChild(parseNamespaceDefinition());
            } else if (isTemplateStart()) {
                program->addChild(parseTemplateDefinition());
            } else if (isCustomStart()) {
                program->addChild(parseCustomDefinition());
            } else if (isOriginStart()) {
                program->addChild(parseOriginBlock());
            } else if (isElementStart()) {
                program->addChild(parseElement());
            } else if (current_token_.type == TokenType::SINGLE_COMMENT ||
                      current_token_.type == TokenType::MULTI_COMMENT ||
                      current_token_.type == TokenType::GENERATOR_COMMENT) {
                program->addChild(parseComment());
            } else {
                reportError(ParseError::UNEXPECTED_TOKEN, 
                           "意外的Token: " + current_token_.value);
                advance();
            }
        } catch (const ParseException& e) {
            errors_.push_back(e);
            synchronize();
        }
    }
    
    return program;
}

std::unique_ptr<ast::ElementNode> CHTLParser::parseElement() {
    if (!isElementStart()) {
        reportError(ParseError::INVALID_ELEMENT, "期望HTML元素名");
        return nullptr;
    }
    
    std::string element_name = current_token_.value;
    TokenPosition position = current_token_.position;
    
    validateElementName(element_name);
    advance();
    
    auto element = std::make_unique<ast::ElementNode>(element_name, position);
    
    // 解析元素内容块
    if (current_token_.type == TokenType::LBRACE) {
        advance(); // 消费 '{'
        
        CHTL_CONTEXT_GUARD(context_, "element_" + element_name);
        
        while (current_token_.type != TokenType::RBRACE && current_token_.type != TokenType::EOF_TOKEN) {
            if (isAttributeStart()) {
                element->addChild(parseAttribute());
            } else if (current_token_.type == TokenType::STYLE) {
                element->addChild(parseStyleBlock());
            } else if (current_token_.type == TokenType::SCRIPT) {
                element->addChild(parseScriptBlock());
            } else if (current_token_.type == TokenType::TEXT) {
                element->addChild(parseText());
            } else if (isElementStart()) {
                element->addChild(parseElement());
            } else if (isOriginStart()) {
                element->addChild(parseOriginBlock());
            } else if (current_token_.type == TokenType::EXCEPT) {
                element->addChild(parseExceptConstraint());
            } else if (current_token_.type == TokenType::SINGLE_COMMENT ||
                      current_token_.type == TokenType::MULTI_COMMENT ||
                      current_token_.type == TokenType::GENERATOR_COMMENT) {
                element->addChild(parseComment());
            } else {
                reportError(ParseError::UNEXPECTED_TOKEN, 
                           "元素内容中意外的Token: " + current_token_.value);
                advance();
            }
        }
        
        consume(TokenType::RBRACE, "期望 '}'");
    }
    
    return element;
}

std::unique_ptr<ast::TextNode> CHTLParser::parseText() {
    TokenPosition position = current_token_.position;
    
    consume(TokenType::TEXT, "期望 'text' 关键字");
    consume(TokenType::LBRACE, "期望 '{'");
    
    std::string content;
    if (current_token_.type == TokenType::STRING_LITERAL ||
        current_token_.type == TokenType::UNQUOTED_LITERAL) {
        content = parseStringValue();
    }
    
    consume(TokenType::RBRACE, "期望 '}'");
    
    return std::make_unique<ast::TextNode>(content, position);
}

std::unique_ptr<ast::AttributeNode> CHTLParser::parseAttribute() {
    if (!isAttributeStart()) {
        reportError(ParseError::INVALID_ATTRIBUTE, "期望属性名");
        return nullptr;
    }
    
    std::string attr_name = current_token_.value;
    TokenPosition position = current_token_.position;
    
    validateAttributeName(attr_name);
    advance();
    
    // 处理CE对等式（: 与 = 等价）
    if (current_token_.type == TokenType::COLON || current_token_.type == TokenType::EQUAL) {
        advance();
    } else {
        reportError(ParseError::MISSING_TOKEN, "期望 ':' 或 '='");
    }
    
    std::string attr_value = parseStringValue();
    consume(TokenType::SEMICOLON, "期望 ';'");
    
    auto attribute = std::make_unique<ast::AttributeNode>(attr_name, attr_value, position);
    
    // 检查是否为引号字符串
    if (current_token_.type == TokenType::STRING_LITERAL) {
        attribute->is_quoted = true;
    }
    
    return attribute;
}

std::unique_ptr<ast::StyleBlockNode> CHTLParser::parseStyleBlock() {
    TokenPosition position = current_token_.position;
    
    consume(TokenType::STYLE, "期望 'style' 关键字");
    consume(TokenType::LBRACE, "期望 '{'");
    
    // 判断是否为局部样式块（在元素内部）
    bool is_local = context_.getStateManager().getCurrentContext() != SyntaxContext::GLOBAL;
    
    auto style_block = std::make_unique<ast::StyleBlockNode>(is_local, position);
    
    CHTL_STATE_GUARD(context_, CompilerState::PARSING_STYLE_BLOCK, 
                     is_local ? SyntaxContext::LOCAL_STYLE : SyntaxContext::GLOBAL);
    
    // 解析样式块内容
    auto content = parseStyleBlockContent();
    for (auto& node : content) {
        style_block->addChild(std::move(node));
    }
    
    consume(TokenType::RBRACE, "期望 '}'");
    
    return style_block;
}

std::unique_ptr<ast::ScriptBlockNode> CHTLParser::parseScriptBlock() {
    TokenPosition position = current_token_.position;
    
    consume(TokenType::SCRIPT, "期望 'script' 关键字");
    consume(TokenType::LBRACE, "期望 '{'");
    
    // 判断是否为局部脚本块（在元素内部）
    bool is_local = context_.getStateManager().getCurrentContext() != SyntaxContext::GLOBAL;
    
    auto script_block = std::make_unique<ast::ScriptBlockNode>(is_local, position);
    
    CHTL_STATE_GUARD(context_, CompilerState::PARSING_SCRIPT_BLOCK, 
                     is_local ? SyntaxContext::LOCAL_SCRIPT : SyntaxContext::GLOBAL);
    
    // 解析脚本块内容
    auto content = parseScriptBlockContent();
    for (auto& node : content) {
        script_block->addChild(std::move(node));
        
        // 检查是否包含CHTL JS语法
        if (ast::isCHTLJSNode(node->type)) {
            script_block->contains_chtl_js = true;
        }
    }
    
    consume(TokenType::RBRACE, "期望 '}'");
    
    return script_block;
}

std::unique_ptr<ast::TemplateDefinitionNode> CHTLParser::parseTemplateDefinition() {
    TokenPosition position = current_token_.position;
    
    consume(TokenType::TEMPLATE, "期望 '[Template]' 关键字");
    
    // 解析模板类型
    std::string template_type;
    if (current_token_.type == TokenType::AT_STYLE ||
        current_token_.type == TokenType::AT_ELEMENT ||
        current_token_.type == TokenType::AT_VAR) {
        template_type = current_token_.value;
        advance();
    } else {
        reportError(ParseError::INVALID_TEMPLATE, "期望模板类型 (@Style, @Element, @Var)");
    }
    
    // 解析模板名称
    std::string template_name = parseIdentifierValue();
    
    auto template_def = std::make_unique<ast::TemplateDefinitionNode>(
        template_type, template_name, position);
    
    consume(TokenType::LBRACE, "期望 '{'");
    
    CHTL_STATE_GUARD(context_, CompilerState::PARSING_TEMPLATE, SyntaxContext::TEMPLATE_DEFINITION);
    context_.enterTemplate(template_name);
    
    // 解析模板内容
    auto content = parseTemplateContent();
    for (auto& node : content) {
        template_def->addChild(std::move(node));
    }
    
    context_.exitTemplate();
    consume(TokenType::RBRACE, "期望 '}'");
    
    // 将模板注册到全局映射中
    SymbolType symbol_type;
    if (template_type == "@Style") symbol_type = SymbolType::TEMPLATE_STYLE;
    else if (template_type == "@Element") symbol_type = SymbolType::TEMPLATE_ELEMENT;
    else symbol_type = SymbolType::TEMPLATE_VAR;
    
    context_.getGlobalMap().registerTemplate(template_name, symbol_type, "");
    
    return template_def;
}

std::unique_ptr<ast::CustomDefinitionNode> CHTLParser::parseCustomDefinition() {
    TokenPosition position = current_token_.position;
    
    consume(TokenType::CUSTOM, "期望 '[Custom]' 关键字");
    
    // 解析自定义类型
    std::string custom_type;
    if (current_token_.type == TokenType::AT_STYLE ||
        current_token_.type == TokenType::AT_ELEMENT ||
        current_token_.type == TokenType::AT_VAR) {
        custom_type = current_token_.value;
        advance();
    } else {
        reportError(ParseError::INVALID_CUSTOM, "期望自定义类型 (@Style, @Element, @Var)");
    }
    
    // 解析自定义名称
    std::string custom_name = parseIdentifierValue();
    
    auto custom_def = std::make_unique<ast::CustomDefinitionNode>(
        custom_type, custom_name, position);
    
    consume(TokenType::LBRACE, "期望 '{'");
    
    CHTL_STATE_GUARD(context_, CompilerState::PARSING_CUSTOM, SyntaxContext::CUSTOM_DEFINITION);
    context_.enterCustom(custom_name);
    
    // 解析自定义内容
    auto content = parseCustomContent();
    for (auto& node : content) {
        custom_def->addChild(std::move(node));
    }
    
    context_.exitCustom();
    consume(TokenType::RBRACE, "期望 '}'");
    
    // 将自定义注册到全局映射中
    SymbolType symbol_type;
    if (custom_type == "@Style") symbol_type = SymbolType::CUSTOM_STYLE;
    else if (custom_type == "@Element") symbol_type = SymbolType::CUSTOM_ELEMENT;
    else symbol_type = SymbolType::CUSTOM_VAR;
    
    context_.getGlobalMap().registerCustom(custom_name, symbol_type, "");
    
    return custom_def;
}

std::unique_ptr<ast::OriginBlockNode> CHTLParser::parseOriginBlock() {
    TokenPosition position = current_token_.position;
    
    consume(TokenType::ORIGIN, "期望 '[Origin]' 关键字");
    
    // 解析原始类型
    std::string origin_type;
    if (current_token_.type == TokenType::AT_HTML ||
        current_token_.type == TokenType::AT_STYLE ||
        current_token_.type == TokenType::AT_JAVASCRIPT ||
        current_token_.value.find("@") == 0) { // 自定义类型
        origin_type = current_token_.value;
        advance();
    } else {
        reportError(ParseError::INVALID_SYNTAX, "期望原始嵌入类型");
    }
    
    auto origin_block = std::make_unique<ast::OriginBlockNode>(origin_type, position);
    
    // 解析可选的块名称
    if (current_token_.type == TokenType::IDENTIFIER) {
        origin_block->block_name = current_token_.value;
        advance();
    }
    
    consume(TokenType::LBRACE, "期望 '{'");
    
    // 读取原始内容（直到遇到匹配的 '}'）
    std::ostringstream content;
    int brace_count = 1;
    
    while (current_token_.type != TokenType::EOF_TOKEN && brace_count > 0) {
        if (current_token_.type == TokenType::LBRACE) {
            brace_count++;
        } else if (current_token_.type == TokenType::RBRACE) {
            brace_count--;
        }
        
        if (brace_count > 0) {
            content << current_token_.value;
            if (current_token_.type == TokenType::NEWLINE) {
                content << "\n";
            } else {
                content << " ";
            }
        }
        
        advance();
    }
    
    origin_block->raw_content = content.str();
    
    return origin_block;
}

std::unique_ptr<ast::ImportStatementNode> CHTLParser::parseImportStatement() {
    TokenPosition position = current_token_.position;
    
    consume(TokenType::IMPORT, "期望 '[Import]' 关键字");
    
    // 解析导入类型
    std::string import_type;
    if (current_token_.type == TokenType::AT_HTML ||
        current_token_.type == TokenType::AT_STYLE ||
        current_token_.type == TokenType::AT_JAVASCRIPT ||
        current_token_.type == TokenType::AT_CHTL ||
        current_token_.type == TokenType::AT_CJMOD) {
        import_type = current_token_.value;
        advance();
    } else if (current_token_.type == TokenType::TEMPLATE || current_token_.type == TokenType::CUSTOM) {
        // 处理 [Template] @Style 或 [Custom] @Element 这样的组合
        std::string prefix = current_token_.value;
        advance();
        
        if (current_token_.type == TokenType::AT_STYLE ||
            current_token_.type == TokenType::AT_ELEMENT ||
            current_token_.type == TokenType::AT_VAR) {
            import_type = prefix + " " + current_token_.value;
            advance();
        }
    } else {
        reportError(ParseError::INVALID_IMPORT, "期望导入类型");
    }
    
    auto import_stmt = std::make_unique<ast::ImportStatementNode>(import_type, position);
    
    // 解析导入目标（可选）
    if (current_token_.type == TokenType::IDENTIFIER) {
        import_stmt->import_target = current_token_.value;
        advance();
    }
    
    consume(TokenType::FROM, "期望 'from' 关键字");
    
    // 解析源路径
    import_stmt->source_path = parseStringValue();
    
    // 解析可选的别名
    if (current_token_.type == TokenType::AS) {
        advance();
        import_stmt->alias = parseIdentifierValue();
    }
    
    // 添加到全局映射的导入列表
    GlobalMap::ImportInfo import_info;
    import_info.source_path = import_stmt->source_path;
    import_info.alias = import_stmt->alias;
    import_info.import_all = import_stmt->import_target.empty();
    
    if (!import_stmt->import_target.empty()) {
        import_info.imported_symbols.push_back(import_stmt->import_target);
    }
    
    context_.getGlobalMap().addImport(import_info);
    
    return import_stmt;
}

std::unique_ptr<ast::NamespaceDefinitionNode> CHTLParser::parseNamespaceDefinition() {
    TokenPosition position = current_token_.position;
    
    consume(TokenType::NAMESPACE, "期望 '[Namespace]' 关键字");
    
    std::string namespace_name = parseIdentifierValue();
    
    auto namespace_def = std::make_unique<ast::NamespaceDefinitionNode>(namespace_name, position);
    
    context_.enterNamespace(namespace_name);
    
    // 如果有花括号，解析命名空间内容
    if (current_token_.type == TokenType::LBRACE) {
        advance();
        
        while (current_token_.type != TokenType::RBRACE && current_token_.type != TokenType::EOF_TOKEN) {
            if (isNamespaceStart()) {
                namespace_def->addChild(parseNamespaceDefinition()); // 嵌套命名空间
            } else if (isTemplateStart()) {
                namespace_def->addChild(parseTemplateDefinition());
            } else if (isCustomStart()) {
                namespace_def->addChild(parseCustomDefinition());
            } else if (current_token_.type == TokenType::EXCEPT) {
                namespace_def->addChild(parseExceptConstraint());
            } else {
                reportError(ParseError::UNEXPECTED_TOKEN, 
                           "命名空间中意外的Token: " + current_token_.value);
                advance();
            }
        }
        
        consume(TokenType::RBRACE, "期望 '}'");
    }
    
    context_.exitNamespace();
    
    return namespace_def;
}

std::unique_ptr<ast::CommentNode> CHTLParser::parseComment() {
    ast::NodeType comment_type;
    std::string content = current_token_.value;
    TokenPosition position = current_token_.position;
    
    switch (current_token_.type) {
        case TokenType::SINGLE_COMMENT:
            comment_type = ast::NodeType::SINGLE_COMMENT;
            break;
        case TokenType::MULTI_COMMENT:
            comment_type = ast::NodeType::MULTI_COMMENT;
            break;
        case TokenType::GENERATOR_COMMENT:
            comment_type = ast::NodeType::GENERATOR_COMMENT;
            break;
        default:
            reportError(ParseError::INVALID_SYNTAX, "无效的注释类型");
            return nullptr;
    }
    
    advance();
    
    return std::make_unique<ast::CommentNode>(comment_type, content, position);
}

// Token管理方法
void CHTLParser::advance() {
    if (lexer_) {
        current_token_ = lexer_->nextToken();
        context_.setCurrentPosition(current_token_.position.line, current_token_.position.column);
    }
}

Token CHTLParser::peek() {
    if (lexer_) {
        return lexer_->peekToken();
    }
    return Token(TokenType::INVALID, "");
}

Token CHTLParser::peek() const {
    if (lexer_) {
        return lexer_->peekToken();
    }
    return Token(TokenType::INVALID, "");
}

bool CHTLParser::match(TokenType type) {
    return current_token_.type == type;
}

bool CHTLParser::expect(TokenType type) {
    if (match(type)) {
        advance();
        return true;
    }
    return false;
}

void CHTLParser::consume(TokenType type, const std::string& error_msg) {
    if (current_token_.type == type) {
        advance();
    } else {
        std::string msg = error_msg.empty() ? 
            ("期望 " + std::string(1, static_cast<char>(type))) : error_msg;
        reportError(ParseError::MISSING_TOKEN, msg);
    }
}

// 错误处理
void CHTLParser::reportError(ParseError error_type, const std::string& message) {
    errors_.emplace_back(error_type, message, current_token_.position);
    context_.reportError(message);
}

void CHTLParser::synchronize() {
    advance();
    
    while (current_token_.type != TokenType::EOF_TOKEN) {
        if (current_token_.type == TokenType::SEMICOLON) {
            advance();
            return;
        }
        
        // 同步到下一个语句开始
        if (isElementStart() || isTemplateStart() || isCustomStart() ||
            isImportStart() || isNamespaceStart() || isOriginStart()) {
            return;
        }
        
        advance();
    }
}

// 辅助判断方法
bool CHTLParser::isElementStart() const {
    return current_token_.type == TokenType::IDENTIFIER &&
           current_token_.value != "text" &&
           current_token_.value != "style" &&
           current_token_.value != "script";
}

bool CHTLParser::isAttributeStart() const {
    return current_token_.type == TokenType::IDENTIFIER &&
           (peek().type == TokenType::COLON || peek().type == TokenType::EQUAL);
}

bool CHTLParser::isTemplateStart() const {
    return current_token_.type == TokenType::TEMPLATE;
}

bool CHTLParser::isCustomStart() const {
    return current_token_.type == TokenType::CUSTOM;
}

bool CHTLParser::isOriginStart() const {
    return current_token_.type == TokenType::ORIGIN;
}

bool CHTLParser::isImportStart() const {
    return current_token_.type == TokenType::IMPORT;
}

bool CHTLParser::isNamespaceStart() const {
    return current_token_.type == TokenType::NAMESPACE;
}

bool CHTLParser::isCHTLJSStart() const {
    return current_token_.type == TokenType::DOUBLE_LBRACE;
}

// 解析表达式
std::string CHTLParser::parseStringValue() {
    std::string value;
    
    if (current_token_.type == TokenType::STRING_LITERAL) {
        value = current_token_.value;
        advance();
    } else if (current_token_.type == TokenType::UNQUOTED_LITERAL) {
        value = current_token_.value;
        advance();
    } else if (current_token_.type == TokenType::IDENTIFIER) {
        value = current_token_.value;
        advance();
    } else {
        reportError(ParseError::INVALID_SYNTAX, "期望字符串值");
    }
    
    return value;
}

std::string CHTLParser::parseIdentifierValue() {
    if (current_token_.type == TokenType::IDENTIFIER) {
        std::string value = current_token_.value;
        advance();
        return value;
    } else {
        reportError(ParseError::INVALID_SYNTAX, "期望标识符");
        return "";
    }
}

// 验证方法
void CHTLParser::validateElementName(const std::string& name) {
    // 验证是否为有效的HTML元素名
    if (name.empty() || !std::isalpha(name[0])) {
        reportError(ParseError::INVALID_ELEMENT, "无效的元素名: " + name);
    }
}

void CHTLParser::validateAttributeName(const std::string& name) {
    // 验证是否为有效的HTML属性名
    if (name.empty() || !std::isalpha(name[0])) {
        reportError(ParseError::INVALID_ATTRIBUTE, "无效的属性名: " + name);
    }
}

// 错误处理方法
bool CHTLParser::hasError() const {
    return !errors_.empty();
}

std::vector<ParseException> CHTLParser::getErrors() const {
    return errors_;
}

void CHTLParser::clearErrors() {
    errors_.clear();
}

std::string CHTLParser::getCurrentTokenInfo() const {
    std::ostringstream oss;
    oss << "当前Token: " << current_token_.getTypeString() 
        << " (" << current_token_.value << ") "
        << "@" << current_token_.position.line << ":" << current_token_.position.column;
    return oss.str();
}

} // namespace parser
} // namespace chtl