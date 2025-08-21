#include "chtl/namespace/NamespaceManager.h"
#include "chtl/ast/NamespaceNode.h"
#include "utils/Logger.h"
#include "utils/StringUtils.h"
#include <sstream>
#include <algorithm>

namespace chtl {
namespace ns {

// Namespace 实现

Namespace::Namespace(const std::string& name, Namespace* parent)
    : m_Name(name)
    , m_Parent(parent) {}

Namespace::~Namespace() = default;

std::string Namespace::GetFullName() const {
    if (!m_Parent || m_Parent->GetName().empty()) {
        return m_Name;
    }
    
    std::string parentName = m_Parent->GetFullName();
    if (parentName.empty()) {
        return m_Name;
    }
    
    return parentName + "::" + m_Name;
}

bool Namespace::AddSymbol(const SymbolInfo& symbol) {
    if (HasConflict(symbol)) {
        return false;
    }
    
    m_Symbols[symbol.name] = symbol;
    return true;
}

SymbolInfo* Namespace::FindSymbol(const std::string& name, SymbolType type) {
    auto it = m_Symbols.find(name);
    if (it != m_Symbols.end()) {
        if (type == SymbolType::NAMESPACE || it->second.type == type) {
            return &it->second;
        }
    }
    return nullptr;
}

const SymbolInfo* Namespace::FindSymbol(const std::string& name, SymbolType type) const {
    auto it = m_Symbols.find(name);
    if (it != m_Symbols.end()) {
        if (type == SymbolType::NAMESPACE || it->second.type == type) {
            return &it->second;
        }
    }
    return nullptr;
}

std::vector<SymbolInfo> Namespace::GetAllSymbols() const {
    std::vector<SymbolInfo> result;
    for (const auto& pair : m_Symbols) {
        result.push_back(pair.second);
    }
    return result;
}

std::vector<SymbolInfo> Namespace::GetSymbolsByType(SymbolType type) const {
    std::vector<SymbolInfo> result;
    for (const auto& pair : m_Symbols) {
        if (pair.second.type == type) {
            result.push_back(pair.second);
        }
    }
    return result;
}

Namespace* Namespace::CreateOrGetChildNamespace(const std::string& name) {
    auto it = m_Children.find(name);
    if (it != m_Children.end()) {
        return it->second.get();
    }
    
    auto child = std::make_unique<Namespace>(name, this);
    Namespace* ptr = child.get();
    m_Children[name] = std::move(child);
    return ptr;
}

Namespace* Namespace::GetChildNamespace(const std::string& name) {
    auto it = m_Children.find(name);
    if (it != m_Children.end()) {
        return it->second.get();
    }
    return nullptr;
}

const Namespace* Namespace::GetChildNamespace(const std::string& name) const {
    auto it = m_Children.find(name);
    if (it != m_Children.end()) {
        return it->second.get();
    }
    return nullptr;
}

std::vector<Namespace*> Namespace::GetChildNamespaces() const {
    std::vector<Namespace*> result;
    for (const auto& pair : m_Children) {
        result.push_back(pair.second.get());
    }
    return result;
}

bool Namespace::Merge(const Namespace& other, std::vector<SymbolInfo>& conflicts) {
    bool success = true;
    
    // 合并符号
    for (const auto& pair : other.m_Symbols) {
        const SymbolInfo& symbol = pair.second;
        
        if (HasConflict(symbol)) {
            conflicts.push_back(symbol);
            success = false;
        } else {
            m_Symbols[symbol.name] = symbol;
            // 更新完全限定名
            m_Symbols[symbol.name].fullName = GetFullName() + "::" + symbol.name;
        }
    }
    
    // 递归合并子命名空间
    for (const auto& pair : other.m_Children) {
        const std::string& childName = pair.first;
        const Namespace* otherChild = pair.second.get();
        
        Namespace* myChild = GetChildNamespace(childName);
        if (!myChild) {
            // 创建新的子命名空间
            myChild = CreateOrGetChildNamespace(childName);
        }
        
        // 递归合并
        if (!myChild->Merge(*otherChild, conflicts)) {
            success = false;
        }
    }
    
    return success;
}

bool Namespace::HasConflict(const SymbolInfo& symbol) const {
    auto it = m_Symbols.find(symbol.name);
    if (it == m_Symbols.end()) {
        return false;
    }
    
    // 同名同类型的符号视为冲突
    return it->second.type == symbol.type;
}

// NamespaceManager 实现

NamespaceManager::NamespaceManager() {
    m_GlobalNamespace = std::make_unique<Namespace>("", nullptr);
    m_CurrentNamespace = m_GlobalNamespace.get();
}

NamespaceManager::~NamespaceManager() = default;

void NamespaceManager::SetCurrentNamespace(Namespace* ns) {
    if (ns) {
        m_CurrentNamespace = ns;
    }
}

void NamespaceManager::EnterNamespace(const std::string& name) {
    m_NamespaceStack.push_back(m_CurrentNamespace);
    
    // 创建或获取命名空间
    Namespace* ns = m_CurrentNamespace->CreateOrGetChildNamespace(name);
    m_CurrentNamespace = ns;
    
    utils::Logger::GetInstance().Debug("进入命名空间: " + ns->GetFullName());
}

void NamespaceManager::ExitNamespace() {
    if (!m_NamespaceStack.empty()) {
        m_CurrentNamespace = m_NamespaceStack.back();
        m_NamespaceStack.pop_back();
        
        utils::Logger::GetInstance().Debug("退出命名空间，当前: " + 
                                         m_CurrentNamespace->GetFullName());
    }
}

Namespace* NamespaceManager::ResolveNamespacePath(const std::string& path) {
    if (path.empty()) {
        return m_CurrentNamespace;
    }
    
    std::vector<std::string> parts = SplitNamespacePath(path);
    Namespace* current = m_GlobalNamespace.get();
    
    for (const auto& part : parts) {
        current = current->GetChildNamespace(part);
        if (!current) {
            return nullptr;
        }
    }
    
    return current;
}

SymbolInfo* NamespaceManager::ResolveSymbol(const std::string& name,
                                           SymbolType type,
                                           bool searchParent) {
    // 首先检查是否是完全限定名
    if (name.find("::") != std::string::npos) {
        std::size_t lastSep = name.rfind("::");
        std::string nsPath = name.substr(0, lastSep);
        std::string symbolName = name.substr(lastSep + 2);
        
        Namespace* ns = ResolveNamespacePath(nsPath);
        if (ns) {
            return ns->FindSymbol(symbolName, type);
        }
        return nullptr;
    }
    
    // 在当前命名空间查找
    Namespace* current = m_CurrentNamespace;
    while (current) {
        SymbolInfo* symbol = current->FindSymbol(name, type);
        if (symbol) {
            return symbol;
        }
        
        if (!searchParent) {
            break;
        }
        
        current = current->GetParent();
    }
    
    return nullptr;
}

SymbolInfo* NamespaceManager::ResolveSymbolFrom(const std::string& name,
                                               const std::string& fromNamespace,
                                               SymbolType type) {
    Namespace* ns = ResolveNamespacePath(fromNamespace);
    if (!ns) {
        utils::Logger::GetInstance().Error("找不到命名空间: " + fromNamespace);
        return nullptr;
    }
    
    return ns->FindSymbol(name, type);
}

bool NamespaceManager::RegisterSymbol(const SymbolInfo& symbol) {
    SymbolInfo newSymbol = symbol;
    newSymbol.fullName = m_CurrentNamespace->GetFullName();
    if (!newSymbol.fullName.empty()) {
        newSymbol.fullName += "::";
    }
    newSymbol.fullName += symbol.name;
    
    bool success = m_CurrentNamespace->AddSymbol(newSymbol);
    
    if (!success) {
        SymbolInfo* existing = m_CurrentNamespace->FindSymbol(symbol.name, symbol.type);
        if (existing) {
            ReportConflict(*existing, newSymbol);
        }
    }
    
    return success;
}

bool NamespaceManager::MergeNamespace(ast::NamespaceNode* nsNode) {
    if (!nsNode) return false;
    
    const std::string& name = nsNode->GetName();
    
    // 查找现有的同名命名空间
    Namespace* existing = m_CurrentNamespace->GetChildNamespace(name);
    if (!existing) {
        // 创建新命名空间
        EnterNamespace(name);
        // TODO: 处理命名空间内容
        ExitNamespace();
        return true;
    }
    
    // 合并到现有命名空间
    std::vector<SymbolInfo> conflicts;
    
    // 创建临时命名空间用于合并
    Namespace temp(name);
    // TODO: 将nsNode的内容添加到temp
    
    bool success = existing->Merge(temp, conflicts);
    
    if (!conflicts.empty()) {
        for (const auto& conflict : conflicts) {
            m_Conflicts.push_back("命名空间合并冲突: " + conflict.fullName);
        }
    }
    
    return success;
}

std::string NamespaceManager::ExportSymbolTable() const {
    std::stringstream ss;
    ss << "符号表:\n";
    
    std::function<void(const Namespace*, int)> dumpNamespace;
    dumpNamespace = [&](const Namespace* ns, int indent) {
        std::string indentStr(indent * 2, ' ');
        
        if (!ns->GetName().empty()) {
            ss << indentStr << "命名空间: " << ns->GetFullName() << "\n";
        }
        
        // 导出符号
        auto symbols = ns->GetAllSymbols();
        for (const auto& symbol : symbols) {
            ss << indentStr << "  " << symbol.name << " [";
            
            switch (symbol.type) {
                case SymbolType::TEMPLATE_ELEMENT:
                    ss << "模板元素";
                    break;
                case SymbolType::TEMPLATE_STYLE:
                    ss << "模板样式";
                    break;
                case SymbolType::TEMPLATE_VAR:
                    ss << "模板变量";
                    break;
                case SymbolType::CUSTOM_ELEMENT:
                    ss << "自定义元素";
                    break;
                case SymbolType::CUSTOM_STYLE:
                    ss << "自定义样式";
                    break;
                case SymbolType::CUSTOM_VAR:
                    ss << "自定义变量";
                    break;
                case SymbolType::ORIGIN_HTML:
                    ss << "原始HTML";
                    break;
                case SymbolType::ORIGIN_STYLE:
                    ss << "原始Style";
                    break;
                case SymbolType::ORIGIN_JAVASCRIPT:
                    ss << "原始JavaScript";
                    break;
                case SymbolType::NAMESPACE:
                    ss << "命名空间";
                    break;
            }
            
            ss << "] " << symbol.fullName << "\n";
        }
        
        // 递归处理子命名空间
        auto children = ns->GetChildNamespaces();
        for (const auto& child : children) {
            dumpNamespace(child, indent + 1);
        }
    };
    
    dumpNamespace(m_GlobalNamespace.get(), 0);
    return ss.str();
}

void NamespaceManager::Reset() {
    m_GlobalNamespace = std::make_unique<Namespace>("", nullptr);
    m_CurrentNamespace = m_GlobalNamespace.get();
    m_NamespaceStack.clear();
    m_Conflicts.clear();
}

std::vector<std::string> NamespaceManager::SplitNamespacePath(const std::string& path) const {
    std::vector<std::string> parts;
    
    // 支持 :: 和 . 作为分隔符
    std::string current;
    for (size_t i = 0; i < path.length(); ++i) {
        if (i + 1 < path.length() && path[i] == ':' && path[i + 1] == ':') {
            if (!current.empty()) {
                parts.push_back(current);
                current.clear();
            }
            i++; // 跳过第二个 :
        } else if (path[i] == '.') {
            if (!current.empty()) {
                parts.push_back(current);
                current.clear();
            }
        } else {
            current += path[i];
        }
    }
    
    if (!current.empty()) {
        parts.push_back(current);
    }
    
    return parts;
}

void NamespaceManager::ReportConflict(const SymbolInfo& existing, const SymbolInfo& newSymbol) {
    std::stringstream ss;
    ss << "符号冲突: " << newSymbol.name << " [" << newSymbol.fullName << "] "
       << "已存在于 " << existing.sourceFile << ":" << existing.line << ":" << existing.column;
    
    std::string conflict = ss.str();
    m_Conflicts.push_back(conflict);
    utils::Logger::GetInstance().Error(conflict);
}

// NamespaceResolver 实现

NamespaceResolver::NamespaceResolver(NamespaceManager& manager)
    : m_Manager(manager) {}

SymbolInfo* NamespaceResolver::ResolveFromClause(const std::string& symbolName,
                                                const std::string& fromNamespace,
                                                SymbolType expectedType) {
    return m_Manager.ResolveSymbolFrom(symbolName, fromNamespace, expectedType);
}

SymbolInfo* NamespaceResolver::ResolveQualifiedName(const std::string& qualifiedName,
                                                   SymbolType expectedType) {
    return m_Manager.ResolveSymbol(qualifiedName, expectedType, false);
}

std::vector<SymbolInfo> NamespaceResolver::ResolveAllSymbols(const std::string& fromNamespace,
                                                            SymbolType type) {
    Namespace* ns = m_Manager.ResolveNamespacePath(fromNamespace);
    if (!ns) {
        return {};
    }
    
    return ns->GetSymbolsByType(type);
}

} // namespace ns
} // namespace chtl