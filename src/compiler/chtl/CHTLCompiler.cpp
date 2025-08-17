#include <string>

namespace chtl {
    // 占位符实现
    class CHTLCompiler {
    public:
        std::string compile(const std::string& code) {
            return "<!-- CHTL: " + code + " -->";
        }
    };
}