#!/bin/bash

echo "Fixing remaining compilation errors..."

# Fix CHTLJSCompiler.cpp - change unique_ptr to shared_ptr
sed -i 's/m_GlobalMap = std::make_unique<CHTLJSGlobalMap>()/m_GlobalMap = std::make_shared<CHTLJSGlobalMap>()/' src/chtl_js/compiler/CHTLJSCompiler.cpp

# Fix SetSource -> SetInput
sed -i 's/m_Lexer->SetSource(/m_Lexer->SetInput(/' src/chtl_js/compiler/CHTLJSCompiler.cpp

# Fix Tokenize -> ScanTokens
sed -i 's/m_Lexer->Tokenize()/m_Lexer->ScanTokens()/' src/chtl_js/compiler/CHTLJSCompiler.cpp

# Remove duplicate GetName method
sed -i '/^std::string CHTLJSCompiler::GetName/,/^}$/d' src/chtl_js/compiler/CHTLJSCompiler.cpp

# Remove GetErrors and ClearErrors methods
sed -i '/^const std::vector<std::string>& CHTLJSCompiler::GetErrors/,/^}$/d' src/chtl_js/compiler/CHTLJSCompiler.cpp
sed -i '/^void CHTLJSCompiler::ClearErrors/,/^}$/d' src/chtl_js/compiler/CHTLJSCompiler.cpp

# Fix JavaScriptLexerBase.cpp include path
sed -i 's|#include "JavaScriptLexer.h"|#include "../JavaScriptLexer.h"|' src/javascript/grammar/Cpp/JavaScriptLexerBase.cpp

# Fix CHTLJSCompiler.h to use shared_ptr
sed -i 's/std::unique_ptr<CHTLJSGlobalMap>/std::shared_ptr<CHTLJSGlobalMap>/' include/chtl_js/compiler/CHTLJSCompiler.h

# Add missing includes for CHTL parser
cat >> include/chtl/parser/CHTLParser.h << 'EOF'
// Forward declarations if includes fail
namespace chtl {
namespace ast {
    class TemplateStyleNode;
    class TemplateElementNode;
    class TemplateVarNode;
    class CustomStyleNode;
    class CustomElementNode;
    class CustomVarNode;
}
}
EOF

echo "Fixes applied."