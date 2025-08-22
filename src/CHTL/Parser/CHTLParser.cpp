// 存根实现 - src/CHTL/Parser/CHTLParser.cpp

#include "CHTL/Parser/CHTLParser.h"
#include "Utils/ErrorHandler.h"
#include "Utils/StringUtils.h"
#include <algorithm>
#include <sstream>

namespace CHTL {
namespace Parser {

CHTLParser::CHTLParser(Core::CHTLGlobalMap& globalMap, Core::CHTLState& stateManager,
                      const ParserConfig& config)
    : config_(config), globalMap_(globalMap), stateManager_(stateManager),
      tokens_(nullptr), nodeCount_(0), inErrorRecovery_(false), errorCount_(0) {}

AST::ASTNodePtr CHTLParser::Parse(Core::TokenStream& tokens, const std::string& fileName) {
    tokens_ = &tokens;
    fileName_ = fileName;
    nodeCount_ = 0;
    errorCount_ = 0;
    inErrorRecovery_ = false;
    context_ = ParseContext();
    
    // 重置Token流
    tokens.Reset();
    
    // 重置状态管理器
    stateManager_.Reset();
    
    if (config_.enableDebug) {
        Utils::ErrorHandler::GetInstance().LogInfo(
            "开始解析文件: " + fileName
        );
    }
    
    // 解析程序
    return ParseProgram();
}

AST::ASTNodePtr CHTLParser::ParseProgram() {
    // 创建根节点
    auto root = std::make_shared<AST::RootNode>();
    root->SetFileName(fileName_);
    
    // 解析顶层声明
    while (!IsAtEnd()) {
        SkipWhitespaceAndComments();
        
        if (IsAtEnd()) {
            break;
        }
        
        auto declaration = ParseDeclaration();
        if (declaration) {
            root->AddChild(declaration);
            nodeCount_++;
        }
        
        // 检查错误数量
        if (errorCount_ >= config_.maxErrors) {
            ReportError("错误数量超过最大限制，停止解析");
            break;
        }
    }
    
    if (config_.enableDebug) {
        Utils::ErrorHandler::GetInstance().LogInfo(
            "解析完成，共创建 " + std::to_string(nodeCount_) + " 个节点"
        );
    }
    
    return root;
}

AST::ASTNodePtr CHTLParser::ParseDeclaration() {
    const auto& token = Current();
    
    // 根据Token类型解析相应的声明
    switch (token.GetType()) {
        case Core::TokenType::TEMPLATE:
            return ParseTemplateDeclaration();
            
        case Core::TokenType::CUSTOM:
            return ParseCustomDeclaration();
            
        case Core::TokenType::ORIGIN:
            return ParseOriginDeclaration();
            
        case Core::TokenType::IMPORT:
            return ParseImportDeclaration();
            
        case Core::TokenType::NAMESPACE:
            return ParseNamespaceDeclaration();
            
        case Core::TokenType::CONFIGURATION:
            return ParseConfigurationDeclaration();
            
        case Core::TokenType::HTML_TAG:
        case Core::TokenType::IDENTIFIER:
            // 可能是HTML元素
            if (Core::CHTLToken::IsHTMLTagString(token.GetValue()) ||
                token.GetValue() == "html" || token.GetValue() == "head" || 
                token.GetValue() == "body" || token.GetValue() == "div") {
                return ParseHTMLElement();
            }
            break;
            
        case Core::TokenType::SINGLE_LINE_COMMENT:
        case Core::TokenType::MULTI_LINE_COMMENT:
        case Core::TokenType::GENERATOR_COMMENT:
            {
                // 创建注释节点
                auto commentNode = std::make_shared<AST::CommentNode>(
                    (token.GetType() == Core::TokenType::SINGLE_LINE_COMMENT) ? 
                        AST::CommentNode::CommentType::SINGLE_LINE :
                    (token.GetType() == Core::TokenType::MULTI_LINE_COMMENT) ?
                        AST::CommentNode::CommentType::MULTI_LINE :
                        AST::CommentNode::CommentType::GENERATOR,
                    token.GetValue(), token);
                Advance(); // 消费注释Token
                return commentNode;
            }
            
        default:
            break;
    }
    
    // 如果无法识别，尝试错误恢复
    ReportError("无法解析的声明: " + token.GetValue());
    
    if (config_.enableErrorRecovery) {
        Synchronize({Core::TokenType::TEMPLATE, Core::TokenType::CUSTOM, 
                    Core::TokenType::ORIGIN, Core::TokenType::IMPORT,
                    Core::TokenType::NAMESPACE, Core::TokenType::CONFIGURATION});
    } else {
        Advance();
    }
    
    return nullptr;
}

AST::ASTNodePtr CHTLParser::ParseTemplateDeclaration() {
    // 消费 [Template] Token
    if (!Consume(Core::TokenType::TEMPLATE, "期望 [Template]")) {
        return nullptr;
    }
    
    // 使用状态保护
    Core::StateGuard guard(stateManager_, Core::CompileState::PARSING_TEMPLATE);
    
    // 解析模板类型
    std::string templateType = ParseTypeIdentifier();
    if (templateType.empty()) {
        ReportError("期望模板类型标识符");
        return nullptr;
    }
    
    // 解析模板名称
    std::string templateName = ParseIdentifier();
    if (templateName.empty()) {
        ReportError("期望模板名称");
        return nullptr;
    }
    
    // 确定模板类型
    AST::TemplateNode::TemplateType type;
    if (templateType == "@Style") {
        type = AST::TemplateNode::TemplateType::STYLE;
    } else if (templateType == "@Element") {
        type = AST::TemplateNode::TemplateType::ELEMENT;
    } else if (templateType == "@Var") {
        type = AST::TemplateNode::TemplateType::VAR;
    } else {
        ReportError("无效的模板类型: " + templateType);
        return nullptr;
    }
    
    // 创建模板节点
    auto templateNode = std::make_shared<AST::TemplateNode>(type, templateName, Current());
    
    // 更新解析上下文
    context_.currentTemplate = templateName;
    
    // 解析模板内容
    if (!Consume(Core::TokenType::LEFT_BRACE, "期望 '{'")) {
        return templateNode;
    }
    
    // 解析模板体
    auto content = ParseBlock();
    for (const auto& child : content) {
        templateNode->AddChild(child);
    }
    
    // 将模板添加到全局映射表
    Core::SymbolInfo symbolInfo;
    symbolInfo.type = (type == AST::TemplateNode::TemplateType::STYLE) ? 
                     Core::SymbolType::TEMPLATE_STYLE :
                     (type == AST::TemplateNode::TemplateType::ELEMENT) ? 
                     Core::SymbolType::TEMPLATE_ELEMENT :
                     Core::SymbolType::TEMPLATE_VAR;
    symbolInfo.name = templateName;
    symbolInfo.namespaceName = context_.currentNamespace;
    symbolInfo.fileName = fileName_;
    symbolInfo.line = templateNode->GetToken().GetLine();
    symbolInfo.column = templateNode->GetToken().GetColumn();
    
    if (!globalMap_.AddSymbol(symbolInfo)) {
        ReportError("模板名称冲突: " + templateName);
    }
    
    return templateNode;
}

AST::ASTNodePtr CHTLParser::ParseCustomDeclaration() {
    // 消费 [Custom] Token
    if (!Consume(Core::TokenType::CUSTOM, "期望 [Custom]")) {
        return nullptr;
    }
    
    // 使用状态保护
    Core::StateGuard guard(stateManager_, Core::CompileState::PARSING_CUSTOM);
    
    // 解析自定义类型
    std::string customType = ParseTypeIdentifier();
    if (customType.empty()) {
        ReportError("期望自定义类型标识符");
        return nullptr;
    }
    
    // 解析自定义名称
    std::string customName = ParseIdentifier();
    if (customName.empty()) {
        ReportError("期望自定义名称");
        return nullptr;
    }
    
    // 确定自定义类型
    AST::CustomNode::CustomType type;
    if (customType == "@Style") {
        type = AST::CustomNode::CustomType::STYLE;
    } else if (customType == "@Element") {
        type = AST::CustomNode::CustomType::ELEMENT;
    } else if (customType == "@Var") {
        type = AST::CustomNode::CustomType::VAR;
    } else {
        ReportError("无效的自定义类型: " + customType);
        return nullptr;
    }
    
    // 创建自定义节点
    auto customNode = std::make_shared<AST::CustomNode>(type, customName, Current());
    
    // 更新解析上下文
    context_.currentCustom = customName;
    
    // 解析自定义内容
    if (!Consume(Core::TokenType::LEFT_BRACE, "期望 '{'")) {
        return customNode;
    }
    
    // 解析自定义体
    auto content = ParseBlock();
    for (const auto& child : content) {
        customNode->AddChild(child);
    }
    
    // 将自定义添加到全局映射表
    Core::SymbolInfo symbolInfo;
    symbolInfo.type = (type == AST::CustomNode::CustomType::STYLE) ? 
                     Core::SymbolType::CUSTOM_STYLE :
                     (type == AST::CustomNode::CustomType::ELEMENT) ? 
                     Core::SymbolType::CUSTOM_ELEMENT :
                     Core::SymbolType::CUSTOM_VAR;
    symbolInfo.name = customName;
    symbolInfo.namespaceName = context_.currentNamespace;
    symbolInfo.fileName = fileName_;
    symbolInfo.line = customNode->GetToken().GetLine();
    symbolInfo.column = customNode->GetToken().GetColumn();
    
    if (!globalMap_.AddSymbol(symbolInfo)) {
        ReportError("自定义名称冲突: " + customName);
    }
    
    return customNode;
}

AST::ASTNodePtr CHTLParser::ParseOriginDeclaration() {
    // 消费 [Origin] Token
    if (!Consume(Core::TokenType::ORIGIN, "期望 [Origin]")) {
        return nullptr;
    }
    
    // 使用状态保护
    Core::StateGuard guard(stateManager_, Core::CompileState::PARSING_ORIGIN);
    
    // 解析原始嵌入类型
    std::string originType = ParseTypeIdentifier();
    if (originType.empty()) {
        ReportError("期望原始嵌入类型标识符");
        return nullptr;
    }
    
    // 解析可选的名称
    std::string originName = "";
    if (Check(Core::TokenType::IDENTIFIER)) {
        originName = ParseIdentifier();
    }
    
    // 确定原始嵌入类型
    AST::OriginNode::OriginType type;
    std::string customTypeName = "";
    
    if (originType == "@Html") {
        type = AST::OriginNode::OriginType::HTML;
    } else if (originType == "@Style") {
        type = AST::OriginNode::OriginType::STYLE;
    } else if (originType == "@JavaScript") {
        type = AST::OriginNode::OriginType::JAVASCRIPT;
    } else {
        // 自定义类型
        type = AST::OriginNode::OriginType::CUSTOM;
        customTypeName = originType;
    }
    
    // 解析原始嵌入内容
    if (!Consume(Core::TokenType::LEFT_BRACE, "期望 '{'")) {
        return nullptr;
    }
    
    // 读取原始内容（保持原样）
    std::string content = "";
    int braceLevel = 1;
    
    while (!IsAtEnd() && braceLevel > 0) {
        const auto& token = Current();
        
        if (token.GetType() == Core::TokenType::LEFT_BRACE) {
            braceLevel++;
        } else if (token.GetType() == Core::TokenType::RIGHT_BRACE) {
            braceLevel--;
        }
        
        if (braceLevel > 0) {
            content += token.GetValue();
            if (token.GetType() == Core::TokenType::NEWLINE) {
                content += "\n";
            } else if (token.GetType() != Core::TokenType::WHITESPACE) {
                content += " ";
            }
        }
        
        Advance();
    }
    
    // 创建原始嵌入节点
    auto originNode = std::make_shared<AST::OriginNode>(type, content, originName, Current());
    if (type == AST::OriginNode::OriginType::CUSTOM) {
        originNode->SetCustomTypeName(customTypeName);
    }
    
    return originNode;
}

AST::ASTNodePtr CHTLParser::ParseHTMLElement() {
    // 解析元素标签名
    std::string tagName = ParseIdentifier();
    if (tagName.empty()) {
        ReportError("期望HTML元素标签名");
        return nullptr;
    }
    
    // 创建元素节点
    auto element = std::make_shared<AST::ElementNode>(tagName, Current());
    
    // 使用状态保护
    Core::StateGuard guard(stateManager_, Core::CompileState::PARSING_HTML_ELEMENT);
    
    // 检查是否有索引访问
    if (Check(Core::TokenType::LEFT_BRACKET)) {
        auto indexAccess = ParseIndexAccess(tagName);
        if (indexAccess) {
            element->AddChild(indexAccess);
        }
    }
    
    // 解析元素内容
    if (Check(Core::TokenType::LEFT_BRACE)) {
        Advance(); // 消费 '{'
        ParseElementContent(element);
        
        if (!Consume(Core::TokenType::RIGHT_BRACE, "期望 '}'")) {
            if (config_.enableErrorRecovery) {
                Synchronize({Core::TokenType::RIGHT_BRACE});
            }
        }
    }
    
    return element;
}

void CHTLParser::ParseElementContent(std::shared_ptr<AST::ElementNode> element) {
    while (!IsAtEnd() && !Check(Core::TokenType::RIGHT_BRACE)) {
        SkipWhitespaceAndComments();
        
        if (IsAtEnd() || Check(Core::TokenType::RIGHT_BRACE)) {
            break;
        }
        
        const auto& token = Current();
        AST::ASTNodePtr child = nullptr;
        
        // 根据Token类型解析元素内容
        switch (token.GetType()) {
            case Core::TokenType::IDENTIFIER:
                // 可能是属性或HTML元素
                if (Peek().GetType() == Core::TokenType::COLON || 
                    Peek().GetType() == Core::TokenType::EQUAL) {
                    // 属性
                    ParseElementAttributes(element);
                    continue;
                } else if (Core::CHTLToken::IsHTMLTagString(token.GetValue())) {
                    // HTML元素
                    child = ParseHTMLElement();
                } else {
                    ReportError("意外的标识符: " + token.GetValue());
                    Advance();
                    continue;
                }
                break;
                
            case Core::TokenType::HTML_TAG:
                child = ParseHTMLElement();
                break;
                
            case Core::TokenType::TEXT:
                child = ParseTextNode();
                break;
                
            case Core::TokenType::STYLE:
                child = ParseStyleBlock();
                break;
                
            case Core::TokenType::SCRIPT:
                child = ParseScriptBlock();
                break;
                
            case Core::TokenType::AT_STYLE:
            case Core::TokenType::AT_ELEMENT:
            case Core::TokenType::AT_VAR:
                child = ParseTemplateReference();
                break;
                
            case Core::TokenType::INHERIT:
                child = ParseInheritanceDeclaration();
                break;
                
            case Core::TokenType::DELETE:
                child = ParseDeletionDeclaration();
                break;
                
            case Core::TokenType::INSERT:
                child = ParseInsertionDeclaration();
                break;
                
            case Core::TokenType::EXCEPT:
                child = ParseConstraintDeclaration();
                break;
                
            default:
                ReportError("意外的元素内容: " + token.GetValue());
                Advance();
                continue;
        }
        
        if (child) {
            element->AddChild(child);
            nodeCount_++;
        }
    }
}

void CHTLParser::ParseElementAttributes(std::shared_ptr<AST::ElementNode> element) {
    while (!IsAtEnd() && 
           !Check(Core::TokenType::LEFT_BRACE) && 
           !Check(Core::TokenType::RIGHT_BRACE) &&
           Check(Core::TokenType::IDENTIFIER)) {
        
        // 解析属性名
        std::string attrName = ParseIdentifier();
        if (attrName.empty()) {
            break;
        }
        
        // 检查分隔符（: 或 =）
        bool usesCEEquality = false;
        if (Check(Core::TokenType::COLON)) {
            Advance();
        } else if (Check(Core::TokenType::EQUAL)) {
            usesCEEquality = true;
            Advance();
        } else {
            ReportError("期望 ':' 或 '=' 在属性 " + attrName + " 后");
            break;
        }
        
        // 解析属性值
        std::string attrValue = ParseStringValue();
        if (attrValue.empty()) {
            ReportError("期望属性值");
            break;
        }
        
        // 创建属性节点
        auto attrNode = std::make_shared<AST::AttributeNode>(attrName, attrValue, Current());
        attrNode->SetUsesCEEquality(usesCEEquality);
        element->AddChild(attrNode);
        
        // 添加属性到元素
        element->AddAttribute(attrName, attrValue);
        
        // 消费可选的分号
        if (Check(Core::TokenType::SEMICOLON)) {
            Advance();
        }
    }
}

AST::ASTNodePtr CHTLParser::ParseTextNode() {
    // 消费 text Token
    if (!Consume(Core::TokenType::TEXT, "期望 'text'")) {
        return nullptr;
    }
    
    // 使用状态保护
    Core::StateGuard guard(stateManager_, Core::CompileState::PARSING_TEXT_NODE);
    
    // 解析文本内容
    if (!Consume(Core::TokenType::LEFT_BRACE, "期望 '{'")) {
        return nullptr;
    }
    
    std::string textContent = "";
    
    // 读取文本内容
    while (!IsAtEnd() && !Check(Core::TokenType::RIGHT_BRACE)) {
        const auto& token = Current();
        
        if (token.GetType() == Core::TokenType::STRING_LITERAL ||
            token.GetType() == Core::TokenType::UNQUOTED_LITERAL ||
            token.GetType() == Core::TokenType::IDENTIFIER ||
            token.GetType() == Core::TokenType::NUMBER) {
            textContent += token.GetValue();
        } else if (token.GetType() == Core::TokenType::WHITESPACE) {
            textContent += " ";
        }
        
        Advance();
    }
    
    // 消费结束的 '}'
    if (!Consume(Core::TokenType::RIGHT_BRACE, "期望 '}'")) {
        if (config_.enableErrorRecovery) {
            Synchronize({Core::TokenType::RIGHT_BRACE});
        }
    }
    
    // 创建文本节点
    auto textNode = std::make_shared<AST::TextNode>(Utils::StringUtils::Trim(textContent), Current());
    
    return textNode;
}

AST::ASTNodePtr CHTLParser::ParseStyleBlock() {
    // 消费 style Token
    if (!Consume(Core::TokenType::STYLE, "期望 'style'")) {
        return nullptr;
    }
    
    // 创建样式块节点
    auto styleBlock = std::make_shared<AST::StyleBlockNode>(Current());
    
    // 使用状态保护
    Core::StateGuard guard(stateManager_, Core::CompileState::PARSING_STYLE_BLOCK);
    
    // 更新解析上下文
    context_.inLocalStyleBlock = true;
    
    // 解析样式块内容
    if (!Consume(Core::TokenType::LEFT_BRACE, "期望 '{'")) {
        return styleBlock;
    }
    
    ParseStyleContent(styleBlock);
    
    if (!Consume(Core::TokenType::RIGHT_BRACE, "期望 '}'")) {
        if (config_.enableErrorRecovery) {
            Synchronize({Core::TokenType::RIGHT_BRACE});
        }
    }
    
    // 恢复解析上下文
    context_.inLocalStyleBlock = false;
    
    return styleBlock;
}

void CHTLParser::ParseStyleContent(std::shared_ptr<AST::StyleBlockNode> styleBlock) {
    while (!IsAtEnd() && !Check(Core::TokenType::RIGHT_BRACE)) {
        SkipWhitespaceAndComments();
        
        if (IsAtEnd() || Check(Core::TokenType::RIGHT_BRACE)) {
            break;
        }
        
        const auto& token = Current();
        AST::ASTNodePtr child = nullptr;
        
        // 根据Token类型解析样式内容
        switch (token.GetType()) {
            case Core::TokenType::CLASS_SELECTOR:
            case Core::TokenType::ID_SELECTOR:
            case Core::TokenType::PSEUDO_CLASS:
            case Core::TokenType::PSEUDO_ELEMENT:
            case Core::TokenType::AMPERSAND:
                child = ParseCSSSelector();
                break;
                
            case Core::TokenType::IDENTIFIER:
                // 可能是CSS属性
                if (Peek().GetType() == Core::TokenType::COLON ||
                    Peek().GetType() == Core::TokenType::EQUAL) {
                    child = ParseCSSProperty();
                } else {
                    ReportError("意外的标识符在样式块中: " + token.GetValue());
                    Advance();
                    continue;
                }
                break;
                
            case Core::TokenType::AT_STYLE:
            case Core::TokenType::AT_VAR:
                child = ParseTemplateReference();
                break;
                
            case Core::TokenType::DELETE:
                child = ParseDeletionDeclaration();
                break;
                
            default:
                ReportError("意外的样式内容: " + token.GetValue());
                Advance();
                continue;
        }
        
        if (child) {
            styleBlock->AddChild(child);
            nodeCount_++;
        }
    }
}

AST::ASTNodePtr CHTLParser::ParseCSSSelector() {
    const auto& token = Current();
    
    AST::CSSSelectorNode::SelectorType selectorType;
    std::string selector = token.GetValue();
    
    // 确定选择器类型
    switch (token.GetType()) {
        case Core::TokenType::CLASS_SELECTOR:
            selectorType = AST::CSSSelectorNode::SelectorType::CLASS;
            break;
        case Core::TokenType::ID_SELECTOR:
            selectorType = AST::CSSSelectorNode::SelectorType::ID;
            break;
        case Core::TokenType::PSEUDO_CLASS:
            selectorType = AST::CSSSelectorNode::SelectorType::PSEUDO_CLASS;
            break;
        case Core::TokenType::PSEUDO_ELEMENT:
            selectorType = AST::CSSSelectorNode::SelectorType::PSEUDO_ELEMENT;
            break;
        case Core::TokenType::AMPERSAND:
            selectorType = AST::CSSSelectorNode::SelectorType::AMPERSAND;
            break;
        default:
            ReportError("无效的CSS选择器类型");
            return nullptr;
    }
    
    Advance(); // 消费选择器Token
    
    // 创建CSS选择器节点
    auto selectorNode = std::make_shared<AST::CSSSelectorNode>(selectorType, selector, token);
    
    // 解析选择器内容
    if (Check(Core::TokenType::LEFT_BRACE)) {
        Advance(); // 消费 '{'
        
        // 解析选择器内的CSS属性
        while (!IsAtEnd() && !Check(Core::TokenType::RIGHT_BRACE)) {
            SkipWhitespaceAndComments();
            
            if (IsAtEnd() || Check(Core::TokenType::RIGHT_BRACE)) {
                break;
            }
            
            auto property = ParseCSSProperty();
            if (property) {
                selectorNode->AddChild(property);
                nodeCount_++;
            }
        }
        
        if (!Consume(Core::TokenType::RIGHT_BRACE, "期望 '}'")) {
            if (config_.enableErrorRecovery) {
                Synchronize({Core::TokenType::RIGHT_BRACE});
            }
        }
    }
    
    return selectorNode;
}

AST::ASTNodePtr CHTLParser::ParseCSSProperty() {
    // 解析属性名
    std::string property = ParseIdentifier();
    if (property.empty()) {
        ReportError("期望CSS属性名");
        return nullptr;
    }
    
    // 检查分隔符（: 或 =）
    bool usesCEEquality = false;
    if (Check(Core::TokenType::COLON)) {
        Advance();
    } else if (Check(Core::TokenType::EQUAL)) {
        usesCEEquality = true;
        Advance();
    } else {
        ReportError("期望 ':' 或 '=' 在CSS属性 " + property + " 后");
        return nullptr;
    }
    
    // 解析属性值
    std::string value = ParseStringValue();
    if (value.empty()) {
        ReportError("期望CSS属性值");
        return nullptr;
    }
    
    // 消费可选的分号
    if (Check(Core::TokenType::SEMICOLON)) {
        Advance();
    }
    
    // 创建CSS属性节点
    auto propertyNode = std::make_shared<AST::CSSPropertyNode>(property, value, Current());
    propertyNode->SetUsesCEEquality(usesCEEquality);
    
    return propertyNode;
}

AST::ASTNodePtr CHTLParser::ParseTemplateReference() {
    // 解析模板类型
    std::string templateType = ParseTypeIdentifier();
    if (templateType.empty()) {
        ReportError("期望模板类型标识符");
        return nullptr;
    }
    
    // 解析模板名称
    std::string templateName = ParseIdentifier();
    if (templateName.empty()) {
        ReportError("期望模板名称");
        return nullptr;
    }
    
    // 检查是否有命名空间引用
    std::string namespaceName = "";
    if (Check(Core::TokenType::FROM)) {
        Advance(); // 消费 from
        namespaceName = ParseIdentifier();
    }
    
    // 创建模板引用节点
    auto templateRef = std::make_shared<AST::TemplateReferenceNode>(
        templateType, templateName, namespaceName, Current());
    
    // 验证模板是否存在
    if (config_.validateSemantics) {
        Core::SymbolType symbolType = (templateType == "@Style") ? 
                                     Core::SymbolType::TEMPLATE_STYLE :
                                     (templateType == "@Element") ? 
                                     Core::SymbolType::TEMPLATE_ELEMENT :
                                     Core::SymbolType::TEMPLATE_VAR;
        
        const Core::SymbolInfo* symbol = globalMap_.FindSymbolByType(templateName, symbolType, namespaceName);
        if (!symbol) {
            ReportError("未找到模板: " + templateType + " " + templateName);
        }
    }
    
    // 消费可选的分号
    if (Check(Core::TokenType::SEMICOLON)) {
        Advance();
    }
    
    return templateRef;
}

// 工具方法实现
bool CHTLParser::Check(Core::TokenType type) {
    return !IsAtEnd() && Current().GetType() == type;
}

bool CHTLParser::CheckAny(const std::vector<Core::TokenType>& types) {
    if (IsAtEnd()) {
        return false;
    }
    
    Core::TokenType currentType = Current().GetType();
    return std::find(types.begin(), types.end(), currentType) != types.end();
}

bool CHTLParser::Consume(Core::TokenType type, const std::string& errorMessage) {
    if (Check(type)) {
        Advance();
        return true;
    }
    
    if (!errorMessage.empty()) {
        ReportError(errorMessage + "，但得到 " + Current().GetValue());
    }
    
    return false;
}

Core::CHTLToken CHTLParser::Advance() {
    if (!IsAtEnd()) {
        auto token = tokens_->Current();
        tokens_->Advance();
        return token;
    }
    return Core::CHTLToken(Core::TokenType::END_OF_FILE, "");
}

const Core::CHTLToken& CHTLParser::Current() const {
    return tokens_->Current();
}

const Core::CHTLToken& CHTLParser::Peek(int offset) const {
    return tokens_->Peek(offset);
}

bool CHTLParser::IsAtEnd() const {
    return tokens_->IsAtEnd() || Current().GetType() == Core::TokenType::END_OF_FILE;
}

void CHTLParser::SkipWhitespaceAndComments() {
    while (!IsAtEnd()) {
        Core::TokenType type = Current().GetType();
        if (type == Core::TokenType::WHITESPACE || 
            type == Core::TokenType::NEWLINE ||
            (type == Core::TokenType::SINGLE_LINE_COMMENT && !config_.enableDebug) ||
            (type == Core::TokenType::MULTI_LINE_COMMENT && !config_.enableDebug)) {
            Advance();
        } else {
            break;
        }
    }
}

std::string CHTLParser::ParseIdentifier() {
    if (Check(Core::TokenType::IDENTIFIER) || Check(Core::TokenType::HTML_TAG)) {
        std::string value = Current().GetValue();
        Advance();
        return value;
    }
    return "";
}

std::string CHTLParser::ParseStringValue() {
    const auto& token = Current();
    
    if (token.GetType() == Core::TokenType::STRING_LITERAL ||
        token.GetType() == Core::TokenType::UNQUOTED_LITERAL ||
        token.GetType() == Core::TokenType::IDENTIFIER ||
        token.GetType() == Core::TokenType::NUMBER) {
        Advance();
        return token.GetValue();
    }
    
    return "";
}

std::string CHTLParser::ParseTypeIdentifier() {
    if (Current().IsTypeKeyword()) {
        std::string value = Current().GetValue();
        Advance();
        return value;
    }
    return "";
}

AST::ASTNodeList CHTLParser::ParseBlock(Core::TokenType endToken) {
    AST::ASTNodeList nodes;
    
    while (!IsAtEnd() && !Check(endToken)) {
        SkipWhitespaceAndComments();
        
        if (IsAtEnd() || Check(endToken)) {
            break;
        }
        
        auto node = ParseDeclaration();
        if (node) {
            nodes.push_back(node);
            nodeCount_++;
        }
    }
    
    return nodes;
}

void CHTLParser::ReportError(const std::string& message) {
    const auto& token = Current();
    Utils::ErrorHandler::GetInstance().LogError(
        message, token.GetFileName(), token.GetLine(), token.GetColumn()
    );
    errorCount_++;
}

void CHTLParser::ReportWarning(const std::string& message) {
    const auto& token = Current();
    Utils::ErrorHandler::GetInstance().LogWarning(
        message, token.GetFileName(), token.GetLine(), token.GetColumn()
    );
}

void CHTLParser::Synchronize(const std::vector<Core::TokenType>& synchronizeTokens) {
    inErrorRecovery_ = true;
    
    // 默认同步Token
    std::vector<Core::TokenType> syncTokens = synchronizeTokens.empty() ? 
        std::vector<Core::TokenType>{
            Core::TokenType::TEMPLATE, Core::TokenType::CUSTOM, Core::TokenType::ORIGIN,
            Core::TokenType::IMPORT, Core::TokenType::NAMESPACE, Core::TokenType::CONFIGURATION,
            Core::TokenType::HTML_TAG, Core::TokenType::RIGHT_BRACE
        } : synchronizeTokens;
    
    while (!IsAtEnd()) {
        if (CheckAny(syncTokens)) {
            inErrorRecovery_ = false;
            return;
        }
        Advance();
    }
    
    inErrorRecovery_ = false;
}

void CHTLParser::Reset() {
    tokens_ = nullptr;
    fileName_.clear();
    context_ = ParseContext();
    nodeCount_ = 0;
    inErrorRecovery_ = false;
    errorCount_ = 0;
}

size_t CHTLParser::GetErrorCount() const {
    return errorCount_;
}

std::string CHTLParser::GetStatistics() const {
    std::ostringstream oss;
    oss << "CHTL语法分析统计:\n";
    oss << "  文件名: " << fileName_ << "\n";
    oss << "  节点总数: " << nodeCount_ << "\n";
    oss << "  错误数量: " << errorCount_ << "\n";
    oss << "  当前命名空间: " << context_.currentNamespace << "\n";
    oss << "  当前模板: " << context_.currentTemplate << "\n";
    oss << "  当前自定义: " << context_.currentCustom << "\n";
    oss << "  嵌套级别: " << context_.nestingLevel << "\n";
    
    return oss.str();
}

// 存根实现（将在完整实现中补充）
AST::ASTNodePtr CHTLParser::ParseImportDeclaration() {
    Advance(); // 跳过Token
    return nullptr;
}

AST::ASTNodePtr CHTLParser::ParseNamespaceDeclaration() {
    Advance(); // 跳过Token
    return nullptr;
}

AST::ASTNodePtr CHTLParser::ParseConfigurationDeclaration() {
    Advance(); // 跳过Token
    return nullptr;
}

AST::ASTNodePtr CHTLParser::ParseScriptBlock() {
    Advance(); // 跳过Token
    return nullptr;
}

AST::ASTNodePtr CHTLParser::ParseInheritanceDeclaration() {
    Advance(); // 跳过Token
    return nullptr;
}

AST::ASTNodePtr CHTLParser::ParseDeletionDeclaration() {
    Advance(); // 跳过Token
    return nullptr;
}

AST::ASTNodePtr CHTLParser::ParseInsertionDeclaration() {
    Advance(); // 跳过Token
    return nullptr;
}

AST::ASTNodePtr CHTLParser::ParseSpecialization() {
    Advance(); // 跳过Token
    return nullptr;
}

AST::ASTNodePtr CHTLParser::ParseCustomReference() {
    Advance(); // 跳过Token
    return nullptr;
}

AST::ASTNodePtr CHTLParser::ParseVariableReference() {
    Advance(); // 跳过Token
    return nullptr;
}

AST::ASTNodePtr CHTLParser::ParseIndexAccess(const std::string& elementName) {
    Advance(); // 跳过Token
    return nullptr;
}

AST::ASTNodePtr CHTLParser::ParseConstraintDeclaration() {
    Advance(); // 跳过Token
    return nullptr;
}

AST::ASTNodePtr CHTLParser::ParseVariableGroup() {
    Advance(); // 跳过Token
    return nullptr;
}

bool CHTLParser::ValidateSemantics(AST::ASTNodePtr node) {
    return true; // 存根实现
}

bool CHTLParser::CheckConstraints(const std::string& nodeName, const std::string& nodeType) {
    return true; // 存根实现
}

std::string CHTLParser::ParseNumberValue() {
    if (Check(Core::TokenType::NUMBER)) {
        std::string value = Current().GetValue();
        Advance();
        return value;
    }
    return "";
}

// ParserFactory 实现
std::unique_ptr<CHTLParser> ParserFactory::CreateDefaultParser(
    Core::CHTLGlobalMap& globalMap, 
    Core::CHTLState& stateManager) {
    return std::make_unique<CHTLParser>(globalMap, stateManager);
}

std::unique_ptr<CHTLParser> ParserFactory::CreateStrictParser(
    Core::CHTLGlobalMap& globalMap, 
    Core::CHTLState& stateManager) {
    ParserConfig config;
    config.strictMode = true;
    config.validateSemantics = true;
    config.enableErrorRecovery = false;
    return std::make_unique<CHTLParser>(globalMap, stateManager, config);
}

std::unique_ptr<CHTLParser> ParserFactory::CreateDebugParser(
    Core::CHTLGlobalMap& globalMap, 
    Core::CHTLState& stateManager) {
    ParserConfig config;
    config.enableDebug = true;
    config.enableErrorRecovery = true;
    config.validateSemantics = true;
    return std::make_unique<CHTLParser>(globalMap, stateManager, config);
}

} // namespace Parser
} // namespace CHTL
