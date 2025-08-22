#include "Lexer.h"
#include <cctype>

using namespace CHTLJSNS;

namespace {
	static bool IsIdentStart(char c) { return std::isalpha(static_cast<unsigned char>(c)) || c == '_'; }
	static bool IsIdentChar(char c) { return std::isalnum(static_cast<unsigned char>(c)) || c == '_' || c == '-'; }
}

std::vector<Token> Lexer::Tokenize(const std::string& s) const {
	std::vector<Token> out; std::size_t i = 0; int line = 1; int col = 1;
	auto emit = [&](TokenType t, std::size_t st, std::size_t ed, const std::string& lx) { out.push_back(Token{t, lx, st, ed, line, col}); };
	while (i < s.size()) {
		char c = s[i];
		if (c == '\n') { ++i; ++line; col = 1; continue; }
		if (std::isspace(static_cast<unsigned char>(c))) { ++i; ++col; continue; }
		std::size_t start = i;
		if (c == '{' && i + 1 < s.size() && s[i+1] == '{') { i += 2; emit(TokenType::DoubleLBrace, start, i, "{{"); continue; }
		if (c == '}' && i + 1 < s.size() && s[i+1] == '}') { i += 2; emit(TokenType::DoubleRBrace, start, i, "}}"); continue; }
		if (c == '-' && i + 1 < s.size() && s[i+1] == '>') { i += 2; emit(TokenType::Arrow, start, i, "->"); continue; }
		switch (c) {
			case '{': ++i; emit(TokenType::LBrace, start, i, "{"); continue;
			case '}': ++i; emit(TokenType::RBrace, start, i, "}"); continue;
			case '(': ++i; emit(TokenType::LParen, start, i, "("); continue;
			case ')': ++i; emit(TokenType::RParen, start, i, ")"); continue;
			case '[': ++i; emit(TokenType::LBracket, start, i, "["); continue;
			case ']': ++i; emit(TokenType::RBracket, start, i, "]"); continue;
			case ':': ++i; emit(TokenType::Colon, start, i, ":"); continue;
			case ';': ++i; emit(TokenType::Semi, start, i, ";"); continue;
			case ',': ++i; emit(TokenType::Comma, start, i, ","); continue;
			case '.': ++i; emit(TokenType::Dot, start, i, "."); continue;
			case '\'': { ++i; while (i < s.size()) { if (s[i] == '\\') { i += 2; continue; } if (s[i] == '\'') { ++i; break; } ++i; } emit(TokenType::String, start, i, std::string()); continue; }
			case '"': { ++i; while (i < s.size()) { if (s[i] == '\\') { i += 2; continue; } if (s[i] == '"') { ++i; break; } ++i; } emit(TokenType::String, start, i, std::string()); continue; }
		}
		if (IsIdentStart(c)) {
			++i; while (i < s.size() && IsIdentChar(s[i])) ++i; std::string id(s.begin() + start, s.begin() + i);
			TokenType t = TokenType::Identifier;
			if (id == "vir") t = TokenType::KeywordVir;
			else if (id == "listen") t = TokenType::KeywordListen;
			else if (id == "delegate") t = TokenType::KeywordDelegate;
			else if (id == "animate") t = TokenType::KeywordAnimate;
			emit(t, start, i, id);
			continue;
		}
		++i; ++col; // skip unknown
	}
	out.push_back(Token{TokenType::EndOfFile, std::string(), s.size(), s.size(), line, col});
	return out;
}