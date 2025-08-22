#pragma once

#include <string>
#include <cstddef>

namespace CHTLJSNS {
	enum class TokenType {
		EndOfFile,
		Identifier,
		Number,
		String,

		LBrace, RBrace,
		LBracket, RBracket,
		LParen, RParen,
		Colon, Semi, Comma, Dot,
		Arrow,
		DoubleLBrace,
		DoubleRBrace,

		KeywordVir,
		KeywordListen,
		KeywordDelegate,
		KeywordAnimate
	};

	struct Token {
		TokenType type;
		std::string lexeme;
		std::size_t startOffset;
		std::size_t endOffset;
		int line;
		int column;
	};
}