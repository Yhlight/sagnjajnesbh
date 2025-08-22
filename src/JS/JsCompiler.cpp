#include "JsCompiler.h"
#include <sstream>
#include <memory>
#include "antlr4-runtime.h"
#include "JavaScriptLexer.h"
#include "JavaScriptParser.h"

std::string JsCompiler::Compile(const std::string& jsSource) {
	antlr4::ANTLRInputStream input(jsSource);
	JavaScriptLexer lexer(&input);
	antlr4::CommonTokenStream tokens(&lexer);
	JavaScriptParser parser(&tokens);
	parser.program();
	return jsSource;
}