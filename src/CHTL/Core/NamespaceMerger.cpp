#include "CHTL/Core/NamespaceMerger.h"
#include "Utils/ErrorHandler.h"
#include <filesystem>
#include <cctype>
#include <algorithm>

namespace CHTL {
namespace Core {

NamespaceMerger::NamespaceMerger() 
    : allowMerge_(true), strictMode_(false) {
}

void NamespaceMerger::SetConflictResolutionStrategy(bool allowMerge, bool strictMode) {
    allowMerge_ = allowMerge;
    strictMode_ = strictMode;
}

std::shared_ptr<AST::NamespaceNode> NamespaceMerger::CreateDefaultNamespace(const std::string& filePath,
                                                                           const std::vector<NamespaceSymbol>& symbols,
                                                                           bool disableDefault) {
    if (disableDefault) {
        return nullptr; // 禁用默认命名空间功能
    }
    
    // 从文件路径提取默认命名空间名称
    std::string defaultName = ExtractDefaultNamespaceName(filePath);
    
    if (defaultName.empty()) {
        return nullptr;
    }
    
    // 创建默认命名空间节点
    auto namespaceNode = std::make_shared<AST::NamespaceNode>(defaultName);
    
    // 将符号添加到命名空间中（这里简化实现，实际需要根据AST结构添加）
    // 注册默认命名空间
    RegisterNamespace(namespaceNode, filePath);
    
    // 记录符号
    namespaceSymbols_[defaultName] = symbols;
    
    Utils::ErrorHandler::GetInstance().LogInfo(
        "为文件 '" + filePath + "' 创建默认命名空间: " + defaultName
    );
    
    return namespaceNode;
}

std::string NamespaceMerger::ExtractDefaultNamespaceName(const std::string& filePath) {
    if (filePath.empty()) {
        return "";
    }
    
    // 从文件路径中提取文件名（不含扩展名）作为默认命名空间名称
    std::filesystem::path path(filePath);
    std::string filename = path.stem().string();
    
    // 确保命名空间名称符合标识符规范
    std::string namespaceName = "";
    for (char c : filename) {
        if (std::isalnum(c) || c == '_') {
            namespaceName += c;
        } else if (c == '-' || c == '.') {
            namespaceName += '_'; // 将连字符和点替换为下划线
        }
        // 忽略其他特殊字符
    }
    
    // 确保以字母或下划线开头
    if (!namespaceName.empty() && std::isdigit(namespaceName[0])) {
        namespaceName = "_" + namespaceName;
    }
    
    // 如果名称为空或无效，使用默认名称
    if (namespaceName.empty() || namespaceName == "_") {
        namespaceName = "DefaultNamespace";
    }
    
    return namespaceName;
}

void NamespaceMerger::RegisterNamespace(std::shared_ptr<AST::NamespaceNode> namespaceNode, const std::string& sourceFile) {
    if (!namespaceNode) {
        return;
    }
    
    std::string namespaceName = namespaceNode->GetName();
    
    // 注册命名空间
    registeredNamespaces_[namespaceName].push_back(namespaceNode);
    
    // 提取并注册符号
    auto symbols = ExtractSymbols(namespaceNode, sourceFile);
    namespaceSymbols_[namespaceName].insert(namespaceSymbols_[namespaceName].end(), 
                                           symbols.begin(), symbols.end());
    
    Utils::ErrorHandler::GetInstance().LogInfo(
        "注册命名空间: " + namespaceName + " (来源: " + sourceFile + ")"
    );
}

std::vector<std::shared_ptr<AST::NamespaceNode>> NamespaceMerger::GetRegisteredNamespaces(const std::string& namespaceName) {
    auto it = registeredNamespaces_.find(namespaceName);
    if (it != registeredNamespaces_.end()) {
        return it->second;
    }
    return {};
}

void NamespaceMerger::Clear() {
    registeredNamespaces_.clear();
    namespaceSymbols_.clear();
}

std::vector<NamespaceSymbol> NamespaceMerger::ExtractSymbols(std::shared_ptr<AST::NamespaceNode> namespaceNode,
                                                            const std::string& sourceFile) {
    std::vector<NamespaceSymbol> symbols;
    
    if (!namespaceNode) {
        return symbols;
    }
    
    // 递归提取所有子节点的符号
    for (const auto& child : namespaceNode->GetChildren()) {
        ExtractSymbolsFromNode(child, sourceFile, symbols);
    }
    
    return symbols;
}

void NamespaceMerger::ExtractSymbolsFromNode(std::shared_ptr<AST::ASTNode> node,
                                            const std::string& sourceFile,
                                            std::vector<NamespaceSymbol>& symbols) {
    if (!node) {
        return;
    }
    
    NamespaceSymbol symbol;
    symbol.source = sourceFile;
    symbol.type = GetNodeTypeString(node);
    
    // 根据节点类型提取符号名称
    switch (node->GetType()) {
        case AST::NodeType::CUSTOM_ELEMENT: {
            auto customElement = std::dynamic_pointer_cast<AST::CustomElementNode>(node);
            if (customElement) {
                symbol.name = customElement->GetName();
                symbol.type = "@Element";
                symbols.push_back(symbol);
            }
            break;
        }
        
        case AST::NodeType::CUSTOM_STYLE: {
            auto customStyle = std::dynamic_pointer_cast<AST::CustomStyleNode>(node);
            if (customStyle) {
                symbol.name = customStyle->GetName();
                symbol.type = "@Style";
                symbols.push_back(symbol);
            }
            break;
        }
        
        case AST::NodeType::CUSTOM_VAR: {
            auto customVar = std::dynamic_pointer_cast<AST::CustomVarNode>(node);
            if (customVar) {
                symbol.name = customVar->GetName();
                symbol.type = "@Var";
                symbols.push_back(symbol);
            }
            break;
        }
        
        case AST::NodeType::TEMPLATE_ELEMENT: {
            auto templateElement = std::dynamic_pointer_cast<AST::TemplateElementNode>(node);
            if (templateElement) {
                symbol.name = templateElement->GetName();
                symbol.type = "[Template]@Element";
                symbols.push_back(symbol);
            }
            break;
        }
        
        case AST::NodeType::TEMPLATE_STYLE: {
            auto templateStyle = std::dynamic_pointer_cast<AST::TemplateStyleNode>(node);
            if (templateStyle) {
                symbol.name = templateStyle->GetName();
                symbol.type = "[Template]@Style";
                symbols.push_back(symbol);
            }
            break;
        }
        
        case AST::NodeType::TEMPLATE_VAR: {
            auto templateVar = std::dynamic_pointer_cast<AST::TemplateVarNode>(node);
            if (templateVar) {
                symbol.name = templateVar->GetName();
                symbol.type = "[Template]@Var";
                symbols.push_back(symbol);
            }
            break;
        }
        
        case AST::NodeType::IMPORT: {
            auto importNode = std::dynamic_pointer_cast<AST::ImportNode>(node);
            if (importNode && !importNode->GetAlias().empty()) {
                symbol.name = importNode->GetAlias();
                symbol.type = "Import";
                symbol.isImported = true;
                symbols.push_back(symbol);
            }
            break;
        }
        
        default:
            // 对于其他类型，递归处理子节点
            for (const auto& child : node->GetChildren()) {
                ExtractSymbolsFromNode(child, sourceFile, symbols);
            }
            break;
    }
}

std::string NamespaceMerger::GetNodeTypeString(std::shared_ptr<AST::ASTNode> node) {
    if (!node) {
        return "Unknown";
    }
    
    switch (node->GetType()) {
        case AST::NodeType::CUSTOM_ELEMENT: return "@Element";
        case AST::NodeType::CUSTOM_STYLE: return "@Style";
        case AST::NodeType::CUSTOM_VAR: return "@Var";
        case AST::NodeType::TEMPLATE_ELEMENT: return "[Template]@Element";
        case AST::NodeType::TEMPLATE_STYLE: return "[Template]@Style";
        case AST::NodeType::TEMPLATE_VAR: return "[Template]@Var";
        case AST::NodeType::IMPORT: return "Import";
        case AST::NodeType::NAMESPACE: return "Namespace";
        default: return "Other";
    }
}

bool NamespaceMerger::AreSymbolsCompatible(const NamespaceSymbol& symbol1, const NamespaceSymbol& symbol2) {
    // 同名但不同类型的符号不兼容
    if (symbol1.name == symbol2.name && symbol1.type != symbol2.type) {
        return false;
    }
    
    // 来自同一文件的符号通常不兼容（重复定义）
    if (symbol1.source == symbol2.source && symbol1.name == symbol2.name) {
        return false;
    }
    
    // 在严格模式下，任何同名符号都不兼容
    if (strictMode_ && symbol1.name == symbol2.name) {
        return false;
    }
    
    return true;
}

NamespaceMergeResult NamespaceMerger::MergeNamespaces(std::shared_ptr<AST::NamespaceNode> source,
                                                     std::shared_ptr<AST::NamespaceNode> target) {
    NamespaceMergeResult result;
    
    if (!source || !target) {
        result.success = false;
        return result;
    }
    
    if (source->GetName() != target->GetName()) {
        result.success = false;
        return result;
    }
    
    // 检测冲突
    result.conflicts = DetectConflicts(source, target);
    
    // 如果不允许合并或存在冲突，则返回失败
    if (!allowMerge_ || (!result.conflicts.empty() && strictMode_)) {
        result.success = false;
        return result;
    }
    
    // 提取符号
    auto sourceSymbols = ExtractSymbols(source, "source");
    auto targetSymbols = ExtractSymbols(target, "target");
    
    // 合并符号
    auto mergedSymbols = MergeSymbols(sourceSymbols, targetSymbols, result.conflicts);
    
    // 创建合并后的命名空间
    result.mergedNamespace = CreateMergedNamespace(source, target, mergedSymbols);
    result.success = (result.mergedNamespace != nullptr);
    
    return result;
}

std::vector<NamespaceConflict> NamespaceMerger::DetectConflicts(std::shared_ptr<AST::NamespaceNode> source,
                                                               std::shared_ptr<AST::NamespaceNode> target) {
    std::vector<NamespaceConflict> conflicts;
    
    if (!source || !target) {
        return conflicts;
    }
    
    // 提取符号
    auto sourceSymbols = ExtractSymbols(source, "source");
    auto targetSymbols = ExtractSymbols(target, "target");
    
    // 检测符号冲突
    auto symbolConflicts = DetectSymbolConflicts(sourceSymbols, targetSymbols, 
                                                source->GetName(), target->GetName());
    conflicts.insert(conflicts.end(), symbolConflicts.begin(), symbolConflicts.end());
    
    // 检测嵌套命名空间冲突
    auto nestedConflicts = DetectNestedConflicts(source, target);
    conflicts.insert(conflicts.end(), nestedConflicts.begin(), nestedConflicts.end());
    
    return conflicts;
}

std::vector<NamespaceConflict> NamespaceMerger::DetectSymbolConflicts(const std::vector<NamespaceSymbol>& sourceSymbols,
                                                                     const std::vector<NamespaceSymbol>& targetSymbols,
                                                                     const std::string& sourceNamespace,
                                                                     const std::string& targetNamespace) {
    std::vector<NamespaceConflict> conflicts;
    
    for (const auto& sourceSymbol : sourceSymbols) {
        for (const auto& targetSymbol : targetSymbols) {
            if (!AreSymbolsCompatible(sourceSymbol, targetSymbol)) {
                NamespaceConflict conflict;
                conflict.conflictingSymbol = sourceSymbol.name;
                conflict.sourceNamespace = sourceNamespace;
                conflict.targetNamespace = targetNamespace;
                
                if (sourceSymbol.name == targetSymbol.name && sourceSymbol.type != targetSymbol.type) {
                    conflict.type = NamespaceConflictType::TYPE_CONFLICT;
                    conflict.description = "符号 '" + sourceSymbol.name + "' 在两个命名空间中有不同类型: " +
                                         sourceSymbol.type + " vs " + targetSymbol.type;
                } else if (sourceSymbol.name == targetSymbol.name) {
                    conflict.type = NamespaceConflictType::SYMBOL_NAME_CONFLICT;
                    conflict.description = "符号 '" + sourceSymbol.name + "' 在两个命名空间中重复定义";
                }
                
                conflicts.push_back(conflict);
            }
        }
    }
    
    return conflicts;
}

std::vector<NamespaceConflict> NamespaceMerger::DetectNestedConflicts(std::shared_ptr<AST::NamespaceNode> source,
                                                                     std::shared_ptr<AST::NamespaceNode> target) {
    std::vector<NamespaceConflict> conflicts;
    
    // 检查嵌套命名空间是否有冲突
    for (const auto& sourceChild : source->GetChildren()) {
        if (sourceChild->GetType() == AST::NodeType::NAMESPACE) {
            auto sourceNestedNS = std::dynamic_pointer_cast<AST::NamespaceNode>(sourceChild);
            if (!sourceNestedNS) continue;
            
            for (const auto& targetChild : target->GetChildren()) {
                if (targetChild->GetType() == AST::NodeType::NAMESPACE) {
                    auto targetNestedNS = std::dynamic_pointer_cast<AST::NamespaceNode>(targetChild);
                    if (!targetNestedNS) continue;
                    
                    if (sourceNestedNS->GetName() == targetNestedNS->GetName()) {
                        // 递归检测嵌套命名空间冲突
                        auto nestedConflicts = DetectConflicts(sourceNestedNS, targetNestedNS);
                        conflicts.insert(conflicts.end(), nestedConflicts.begin(), nestedConflicts.end());
                    }
                }
            }
        }
    }
    
    return conflicts;
}

std::vector<NamespaceSymbol> NamespaceMerger::MergeSymbols(const std::vector<NamespaceSymbol>& sourceSymbols,
                                                          const std::vector<NamespaceSymbol>& targetSymbols,
                                                          const std::vector<NamespaceConflict>& conflicts) {
    std::vector<NamespaceSymbol> mergedSymbols;
    
    // 添加所有源符号
    mergedSymbols.insert(mergedSymbols.end(), sourceSymbols.begin(), sourceSymbols.end());
    
    // 添加目标符号，但跳过冲突的符号
    for (const auto& targetSymbol : targetSymbols) {
        bool hasConflict = false;
        
        for (const auto& conflict : conflicts) {
            if (conflict.conflictingSymbol == targetSymbol.name) {
                hasConflict = true;
                break;
            }
        }
        
        if (!hasConflict) {
            mergedSymbols.push_back(targetSymbol);
        }
    }
    
    return mergedSymbols;
}

std::shared_ptr<AST::NamespaceNode> NamespaceMerger::CreateMergedNamespace(std::shared_ptr<AST::NamespaceNode> source,
                                                                          std::shared_ptr<AST::NamespaceNode> target,
                                                                          const std::vector<NamespaceSymbol>& mergedSymbols) {
    if (!source || !target) {
        return nullptr;
    }
    
    // 创建新的命名空间节点
    auto mergedNamespace = std::make_shared<AST::NamespaceNode>(source->GetName(), source->GetToken());
    
    // 首先添加源命名空间的所有子节点
    for (const auto& child : source->GetChildren()) {
        mergedNamespace->AddChild(child->Clone());
    }
    
    // 然后添加目标命名空间的非冲突子节点
    for (const auto& child : target->GetChildren()) {
        bool shouldAdd = true;
        
        // 检查是否有同名的符号已经存在
        if (child->GetType() == AST::NodeType::CUSTOM_ELEMENT ||
            child->GetType() == AST::NodeType::CUSTOM_STYLE ||
            child->GetType() == AST::NodeType::CUSTOM_VAR ||
            child->GetType() == AST::NodeType::TEMPLATE_ELEMENT ||
            child->GetType() == AST::NodeType::TEMPLATE_STYLE ||
            child->GetType() == AST::NodeType::TEMPLATE_VAR) {
            
            std::string childName = GetNodeName(child);
            for (const auto& existingChild : mergedNamespace->GetChildren()) {
                if (GetNodeName(existingChild) == childName &&
                    existingChild->GetType() == child->GetType()) {
                    shouldAdd = false;
                    break;
                }
            }
        }
        
        if (shouldAdd) {
            mergedNamespace->AddChild(child->Clone());
        }
    }
    
    return mergedNamespace;
}

std::string NamespaceMerger::GetNodeName(std::shared_ptr<AST::ASTNode> node) {
    if (!node) return "";
    
    switch (node->GetType()) {
        case AST::NodeType::CUSTOM_ELEMENT: {
            auto customElement = std::dynamic_pointer_cast<AST::CustomElementNode>(node);
            return customElement ? customElement->GetName() : "";
        }
        case AST::NodeType::CUSTOM_STYLE: {
            auto customStyle = std::dynamic_pointer_cast<AST::CustomStyleNode>(node);
            return customStyle ? customStyle->GetName() : "";
        }
        case AST::NodeType::CUSTOM_VAR: {
            auto customVar = std::dynamic_pointer_cast<AST::CustomVarNode>(node);
            return customVar ? customVar->GetName() : "";
        }
        case AST::NodeType::TEMPLATE_ELEMENT: {
            auto templateElement = std::dynamic_pointer_cast<AST::TemplateElementNode>(node);
            return templateElement ? templateElement->GetName() : "";
        }
        case AST::NodeType::TEMPLATE_STYLE: {
            auto templateStyle = std::dynamic_pointer_cast<AST::TemplateStyleNode>(node);
            return templateStyle ? templateStyle->GetName() : "";
        }
        case AST::NodeType::TEMPLATE_VAR: {
            auto templateVar = std::dynamic_pointer_cast<AST::TemplateVarNode>(node);
            return templateVar ? templateVar->GetName() : "";
        }
        case AST::NodeType::NAMESPACE: {
            auto namespaceNode = std::dynamic_pointer_cast<AST::NamespaceNode>(node);
            return namespaceNode ? namespaceNode->GetName() : "";
        }
        default:
            return "";
    }
}

} // namespace Core
} // namespace CHTL