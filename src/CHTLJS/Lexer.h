#pragma once

#include <string>
#include <vector>
#include "Token.h"

namespace CHTLJSNS {
	class Lexer {
	public:
		std::vector<Token> Tokenize(const std::string& source) const;
	};
}