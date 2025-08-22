#include "Lexer.h"
#include <cctype>

using namespace CHTLNS;

namespace {
	static bool IsIdentStart(char c) {
		return std::isalpha(static_cast<unsigned char>(c)) || c == '_';
	}
	static bool IsIdentChar(char c) {
		return std::isalnum(static_cast<unsigned char>(c)) || c == '_' || c == '-';
	}
	static void SkipLine(const std::string& s, std::size_t& i) {
		i += 2; while (i < s.size() && s[i] != '\n') ++i;
	}
	static void SkipBlock(const std::string& s, std::size_t& i) {
		i += 2; while (i + 1 < s.size()) { if (s[i] == '*' && s[i+1] == '/') { i += 2; break; } ++i; }
	}
	static TokenType KeywordOf(const std::string& id) {
		if (id == "text") return TokenType::KeywordText;
		if (id == "style") return TokenType::KeywordStyle;
		if (id == "script") return TokenType::KeywordScript;
		if (id == "from") return TokenType::KeywordFrom;
		if (id == "as") return TokenType::KeywordAs;
		if (id == "except") return TokenType::KeywordExcept;
		if (id == "inherit") return TokenType::KeywordInherit;
		if (id == "delete") return TokenType::KeywordDelete;
		if (id == "insert") return TokenType::KeywordInsert;
		return TokenType::Identifier;
	}
}

std::vector<Token> Lexer::Tokenize(const std::string& s) const {
	std::vector<Token> out;
	std::size_t i = 0; int line = 1; int col = 1;
	auto emit = [&](TokenType t, std::size_t start, std::size_t end, const std::string& lex) {
		out.push_back(Token{t, lex, start, end, line, col});
	};
	while (i < s.size()) {
		char c = s[i];
		if (c == '\n') { ++i; ++line; col = 1; continue; }
		if (std::isspace(static_cast<unsigned char>(c))) { ++i; ++col; continue; }
		if (i + 1 < s.size() && s[i] == '/' && s[i+1] == '/') { SkipLine(s, i); continue; }
		if (i + 1 < s.size() && s[i] == '/' && s[i+1] == '*') { SkipBlock(s, i); continue; }
		std::size_t start = i;
		switch (c) {
			case '{': ++i; emit(TokenType::LBrace, start, i, "{"); ++col; continue;
			case '}': ++i; emit(TokenType::RBrace, start, i, "}"); ++col; continue;
			case '[': ++i; emit(TokenType::LBracket, start, i, "["); ++col; continue;
			case ']': ++i; emit(TokenType::RBracket, start, i, "]"); ++col; continue;
			case '(': ++i; emit(TokenType::LParen, start, i, "("); ++col; continue;
			case ')': ++i; emit(TokenType::RParen, start, i, ")"); ++col; continue;
			case ':': ++i; emit(TokenType::Colon, start, i, ":"); ++col; continue;
			case ';': ++i; emit(TokenType::Semi, start, i, ";"); ++col; continue;
			case ',': ++i; emit(TokenType::Comma, start, i, ","); ++col; continue;
			case '.': ++i; emit(TokenType::Dot, start, i, "."); ++col; continue;
			case '=': ++i; emit(TokenType::Equal, start, i, "="); ++col; continue;
			case '@': ++i; emit(TokenType::At, start, i, "@"); ++col; continue;
			case '\'': {
				++i; while (i < s.size()) { if (s[i] == '\\') { i += 2; continue; } if (s[i] == '\'') { ++i; break; } ++i; }
				emit(TokenType::String, start, i, std::string()); ++col; continue; }
			case '"': {
				++i; while (i < s.size()) { if (s[i] == '\\') { i += 2; continue; } if (s[i] == '"') { ++i; break; } ++i; }
				emit(TokenType::String, start, i, std::string()); ++col; continue; }
		}
		if (IsIdentStart(c)) {
			++i; while (i < s.size() && IsIdentChar(s[i])) ++i;
			std::string id(s.begin() + start, s.begin() + i);
			emit(KeywordOf(id), start, i, id);
			continue;
		}
		// 未知字符，跳过
		++i; ++col;
	}
	out.push_back(Token{TokenType::EndOfFile, std::string(), s.size(), s.size(), line, col});
	return out;
}