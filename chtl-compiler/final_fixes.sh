#!/bin/bash
echo "应用最终修复..."

cd /workspace/chtl-compiler

# 1. 修复 CodeFragmentType -> FragmentType
sed -i 's/CodeFragmentType/FragmentType/g' include/dispatcher/CompilerDispatcher.h

# 2. 添加缺失的节点类型
cat >> include/chtl/ast/StyleBlockNode.h.patch << 'EOF'

// 添加到 StyleBlockNode.h
class StyleReferenceNode : public ReferenceNode {
public:
    StyleReferenceNode(const std::string& ref) 
        : ReferenceNode(ASTNodeType::STYLE_REFERENCE, ref) {}
    
    void Accept(ASTVisitor* visitor) override {
        visitor->VisitReference(this);
    }
};
EOF

# 3. 添加缺失的字面值节点
cat >> include/chtl/ast/LiteralNodes.h.patch << 'EOF'

// 添加到 LiteralNodes.h
using StringLiteralNode = LiteralNode;
using UnquotedLiteralNode = LiteralNode;
EOF

# 4. 修复 CHTLCompiler.cpp
sed -i 's/Compile(const std::string& sourceCode, const std::string& filename)/Compile(const CodeFragment\& fragment)/' src/chtl/compiler/CHTLCompiler.cpp

echo "修复脚本准备完成"