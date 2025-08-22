#pragma once

#include <string>
#include <vector>
#include "AST.h"
#include "Lexer.h"

namespace CHTLNS {
	class Parser {
	public:
		Document* Parse(const std::string& source);
	};
}