#pragma once

#include <string>
#include <vector>
#include <algorithm>
#include <sstream>
#include <cctype>

namespace chtl {
namespace utils {

class StringUtils {
public:
    static std::string trim(const std::string& str) {
        size_t first = str.find_first_not_of(" \t\n\r");
        if (first == std::string::npos) return "";
        size_t last = str.find_last_not_of(" \t\n\r");
        return str.substr(first, (last - first + 1));
    }

    static std::string ltrim(const std::string& str) {
        size_t first = str.find_first_not_of(" \t\n\r");
        if (first == std::string::npos) return "";
        return str.substr(first);
    }

    static std::string rtrim(const std::string& str) {
        size_t last = str.find_last_not_of(" \t\n\r");
        if (last == std::string::npos) return "";
        return str.substr(0, last + 1);
    }

    static std::vector<std::string> split(const std::string& str, char delimiter) {
        std::vector<std::string> tokens;
        std::stringstream ss(str);
        std::string token;
        
        while (std::getline(ss, token, delimiter)) {
            if (!token.empty()) {
                tokens.push_back(token);
            }
        }
        
        return tokens;
    }

    static std::string join(const std::vector<std::string>& strings, 
                           const std::string& delimiter) {
        if (strings.empty()) return "";
        
        std::stringstream ss;
        ss << strings[0];
        
        for (size_t i = 1; i < strings.size(); ++i) {
            ss << delimiter << strings[i];
        }
        
        return ss.str();
    }

    static std::string toLower(const std::string& str) {
        std::string result = str;
        std::transform(result.begin(), result.end(), result.begin(), ::tolower);
        return result;
    }

    static std::string toUpper(const std::string& str) {
        std::string result = str;
        std::transform(result.begin(), result.end(), result.begin(), ::toupper);
        return result;
    }

    static bool startsWith(const std::string& str, const std::string& prefix) {
        if (prefix.size() > str.size()) return false;
        return str.compare(0, prefix.size(), prefix) == 0;
    }

    static bool endsWith(const std::string& str, const std::string& suffix) {
        if (suffix.size() > str.size()) return false;
        return str.compare(str.size() - suffix.size(), suffix.size(), suffix) == 0;
    }

    static std::string replace(const std::string& str, 
                              const std::string& from, 
                              const std::string& to) {
        std::string result = str;
        size_t pos = 0;
        
        while ((pos = result.find(from, pos)) != std::string::npos) {
            result.replace(pos, from.length(), to);
            pos += to.length();
        }
        
        return result;
    }

    static bool isNumber(const std::string& str) {
        if (str.empty()) return false;
        
        size_t start = 0;
        if (str[0] == '-' || str[0] == '+') {
            if (str.size() == 1) return false;
            start = 1;
        }
        
        bool hasDecimal = false;
        for (size_t i = start; i < str.size(); ++i) {
            if (str[i] == '.') {
                if (hasDecimal) return false;
                hasDecimal = true;
            } else if (!std::isdigit(str[i])) {
                return false;
            }
        }
        
        return true;
    }

    static std::string escapeHtml(const std::string& str) {
        std::string result;
        result.reserve(str.size() * 1.2);
        
        for (char c : str) {
            switch (c) {
                case '<': result += "&lt;"; break;
                case '>': result += "&gt;"; break;
                case '&': result += "&amp;"; break;
                case '"': result += "&quot;"; break;
                case '\'': result += "&#39;"; break;
                default: result += c; break;
            }
        }
        
        return result;
    }
};

} // namespace utils
} // namespace chtl