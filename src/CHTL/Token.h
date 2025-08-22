#pragma once

#include <string>
#include <cstddef>

namespace CHTLNS {
	enum class TokenType {
		EndOfFile,
		Identifier,
		Number,
		String,

		LBrace, RBrace,
		LBracket, RBracket,
		LParen, RParen,
		Colon, Equal, Semi, Comma, Dot,
		At,

		KeywordText,
		KeywordStyle,
		KeywordScript,
		KeywordCustom,
		KeywordTemplate,
		KeywordOrigin,
		KeywordImport,
		KeywordNamespace,
		KeywordInherit,
		KeywordDelete,
		KeywordInsert,
		KeywordAfter,
		KeywordBefore,
		KeywordReplace,
		KeywordAtTop,
		KeywordAtBottom,
		KeywordFrom,
		KeywordAs,
		KeywordExcept
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