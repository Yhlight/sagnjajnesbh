#pragma once

#include <string>

namespace CHTLNS {
	class CHTLCompiler {
	public:
		std::string Compile(const std::string& source);
	};
}