#include "../../include/CHTLParser.h"
#include "../../include/CHTLLexer.h"
#include <iostream>
#include <sstream>
#include <algorithm>

namespace chtl {

CHTLParser::CHTLParser(const CompilerOptions& options) 
    : currentPos_(0), options_(options), debugMode_(options.debugMode),
      stateMachine_(std::make_unique<StateMachine>()),
      contextManager_(std::make_unique<ContextManager>()),
      functionCounter_(0) {
    
    // 初始化解析上下文
    currentContext_.nestingLevel = 0;
    
    if (debugMode_) {
        debugLog("CHTL解析器初始化完成");
    }
}

ParseResult CHTLParser::parse(const String& source, const String& filename) {
    currentFilename_ = filename;
    result_ = ParseResult{};
    
    if (debugMode_) {
        debugLog("开始解析文件: " + filename);
    }
    
    // 使用词法分析器生成Token
    CHTLLexer lexer(options_);
    auto lexResult = lexer.tokenize(source, filename);
    
    if (!lexResult.success) {
        result_.success = false;
        for (const auto& error : lexResult.errors) {
            result_.addError(ParseErrorType::SYNTAX_ERROR, error.message, error.line, error.column, filename);
        }
        return result_;
    }
    
    return parseFromTokens(lexResult.tokens, filename);
}

ParseResult CHTLParser::parseFromTokens(const std::vector<Token>& tokens, const String& filename) {
    tokens_ = tokens;
    currentPos_ = 0;
    currentFilename_ = filename;
    result_ = ParseResult{};
    
    if (tokens_.empty()) {
        result_.addError(ParseErrorType::SYNTAX_ERROR, "空的Token流", 0, 0, filename);
        return result_;
    }
    
    try {
        // 创建根节点
        auto root = std::make_shared<RootNode>(0, 0);
        
        // 解析顶层结构
        while (!isAtEnd()) {
            skipWhitespace();
            skipComments();
            
            if (isAtEnd()) break;
            
            auto node = parseTopLevelStatement();
            if (node) {
                root->addChild(node);
            }
        }
        
        result_.ast = root;
        result_.success = true;
        
        if (debugMode_) {
            debugLog("解析完成，生成AST节点: " + std::to_string(root->getChildren().size()) + " 个顶层节点");
        }
        
    } catch (const std::exception& e) {
        result_.addError(ParseErrorType::SYNTAX_ERROR, "解析异常: " + String(e.what()), 
                        currentToken().line, currentToken().column, filename);
    }
    
    return result_;
}

std::shared_ptr<CHTLASTNode> CHTLParser::parseTopLevelStatement() {
    const auto& token = currentToken();
    
    switch (token.type) {
        case TokenType::TEMPLATE_DECL:
            return parseTemplate();
        case TokenType::CUSTOM_DECL:
            return parseCustom();
        case TokenType::IMPORT_DECL:
            return parseImport();
        case TokenType::NAMESPACE_DECL:
            return parseNamespace();
        case TokenType::CONFIGURATION_DECL:
            return parseConfiguration();
        case TokenType::ORIGIN_DECL:
            return parseOrigin();
        case TokenType::IDENTIFIER:
        case TokenType::HTML_ELEMENT:
            return parseElement();
        case TokenType::TEXT:
            return parseText();
        case TokenType::COMMENT_GENERATOR:
            return parseGeneratorComment();
        default:
            reportError(ParseErrorType::UNEXPECTED_TOKEN, 
                       "意外的Token: " + token.value, 
                       "顶层语句");
            skipToken();
            return nullptr;
    }
}

// 添加缺失方法的实现
std::shared_ptr<CHTLASTNode> CHTLParser::parseCustom() {
    const auto& customToken = consumeToken(TokenType::CUSTOM_DECL);
    
    if (!matchAnyToken({TokenType::AT_STYLE, TokenType::AT_ELEMENT, TokenType::AT_VAR})) {
        reportError(ParseErrorType::TEMPLATE_ERROR, "自定义声明后必须跟类型");
        return nullptr;
    }
    
    const auto& typeToken = consumeToken();
    const auto& nameToken = consumeToken(TokenType::IDENTIFIER);
    
    String customType = typeToken.value;
    String customName = nameToken.value;
    
    if (customType == "@Style") {
        return parseCustomStyleImpl(customName, nameToken.line, nameToken.column);
    } else if (customType == "@Element") {
        return parseCustomElementImpl(customName, nameToken.line, nameToken.column);
    } else if (customType == "@Var") {
        return parseCustomVarImpl(customName, nameToken.line, nameToken.column);
    } else {
        reportError(ParseErrorType::TEMPLATE_ERROR, "未知的自定义类型: " + customType);
        return nullptr;
    }
}

std::shared_ptr<CustomStyleNode> CHTLParser::parseCustomStyleImpl(const String& customName, size_t line, size_t column) {
    auto customStyle = std::make_shared<CustomStyleNode>(customName, line, column);
    
    enterCustomContext(customName);
    
    if (matchToken(TokenType::LEFT_BRACE)) {
        consumeToken(TokenType::LEFT_BRACE);
        
        while (!matchToken(TokenType::RIGHT_BRACE) && !isAtEnd()) {
            skipWhitespace();
            skipComments();
            
            if (matchToken(TokenType::RIGHT_BRACE)) break;
            
            if (matchToken(TokenType::INHERIT)) {
                // 显性继承: inherit @Style TemplateName;
                auto inherit = parseInherit();
                if (inherit) {
                    auto inheritNode = std::static_pointer_cast<InheritNode>(inherit);
                    customStyle->addInheritance(inheritNode->getTarget());
                }
            } else if (matchToken(TokenType::AT_STYLE)) {
                // 组合式继承或特例化: @Style TemplateName { ... }
                consumeToken(TokenType::AT_STYLE);
                const auto& inheritNameToken = consumeToken(TokenType::IDENTIFIER);
                String inheritName = inheritNameToken.value;
                
                if (matchToken(TokenType::LEFT_BRACE)) {
                    // 特例化操作
                    auto specialization = parseStyleSpecialization(inheritName);
                    if (specialization) {
                        customStyle->addChild(specialization);
                    }
                } else {
                    // 简单继承
                    customStyle->addInheritance(inheritName);
                    if (matchToken(TokenType::SEMICOLON)) {
                        consumeToken(TokenType::SEMICOLON);
                    }
                }
            } else if (matchToken(TokenType::DELETE)) {
                // 删除操作: delete property1, property2;
                auto deleteNode = parseDelete();
                if (deleteNode) {
                    auto delNode = std::static_pointer_cast<DeleteNode>(deleteNode);
                    customStyle->addDeletedProperty(delNode->getTarget());
                }
            } else if (matchToken(TokenType::IDENTIFIER)) {
                // 检查是否为无值样式组属性
                const auto& propToken = currentToken();
                if (peekToken().type == TokenType::COMMA || peekToken().type == TokenType::SEMICOLON) {
                    // 无值属性: color, font-size;
                    String propName = propToken.value;
                    consumeToken(TokenType::IDENTIFIER);
                    
                    // 创建无值属性节点
                    auto property = std::make_shared<CSSPropertyNode>(propName, "");
                    customStyle->addChild(property);
                    
                    if (matchToken(TokenType::COMMA)) {
                        consumeToken(TokenType::COMMA);
                        continue; // 继续解析下一个无值属性
                    } else if (matchToken(TokenType::SEMICOLON)) {
                        consumeToken(TokenType::SEMICOLON);
                    }
                } else {
                    // 普通样式属性
                    auto property = parseStyleProperty();
                    if (property) {
                        customStyle->addChild(property);
                    }
                }
            } else {
                auto property = parseStyleProperty();
                if (property) {
                    customStyle->addChild(property);
                }
            }
        }
        
        if (matchToken(TokenType::RIGHT_BRACE)) {
            consumeToken(TokenType::RIGHT_BRACE);
        } else {
            reportError(ParseErrorType::MISSING_TOKEN, "自定义样式缺少右花括号 '}'");
        }
    }
    
    exitCustomContext();
    return customStyle;
}

std::shared_ptr<CustomElementNode> CHTLParser::parseCustomElementImpl(const String& customName, size_t line, size_t column) {
    auto customElement = std::make_shared<CustomElementNode>(customName, line, column);
    
    enterCustomContext(customName);
    
    if (matchToken(TokenType::LEFT_BRACE)) {
        consumeToken(TokenType::LEFT_BRACE);
        
        while (!matchToken(TokenType::RIGHT_BRACE) && !isAtEnd()) {
            skipWhitespace();
            skipComments();
            
            if (matchToken(TokenType::RIGHT_BRACE)) break;
            
            if (matchToken(TokenType::INHERIT)) {
                // 显性继承: inherit @Element TemplateName;
                auto inherit = parseInherit();
                if (inherit) {
                    auto inheritNode = std::static_pointer_cast<InheritNode>(inherit);
                    customElement->addInheritance(inheritNode->getTarget());
                }
            } else if (matchToken(TokenType::AT_ELEMENT)) {
                // 组合式继承: @Element TemplateName;
                consumeToken(TokenType::AT_ELEMENT);
                const auto& inheritNameToken = consumeToken(TokenType::IDENTIFIER);
                customElement->addInheritance(inheritNameToken.value);
                
                if (matchToken(TokenType::SEMICOLON)) {
                    consumeToken(TokenType::SEMICOLON);
                }
            } else if (matchToken(TokenType::INSERT)) {
                // 插入操作: insert after div[0] { ... }
                auto insertNode = parseInsert();
                if (insertNode) {
                    customElement->addChild(insertNode);
                }
            } else if (matchToken(TokenType::DELETE)) {
                // 删除操作: delete span; delete div[1];
                auto deleteNode = parseDelete();
                if (deleteNode) {
                    auto delNode = std::static_pointer_cast<DeleteNode>(deleteNode);
                    customElement->addDeletedElement(delNode->getTarget());
                }
            } else {
                // 普通元素定义
                auto element = parseElement();
                if (element) {
                    customElement->addChild(element);
                }
            }
        }
        
        if (matchToken(TokenType::RIGHT_BRACE)) {
            consumeToken(TokenType::RIGHT_BRACE);
        } else {
            reportError(ParseErrorType::MISSING_TOKEN, "自定义元素缺少右花括号 '}'");
        }
    }
    
    exitCustomContext();
    return customElement;
}

std::shared_ptr<CustomVarNode> CHTLParser::parseCustomVarImpl(const String& customName, size_t line, size_t column) {
    auto customVar = std::make_shared<CustomVarNode>(customName, line, column);
    
    enterCustomContext(customName);
    
    if (matchToken(TokenType::LEFT_BRACE)) {
        consumeToken(TokenType::LEFT_BRACE);
        
        while (!matchToken(TokenType::RIGHT_BRACE) && !isAtEnd()) {
            skipWhitespace();
            skipComments();
            
            if (matchToken(TokenType::RIGHT_BRACE)) break;
            
            // 解析变量定义 variableName: "value";
            const auto& varToken = consumeToken(TokenType::IDENTIFIER);
            String varName = varToken.value;
            
            // CE对等式：支持 ':' 或 '='
            if (matchToken(TokenType::COLON) || matchToken(TokenType::EQUALS)) {
                consumeToken(); // 消费 : 或 =
                
                String varValue = parseLiteral();
                customVar->addVariable(varName, varValue);
                
                // 可选的分号
                if (matchToken(TokenType::SEMICOLON)) {
                    consumeToken(TokenType::SEMICOLON);
                }
            } else {
                reportError(ParseErrorType::SYNTAX_ERROR, "变量定义缺少 ':' 或 '='");
            }
        }
        
        if (matchToken(TokenType::RIGHT_BRACE)) {
            consumeToken(TokenType::RIGHT_BRACE);
        } else {
            reportError(ParseErrorType::MISSING_TOKEN, "自定义变量缺少右花括号 '}'");
        }
    }
    
    exitCustomContext();
    return customVar;
}

std::shared_ptr<CHTLASTNode> CHTLParser::parseImport() {
    const auto& importToken = consumeToken(TokenType::IMPORT_DECL);
    
    // 解析导入类型和路径
    ImportNode::ImportType importType;
    String itemName;
    String path;
    String alias;
    
    // 解析导入类型
    if (matchToken(TokenType::AT_HTML)) {
        importType = ImportNode::ImportType::HTML;
        consumeToken(TokenType::AT_HTML);
    } else if (matchToken(TokenType::AT_STYLE)) {
        importType = ImportNode::ImportType::STYLE;
        consumeToken(TokenType::AT_STYLE);
    } else if (matchToken(TokenType::AT_JAVASCRIPT)) {
        importType = ImportNode::ImportType::JAVASCRIPT;
        consumeToken(TokenType::AT_JAVASCRIPT);
    } else if (matchToken(TokenType::AT_CHTL)) {
        importType = ImportNode::ImportType::CHTL;
        consumeToken(TokenType::AT_CHTL);
    } else if (matchToken(TokenType::AT_CJMOD)) {
        importType = ImportNode::ImportType::CJMOD;
        consumeToken(TokenType::AT_CJMOD);
    } else if (matchToken(TokenType::CUSTOM_DECL)) {
        // 解析 [Custom] @Type
        consumeToken(TokenType::CUSTOM_DECL);
        if (matchToken(TokenType::AT_STYLE)) {
            importType = ImportNode::ImportType::CUSTOM_STYLE;
            consumeToken(TokenType::AT_STYLE);
        } else if (matchToken(TokenType::AT_ELEMENT)) {
            importType = ImportNode::ImportType::CUSTOM_ELEMENT;
            consumeToken(TokenType::AT_ELEMENT);
        } else if (matchToken(TokenType::AT_VAR)) {
            importType = ImportNode::ImportType::CUSTOM_VAR;
            consumeToken(TokenType::AT_VAR);
        } else {
            addError(ParseErrorType::SYNTAX_ERROR, "Expected @Style, @Element, or @Var after [Custom]");
            return nullptr;
        }
        
        // 解析具体项目名称
        if (matchToken(TokenType::IDENTIFIER)) {
            itemName = getCurrentToken().value;
            consumeToken(TokenType::IDENTIFIER);
        }
    } else if (matchToken(TokenType::TEMPLATE_DECL)) {
        // 解析 [Template] @Type
        consumeToken(TokenType::TEMPLATE_DECL);
        if (matchToken(TokenType::AT_STYLE)) {
            importType = ImportNode::ImportType::TEMPLATE_STYLE;
            consumeToken(TokenType::AT_STYLE);
        } else if (matchToken(TokenType::AT_ELEMENT)) {
            importType = ImportNode::ImportType::TEMPLATE_ELEMENT;
            consumeToken(TokenType::AT_ELEMENT);
        } else if (matchToken(TokenType::AT_VAR)) {
            importType = ImportNode::ImportType::TEMPLATE_VAR;
            consumeToken(TokenType::AT_VAR);
        } else {
            addError(ParseErrorType::SYNTAX_ERROR, "Expected @Style, @Element, or @Var after [Template]");
            return nullptr;
        }
        
        // 解析具体项目名称
        if (matchToken(TokenType::IDENTIFIER)) {
            itemName = getCurrentToken().value;
            consumeToken(TokenType::IDENTIFIER);
        }
    } else {
        addError(ParseErrorType::SYNTAX_ERROR, "Expected import type after [Import]");
        return nullptr;
    }
    
    // 解析 from 关键字
    if (!matchToken(TokenType::FROM)) {
        addError(ParseErrorType::MISSING_TOKEN, "Expected 'from' keyword in import statement");
        return nullptr;
    }
    consumeToken(TokenType::FROM);
    
    // 解析路径
    if (matchToken(TokenType::STRING_LITERAL) || matchToken(TokenType::IDENTIFIER) || matchToken(TokenType::UNQUOTED_LITERAL)) {
        path = getCurrentToken().value;
        skipToken();
    } else {
        addError(ParseErrorType::SYNTAX_ERROR, "Expected import path after 'from'");
        return nullptr;
    }
    
    // 解析可选的 as 别名
    if (matchToken(TokenType::AS)) {
        consumeToken(TokenType::AS);
        if (matchToken(TokenType::IDENTIFIER)) {
            alias = getCurrentToken().value;
            consumeToken(TokenType::IDENTIFIER);
        } else {
            addError(ParseErrorType::SYNTAX_ERROR, "Expected identifier after 'as'");
            return nullptr;
        }
    }
    
    // 消费分号
    if (matchToken(TokenType::SEMICOLON)) {
        consumeToken(TokenType::SEMICOLON);
    }
    
    // 创建导入节点
    auto importNode = std::make_shared<ImportNode>(importType, path, importToken.line, importToken.column);
    importNode->setAlias(alias);
    importNode->setItemName(itemName);
    
    return importNode;
    
    // 这行代码已经被前面的return语句替代，不应该执行到这里
    // return std::make_shared<ImportNode>("", importToken.line, importToken.column);
}

std::shared_ptr<CHTLASTNode> CHTLParser::parseNamespace() {
    const auto& namespaceToken = consumeToken(TokenType::NAMESPACE_DECL);
    
    const auto& nameToken = consumeToken(TokenType::IDENTIFIER);
    String namespaceName = nameToken.value;
    
    auto namespaceNode = std::make_shared<NamespaceNode>(namespaceName, namespaceToken.line, namespaceToken.column);
    
    if (matchToken(TokenType::LEFT_BRACE)) {
        consumeToken(TokenType::LEFT_BRACE);
        
        while (!matchToken(TokenType::RIGHT_BRACE) && !isAtEnd()) {
            skipWhitespace();
            skipComments();
            
            if (matchToken(TokenType::RIGHT_BRACE)) break;
            
            auto child = parseTopLevelStatement();
            if (child) {
                namespaceNode->addChild(child);
            }
        }
        
        if (matchToken(TokenType::RIGHT_BRACE)) {
            consumeToken(TokenType::RIGHT_BRACE);
        }
    }
    
    return namespaceNode;
}

std::shared_ptr<CHTLASTNode> CHTLParser::parseConfiguration() {
    const auto& configToken = consumeToken(TokenType::CONFIGURATION_DECL);
    
    // 简化实现 - 跳过配置块
    if (matchToken(TokenType::LEFT_BRACE)) {
        consumeToken(TokenType::LEFT_BRACE);
        skipToMatchingBrace();
    }
    
    return std::make_shared<ConfigurationNode>(configToken.line, configToken.column);
}

std::shared_ptr<CHTLASTNode> CHTLParser::parseOrigin() {
    const auto& originToken = consumeToken(TokenType::ORIGIN_DECL);
    
    // 简化实现 - 跳过原始嵌入块
    if (matchToken(TokenType::LEFT_BRACE)) {
        consumeToken(TokenType::LEFT_BRACE);
        skipToMatchingBrace();
    }
    
    return std::make_shared<OriginNode>("", originToken.line, originToken.column);
}

std::shared_ptr<CHTLASTNode> CHTLParser::parseGeneratorComment() {
    const auto& commentToken = consumeToken(TokenType::COMMENT_GENERATOR);
    return std::make_shared<CommentNode>(commentToken.value, commentToken.line, commentToken.column);
}

std::shared_ptr<CHTLASTNode> CHTLParser::parseTemplateUsage() {
    const auto& typeToken = consumeToken(); // @Style, @Element, @Var
    const auto& nameToken = consumeToken(TokenType::IDENTIFIER);
    
    auto usage = std::make_shared<TemplateUsageNode>(typeToken.value, nameToken.value, 
                                                    typeToken.line, typeToken.column);
    
    // 处理变量组特例化: ThemeColor(tableColor = rgb(145, 155, 200))
    if (typeToken.value == "@Var" && matchToken(TokenType::LEFT_PAREN)) {
        consumeToken(TokenType::LEFT_PAREN);
        
        while (!matchToken(TokenType::RIGHT_PAREN) && !isAtEnd()) {
            const auto& varNameToken = consumeToken(TokenType::IDENTIFIER);
            String varName = varNameToken.value;
            
            if (matchToken(TokenType::EQUALS)) {
                consumeToken(TokenType::EQUALS);
                String varValue = parseLiteral();
                usage->addSpecialization(varName, varValue);
            }
            
            if (matchToken(TokenType::COMMA)) {
                consumeToken(TokenType::COMMA);
            }
        }
        
        if (matchToken(TokenType::RIGHT_PAREN)) {
            consumeToken(TokenType::RIGHT_PAREN);
        } else {
            reportError(ParseErrorType::MISSING_TOKEN, "变量组特例化缺少右括号 ')'");
        }
    }
    
    // 处理自定义特例化块 (用于自定义系统)
    if (matchToken(TokenType::LEFT_BRACE)) {
        consumeToken(TokenType::LEFT_BRACE);
        
        while (!matchToken(TokenType::RIGHT_BRACE) && !isAtEnd()) {
            skipWhitespace();
            skipComments();
            
            if (matchToken(TokenType::RIGHT_BRACE)) break;
            
            // 解析特例化内容
            auto content = parseElementContent();
            if (content) {
                usage->addChild(content);
            }
        }
        
        if (matchToken(TokenType::RIGHT_BRACE)) {
            consumeToken(TokenType::RIGHT_BRACE);
        } else {
            reportError(ParseErrorType::MISSING_TOKEN, "模板使用特例化块缺少右花括号 '}'");
        }
    }
    
    // 可选的分号
    if (matchToken(TokenType::SEMICOLON)) {
        consumeToken(TokenType::SEMICOLON);
    }
    
    return usage;
}

std::shared_ptr<CHTLASTNode> CHTLParser::parseVariableReference() {
    const auto& nameToken = consumeToken(TokenType::IDENTIFIER);
    auto varRef = std::make_shared<VariableReferenceNode>(nameToken.value, nameToken.line, nameToken.column);
    
    // 检查是否为变量组引用: GroupName(variableName)
    if (matchToken(TokenType::LEFT_PAREN)) {
        consumeToken(TokenType::LEFT_PAREN);
        
        const auto& varNameToken = consumeToken(TokenType::IDENTIFIER);
        String variableName = varNameToken.value;
        
        if (matchToken(TokenType::RIGHT_PAREN)) {
            consumeToken(TokenType::RIGHT_PAREN);
            varRef->setGroupReference(nameToken.value, variableName);
        } else {
            reportError(ParseErrorType::MISSING_TOKEN, "变量组引用缺少右括号 ')'");
        }
    }
    
    return varRef;
}

std::shared_ptr<CHTLASTNode> CHTLParser::parseSelector() {
    const auto& selectorToken = consumeToken(TokenType::IDENTIFIER);
    String selectorName = selectorToken.value;
    
    auto selector = std::make_shared<SelectorNode>(selectorName, selectorToken.line, selectorToken.column);
    
    if (matchToken(TokenType::LEFT_BRACE)) {
        consumeToken(TokenType::LEFT_BRACE);
        
        while (!matchToken(TokenType::RIGHT_BRACE) && !isAtEnd()) {
            skipWhitespace();
            skipComments();
            
            if (matchToken(TokenType::RIGHT_BRACE)) break;
            
            auto property = parseStyleProperty();
            if (property) {
                selector->addChild(property);
            }
        }
        
        if (matchToken(TokenType::RIGHT_BRACE)) {
            consumeToken(TokenType::RIGHT_BRACE);
        }
    }
    
    return selector;
}

std::shared_ptr<CHTLASTNode> CHTLParser::parseStyleSpecialization(const String& templateName) {
    auto specialization = std::make_shared<SpecializationNode>("@Style", templateName);
    
    consumeToken(TokenType::LEFT_BRACE);
    
    while (!matchToken(TokenType::RIGHT_BRACE) && !isAtEnd()) {
        skipWhitespace();
        skipComments();
        
        if (matchToken(TokenType::RIGHT_BRACE)) break;
        
        if (matchToken(TokenType::DELETE)) {
            // 删除属性或继承: delete property1, property2; 或 delete @Style TemplateName;
            consumeToken(TokenType::DELETE);
            
            if (matchToken(TokenType::AT_STYLE)) {
                // 删除继承: delete @Style TemplateName;
                consumeToken(TokenType::AT_STYLE);
                const auto& nameToken = consumeToken(TokenType::IDENTIFIER);
                specialization->addDeletedInheritance(nameToken.value);
            } else {
                // 删除属性: delete property1, property2;
                do {
                    const auto& propToken = consumeToken(TokenType::IDENTIFIER);
                    specialization->addDeletedProperty(propToken.value);
                    
                    if (matchToken(TokenType::COMMA)) {
                        consumeToken(TokenType::COMMA);
                    } else {
                        break;
                    }
                } while (!isAtEnd());
            }
            
            if (matchToken(TokenType::SEMICOLON)) {
                consumeToken(TokenType::SEMICOLON);
            }
        } else {
            // 普通样式属性
            auto property = parseStyleProperty();
            if (property) {
                specialization->addChild(property);
            }
        }
    }
    
    if (matchToken(TokenType::RIGHT_BRACE)) {
        consumeToken(TokenType::RIGHT_BRACE);
    } else {
        reportError(ParseErrorType::MISSING_TOKEN, "特例化块缺少右花括号 '}'");
    }
    
    return specialization;
}

std::shared_ptr<CHTLASTNode> CHTLParser::parseInsert() {
    consumeToken(TokenType::INSERT);
    
    // 解析位置: after, before, replace, at top, at bottom
    InsertNode::InsertPosition position;
    String selector;
    
    if (matchToken(TokenType::AFTER)) {
        position = InsertNode::InsertPosition::AFTER;
        consumeToken(TokenType::AFTER);
        
        // 解析选择器: div[0]
        selector = parseSelector()->toString(); // 简化处理
    } else if (matchToken(TokenType::BEFORE)) {
        position = InsertNode::InsertPosition::BEFORE;
        consumeToken(TokenType::BEFORE);
        selector = parseSelector()->toString();
    } else if (matchToken(TokenType::REPLACE)) {
        position = InsertNode::InsertPosition::REPLACE;
        consumeToken(TokenType::REPLACE);
        selector = parseSelector()->toString();
    } else if (matchToken(TokenType::AT_TOP)) {
        position = InsertNode::InsertPosition::AT_TOP;
        consumeToken(TokenType::AT_TOP);
    } else if (matchToken(TokenType::AT_BOTTOM)) {
        position = InsertNode::InsertPosition::AT_BOTTOM;
        consumeToken(TokenType::AT_BOTTOM);
    } else {
        reportError(ParseErrorType::SYNTAX_ERROR, "插入操作缺少位置关键字");
        return nullptr;
    }
    
    auto insertNode = std::make_shared<InsertNode>(position, selector);
    
    if (matchToken(TokenType::LEFT_BRACE)) {
        consumeToken(TokenType::LEFT_BRACE);
        
        while (!matchToken(TokenType::RIGHT_BRACE) && !isAtEnd()) {
            skipWhitespace();
            skipComments();
            
            if (matchToken(TokenType::RIGHT_BRACE)) break;
            
            auto element = parseElement();
            if (element) {
                insertNode->addChild(element);
            }
        }
        
        if (matchToken(TokenType::RIGHT_BRACE)) {
            consumeToken(TokenType::RIGHT_BRACE);
        } else {
            reportError(ParseErrorType::MISSING_TOKEN, "插入操作缺少右花括号 '}'");
        }
    }
    
    return insertNode;
}

std::shared_ptr<CHTLASTNode> CHTLParser::parseDelete() {
    consumeToken(TokenType::DELETE);
    
    // 解析要删除的项目
    String deleteTarget;
    while (!matchToken(TokenType::SEMICOLON) && !isAtEnd()) {
        deleteTarget += currentToken().value + " ";
        skipToken();
    }
    
    if (matchToken(TokenType::SEMICOLON)) {
        skipToken();
    }
    
    return std::make_shared<DeleteNode>(deleteTarget, currentToken().line, currentToken().column);
}

std::shared_ptr<CHTLASTNode> CHTLParser::parseInherit() {
    consumeToken(TokenType::INHERIT);
    
    // 解析继承目标
    String inheritTarget;
    while (!matchToken(TokenType::SEMICOLON) && !isAtEnd()) {
        inheritTarget += currentToken().value + " ";
        skipToken();
    }
    
    if (matchToken(TokenType::SEMICOLON)) {
        skipToken();
    }
    
    return std::make_shared<InheritNode>(inheritTarget, currentToken().line, currentToken().column);
}

std::shared_ptr<CHTLASTNode> CHTLParser::parseExcept() {
    consumeToken(TokenType::EXCEPT);
    
    // 解析约束目标
    String exceptTarget;
    while (!matchToken(TokenType::SEMICOLON) && !isAtEnd()) {
        exceptTarget += currentToken().value + " ";
        skipToken();
    }
    
    if (matchToken(TokenType::SEMICOLON)) {
        skipToken();
    }
    
    return std::make_shared<ExceptNode>(exceptTarget, currentToken().line, currentToken().column);
}

void CHTLParser::skipToMatchingBrace() {
    int braceCount = 1;
    
    while (braceCount > 0 && !isAtEnd()) {
        const auto& token = currentToken();
        
        if (token.type == TokenType::LEFT_BRACE) {
            braceCount++;
        } else if (token.type == TokenType::RIGHT_BRACE) {
            braceCount--;
        }
        
        skipToken();
    }
}

std::shared_ptr<CHTLASTNode> CHTLParser::parseElement() {
    const auto& nameToken = consumeToken(); // 元素名
    String elementName = nameToken.value;
    
    if (!isValidElementName(elementName)) {
        reportError(ParseErrorType::INVALID_STRUCTURE, 
                   "无效的元素名: " + elementName);
        return nullptr;
    }
    
    auto element = std::make_shared<ElementNode>(elementName, nameToken.line, nameToken.column);
    
    enterElementContext(elementName);
    
    // 检查约束
    if (!checkElementConstraints(elementName)) {
        reportError(ParseErrorType::CONSTRAINT_VIOLATION,
                   "元素 " + elementName + " 在当前上下文中被禁止使用");
        exitElementContext();
        return nullptr;
    }
    
    if (matchToken(TokenType::LEFT_BRACE)) {
        consumeToken(TokenType::LEFT_BRACE);
        
        // 解析元素内容
        while (!matchToken(TokenType::RIGHT_BRACE) && !isAtEnd()) {
            skipWhitespace();
            skipComments();
            
            if (matchToken(TokenType::RIGHT_BRACE)) break;
            
            auto child = parseElementContent();
            if (child) {
                element->addChild(child);
            }
        }
        
        if (matchToken(TokenType::RIGHT_BRACE)) {
            consumeToken(TokenType::RIGHT_BRACE);
        } else {
            reportError(ParseErrorType::MISSING_TOKEN, "缺少右花括号 '}'");
        }
    }
    
    exitElementContext();
    return element;
}

std::shared_ptr<CHTLASTNode> CHTLParser::parseElementContent() {
    const auto& token = currentToken();
    
    switch (token.type) {
        case TokenType::IDENTIFIER:
        case TokenType::HTML_ELEMENT:
            // 可能是子元素或属性
            if (peekToken().type == TokenType::COLON || peekToken().type == TokenType::EQUALS) {
                return parseAttribute();
            } else {
                return parseElement();
            }
        case TokenType::TEXT:
            return parseText();
        case TokenType::STYLE:
            return parseStyle();
        case TokenType::SCRIPT:
            return parseScript();
        case TokenType::AT_STYLE:
        case TokenType::AT_ELEMENT:
        case TokenType::AT_VAR:
            return parseTemplateUsage();
        case TokenType::TEMPLATE_DECL:
            return parseTemplate();
        case TokenType::CUSTOM_DECL:
            return parseCustom();
        case TokenType::ORIGIN_DECL:
            return parseOrigin();
        case TokenType::EXCEPT:
            return parseExcept();
        case TokenType::COMMENT_GENERATOR:
            return parseGeneratorComment();
        default:
            reportError(ParseErrorType::UNEXPECTED_TOKEN, 
                       "元素内容中意外的Token: " + token.value);
            skipToken();
            return nullptr;
    }
}

std::shared_ptr<CHTLASTNode> CHTLParser::parseText() {
    const auto& textToken = consumeToken(TokenType::TEXT);
    auto textNode = std::make_shared<TextNode>("", textToken.line, textToken.column);
    
    if (matchToken(TokenType::LEFT_BRACE)) {
        consumeToken(TokenType::LEFT_BRACE);
        
        // 解析文本内容
        String textContent;
        while (!matchToken(TokenType::RIGHT_BRACE) && !isAtEnd()) {
            const auto& token = currentToken();
            
            if (token.type == TokenType::STRING_LITERAL ||
                token.type == TokenType::SINGLE_STRING ||
                token.type == TokenType::UNQUOTED_LITERAL) {
                textContent += parseLiteral();
            } else if (token.type == TokenType::IDENTIFIER) {
                // 可能是变量引用
                textContent += parseVariableReference()->toString();
            } else {
                textContent += token.value;
                skipToken();
            }
        }
        
        if (matchToken(TokenType::RIGHT_BRACE)) {
            consumeToken(TokenType::RIGHT_BRACE);
        } else {
            reportError(ParseErrorType::MISSING_TOKEN, "text块缺少右花括号 '}'");
        }
        
        textNode->setContent(textContent);
    }
    
    return textNode;
}

std::shared_ptr<CHTLASTNode> CHTLParser::parseStyle() {
    const auto& styleToken = consumeToken(TokenType::STYLE);
    auto styleBlock = std::make_shared<StyleBlockNode>(styleToken.line, styleToken.column);
    
    enterStyleContext();
    
    if (matchToken(TokenType::LEFT_BRACE)) {
        consumeToken(TokenType::LEFT_BRACE);
        
        while (!matchToken(TokenType::RIGHT_BRACE) && !isAtEnd()) {
            skipWhitespace();
            skipComments();
            
            if (matchToken(TokenType::RIGHT_BRACE)) break;
            
            auto styleContent = parseStyleContent();
            if (styleContent) {
                styleBlock->addChild(styleContent);
            }
        }
        
        if (matchToken(TokenType::RIGHT_BRACE)) {
            consumeToken(TokenType::RIGHT_BRACE);
        } else {
            reportError(ParseErrorType::MISSING_TOKEN, "style块缺少右花括号 '}'");
        }
    }
    
    exitStyleContext();
    return styleBlock;
}

std::shared_ptr<CHTLASTNode> CHTLParser::parseStyleContent() {
    const auto& token = currentToken();
    
    if (token.type == TokenType::DOT) {
        // 类选择器 .className
        return parseClassSelector();
    } else if (token.type == TokenType::HASH) {
        // ID选择器 #idName
        return parseIdSelector();
    } else if (token.type == TokenType::AMPERSAND) {
        // 上下文推导 &:hover, &::before
        return parsePseudoSelector();
    } else if (token.type == TokenType::AT_STYLE) {
        // 样式组模板使用
        return parseTemplateUsage();
    } else if (token.type == TokenType::IDENTIFIER) {
        // 可能是CSS属性或选择器
        if (peekToken().type == TokenType::COLON || peekToken().type == TokenType::EQUALS) {
            return parseStyleProperty();
        } else {
            // 元素选择器或其他
            return parseSelector();
        }
    } else if (token.type == TokenType::DELETE) {
        return parseDelete();
    } else if (token.type == TokenType::INHERIT) {
        return parseInherit();
    } else {
        reportError(ParseErrorType::UNEXPECTED_TOKEN, 
                   "style块中意外的Token: " + token.value);
        skipToken();
        return nullptr;
    }
}

std::shared_ptr<CHTLASTNode> CHTLParser::parseClassSelector() {
    consumeToken(TokenType::DOT); // 消费 '.'
    const auto& nameToken = consumeToken(TokenType::IDENTIFIER);
    
    auto classSelector = std::make_shared<ClassSelectorNode>(nameToken.value, nameToken.line, nameToken.column);
    
    if (matchToken(TokenType::LEFT_BRACE)) {
        consumeToken(TokenType::LEFT_BRACE);
        
        while (!matchToken(TokenType::RIGHT_BRACE) && !isAtEnd()) {
            skipWhitespace();
            skipComments();
            
            if (matchToken(TokenType::RIGHT_BRACE)) break;
            
            auto property = parseStyleProperty();
            if (property) {
                classSelector->addChild(property);
            }
        }
        
        if (matchToken(TokenType::RIGHT_BRACE)) {
            consumeToken(TokenType::RIGHT_BRACE);
        } else {
            reportError(ParseErrorType::MISSING_TOKEN, "类选择器缺少右花括号 '}'");
        }
    }
    
    return classSelector;
}

std::shared_ptr<CHTLASTNode> CHTLParser::parseIdSelector() {
    consumeToken(TokenType::HASH); // 消费 '#'
    const auto& nameToken = consumeToken(TokenType::IDENTIFIER);
    
    auto idSelector = std::make_shared<IdSelectorNode>(nameToken.value, nameToken.line, nameToken.column);
    
    if (matchToken(TokenType::LEFT_BRACE)) {
        consumeToken(TokenType::LEFT_BRACE);
        
        while (!matchToken(TokenType::RIGHT_BRACE) && !isAtEnd()) {
            skipWhitespace();
            skipComments();
            
            if (matchToken(TokenType::RIGHT_BRACE)) break;
            
            auto property = parseStyleProperty();
            if (property) {
                idSelector->addChild(property);
            }
        }
        
        if (matchToken(TokenType::RIGHT_BRACE)) {
            consumeToken(TokenType::RIGHT_BRACE);
        } else {
            reportError(ParseErrorType::MISSING_TOKEN, "ID选择器缺少右花括号 '}'");
        }
    }
    
    return idSelector;
}

std::shared_ptr<CHTLASTNode> CHTLParser::parsePseudoSelector() {
    consumeToken(TokenType::AMPERSAND); // 消费 '&'
    
    String pseudoSelector = "&";
    
    // 解析伪类或伪元素
    while (!matchToken(TokenType::LEFT_BRACE) && !isAtEnd()) {
        pseudoSelector += currentToken().value;
        skipToken();
    }
    
    auto pseudo = std::make_shared<PseudoSelectorNode>(pseudoSelector, currentToken().line, currentToken().column);
    
    if (matchToken(TokenType::LEFT_BRACE)) {
        consumeToken(TokenType::LEFT_BRACE);
        
        while (!matchToken(TokenType::RIGHT_BRACE) && !isAtEnd()) {
            skipWhitespace();
            skipComments();
            
            if (matchToken(TokenType::RIGHT_BRACE)) break;
            
            auto property = parseStyleProperty();
            if (property) {
                pseudo->addChild(property);
            }
        }
        
        if (matchToken(TokenType::RIGHT_BRACE)) {
            consumeToken(TokenType::RIGHT_BRACE);
        } else {
            reportError(ParseErrorType::MISSING_TOKEN, "伪选择器缺少右花括号 '}'");
        }
    }
    
    return pseudo;
}

std::shared_ptr<CHTLASTNode> CHTLParser::parseStyleProperty() {
    const auto& propToken = consumeToken(TokenType::IDENTIFIER);
    String propertyName = propToken.value;
    
    // CE对等式：支持 ':' 或 '='
    if (matchToken(TokenType::COLON) || matchToken(TokenType::EQUALS)) {
        skipToken(); // 消费 ':' 或 '='
    } else {
        reportError(ParseErrorType::MISSING_TOKEN, "CSS属性缺少 ':' 或 '='");
        return nullptr;
    }
    
    String propertyValue = parseLiteral();
    
    // 可选的分号
    if (matchToken(TokenType::SEMICOLON)) {
        skipToken();
    }
    
    auto property = std::make_shared<StylePropertyNode>(propertyName, propertyValue, 
                                                       propToken.line, propToken.column);
    return property;
}

std::shared_ptr<AttributeNode> CHTLParser::parseAttribute() {
    const auto& nameToken = consumeToken(TokenType::IDENTIFIER);
    String attributeName = nameToken.value;
    
    if (!isValidAttributeName(attributeName)) {
        reportError(ParseErrorType::INVALID_STRUCTURE, "无效的属性名: " + attributeName);
        return nullptr;
    }
    
    // CE对等式：支持 ':' 或 '='
    if (matchToken(TokenType::COLON) || matchToken(TokenType::EQUALS)) {
        skipToken(); // 消费 ':' 或 '='
    } else {
        reportError(ParseErrorType::MISSING_TOKEN, "属性缺少 ':' 或 '='");
        return nullptr;
    }
    
    String attributeValue = parseLiteral();
    
    // 可选的分号
    if (matchToken(TokenType::SEMICOLON)) {
        skipToken();
    }
    
    auto attribute = std::make_shared<AttributeNode>(attributeName, attributeValue, 
                                                    nameToken.line, nameToken.column);
    return attribute;
}

std::shared_ptr<CHTLASTNode> CHTLParser::parseScript() {
    const auto& scriptToken = consumeToken(TokenType::SCRIPT);
    auto scriptBlock = std::make_shared<ScriptBlockNode>(scriptToken.line, scriptToken.column);
    
    enterScriptContext();
    
    if (matchToken(TokenType::LEFT_BRACE)) {
        consumeToken(TokenType::LEFT_BRACE);
        
        // 收集script内容，这里需要特殊处理CHTL JS语法
        String scriptContent;
        int braceCount = 1;
        
        while (braceCount > 0 && !isAtEnd()) {
            const auto& token = currentToken();
            
            if (token.type == TokenType::LEFT_BRACE) {
                braceCount++;
            } else if (token.type == TokenType::RIGHT_BRACE) {
                braceCount--;
            }
            
            if (braceCount > 0) {
                scriptContent += token.value;
                if (token.type != TokenType::WHITESPACE) {
                    scriptContent += " ";
                }
            }
            
            skipToken();
        }
        
        // 设置脚本内容，后续将由CHTL JS解析器处理
        scriptBlock->setContent(scriptContent);
    }
    
    exitScriptContext();
    return scriptBlock;
}

std::shared_ptr<CHTLASTNode> CHTLParser::parseTemplate() {
    const auto& templateToken = consumeToken(TokenType::TEMPLATE_DECL);
    
    // 解析模板类型
    if (!matchAnyToken({TokenType::AT_STYLE, TokenType::AT_ELEMENT, TokenType::AT_VAR})) {
        reportError(ParseErrorType::TEMPLATE_ERROR, "模板声明后必须跟模板类型");
        return nullptr;
    }
    
    const auto& typeToken = consumeToken();
    String templateType = typeToken.value;
    
    const auto& nameToken = consumeToken(TokenType::IDENTIFIER);
    String templateName = nameToken.value;
    
    if (templateType == "@Style") {
        return parseTemplateStyleImpl(templateName, nameToken.line, nameToken.column);
    } else if (templateType == "@Element") {
        return parseTemplateElementImpl(templateName, nameToken.line, nameToken.column);
    } else if (templateType == "@Var") {
        return parseTemplateVarImpl(templateName, nameToken.line, nameToken.column);
    } else {
        reportError(ParseErrorType::TEMPLATE_ERROR, "未知的模板类型: " + templateType);
        return nullptr;
    }
}

std::shared_ptr<TemplateStyleNode> CHTLParser::parseTemplateStyleImpl(const String& templateName, size_t line, size_t column) {
    auto templateStyle = std::make_shared<TemplateStyleNode>(templateName, line, column);
    
    enterTemplateContext(templateName);
    
    if (matchToken(TokenType::LEFT_BRACE)) {
        consumeToken(TokenType::LEFT_BRACE);
        
        while (!matchToken(TokenType::RIGHT_BRACE) && !isAtEnd()) {
            skipWhitespace();
            skipComments();
            
            if (matchToken(TokenType::RIGHT_BRACE)) break;
            
            // 解析样式属性或继承
            if (matchToken(TokenType::INHERIT)) {
                auto inherit = parseInherit();
                if (inherit) {
                    auto inheritNode = std::static_pointer_cast<InheritNode>(inherit);
                    templateStyle->addInheritance(inheritNode->getTarget());
                }
            } else if (matchToken(TokenType::AT_STYLE)) {
                // 组合式继承: @Style OtherTemplate;
                consumeToken(TokenType::AT_STYLE);
                const auto& inheritNameToken = consumeToken(TokenType::IDENTIFIER);
                templateStyle->addInheritance(inheritNameToken.value);
                
                if (matchToken(TokenType::SEMICOLON)) {
                    consumeToken(TokenType::SEMICOLON);
                }
            } else {
                auto property = parseStyleProperty();
                if (property) {
                    templateStyle->addChild(property);
                }
            }
        }
        
        if (matchToken(TokenType::RIGHT_BRACE)) {
            consumeToken(TokenType::RIGHT_BRACE);
        } else {
            reportError(ParseErrorType::MISSING_TOKEN, "模板样式缺少右花括号 '}'");
        }
    }
    
    exitTemplateContext();
    return templateStyle;
}

std::shared_ptr<TemplateElementNode> CHTLParser::parseTemplateElementImpl(const String& templateName, size_t line, size_t column) {
    auto templateElement = std::make_shared<TemplateElementNode>(templateName, line, column);
    
    enterTemplateContext(templateName);
    
    if (matchToken(TokenType::LEFT_BRACE)) {
        consumeToken(TokenType::LEFT_BRACE);
        
        while (!matchToken(TokenType::RIGHT_BRACE) && !isAtEnd()) {
            skipWhitespace();
            skipComments();
            
            if (matchToken(TokenType::RIGHT_BRACE)) break;
            
            // 解析元素或继承
            if (matchToken(TokenType::INHERIT)) {
                auto inherit = parseInherit();
                if (inherit) {
                    auto inheritNode = std::static_pointer_cast<InheritNode>(inherit);
                    templateElement->addInheritance(inheritNode->getTarget());
                }
            } else if (matchToken(TokenType::AT_ELEMENT)) {
                // 组合式继承: @Element OtherTemplate;
                consumeToken(TokenType::AT_ELEMENT);
                const auto& inheritNameToken = consumeToken(TokenType::IDENTIFIER);
                templateElement->addInheritance(inheritNameToken.value);
                
                if (matchToken(TokenType::SEMICOLON)) {
                    consumeToken(TokenType::SEMICOLON);
                }
            } else {
                auto element = parseElement();
                if (element) {
                    templateElement->addChild(element);
                }
            }
        }
        
        if (matchToken(TokenType::RIGHT_BRACE)) {
            consumeToken(TokenType::RIGHT_BRACE);
        } else {
            reportError(ParseErrorType::MISSING_TOKEN, "模板元素缺少右花括号 '}'");
        }
    }
    
    exitTemplateContext();
    return templateElement;
}

std::shared_ptr<TemplateVarNode> CHTLParser::parseTemplateVarImpl(const String& templateName, size_t line, size_t column) {
    auto templateVar = std::make_shared<TemplateVarNode>(templateName, line, column);
    
    enterTemplateContext(templateName);
    
    if (matchToken(TokenType::LEFT_BRACE)) {
        consumeToken(TokenType::LEFT_BRACE);
        
        while (!matchToken(TokenType::RIGHT_BRACE) && !isAtEnd()) {
            skipWhitespace();
            skipComments();
            
            if (matchToken(TokenType::RIGHT_BRACE)) break;
            
            // 解析变量定义 variableName: "value";
            const auto& varToken = consumeToken(TokenType::IDENTIFIER);
            String varName = varToken.value;
            
            // CE对等式：支持 ':' 或 '='
            if (matchToken(TokenType::COLON) || matchToken(TokenType::EQUALS)) {
                consumeToken(); // 消费 : 或 =
                
                String varValue = parseLiteral();
                templateVar->addVariable(varName, varValue);
                
                // 可选的分号
                if (matchToken(TokenType::SEMICOLON)) {
                    consumeToken(TokenType::SEMICOLON);
                }
            } else {
                reportError(ParseErrorType::SYNTAX_ERROR, "变量定义缺少 ':' 或 '='");
            }
        }
        
        if (matchToken(TokenType::RIGHT_BRACE)) {
            consumeToken(TokenType::RIGHT_BRACE);
        } else {
            reportError(ParseErrorType::MISSING_TOKEN, "模板变量缺少右花括号 '}'");
        }
    }
    
    exitTemplateContext();
    return templateVar;
}

String CHTLParser::parseLiteral() {
    const auto& token = currentToken();
    
    switch (token.type) {
        case TokenType::STRING_LITERAL:
        case TokenType::SINGLE_STRING:
            skipToken();
            // 移除引号
            return token.value.substr(1, token.value.length() - 2);
            
        case TokenType::UNQUOTED_LITERAL:
        case TokenType::IDENTIFIER:
        case TokenType::NUMBER:
            skipToken();
            return token.value;
            
        default:
            reportError(ParseErrorType::UNEXPECTED_TOKEN, "期望字面量，得到: " + token.value);
            return "";
    }
}

// Token操作实现
const Token& CHTLParser::currentToken() const {
    static Token eofToken{TokenType::EOF_TOKEN, "", 0, 0};
    return currentPos_ < tokens_.size() ? tokens_[currentPos_] : eofToken;
}

const Token& CHTLParser::peekToken(size_t offset) const {
    static Token eofToken{TokenType::EOF_TOKEN, "", 0, 0};
    size_t pos = currentPos_ + offset;
    return pos < tokens_.size() ? tokens_[pos] : eofToken;
}

Token CHTLParser::consumeToken() {
    if (currentPos_ < tokens_.size()) {
        return tokens_[currentPos_++];
    }
    return Token{TokenType::EOF_TOKEN, "", 0, 0};
}

Token CHTLParser::consumeToken(TokenType expectedType) {
    const auto& token = currentToken();
    if (token.type != expectedType) {
        reportError(ParseErrorType::UNEXPECTED_TOKEN, 
                   "期望 " + tokenTypeToString(expectedType) + "，得到 " + token.value);
    }
    return consumeToken();
}

bool CHTLParser::matchToken(TokenType type) const {
    return currentToken().type == type;
}

bool CHTLParser::matchAnyToken(const std::vector<TokenType>& types) const {
    const auto& currentType = currentToken().type;
    return std::find(types.begin(), types.end(), currentType) != types.end();
}

void CHTLParser::skipToken() {
    if (currentPos_ < tokens_.size()) {
        currentPos_++;
    }
}

bool CHTLParser::isAtEnd() const {
    return currentPos_ >= tokens_.size() || currentToken().type == TokenType::EOF_TOKEN;
}

void CHTLParser::skipWhitespace() {
    while (matchToken(TokenType::WHITESPACE) && !isAtEnd()) {
        skipToken();
    }
}

void CHTLParser::skipComments() {
    while ((matchToken(TokenType::COMMENT_SINGLE) || matchToken(TokenType::COMMENT_MULTI)) && !isAtEnd()) {
        skipToken();
    }
}

// 错误处理
void CHTLParser::reportError(ParseErrorType type, const String& message, const String& context) {
    const auto& token = currentToken();
    result_.addError(type, message, token.line, token.column, context);
    
    if (debugMode_) {
        std::cerr << "解析错误: " << message << " (行:" << token.line << ", 列:" << token.column << ")" << std::endl;
    }
}

void CHTLParser::reportWarning(const String& message) {
    result_.addWarning(message);
    
    if (debugMode_) {
        std::cerr << "解析警告: " << message << std::endl;
    }
}

// 上下文管理
void CHTLParser::enterElementContext(const String& elementName) {
    currentContext_.nestingLevel++;
    contextStack_.push("element:" + elementName);
    
    if (debugMode_) {
        debugLog("进入元素上下文: " + elementName);
    }
}

void CHTLParser::exitElementContext() {
    if (currentContext_.nestingLevel > 0) {
        currentContext_.nestingLevel--;
    }
    if (!contextStack_.empty()) {
        contextStack_.pop();
    }
    
    if (debugMode_) {
        debugLog("退出元素上下文");
    }
}

void CHTLParser::enterStyleContext() {
    currentContext_.inLocalStyle = true;
    contextStack_.push("style");
    
    if (debugMode_) {
        debugLog("进入样式上下文");
    }
}

void CHTLParser::exitStyleContext() {
    currentContext_.inLocalStyle = false;
    if (!contextStack_.empty()) {
        contextStack_.pop();
    }
    
    if (debugMode_) {
        debugLog("退出样式上下文");
    }
}

void CHTLParser::enterScriptContext() {
    currentContext_.inLocalScript = true;
    contextStack_.push("script");
    
    if (debugMode_) {
        debugLog("进入脚本上下文");
    }
}

void CHTLParser::exitScriptContext() {
    currentContext_.inLocalScript = false;
    if (!contextStack_.empty()) {
        contextStack_.pop();
    }
    
    if (debugMode_) {
        debugLog("退出脚本上下文");
    }
}

void CHTLParser::enterTemplateContext(const String& templateName) {
    currentContext_.inTemplate = true;
    currentContext_.templateNames.insert(templateName);
    contextStack_.push("template:" + templateName);
    
    if (debugMode_) {
        debugLog("进入模板上下文: " + templateName);
    }
}

void CHTLParser::exitTemplateContext() {
    currentContext_.inTemplate = false;
    if (!contextStack_.empty()) {
        contextStack_.pop();
    }
    
    if (debugMode_) {
        debugLog("退出模板上下文");
    }
}

void CHTLParser::enterCustomContext(const String& customName) {
    currentContext_.inCustom = true;
    currentContext_.customNames.insert(customName);
    contextStack_.push("custom:" + customName);
    
    if (debugMode_) {
        debugLog("进入自定义上下文: " + customName);
    }
}

void CHTLParser::exitCustomContext() {
    currentContext_.inCustom = false;
    if (!contextStack_.empty()) {
        contextStack_.pop();
    }
    
    if (debugMode_) {
        debugLog("退出自定义上下文");
    }
}

// 语法验证
bool CHTLParser::isValidElementName(const String& name) const {
    // HTML元素名验证
    if (name.empty()) return false;
    
    // 检查是否为有效的HTML元素名或自定义元素名
    return std::all_of(name.begin(), name.end(), [](char c) {
        return std::isalnum(c) || c == '-' || c == '_';
    });
}

bool CHTLParser::isValidAttributeName(const String& name) const {
    if (name.empty()) return false;
    
    return std::all_of(name.begin(), name.end(), [](char c) {
        return std::isalnum(c) || c == '-' || c == '_';
    });
}

// 约束检查
bool CHTLParser::checkElementConstraints(const String& elementName) const {
    return currentContext_.forbiddenElements.find(elementName) == currentContext_.forbiddenElements.end();
}

void CHTLParser::debugLog(const String& message) const {
    if (debugMode_) {
        std::cout << "[CHTL Parser] " << message << std::endl;
    }
}

// 工厂方法实现
std::unique_ptr<CHTLParser> CHTLParserFactory::createParser(const CompilerOptions& options) {
    return std::make_unique<CHTLParser>(options);
}

std::unique_ptr<CHTLParser> CHTLParserFactory::createDebugParser() {
    CompilerOptions debugOptions;
    debugOptions.debugMode = true;
    return std::make_unique<CHTLParser>(debugOptions);
}

// 辅助函数
String tokenTypeToString(TokenType type) {
    switch (type) {
        case TokenType::IDENTIFIER: return "IDENTIFIER";
        case TokenType::STRING_LITERAL: return "STRING_LITERAL";
        case TokenType::LEFT_BRACE: return "LEFT_BRACE";
        case TokenType::RIGHT_BRACE: return "RIGHT_BRACE";
        case TokenType::COLON: return "COLON";
        case TokenType::SEMICOLON: return "SEMICOLON";
        default: return "UNKNOWN";
    }
}

} // namespace chtl