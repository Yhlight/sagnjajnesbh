#include "CHTL/Parser/Parser.h"
#include <iostream>
#include <sstream>
#include <algorithm>
#include <stack>
#include <cctype>

namespace CHTL {
namespace Compiler {

class Parser::Impl {
public:
    CompileContext* context;
    ParserConfig config;
    std::vector<std::string> errors;
    
    // 解析状态
    struct ParseState {
        const std::vector<Token>* tokens;
        size_t currentIndex;
        
        bool IsEOF() const {
            return currentIndex >= tokens->size() || 
                   Current().type == TokenType::EOF_TOKEN;
        }
        
        const Token& Current() const {
            if (currentIndex < tokens->size()) {
                return (*tokens)[currentIndex];
            }
            static Token eof(TokenType::EOF_TOKEN, "", 0, 0);
            return eof;
        }
        
        const Token& Peek(size_t offset = 1) const {
            size_t index = currentIndex + offset;
            if (index < tokens->size()) {
                return (*tokens)[index];
            }
            static Token eof(TokenType::EOF_TOKEN, "", 0, 0);
            return eof;
        }
        
        void Advance() {
            if (currentIndex < tokens->size()) {
                currentIndex++;
            }
        }
        
        bool Match(TokenType type) {
            if (Current().type == type) {
                Advance();
                return true;
            }
            return false;
        }
        
        bool Expect(TokenType type, const std::string& message) {
            if (Current().type == type) {
                Advance();
                return true;
            }
            return false;
        }
    };
    
    Impl(CompileContext* ctx, const ParserConfig& cfg) 
        : context(ctx), config(cfg) {}
    
    std::shared_ptr<ASTNode> Parse(const std::vector<Token>& tokens) {
        ParseState state;
        state.tokens = &tokens;
        state.currentIndex = 0;
        
        auto root = std::make_shared<ASTNode>(ASTNodeType::ROOT);
        
        // 解析顶层声明
        while (!state.IsEOF()) {
            auto node = ParseTopLevel(state);
            if (node) {
                root->AddChild(node);
            } else if (!state.IsEOF()) {
                // 跳过当前token，避免无限循环
                state.Advance();
            }
        }
        
        return root;
    }
    
private:
    std::shared_ptr<ASTNode> ParseTopLevel(ParseState& state) {
        const Token& current = state.Current();
        
        // 跳过注释
        if (current.type == TokenType::COMMENT || 
            current.type == TokenType::MULTI_COMMENT) {
            state.Advance();
            return nullptr;
        }
        
        // 生成器注释
        if (current.type == TokenType::GENERATOR_COMMENT) {
            auto node = std::make_shared<ASTNode>(ASTNodeType::GENERATOR_COMMENT, 
                                                  current.value, 
                                                  current.line, 
                                                  current.column);
            state.Advance();
            return node;
        }
        
        // 处理方括号声明
        if (current.type == TokenType::KEYWORD_TEMPLATE ||
            current.type == TokenType::KEYWORD_CUSTOM ||
            current.type == TokenType::KEYWORD_ORIGIN ||
            current.type == TokenType::KEYWORD_CONFIGURATION ||
            current.type == TokenType::KEYWORD_NAMESPACE ||
            current.type == TokenType::KEYWORD_IMPORT) {
            return ParseDeclaration(state);
        }
        
        // HTML元素
        if (current.type == TokenType::IDENTIFIER && 
            TokenUtils::IsHTMLElement(current.value)) {
            return ParseHTMLElement(state);
        }
        
        AddError("意外的token: " + current.value, current.line, current.column);
        return nullptr;
    }
    
    std::shared_ptr<ASTNode> ParseDeclaration(ParseState& state) {
        const Token& keyword = state.Current();
        
        switch (keyword.type) {
            case TokenType::KEYWORD_TEMPLATE:
                return ParseTemplateDeclaration(state);
            case TokenType::KEYWORD_CUSTOM:
                return ParseCustomDeclaration(state);
            case TokenType::KEYWORD_ORIGIN:
                return ParseOriginDeclaration(state);
            case TokenType::KEYWORD_CONFIGURATION:
                return ParseConfigurationDeclaration(state);
            case TokenType::KEYWORD_NAMESPACE:
                return ParseNamespaceDeclaration(state);
            case TokenType::KEYWORD_IMPORT:
                return ParseImportDeclaration(state);
            default:
                return nullptr;
        }
    }
    
    std::shared_ptr<ASTNode> ParseTemplateDeclaration(ParseState& state) {
        auto node = std::make_shared<ASTNode>(ASTNodeType::TEMPLATE_DECLARATION);
        node->line = state.Current().line;
        node->column = state.Current().column;
        
        state.Advance(); // skip [Template]
        
        // 期望类型标记 @Style, @Element, 或 @Var
        if (!state.IsEOF()) {
            const Token& typeToken = state.Current();
            
            if (typeToken.type == TokenType::AT_STYLE) {
                state.Advance();
                node->SetAttribute("type", "Style");
                return ParseStyleDeclaration(state, node, true);
            } else if (typeToken.type == TokenType::AT_ELEMENT) {
                state.Advance();
                node->SetAttribute("type", "Element");
                return ParseElementDeclaration(state, node, true);
            } else if (typeToken.type == TokenType::AT_VAR) {
                state.Advance();
                node->SetAttribute("type", "Var");
                return ParseVarDeclaration(state, node, true);
            } else {
                AddError("期望类型标记 @Style, @Element 或 @Var", 
                        typeToken.line, typeToken.column);
            }
        }
        
        return nullptr;
    }
    
    std::shared_ptr<ASTNode> ParseCustomDeclaration(ParseState& state) {
        auto node = std::make_shared<ASTNode>(ASTNodeType::CUSTOM_DECLARATION);
        node->line = state.Current().line;
        node->column = state.Current().column;
        
        state.Advance(); // skip [Custom]
        
        // 期望类型标记
        if (!state.IsEOF()) {
            const Token& typeToken = state.Current();
            
            if (typeToken.type == TokenType::AT_STYLE) {
                state.Advance();
                node->SetAttribute("type", "Style");
                return ParseStyleDeclaration(state, node, false);
            } else if (typeToken.type == TokenType::AT_ELEMENT) {
                state.Advance();
                node->SetAttribute("type", "Element");
                return ParseElementDeclaration(state, node, false);
            } else if (typeToken.type == TokenType::AT_VAR) {
                state.Advance();
                node->SetAttribute("type", "Var");
                return ParseVarDeclaration(state, node, false);
            } else {
                AddError("期望类型标记 @Style, @Element 或 @Var", 
                        typeToken.line, typeToken.column);
            }
        }
        
        return nullptr;
    }
    
    std::shared_ptr<ASTNode> ParseStyleDeclaration(ParseState& state, 
                                                   std::shared_ptr<ASTNode> parent,
                                                   bool isTemplate) {
        // 期望样式组名称
        if (state.Current().type != TokenType::IDENTIFIER) {
            AddError("期望样式组名称", state.Current().line, state.Current().column);
            return nullptr;
        }
        
        std::string styleName = state.Current().value;
        parent->value = styleName;
        state.Advance();
        
        // 注册到全局映射表
        GlobalObject obj;
        obj.type = isTemplate ? GlobalObjectType::TEMPLATE_STYLE : GlobalObjectType::CUSTOM_STYLE;
        obj.name = styleName;
        obj.namespacePath = context->GetStateManager().GetCurrentNamespace();
        obj.astNode = parent;
        obj.sourceFile = context->GetCurrentFile();
        obj.line = parent->line;
        obj.column = parent->column;
        
        if (!context->GetGlobalMap().Register(obj)) {
            AddError("样式组 " + styleName + " 已存在", parent->line, parent->column);
        }
        
        // 期望 {
        if (!state.Match(TokenType::LEFT_BRACE)) {
            AddError("期望 {", state.Current().line, state.Current().column);
            return nullptr;
        }
        
        // 解析样式属性
        while (!state.IsEOF() && state.Current().type != TokenType::RIGHT_BRACE) {
            auto prop = ParseStyleProperty(state);
            if (prop) {
                parent->AddChild(prop);
            }
        }
        
        // 期望 }
        if (!state.Match(TokenType::RIGHT_BRACE)) {
            AddError("期望 }", state.Current().line, state.Current().column);
        }
        
        return parent;
    }
    
    std::shared_ptr<ASTNode> ParseElementDeclaration(ParseState& state,
                                                     std::shared_ptr<ASTNode> parent,
                                                     bool isTemplate) {
        // 期望元素名称
        if (state.Current().type != TokenType::IDENTIFIER) {
            AddError("期望元素名称", state.Current().line, state.Current().column);
            return nullptr;
        }
        
        std::string elementName = state.Current().value;
        parent->value = elementName;
        state.Advance();
        
        // 注册到全局映射表
        GlobalObject obj;
        obj.type = isTemplate ? GlobalObjectType::TEMPLATE_ELEMENT : GlobalObjectType::CUSTOM_ELEMENT;
        obj.name = elementName;
        obj.namespacePath = context->GetStateManager().GetCurrentNamespace();
        obj.astNode = parent;
        obj.sourceFile = context->GetCurrentFile();
        obj.line = parent->line;
        obj.column = parent->column;
        
        if (!context->GetGlobalMap().Register(obj)) {
            AddError("元素 " + elementName + " 已存在", parent->line, parent->column);
        }
        
        // 期望 {
        if (!state.Match(TokenType::LEFT_BRACE)) {
            AddError("期望 {", state.Current().line, state.Current().column);
            return nullptr;
        }
        
        // 进入元素上下文
        auto guard = context->GetStateManager().EnterState(CompileState::IN_CUSTOM, elementName);
        
        // 解析元素内容
        while (!state.IsEOF() && state.Current().type != TokenType::RIGHT_BRACE) {
            auto child = ParseElementContent(state);
            if (child) {
                parent->AddChild(child);
            }
        }
        
        // 期望 }
        if (!state.Match(TokenType::RIGHT_BRACE)) {
            AddError("期望 }", state.Current().line, state.Current().column);
        }
        
        return parent;
    }
    
    std::shared_ptr<ASTNode> ParseVarDeclaration(ParseState& state,
                                                 std::shared_ptr<ASTNode> parent,
                                                 bool isTemplate) {
        // 期望变量组名称
        if (state.Current().type != TokenType::IDENTIFIER) {
            AddError("期望变量组名称", state.Current().line, state.Current().column);
            return nullptr;
        }
        
        std::string varName = state.Current().value;
        parent->value = varName;
        state.Advance();
        
        // 注册到全局映射表
        GlobalObject obj;
        obj.type = isTemplate ? GlobalObjectType::TEMPLATE_VAR : GlobalObjectType::CUSTOM_VAR;
        obj.name = varName;
        obj.namespacePath = context->GetStateManager().GetCurrentNamespace();
        obj.astNode = parent;
        obj.sourceFile = context->GetCurrentFile();
        obj.line = parent->line;
        obj.column = parent->column;
        
        if (!context->GetGlobalMap().Register(obj)) {
            AddError("变量组 " + varName + " 已存在", parent->line, parent->column);
        }
        
        // 期望 {
        if (!state.Match(TokenType::LEFT_BRACE)) {
            AddError("期望 {", state.Current().line, state.Current().column);
            return nullptr;
        }
        
        // 解析变量定义
        while (!state.IsEOF() && state.Current().type != TokenType::RIGHT_BRACE) {
            auto var = ParseVarProperty(state);
            if (var) {
                parent->AddChild(var);
            }
        }
        
        // 期望 }
        if (!state.Match(TokenType::RIGHT_BRACE)) {
            AddError("期望 }", state.Current().line, state.Current().column);
        }
        
        return parent;
    }
    
    std::shared_ptr<ASTNode> ParseHTMLElement(ParseState& state) {
        std::string tagName = state.Current().value;
        auto node = std::make_shared<HTMLElementNode>(tagName);
        node->line = state.Current().line;
        node->column = state.Current().column;
        
        state.Advance();
        
        // 期望 {
        if (!state.Match(TokenType::LEFT_BRACE)) {
            AddError("期望 {", state.Current().line, state.Current().column);
            return nullptr;
        }
        
        // 进入元素上下文
        auto guard = context->GetStateManager().EnterState(CompileState::IN_HTML_ELEMENT, tagName);
        
        // 解析元素内容
        while (!state.IsEOF() && state.Current().type != TokenType::RIGHT_BRACE) {
            auto child = ParseElementContent(state);
            if (child) {
                node->AddChild(child);
            }
        }
        
        // 期望 }
        if (!state.Match(TokenType::RIGHT_BRACE)) {
            AddError("期望 }", state.Current().line, state.Current().column);
        }
        
        node->endLine = state.Current().line;
        node->endColumn = state.Current().column;
        
        return node;
    }
    
    std::shared_ptr<ASTNode> ParseElementContent(ParseState& state) {
        const Token& current = state.Current();
        
        // 跳过注释
        if (current.type == TokenType::COMMENT || 
            current.type == TokenType::MULTI_COMMENT) {
            state.Advance();
            return nullptr;
        }
        
        // 生成器注释
        if (current.type == TokenType::GENERATOR_COMMENT) {
            auto node = std::make_shared<ASTNode>(ASTNodeType::GENERATOR_COMMENT, 
                                                  current.value, 
                                                  current.line, 
                                                  current.column);
            state.Advance();
            return node;
        }
        
        // 属性（id: value; 或 class: value;）
        if (current.type == TokenType::IDENTIFIER && 
            (state.Peek().type == TokenType::COLON || 
             state.Peek().type == TokenType::EQUALS)) {
            return ParseAttribute(state);
        }
        
        // text块
        if (current.type == TokenType::KEYWORD_TEXT) {
            return ParseTextNode(state);
        }
        
        // style块
        if (current.type == TokenType::KEYWORD_STYLE) {
            return ParseStyleBlock(state);
        }
        
        // script块
        if (current.type == TokenType::KEYWORD_SCRIPT) {
            return ParseScriptBlock(state);
        }
        
        // 嵌套的HTML元素
        if (current.type == TokenType::IDENTIFIER && 
            TokenUtils::IsHTMLElement(current.value)) {
            return ParseHTMLElement(state);
        }
        
        // @Style, @Element, @Var 使用
        if (current.type == TokenType::AT_STYLE ||
            current.type == TokenType::AT_ELEMENT ||
            current.type == TokenType::AT_VAR) {
            return ParseUsage(state);
        }
        
        // 原始嵌入
        if (current.type == TokenType::KEYWORD_ORIGIN) {
            return ParseOriginEmbed(state);
        }
        
        AddError("意外的元素内容: " + current.value, current.line, current.column);
        state.Advance();
        return nullptr;
    }
    
    std::shared_ptr<ASTNode> ParseAttribute(ParseState& state) {
        auto node = std::make_shared<ASTNode>(ASTNodeType::ATTRIBUTE_STATEMENT);
        node->line = state.Current().line;
        node->column = state.Current().column;
        
        std::string attrName = state.Current().value;
        node->SetAttribute("name", attrName);
        state.Advance();
        
        // 跳过 : 或 =
        if (state.Current().type == TokenType::COLON || 
            state.Current().type == TokenType::EQUALS) {
            state.Advance();
        }
        
        // 获取属性值
        if (state.Current().type == TokenType::STRING_LITERAL ||
            state.Current().type == TokenType::IDENTIFIER ||
            state.Current().type == TokenType::UNQUOTED_LITERAL ||
            state.Current().type == TokenType::NUMBER) {
            node->value = state.Current().value;
            state.Advance();
        } else {
            AddError("期望属性值", state.Current().line, state.Current().column);
        }
        
        // 期望 ;
        if (!state.Match(TokenType::SEMICOLON)) {
            AddError("期望 ;", state.Current().line, state.Current().column);
        }
        
        return node;
    }
    
    std::shared_ptr<ASTNode> ParseTextNode(ParseState& state) {
        auto node = std::make_shared<ASTNode>(ASTNodeType::TEXT_NODE);
        node->line = state.Current().line;
        node->column = state.Current().column;
        
        state.Advance(); // skip text
        
        // 期望 {
        if (!state.Match(TokenType::LEFT_BRACE)) {
            AddError("期望 {", state.Current().line, state.Current().column);
            return nullptr;
        }
        
        // 收集文本内容
        std::string textContent;
        while (!state.IsEOF() && state.Current().type != TokenType::RIGHT_BRACE) {
            if (state.Current().type == TokenType::STRING_LITERAL) {
                textContent += state.Current().value;
            } else if (state.Current().type == TokenType::UNQUOTED_LITERAL ||
                      state.Current().type == TokenType::IDENTIFIER) {
                if (!textContent.empty()) textContent += " ";
                textContent += state.Current().value;
            }
            state.Advance();
        }
        
        node->value = textContent;
        
        // 期望 }
        if (!state.Match(TokenType::RIGHT_BRACE)) {
            AddError("期望 }", state.Current().line, state.Current().column);
        }
        
        return node;
    }
    
    std::shared_ptr<ASTNode> ParseStyleBlock(ParseState& state) {
        auto node = std::make_shared<ASTNode>(ASTNodeType::STYLE_BLOCK);
        node->line = state.Current().line;
        node->column = state.Current().column;
        
        state.Advance(); // skip style
        
        // 期望 {
        if (!state.Match(TokenType::LEFT_BRACE)) {
            AddError("期望 {", state.Current().line, state.Current().column);
            return nullptr;
        }
        
        // 进入样式块上下文
        auto guard = context->GetStateManager().EnterState(CompileState::IN_STYLE_BLOCK);
        
        // 解析样式内容
        while (!state.IsEOF() && state.Current().type != TokenType::RIGHT_BRACE) {
            auto child = ParseStyleContent(state);
            if (child) {
                node->AddChild(child);
            }
        }
        
        // 期望 }
        if (!state.Match(TokenType::RIGHT_BRACE)) {
            AddError("期望 }", state.Current().line, state.Current().column);
        }
        
        return node;
    }
    
    std::shared_ptr<ASTNode> ParseStyleContent(ParseState& state) {
        const Token& current = state.Current();
        
        // 类选择器 .class
        if (current.type == TokenType::DOT && 
            state.Peek().type == TokenType::IDENTIFIER) {
            return ParseClassSelector(state);
        }
        
        // ID选择器 #id
        if (current.type == TokenType::IDENTIFIER && 
            current.value.length() > 0 && current.value[0] == '#') {
            return ParseIdSelector(state);
        }
        
        // & 选择器
        if (current.type == TokenType::AMPERSAND) {
            return ParseAmpersandSelector(state);
        }
        
        // @Style 使用
        if (current.type == TokenType::AT_STYLE) {
            return ParseStyleUse(state);
        }
        
        // 内联样式属性
        if (current.type == TokenType::IDENTIFIER &&
            (state.Peek().type == TokenType::COLON || 
             state.Peek().type == TokenType::EQUALS)) {
            return ParseStyleProperty(state);
        }
        
        AddError("意外的样式内容: " + current.value, current.line, current.column);
        state.Advance();
        return nullptr;
    }
    
    std::shared_ptr<ASTNode> ParseStyleProperty(ParseState& state) {
        std::string property = state.Current().value;
        size_t line = state.Current().line;
        size_t column = state.Current().column;
        
        state.Advance();
        
        // 跳过 : 或 =
        if (state.Current().type == TokenType::COLON || 
            state.Current().type == TokenType::EQUALS) {
            state.Advance();
        }
        
        // 收集属性值
        std::string value;
        while (!state.IsEOF() && state.Current().type != TokenType::SEMICOLON) {
            // 检查是否需要添加空格
            bool needSpace = false;
            if (!value.empty()) {
                char lastChar = value.back();
                const Token& curToken = state.Current();
                
                // 特殊情况不需要空格
                if (lastChar == '#' || lastChar == '(' || lastChar == ',' ||
                    curToken.value == ")" || curToken.value == "," ||
                    curToken.value == "px" || curToken.value == "em" || 
                    curToken.value == "%" || curToken.value == "rem" ||
                    // 颜色值的十六进制数字
                    (lastChar == '#' && std::isxdigit(curToken.value[0])) ||
                    // 连字符后的单词（如text-align）
                    (lastChar == '-' && std::isalpha(curToken.value[0]))) {
                    needSpace = false;
                } else if (state.Current().value == "-" && state.Peek().type == TokenType::NUMBER) {
                    // 负数前需要空格
                    needSpace = true;
                } else {
                    needSpace = true;
                }
            }
            
            if (needSpace) value += " ";
            
            // 处理变量表达式（只处理已注册的变量组）
            if (state.Current().type == TokenType::IDENTIFIER &&
                state.Peek().type == TokenType::LEFT_PAREN) {
                std::string varGroupName = state.Current().value;
                
                // 检查是否是已注册的变量组
                bool isVarGroup = false;
                auto varObj = context->GetGlobalMap().Find(varGroupName, GlobalObjectType::TEMPLATE_VAR);
                if (varObj) {
                    isVarGroup = true;
                } else {
                    varObj = context->GetGlobalMap().Find(varGroupName, GlobalObjectType::CUSTOM_VAR);
                    if (varObj) {
                        isVarGroup = true;
                    }
                }
                
                if (isVarGroup) {
                    auto varExpr = ParseVarExpression(state);
                    if (varExpr) {
                        value += varExpr->value;
                    }
                } else {
                    // 普通CSS函数调用
                    value += state.Current().value;
                    state.Advance();
                    value += state.Current().value; // (
                    state.Advance();
                    
                    // 收集函数参数
                    int parenDepth = 1;
                    while (!state.IsEOF() && parenDepth > 0) {
                        if (state.Current().type == TokenType::LEFT_PAREN) {
                            parenDepth++;
                        } else if (state.Current().type == TokenType::RIGHT_PAREN) {
                            parenDepth--;
                        }
                        
                        value += state.Current().value;
                        state.Advance();
                        
                        // 只在需要时添加空格
                        if (parenDepth > 0 && !state.IsEOF() && 
                            state.Current().type != TokenType::RIGHT_PAREN &&
                            state.Current().type != TokenType::COMMA &&
                            state.Current().value != "," &&
                            state.Peek(-1).type != TokenType::COMMA &&
                            state.Peek(-1).value != ",") {
                            value += " ";
                        }
                    }
                }
            } else {
                // 特殊处理CSS函数和负数
                if (state.Current().value == "-" && 
                    state.Peek().type == TokenType::NUMBER) {
                    value += "-";
                    state.Advance();
                    value += state.Current().value;
                    state.Advance();
                } else {
                    value += state.Current().value;
                    state.Advance();
                }
            }
        }
        
        auto node = std::make_shared<StylePropertyNode>(property, value);
        node->line = line;
        node->column = column;
        
        // 期望 ;
        if (!state.Match(TokenType::SEMICOLON)) {
            AddError("期望 ;", state.Current().line, state.Current().column);
        }
        
        return node;
    }
    
    std::shared_ptr<ASTNode> ParseVarExpression(ParseState& state) {
        std::string varGroupName = state.Current().value;
        size_t line = state.Current().line;
        size_t column = state.Current().column;
        
        state.Advance();
        state.Advance(); // skip (
        
        if (state.Current().type != TokenType::IDENTIFIER) {
            AddError("期望变量键名", state.Current().line, state.Current().column);
            return nullptr;
        }
        
        std::string varKey = state.Current().value;
        state.Advance();
        
        auto node = std::make_shared<VarExpressionNode>(varGroupName, varKey);
        node->line = line;
        node->column = column;
        
        // 可选的默认值
        if (state.Current().type == TokenType::EQUALS) {
            state.Advance();
            
            std::string defaultValue;
            while (!state.IsEOF() && state.Current().type != TokenType::RIGHT_PAREN) {
                if (!defaultValue.empty()) defaultValue += " ";
                defaultValue += state.Current().value;
                state.Advance();
            }
            
            node->defaultValue = defaultValue;
        }
        
        // 期望 )
        if (!state.Match(TokenType::RIGHT_PAREN)) {
            AddError("期望 )", state.Current().line, state.Current().column);
        }
        
        return node;
    }
    
    std::shared_ptr<ASTNode> ParseScriptBlock(ParseState& state) {
        auto node = std::make_shared<ASTNode>(ASTNodeType::SCRIPT_BLOCK);
        node->line = state.Current().line;
        node->column = state.Current().column;
        
        state.Advance(); // skip script
        
        // 期望 {
        if (!state.Match(TokenType::LEFT_BRACE)) {
            AddError("期望 {", state.Current().line, state.Current().column);
            return nullptr;
        }
        
        // 进入脚本块上下文
        auto guard = context->GetStateManager().EnterState(CompileState::IN_SCRIPT_BLOCK);
        
        // 收集脚本内容（暂时作为原始文本）
        std::string scriptContent;
        int braceDepth = 1;
        
        while (!state.IsEOF() && braceDepth > 0) {
            const Token& token = state.Current();
            
            if (token.type == TokenType::LEFT_BRACE) {
                braceDepth++;
            } else if (token.type == TokenType::RIGHT_BRACE) {
                braceDepth--;
                if (braceDepth == 0) break;
            }
            
            scriptContent += token.value;
            if (token.type != TokenType::EOF_TOKEN) {
                scriptContent += " ";
            }
            
            state.Advance();
        }
        
        node->value = scriptContent;
        
        // 期望 }
        if (!state.Match(TokenType::RIGHT_BRACE)) {
            AddError("期望 }", state.Current().line, state.Current().column);
        }
        
        return node;
    }
    
    std::shared_ptr<ASTNode> ParseClassSelector(ParseState& state) {
        auto node = std::make_shared<ASTNode>(ASTNodeType::CLASS_SELECTOR);
        node->line = state.Current().line;
        node->column = state.Current().column;
        
        state.Advance(); // skip .
        
        if (state.Current().type != TokenType::IDENTIFIER) {
            AddError("期望类名", state.Current().line, state.Current().column);
            return nullptr;
        }
        
        node->value = state.Current().value;
        state.Advance();
        
        // 期望 {
        if (!state.Match(TokenType::LEFT_BRACE)) {
            AddError("期望 {", state.Current().line, state.Current().column);
            return nullptr;
        }
        
        // 解析样式属性
        while (!state.IsEOF() && state.Current().type != TokenType::RIGHT_BRACE) {
            auto prop = ParseStyleProperty(state);
            if (prop) {
                node->AddChild(prop);
            }
        }
        
        // 期望 }
        if (!state.Match(TokenType::RIGHT_BRACE)) {
            AddError("期望 }", state.Current().line, state.Current().column);
        }
        
        return node;
    }
    
    std::shared_ptr<ASTNode> ParseIdSelector(ParseState& state) {
        auto node = std::make_shared<ASTNode>(ASTNodeType::ID_SELECTOR);
        node->line = state.Current().line;
        node->column = state.Current().column;
        
        // 去掉 # 前缀
        node->value = state.Current().value.substr(1);
        state.Advance();
        
        // 期望 {
        if (!state.Match(TokenType::LEFT_BRACE)) {
            AddError("期望 {", state.Current().line, state.Current().column);
            return nullptr;
        }
        
        // 解析样式属性
        while (!state.IsEOF() && state.Current().type != TokenType::RIGHT_BRACE) {
            auto prop = ParseStyleProperty(state);
            if (prop) {
                node->AddChild(prop);
            }
        }
        
        // 期望 }
        if (!state.Match(TokenType::RIGHT_BRACE)) {
            AddError("期望 }", state.Current().line, state.Current().column);
        }
        
        return node;
    }
    
    std::shared_ptr<ASTNode> ParseAmpersandSelector(ParseState& state) {
        auto node = std::make_shared<ASTNode>(ASTNodeType::AMPERSAND_SELECTOR);
        node->line = state.Current().line;
        node->column = state.Current().column;
        
        state.Advance(); // skip &
        
        // 收集伪类/伪元素
        std::string selector = "&";
        if (state.Current().type == TokenType::COLON) {
            while (state.Current().type == TokenType::COLON ||
                   state.Current().type == TokenType::IDENTIFIER) {
                selector += state.Current().value;
                state.Advance();
            }
        }
        
        node->value = selector;
        
        // 期望 {
        if (!state.Match(TokenType::LEFT_BRACE)) {
            AddError("期望 {", state.Current().line, state.Current().column);
            return nullptr;
        }
        
        // 解析样式属性
        while (!state.IsEOF() && state.Current().type != TokenType::RIGHT_BRACE) {
            auto prop = ParseStyleProperty(state);
            if (prop) {
                node->AddChild(prop);
            }
        }
        
        // 期望 }
        if (!state.Match(TokenType::RIGHT_BRACE)) {
            AddError("期望 }", state.Current().line, state.Current().column);
        }
        
        return node;
    }
    
    std::shared_ptr<ASTNode> ParseOriginDeclaration(ParseState& state) {
        auto node = std::make_shared<ASTNode>(ASTNodeType::ORIGIN_DECLARATION);
        node->line = state.Current().line;
        node->column = state.Current().column;
        
        state.Advance(); // skip [Origin]
        
        // TODO: 实现原始嵌入解析
        AddError("原始嵌入暂未实现", state.Current().line, state.Current().column);
        return node;
    }
    
    std::shared_ptr<ASTNode> ParseConfigurationDeclaration(ParseState& state) {
        auto node = std::make_shared<ASTNode>(ASTNodeType::CONFIGURATION_DECLARATION);
        node->line = state.Current().line;
        node->column = state.Current().column;
        
        state.Advance(); // skip [Configuration]
        
        // TODO: 实现配置解析
        AddError("配置块暂未实现", state.Current().line, state.Current().column);
        return node;
    }
    
    std::shared_ptr<ASTNode> ParseNamespaceDeclaration(ParseState& state) {
        auto node = std::make_shared<ASTNode>(ASTNodeType::NAMESPACE_DECLARATION);
        node->line = state.Current().line;
        node->column = state.Current().column;
        
        state.Advance(); // skip [Namespace]
        
        // TODO: 实现命名空间解析
        AddError("命名空间暂未实现", state.Current().line, state.Current().column);
        return node;
    }
    
    std::shared_ptr<ASTNode> ParseImportDeclaration(ParseState& state) {
        auto node = std::make_shared<ASTNode>(ASTNodeType::IMPORT_DECLARATION);
        node->line = state.Current().line;
        node->column = state.Current().column;
        
        state.Advance(); // skip [Import]
        
        // TODO: 实现导入解析
        AddError("导入功能暂未实现", state.Current().line, state.Current().column);
        return node;
    }
    
    std::shared_ptr<ASTNode> ParseUsage(ParseState& state) {
        const Token& current = state.Current();
        
        if (current.type == TokenType::AT_STYLE) {
            return ParseStyleUse(state);
        } else if (current.type == TokenType::AT_ELEMENT) {
            return ParseElementUse(state);
        } else if (current.type == TokenType::AT_VAR) {
            return ParseVarUse(state);
        }
        
        AddError("意外的使用语法", state.Current().line, state.Current().column);
        state.Advance();
        return nullptr;
    }
    
    std::shared_ptr<ASTNode> ParseElementUse(ParseState& state) {
        auto node = std::make_shared<ASTNode>(ASTNodeType::ELEMENT_USE);
        node->line = state.Current().line;
        node->column = state.Current().column;
        
        state.Advance(); // skip @Element
        
        // 期望元素名称（可能是IDENTIFIER或UNQUOTED_LITERAL）
        if (state.Current().type != TokenType::IDENTIFIER && 
            state.Current().type != TokenType::UNQUOTED_LITERAL) {
            AddError("期望元素名称", state.Current().line, state.Current().column);
            return nullptr;
        }
        
        std::string elementName = state.Current().value;
        node->value = elementName;
        state.Advance();
        
        // 查找元素定义
        auto elementObj = context->GetGlobalMap().Find(elementName, GlobalObjectType::TEMPLATE_ELEMENT);
        if (!elementObj) {
            elementObj = context->GetGlobalMap().Find(elementName, GlobalObjectType::CUSTOM_ELEMENT);
        }
        
        if (!elementObj) {
            AddError("未找到元素: " + elementName, node->line, node->column);
        } else {
            node->SetAttribute("type", elementObj->type == GlobalObjectType::TEMPLATE_ELEMENT ? "template" : "custom");
        }
        
        // 期望 ;
        if (!state.Match(TokenType::SEMICOLON)) {
            AddError("期望 ;", state.Current().line, state.Current().column);
        }
        
        return node;
    }
    
    std::shared_ptr<ASTNode> ParseVarUse(ParseState& state) {
        // TODO: 实现@Var使用解析
        AddError("@Var使用暂未实现", state.Current().line, state.Current().column);
        state.Advance();
        return nullptr;
    }
    
    std::shared_ptr<ASTNode> ParseStyleUse(ParseState& state) {
        auto node = std::make_shared<ASTNode>(ASTNodeType::STYLE_USE);
        node->line = state.Current().line;
        node->column = state.Current().column;
        
        state.Advance(); // skip @Style
        
        // 期望样式组名称（可能是IDENTIFIER或UNQUOTED_LITERAL）
        if (state.Current().type != TokenType::IDENTIFIER && 
            state.Current().type != TokenType::UNQUOTED_LITERAL) {
            AddError("期望样式组名称", state.Current().line, state.Current().column);
            return nullptr;
        }
        
        std::string styleName = state.Current().value;
        node->value = styleName;
        state.Advance();
        
        // 查找样式组
        auto styleObj = context->GetGlobalMap().Find(styleName, GlobalObjectType::TEMPLATE_STYLE);
        if (!styleObj) {
            styleObj = context->GetGlobalMap().Find(styleName, GlobalObjectType::CUSTOM_STYLE);
        }
        
        if (!styleObj) {
            AddError("未找到样式组: " + styleName, node->line, node->column);
        } else {
            node->SetAttribute("type", styleObj->type == GlobalObjectType::TEMPLATE_STYLE ? "template" : "custom");
        }
        
        // 期望 ;
        if (!state.Match(TokenType::SEMICOLON)) {
            AddError("期望 ;", state.Current().line, state.Current().column);
        }
        
        return node;
    }
    
    std::shared_ptr<ASTNode> ParseOriginEmbed(ParseState& state) {
        // TODO: 实现原始嵌入使用解析
        AddError("原始嵌入使用暂未实现", state.Current().line, state.Current().column);
        state.Advance();
        return nullptr;
    }
    
    std::shared_ptr<ASTNode> ParseVarProperty(ParseState& state) {
        auto node = std::make_shared<ASTNode>(ASTNodeType::CONFIG_PROPERTY);
        node->line = state.Current().line;
        node->column = state.Current().column;
        
        if (state.Current().type != TokenType::IDENTIFIER) {
            AddError("期望变量名", state.Current().line, state.Current().column);
            return nullptr;
        }
        
        std::string varName = state.Current().value;
        node->SetAttribute("name", varName);
        state.Advance();
        
        // 可选的值（对于自定义变量组，可能没有值）
        if (state.Current().type == TokenType::COLON || 
            state.Current().type == TokenType::EQUALS) {
            state.Advance();
            
            // 收集值
            std::string value;
            while (!state.IsEOF() && 
                   state.Current().type != TokenType::SEMICOLON &&
                   state.Current().type != TokenType::COMMA &&
                   state.Current().type != TokenType::RIGHT_BRACE) {
                if (!value.empty()) value += " ";
                value += state.Current().value;
                state.Advance();
            }
            
            node->value = value;
        }
        
        // 跳过分隔符
        if (state.Current().type == TokenType::SEMICOLON ||
            state.Current().type == TokenType::COMMA) {
            state.Advance();
        }
        
        return node;
    }
    
    void AddError(const std::string& message, size_t line, size_t column) {
        std::stringstream ss;
        ss << context->GetCurrentFile() << ":" << line << ":" << column 
           << " 错误: " << message;
        errors.push_back(ss.str());
        context->AddError(message, line, column);
    }
};

// Parser公共接口实现
Parser::Parser(CompileContext* context, const ParserConfig& config) 
    : pImpl(std::make_unique<Impl>(context, config)) {}

Parser::~Parser() = default;

std::shared_ptr<ASTNode> Parser::Parse(const std::vector<Token>& tokens) {
    return pImpl->Parse(tokens);
}

bool Parser::HasErrors() const {
    return !pImpl->errors.empty();
}

const std::vector<std::string>& Parser::GetErrors() const {
    return pImpl->errors;
}

} // namespace Compiler
} // namespace CHTL