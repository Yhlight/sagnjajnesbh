#include "CHTLCompiler.h"
#include "Parser.h"
#include "Generator.h"

using namespace CHTLNS;

GenerateResult CHTLCompiler::CompileAll(const std::string& source) {
	Parser p; Document* doc = p.Parse(source);
	Generator g; auto r = g.Generate(doc);
	delete doc;
	return r;
}

std::string CHTLCompiler::Compile(const std::string& source) {
	auto r = CompileAll(source);
	return r.html;
}