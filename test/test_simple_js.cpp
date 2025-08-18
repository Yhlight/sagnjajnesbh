#include "../src/Shared/AST/CommentNode.h"
#include <iostream>

using namespace chtl::ast;

int main() {
    std::cout << "Testing simple JS comment generation..." << std::endl;
    
    CommentNode comment("Test comment", CommentType::SINGLE_LINE, CommentContext::JAVASCRIPT);
    std::string result = comment.generateComment();
    
    std::cout << "Generated: '" << result << "'" << std::endl;
    std::cout << "Expected: '// Test comment'" << std::endl;
    
    return 0;
}