#include "CHTLParserV3.h"
#include <sstream>
#include <algorithm>

namespace chtl {

CHTLParserV3::CHTLParserV3() : m_Current(0) {
}

std::shared_ptr<ast::v3::DocumentNode> CHTLParserV3::Parse(const std::string& source, const std::string& filename) {
    CHTLLexerV2 lexer;
    lexer.SetInput(source, filename);
    
    std::vector<CHTLToken> tokens;
    while (true) {
        auto token = lexer.NextToken();
        tokens.push_back(token);
        if (token.type == CHTLTokenType::ERROR) {
            m_Errors.push_back("Lexer error: " + token.value);
            return nullptr;
        }
        if (token.type == CHTLTokenType::EOF_TOKEN) {
            break;
        }
    }
    
    return ParseTokens(tokens);
}

std::shared_ptr<ast::v3::DocumentNode> CHTLParserV3::ParseTokens(const std::vector<CHTLToken>& tokens) {
    m_Tokens = tokens;
    m_Current = 0;
    m_Errors.clear();
    m_ContextStack.push(Context::ELEMENT);
    m_CurrentNamespace = "";
    
    auto document = std::make_shared<ast::v3::DocumentNode>();
    
    while (!IsAtEnd()) {
        auto node = ParseTopLevel();
        if (node) {
            document->AddChild(node);
        }
    }
    
    return document;
}

std::shared_ptr<ast::v3::ASTNode> CHTLParserV3::ParseTopLevel() {
    // Skip whitespace and comments at top level
    while (Check(CHTLTokenType::COMMENT_SINGLE) || 
           Check(CHTLTokenType::COMMENT_MULTI) || 
           Check(CHTLTokenType::COMMENT_HTML)) {
        return ParseComment();
    }
    
    // Configuration blocks
    if (Check(CHTLTokenType::CONFIGURATION)) {
        return ParseConfiguration();
    }
    
    // Import statements
    if (Check(CHTLTokenType::IMPORT)) {
        return ParseImport();
    }
    
    // Namespace declarations
    if (Check(CHTLTokenType::NAMESPACE)) {
        return ParseNamespace();
    }
    
    // Template definitions
    if (Check(CHTLTokenType::TEMPLATE)) {
        return ParseTemplate();
    }
    
    // Custom element definitions
    if (Check(CHTLTokenType::CUSTOM)) {
        return ParseCustom();
    }
    
    // Origin blocks
    if (Check(CHTLTokenType::ORIGIN)) {
        return ParseOrigin();
    }
    
    // Except blocks
    if (Check(CHTLTokenType::EXCEPT)) {
        return ParseExcept();
    }
    
    // Delete statements
    if (Check(CHTLTokenType::DELETE)) {
        return ParseDelete();
    }
    
    // Insert statements
    if (Check(CHTLTokenType::INSERT)) {
        return ParseInsert();
    }
    
    // Variable declarations
    if (Match(CHTLTokenType::DOLLAR)) {
        return ParseVar();
    }
    
    // Variable calls (without explicit $)
    if (Check(CHTLTokenType::AT_VAR)) {
        return ParseVarCall();
    }
    
    // Global style blocks
    if (Check(CHTLTokenType::STYLE)) {
        return ParseStyle();
    }
    
    // Global script blocks
    if (Check(CHTLTokenType::SCRIPT)) {
        return ParseScript();
    }
    
    // Element nodes
    if (Check(CHTLTokenType::IDENTIFIER)) {
        return ParseElement();
    }
    
    // Text blocks
    if (Check(CHTLTokenType::TEXT)) {
        return ParseText();
    }
    
    // String literals as text
    if (Check(CHTLTokenType::STRING_DOUBLE) || Check(CHTLTokenType::STRING_SINGLE)) {
        auto token = Advance();
        return std::make_shared<ast::v3::TextNode>(token.value);
    }
    
    // Error handling
    auto token = Peek();
    ReportError("Unexpected token at top level: " + token.value);
    Advance(); // Skip the problematic token
    return nullptr;
}

std::shared_ptr<ast::v3::ElementNode> CHTLParserV3::ParseElement() {
    auto nameToken = Advance();
    auto element = std::make_shared<ast::v3::ElementNode>(nameToken.value);
    
    // Parse attributes
    auto attributes = ParseAttributes();
    for (const auto& [key, value] : attributes) {
        element->AddAttribute(key, value);
    }
    
    // Parse content
    if (Match(CHTLTokenType::LBRACE)) {
        PushContext(Context::ELEMENT);
        ParseElementContent(element.get());
        PopContext();
        Match(CHTLTokenType::RBRACE);
    } else {
        Match(CHTLTokenType::SEMICOLON);
    }
    
    return element;
}

void CHTLParserV3::ParseElementContent(ast::v3::ElementNode* element) {
    while (!IsAtEnd() && !Check(CHTLTokenType::RBRACE)) {
        // Comments
        if (Check(CHTLTokenType::COMMENT_SINGLE) || 
            Check(CHTLTokenType::COMMENT_MULTI) || 
            Check(CHTLTokenType::COMMENT_HTML)) {
            element->AddChild(ParseComment());
            continue;
        }
        
        // Local style blocks
        if (Check(CHTLTokenType::STYLE)) {
            auto styleNode = ParseLocalStyle();
            if (styleNode) {
                element->AddChild(styleNode);
                // Apply auto-generated class/id
                if (!styleNode->GetAutoClassName().empty() && !element->HasAttribute("class")) {
                    element->AddAttribute("class", styleNode->GetAutoClassName());
                }
                if (!styleNode->GetAutoId().empty() && !element->HasAttribute("id")) {
                    element->AddAttribute("id", styleNode->GetAutoId());
                }
            }
            continue;
        }
        
        // Local script blocks
        if (Check(CHTLTokenType::SCRIPT)) {
            auto scriptNode = ParseLocalScript();
            if (scriptNode) {
                element->AddChild(scriptNode);
            }
            continue;
        }
        
        // Text blocks
        if (Check(CHTLTokenType::TEXT)) {
            element->AddChild(ParseText());
            continue;
        }
        
        // String literals as text
        if (Check(CHTLTokenType::STRING_DOUBLE) || Check(CHTLTokenType::STRING_SINGLE)) {
            auto token = Advance();
            element->AddChild(std::make_shared<ast::v3::TextNode>(token.value));
            continue;
        }
        
        // Template instantiation
        if (Check(CHTLTokenType::AT_STYLE) || Check(CHTLTokenType::AT_ELEMENT)) {
            element->AddChild(ParseSpecialization());
            continue;
        }
        
        // Variable calls
        if (Check(CHTLTokenType::AT_VAR)) {
            element->AddChild(ParseVarCall());
            continue;
        }
        
        // Nested elements
        if (Check(CHTLTokenType::IDENTIFIER)) {
            element->AddChild(ParseElement());
            continue;
        }
        
        // Error handling
        ReportError("Unexpected token in element: " + Peek().value);
        Advance();
    }
}

std::unordered_map<std::string, std::string> CHTLParserV3::ParseAttributes() {
    std::unordered_map<std::string, std::string> attributes;
    
    while (!IsAtEnd() && !Check(CHTLTokenType::LBRACE) && Check(CHTLTokenType::IDENTIFIER)) {
        // Check if this might be an element name (no colon/equals after it)
        auto savedPos = m_Current;
        auto name = Advance();
        
        if (MatchColonOrEquals()) {
            // This is an attribute
            std::string value = ParseStringOrUnquoted();
            attributes[name.value] = value;
            Match(CHTLTokenType::SEMICOLON); // Optional semicolon
        } else {
            // This was actually an element name, backtrack
            m_Current = savedPos;
            break;
        }
    }
    
    return attributes;
}

std::shared_ptr<ast::v3::TextNode> CHTLParserV3::ParseText() {
    auto startToken = Advance(); // text
    
    if (!Match(CHTLTokenType::LBRACE)) {
        ReportError("Expected '{' after 'text'");
        return nullptr;
    }
    
    std::string content;
    
    // Collect all content until closing brace
    while (!IsAtEnd() && !Check(CHTLTokenType::RBRACE)) {
        auto token = Advance();
        
        // Convert token back to its original text
        if (token.type == CHTLTokenType::STRING_DOUBLE || token.type == CHTLTokenType::STRING_SINGLE) {
            content += token.value;
        } else if (token.type == CHTLTokenType::LITERAL_UNQUOTED) {
            content += token.value;
        } else {
            content += token.value;
        }
        
        // Add spacing if needed
        if (!IsAtEnd() && !Check(CHTLTokenType::RBRACE)) {
            content += " ";
        }
    }
    
    Match(CHTLTokenType::RBRACE);
    
    return std::make_shared<ast::v3::TextNode>(content);
}

std::shared_ptr<ast::v3::ASTNode> CHTLParserV3::ParseStyle() {
    auto token = Advance(); // style
    
    if (!Match(CHTLTokenType::LBRACE)) {
        ReportError("Expected '{' after 'style'");
        return nullptr;
    }
    
    auto style = std::make_shared<ast::v3::StyleNode>(
        CurrentContext() == Context::ELEMENT ? ast::v3::StyleNode::LOCAL : ast::v3::StyleNode::GLOBAL
    );
    
    // Parse CSS rules
    while (!IsAtEnd() && !Check(CHTLTokenType::RBRACE)) {
        ParseStyleRule(style.get());
    }
    
    Match(CHTLTokenType::RBRACE);
    
    return style;
}

void CHTLParserV3::ParseStyleRule(ast::v3::StyleNode* style) {
    // Parse selector
    std::string selector;
    while (!IsAtEnd() && !Check(CHTLTokenType::LBRACE)) {
        auto token = Advance();
        selector += token.value;
        if (!IsAtEnd() && !Check(CHTLTokenType::LBRACE)) {
            selector += " ";
        }
    }
    
    if (!Match(CHTLTokenType::LBRACE)) {
        ReportError("Expected '{' after style selector");
        return;
    }
    
    std::string properties;
    int braceDepth = 0;
    
    while (!IsAtEnd()) {
        auto token = Peek();
        if (token.type == CHTLTokenType::LBRACE) {
            braceDepth++;
        } else if (token.type == CHTLTokenType::RBRACE) {
            if (braceDepth == 0) {
                break;
            }
            braceDepth--;
        }
        
        properties += Advance().value;
        if (!IsAtEnd() && !(Peek().type == CHTLTokenType::RBRACE && braceDepth == 0)) {
            properties += " ";
        }
    }
    
    Match(CHTLTokenType::RBRACE);
    
    style->AddRule(selector, properties);
}

std::shared_ptr<ast::v3::ASTNode> CHTLParserV3::ParseScript() {
    auto token = Advance(); // script
    
    if (!Match(CHTLTokenType::LBRACE)) {
        ReportError("Expected '{' after 'script'");
        return nullptr;
    }
    
    auto script = std::make_shared<ast::v3::ScriptNode>(
        CurrentContext() == Context::ELEMENT ? ast::v3::ScriptNode::LOCAL : ast::v3::ScriptNode::GLOBAL
    );
    
    // Parse script content
    std::string content;
    int braceDepth = 0;
    size_t startPos = m_Current;
    
    while (!IsAtEnd()) {
        auto token = Peek();
        
        if (token.type == CHTLTokenType::LBRACE) {
            braceDepth++;
        } else if (token.type == CHTLTokenType::RBRACE) {
            if (braceDepth == 0) {
                break;
            }
            braceDepth--;
        }
        
        Advance();
    }
    
    content = ExtractOriginContent(startPos, m_Current);
    Match(CHTLTokenType::RBRACE);
    
    script->SetContent(content);
    return script;
}

std::shared_ptr<ast::v3::CommentNode> CHTLParserV3::ParseComment() {
    auto token = Advance();
    
    ast::v3::CommentNode::CommentType type;
    if (token.type == CHTLTokenType::COMMENT_HTML) {
        type = ast::v3::CommentNode::HTML_COMMENT;
    } else if (token.type == CHTLTokenType::COMMENT_SINGLE) {
        type = ast::v3::CommentNode::SINGLE_LINE;
    } else {
        type = ast::v3::CommentNode::MULTI_LINE;
    }
    
    return std::make_shared<ast::v3::CommentNode>(type, token.value);
}

std::shared_ptr<ast::v3::TemplateNode> CHTLParserV3::ParseTemplate() {
    auto token = Advance(); // [Template]
    
    ast::v3::TemplateNode::TemplateType type = ast::v3::TemplateNode::STYLE;
    std::string name;
    
    // Parse template type
    if (Match(CHTLTokenType::AT_STYLE)) {
        type = ast::v3::TemplateNode::STYLE;
    } else if (Match(CHTLTokenType::AT_ELEMENT)) {
        type = ast::v3::TemplateNode::ELEMENT;
    } else if (Match(CHTLTokenType::AT_VAR)) {
        type = ast::v3::TemplateNode::VAR;
    } else {
        ReportError("Expected @Style, @Element, or @Var after [Template]");
        return nullptr;
    }
    
    // Parse template name
    if (Check(CHTLTokenType::IDENTIFIER)) {
        name = Advance().value;
    } else {
        ReportError("Expected template name");
        return nullptr;
    }
    
    auto tmpl = std::make_shared<ast::v3::TemplateNode>(type, name);
    
    // Parse parameters
    if (Match(CHTLTokenType::LPAREN)) {
        ParseTemplateParameters(tmpl.get());
        Match(CHTLTokenType::RPAREN);
    }
    
    // Parse template body
    if (!Match(CHTLTokenType::LBRACE)) {
        ReportError("Expected '{' after template declaration");
        return nullptr;
    }
    
    PushContext(Context::TEMPLATE);
    
    // Parse template content based on type
    if (type == ast::v3::TemplateNode::STYLE) {
        // Parse style properties
        while (!IsAtEnd() && !Check(CHTLTokenType::RBRACE)) {
            if (Check(CHTLTokenType::IDENTIFIER)) {
                auto prop = Advance();
                if (MatchColonOrEquals()) {
                    std::string value = ParseStringOrUnquoted();
                    tmpl->AddStyleProperty(prop.value, value);
                    Match(CHTLTokenType::SEMICOLON);
                }
            } else {
                ReportError("Unexpected token in style template");
                Advance();
            }
        }
    } else if (type == ast::v3::TemplateNode::ELEMENT) {
        // Parse element content
        while (!IsAtEnd() && !Check(CHTLTokenType::RBRACE)) {
            auto node = ParseTopLevel();
            if (node) {
                tmpl->AddChild(node);
            }
        }
    } else if (type == ast::v3::TemplateNode::VAR) {
        // Parse variable content
        if (Check(CHTLTokenType::STRING_DOUBLE) || Check(CHTLTokenType::STRING_SINGLE)) {
            tmpl->SetVarContent(Advance().value);
        }
    }
    
    PopContext();
    Match(CHTLTokenType::RBRACE);
    
    return tmpl;
}

void CHTLParserV3::ParseTemplateParameters(ast::v3::TemplateNode* tmpl) {
    while (!IsAtEnd() && !Check(CHTLTokenType::RPAREN)) {
        if (Check(CHTLTokenType::IDENTIFIER)) {
            std::string paramName = Advance().value;
            std::string defaultValue;
            
            if (Match(CHTLTokenType::EQUALS)) {
                defaultValue = ParseStringOrUnquoted();
            }
            
            tmpl->AddParameter(paramName, defaultValue);
            
            if (!Check(CHTLTokenType::RPAREN)) {
                Match(CHTLTokenType::COMMA);
            }
        } else {
            ReportError("Expected parameter name");
            break;
        }
    }
}

std::shared_ptr<ast::v3::CustomNode> CHTLParserV3::ParseCustom() {
    auto token = Advance(); // [Custom]
    
    ast::v3::CustomNode::CustomType type = ast::v3::CustomNode::STYLE;
    std::string name;
    
    // Parse custom type
    if (Match(CHTLTokenType::AT_STYLE)) {
        type = ast::v3::CustomNode::STYLE;
    } else if (Match(CHTLTokenType::AT_ELEMENT)) {
        type = ast::v3::CustomNode::ELEMENT;
    } else if (Match(CHTLTokenType::AT_VAR)) {
        type = ast::v3::CustomNode::VAR;
    } else {
        ReportError("Expected @Style, @Element, or @Var after [Custom]");
        return nullptr;
    }
    
    // Parse custom name
    if (Check(CHTLTokenType::IDENTIFIER)) {
        name = Advance().value;
    } else {
        ReportError("Expected custom name");
        return nullptr;
    }
    
    auto custom = std::make_shared<ast::v3::CustomNode>(type, name);
    
    // Parse custom body
    if (!Match(CHTLTokenType::LBRACE)) {
        ReportError("Expected '{' after custom declaration");
        return nullptr;
    }
    
    PushContext(Context::CUSTOM);
    ParseCustomDefinition(custom.get());
    PopContext();
    
    Match(CHTLTokenType::RBRACE);
    
    return custom;
}

void CHTLParserV3::ParseCustomDefinition(ast::v3::CustomNode* custom) {
    if (custom->GetType() == ast::v3::CustomNode::STYLE) {
        ParseCustomStyleDefinition(custom);
    } else if (custom->GetType() == ast::v3::CustomNode::ELEMENT) {
        ParseCustomElementDefinition(custom);
    } else if (custom->GetType() == ast::v3::CustomNode::VAR) {
        ParseCustomVarDefinition(custom);
    }
}

void CHTLParserV3::ParseCustomStyleDefinition(ast::v3::CustomNode* custom) {
    auto styleNode = std::make_shared<ast::v3::StyleNode>(ast::v3::StyleNode::CUSTOM);
    
    while (!IsAtEnd() && !Check(CHTLTokenType::RBRACE)) {
        // Delete inheritance
        if (Check(CHTLTokenType::DELETE)) {
            custom->AddChild(ParseDelete());
            continue;
        }
        
        // Insert directives
        if (Check(CHTLTokenType::INSERT)) {
            custom->AddChild(ParseInsert());
            continue;
        }
        
        // Inherit from other styles
        if (Check(CHTLTokenType::AT_STYLE)) {
            Advance();
            std::string templateName = ParseFullQualifiedName();
            
            if (Match(CHTLTokenType::LBRACE)) {
                // Specialization
                auto subCustom = std::make_shared<ast::v3::CustomNode>(ast::v3::CustomNode::STYLE, templateName);
                subCustom->SetSpecialization(true);
                
                PushContext(Context::SPECIALIZATION);
                ParseCustomDefinition(subCustom.get());
                PopContext();
                
                Match(CHTLTokenType::RBRACE);
                custom->AddChild(subCustom);
            } else {
                // Simple inheritance
                auto inherit = std::make_shared<ast::v3::InheritNode>(templateName);
                custom->AddChild(inherit);
                Match(CHTLTokenType::SEMICOLON);
            }
            continue;
        }
        
        // Style properties
        if (Check(CHTLTokenType::IDENTIFIER)) {
            auto prop = Advance();
            if (MatchColonOrEquals()) {
                std::string value = ParseStringOrUnquoted();
                styleNode->AddInlineProperty(prop.value, value);
                Match(CHTLTokenType::SEMICOLON);
            } else if (Match(CHTLTokenType::SEMICOLON) || Check(CHTLTokenType::COMMA)) {
                // No-value property
                styleNode->AddNoValueProperty(prop.value);
                custom->SetHasNoValueProperties(true);
                
                // Parse comma-separated no-value properties
                while (Match(CHTLTokenType::COMMA)) {
                    if (Check(CHTLTokenType::IDENTIFIER)) {
                        auto nextProp = Advance();
                        styleNode->AddNoValueProperty(nextProp.value);
                    }
                }
                Match(CHTLTokenType::SEMICOLON);
            }
        } else {
            ReportError("Unexpected token in custom style");
            Advance();
        }
    }
    
    if (!styleNode->GetInlineProperties().empty() || !styleNode->GetNoValueProperties().empty()) {
        custom->AddChild(styleNode);
    }
}

void CHTLParserV3::ParseCustomElementDefinition(ast::v3::CustomNode* custom) {
    while (!IsAtEnd() && !Check(CHTLTokenType::RBRACE)) {
        // Delete directives
        if (Check(CHTLTokenType::DELETE)) {
            custom->AddChild(ParseDelete());
            continue;
        }
        
        // Insert directives
        if (Check(CHTLTokenType::INSERT)) {
            custom->AddChild(ParseInsert());
            continue;
        }
        
        // Inherit from other elements
        if (Check(CHTLTokenType::AT_ELEMENT)) {
            Advance();
            std::string templateName = ParseFullQualifiedName();
            
            if (Match(CHTLTokenType::LBRACE)) {
                // Specialization
                auto subCustom = std::make_shared<ast::v3::CustomNode>(ast::v3::CustomNode::ELEMENT, templateName);
                subCustom->SetSpecialization(true);
                
                PushContext(Context::SPECIALIZATION);
                ParseCustomDefinition(subCustom.get());
                PopContext();
                
                Match(CHTLTokenType::RBRACE);
                custom->AddChild(subCustom);
            } else {
                // Simple inheritance
                auto inherit = std::make_shared<ast::v3::InheritNode>(templateName);
                custom->AddChild(inherit);
                Match(CHTLTokenType::SEMICOLON);
            }
            continue;
        }
        
        // Element content
        auto node = ParseTopLevel();
        if (node) {
            custom->AddChild(node);
        }
    }
}

void CHTLParserV3::ParseCustomVarDefinition(ast::v3::CustomNode* custom) {
    while (!IsAtEnd() && !Check(CHTLTokenType::RBRACE)) {
        // Variable content
        if (Check(CHTLTokenType::STRING_DOUBLE) || Check(CHTLTokenType::STRING_SINGLE)) {
            auto token = Advance();
            custom->AddChild(std::make_shared<ast::v3::TextNode>(token.value));
        } else if (Check(CHTLTokenType::AT_VAR)) {
            // Inherit from other variables
            Advance();
            std::string varName = ParseFullQualifiedName();
            auto inherit = std::make_shared<ast::v3::InheritNode>(varName);
            custom->AddChild(inherit);
            Match(CHTLTokenType::SEMICOLON);
        } else {
            ReportError("Unexpected token in custom var");
            Advance();
        }
    }
}

std::shared_ptr<ast::v3::VarNode> CHTLParserV3::ParseVar() {
    // $ already consumed
    
    if (!Check(CHTLTokenType::IDENTIFIER)) {
        ReportError("Expected variable name after '$'");
        return nullptr;
    }
    
    std::string name = Advance().value;
    std::string value;
    
    if (MatchColonOrEquals()) {
        value = ParseStringOrUnquoted();
    }
    
    Match(CHTLTokenType::SEMICOLON);
    
    return std::make_shared<ast::v3::VarNode>(name, value);
}

std::shared_ptr<ast::v3::VarCallNode> CHTLParserV3::ParseVarCall() {
    auto token = Advance(); // @Var
    
    if (!Check(CHTLTokenType::IDENTIFIER)) {
        ReportError("Expected variable name after '@Var'");
        return nullptr;
    }
    
    std::string name = Advance().value;
    Match(CHTLTokenType::SEMICOLON);
    
    return std::make_shared<ast::v3::VarCallNode>(name);
}

std::shared_ptr<ast::v3::ImportNode> CHTLParserV3::ParseImport() {
    auto token = Advance(); // [Import]
    
    ast::v3::ImportNode::ImportType type = ast::v3::ImportNode::NORMAL;
    std::string path;
    std::string alias;
    std::vector<std::string> targets;
    
    // Parse import type
    if (Match(CHTLTokenType::AT_STYLE)) {
        type = ast::v3::ImportNode::STYLE;
    } else if (Match(CHTLTokenType::AT_ELEMENT)) {
        type = ast::v3::ImportNode::ELEMENT;
    } else if (Match(CHTLTokenType::AT_VAR)) {
        type = ast::v3::ImportNode::VAR;
    } else if (Match(CHTLTokenType::AT_HTML)) {
        type = ast::v3::ImportNode::HTML;
    } else if (Match(CHTLTokenType::AT_JAVASCRIPT)) {
        type = ast::v3::ImportNode::JAVASCRIPT;
    } else if (Match(CHTLTokenType::AT_CHTL)) {
        type = ast::v3::ImportNode::CHTL;
    } else if (Match(CHTLTokenType::AT_CJMOD)) {
        type = ast::v3::ImportNode::CJMOD;
    }
    
    // Parse specific targets
    if (type != ast::v3::ImportNode::NORMAL && Check(CHTLTokenType::IDENTIFIER)) {
        while (Check(CHTLTokenType::IDENTIFIER)) {
            targets.push_back(Advance().value);
            if (!Match(CHTLTokenType::COMMA)) {
                break;
            }
        }
    }
    
    // Parse 'from' keyword
    if (!Match(CHTLTokenType::FROM)) {
        ReportError("Expected 'from' in import statement");
        return nullptr;
    }
    
    // Parse import path
    path = ParseImportPath();
    
    // Parse optional 'as' alias
    if (Match(CHTLTokenType::AS)) {
        if (Check(CHTLTokenType::IDENTIFIER)) {
            alias = Advance().value;
        } else {
            ReportError("Expected alias after 'as'");
        }
    }
    
    Match(CHTLTokenType::SEMICOLON);
    
    auto import = std::make_shared<ast::v3::ImportNode>(type, path);
    if (!alias.empty()) {
        import->SetAlias(alias);
    }
    for (const auto& target : targets) {
        import->AddTarget(target);
    }
    
    return import;
}

std::shared_ptr<ast::v3::NamespaceNode> CHTLParserV3::ParseNamespace() {
    auto token = Advance(); // [Namespace]
    
    if (!Check(CHTLTokenType::IDENTIFIER)) {
        ReportError("Expected namespace name");
        return nullptr;
    }
    
    std::string name = ParseFullQualifiedName();
    auto ns = std::make_shared<ast::v3::NamespaceNode>(name);
    
    if (!Match(CHTLTokenType::LBRACE)) {
        ReportError("Expected '{' after namespace declaration");
        return nullptr;
    }
    
    // Save current namespace and update
    std::string prevNamespace = m_CurrentNamespace;
    m_CurrentNamespace = name;
    m_NamespaceStack.push(name);
    
    // Parse namespace content
    while (!IsAtEnd() && !Check(CHTLTokenType::RBRACE)) {
        auto node = ParseTopLevel();
        if (node) {
            ns->AddChild(node);
        }
    }
    
    Match(CHTLTokenType::RBRACE);
    
    // Restore namespace
    m_CurrentNamespace = prevNamespace;
    if (!m_NamespaceStack.empty()) {
        m_NamespaceStack.pop();
    }
    
    return ns;
}

std::shared_ptr<ast::v3::ConfigurationNode> CHTLParserV3::ParseConfiguration() {
    auto token = Advance(); // [Configuration]
    
    std::string name;
    
    // Check for @Config name
    if (Match(CHTLTokenType::AT_CONFIG)) {
        if (Check(CHTLTokenType::IDENTIFIER)) {
            name = Advance().value;
        }
    }
    
    auto config = std::make_shared<ast::v3::ConfigurationNode>(name);
    
    if (!Match(CHTLTokenType::LBRACE)) {
        ReportError("Expected '{' after configuration declaration");
        return nullptr;
    }
    
    // Parse configuration items
    while (!IsAtEnd() && !Check(CHTLTokenType::RBRACE)) {
        if (Check(CHTLTokenType::IDENTIFIER)) {
            auto key = Advance().value;
            
            if (key == "name") {
                ParseNameConfig(config.get());
            } else if (key == "origin") {
                ParseOriginTypeConfig(config.get());
            } else {
                // Generic key-value pair
                if (MatchColonOrEquals()) {
                    std::string value = ParseStringOrUnquoted();
                    config->AddConfig(key, value);
                    Match(CHTLTokenType::SEMICOLON);
                }
            }
        } else {
            ReportError("Expected configuration key");
            Advance();
        }
    }
    
    Match(CHTLTokenType::RBRACE);
    
    return config;
}

void CHTLParserV3::ParseNameConfig(ast::v3::ConfigurationNode* config) {
    if (!MatchColonOrEquals()) {
        ReportError("Expected ':' or '=' after 'name'");
        return;
    }
    
    if (!Match(CHTLTokenType::LBRACE)) {
        // Single name configuration
        std::string value = ParseStringOrUnquoted();
        config->AddConfig("name", value);
        Match(CHTLTokenType::SEMICOLON);
        return;
    }
    
    // Multiple name configurations
    while (!IsAtEnd() && !Check(CHTLTokenType::RBRACE)) {
        if (Check(CHTLTokenType::IDENTIFIER)) {
            std::string type = Advance().value; // e.g., "template", "custom"
            
            if (MatchColonOrEquals()) {
                if (Match(CHTLTokenType::LBRACE)) {
                    // Nested configuration
                    while (!IsAtEnd() && !Check(CHTLTokenType::RBRACE)) {
                        if (Check(CHTLTokenType::IDENTIFIER)) {
                            std::string subKey = Advance().value;
                            if (MatchColonOrEquals()) {
                                std::string value = ParseStringOrUnquoted();
                                config->AddConfig("name." + type + "." + subKey, value);
                                Match(CHTLTokenType::SEMICOLON);
                            }
                        } else {
                            Advance();
                        }
                    }
                    Match(CHTLTokenType::RBRACE);
                } else {
                    // Simple value
                    std::string value = ParseStringOrUnquoted();
                    config->AddConfig("name." + type, value);
                }
                Match(CHTLTokenType::SEMICOLON);
            }
        } else {
            Advance();
        }
    }
    
    Match(CHTLTokenType::RBRACE);
    Match(CHTLTokenType::SEMICOLON);
}

void CHTLParserV3::ParseOriginTypeConfig(ast::v3::ConfigurationNode* config) {
    if (!MatchColonOrEquals()) {
        ReportError("Expected ':' or '=' after 'origin'");
        return;
    }
    
    if (!Match(CHTLTokenType::LBRACE)) {
        // Single origin configuration
        std::string value = ParseStringOrUnquoted();
        config->AddConfig("origin", value);
        Match(CHTLTokenType::SEMICOLON);
        return;
    }
    
    // Multiple origin configurations
    while (!IsAtEnd() && !Check(CHTLTokenType::RBRACE)) {
        if (Check(CHTLTokenType::IDENTIFIER)) {
            std::string key = Advance().value;
            if (MatchColonOrEquals()) {
                std::string value = ParseStringOrUnquoted();
                config->AddConfig("origin." + key, value);
                Match(CHTLTokenType::SEMICOLON);
            }
        } else {
            Advance();
        }
    }
    
    Match(CHTLTokenType::RBRACE);
    Match(CHTLTokenType::SEMICOLON);
}

std::shared_ptr<ast::v3::OriginNode> CHTLParserV3::ParseOrigin() {
    auto token = Advance(); // [Origin]
    
    ast::v3::OriginNode::OriginType type = ast::v3::OriginNode::HTML;
    std::string customType;
    std::string name;
    
    // Parse origin type
    if (Match(CHTLTokenType::AT_HTML)) {
        type = ast::v3::OriginNode::HTML;
    } else if (Match(CHTLTokenType::AT_STYLE)) {
        type = ast::v3::OriginNode::STYLE;
    } else if (Match(CHTLTokenType::AT_JAVASCRIPT)) {
        type = ast::v3::OriginNode::JAVASCRIPT;
    } else if (Check(CHTLTokenType::IDENTIFIER) && Peek().value.compare(0, 1, "@") == 0) {
        // Custom type
        type = ast::v3::OriginNode::CUSTOM;
        customType = Advance().value;
    }
    
    // Parse optional name
    if (Check(CHTLTokenType::IDENTIFIER)) {
        name = Advance().value;
    }
    
    auto origin = std::make_shared<ast::v3::OriginNode>(type, name);
    if (!customType.empty()) {
        origin->SetCustomType(customType);
    }
    
    if (!Match(CHTLTokenType::LBRACE)) {
        ReportError("Expected '{' after origin declaration");
        return nullptr;
    }
    
    // Parse raw content
    ParseOriginContent(origin.get());
    
    Match(CHTLTokenType::RBRACE);
    
    return origin;
}

void CHTLParserV3::ParseOriginContent(ast::v3::OriginNode* origin) {
    // Origin blocks contain raw content that should not be parsed
    size_t startPos = m_Current;
    int braceDepth = 0;
    
    while (!IsAtEnd()) {
        auto token = Peek();
        
        if (token.type == CHTLTokenType::LBRACE) {
            braceDepth++;
        } else if (token.type == CHTLTokenType::RBRACE) {
            if (braceDepth == 0) {
                break;
            }
            braceDepth--;
        }
        
        Advance();
    }
    
    std::string content = ExtractOriginContent(startPos, m_Current);
    origin->SetContent(content);
}

std::shared_ptr<ast::v3::ExceptNode> CHTLParserV3::ParseExcept() {
    auto token = Advance(); // except
    
    ast::v3::ExceptNode::ExceptType type = ast::v3::ExceptNode::PRECISE;
    std::vector<std::string> targets;
    
    // Parse except type
    if (Check(CHTLTokenType::IDENTIFIER)) {
        auto typeToken = Peek();
        if (typeToken.value == "type") {
            Advance();
            type = ast::v3::ExceptNode::TYPE;
        } else if (typeToken.value == "global") {
            Advance();
            type = ast::v3::ExceptNode::GLOBAL;
        } else {
            // Precise targeting
            targets = ParseExceptTargets();
        }
    }
    
    auto except = std::make_shared<ast::v3::ExceptNode>(type);
    for (const auto& target : targets) {
        except->AddTarget(target);
    }
    
    if (!Match(CHTLTokenType::LBRACE)) {
        ReportError("Expected '{' after except declaration");
        return nullptr;
    }
    
    // Parse except content
    while (!IsAtEnd() && !Check(CHTLTokenType::RBRACE)) {
        auto node = ParseTopLevel();
        if (node) {
            except->AddChild(node);
        }
    }
    
    Match(CHTLTokenType::RBRACE);
    
    return except;
}

std::vector<std::string> CHTLParserV3::ParseExceptTargets() {
    std::vector<std::string> targets;
    
    while (Check(CHTLTokenType::IDENTIFIER)) {
        std::string target = Advance().value;
        
        // Check for array index
        if (Match(CHTLTokenType::LBRACKET)) {
            if (Check(CHTLTokenType::NUMBER)) {
                target += "[" + Advance().value + "]";
                Match(CHTLTokenType::RBRACKET);
            }
        }
        
        targets.push_back(target);
        
        if (!Match(CHTLTokenType::COMMA)) {
            break;
        }
    }
    
    return targets;
}

std::shared_ptr<ast::v3::DeleteNode> CHTLParserV3::ParseDelete() {
    auto token = Advance(); // delete
    
    ast::v3::DeleteNode::DeleteType type = ast::v3::DeleteNode::PRECISE;
    std::vector<std::string> targets;
    
    // Parse delete targets
    do {
        if (Match(CHTLTokenType::AT_STYLE)) {
            type = ast::v3::DeleteNode::INHERITANCE;
            if (Check(CHTLTokenType::IDENTIFIER)) {
                targets.push_back(Advance().value);
            }
        } else if (Match(CHTLTokenType::AT_ELEMENT)) {
            type = ast::v3::DeleteNode::INHERITANCE;
            if (Check(CHTLTokenType::IDENTIFIER)) {
                targets.push_back(Advance().value);
            }
        } else if (Check(CHTLTokenType::IDENTIFIER)) {
            std::string target = Advance().value;
            
            // Check for array index
            if (Match(CHTLTokenType::LBRACKET)) {
                if (Check(CHTLTokenType::NUMBER)) {
                    type = ast::v3::DeleteNode::ELEMENT;
                    target += "[" + Advance().value + "]";
                    Match(CHTLTokenType::RBRACKET);
                }
            }
            
            targets.push_back(target);
        }
    } while (Match(CHTLTokenType::COMMA));
    
    Match(CHTLTokenType::SEMICOLON);
    
    auto deleteNode = std::make_shared<ast::v3::DeleteNode>(type, targets.empty() ? "" : targets[0]);
    for (size_t i = 1; i < targets.size(); ++i) {
        deleteNode->AddAdditionalTarget(targets[i]);
    }
    
    return deleteNode;
}

std::shared_ptr<ast::v3::InsertNode> CHTLParserV3::ParseInsert() {
    auto token = Advance(); // insert
    
    ast::v3::InsertNode::Position position = ast::v3::InsertNode::BEFORE;
    std::string target;
    
    // Parse position
    if (Match(CHTLTokenType::BEFORE)) {
        position = ast::v3::InsertNode::BEFORE;
    } else if (Match(CHTLTokenType::AFTER)) {
        position = ast::v3::InsertNode::AFTER;
    } else if (Match(CHTLTokenType::REPLACE)) {
        position = ast::v3::InsertNode::REPLACE;
    } else if (Match(CHTLTokenType::AT)) {
        if (Match(CHTLTokenType::IDENTIFIER)) {
            std::string pos = Previous().value;
            if (pos == "top") {
                position = ast::v3::InsertNode::AT_TOP;
            } else if (pos == "bottom") {
                position = ast::v3::InsertNode::AT_BOTTOM;
            }
        }
    }
    
    // Parse target element
    if (position == ast::v3::InsertNode::BEFORE ||
        position == ast::v3::InsertNode::AFTER ||
        position == ast::v3::InsertNode::REPLACE) {
        if (Check(CHTLTokenType::IDENTIFIER)) {
            target = Advance().value;
            
            // Check for array index
            if (Match(CHTLTokenType::LBRACKET)) {
                if (Check(CHTLTokenType::NUMBER)) {
                    target += "[" + Advance().value + "]";
                    Match(CHTLTokenType::RBRACKET);
                }
            }
        }
    }
    
    auto insertNode = std::make_shared<ast::v3::InsertNode>(position, target);
    
    // Parse content to insert
    if (Match(CHTLTokenType::LBRACE)) {
        while (!IsAtEnd() && !Check(CHTLTokenType::RBRACE)) {
            auto node = ParseTopLevel();
            if (node) {
                insertNode->AddChild(node);
            }
        }
        Match(CHTLTokenType::RBRACE);
    }
    
    return insertNode;
}

std::shared_ptr<ast::v3::ASTNode> CHTLParserV3::ParseSpecialization() {
    // Parse @Style or @Element specialization
    if (Match(CHTLTokenType::AT_STYLE)) {
        std::string templateName = ParseFullQualifiedName();
        
        if (Match(CHTLTokenType::LBRACE)) {
            // Style specialization with modifications
            auto custom = std::make_shared<ast::v3::CustomNode>(ast::v3::CustomNode::STYLE, templateName);
            custom->SetSpecialization(true);
            
            PushContext(Context::SPECIALIZATION);
            ParseCustomStyleDefinition(custom.get());
            PopContext();
            
            Match(CHTLTokenType::RBRACE);
            return custom;
        } else {
            // Simple style instantiation
            auto inherit = std::make_shared<ast::v3::InheritNode>(templateName);
            Match(CHTLTokenType::SEMICOLON);
            return inherit;
        }
    } else if (Match(CHTLTokenType::AT_ELEMENT)) {
        std::string templateName = ParseFullQualifiedName();
        
        if (Match(CHTLTokenType::LBRACE)) {
            // Element specialization with modifications
            auto custom = std::make_shared<ast::v3::CustomNode>(ast::v3::CustomNode::ELEMENT, templateName);
            custom->SetSpecialization(true);
            
            PushContext(Context::SPECIALIZATION);
            ParseCustomElementDefinition(custom.get());
            PopContext();
            
            Match(CHTLTokenType::RBRACE);
            return custom;
        } else {
            // Simple element instantiation
            auto inherit = std::make_shared<ast::v3::InheritNode>(templateName);
            Match(CHTLTokenType::SEMICOLON);
            return inherit;
        }
    }
    
    ReportError("Expected @Style or @Element in specialization");
    return nullptr;
}

std::shared_ptr<ast::v3::StyleNode> CHTLParserV3::ParseLocalStyle() {
    auto styleNode = std::dynamic_pointer_cast<ast::v3::StyleNode>(ParseStyle());
    if (styleNode) {
        // Process auto-generated class names and IDs
        for (const auto& rule : styleNode->GetRules()) {
            const std::string& selector = rule.first;
            
            // Check for class selector
            if (selector.length() > 1 && selector[0] == '.') {
                std::string className = selector.substr(1);
                // Remove pseudo-classes and pseudo-elements
                size_t colonPos = className.find(':');
                if (colonPos != std::string::npos) {
                    className = className.substr(0, colonPos);
                }
                
                // Record auto-generated class name
                styleNode->SetAutoClassName(className);
            }
            // Check for ID selector
            else if (selector.length() > 1 && selector[0] == '#') {
                std::string id = selector.substr(1);
                // Remove pseudo-classes and pseudo-elements
                size_t colonPos = id.find(':');
                if (colonPos != std::string::npos) {
                    id = id.substr(0, colonPos);
                }
                
                // Record auto-generated ID
                styleNode->SetAutoId(id);
            }
        }
    }
    
    return styleNode;
}

std::shared_ptr<ast::v3::ScriptNode> CHTLParserV3::ParseLocalScript() {
    return std::dynamic_pointer_cast<ast::v3::ScriptNode>(ParseScript());
}

// Helper methods

std::string CHTLParserV3::ParseFullQualifiedName() {
    std::string name;
    
    if (Check(CHTLTokenType::IDENTIFIER)) {
        name = Advance().value;
        
        // Parse namespace qualification
        while (Match(CHTLTokenType::DOT)) {
            if (Check(CHTLTokenType::IDENTIFIER)) {
                name += "." + Advance().value;
            }
        }
    }
    
    return name;
}

std::string CHTLParserV3::ParseImportPath() {
    std::string path;
    
    if (Check(CHTLTokenType::STRING_DOUBLE) || Check(CHTLTokenType::STRING_SINGLE)) {
        path = Advance().value;
    } else if (Check(CHTLTokenType::IDENTIFIER)) {
        // Parse identifier-based path
        path = ParseIdentifierPath();
    }
    
    return path;
}

std::string CHTLParserV3::ParseIdentifierPath() {
    std::string path;
    
    if (Check(CHTLTokenType::IDENTIFIER)) {
        path = Advance().value;
        
        // Support . and / as path separators
        while (Match(CHTLTokenType::DOT) || 
               (Check(CHTLTokenType::LITERAL_UNQUOTED) && Peek().value == "/")) {
            if (Previous().type == CHTLTokenType::LITERAL_UNQUOTED) {
                path += "/";
            } else {
                path += ".";
            }
            
            if (Check(CHTLTokenType::IDENTIFIER)) {
                path += Advance().value;
            } else if (Check(CHTLTokenType::STAR)) {
                path += Advance().value;
            }
        }
    }
    
    return path;
}

std::string CHTLParserV3::ParseStringOrUnquoted() {
    if (Check(CHTLTokenType::STRING_DOUBLE) || Check(CHTLTokenType::STRING_SINGLE)) {
        return Advance().value;
    } else if (Check(CHTLTokenType::LITERAL_UNQUOTED) ||
               Check(CHTLTokenType::IDENTIFIER) ||
               Check(CHTLTokenType::NUMBER)) {
        return Advance().value;
    }
    
    ReportError("Expected string or literal value");
    return "";
}

std::string CHTLParserV3::ExtractOriginContent(size_t startPos, size_t endPos) {
    std::string content;
    
    for (size_t i = startPos; i < endPos && i < m_Tokens.size(); ++i) {
        content += m_Tokens[i].value;
        if (i < endPos - 1) {
            content += " ";
        }
    }
    
    return content;
}

bool CHTLParserV3::Match(CHTLTokenType type) {
    if (Check(type)) {
        Advance();
        return true;
    }
    return false;
}

bool CHTLParserV3::Check(CHTLTokenType type) const {
    if (IsAtEnd()) return false;
    return Peek().type == type;
}

bool CHTLParserV3::CheckAny(const std::vector<CHTLTokenType>& types) const {
    for (auto type : types) {
        if (Check(type)) return true;
    }
    return false;
}

const CHTLToken& CHTLParserV3::Advance() {
    if (!IsAtEnd()) m_Current++;
    return Previous();
}

const CHTLToken& CHTLParserV3::Peek(size_t offset) const {
    return m_Tokens[std::min(m_Current + offset, m_Tokens.size() - 1)];
}

const CHTLToken& CHTLParserV3::Previous() const {
    return m_Tokens[m_Current - 1];
}

bool CHTLParserV3::IsAtEnd() const {
    return m_Current >= m_Tokens.size() || Peek().type == CHTLTokenType::EOF_TOKEN;
}

bool CHTLParserV3::MatchColonOrEquals() {
    return Match(CHTLTokenType::COLON) || Match(CHTLTokenType::EQUALS);
}

CHTLParserV3::Context CHTLParserV3::CurrentContext() const {
    return m_ContextStack.empty() ? Context::ELEMENT : m_ContextStack.top();
}

void CHTLParserV3::PushContext(Context ctx) {
    m_ContextStack.push(ctx);
}

void CHTLParserV3::PopContext() {
    if (!m_ContextStack.empty()) {
        m_ContextStack.pop();
    }
}

void CHTLParserV3::AddError(const std::string& message) {
    std::stringstream ss;
    if (m_Current < m_Tokens.size()) {
        const auto& token = m_Tokens[m_Current];
        ss << "Error at line " << token.line << ", column " << token.column << ": " << message;
    } else {
        ss << "Error at end of file: " << message;
    }
    m_Errors.push_back(ss.str());
}

const std::vector<std::string>& CHTLParserV3::GetErrors() const {
    return m_Errors;
}

} // namespace chtl