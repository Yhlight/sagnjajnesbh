#pragma once

#include <string>
#include "Generator.h"

namespace CHTLNS {
	class CHTLCompiler {
	public:
		std::string Compile(const std::string& source);
		GenerateResult CompileAll(const std::string& source);
	};
}