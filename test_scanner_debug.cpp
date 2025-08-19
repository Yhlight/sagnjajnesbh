#include <iostream>
#include "Scanner/CHTLUnifiedScanner.h"

using namespace chtl;

int main() {
    std::cout << "🔍 统一扫描器调试" << std::endl;
    std::cout << "=================" << std::endl;

    scanner::CHTLUnifiedScanner scanner;
    
    std::string testCode = R"(
html {
    head {
        title { text { Test } }
    }
    body {
        div {
            class: container;
            h1 { text { Hello } }
        }
    }
}
)";
    
    auto result = scanner.scanCode(testCode);
    
    std::cout << "扫描结果:" << std::endl;
    std::cout << "片段数量: " << result.fragments.size() << std::endl;
    std::cout << "是否有错误: " << (result.hasErrors ? "是" : "否") << std::endl;
    
    for (size_t i = 0; i < result.fragments.size(); i++) {
        const auto& fragment = result.fragments[i];
        std::cout << "\n片段 " << i << ":" << std::endl;
        std::cout << "  上下文: " << static_cast<int>(fragment.context) << std::endl;
        std::cout << "  内容长度: " << fragment.content.length() << std::endl;
        std::cout << "  内容预览: " << fragment.content.substr(0, 50) << "..." << std::endl;
    }
    
    return 0;
}