#include "scanner/CodeFragment.h"
#include "utils/StringUtils.h"
#include <sstream>

namespace chtl {

std::string CodeFragment::ToString() const {
    std::stringstream ss;
    ss << "Fragment[Type: ";
    
    switch (m_Type) {
        case FragmentType::CHTL: ss << "CHTL"; break;
        case FragmentType::CHTL_JS: ss << "CHTL_JS"; break;
        case FragmentType::CSS: ss << "CSS"; break;
        case FragmentType::JAVASCRIPT: ss << "JAVASCRIPT"; break;
        case FragmentType::HTML: ss << "HTML"; break;
        case FragmentType::TEXT: ss << "TEXT"; break;
        case FragmentType::UNKNOWN: ss << "UNKNOWN"; break;
    }
    
    ss << ", Lines: " << m_StartLine << "-" << m_EndLine;
    ss << ", Content: \"" << utils::StringUtils::Trim(m_Content) << "\"";
    if (m_Content.length() > 50) {
        ss << "...";
    }
    ss << "]";
    
    return ss.str();
}

bool CodeFragment::IsCHTLElement() const {
    if (m_Type != FragmentType::CHTL) return false;
    
    std::string trimmed = utils::StringUtils::Trim(m_Content);
    // 检查是否是CHTL元素（不以[开头的）
    return !trimmed.empty() && trimmed[0] != '[' && trimmed[0] != '@';
}

bool CodeFragment::IsCHTLTemplate() const {
    if (m_Type != FragmentType::CHTL) return false;
    
    std::string trimmed = utils::StringUtils::Trim(m_Content);
    return utils::StringUtils::StartsWith(trimmed, "[Template]");
}

bool CodeFragment::IsCHTLCustom() const {
    if (m_Type != FragmentType::CHTL) return false;
    
    std::string trimmed = utils::StringUtils::Trim(m_Content);
    return utils::StringUtils::StartsWith(trimmed, "[Custom]");
}

bool CodeFragment::IsCHTLImport() const {
    if (m_Type != FragmentType::CHTL) return false;
    
    std::string trimmed = utils::StringUtils::Trim(m_Content);
    return utils::StringUtils::StartsWith(trimmed, "[Import]");
}

bool CodeFragment::IsCHTLConfiguration() const {
    if (m_Type != FragmentType::CHTL) return false;
    
    std::string trimmed = utils::StringUtils::Trim(m_Content);
    return utils::StringUtils::StartsWith(trimmed, "[Configuration]");
}

bool CodeFragment::IsCHTLNamespace() const {
    if (m_Type != FragmentType::CHTL) return false;
    
    std::string trimmed = utils::StringUtils::Trim(m_Content);
    return utils::StringUtils::StartsWith(trimmed, "[Namespace]");
}

bool CodeFragment::IsCHTLOrigin() const {
    if (m_Type != FragmentType::CHTL) return false;
    
    std::string trimmed = utils::StringUtils::Trim(m_Content);
    return utils::StringUtils::StartsWith(trimmed, "[Origin]");
}

bool CodeFragment::IsLocalStyle() const {
    if (m_Type != FragmentType::CHTL) return false;
    
    std::string trimmed = utils::StringUtils::Trim(m_Content);
    return trimmed == "style" || utils::StringUtils::StartsWith(trimmed, "style{") ||
           utils::StringUtils::StartsWith(trimmed, "style ");
}

bool CodeFragment::IsLocalScript() const {
    if (m_Type != FragmentType::CHTL) return false;
    
    std::string trimmed = utils::StringUtils::Trim(m_Content);
    return trimmed == "script" || utils::StringUtils::StartsWith(trimmed, "script{") ||
           utils::StringUtils::StartsWith(trimmed, "script ");
}

} // namespace chtl