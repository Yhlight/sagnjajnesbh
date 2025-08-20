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
        parse(argc, argv);
    }

    bool hasOption(const std::string& option) const {
        return options_.find(option) != options_.end();
    }

    std::string getOptionValue(const std::string& option, 
                              const std::string& defaultValue = "") const {
        auto it = options_.find(option);
        if (it != options_.end() && !it->second.empty()) {
            return it->second;
        }
        return defaultValue;
    }

    const std::vector<std::string>& getPositionalArgs() const {
        return positionalArgs_;
    }

private:
    void parse(int argc, char* argv[]) {
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
                
                options_[option] = value;
            } else {
                // 位置参数
                positionalArgs_.push_back(arg);
            }
        }
    }

    std::unordered_map<std::string, std::string> options_;
    std::vector<std::string> positionalArgs_;
};

} // namespace utils
} // namespace chtl