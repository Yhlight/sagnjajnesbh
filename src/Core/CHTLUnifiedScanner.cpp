#include "CHTLUnifiedScanner.h"
#include <cctype>
#include <string>

namespace {
	static inline bool IsIdentifierStart(char c) {
		return std::isalpha(static_cast<unsigned char>(c)) || c == '_' || c == '@' || c == '#';
	}

	static inline bool IsIdentifierChar(char c) {
		return std::isalnum(static_cast<unsigned char>(c)) || c == '_' || c == '-' || c == '@' || c == '#';
	}

	static void SkipLineComment(const std::string& s, std::size_t& i) {
		// assumes s[i] == '/' and s[i+1] == '/'
		i += 2;
		while (i < s.size() && s[i] != '\n') ++i;
	}

	static void SkipBlockComment(const std::string& s, std::size_t& i) {
		// assumes s[i] == '/' and s[i+1] == '*'
		i += 2;
		while (i + 1 < s.size()) {
			if (s[i] == '*' && s[i+1] == '/') { i += 2; break; }
			++i;
		}
	}

	static void SkipSpaces(const std::string& s, std::size_t& i) {
		while (i < s.size()) {
			if (std::isspace(static_cast<unsigned char>(s[i]))) { ++i; continue; }
			if (i + 1 < s.size() && s[i] == '/' && s[i+1] == '/') { SkipLineComment(s, i); continue; }
			if (i + 1 < s.size() && s[i] == '/' && s[i+1] == '*') { SkipBlockComment(s, i); continue; }
			break;
		}
	}

	static std::string ReadIdentifier(const std::string& s, std::size_t& i) {
		std::size_t start = i;
		if (i < s.size() && IsIdentifierStart(s[i])) {
			++i;
			while (i < s.size() && IsIdentifierChar(s[i])) ++i;
		}
		return std::string(s.begin() + start, s.begin() + i);
	}

	static void SkipString(const std::string& s, std::size_t& i) {
		char quote = s[i++];
		while (i < s.size()) {
			char c = s[i++];
			if (c == '\\' && i < s.size()) { ++i; continue; }
			if (c == quote) break;
		}
	}

	static bool ReadBraceBlock(const std::string& s, std::size_t& i, std::size_t& contentStart, std::size_t& contentEnd) {
		// assumes s[i] == '{'
		std::size_t startBrace = i;
		int depth = 0;
		bool inString = false;
		char stringQuote = '\0';
		contentStart = std::string::npos;
		for (; i < s.size(); ++i) {
			char c = s[i];
			if (!inString) {
				if (c == '"' || c == '\'') { inString = true; stringQuote = c; continue; }
				if (i + 1 < s.size() && s[i] == '/' && s[i+1] == '/') { SkipLineComment(s, i); if (i >= s.size()) break; continue; }
				if (i + 1 < s.size() && s[i] == '/' && s[i+1] == '*') { SkipBlockComment(s, i); if (i >= s.size()) break; --i; continue; }
				if (c == '{') { if (depth == 0) contentStart = i + 1; ++depth; }
				else if (c == '}') { --depth; if (depth == 0) { contentEnd = i; ++i; return true; } }
			} else {
				if (c == '\\' && i + 1 < s.size()) { ++i; continue; }
				if (c == stringQuote) { inString = false; stringQuote = '\0'; }
			}
		}
		// not closed
		contentStart = (startBrace + 1 < s.size() ? startBrace + 1 : startBrace);
		contentEnd = s.size();
		return false;
	}

	static bool StartsWithKeyword(const std::string& s, std::size_t i, const char* kw) {
		std::size_t k = 0;
		while (kw[k] != '\0') {
			if (i + k >= s.size() || s[i + k] != kw[k]) return false;
			++k;
		}
		// ensure boundary
		if (i + k < s.size()) {
			char c = s[i + k];
			if (IsIdentifierChar(c)) return false;
		}
		return true;
	}

	static bool IsCHTLJSCode(const std::string& t) {
		if (t.find("{{") != std::string::npos) return true;
		if (t.find("->") != std::string::npos) return true;
		if (t.find("vir ") != std::string::npos || t.rfind("vir\n", 0) == 0) return true;
		if (t.find("listen(") != std::string::npos) return true;
		if (t.find("delegate(") != std::string::npos) return true;
		if (t.find("animate(") != std::string::npos) return true;
		return false;
	}

	static bool IEquals(const std::string& a, const std::string& b) {
		if (a.size() != b.size()) return false;
		for (std::size_t i = 0; i < a.size(); ++i) {
			if (std::tolower(static_cast<unsigned char>(a[i])) != std::tolower(static_cast<unsigned char>(b[i]))) return false;
		}
		return true;
	}
}

std::vector<Segment> CHTLUnifiedScanner::SliceSegments(const std::string& source) const {
	std::vector<Segment> segments;
	std::size_t i = 0;
	std::size_t lastCHTLStart = 0;

	auto flushCHTL = [&](std::size_t endExclusive) {
		if (endExclusive > lastCHTLStart) {
			segments.emplace_back(SegmentType::CHTL,
				std::string(source.begin() + lastCHTLStart, source.begin() + endExclusive),
				lastCHTLStart, endExclusive);
		}
		lastCHTLStart = endExclusive;
	};

	while (i < source.size()) {
		if (i + 1 < source.size() && source[i] == '/' && source[i+1] == '/') { SkipLineComment(source, i); continue; }
		if (i + 1 < source.size() && source[i] == '/' && source[i+1] == '*') { SkipBlockComment(source, i); continue; }
		if (source[i] == '"' || source[i] == '\'') { SkipString(source, i); continue; }

		if (source[i] == '[') {
			std::size_t j = i + 1;
			SkipSpaces(source, j);
			std::string tag;
			while (j < source.size() && source[j] != ']' && !std::isspace(static_cast<unsigned char>(source[j]))) {
				tag.push_back(source[j]);
				++j;
			}
			// 跳到 ']' 之后
			while (j < source.size() && source[j] != ']') ++j;
			if (j < source.size() && source[j] == ']') ++j;

			if (IEquals(tag, "Origin")) {
				SkipSpaces(source, j);
				if (j < source.size() && source[j] == '@') {
					// 读取类型名 @Type
					std::size_t tStart = j + 1;
					std::size_t t = tStart;
					while (t < source.size() && IsIdentifierChar(source[t])) ++t;
					std::string typeName = std::string(source.begin() + tStart, source.begin() + t);
					SkipSpaces(source, t);
					// 可选名称
					if (t < source.size() && (IsIdentifierStart(source[t]) || source[t] == '"' || source[t] == '\'')) {
						if (source[t] == '"' || source[t] == '\'') { SkipString(source, t); }
						else { (void)ReadIdentifier(source, t); }
						SkipSpaces(source, t);
					}
					if (t < source.size() && source[t] == '{') {
						// 切片：在块出现之前刷新CHTL
						flushCHTL(i);
						std::size_t contentStart = 0, contentEnd = 0;
						ReadBraceBlock(source, t, contentStart, contentEnd);
						std::string inner(source.begin() + contentStart, source.begin() + contentEnd);
						SegmentType segType = SegmentType::OriginCustom;
						if (IEquals(typeName, "Html")) segType = SegmentType::OriginHtml;
						else if (IEquals(typeName, "Style")) segType = SegmentType::OriginStyle;
						else if (IEquals(typeName, "JavaScript") || IEquals(typeName, "Javascript")) segType = SegmentType::OriginJavaScript;
						Segment seg(segType, std::move(inner), contentStart, contentEnd);
						if (segType == SegmentType::OriginCustom) seg.originTypeName = typeName;
						segments.emplace_back(std::move(seg));
						lastCHTLStart = t; // 块结束位置后的索引已在ReadBraceBlock中移动
						i = t; // 继续从块后扫描
						continue;
					}
				}
			}
		}

		// script 块
		if (StartsWithKeyword(source, i, "script")) {
			std::size_t j = i + 6;
			SkipSpaces(source, j);
			if (j < source.size() && source[j] == '{') {
				flushCHTL(i);
				std::size_t contentStart = 0, contentEnd = 0;
				ReadBraceBlock(source, j, contentStart, contentEnd);
				std::string inner(source.begin() + contentStart, source.begin() + contentEnd);
				if (IsCHTLJSCode(inner)) {
					segments.emplace_back(SegmentType::CHTLJS, std::move(inner), contentStart, contentEnd);
				} else {
					segments.emplace_back(SegmentType::JS, std::move(inner), contentStart, contentEnd);
				}
				lastCHTLStart = j; // 块开始处被切掉，lastCHTLStart移至块结束
				i = j; // 已移动到块后
				continue;
			}
		}

		// style 块
		if (StartsWithKeyword(source, i, "style")) {
			std::size_t j = i + 5;
			SkipSpaces(source, j);
			if (j < source.size() && source[j] == '{') {
				flushCHTL(i);
				std::size_t contentStart = 0, contentEnd = 0;
				ReadBraceBlock(source, j, contentStart, contentEnd);
				std::string inner(source.begin() + contentStart, source.begin() + contentEnd);
				segments.emplace_back(SegmentType::CSS, std::move(inner), contentStart, contentEnd);
				lastCHTLStart = j;
				i = j;
				continue;
			}
		}

		++i;
	}

	flushCHTL(source.size());
	return segments;
}