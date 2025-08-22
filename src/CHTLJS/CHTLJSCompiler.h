#pragma once

#include <string>

namespace CHTLJSNS {
	class CHTLJSCompiler {
	public:
		std::string CompileToJavaScript(const std::string& source);
	};
}