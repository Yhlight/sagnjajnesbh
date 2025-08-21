#!/bin/bash

echo "全面修复V3编译错误..."

# 1. 修复CHTLCompilerV3.h中的ast::DocumentNode引用
sed -i 's/std::shared_ptr<ast::DocumentNode>/std::shared_ptr<ast::v3::DocumentNode>/g' src/chtl/CHTLCompilerV3.h

# 2. 修复所有返回类型不匹配的Parse函数
# ParseConfiguration
sed -i 's/std::shared_ptr<ast::v3::ASTNode> CHTLParserV3::ParseConfiguration()/std::shared_ptr<ast::v3::ConfigurationNode> CHTLParserV3::ParseConfiguration()/' src/chtl/parser/CHTLParserV3.cpp

# ParseImport
sed -i 's/std::shared_ptr<ast::v3::ASTNode> CHTLParserV3::ParseImport()/std::shared_ptr<ast::v3::ImportNode> CHTLParserV3::ParseImport()/' src/chtl/parser/CHTLParserV3.cpp

# ParseNamespace
sed -i 's/std::shared_ptr<ast::v3::ASTNode> CHTLParserV3::ParseNamespace()/std::shared_ptr<ast::v3::NamespaceNode> CHTLParserV3::ParseNamespace()/' src/chtl/parser/CHTLParserV3.cpp

# ParseOrigin
sed -i 's/std::shared_ptr<ast::v3::ASTNode> CHTLParserV3::ParseOrigin()/std::shared_ptr<ast::v3::OriginNode> CHTLParserV3::ParseOrigin()/' src/chtl/parser/CHTLParserV3.cpp

# ParseExcept
sed -i 's/std::shared_ptr<ast::v3::ASTNode> CHTLParserV3::ParseExcept()/std::shared_ptr<ast::v3::ExceptNode> CHTLParserV3::ParseExcept()/' src/chtl/parser/CHTLParserV3.cpp

# ParseVar
sed -i 's/std::shared_ptr<ast::v3::ASTNode> CHTLParserV3::ParseVar()/std::shared_ptr<ast::v3::VarNode> CHTLParserV3::ParseVar()/' src/chtl/parser/CHTLParserV3.cpp

# ParseVarCall
sed -i 's/std::shared_ptr<ast::v3::ASTNode> CHTLParserV3::ParseVarCall()/std::shared_ptr<ast::v3::VarCallNode> CHTLParserV3::ParseVarCall()/' src/chtl/parser/CHTLParserV3.cpp

# ParseDelete
sed -i 's/std::shared_ptr<ast::v3::ASTNode> CHTLParserV3::ParseDelete()/std::shared_ptr<ast::v3::DeleteNode> CHTLParserV3::ParseDelete()/' src/chtl/parser/CHTLParserV3.cpp

# ParseInsert
sed -i 's/std::shared_ptr<ast::v3::ASTNode> CHTLParserV3::ParseInsert()/std::shared_ptr<ast::v3::InsertNode> CHTLParserV3::ParseInsert()/' src/chtl/parser/CHTLParserV3.cpp

# ParseComment
sed -i 's/std::shared_ptr<ast::v3::ASTNode> CHTLParserV3::ParseComment()/std::shared_ptr<ast::v3::CommentNode> CHTLParserV3::ParseComment()/' src/chtl/parser/CHTLParserV3.cpp

# 3. 修复Advance等函数的返回类型
sed -i 's/CHTLToken CHTLParserV3::Advance()/const CHTLToken\& CHTLParserV3::Advance()/' src/chtl/parser/CHTLParserV3.cpp
sed -i 's/CHTLToken CHTLParserV3::Peek()/const CHTLToken\& CHTLParserV3::Peek(size_t offset)/' src/chtl/parser/CHTLParserV3.cpp
sed -i 's/CHTLToken CHTLParserV3::Previous()/const CHTLToken\& CHTLParserV3::Previous()/' src/chtl/parser/CHTLParserV3.cpp

# 4. 修复CheckAny签名
sed -i 's/bool CHTLParserV3::CheckAny(std::initializer_list<CHTLTokenType> types)/bool CHTLParserV3::CheckAny(const std::vector<CHTLTokenType>\& types)/' src/chtl/parser/CHTLParserV3.cpp

# 5. 删除重复的ReportError定义
sed -i '/^void CHTLParserV3::ReportError(const std::string& message) {/,/^}/d' src/chtl/parser/CHTLParserV3.cpp

# 6. 修复ParseLocalStyle和ParseLocalScript签名
sed -i 's/void CHTLParserV3::ParseLocalStyle(ast::v3::ElementNode\* element)/std::shared_ptr<ast::v3::StyleNode> CHTLParserV3::ParseLocalStyle()/' src/chtl/parser/CHTLParserV3.cpp
sed -i 's/void CHTLParserV3::ParseLocalScript(ast::v3::ElementNode\* element)/std::shared_ptr<ast::v3::ScriptNode> CHTLParserV3::ParseLocalScript()/' src/chtl/parser/CHTLParserV3.cpp

# 7. 添加缺失的函数声明到头文件
# 在ParseOriginContent之前添加缺失的辅助函数声明
cat >> /tmp/missing_decls.txt << 'EOF'
    // 辅助解析函数
    void ParseTemplateParameters(ast::v3::TemplateNode* tmpl);
    void ParseCustomElementDefinition(ast::v3::CustomNode* custom);
    void ParseCustomVarDefinition(ast::v3::CustomNode* custom);
    void ParseNameConfig(ast::v3::ConfigurationNode* config);
    void ParseOriginTypeConfig(ast::v3::ConfigurationNode* config);
    std::string ParseImportPath();
    std::string ParseIdentifierPath();
    std::vector<std::string> ParseExceptTargets();
    std::string ExtractOriginContent(size_t startPos, size_t endPos);
    void ParseStyleRule(ast::v3::StyleNode* style);
    std::shared_ptr<ast::v3::ASTNode> ParseSpecialization();
EOF

# 8. 在main_v3.cpp中添加AST头文件包含
sed -i '/#include "CHTLCompilerV3.h"/a#include "ast/CHTLASTNodesV3.h"' src/chtl/main_v3.cpp

# 9. 修复ParseAttributes返回值
sed -i '/std::unordered_map<std::string, std::string> CHTLParserV3::ParseAttributes()/,/^}/ { /^}/ i\    return attributes;
}' src/chtl/parser/CHTLParserV3.cpp

echo "修复完成！"