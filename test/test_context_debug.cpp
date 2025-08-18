#include "../src/Shared/AST/CommentNode.h"
#include "../src/Shared/AST/CommentGenerator.h"
#include <iostream>

using namespace chtl::ast;

int main() {
    CommentGenerator generator;
    
    CommentNode comment("Test comment", CommentType::SINGLE_LINE, CommentContext::CHTL_JS);
    std::string chtl_code = "@Html { text { Hello World } }";
    
    // 检查上下文推断
    CodeBlockType block_type = generator.inferBlockType(chtl_code);
    CommentContext target_context = generator.mapBlockTypeToContext(block_type);
    
    std::cout << "Code: " << chtl_code << std::endl;
    std::cout << "Block type: " << static_cast<int>(block_type) << std::endl;
    std::cout << "Target context: " << static_cast<int>(target_context) << std::endl;
    
    // 检查各种语法检测
    std::cout << "Contains CHTL syntax: " << (generator.containsCHTLSyntax(chtl_code) ? "true" : "false") << std::endl;
    std::cout << "Contains CHTL JS syntax: " << (generator.containsCHTLJSSyntax(chtl_code) ? "true" : "false") << std::endl;
    std::cout << "Contains CSS syntax: " << (generator.containsCSSSyntax(chtl_code) ? "true" : "false") << std::endl;
    std::cout << "Contains JS syntax: " << (generator.containsJSSyntax(chtl_code) ? "true" : "false") << std::endl;
    std::cout << "Contains HTML syntax: " << (generator.containsHTMLSyntax(chtl_code) ? "true" : "false") << std::endl;
    
    std::string result = generator.generateContextualComment(comment, chtl_code);
    std::cout << "Generated comment: '" << result << "'" << std::endl;
    
    // 直接测试CommentNode的generateComment
    CommentNode direct_test("Test comment", CommentType::SINGLE_LINE, CommentContext::CHTL);
    std::string direct_result = direct_test.generateComment();
    std::cout << "Direct CHTL context result: '" << direct_result << "'" << std::endl;
    
    return 0;
}