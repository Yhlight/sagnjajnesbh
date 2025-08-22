#include <iostream>
#include <string>
#include <vector>
#include "Core/CHTLUnifiedScanner.h"
#include "Core/CompilerDispatcher.h"
#include "Core/Merge/ResultMerger.h"
#include "Utils/FileIO.h"
#include "CSS/CssCompiler.h"
#include "JS/JsCompiler.h"
#include "CHTL/CHTLCompiler.h"
#include "CHTLJS/CHTLJSCompiler.h"

int main(int argc, char** argv) {
	if (argc < 2) {
		std::cerr << "用法: chtlc <输入CHTL文件路径>" << std::endl;
		return 1;
	}

	const std::string inputPath = argv[1];
	std::string source;
	if (!FileIO::ReadTextFile(inputPath, source)) {
		std::cerr << "无法读取文件: " << inputPath << std::endl;
		return 1;
	}

	CHTLUnifiedScanner scanner;
	auto segments = scanner.SliceSegments(source);

	CssCompiler cssCompiler;
	JsCompiler jsCompiler;
	CompilerDispatcher dispatcher(cssCompiler, jsCompiler);
	CHTLNS::CHTLCompiler chtlCompiler;
	CHTLJSNS::CHTLJSCompiler chtljsCompiler;
	dispatcher.SetChtlCompilers(&chtlCompiler, &chtljsCompiler);
	auto result = dispatcher.DispatchAndCompile(segments);

	const std::string merged = ResultMerger::MergeToHtml(result);
	std::cout << merged << std::endl;
	return 0;
}