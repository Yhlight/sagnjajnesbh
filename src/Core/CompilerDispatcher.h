#pragma once

#include <vector>
#include "Core/CompilationResult.h"
#include "Core/Segment.h"
#include "CHTL/CHTLCompiler.h"
#include "CHTLJS/CHTLJSCompiler.h"

class CssCompiler;
class JsCompiler;

class CompilerDispatcher {
public:
	CompilerDispatcher(CssCompiler& css, JsCompiler& js);
	void SetChtlCompilers(class CHTLNS::CHTLCompiler* c, class CHTLJSNS::CHTLJSCompiler* j);
	CompilationResult DispatchAndCompile(const std::vector<Segment>& segments);

private:
	CssCompiler& cssCompiler;
	JsCompiler& jsCompiler;
	class CHTLNS::CHTLCompiler* chtlCompiler = nullptr;
	class CHTLJSNS::CHTLJSCompiler* chtljsCompiler = nullptr;
};