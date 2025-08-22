#include "CssCompiler.h"
#include <sstream>
#include <memory>
#include "antlr4-runtime.h"
#include "css3Lexer.h"
#include "css3Parser.h"

std::string CssCompiler::Compile(const std::string& cssSource) {
	antlr4::ANTLRInputStream input(cssSource);
	css3Lexer lexer(&input);
	antlr4::CommonTokenStream tokens(&lexer);
	css3Parser parser(&tokens);
	parser.stylesheet();
	// 首版：解析以验证集成，保持原样输出
	return cssSource;
}