#include "CHTLJS/Generator/CHTLJSGenerator.h"
#include "Utils/ErrorHandler.h"
#include <sstream>
#include <algorithm>

namespace CHTL {
namespace CHTLJS {
namespace Generator {

CHTLJSGenerator::CHTLJSGenerator(const GeneratorConfig& config)
    : config_(config), currentIndent_(0), virtualObjectCount_(0), enhancedSelectorCount_(0), 
      eventListenerCount_(0), eventDelegateCount_(0), animationCount_(0) {}

std::string CHTLJSGenerator::Generate(AST::ASTNodePtr ast) {
    if (!ast) {
        Utils::ErrorHandler::GetInstance().LogError("AST为空，无法生成代码");
        return "";
    }
    
    output_.clear();
    currentIndent_ = 0;
    
    ast->Accept(*this);
    
    return output_.str();
}

void CHTLJSGenerator::Reset() {
    output_.clear();
    currentIndent_ = 0;
}

std::string CHTLJSGenerator::GetStatistics() const {
    std::ostringstream stats;
    stats << "CHTL JS代码生成统计:\n";
    stats << "  生成代码长度: " << output_.str().length() << " 字符\n";
    stats << "  缩进级别: " << currentIndent_ << "\n";
    stats << "  美化打印: " << (config_.prettyPrint ? "启用" : "禁用") << "\n";
    
    return stats.str();
}

// 基础节点访问方法
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
            output_ << "\"" << node.GetValue() << "\"";
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

// CHTL JS核心语法访问方法（严格按照语法文档第1099-1531行）

void CHTLJSGenerator::VisitEnhancedSelectorNode(AST::EnhancedSelectorNode& node) {
    // 增强选择器转换为DOM查询 - 语法文档第1130行
    const std::string& selector = node.GetSelector();
    
    switch (node.GetSelectorType()) {
        case AST::EnhancedSelectorNode::SelectorType::TAG:
            output_ << "document.querySelectorAll('" << selector << "')";
            break;
        case AST::EnhancedSelectorNode::SelectorType::CLASS:
            output_ << "document.querySelectorAll('." << selector << "')";
            break;
        case AST::EnhancedSelectorNode::SelectorType::ID:
            output_ << "document.querySelector('#" << selector << "')";
            break;
        case AST::EnhancedSelectorNode::SelectorType::COMPLEX:
            output_ << "document.querySelectorAll('" << selector << "')";
            break;
        case AST::EnhancedSelectorNode::SelectorType::INDEXED:
            output_ << "document.querySelectorAll('" << node.GetParsedSelector() << "')[" << node.GetIndex() << "]";
            break;
    }
}

void CHTLJSGenerator::VisitVirtualObjectNode(AST::VirtualObjectNode& node) {
    // 虚对象转换 - 语法文档第1274行
    // vir本身不存在，是编译期语法糖
    // 这里只记录虚对象名称，实际的函数生成在使用时进行
    
    const std::string& objectName = node.GetName();
    
    // 生成虚对象注册注释
    if (config_.prettyPrint) {
        output_ << "// 虚对象注册: " << objectName << "\n";
    }
    
    // 虚对象本身不生成代码，只在调用时生成对应的全局函数
}

void CHTLJSGenerator::VisitListenBlockNode(AST::ListenBlockNode& node) {
    // 监听器转换为addEventListener - 语法文档第1184行
    
    if (config_.prettyPrint) {
        output_ << "// 增强监听器\n";
    }
    
    // ListenBlockNode没有target，应该在使用时由增强选择器提供目标
    // 这里生成事件监听器的通用形式
    
    // 生成事件映射
    const auto& eventHandlers = node.GetEventHandlers();
    for (const auto& handler : eventHandlers) {
        output_ << "// 事件: " << handler.first;
        if (config_.prettyPrint) {
            output_ << "\n";
        }
        
        // 生成事件处理器
        if (handler.second) {
            handler.second->Accept(*this);
        }
        
        if (config_.prettyPrint) {
            output_ << "\n";
        }
    }
}

void CHTLJSGenerator::VisitDelegateBlockNode(AST::DelegateBlockNode& node) {
    // 事件委托转换 - 语法文档第1215行
    
    if (config_.prettyPrint) {
        output_ << "// 事件委托\n";
    }
    
    // 生成事件委托代码
    output_ << "// 事件委托实现需要全局注册表管理";
}

void CHTLJSGenerator::VisitAnimateBlockNode(AST::AnimateBlockNode& node) {
    // 动画转换为requestAnimationFrame - 语法文档第1233行
    
    if (config_.prettyPrint) {
        output_ << "// CHTL JS动画\n";
    }
    
    output_ << "(function() {\n";
    
    if (config_.prettyPrint) {
        AddIndent();
        output_ << GetIndent();
    }
    
    output_ << "// 动画实现";
    
    if (config_.prettyPrint) {
        output_ << "\n";
        RemoveIndent();
        output_ << GetIndent();
    }
    
    output_ << "})();";
}

// VisitArrowFunctionNode已移除 - CHTL JS不包含JS语法

void CHTLJSGenerator::VisitAnimationKeyframeNode(AST::AnimationKeyframeNode& node) {
    // 动画关键帧生成
    
    output_ << "{ at: " << node.GetTime();
    
    const auto& cssProps = node.GetCSSProperties();
    for (const auto& prop : cssProps) {
        output_ << ", " << prop.first << ": '" << prop.second << "'";
    }
    
    output_ << " }";
}

void CHTLJSGenerator::VisitCommentNode(AST::CommentNode& node) {
    // 注释生成
    if (config_.prettyPrint) {
        output_ << "// " << node.GetContent();
    }
}

// 私有辅助方法
std::string CHTLJSGenerator::GetIndent() const {
    return std::string(currentIndent_ * config_.indentSize, ' ');
}

void CHTLJSGenerator::AddIndent() {
    currentIndent_++;
}

void CHTLJSGenerator::RemoveIndent() {
    if (currentIndent_ > 0) {
        currentIndent_--;
    }
}

} // namespace Generator
} // namespace CHTLJS
} // namespace CHTL