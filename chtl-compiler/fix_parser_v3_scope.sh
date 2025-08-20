#!/bin/bash

echo "修复Parser V3的作用域问题..."

# 备份原文件
cp src/chtl/parser/CHTLParserV3.cpp src/chtl/parser/CHTLParserV3.cpp.bak

# 1. 查找所有缺少CHTLParserV3::前缀的函数定义
# 这些函数在错误信息中显示为"has not been declared"

# 修复ParseConfiguration等函数
sed -i 's/^std::shared_ptr<ast::v3::ConfigurationNode> ParseConfiguration()/std::shared_ptr<ast::v3::ConfigurationNode> CHTLParserV3::ParseConfiguration()/' src/chtl/parser/CHTLParserV3.cpp

# 修复ParseDelete
sed -i 's/^std::shared_ptr<ast::v3::DeleteNode> ParseDelete()/std::shared_ptr<ast::v3::DeleteNode> CHTLParserV3::ParseDelete()/' src/chtl/parser/CHTLParserV3.cpp

# 修复ParseInsert
sed -i 's/^std::shared_ptr<ast::v3::InsertNode> ParseInsert()/std::shared_ptr<ast::v3::InsertNode> CHTLParserV3::ParseInsert()/' src/chtl/parser/CHTLParserV3.cpp

# 修复其他函数
sed -i 's/^std::string ParseFullQualifiedName()/std::string CHTLParserV3::ParseFullQualifiedName()/' src/chtl/parser/CHTLParserV3.cpp
sed -i 's/^bool Match(CHTLTokenType type)/bool CHTLParserV3::Match(CHTLTokenType type)/' src/chtl/parser/CHTLParserV3.cpp
sed -i 's/^bool Check(CHTLTokenType type) const/bool CHTLParserV3::Check(CHTLTokenType type) const/' src/chtl/parser/CHTLParserV3.cpp
sed -i 's/^bool CheckAny(const int&)/bool CHTLParserV3::CheckAny(const std::vector<CHTLTokenType>\& types) const/' src/chtl/parser/CHTLParserV3.cpp
sed -i 's/^bool IsAtEnd() const/bool CHTLParserV3::IsAtEnd() const/' src/chtl/parser/CHTLParserV3.cpp
sed -i 's/^bool MatchColonOrEquals()/bool CHTLParserV3::MatchColonOrEquals()/' src/chtl/parser/CHTLParserV3.cpp
sed -i 's/^std::string ParseStringOrUnquoted()/std::string CHTLParserV3::ParseStringOrUnquoted()/' src/chtl/parser/CHTLParserV3.cpp
sed -i 's/^void PushContext(Context ctx)/void CHTLParserV3::PushContext(Context ctx)/' src/chtl/parser/CHTLParserV3.cpp
sed -i 's/^void PopContext()/void CHTLParserV3::PopContext()/' src/chtl/parser/CHTLParserV3.cpp
sed -i 's/^std::string ExtractOriginContent(size_t startPos, size_t endPos)/std::string CHTLParserV3::ExtractOriginContent(size_t startPos, size_t endPos)/' src/chtl/parser/CHTLParserV3.cpp
sed -i 's/^int ParseLocalStyle()/std::shared_ptr<ast::v3::StyleNode> CHTLParserV3::ParseLocalStyle()/' src/chtl/parser/CHTLParserV3.cpp
sed -i 's/^int ParseLocalScript()/std::shared_ptr<ast::v3::ScriptNode> CHTLParserV3::ParseLocalScript()/' src/chtl/parser/CHTLParserV3.cpp
sed -i 's/^int ParseScript()/std::shared_ptr<ast::v3::ASTNode> CHTLParserV3::ParseScript()/' src/chtl/parser/CHTLParserV3.cpp
sed -i 's/^int ParseComment()/std::shared_ptr<ast::v3::CommentNode> CHTLParserV3::ParseComment()/' src/chtl/parser/CHTLParserV3.cpp
sed -i 's/^std::string ParseImportPath()/std::string CHTLParserV3::ParseImportPath()/' src/chtl/parser/CHTLParserV3.cpp
sed -i 's/^std::string ParseIdentifierPath()/std::string CHTLParserV3::ParseIdentifierPath()/' src/chtl/parser/CHTLParserV3.cpp

# 2. 修复void函数的声明
sed -i 's/^void ParseTemplateParameters(ast::v3::TemplateNode\* tmpl)/void CHTLParserV3::ParseTemplateParameters(ast::v3::TemplateNode* tmpl)/' src/chtl/parser/CHTLParserV3.cpp
sed -i 's/^void ParseCustomElementDefinition(ast::v3::CustomNode\* custom)/void CHTLParserV3::ParseCustomElementDefinition(ast::v3::CustomNode* custom)/' src/chtl/parser/CHTLParserV3.cpp
sed -i 's/^void ParseCustomVarDefinition(ast::v3::CustomNode\* custom)/void CHTLParserV3::ParseCustomVarDefinition(ast::v3::CustomNode* custom)/' src/chtl/parser/CHTLParserV3.cpp
sed -i 's/^void ParseNameConfig(ast::v3::ConfigurationNode\* config)/void CHTLParserV3::ParseNameConfig(ast::v3::ConfigurationNode* config)/' src/chtl/parser/CHTLParserV3.cpp
sed -i 's/^void ParseOriginTypeConfig(ast::v3::ConfigurationNode\* config)/void CHTLParserV3::ParseOriginTypeConfig(ast::v3::ConfigurationNode* config)/' src/chtl/parser/CHTLParserV3.cpp

# 3. 修复CHTLToken返回类型
sed -i 's/^const CHTLToken& Advance()/const CHTLToken\& CHTLParserV3::Advance()/' src/chtl/parser/CHTLParserV3.cpp
sed -i 's/^const CHTLToken& Peek(size_t offset) const/const CHTLToken\& CHTLParserV3::Peek(size_t offset) const/' src/chtl/parser/CHTLParserV3.cpp
sed -i 's/^const CHTLToken& Previous() const/const CHTLToken\& CHTLParserV3::Previous() const/' src/chtl/parser/CHTLParserV3.cpp

# 4. 修复CurrentContext函数
sed -i 's/^CHTLParserV3::Context CurrentContext() const/CHTLParserV3::Context CHTLParserV3::CurrentContext() const/' src/chtl/parser/CHTLParserV3.cpp

# 5. 删除重复的命名空间闭合
# 统计namespace chtl出现的次数
count=$(grep -c "} // namespace chtl" src/chtl/parser/CHTLParserV3.cpp)
if [ $count -gt 1 ]; then
    # 删除最后一个
    tac src/chtl/parser/CHTLParserV3.cpp | sed '0,/} \/\/ namespace chtl/{//d;}' | tac > /tmp/parser_temp.cpp
    mv /tmp/parser_temp.cpp src/chtl/parser/CHTLParserV3.cpp
fi

echo "修复完成！"