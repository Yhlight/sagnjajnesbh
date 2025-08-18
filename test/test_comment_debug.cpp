#include "../src/Shared/AST/CommentNode.h"
#include <iostream>

using namespace chtl::ast;

int main() {
    CommentNode generator("Generator comment", CommentType::GENERATOR, CommentContext::CHTL);
    std::string result = generator.generateComment();
    
    std::cout << "Expected: '-- Generator comment'" << std::endl;
    std::cout << "Actual: '" << result << "'" << std::endl;
    std::cout << "Length: " << result.length() << std::endl;
    
    // 检查字符
    for (size_t i = 0; i < result.length(); ++i) {
        std::cout << "Char[" << i << "]: '" << result[i] << "' (ASCII: " << static_cast<int>(result[i]) << ")" << std::endl;
    }
    
    return 0;
}