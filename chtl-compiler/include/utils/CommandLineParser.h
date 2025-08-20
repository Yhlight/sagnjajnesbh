#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <algorithm>

namespace chtl {
namespace utils {

class CommandLineParser {
public:
    CommandLineParser(int argc, char* argv[]) {
        Parse(argc, argv);
    }

    bool HasOption(const std::string& option) const {
        return m_Options.find(option) != m_Options.end();
    }

    std::string GetOptionValue(const std::string& option, 
                              const std::string& defaultValue = "") const {
        auto it = m_Options.find(option);
        if (it != m_Options.end() && !it->second.empty()) {
            return it->second;
        }
        return defaultValue;
    }

    const std::vector<std::string>& GetPositionalArgs() const {
        return m_PositionalArgs;
    }

private:
    void Parse(int argc, char* argv[]) {
        for (int i = 1; i < argc; ++i) {
            std::string arg = argv[i];
            
            if (arg.starts_with("-")) {
                // 选项参数
                std::string option = arg;
                std::string value;
                
                // 检查是否有值
                if (i + 1 < argc && !std::string(argv[i + 1]).starts_with("-")) {
                    value = argv[++i];
                }
                
                m_Options[option] = value;
            } else {
                // 位置参数
                m_PositionalArgs.push_back(arg);
            }
        }
    }

    std::unordered_map<std::string, std::string> m_Options;
    std::vector<std::string> m_PositionalArgs;
};

} // namespace utils
} // namespace chtl