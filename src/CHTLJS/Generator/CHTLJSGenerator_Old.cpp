#include "CHTLJS/Generator/CHTLJSGenerator.h"
#include "Utils/StringUtils.h"
#include "Utils/ErrorHandler.h"
#include <algorithm>
#include <regex>

namespace CHTL {
namespace CHTLJS {
namespace Generator {

CHTLJSGenerator::CHTLJSGenerator(const GeneratorConfig& config)
    : config_(config), currentIndent_(0), virtualObjectCount_(0), enhancedSelectorCount_(0),
      eventListenerCount_(0), eventDelegateCount_(0), animationCount_(0), iNeverAwayCount_(0) {}

std::string CHTLJSGenerator::Generate(AST::ASTNodePtr ast) {
    if (!ast) {
        return "";
    }
    
    // 重置状态
    Reset();
    
    // 遍历AST生成代码
    ast->Accept(*this);
    
    // 组合最终输出
    std::ostringstream finalOutput;
    
    // 添加全局代码
    if (!globalCode_.str().empty()) {
        finalOutput << "// CHTL JS全局函数和变量\n";
        finalOutput << globalCode_.str();
        finalOutput << "\n";
    }
    
    // 添加初始化代码
    if (!initCode_.str().empty()) {
        finalOutput << "// CHTL JS初始化代码\n";
        finalOutput << "(function() {\n";
        finalOutput << initCode_.str();
        finalOutput << "})();\n\n";
    }
    
    // 添加主代码
    finalOutput << "// CHTL JS主代码\n";
    finalOutput << output_.str();
    
    return finalOutput.str();
}

void CHTLJSGenerator::Reset() {
    output_.str("");
    output_.clear();
    globalCode_.str("");
    globalCode_.clear();
    initCode_.str("");
    initCode_.clear();
    
    context_ = GenerationContext();
    currentIndent_ = 0;
    
    virtualObjectCount_ = 0;
    enhancedSelectorCount_ = 0;
    eventListenerCount_ = 0;
    eventDelegateCount_ = 0;
    animationCount_ = 0;
    iNeverAwayCount_ = 0;
}

std::string CHTLJSGenerator::GetStatistics() const {
    std::ostringstream oss;
    oss << "CHTL JS生成统计:\n";
    oss << "  虚对象数量: " << virtualObjectCount_ << "\n";
    oss << "  增强选择器数量: " << enhancedSelectorCount_ << "\n";
    oss << "  事件监听器数量: " << eventListenerCount_ << "\n";
    oss << "  事件委托数量: " << eventDelegateCount_ << "\n";
    oss << "  动画数量: " << animationCount_ << "\n";
    oss << "  iNeverAway函数数量: " << iNeverAwayCount_ << "\n";
    oss << "  生成的全局函数数量: " << context_.globalFunctions.size() << "\n";
    return oss.str();
}

void CHTLJSGenerator::VisitRootNode(AST::RootNode& node) {
    for (const auto& child : node.GetChildren()) {
        child->Accept(*this);
        if (config_.prettyPrint) {
            output_ << "\n";
        }
    }
}

void CHTLJSGenerator::VisitIdentifierNode(AST::IdentifierNode& node) {
    output_ << node.GetName();
}

void CHTLJSGenerator::VisitLiteralNode(AST::LiteralNode& node) {
    switch (node.GetLiteralType()) {
        case AST::LiteralNode::LiteralType::STRING:
            output_ << "\"" << Utils::StringUtils::EscapeJavaScript(node.GetValue()) << "\"";
            break;
        case AST::LiteralNode::LiteralType::NUMBER:
            output_ << node.GetValue();
            break;
        case AST::LiteralNode::LiteralType::BOOLEAN:
            output_ << node.GetValue();
            break;
        case AST::LiteralNode::LiteralType::NULL_LITERAL:
            output_ << "null";
            break;
    }
}

void CHTLJSGenerator::VisitEnhancedSelectorNode(AST::EnhancedSelectorNode& node) {
    enhancedSelectorCount_++;
    
    // 生成DOM查询代码
    std::string domQuery = GenerateDOMQuery(node);
    output_ << domQuery;
    
    // 缓存选择器
    context_.usedSelectors.insert(node.GetSelector());
}

void CHTLJSGenerator::VisitVirtualObjectNode(AST::VirtualObjectNode& node) {
    virtualObjectCount_++;
    
    // 生成虚对象的全局函数
    GenerateVirtualObjectFunctions(node);
    
    // 记录虚对象
    context_.virtualObjects[node.GetName()] = "defined";
}

void CHTLJSGenerator::VisitListenBlockNode(AST::ListenBlockNode& node) {
    eventListenerCount_++;
    
    // 生成事件监听器代码
    std::string listenerCode = GenerateEventListener(node);
    output_ << listenerCode;
}

void CHTLJSGenerator::VisitDelegateBlockNode(AST::DelegateBlockNode& node) {
    eventDelegateCount_++;
    
    // 生成事件委托代码
    std::string delegateCode = GenerateEventDelegate(node);
    output_ << delegateCode;
    
    // 添加到委托注册表
    context_.eventDelegates.push_back(delegateCode);
}

void CHTLJSGenerator::VisitAnimateBlockNode(AST::AnimateBlockNode& node) {
    animationCount_++;
    
    // 生成动画代码
    std::string animationCode = GenerateAnimation(node);
    output_ << animationCode;
    
    // 记录动画
    context_.animations.push_back(animationCode);
}

void CHTLJSGenerator::VisitINeverAwayBlockNode(AST::INeverAwayBlockNode& node) {
    iNeverAwayCount_++;
    
    // 生成iNeverAway函数
    GenerateINeverAwayFunctions(node);
}

void CHTLJSGenerator::VisitMethodCallNode(AST::MethodCallNode& node) {
    // 生成对象
    node.GetObject()->Accept(*this);
    
    // 生成方法调用
    output_ << "." << node.GetMethod() << "(";
    
    // 生成参数
    const auto& args = node.GetArguments();
    for (size_t i = 0; i < args.size(); ++i) {
        if (i > 0) output_ << ", ";
        args[i]->Accept(*this);
    }
    
    output_ << ")";
}

void CHTLJSGenerator::VisitVirtualMethodCallNode(AST::VirtualMethodCallNode& node) {
    // 生成虚对象方法调用
    std::string functionName;
    
    if (node.IsVoidStateCall()) {
        // Void<State>调用
        functionName = GenerateGlobalFunctionName(node.GetObjectName() + "_Void_" + node.GetVoidState());
    } else {
        // 普通方法调用
        functionName = GenerateGlobalFunctionName(node.GetObjectName() + "_" + node.GetMethod());
    }
    
    output_ << functionName << "(";
    
    // 生成参数
    const auto& args = node.GetArguments();
    for (size_t i = 0; i < args.size(); ++i) {
        if (i > 0) output_ << ", ";
        args[i]->Accept(*this);
    }
    
    output_ << ")";
}

void CHTLJSGenerator::VisitArrowFunctionNode(AST::ArrowFunctionNode& node) {
    output_ << "(";
    
    // 生成参数
    const auto& params = node.GetParameters();
    for (size_t i = 0; i < params.size(); ++i) {
        if (i > 0) output_ << ", ";
        output_ << params[i];
    }
    
    output_ << ") => ";
    
    // 生成函数体
    if (node.GetBody()) {
        node.GetBody()->Accept(*this);
    }
}

// VisitObjectLiteralNode已移除 - 对象字面量是JavaScript语法，不属于CHTL JS
// 语法文档第1100行明确说明"CHTL JS不支持JS的语法"

void CHTLJSGenerator::VisitArrayLiteralNode(AST::ArrayLiteralNode& node) {
    output_ << "[";
    
    const auto& elements = node.GetElements();
    for (size_t i = 0; i < elements.size(); ++i) {
        if (i > 0) output_ << ", ";
        elements[i]->Accept(*this);
    }
    
    output_ << "]";
}

void CHTLJSGenerator::VisitVariableDeclarationNode(AST::VariableDeclarationNode& node) {
    switch (node.GetDeclarationType()) {
        case AST::VariableDeclarationNode::DeclarationType::CONST:
            output_ << "const ";
            break;
        case AST::VariableDeclarationNode::DeclarationType::LET:
            output_ << "let ";
            break;
        case AST::VariableDeclarationNode::DeclarationType::VAR:
            output_ << "var ";
            break;
    }
    
    output_ << node.GetName();
    
    if (node.GetInitializer()) {
        output_ << " = ";
        node.GetInitializer()->Accept(*this);
    }
    
    output_ << ";";
}

void CHTLJSGenerator::VisitAssignmentExpressionNode(AST::AssignmentExpressionNode& node) {
    node.GetLeft()->Accept(*this);
    output_ << " = ";
    node.GetRight()->Accept(*this);
}

void CHTLJSGenerator::VisitCommentNode(AST::CommentNode& node) {
    if (config_.includeComments) {
        output_ << node.GetContent();
        if (config_.prettyPrint) {
            output_ << "\n";
        }
    }
}

void CHTLJSGenerator::VisitFunctionDefinitionNode(AST::FunctionDefinitionNode& node) {
    output_ << "function " << node.GetName() << "(";
    
    const auto& params = node.GetParameters();
    for (size_t i = 0; i < params.size(); ++i) {
        if (i > 0) output_ << ", ";
        output_ << params[i];
    }
    
    output_ << ") ";
    
    if (node.GetBody()) {
        node.GetBody()->Accept(*this);
    }
}

void CHTLJSGenerator::VisitAnimationKeyframeNode(AST::AnimationKeyframeNode& node) {
    output_ << "{\n";
    output_ << "  at: " << node.GetTime() << ",\n";
    
    for (const auto& prop : node.GetCSSProperties()) {
        output_ << "  " << prop.first << ": '" << prop.second << "',\n";
    }
    
    output_ << "}";
}

void CHTLJSGenerator::VisitPropertyAccessNode(AST::PropertyAccessNode& node) {
    node.GetObject()->Accept(*this);
    output_ << "." << node.GetProperty();
}

std::string CHTLJSGenerator::GenerateDOMQuery(AST::EnhancedSelectorNode& selector) {
    std::string query;
    
    switch (selector.GetSelectorType()) {
        case AST::EnhancedSelectorNode::SelectorType::TAG:
            // {{button}} -> document.querySelectorAll('button')
            query = "document.querySelectorAll('" + selector.GetParsedSelector() + "')";
            break;
            
        case AST::EnhancedSelectorNode::SelectorType::CLASS:
            // {{.box}} -> document.querySelectorAll('.box')
            query = "document.querySelectorAll('." + selector.GetParsedSelector() + "')";
            break;
            
        case AST::EnhancedSelectorNode::SelectorType::ID:
            // {{#box}} -> document.getElementById('box')
            query = "document.getElementById('" + selector.GetParsedSelector() + "')";
            break;
            
        case AST::EnhancedSelectorNode::SelectorType::COMPLEX:
            // {{.box button}} -> document.querySelectorAll('.box button')
            query = "document.querySelectorAll('" + selector.GetParsedSelector() + "')";
            break;
            
        case AST::EnhancedSelectorNode::SelectorType::INDEXED:
            // {{button[0]}} -> document.querySelectorAll('button')[0]
            query = "document.querySelectorAll('" + selector.GetParsedSelector() + "')[" + 
                   std::to_string(selector.GetIndex()) + "]";
            break;
    }
    
    return query;
}

void CHTLJSGenerator::GenerateVirtualObjectFunctions(AST::VirtualObjectNode& virtualObject) {
    if (!virtualObject.GetAssignment()) {
        return;
    }
    
    // 检查赋值是否为listen或iNeverAway
    auto assignment = virtualObject.GetAssignment();
    
    if (auto listenBlock = std::dynamic_pointer_cast<AST::ListenBlockNode>(assignment)) {
        // 为listen块中的每个事件处理器生成全局函数
        for (const auto& handler : listenBlock->GetEventHandlers()) {
            std::string functionName = GenerateGlobalFunctionName(
                virtualObject.GetName() + "_" + handler.first);
            
            globalCode_ << "function " << functionName << "() {\n";
            
            // 生成函数体（简化实现）
            if (auto arrowFunc = std::dynamic_pointer_cast<AST::ArrowFunctionNode>(handler.second)) {
                globalCode_ << "  return (";
                // 这里需要生成箭头函数的参数和体
                globalCode_ << ") => {\n";
                globalCode_ << "    // 事件处理逻辑\n";
                globalCode_ << "  };\n";
            }
            
            globalCode_ << "}\n\n";
            
            // 记录全局函数
            context_.globalFunctions[virtualObject.GetName() + "." + handler.first] = functionName;
        }
    } else if (auto iNeverAwayBlock = std::dynamic_pointer_cast<AST::INeverAwayBlockNode>(assignment)) {
        // 为iNeverAway块生成全局函数
        GenerateINeverAwayFunctions(*iNeverAwayBlock);
    }
}

std::string CHTLJSGenerator::GenerateEventListener(AST::ListenBlockNode& listenBlock) {
    std::ostringstream code;
    
    // 生成addEventListener调用
    code << ".forEach(element => {\n";
    
    for (const auto& handler : listenBlock.GetEventHandlers()) {
        code << "  element.addEventListener('" << handler.first << "', ";
        
        // 生成事件处理器
        if (auto arrowFunc = std::dynamic_pointer_cast<AST::ArrowFunctionNode>(handler.second)) {
            arrowFunc->Accept(*this);
            code << output_.str();
            output_.str(""); // 清空临时输出
        } else {
            handler.second->Accept(*this);
            code << output_.str();
            output_.str(""); // 清空临时输出
        }
        
        code << ");\n";
    }
    
    code << "})";
    
    return code.str();
}

std::string CHTLJSGenerator::GenerateEventDelegate(AST::DelegateBlockNode& delegateBlock) {
    std::ostringstream code;
    
    // 生成事件委托代码（语法文档第1215行）
    code << ".addEventListener('click', function(event) {\n";
    code << "  const target = event.target;\n";
    
    // 生成目标检查
    if (delegateBlock.GetTarget()) {
        code << "  if (";
        
        // 检查目标是否匹配
        if (auto arrayTarget = std::dynamic_pointer_cast<AST::ArrayLiteralNode>(delegateBlock.GetTarget())) {
            // 多个目标选择器
            const auto& elements = arrayTarget->GetElements();
            for (size_t i = 0; i < elements.size(); ++i) {
                if (i > 0) code << " || ";
                
                if (auto selectorNode = std::dynamic_pointer_cast<AST::EnhancedSelectorNode>(elements[i])) {
                    std::string selector = selectorNode->GetParsedSelector();
                    if (selectorNode->GetSelectorType() == AST::EnhancedSelectorNode::SelectorType::CLASS) {
                        code << "target.classList.contains('" << selector << "')";
                    } else if (selectorNode->GetSelectorType() == AST::EnhancedSelectorNode::SelectorType::ID) {
                        code << "target.id === '" << selector << "'";
                    } else {
                        code << "target.tagName.toLowerCase() === '" << selector << "'";
                    }
                }
            }
        } else if (auto selectorNode = std::dynamic_pointer_cast<AST::EnhancedSelectorNode>(delegateBlock.GetTarget())) {
            // 单个目标选择器
            std::string selector = selectorNode->GetParsedSelector();
            if (selectorNode->GetSelectorType() == AST::EnhancedSelectorNode::SelectorType::CLASS) {
                code << "target.classList.contains('" << selector << "')";
            } else if (selectorNode->GetSelectorType() == AST::EnhancedSelectorNode::SelectorType::ID) {
                code << "target.id === '" << selector << "'";
            } else {
                code << "target.tagName.toLowerCase() === '" << selector << "'";
            }
        }
        
        code << ") {\n";
        
        // 生成事件处理器
        for (const auto& handler : delegateBlock.GetEventHandlers()) {
            if (handler.first == "click") {
                code << "    ";
                handler.second->Accept(*this);
                code << output_.str();
                output_.str(""); // 清空临时输出
                code << "(event);\n";
            }
        }
        
        code << "  }\n";
    }
    
    code << "})";
    
    return code.str();
}

std::string CHTLJSGenerator::GenerateAnimation(AST::AnimateBlockNode& animateBlock) {
    std::ostringstream code;
    
    // 生成动画函数（语法文档第1233行）
    std::string animFuncName = GenerateGlobalFunctionName("animate_" + std::to_string(animationCount_));
    
    globalCode_ << "function " << animFuncName << "(target) {\n";
    globalCode_ << "  const duration = " << animateBlock.GetDuration() << ";\n";
    globalCode_ << "  const easing = '" << animateBlock.GetEasing() << "';\n";
    globalCode_ << "  let startTime = null;\n";
    globalCode_ << "  \n";
    globalCode_ << "  function animate(timestamp) {\n";
    globalCode_ << "    if (!startTime) startTime = timestamp;\n";
    globalCode_ << "    const progress = Math.min((timestamp - startTime) / duration, 1);\n";
    globalCode_ << "    \n";
    
    // 生成关键帧插值
    if (!animateBlock.GetKeyframes().empty()) {
        globalCode_ << "    // 关键帧插值\n";
        for (const auto& keyframe : animateBlock.GetKeyframes()) {
            if (auto kfNode = std::dynamic_pointer_cast<AST::AnimationKeyframeNode>(keyframe)) {
                globalCode_ << "    if (progress >= " << kfNode->GetTime() << ") {\n";
                for (const auto& prop : kfNode->GetCSSProperties()) {
                    globalCode_ << "      target.style." << prop.first << " = '" << prop.second << "';\n";
                }
                globalCode_ << "    }\n";
            }
        }
    }
    
    globalCode_ << "    \n";
    globalCode_ << "    if (progress < 1) {\n";
    globalCode_ << "      requestAnimationFrame(animate);\n";
    globalCode_ << "    } else {\n";
    
    // 生成回调
    if (animateBlock.GetCallback()) {
        globalCode_ << "      ";
        // 这里需要生成回调函数调用
        globalCode_ << "// 回调函数调用\n";
    }
    
    globalCode_ << "    }\n";
    globalCode_ << "  }\n";
    globalCode_ << "  \n";
    globalCode_ << "  requestAnimationFrame(animate);\n";
    globalCode_ << "}\n\n";
    
    // 生成调用代码
    code << animFuncName << "(";
    if (animateBlock.GetTarget()) {
        animateBlock.GetTarget()->Accept(*this);
        code << output_.str();
        output_.str(""); // 清空临时输出
    }
    code << ")";
    
    return code.str();
}

void CHTLJSGenerator::GenerateINeverAwayFunctions(AST::INeverAwayBlockNode& iNeverAwayBlock) {
    // 为每个Void<State>方法生成全局函数
    for (const auto& voidMethod : iNeverAwayBlock.GetVoidMethods()) {
        std::string functionName = GenerateGlobalFunctionName("iNeverAway_Void_" + voidMethod.first);
        
        globalCode_ << "function " << functionName << "() {\n";
        globalCode_ << "  // Void<" << voidMethod.first << "> 实现\n";
        globalCode_ << "  return ";
        
        // 生成方法体（简化实现）
        if (auto funcDef = std::dynamic_pointer_cast<AST::FunctionDefinitionNode>(voidMethod.second)) {
            globalCode_ << "function(";
            const auto& params = funcDef->GetParameters();
            for (size_t i = 0; i < params.size(); ++i) {
                if (i > 0) globalCode_ << ", ";
                globalCode_ << params[i];
            }
            globalCode_ << ") {\n";
            globalCode_ << "    // 函数体实现\n";
            globalCode_ << "  };\n";
        }
        
        globalCode_ << "}\n\n";
    }
    
    // 为自定义方法生成全局函数
    for (const auto& customMethod : iNeverAwayBlock.GetCustomMethods()) {
        std::string functionName = GenerateGlobalFunctionName("iNeverAway_" + customMethod.first);
        
        globalCode_ << "function " << functionName << "() {\n";
        globalCode_ << "  // 自定义方法 " << customMethod.first << " 实现\n";
        globalCode_ << "  return ";
        
        // 生成方法体
        customMethod.second->Accept(*this);
        globalCode_ << output_.str();
        output_.str(""); // 清空临时输出
        
        globalCode_ << ";\n";
        globalCode_ << "}\n\n";
    }
}

std::string CHTLJSGenerator::GenerateGlobalFunctionName(const std::string& baseName) {
    std::string cleanName = Utils::StringUtils::ReplaceAll(baseName, "<", "_");
    cleanName = Utils::StringUtils::ReplaceAll(cleanName, ">", "_");
    cleanName = Utils::StringUtils::ReplaceAll(cleanName, " ", "_");
    
    return "CHTLJS_" + cleanName + "_" + std::to_string(context_.functionCounter++);
}

std::string CHTLJSGenerator::GenerateSelectorCacheName(const std::string& selector) {
    std::string cleanSelector = Utils::StringUtils::ReplaceAll(selector, "{{", "");
    cleanSelector = Utils::StringUtils::ReplaceAll(cleanSelector, "}}", "");
    cleanSelector = Utils::StringUtils::ReplaceAll(cleanSelector, ".", "_");
    cleanSelector = Utils::StringUtils::ReplaceAll(cleanSelector, "#", "_");
    cleanSelector = Utils::StringUtils::ReplaceAll(cleanSelector, " ", "_");
    
    return "CHTLJS_selector_" + cleanSelector + "_" + std::to_string(context_.selectorCounter++);
}

void CHTLJSGenerator::AddIndent() {
    currentIndent_++;
}

void CHTLJSGenerator::RemoveIndent() {
    if (currentIndent_ > 0) {
        currentIndent_--;
    }
}

std::string CHTLJSGenerator::GetIndent() const {
    std::string indent = "";
    for (int i = 0; i < currentIndent_ * config_.indentSize; ++i) {
        indent += config_.indentChar;
    }
    return indent;
}

void CHTLJSGenerator::CollectGlobalCode(const std::string& code) {
    globalCode_ << code;
}

void CHTLJSGenerator::CollectInitCode(const std::string& code) {
    initCode_ << code;
}

} // namespace Generator
} // namespace CHTLJS
} // namespace CHTL
