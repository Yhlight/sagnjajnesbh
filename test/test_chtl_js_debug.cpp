#include "../src/Shared/AST/CommentNode.h"
#include "../src/Shared/AST/CommentGenerator.h"
#include <iostream>

using namespace chtl::ast;

int main() {
    CommentGenerator generator;
    
    CommentNode comment("Test comment", CommentType::SINGLE_LINE, CommentContext::CHTL_JS);
    std::string chtl_js_code = "{{button}}->click({ action: function() { alert('test'); } })";
    
    // 检查上下文推断
    CodeBlockType block_type = generator.inferBlockType(chtl_js_code);
    CommentContext target_context = generator.mapBlockTypeToContext(block_type);
    
    std::cout << "CHTL JS Code: " << chtl_js_code << std::endl;
    std::cout << "Block type: " << static_cast<int>(block_type) << std::endl;
    std::cout << "Target context: " << static_cast<int>(target_context) << std::endl;
    
    // 检查各种语法检测
    std::cout << "Contains CHTL syntax: " << (generator.containsCHTLSyntax(chtl_js_code) ? "true" : "false") << std::endl;
    std::cout << "Contains CHTL JS syntax: " << (generator.containsCHTLJSSyntax(chtl_js_code) ? "true" : "false") << std::endl;
    std::cout << "Contains CSS syntax: " << (generator.containsCSSSyntax(chtl_js_code) ? "true" : "false") << std::endl;
    std::cout << "Contains specific CSS syntax: " << (generator.containsSpecificCSSSyntax(chtl_js_code) ? "true" : "false") << std::endl;
    std::cout << "Contains JS syntax: " << (generator.containsJSSyntax(chtl_js_code) ? "true" : "false") << std::endl;
    std::cout << "Contains HTML syntax: " << (generator.containsHTMLSyntax(chtl_js_code) ? "true" : "false") << std::endl;
    
    std::string result = generator.generateContextualComment(comment, chtl_js_code);
    std::cout << "Generated comment: '" << result << "'" << std::endl;
    
    return 0;
}