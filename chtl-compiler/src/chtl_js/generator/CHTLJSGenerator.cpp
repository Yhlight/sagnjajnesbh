#include "chtl_js/generator/CHTLJSGenerator.h"
#include "chtl_js/ast/CHTLJSDocumentNode.h"
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
#include <algorithm>

namespace chtl {
namespace chtljs {
namespace generator {

using namespace ast;
using namespace compiler;

CHTLJSGenerator::CHTLJSGenerator()
    : m_CurrentStream(&m_Output)
    , m_IndentLevel(0)
    , m_InObjectLiteral(false)
    , m_InFunctionBody(false)
    , m_PrettyPrint(true)
    , m_IndentSize(2)
    , m_GenerateSourceMap(false) {
    m_GlobalMap = std::make_shared<CHTLJSGlobalMap>();
}

CHTLJSGenerator::~CHTLJSGenerator() = default;

CHTLJSGenerateResult CHTLJSGenerator::Generate(CHTLJSASTNode* root) {
    CHTLJSGenerateResult result;
    
    // 清空状态
    m_Output.str("");
    m_Output.clear();
    m_IndentLevel = 0;
    m_InObjectLiteral = false;
    m_InFunctionBody = false;
    m_GeneratedFunctions.clear();
    m_VirMappings.clear();
    m_UsedSelectors.clear();
    m_FunctionCounter.clear();
    m_Errors.clear();
    
    // 生成工具函数
    GenerateUtilityFunctions();
    
    // 访问AST
    if (root) {
        root->Accept(this);
    }
    
    // 构建结果
    result.Success = m_Errors.empty();
    result.JavaScript = m_Output.str();
    result.Errors = m_Errors;
    result.GeneratedFunctions = m_GeneratedFunctions;
    result.VirMappings = m_VirMappings;
    result.UsedSelectors = m_UsedSelectors;
    
    return result;
}

void CHTLJSGenerator::Write(const std::string& text) {
    *m_CurrentStream << text;
}

void CHTLJSGenerator::WriteLine(const std::string& text) {
    if (m_PrettyPrint) {
        WriteIndent();
    }
    Write(text);
    if (m_PrettyPrint) {
        Write("\n");
    }
}

void CHTLJSGenerator::WriteIndent() {
    if (m_PrettyPrint) {
        for (int i = 0; i < m_IndentLevel * m_IndentSize; ++i) {
            Write(" ");
        }
    }
}

void CHTLJSGenerator::GenerateUtilityFunctions() {
    // 生成CHTL JS工具函数
    WriteLine("// CHTL JS Runtime");
    WriteLine("(function() {");
    IncreaseIndent();
    WriteLine("'use strict';");
    WriteLine();
    
    // 生成选择器函数
    GenerateSelectorFunction();
    WriteLine();
    
    // 生成listen工具
    GenerateListenUtility();
    WriteLine();
    
    // 生成delegate工具和注册表
    GenerateDelegateRegistry();
    GenerateDelegateUtility();
    WriteLine();
    
    // 生成animate工具
    GenerateAnimateUtility();
    WriteLine();
    
    DecreaseIndent();
    WriteLine("})();");
    WriteLine();
}

void CHTLJSGenerator::GenerateSelectorFunction() {
    WriteLine("// 增强选择器函数");
    WriteLine("window.__chtljs_select = function(selector) {");
    IncreaseIndent();
    
    WriteLine("if (!selector) return null;");
    WriteLine();
    WriteLine("// 处理索引访问");
    WriteLine("var match = selector.match(/^(.+?)\\[(\\d+)\\]$/);");
    WriteLine("if (match) {");
    IncreaseIndent();
    WriteLine("var elements = __chtljs_select(match[1]);");
    WriteLine("return elements ? elements[parseInt(match[2])] : null;");
    DecreaseIndent();
    WriteLine("}");
    WriteLine();
    
    WriteLine("// 自动识别选择器类型");
    WriteLine("if (selector[0] === '.') {");
    IncreaseIndent();
    WriteLine("// 类选择器");
    WriteLine("return document.querySelectorAll(selector);");
    DecreaseIndent();
    WriteLine("} else if (selector[0] === '#') {");
    IncreaseIndent();
    WriteLine("// ID选择器");
    WriteLine("return document.querySelector(selector);");
    DecreaseIndent();
    WriteLine("} else if (selector.indexOf(' ') > -1) {");
    IncreaseIndent();
    WriteLine("// 后代选择器");
    WriteLine("return document.querySelectorAll(selector);");
    DecreaseIndent();
    WriteLine("} else {");
    IncreaseIndent();
    WriteLine("// 先尝试作为标签");
    WriteLine("var elements = document.getElementsByTagName(selector);");
    WriteLine("if (elements.length > 0) return elements;");
    WriteLine();
    WriteLine("// 再尝试作为ID");
    WriteLine("var byId = document.getElementById(selector);");
    WriteLine("if (byId) return byId;");
    WriteLine();
    WriteLine("// 最后尝试作为类名");
    WriteLine("return document.getElementsByClassName(selector);");
    DecreaseIndent();
    WriteLine("}");
    
    DecreaseIndent();
    WriteLine("};");
}

void CHTLJSGenerator::GenerateListenUtility() {
    WriteLine("// listen增强监听器");
    WriteLine("window.__chtljs_listen = function(element, config) {");
    IncreaseIndent();
    
    WriteLine("if (!element || !config) return;");
    WriteLine();
    WriteLine("// 如果是NodeList，为每个元素绑定");
    WriteLine("if (element instanceof NodeList || element instanceof HTMLCollection) {");
    IncreaseIndent();
    WriteLine("Array.from(element).forEach(function(el) {");
    IncreaseIndent();
    WriteLine("__chtljs_listen(el, config);");
    DecreaseIndent();
    WriteLine("});");
    WriteLine("return;");
    DecreaseIndent();
    WriteLine("}");
    WriteLine();
    
    WriteLine("// 绑定事件");
    WriteLine("Object.keys(config).forEach(function(event) {");
    IncreaseIndent();
    WriteLine("if (typeof config[event] === 'function') {");
    IncreaseIndent();
    WriteLine("element.addEventListener(event, config[event]);");
    DecreaseIndent();
    WriteLine("}");
    DecreaseIndent();
    WriteLine("});");
    
    DecreaseIndent();
    WriteLine("};");
}

void CHTLJSGenerator::GenerateDelegateRegistry() {
    WriteLine("// 事件委托注册表");
    WriteLine("window." + GetDelegateRegistryName() + " = new Map();");
}

void CHTLJSGenerator::GenerateDelegateUtility() {
    WriteLine("// delegate事件委托");
    WriteLine("window.__chtljs_delegate = function(parent, config) {");
    IncreaseIndent();
    
    WriteLine("if (!parent || !config || !config.target) return;");
    WriteLine();
    WriteLine("var registry = window." + GetDelegateRegistryName() + ";");
    WriteLine("var parentKey = parent;");
    WriteLine();
    
    WriteLine("// 获取或创建父元素的委托管理器");
    WriteLine("if (!registry.has(parentKey)) {");
    IncreaseIndent();
    WriteLine("registry.set(parentKey, new Map());");
    DecreaseIndent();
    WriteLine("}");
    WriteLine("var parentDelegates = registry.get(parentKey);");
    WriteLine();
    
    WriteLine("// 处理每个事件类型");
    WriteLine("Object.keys(config).forEach(function(key) {");
    IncreaseIndent();
    WriteLine("if (key === 'target') return;");
    WriteLine();
    WriteLine("var handler = config[key];");
    WriteLine("if (typeof handler !== 'function') return;");
    WriteLine();
    
    WriteLine("// 创建委托处理器");
    WriteLine("var delegateHandler = function(e) {");
    IncreaseIndent();
    WriteLine("var targets = Array.isArray(config.target) ? config.target : [config.target];");
    WriteLine("targets.forEach(function(selector) {");
    IncreaseIndent();
    WriteLine("var target = e.target.closest(selector);");
    WriteLine("if (target && parent.contains(target)) {");
    IncreaseIndent();
    WriteLine("handler.call(target, e);");
    DecreaseIndent();
    WriteLine("}");
    DecreaseIndent();
    WriteLine("});");
    DecreaseIndent();
    WriteLine("};");
    WriteLine();
    
    WriteLine("// 检查是否已经有相同的委托");
    WriteLine("if (!parentDelegates.has(key)) {");
    IncreaseIndent();
    WriteLine("parent.addEventListener(key, delegateHandler);");
    WriteLine("parentDelegates.set(key, [delegateHandler]);");
    DecreaseIndent();
    WriteLine("} else {");
    IncreaseIndent();
    WriteLine("parentDelegates.get(key).push(delegateHandler);");
    DecreaseIndent();
    WriteLine("}");
    
    DecreaseIndent();
    WriteLine("});");
    
    DecreaseIndent();
    WriteLine("};");
}

void CHTLJSGenerator::GenerateAnimateUtility() {
    WriteLine("// animate动画函数");
    WriteLine("window.__chtljs_animate = function(config) {");
    IncreaseIndent();
    
    WriteLine("if (!config || !config.target) return;");
    WriteLine();
    WriteLine("// TODO: 实现动画逻辑");
    WriteLine("console.log('CHTL JS animate:', config);");
    
    DecreaseIndent();
    WriteLine("};");
}

void CHTLJSGenerator::VisitDocument(CHTLJSDocumentNode* node) {
    if (node->IsLocal()) {
        WriteLine("// CHTL JS 局部脚本");
        WriteLine("(function() {");
        IncreaseIndent();
        WriteLine("'use strict';");
        WriteLine();
    }
    
    // 处理子节点
    for (const auto& child : node->GetChildren()) {
        if (child) {
            child->Accept(this);
        }
    }
    
    if (node->IsLocal()) {
        DecreaseIndent();
        WriteLine("})();");
    }
}

void CHTLJSGenerator::VisitScriptBlock(ScriptBlockNode* node) {
    // Script块已经在文档级别处理
    for (const auto& child : node->GetChildren()) {
        if (child) {
            child->Accept(this);
        }
    }
}

void CHTLJSGenerator::VisitEnhancedSelector(EnhancedSelectorNode* node) {
    std::string jsCode = GenerateSelector(node);
    Write(jsCode);
}

std::string CHTLJSGenerator::GenerateSelector(EnhancedSelectorNode* node) {
    const std::string& selector = node->GetSelector();
    m_UsedSelectors.insert(selector);
    
    std::string jsSelector = ConvertSelectorToJS(selector, node->GetSelectorType());
    
    if (node->HasIndex()) {
        return "__chtljs_select(" + QuoteString(selector + "[" + 
               std::to_string(node->GetIndex()) + "]") + ")";
    } else {
        return "__chtljs_select(" + QuoteString(selector) + ")";
    }
}

std::string CHTLJSGenerator::ConvertSelectorToJS(const std::string& selector, 
                                                  EnhancedSelectorNode::SelectorType type) {
    // 选择器已经在__chtljs_select函数中处理
    return selector;
}

void CHTLJSGenerator::VisitArrowOperator(ArrowOperatorNode* node) {
    // 左侧
    if (node->GetLeft()) {
        node->GetLeft()->Accept(this);
    }
    
    // 箭头操作符转换为点
    Write(".");
    
    // 右侧
    if (node->GetRight()) {
        node->GetRight()->Accept(this);
    }
}

void CHTLJSGenerator::VisitListenCall(ListenCallNode* node) {
    GenerateListenFunction(node);
}

void CHTLJSGenerator::GenerateListenFunction(ListenCallNode* node) {
    // 生成目标元素
    if (node->GetTarget()) {
        node->GetTarget()->Accept(this);
    } else {
        Write("null");
    }
    
    Write(", ");
    
    // 生成配置对象
    if (node->GetConfig()) {
        node->GetConfig()->Accept(this);
    } else {
        Write("{}");
    }
    
    Write(")");
}

void CHTLJSGenerator::VisitDelegateCall(DelegateCallNode* node) {
    GenerateDelegateFunction(node);
}

void CHTLJSGenerator::GenerateDelegateFunction(DelegateCallNode* node) {
    Write("__chtljs_delegate(");
    
    // 生成父元素
    if (node->GetParent()) {
        node->GetParent()->Accept(this);
    } else {
        Write("document.body");
    }
    
    Write(", ");
    
    // 生成配置对象
    if (node->GetConfig()) {
        node->GetConfig()->Accept(this);
    } else {
        Write("{}");
    }
    
    Write(")");
}

void CHTLJSGenerator::VisitAnimateCall(AnimateCallNode* node) {
    GenerateAnimateFunction(node);
}

void CHTLJSGenerator::GenerateAnimateFunction(AnimateCallNode* node) {
    Write("__chtljs_animate(");
    
    if (node->GetConfig()) {
        node->GetConfig()->Accept(this);
    } else {
        Write("{}");
    }
    
    Write(")");
}

void CHTLJSGenerator::VisitINeverAwayCall(INeverAwayCallNode* node) {
    // iNeverAway需要特殊处理，因为它涉及到函数重载
    // 这里暂时生成占位代码
    Write("/* iNeverAway - TODO: 需要CJMOD支持 */");
}

void CHTLJSGenerator::VisitPrintMyLoveCall(PrintMyLoveCallNode* node) {
    // printMylove需要特殊处理
    // 这里暂时生成占位代码
    Write("/* printMylove - TODO: 需要CJMOD支持 */");
}

void CHTLJSGenerator::VisitVirDeclaration(VirDeclarationNode* node) {
    ProcessVirDeclaration(node);
}

void CHTLJSGenerator::ProcessVirDeclaration(VirDeclarationNode* node) {
    // 虚对象不生成实际的变量声明
    // 只处理初始化表达式
    WriteLine("// vir " + node->GetVirName());
    
    if (node->GetInitializer()) {
        // 初始化表达式的结果被忽略
        node->GetInitializer()->Accept(this);
        WriteLine(";");
    }
    
    // 为虚对象的每个函数键生成全局函数
    for (const auto& key : node->GetFunctionKeys()) {
        std::string funcName = GenerateVirFunction(node->GetVirName(), key);
        m_VirMappings[node->GetVirName() + "->" + key] = funcName;
    }
}

std::string CHTLJSGenerator::GenerateVirFunction(const std::string& virName, 
                                                  const std::string& functionKey) {
    std::string funcName = GetVirFunctionName(virName, functionKey);
    
    WriteLine("window." + funcName + " = function() {");
    IncreaseIndent();
    WriteLine("// TODO: 虚对象函数实现");
    WriteLine("console.log('vir " + virName + " -> " + functionKey + "');");
    DecreaseIndent();
    WriteLine("};");
    
    m_GeneratedFunctions.insert(funcName);
    return funcName;
}

std::string CHTLJSGenerator::GetVirFunctionName(const std::string& virName, 
                                                 const std::string& functionKey) {
    return "__chtljs_vir_" + virName + "_" + functionKey;
}

void CHTLJSGenerator::VisitVirAccess(VirAccessNode* node) {
    // 虚对象访问转换为全局函数调用
    std::string key = node->GetVirObject() + "->" + node->GetMember();
    
    auto it = m_VirMappings.find(key);
    if (it != m_VirMappings.end()) {
        Write(it->second);
        
        if (node->IsMethodCall()) {
            Write("(");
            if (node->GetArguments()) {
                node->GetArguments()->Accept(this);
            }
            Write(")");
        }
    } else {
        Write("/* 未定义的虚对象访问: " + key + " */");
    }
}

void CHTLJSGenerator::VisitStateTag(StateTagNode* node) {
    // 状态标签在函数名生成时处理
}

void CHTLJSGenerator::VisitObjectLiteral(ObjectLiteralNode* node) {
    Write("{");
    
    if (m_PrettyPrint && !node->GetProperties().empty()) {
        WriteLine();
        IncreaseIndent();
    }
    
    m_InObjectLiteral = true;
    
    bool first = true;
    for (const auto& prop : node->GetProperties()) {
        if (!first) {
            Write(",");
            if (m_PrettyPrint) WriteLine();
        }
        prop->Accept(this);
        first = false;
    }
    
    m_InObjectLiteral = false;
    
    if (m_PrettyPrint && !node->GetProperties().empty()) {
        DecreaseIndent();
        WriteLine();
        WriteIndent();
    }
    
    Write("}");
}

void CHTLJSGenerator::VisitProperty(PropertyNode* node) {
    if (m_PrettyPrint) WriteIndent();
    
    // 属性键
    std::string key = node->GetKey();
    
    // 处理状态标签
    if (node->HasStateTag()) {
        key += ProcessStateTag(node->GetStateTag().get());
    }
    
    Write(QuoteString(key));
    Write(": ");
    
    // 属性值
    if (node->GetValue()) {
        node->GetValue()->Accept(this);
    } else {
        Write("undefined");
    }
}

void CHTLJSGenerator::VisitIdentifier(IdentifierNode* node) {
    Write(node->GetName());
}

void CHTLJSGenerator::VisitLiteral(LiteralNode* node) {
    if (node->GetType() == CHTLJSNodeType::STRING_LITERAL) {
        Write(QuoteString(node->GetValue()));
    } else if (node->GetType() == CHTLJSNodeType::NUMBER_LITERAL) {
        Write(node->GetValue());
    } else if (node->GetType() == CHTLJSNodeType::BOOLEAN_LITERAL) {
        Write(node->GetValue());
    } else if (node->GetType() == CHTLJSNodeType::ARRAY_LITERAL) {
        Write("[");
        auto arr = static_cast<ArrayLiteralNode*>(node);
        bool first = true;
        for (const auto& elem : arr->GetElements()) {
            if (!first) Write(", ");
            elem->Accept(this);
            first = false;
        }
        Write("]");
    }
}

void CHTLJSGenerator::VisitJSCodeFragment(JSCodeFragmentNode* node) {
    // 直接输出JavaScript代码
    Write(node->GetCode());
}

std::string CHTLJSGenerator::ProcessStateTag(StateTagNode* node) {
    return "<" + node->GetStateName() + ">";
}

void CHTLJSGenerator::ReportError(const std::string& message) {
    m_Errors.push_back(message);
    utils::Logger::GetInstance().Error("CHTL JS生成错误: " + message);
}

std::string CHTLJSGenerator::EscapeJS(const std::string& text) {
    std::string escaped;
    for (char c : text) {
        switch (c) {
            case '"': escaped += "\\\""; break;
            case '\'': escaped += "\\'"; break;
            case '\\': escaped += "\\\\"; break;
            case '\n': escaped += "\\n"; break;
            case '\r': escaped += "\\r"; break;
            case '\t': escaped += "\\t"; break;
            default: escaped += c; break;
        }
    }
    return escaped;
}

std::string CHTLJSGenerator::QuoteString(const std::string& text) {
    return "\"" + EscapeJS(text) + "\"";
}

} // namespace generator
} // namespace chtljs
} // namespace chtl