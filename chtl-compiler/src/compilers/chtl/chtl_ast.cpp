#include "compilers/chtl/chtl_ast.h"

namespace chtl {

void DocumentNode::accept(CHTLASTVisitor* visitor) {
    visitor->visitDocument(this);
}

void ElementNode::accept(CHTLASTVisitor* visitor) {
    visitor->visitElement(this);
}

void TextNode::accept(CHTLASTVisitor* visitor) {
    visitor->visitText(this);
}

void StyleBlockNode::accept(CHTLASTVisitor* visitor) {
    visitor->visitStyleBlock(this);
}

void TemplateDefinitionNode::accept(CHTLASTVisitor* visitor) {
    visitor->visitTemplateDefinition(this);
}

} // namespace chtl