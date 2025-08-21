#include "CHTL/Parser/Parser.h"
#include "CHTL/AST/TemplateNodes.h"
#include "CHTL/AST/SpecialNodes.h"
#include "CHTL/AST/StyleNodes.h"
#include "Utils/Logger.h"
#include <sstream>
#include <set>

namespace CHTL {

// 解析原始嵌入块
std::shared_ptr<AST::ASTNode> Parser::ParseOriginBlock() {
    Consume(TokenType::Origin, "期望 '[Origin]'");
    
    // 解析原始类型
    AST::OriginBlockNode::OriginType originType;
    std::string customTypeName;
    
    if (Match(TokenType::AtHtml)) {
        originType = AST::OriginBlockNode::OriginType::Html;
    } else if (Match(TokenType::AtStyle)) {
        originType = AST::OriginBlockNode::OriginType::Style;
    } else if (Match(TokenType::AtJavaScript)) {
        originType = AST::OriginBlockNode::OriginType::JavaScript;
    } else if (Check(TokenType::Identifier)) {
        // 自定义原始类型
        originType = AST::OriginBlockNode::OriginType::Custom;
        customTypeName = Advance().value;
    } else {
        ReportError("期望原始类型");
        return nullptr;
    }
    
    // 解析名称
    std::string blockName = Consume(TokenType::Identifier, "期望原始块名称").value;
    
    std::shared_ptr<AST::OriginBlockNode> originNode;
    if (originType == AST::OriginBlockNode::OriginType::Custom) {
        originNode = std::make_shared<AST::OriginBlockNode>(customTypeName, blockName, previousToken_);
    } else {
        originNode = std::make_shared<AST::OriginBlockNode>(originType, blockName, previousToken_);
    }
    
    // 解析内容
    Consume(TokenType::LeftBrace, "期望 '{'");
    
    // 收集原始内容直到找到匹配的右大括号
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
    
    originNode->SetContent(content.str());
    
    // 添加到符号表
    Symbol symbol(blockName, SymbolType::OriginBlock, currentFilename_, 
                  previousToken_.line, previousToken_.column);
    globalMap_->AddSymbol(symbol);
    
    return originNode;
}

// 解析导入语句
std::shared_ptr<AST::ASTNode> Parser::ParseImportStatement() {
    Consume(TokenType::Import, "期望 '[Import]'");
    
    // 确定导入类型
    AST::ImportStatementNode::ImportType importType;
    
    // 检查是否有子类型
    if (Check(TokenType::Template) || Check(TokenType::Custom) || Check(TokenType::Origin)) {
        Token categoryToken = Advance();
        
        if (categoryToken.type == TokenType::Template) {
            if (Match(TokenType::AtStyle)) {
                importType = AST::ImportStatementNode::ImportType::TemplateStyle;
            } else if (Match(TokenType::AtElement)) {
                importType = AST::ImportStatementNode::ImportType::TemplateElement;
            } else if (Match(TokenType::AtVar)) {
                importType = AST::ImportStatementNode::ImportType::TemplateVar;
            } else {
                importType = AST::ImportStatementNode::ImportType::Template;
            }
        } else if (categoryToken.type == TokenType::Custom) {
            if (Match(TokenType::AtStyle)) {
                importType = AST::ImportStatementNode::ImportType::CustomStyle;
            } else if (Match(TokenType::AtElement)) {
                importType = AST::ImportStatementNode::ImportType::CustomElement;
            } else if (Match(TokenType::AtVar)) {
                importType = AST::ImportStatementNode::ImportType::CustomVar;
            } else {
                importType = AST::ImportStatementNode::ImportType::Custom;
            }
        } else if (categoryToken.type == TokenType::Origin) {
            importType = AST::ImportStatementNode::ImportType::Origin;
        }
    } else if (Match(TokenType::AtHtml)) {
        importType = AST::ImportStatementNode::ImportType::Html;
    } else if (Match(TokenType::AtStyle)) {
        importType = AST::ImportStatementNode::ImportType::Style;
    } else if (Match(TokenType::AtJavaScript)) {
        importType = AST::ImportStatementNode::ImportType::JavaScript;
    } else if (Match(TokenType::AtChtl)) {
        importType = AST::ImportStatementNode::ImportType::Chtl;
    } else if (Match(TokenType::AtCJmod)) {
        importType = AST::ImportStatementNode::ImportType::CJmod;
    } else if (Match(TokenType::AtConfig)) {
        importType = AST::ImportStatementNode::ImportType::Config;
    } else {
        ReportError("期望导入类型");
        return nullptr;
    }
    
    auto importNode = std::make_shared<AST::ImportStatementNode>(importType, previousToken_);
    
    // 解析目标（可选）
    if (Check(TokenType::Identifier) && !Check(TokenType::From)) {
        importNode->SetTarget(Advance().value);
    }
    
    // 解析from路径
    Consume(TokenType::From, "期望 'from'");
    std::string fromPath = ParseStringLiteral();
    importNode->SetFromPath(fromPath);
    
    // 解析as别名（可选）
    if (Match(TokenType::As)) {
        std::string asName = Consume(TokenType::Identifier, "期望别名").value;
        importNode->SetAsName(asName);
    }
    
    // 可选的分号
    Match(TokenType::Semicolon);
    
    return importNode;
}

// 解析命名空间声明
std::shared_ptr<AST::ASTNode> Parser::ParseNamespaceDeclaration() {
    Consume(TokenType::Namespace, "期望 '[Namespace]'");
    
    std::string namespaceName = Consume(TokenType::Identifier, "期望命名空间名称").value;
    auto namespaceNode = std::make_shared<AST::NamespaceDeclarationNode>(namespaceName, previousToken_);
    
    // 创建命名空间
    globalMap_->CreateNamespace(namespaceName, globalMap_->GetCurrentNamespace());
    
    // 进入命名空间
    globalMap_->EnterNamespace(namespaceName);
    contextManager_->PushContext(ContextUtils::CreateNamespaceContext(namespaceName));
    stateMachine_->PushState(ParseState::InNamespace, namespaceName);
    
    Consume(TokenType::LeftBrace, "期望 '{'");
    
    // 解析命名空间内容
    while (!Check(TokenType::RightBrace) && !IsAtEnd()) {
        auto statement = ParseTopLevelStatement();
        if (statement) {
            namespaceNode->AddChild(statement);
        }
    }
    
    Consume(TokenType::RightBrace, "期望 '}'");
    
    // 退出命名空间
    globalMap_->ExitNamespace();
    contextManager_->PopContext();
    stateMachine_->PopState();
    
    return namespaceNode;
}

// 解析配置块
std::shared_ptr<AST::ASTNode> Parser::ParseConfigurationBlock() {
    Consume(TokenType::Configuration, "期望 '[Configuration]'");
    
    // 配置块名称（可选）
    std::string configName;
    if (Check(TokenType::Identifier)) {
        configName = Advance().value;
    }
    
    auto configNode = std::make_shared<AST::ConfigurationBlockNode>(configName, previousToken_);
    
    Consume(TokenType::LeftBrace, "期望 '{'");
    
    // 解析配置项
    while (!Check(TokenType::RightBrace) && !IsAtEnd()) {
        // 跳过注释
        if (Match(TokenType::SingleLineComment) || Match(TokenType::MultiLineComment)) {
            continue;
        }
        
        // 解析配置项：key: value;
        if (Check(TokenType::Identifier)) {
            std::string key = Advance().value;
            Consume(TokenType::Colon, "期望 ':'");
            std::string value = ParseStringLiteral();
            
            configNode->AddConfig(key, value);
            
            // 可选的分号
            Match(TokenType::Semicolon);
        } else {
            ReportError("期望配置项");
            Advance();
        }
    }
    
    Consume(TokenType::RightBrace, "期望 '}'");
    
    // 添加到符号表
    Symbol symbol(configName.empty() ? "<config>" : configName, 
                  SymbolType::Configuration, currentFilename_, 
                  previousToken_.line, previousToken_.column);
    globalMap_->AddSymbol(symbol);
    
    return configNode;
}

// 解析模板内容
void Parser::ParseTemplateContent(AST::TemplateType type) {
    auto parentNode = contextManager_->GetCurrentContext().parentName;
    
    switch (type) {
        case AST::TemplateType::Style:
            // 样式组模板内容
            while (!Check(TokenType::RightBrace) && !IsAtEnd()) {
                if (Check(TokenType::AtStyle)) {
                    // 模板继承
                    Advance();
                    std::string inheritName = Consume(TokenType::Identifier, "期望模板名称").value;
                    Match(TokenType::Semicolon);
                    // TODO: 处理模板继承
                } else if (Check(TokenType::Inherit)) {
                    // 显式继承
                    Advance();
                    Consume(TokenType::AtStyle, "期望 '@Style'");
                    std::string inheritName = Consume(TokenType::Identifier, "期望模板名称").value;
                    Match(TokenType::Semicolon);
                    // TODO: 处理显式继承
                } else {
                    // 解析样式属性
                    auto property = ParseStyleProperty();
                    if (property) {
                        // TODO: 添加到模板内容
                    }
                }
            }
            break;
            
        case AST::TemplateType::Element:
            // 元素模板内容
            while (!Check(TokenType::RightBrace) && !IsAtEnd()) {
                auto element = ParseTopLevelStatement();
                if (element) {
                    // TODO: 添加到模板内容
                }
            }
            break;
            
        case AST::TemplateType::Var:
            // 变量组模板内容
            while (!Check(TokenType::RightBrace) && !IsAtEnd()) {
                if (Check(TokenType::Identifier)) {
                    std::string varName = Advance().value;
                    Consume(TokenType::Colon, "期望 ':'");
                    std::string varValue = ParseStringLiteral();
                    Match(TokenType::Semicolon);
                    // TODO: 添加变量到模板
                } else {
                    ReportError("期望变量定义");
                    Advance();
                }
            }
            break;
    }
}

// 解析特例化
std::shared_ptr<AST::ASTNode> Parser::ParseSpecialization() {
    auto specNode = std::make_shared<AST::SpecializationNode>(currentToken_);
    
    // 进入特例化上下文
    contextManager_->EnterSpecialization();
    
    Consume(TokenType::LeftBrace, "期望 '{'");
    
    // 解析特例化内容
    while (!Check(TokenType::RightBrace) && !IsAtEnd()) {
        if (Match(TokenType::Delete)) {
            auto deleteBlock = ParseDeleteBlock();
            if (deleteBlock) {
                specNode->AddChild(deleteBlock);
            }
        } else if (Match(TokenType::Insert)) {
            auto insertBlock = ParseInsertBlock();
            if (insertBlock) {
                specNode->AddChild(insertBlock);
            }
        } else {
            // 其他特例化内容
            auto child = ParseTopLevelStatement();
            if (child) {
                specNode->AddChild(child);
            }
        }
    }
    
    Consume(TokenType::RightBrace, "期望 '}'");
    
    // 退出特例化上下文
    contextManager_->ExitSpecialization();
    
    return specNode;
}

// 解析插入块
std::shared_ptr<AST::ASTNode> Parser::ParseInsertBlock() {
    AST::InsertBlockNode::InsertPosition position;
    std::string target;
    
    // 解析插入位置
    if (Match(TokenType::After)) {
        position = AST::InsertBlockNode::InsertPosition::After;
        target = Consume(TokenType::Identifier, "期望目标名称").value;
    } else if (Match(TokenType::Before)) {
        position = AST::InsertBlockNode::InsertPosition::Before;
        target = Consume(TokenType::Identifier, "期望目标名称").value;
    } else if (Match(TokenType::Replace)) {
        position = AST::InsertBlockNode::InsertPosition::Replace;
        target = Consume(TokenType::Identifier, "期望目标名称").value;
    } else if (Match(TokenType::At)) {
        if (Match(TokenType::Top)) {
            position = AST::InsertBlockNode::InsertPosition::AtTop;
        } else if (Match(TokenType::Bottom)) {
            position = AST::InsertBlockNode::InsertPosition::AtBottom;
        } else {
            ReportError("期望 'top' 或 'bottom'");
            return nullptr;
        }
    } else {
        ReportError("期望插入位置");
        return nullptr;
    }
    
    auto insertNode = std::make_shared<AST::InsertBlockNode>(position, target, previousToken_);
    
    Consume(TokenType::LeftBrace, "期望 '{'");
    
    // 解析插入内容
    while (!Check(TokenType::RightBrace) && !IsAtEnd()) {
        auto child = ParseTopLevelStatement();
        if (child) {
            insertNode->AddChild(child);
        }
    }
    
    Consume(TokenType::RightBrace, "期望 '}'");
    
    return insertNode;
}

// 解析删除块
std::shared_ptr<AST::ASTNode> Parser::ParseDeleteBlock() {
    auto deleteNode = std::make_shared<AST::DeleteBlockNode>(previousToken_);
    
    Consume(TokenType::LeftBrace, "期望 '{'");
    
    // 解析删除目标
    while (!Check(TokenType::RightBrace) && !IsAtEnd()) {
        if (Check(TokenType::Identifier)) {
            deleteNode->AddDeleteTarget(Advance().value);
            Match(TokenType::Semicolon);
        } else {
            ReportError("期望删除目标");
            Advance();
        }
    }
    
    Consume(TokenType::RightBrace, "期望 '}'");
    
    return deleteNode;
}

// 解析约束块
std::shared_ptr<AST::ASTNode> Parser::ParseExceptBlock() {
    Consume(TokenType::Except, "期望 'except'");
    
    auto exceptNode = std::make_shared<AST::ExceptBlockNode>(previousToken_);
    
    // 解析约束类型（可选）
    if (currentToken_.value == "type") {
        Advance();
        exceptNode->SetExceptType(AST::ExceptBlockNode::ExceptType::Type);
    } else if (currentToken_.value == "global") {
        Advance();
        exceptNode->SetExceptType(AST::ExceptBlockNode::ExceptType::Global);
    } else {
        exceptNode->SetExceptType(AST::ExceptBlockNode::ExceptType::Precise);
    }
    
    Consume(TokenType::LeftBrace, "期望 '{'");
    
    // 解析约束目标
    while (!Check(TokenType::RightBrace) && !IsAtEnd()) {
        if (Check(TokenType::Identifier) || Check(TokenType::HtmlElement)) {
            exceptNode->AddExceptTarget(Advance().value);
            Match(TokenType::Semicolon);
        } else {
            ReportError("期望约束目标");
            Advance();
        }
    }
    
    Consume(TokenType::RightBrace, "期望 '}'");
    
    return exceptNode;
}

// ParserUtils实现
static const std::set<std::string> voidElements = {
    "area", "base", "br", "col", "embed", "hr", "img", "input",
    "link", "meta", "param", "source", "track", "wbr"
};

static const std::set<std::string> blockElements = {
    "address", "article", "aside", "blockquote", "canvas", "dd", "div",
    "dl", "dt", "fieldset", "figcaption", "figure", "footer", "form",
    "h1", "h2", "h3", "h4", "h5", "h6", "header", "hr", "li", "main",
    "nav", "noscript", "ol", "p", "pre", "section", "table", "tfoot", "ul"
};

static const std::set<std::string> inlineElements = {
    "a", "abbr", "acronym", "b", "bdo", "big", "br", "button", "cite",
    "code", "dfn", "em", "i", "img", "input", "kbd", "label", "map",
    "object", "output", "q", "samp", "script", "select", "small", "span",
    "strong", "sub", "sup", "textarea", "time", "tt", "var"
};

static const std::set<std::string> globalAttributes = {
    "accesskey", "class", "contenteditable", "dir", "draggable", "hidden",
    "id", "lang", "spellcheck", "style", "tabindex", "title", "translate"
};

bool ParserUtils::IsVoidElement(const std::string& tagName) {
    return voidElements.find(tagName) != voidElements.end();
}

bool ParserUtils::IsBlockElement(const std::string& tagName) {
    return blockElements.find(tagName) != blockElements.end();
}

bool ParserUtils::IsInlineElement(const std::string& tagName) {
    return inlineElements.find(tagName) != inlineElements.end();
}

bool ParserUtils::IsGlobalAttribute(const std::string& attrName) {
    return globalAttributes.find(attrName) != globalAttributes.end();
}

bool ParserUtils::IsValidAttributeForElement(const std::string& element, const std::string& attr) {
    // 全局属性对所有元素都有效
    if (IsGlobalAttribute(attr)) return true;
    
    // TODO: 实现元素特定属性验证
    return true;
}

bool ParserUtils::CanContain(const std::string& parent, const std::string& child) {
    // 一些基本的嵌套规则
    if (parent == "p" && IsBlockElement(child)) {
        return false;  // p元素不能包含块级元素
    }
    
    if ((parent == "ul" || parent == "ol") && child != "li") {
        return false;  // ul/ol只能直接包含li
    }
    
    // TODO: 实现更多嵌套规则
    return true;
}

} // namespace CHTL