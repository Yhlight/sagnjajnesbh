#include "chtl/parser/CHTLParser.h"
#include "chtl/ast/TemplateNode.h"
#include "chtl/ast/CustomNode.h"
#include "chtl/ast/SpecializationNodes.h"
#include "utils/Logger.h"

namespace chtl {
namespace parser {

using namespace compiler;
using namespace ast;

std::shared_ptr<TemplateNode> CHTLParser::ParseTemplate() {
    Advance(); // 消费 [Template]
    
    // 期望 @Style, @Element 或 @Var
    if (!Match({CHTLTokenType::AT_STYLE, CHTLTokenType::AT_ELEMENT, CHTLTokenType::AT_VAR})) {
        ReportError("期望 @Style, @Element 或 @Var 在 [Template] 后");
        return nullptr;
    }
    
    auto templateType = Previous().GetType();
    
    // 期望模板名称
    if (!Check(CHTLTokenType::IDENTIFIER)) {
        ReportError("期望模板名称");
        return nullptr;
    }
    
    auto templateName = Advance().GetValue();
    
    if (templateType == CHTLTokenType::AT_STYLE) {
        return ParseTemplateStyle();
    } else if (templateType == CHTLTokenType::AT_ELEMENT) {
        return ParseTemplateElement();
    } else {
        return ParseTemplateVar();
    }
}

std::shared_ptr<TemplateStyleNode> CHTLParser::ParseTemplateStyle() {
    auto templateName = Previous().GetValue();
    auto node = std::make_shared<TemplateStyleNode>(templateName);
    node->SetLocation(Previous().GetLine(), Previous().GetColumn());
    
    // 期望 {
    if (!ExpectToken(CHTLTokenType::LEFT_BRACE, "期望 '{' 在模板样式 " + templateName + " 后")) {
        return node;
    }
    
    // 进入模板状态
    EnterState(CHTLStateType::TEMPLATE);
    m_GlobalMap->AddTemplate(templateName, "Style");
    
    // 解析样式属性
    while (!Check(CHTLTokenType::RIGHT_BRACE) && !IsAtEnd()) {
        // @Style 引用（继承）
        if (Check(CHTLTokenType::AT_STYLE)) {
            auto styleRef = ParseStyleReference();
            if (styleRef) {
                node->AddStyleReference(styleRef);
                node->AddInheritedTemplate(styleRef->GetStyleName());
            }
            continue;
        }
        
        // inherit 显式继承
        if (Match(CHTLTokenType::KEYWORD_INHERIT)) {
            if (Match(CHTLTokenType::AT_STYLE)) {
                if (Check(CHTLTokenType::IDENTIFIER)) {
                    auto inheritName = Advance().GetValue();
                    node->AddInheritedTemplate(inheritName);
                    ExpectToken(CHTLTokenType::SEMICOLON, "期望 ';'");
                }
            }
            continue;
        }
        
        // 样式属性
        if (Check(CHTLTokenType::IDENTIFIER)) {
            auto propName = Advance().GetValue();
            
            if (Match(CHTLTokenType::COLON)) {
                std::string value;
                while (!Check(CHTLTokenType::SEMICOLON) && !IsAtEnd()) {
                    value += Current().GetValue() + " ";
                    Advance();
                }
                if (!value.empty()) {
                    value.pop_back();
                }
                
                ExpectToken(CHTLTokenType::SEMICOLON, "期望 ';'");
                node->AddProperty(propName, value);
            }
            continue;
        }
        
        // 跳过未知token
        Advance();
    }
    
    // 期望 }
    ExpectToken(CHTLTokenType::RIGHT_BRACE, "期望 '}' 结束模板样式");
    
    // 退出模板状态
    ExitState();
    
    return node;
}

std::shared_ptr<TemplateElementNode> CHTLParser::ParseTemplateElement() {
    auto templateName = Previous().GetValue();
    auto node = std::make_shared<TemplateElementNode>(templateName);
    node->SetLocation(Previous().GetLine(), Previous().GetColumn());
    
    // 期望 {
    if (!ExpectToken(CHTLTokenType::LEFT_BRACE, "期望 '{' 在模板元素 " + templateName + " 后")) {
        return node;
    }
    
    // 进入模板状态
    EnterState(CHTLStateType::TEMPLATE);
    m_GlobalMap->AddTemplate(templateName, "Element");
    
    // 解析元素内容
    while (!Check(CHTLTokenType::RIGHT_BRACE) && !IsAtEnd()) {
        // 元素
        if (Check(CHTLTokenType::IDENTIFIER)) {
            auto element = ParseElement();
            if (element) {
                node->AddElement(element);
            }
            continue;
        }
        
        // @Element 引用
        if (Check(CHTLTokenType::AT_ELEMENT)) {
            auto elemRef = ParseElementReference();
            if (elemRef) {
                node->AddElementReference(elemRef);
            }
            continue;
        }
        
        // text
        if (Check(CHTLTokenType::KEYWORD_TEXT)) {
            auto textNode = ParseTextNode();
            if (textNode) {
                auto textElement = std::make_shared<ElementNode>("text");
                textElement->AddChild(textNode);
                node->AddElement(textElement);
            }
            continue;
        }
        
        // 跳过未知token
        Advance();
    }
    
    // 期望 }
    ExpectToken(CHTLTokenType::RIGHT_BRACE, "期望 '}' 结束模板元素");
    
    // 退出模板状态
    ExitState();
    
    return node;
}

std::shared_ptr<TemplateVarNode> CHTLParser::ParseTemplateVar() {
    auto templateName = Previous().GetValue();
    auto node = std::make_shared<TemplateVarNode>(templateName);
    node->SetLocation(Previous().GetLine(), Previous().GetColumn());
    
    // 期望 {
    if (!ExpectToken(CHTLTokenType::LEFT_BRACE, "期望 '{' 在变量组模板 " + templateName + " 后")) {
        return node;
    }
    
    // 进入模板状态
    EnterState(CHTLStateType::TEMPLATE);
    m_GlobalMap->AddTemplate(templateName, "Var");
    
    // 解析变量
    while (!Check(CHTLTokenType::RIGHT_BRACE) && !IsAtEnd()) {
        if (Check(CHTLTokenType::IDENTIFIER)) {
            auto varName = Advance().GetValue();
            
            if (Match(CHTLTokenType::COLON)) {
                std::string value;
                if (Check(CHTLTokenType::STRING_LITERAL)) {
                    value = Advance().GetValue();
                } else {
                    // 收集到分号为止
                    while (!Check(CHTLTokenType::SEMICOLON) && !IsAtEnd()) {
                        value += Current().GetValue() + " ";
                        Advance();
                    }
                    if (!value.empty()) {
                        value.pop_back();
                    }
                }
                
                ExpectToken(CHTLTokenType::SEMICOLON, "期望 ';'");
                node->AddVariable(varName, value);
            }
            continue;
        }
        
        // 跳过未知token
        Advance();
    }
    
    // 期望 }
    ExpectToken(CHTLTokenType::RIGHT_BRACE, "期望 '}' 结束变量组模板");
    
    // 退出模板状态
    ExitState();
    
    return node;
}

std::shared_ptr<CustomNode> CHTLParser::ParseCustom() {
    Advance(); // 消费 [Custom]
    
    // 期望 @Style, @Element 或 @Var
    if (!Match({CHTLTokenType::AT_STYLE, CHTLTokenType::AT_ELEMENT, CHTLTokenType::AT_VAR})) {
        ReportError("期望 @Style, @Element 或 @Var 在 [Custom] 后");
        return nullptr;
    }
    
    auto customType = Previous().GetType();
    
    // 期望自定义名称
    if (!Check(CHTLTokenType::IDENTIFIER)) {
        ReportError("期望自定义名称");
        return nullptr;
    }
    
    auto customName = Advance().GetValue();
    
    if (customType == CHTLTokenType::AT_STYLE) {
        return ParseCustomStyle();
    } else if (customType == CHTLTokenType::AT_ELEMENT) {
        return ParseCustomElement();
    } else {
        return ParseCustomVar();
    }
}

std::shared_ptr<CustomStyleNode> CHTLParser::ParseCustomStyle() {
    auto customName = Previous().GetValue();
    auto node = std::make_shared<CustomStyleNode>(customName);
    node->SetLocation(Previous().GetLine(), Previous().GetColumn());
    
    // 期望 {
    if (!ExpectToken(CHTLTokenType::LEFT_BRACE, "期望 '{' 在自定义样式 " + customName + " 后")) {
        return node;
    }
    
    // 进入自定义状态
    EnterState(CHTLStateType::CUSTOM);
    m_GlobalMap->AddCustom(customName, "Style");
    
    // 解析内容
    while (!Check(CHTLTokenType::RIGHT_BRACE) && !IsAtEnd()) {
        // 无值属性（后面跟逗号或分号）
        if (Check(CHTLTokenType::IDENTIFIER)) {
            auto peek1 = Peek(1);
            if (peek1.GetType() == CHTLTokenType::COMMA || 
                peek1.GetType() == CHTLTokenType::SEMICOLON) {
                auto propName = Advance().GetValue();
                node->AddEmptyProperty(propName);
                
                if (Match(CHTLTokenType::COMMA)) {
                    // 继续解析更多无值属性
                    continue;
                } else {
                    ExpectToken(CHTLTokenType::SEMICOLON, "期望 ';'");
                }
                continue;
            }
        }
        
        // @Style 引用（可能带特化）
        if (Check(CHTLTokenType::AT_STYLE)) {
            auto styleRef = ParseStyleReference();
            if (styleRef) {
                node->AddStyleReference(styleRef);
                
                // 检查是否有特化块
                if (Check(CHTLTokenType::LEFT_BRACE)) {
                    Advance(); // 消费 {
                    
                    // 解析特化内容
                    while (!Check(CHTLTokenType::RIGHT_BRACE) && !IsAtEnd()) {
                        if (Match(CHTLTokenType::KEYWORD_DELETE)) {
                            // delete属性或继承
                            if (Check(CHTLTokenType::IDENTIFIER)) {
                                // 删除属性
                                while (Check(CHTLTokenType::IDENTIFIER)) {
                                    auto prop = Advance().GetValue();
                                    styleRef->AddDeletedProperty(prop);
                                    
                                    if (!Match(CHTLTokenType::COMMA)) {
                                        break;
                                    }
                                }
                            } else if (Match(CHTLTokenType::AT_STYLE)) {
                                // 删除继承
                                if (Check(CHTLTokenType::IDENTIFIER)) {
                                    Advance(); // 跳过继承的样式名
                                    styleRef->SetDeleteInheritance(true);
                                }
                            }
                            ExpectToken(CHTLTokenType::SEMICOLON, "期望 ';'");
                        } else if (Check(CHTLTokenType::IDENTIFIER)) {
                            // 覆盖属性
                            auto prop = Advance().GetValue();
                            if (Match(CHTLTokenType::COLON)) {
                                std::string value;
                                while (!Check(CHTLTokenType::SEMICOLON) && !IsAtEnd()) {
                                    value += Current().GetValue() + " ";
                                    Advance();
                                }
                                if (!value.empty()) {
                                    value.pop_back();
                                }
                                ExpectToken(CHTLTokenType::SEMICOLON, "期望 ';'");
                                styleRef->AddOverride(prop, value);
                            }
                        } else {
                            Advance();
                        }
                    }
                    
                    ExpectToken(CHTLTokenType::RIGHT_BRACE, "期望 '}' 结束特化块");
                }
            }
            continue;
        }
        
        // 有值属性
        if (Check(CHTLTokenType::IDENTIFIER)) {
            auto propName = Advance().GetValue();
            
            if (Match(CHTLTokenType::COLON)) {
                std::string value;
                while (!Check(CHTLTokenType::SEMICOLON) && !IsAtEnd()) {
                    value += Current().GetValue() + " ";
                    Advance();
                }
                if (!value.empty()) {
                    value.pop_back();
                }
                
                ExpectToken(CHTLTokenType::SEMICOLON, "期望 ';'");
                node->AddProperty(propName, value);
            }
            continue;
        }
        
        // 跳过未知token
        Advance();
    }
    
    // 期望 }
    ExpectToken(CHTLTokenType::RIGHT_BRACE, "期望 '}' 结束自定义样式");
    
    // 退出自定义状态
    ExitState();
    
    return node;
}

std::shared_ptr<CustomElementNode> CHTLParser::ParseCustomElement() {
    auto customName = Previous().GetValue();
    auto node = std::make_shared<CustomElementNode>(customName);
    node->SetLocation(Previous().GetLine(), Previous().GetColumn());
    
    // 期望 {
    if (!ExpectToken(CHTLTokenType::LEFT_BRACE, "期望 '{' 在自定义元素 " + customName + " 后")) {
        return node;
    }
    
    // 进入自定义状态
    EnterState(CHTLStateType::CUSTOM);
    m_GlobalMap->AddCustom(customName, "Element");
    
    // 解析内容
    while (!Check(CHTLTokenType::RIGHT_BRACE) && !IsAtEnd()) {
        // 元素
        if (Check(CHTLTokenType::IDENTIFIER)) {
            auto element = ParseElement();
            if (element) {
                node->AddElement(element);
            }
            continue;
        }
        
        // @Element 引用
        if (Check(CHTLTokenType::AT_ELEMENT)) {
            auto elemRef = ParseElementReference();
            if (elemRef) {
                node->AddElementReference(elemRef);
            }
            continue;
        }
        
        // 插入操作
        if (Check(CHTLTokenType::KEYWORD_INSERT)) {
            auto insert = ParseInsert();
            if (insert) {
                node->AddInsertOperation(insert);
            }
            continue;
        }
        
        // 删除操作
        if (Check(CHTLTokenType::KEYWORD_DELETE)) {
            auto deleteOp = ParseDelete();
            if (deleteOp) {
                node->AddDeleteOperation(deleteOp);
            }
            continue;
        }
        
        // 跳过未知token
        Advance();
    }
    
    // 期望 }
    ExpectToken(CHTLTokenType::RIGHT_BRACE, "期望 '}' 结束自定义元素");
    
    // 退出自定义状态
    ExitState();
    
    return node;
}

std::shared_ptr<CustomVarNode> CHTLParser::ParseCustomVar() {
    auto customName = Previous().GetValue();
    auto node = std::make_shared<CustomVarNode>(customName);
    node->SetLocation(Previous().GetLine(), Previous().GetColumn());
    
    // 期望 {
    if (!ExpectToken(CHTLTokenType::LEFT_BRACE, "期望 '{' 在自定义变量组 " + customName + " 后")) {
        return node;
    }
    
    // 进入自定义状态
    EnterState(CHTLStateType::CUSTOM);
    m_GlobalMap->AddCustom(customName, "Var");
    
    // 解析变量
    while (!Check(CHTLTokenType::RIGHT_BRACE) && !IsAtEnd()) {
        if (Check(CHTLTokenType::IDENTIFIER)) {
            auto varName = Advance().GetValue();
            
            if (Match(CHTLTokenType::COLON)) {
                std::string value;
                if (Check(CHTLTokenType::STRING_LITERAL)) {
                    value = Advance().GetValue();
                } else {
                    while (!Check(CHTLTokenType::SEMICOLON) && !IsAtEnd()) {
                        value += Current().GetValue() + " ";
                        Advance();
                    }
                    if (!value.empty()) {
                        value.pop_back();
                    }
                }
                
                ExpectToken(CHTLTokenType::SEMICOLON, "期望 ';'");
                node->AddVariable(varName, value);
            }
            continue;
        }
        
        // 变量引用
        if (Check(CHTLTokenType::AT_VAR)) {
            auto varRef = ParseVarReference();
            if (varRef) {
                node->AddVarReference(varRef);
            }
            continue;
        }
        
        // 跳过未知token
        Advance();
    }
    
    // 期望 }
    ExpectToken(CHTLTokenType::RIGHT_BRACE, "期望 '}' 结束自定义变量组");
    
    // 退出自定义状态
    ExitState();
    
    return node;
}

} // namespace parser
} // namespace chtl