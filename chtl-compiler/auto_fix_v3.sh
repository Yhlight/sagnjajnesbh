#!/bin/bash

echo "自动修复V3编译错误..."

# 1. 修复ParseText返回类型
sed -i 's/std::shared_ptr<ast::v3::ASTNode> CHTLParserV3::ParseText()/std::shared_ptr<ast::v3::TextNode> CHTLParserV3::ParseText()/' src/chtl/parser/CHTLParserV3.cpp

# 2. 修复ParseAttributes签名
sed -i 's/void CHTLParserV3::ParseAttributes(ast::v3::ElementNode\* element)/std::unordered_map<std::string, std::string> CHTLParserV3::ParseAttributes()/' src/chtl/parser/CHTLParserV3.cpp

# 3. 修复ParseLocalStyle/ParseLocalScript调用
sed -i 's/ParseLocalStyle(element);/auto localStyle = ParseLocalStyle();\n            if (localStyle) element->AddChild(localStyle);/' src/chtl/parser/CHTLParserV3.cpp
sed -i 's/ParseLocalScript(element);/auto localScript = ParseLocalScript();\n            if (localScript) element->AddChild(localScript);/' src/chtl/parser/CHTLParserV3.cpp

# 4. 修复INHERIT token类型
sed -i 's/CHTLTokenType::INHERIT/CHTLTokenType::IDENTIFIER/' src/chtl/parser/CHTLParserV3.cpp

# 5. 修复ParseCustomStyleDefinition
sed -i 's/ParseCustomStyleDefinition/ParseCustomDefinition/' src/chtl/parser/CHTLParserV3.cpp

# 6. 修复ParseAttributes调用
sed -i 's/ParseAttributes(element);/auto attrs = ParseAttributes();\n                for (const auto\& [key, value] : attrs) {\n                    element->AddAttribute(key, value);\n                }/' src/chtl/parser/CHTLParserV3.cpp

# 7. 添加缺失的ParseSpecialization声明
if ! grep -q "ParseSpecialization" src/chtl/parser/CHTLParserV3.h; then
    sed -i '/ParseInsert/a\    std::shared_ptr<ast::v3::ASTNode> ParseSpecialization();' src/chtl/parser/CHTLParserV3.h
fi

echo "修复完成！"

# 尝试编译
./build_v3.sh