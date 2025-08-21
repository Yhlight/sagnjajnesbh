#include "chtl_js/parser/CHTLJSParser.h"
#include "chtl_js/lexer/CHTLJSLexer.h"
#include "chtl_js/ast/ScriptBlockNode.h"
#include "chtl_js/ast/EnhancedSelectorNode.h"
#include "chtl_js/ast/OperatorNodes.h"
#include "chtl_js/ast/SpecialFunctionNodes.h"
#include "chtl_js/ast/VirNodes.h"
#include "chtl_js/ast/StateTagNode.h"
#include "chtl_js/ast/ObjectLiteralNode.h"
#include "chtl_js/ast/LiteralNodes.h"
#include "chtl_js/ast/FunctionNodes.h"
#include "chtl_js/ast/JSCodeFragmentNode.h"
#include "utils/Logger.h"
#include <stdexcept>

namespace chtl {
namespace chtljs {
namespace parser {

using namespace compiler;
using namespace ast;

CHTLJSParser::CHTLJSParser()
    : m_Current(0)
    , m_DebugMode(false) {
    m_StateMachine = std::make_unique<CHTLJSStateMachine>();
    m_Context = std::make_unique<CHTLJSContext>();
    m_GlobalMap = std::make_shared<CHTLJSGlobalMap>();
}

CHTLJSParser::~CHTLJSParser() = default;

std::shared_ptr<CHTLJSDocumentNode> CHTLJSParser::Parse(
    const std::vector<CHTLJSToken>& tokens,
    bool isLocal) {
    
    m_Tokens = tokens;
    m_Current = 0;
    m_Errors.clear();
    m_VirRegistry.clear();
    
    auto document = std::make_shared<CHTLJSDocumentNode>();
    document->SetIsLocal(isLocal);
    
    // 进入脚本状态
    m_StateMachine->PushState(CHTLJSStateType::SCRIPT);
    
    // 解析语句
    while (!IsAtEnd()) {
        try {
            auto stmt = ParseStatement();
            if (stmt) {
                document->AddChild(stmt);
            }
        } catch (const std::exception& e) {
            ReportError(std::string("解析错误: ") + e.what());
            Synchronize();
        }
    }
    
    // 退出脚本状态
    m_StateMachine->PopState();
    
    return document;
}

std::shared_ptr<CHTLJSDocumentNode> CHTLJSParser::ParseScript(
    const std::string& scriptContent,
    bool isLocal) {
    
    // 使用CHTL JS词法分析器
    CHTLJSLexer lexer;
    lexer.SetSource(scriptContent, "script");
    auto tokens = lexer.Tokenize();
    
    if (lexer.HasErrors()) {
        for (const auto& error : lexer.GetErrors()) {
            m_Errors.push_back(error);
        }
        return std::make_shared<CHTLJSDocumentNode>();
    }
    
    return Parse(tokens, isLocal);
}

bool CHTLJSParser::IsAtEnd() const {
    return m_Current >= m_Tokens.size() || 
           Current().GetType() == CHTLJSTokenType::END_OF_FILE;
}

const CHTLJSToken& CHTLJSParser::Current() const {
    if (m_Current < m_Tokens.size()) {
        return m_Tokens[m_Current];
    }
    static CHTLJSToken eof(CHTLJSTokenType::END_OF_FILE, "", 0, 0);
    return eof;
}

const CHTLJSToken& CHTLJSParser::Previous() const {
    if (m_Current > 0 && m_Current <= m_Tokens.size()) {
        return m_Tokens[m_Current - 1];
    }
    return Current();
}

const CHTLJSToken& CHTLJSParser::Peek(int offset) const {
    size_t index = m_Current + offset;
    if (index < m_Tokens.size()) {
        return m_Tokens[index];
    }
    static CHTLJSToken eof(CHTLJSTokenType::END_OF_FILE, "", 0, 0);
    return eof;
}

CHTLJSToken CHTLJSParser::Advance() {
    if (!IsAtEnd()) {
        m_Current++;
    }
    return Previous();
}

bool CHTLJSParser::Check(CHTLJSTokenType type) const {
    if (IsAtEnd()) return false;
    return Current().GetType() == type;
}

bool CHTLJSParser::Match(CHTLJSTokenType type) {
    if (Check(type)) {
        Advance();
        return true;
    }
    return false;
}

bool CHTLJSParser::Match(std::initializer_list<CHTLJSTokenType> types) {
    for (auto type : types) {
        if (Check(type)) {
            Advance();
            return true;
        }
    }
    return false;
}

std::shared_ptr<CHTLJSASTNode> CHTLJSParser::ParseStatement() {
    // vir声明
    if (IsVirDeclaration()) {
        return ParseVirDeclaration();
    }
    
    // 表达式语句
    auto expr = ParseExpression();
    if (expr) {
        // 消费分号（如果有）
        Match(CHTLJSTokenType::SEMICOLON);
        
        auto stmt = std::make_shared<ExpressionStatementNode>();
        stmt->SetExpression(expr);
        return stmt;
    }
    
    return nullptr;
}

std::shared_ptr<CHTLJSASTNode> CHTLJSParser::ParseExpression() {
    return ParseArrowExpression();
}

std::shared_ptr<CHTLJSASTNode> CHTLJSParser::ParseArrowExpression() {
    auto left = ParseMemberAccess();
    
    // 检查箭头操作符
    if (Match(CHTLJSTokenType::ARROW)) {
        auto arrow = std::make_shared<ArrowOperatorNode>();
        arrow->SetLeft(left);
        
        // 解析右侧
        if (Check(CHTLJSTokenType::IDENTIFIER)) {
            auto member = Advance().GetValue();
            
            // 检查是否是方法调用
            if (Check(CHTLJSTokenType::LEFT_PAREN)) {
                // 方法调用
                auto call = ParseCall();
                arrow->SetRight(call);
                arrow->SetIsMethodCall(true);
            } else {
                // 属性访问
                auto id = std::make_shared<IdentifierNode>(member);
                arrow->SetRight(id);
                arrow->SetIsMethodCall(false);
            }
            
            return arrow;
        }
    }
    
    return left;
}

std::shared_ptr<CHTLJSASTNode> CHTLJSParser::ParseMemberAccess() {
    auto left = ParseCall();
    
    // 处理点操作符（与箭头操作符等价）
    while (Match(CHTLJSTokenType::DOT)) {
        auto dot = std::make_shared<DotOperatorNode>();
        dot->SetLeft(left);
        
        if (Check(CHTLJSTokenType::IDENTIFIER)) {
            auto member = Advance().GetValue();
            auto id = std::make_shared<IdentifierNode>(member);
            dot->SetRight(id);
            
            // 检查是否是方法调用
            if (Check(CHTLJSTokenType::LEFT_PAREN)) {
                dot->SetIsMethodCall(true);
            }
        }
        
        left = dot;
    }
    
    return left;
}

std::shared_ptr<CHTLJSASTNode> CHTLJSParser::ParseCall() {
    auto expr = ParsePrimary();
    
    // 处理函数调用
    while (true) {
        if (Match(CHTLJSTokenType::LEFT_PAREN)) {
            // 解析参数
            std::vector<std::shared_ptr<CHTLJSASTNode>> args;
            
            if (!Check(CHTLJSTokenType::RIGHT_PAREN)) {
                do {
                    args.push_back(ParseExpression());
                } while (Match(CHTLJSTokenType::COMMA));
            }
            
            if (!Match(CHTLJSTokenType::RIGHT_PAREN)) {
                ReportError("期望 ')' 结束函数调用");
            }
            
            // 根据函数名创建特殊节点
            if (expr->GetType() == CHTLJSNodeType::IDENTIFIER) {
                auto id = std::static_pointer_cast<IdentifierNode>(expr);
                const std::string& name = id->GetName();
                
                if (name == "listen" && args.size() == 1) {
                    auto listen = std::make_shared<ListenCallNode>();
                    if (args[0]->GetType() == CHTLJSNodeType::OBJECT_LITERAL) {
                        listen->SetConfig(std::static_pointer_cast<ObjectLiteralNode>(args[0]));
                    }
                    return listen;
                } else if (name == "delegate" && args.size() == 1) {
                    auto delegate = std::make_shared<DelegateCallNode>();
                    if (args[0]->GetType() == CHTLJSNodeType::OBJECT_LITERAL) {
                        delegate->SetConfig(std::static_pointer_cast<ObjectLiteralNode>(args[0]));
                    }
                    return delegate;
                } else if (name == "animate" && args.size() == 1) {
                    auto animate = std::make_shared<AnimateCallNode>();
                    if (args[0]->GetType() == CHTLJSNodeType::OBJECT_LITERAL) {
                        animate->SetConfig(std::static_pointer_cast<ObjectLiteralNode>(args[0]));
                    }
                    return animate;
                } else if (name == "iNeverAway" && args.size() == 1) {
                    auto ina = std::make_shared<INeverAwayCallNode>();
                    if (args[0]->GetType() == CHTLJSNodeType::OBJECT_LITERAL) {
                        ina->SetConfig(std::static_pointer_cast<ObjectLiteralNode>(args[0]));
                    }
                    return ina;
                } else if (name == "printMylove" && args.size() == 1) {
                    auto pml = std::make_shared<PrintMyLoveCallNode>();
                    if (args[0]->GetType() == CHTLJSNodeType::OBJECT_LITERAL) {
                        pml->SetConfig(std::static_pointer_cast<ObjectLiteralNode>(args[0]));
                    }
                    return pml;
                }
            }
            
            // 普通函数调用（作为JS片段）
            // TODO: 创建函数调用节点
        } else {
            break;
        }
    }
    
    return expr;
}

std::shared_ptr<CHTLJSASTNode> CHTLJSParser::ParsePrimary() {
    // 增强选择器
    if (IsEnhancedSelector()) {
        return ParseEnhancedSelector();
    }
    
    // 虚对象访问
    if (Check(CHTLJSTokenType::IDENTIFIER)) {
        auto name = Current().GetValue();
        if (m_VirRegistry.count(name) > 0) {
            Advance();
            
            if (Match(CHTLJSTokenType::ARROW) || Match(CHTLJSTokenType::DOT)) {
                auto virAccess = std::make_shared<VirAccessNode>();
                virAccess->SetVirObject(name);
                virAccess->SetUseArrow(Previous().GetType() == CHTLJSTokenType::ARROW);
                
                if (Check(CHTLJSTokenType::IDENTIFIER)) {
                    virAccess->SetMember(Advance().GetValue());
                    
                    // 检查是否是方法调用
                    if (Check(CHTLJSTokenType::LEFT_PAREN)) {
                        virAccess->SetIsMethodCall(true);
                        // TODO: 解析参数
                    }
                }
                
                return virAccess;
            }
        }
    }
    
    // 标识符
    if (Check(CHTLJSTokenType::IDENTIFIER)) {
        return ParseIdentifier();
    }
    
    // 字面量
    if (Check(CHTLJSTokenType::STRING)) {
        return std::make_shared<StringLiteralNode>(Advance().GetValue());
    }
    
    if (Check(CHTLJSTokenType::NUMBER)) {
        return std::make_shared<NumberLiteralNode>(std::stod(Advance().GetValue()));
    }
    
    if (Match(CHTLJSTokenType::TRUE)) {
        return std::make_shared<BooleanLiteralNode>(true);
    }
    
    if (Match(CHTLJSTokenType::FALSE)) {
        return std::make_shared<BooleanLiteralNode>(false);
    }
    
    // 对象字面量
    if (Check(CHTLJSTokenType::LEFT_BRACE)) {
        return ParseObjectLiteral();
    }
    
    // 数组字面量
    if (Check(CHTLJSTokenType::LEFT_BRACKET)) {
        return ParseArrayLiteral();
    }
    
    // 其他情况作为JS代码片段
    auto jsFragment = std::make_shared<JSCodeFragmentNode>(Current().GetValue());
    Advance();
    return jsFragment;
}

std::shared_ptr<EnhancedSelectorNode> CHTLJSParser::ParseEnhancedSelector() {
    // 消费 {{
    Advance();
    Advance();
    
    std::string selector;
    
    // 收集选择器内容直到 }}
    while (!IsAtEnd() && !(Check(CHTLJSTokenType::RIGHT_BRACE) && 
                           Peek(1).GetType() == CHTLJSTokenType::RIGHT_BRACE)) {
        selector += Current().GetValue();
        Advance();
    }
    
    // 消费 }}
    if (Match(CHTLJSTokenType::RIGHT_BRACE) && Match(CHTLJSTokenType::RIGHT_BRACE)) {
        auto node = std::make_shared<EnhancedSelectorNode>(selector);
        
        // 检查索引访问
        if (Match(CHTLJSTokenType::LEFT_BRACKET)) {
            if (Check(CHTLJSTokenType::NUMBER)) {
                int index = std::stoi(Advance().GetValue());
                node->SetIndex(index);
                
                if (!Match(CHTLJSTokenType::RIGHT_BRACKET)) {
                    ReportError("期望 ']' 结束索引");
                }
            }
        }
        
        return node;
    } else {
        ReportError("期望 '}}' 结束增强选择器");
        return std::make_shared<EnhancedSelectorNode>(selector);
    }
}

std::shared_ptr<VirDeclarationNode> CHTLJSParser::ParseVirDeclaration() {
    Advance(); // 消费 vir
    
    if (!Check(CHTLJSTokenType::IDENTIFIER)) {
        ReportError("期望虚对象名称");
        return nullptr;
    }
    
    auto name = Advance().GetValue();
    auto virNode = std::make_shared<VirDeclarationNode>(name);
    
    if (!Match(CHTLJSTokenType::EQUAL)) {
        ReportError("期望 '=' 在虚对象声明中");
        return virNode;
    }
    
    // 解析初始化表达式
    auto init = ParseExpression();
    virNode->SetInitializer(init);
    
    // 如果初始化是CHTL JS函数，提取函数键
    if (init && (init->GetType() == CHTLJSNodeType::LISTEN_CALL ||
                 init->GetType() == CHTLJSNodeType::I_NEVER_AWAY_CALL)) {
        // TODO: 提取函数键
    }
    
    // 注册虚对象
    RegisterVirObject(virNode);
    
    return virNode;
}

std::shared_ptr<ObjectLiteralNode> CHTLJSParser::ParseObjectLiteral() {
    Advance(); // 消费 {
    
    auto obj = std::make_shared<ObjectLiteralNode>();
    
    while (!Check(CHTLJSTokenType::RIGHT_BRACE) && !IsAtEnd()) {
        // 解析属性
        if (Check(CHTLJSTokenType::IDENTIFIER)) {
            auto prop = ParseProperty();
            if (prop) {
                obj->AddProperty(prop);
            }
        }
        
        // 逗号分隔
        if (!Match(CHTLJSTokenType::COMMA)) {
            break;
        }
    }
    
    if (!Match(CHTLJSTokenType::RIGHT_BRACE)) {
        ReportError("期望 '}' 结束对象字面量");
    }
    
    return obj;
}

std::shared_ptr<PropertyNode> CHTLJSParser::ParseProperty() {
    auto key = Advance().GetValue();
    auto prop = std::make_shared<PropertyNode>(key);
    
    // 检查状态标签（用于iNeverAway）
    if (Match(CHTLJSTokenType::LESS)) {
        if (Check(CHTLJSTokenType::IDENTIFIER)) {
            auto state = Advance().GetValue();
            auto stateTag = std::make_shared<StateTagNode>(state);
            stateTag->SetIsForDeclaration(true);
            prop->SetStateTag(stateTag);
            
            if (!Match(CHTLJSTokenType::GREATER)) {
                ReportError("期望 '>' 结束状态标签");
            }
        }
    }
    
    if (!Match(CHTLJSTokenType::COLON)) {
        ReportError("期望 ':' 在属性键后");
        return prop;
    }
    
    // 解析属性值
    auto value = ParseExpression();
    prop->SetValue(value);
    
    return prop;
}

std::shared_ptr<ArrayLiteralNode> CHTLJSParser::ParseArrayLiteral() {
    Advance(); // 消费 [
    
    auto arr = std::make_shared<ArrayLiteralNode>();
    
    while (!Check(CHTLJSTokenType::RIGHT_BRACKET) && !IsAtEnd()) {
        arr->AddElement(ParseExpression());
        
        if (!Match(CHTLJSTokenType::COMMA)) {
            break;
        }
    }
    
    if (!Match(CHTLJSTokenType::RIGHT_BRACKET)) {
        ReportError("期望 ']' 结束数组字面量");
    }
    
    return arr;
}

std::shared_ptr<IdentifierNode> CHTLJSParser::ParseIdentifier() {
    return std::make_shared<IdentifierNode>(Advance().GetValue());
}

bool CHTLJSParser::IsEnhancedSelector() {
    return Check(CHTLJSTokenType::LEFT_BRACE) && 
           Peek(1).GetType() == CHTLJSTokenType::LEFT_BRACE;
}

bool CHTLJSParser::IsCHTLJSFunction(const std::string& name) {
    return name == "listen" || 
           name == "delegate" || 
           name == "animate" || 
           name == "iNeverAway" || 
           name == "printMylove";
}

bool CHTLJSParser::IsVirDeclaration() {
    return Check(CHTLJSTokenType::VIR);
}

void CHTLJSParser::RegisterVirObject(std::shared_ptr<VirDeclarationNode> virNode) {
    if (virNode) {
        m_VirRegistry[virNode->GetVirName()] = virNode;
        m_GlobalMap->AddVirtualObject(virNode->GetVirName());
    }
}

std::shared_ptr<VirDeclarationNode> CHTLJSParser::GetVirObject(const std::string& name) {
    auto it = m_VirRegistry.find(name);
    if (it != m_VirRegistry.end()) {
        return it->second;
    }
    return nullptr;
}

void CHTLJSParser::Synchronize() {
    Advance();
    
    while (!IsAtEnd()) {
        if (Previous().GetType() == CHTLJSTokenType::SEMICOLON) {
            return;
        }
        
        switch (Current().GetType()) {
            case CHTLJSTokenType::VIR:
            case CHTLJSTokenType::LEFT_BRACE:
                return;
            default:
                break;
        }
        
        Advance();
    }
}

void CHTLJSParser::ReportError(const std::string& message) {
    auto& token = Current();
    ReportError(token, message);
}

void CHTLJSParser::ReportError(const CHTLJSToken& token, const std::string& message) {
    std::string error = "[CHTL JS 行 " + std::to_string(token.GetLine()) + 
                       ", 列 " + std::to_string(token.GetColumn()) + "] " + message;
    m_Errors.push_back(error);
    
    if (m_DebugMode) {
        utils::Logger::GetInstance().Error(error);
    }
}

} // namespace parser
} // namespace chtljs
} // namespace chtl