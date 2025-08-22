#include "CompilerDispatcher.h"
#include "CSS/CssCompiler.h"
#include "JS/JsCompiler.h"
#include "CHTL/CHTLCompiler.h"
#include "CHTLJS/CHTLJSCompiler.h"

CompilerDispatcher::CompilerDispatcher(CssCompiler& css, JsCompiler& js)
	: cssCompiler(css), jsCompiler(js) {}

void CompilerDispatcher::SetChtlCompilers(CHTLNS::CHTLCompiler* c, CHTLJSNS::CHTLJSCompiler* j) {
	chtlCompiler = c;
	chtljsCompiler = j;
}

CompilationResult CompilerDispatcher::DispatchAndCompile(const std::vector<Segment>& segments) {
	CompilationResult result;
	std::string htmlBuffer;
	std::string styleBuffer;
	std::string scriptBuffer;

	// 将CHTL编译器生成的局部style也合并到全局样式中
	for (const auto& seg : segments) {
		switch (seg.type) {
			case SegmentType::CSS: {
				styleBuffer += cssCompiler.Compile(seg.text);
				break;
			}
			case SegmentType::JS: {
				scriptBuffer += jsCompiler.Compile(seg.text);
				break;
			}
			case SegmentType::OriginHtml: {
				htmlBuffer += seg.text;
				break;
			}
			case SegmentType::OriginStyle: {
				styleBuffer += seg.text;
				break;
			}
			case SegmentType::OriginJavaScript: {
				scriptBuffer += seg.text;
				break;
			}
			case SegmentType::OriginCustom: {
				htmlBuffer += seg.text;
				break;
			}
			case SegmentType::CHTL: {
				if (chtlCompiler) {
					auto r = chtlCompiler->CompileAll(seg.text);
					htmlBuffer += r.html;
					styleBuffer += r.localStyle;
				} else {
					htmlBuffer += seg.text;
				}
				break;
			}
			case SegmentType::CHTLJS: {
				if (chtljsCompiler) scriptBuffer += chtljsCompiler->CompileToJavaScript(seg.text);
				// 未配置时，为避免输出无效JS，不注入
				break;
			}
		}
	}

	result.htmlOutput = std::move(htmlBuffer);
	result.globalStyleOutput = std::move(styleBuffer);
	result.globalScriptOutput = std::move(scriptBuffer);
	return result;
}