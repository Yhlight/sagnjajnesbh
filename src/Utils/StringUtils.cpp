#include "Utils/StringUtils.h"
#include <algorithm>
#include <cctype>
#include <memory>

namespace CHTL {
namespace Utils {

std::string StringUtils::Trim(const std::string& str) {
    return TrimRight(TrimLeft(str));
}

std::string StringUtils::TrimLeft(const std::string& str) {
    size_t start = str.find_first_not_of(" \t\n\r\f\v");
    return (start == std::string::npos) ? "" : str.substr(start);
}

std::string StringUtils::TrimRight(const std::string& str) {
    size_t end = str.find_last_not_of(" \t\n\r\f\v");
    return (end == std::string::npos) ? "" : str.substr(0, end + 1);
}

std::vector<std::string> StringUtils::Split(const std::string& str, const std::string& delimiter) {
    std::vector<std::string> tokens;
    size_t start = 0;
    size_t end = str.find(delimiter);
    
    while (end != std::string::npos) {
        tokens.push_back(str.substr(start, end - start));
        start = end + delimiter.length();
        end = str.find(delimiter, start);
    }
    
    tokens.push_back(str.substr(start));
    return tokens;
}

std::string StringUtils::Join(const std::vector<std::string>& strings, const std::string& delimiter) {
    if (strings.empty()) {
        return "";
    }
    
    std::string result = strings[0];
    for (size_t i = 1; i < strings.size(); ++i) {
        result += delimiter + strings[i];
    }
    
    return result;
}

std::string StringUtils::ReplaceAll(const std::string& str, const std::string& from, const std::string& to) {
    if (from.empty()) {
        return str;
    }
    
    std::string result = str;
    size_t pos = 0;
    
    while ((pos = result.find(from, pos)) != std::string::npos) {
        result.replace(pos, from.length(), to);
        pos += to.length();
    }
    
    return result;
}

std::string StringUtils::ToLower(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(),
                   [](unsigned char c) { return std::tolower(c); });
    return result;
}

std::string StringUtils::ToUpper(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(),
                   [](unsigned char c) { return std::toupper(c); });
    return result;
}

bool StringUtils::StartsWith(const std::string& str, const std::string& prefix) {
    if (prefix.length() > str.length()) {
        return false;
    }
    return str.substr(0, prefix.length()) == prefix;
}

bool StringUtils::EndsWith(const std::string& str, const std::string& suffix) {
    if (suffix.length() > str.length()) {
        return false;
    }
    return str.substr(str.length() - suffix.length()) == suffix;
}

bool StringUtils::Contains(const std::string& str, const std::string& substr) {
    return str.find(substr) != std::string::npos;
}

bool StringUtils::IsWhitespace(char ch) {
    return std::isspace(static_cast<unsigned char>(ch));
}

bool StringUtils::IsAlpha(char ch) {
    return std::isalpha(static_cast<unsigned char>(ch));
}

bool StringUtils::IsDigit(char ch) {
    return std::isdigit(static_cast<unsigned char>(ch));
}

bool StringUtils::IsAlphaNumeric(char ch) {
    return std::isalnum(static_cast<unsigned char>(ch));
}

std::string StringUtils::EscapeHtml(const std::string& str) {
    std::string result = str;
    result = ReplaceAll(result, "&", "&amp;");
    result = ReplaceAll(result, "<", "&lt;");
    result = ReplaceAll(result, ">", "&gt;");
    result = ReplaceAll(result, "\"", "&quot;");
    result = ReplaceAll(result, "'", "&#39;");
    return result;
}

std::string StringUtils::EscapeCss(const std::string& str) {
    std::string result = str;
    result = ReplaceAll(result, "\\", "\\\\");
    result = ReplaceAll(result, "\"", "\\\"");
    result = ReplaceAll(result, "'", "\\'");
    result = ReplaceAll(result, "\n", "\\A ");
    result = ReplaceAll(result, "\r", "\\D ");
    result = ReplaceAll(result, "\t", "\\9 ");
    return result;
}

std::string StringUtils::EscapeJavaScript(const std::string& str) {
    std::string result = str;
    result = ReplaceAll(result, "\\", "\\\\");
    result = ReplaceAll(result, "\"", "\\\"");
    result = ReplaceAll(result, "'", "\\'");
    result = ReplaceAll(result, "\n", "\\n");
    result = ReplaceAll(result, "\r", "\\r");
    result = ReplaceAll(result, "\t", "\\t");
    result = ReplaceAll(result, "\b", "\\b");
    result = ReplaceAll(result, "\f", "\\f");
    result = ReplaceAll(result, "\v", "\\v");
    return result;
}

} // namespace Utils
} // namespace CHTL