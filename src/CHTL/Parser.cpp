#include "Parser.h"
#include <cctype>

using namespace CHTLNS;

namespace {
	struct Cursor {
		const std::vector<Token>& tokens;
		std::size_t i = 0;
		const Token& peek() const { return tokens[i]; }
		bool eof() const { return tokens[i].type == TokenType::EndOfFile; }
		const Token& advance() { return tokens[i++]; }
		bool match(TokenType t) { if (peek().type == t) { ++i; return true; } return false; }
	};

	bool isIdentifier(const Token& t) {
		return t.type == TokenType::Identifier || t.type == TokenType::KeywordText || t.type == TokenType::KeywordStyle || t.type == TokenType::KeywordScript;
	}

	std::string tokenLexeme(const Token& t) {
		return t.lexeme;
	}
}

static ElementNode* parseElement(Cursor& c, Document* doc);
static Node* parseNode(Cursor& c, Document* doc);
static Node* parseStyleBlock(Cursor& c, Document* doc);

static void parseAttributes(Cursor& c, ElementNode* el) {
	while (isIdentifier(c.peek())) {
		Token nameTok = c.advance();
		if (!(c.match(TokenType::Colon) || c.match(TokenType::Equal))) break;
		std::string val;
		Token vt = c.peek();
		if (vt.type == TokenType::String || vt.type == TokenType::Identifier) {
			val = tokenLexeme(c.advance());
		}
		if (c.peek().type == TokenType::Semi) c.advance();
		el->attributes.push_back(Attribute{tokenLexeme(nameTok), val});
	}
}

static StyleTemplateDef* parseStyleTemplate(Cursor& c) {
	// 期待形如：[Template] @Style Name { prop: val; ... }
	if (!c.match(TokenType::LBracket)) return nullptr;
	Token kw = c.advance(); // KeywordTemplate
	if (kw.type != TokenType::KeywordTemplate) return nullptr;
	if (!c.match(TokenType::RBracket)) return nullptr;
	if (!c.match(TokenType::At)) return nullptr;
	Token sty = c.advance();
	if (sty.type != TokenType::KeywordStyle && sty.type != TokenType::Identifier) return nullptr;
	Token nameTok = c.advance(); // 模板名
	if (nameTok.type != TokenType::Identifier) return nullptr;
	if (!c.match(TokenType::LBrace)) return nullptr;
	auto* def = new StyleTemplateDef(); def->name = tokenLexeme(nameTok);
	// 读取属性列表直到'}'
	while (!c.eof() && c.peek().type != TokenType::RBrace) {
		// 支持组合/继承：@Style Other; 或 inherit @Style Other;
		if (c.peek().type == TokenType::At) {
			c.advance();
			Token kw = c.advance();
			if (kw.type == TokenType::KeywordStyle || kw.type == TokenType::Identifier) {
				Token nm = c.advance();
				if (nm.type == TokenType::Identifier) { def->includeTemplates.push_back(tokenLexeme(nm)); }
				if (c.peek().type == TokenType::Semi) c.advance();
				continue;
			}
		}
		if (c.peek().type == TokenType::KeywordInherit) {
			c.advance();
			if (c.match(TokenType::At)) {
				Token kw2 = c.advance();
				if (kw2.type == TokenType::KeywordStyle || kw2.type == TokenType::Identifier) {
					Token nm = c.advance();
					if (nm.type == TokenType::Identifier) { def->includeTemplates.push_back(tokenLexeme(nm)); }
					if (c.peek().type == TokenType::Semi) c.advance();
					continue;
				}
			}
		}
		Token prop = c.advance();
		if (prop.type != TokenType::Identifier) break;
		if (!(c.match(TokenType::Colon) || c.match(TokenType::Equal))) break;
		Token val = c.advance();
		std::string v;
		if (val.type == TokenType::String || val.type == TokenType::Identifier) v = tokenLexeme(val);
		if (c.peek().type == TokenType::Semi) c.advance();
		def->properties.push_back(Attribute{ tokenLexeme(prop), v });
	}
	c.match(TokenType::RBrace);
	return def;
}

static Node* parseStyleBlock(Cursor& c, Document* doc) {
	if (c.peek().type == TokenType::KeywordStyle) {
		c.advance();
		if (!c.match(TokenType::LBrace)) return nullptr;
		std::string content;
		int depth = 1;
		while (!c.eof() && depth > 0) {
			// 支持 @Style Name; 使用点的捕获：简化为原样记入内容，生成器阶段展开
			Token t = c.advance();
			if (t.type == TokenType::At) {
				Token nxt = c.advance();
				if (nxt.type == TokenType::KeywordStyle || nxt.type == TokenType::Identifier) {
					Token nm = c.advance();
					if (nm.type == TokenType::Identifier) {
						content += "/*CHTL_STYLE_REF:"; content += nm.lexeme; content += "*/";
						if (c.peek().type == TokenType::Semi) c.advance();
						continue;
					}
				}
				content += "@";
				content += nxt.lexeme;
				continue;
			}
			if (t.type == TokenType::LBrace) { content += '{'; ++depth; }
			else if (t.type == TokenType::RBrace) { --depth; if (depth == 0) break; content += '}'; }
			else if (t.type == TokenType::String) { content += '"'; content += '"'; }
			else { content += t.lexeme; }
		}
		auto* s = new StyleBlockNode(); s->content = std::move(content); return s;
	}
	return nullptr;
}

static Node* parseTextBlock(Cursor& c) {
	if (c.peek().type == TokenType::KeywordText) {
		c.advance(); if (!c.match(TokenType::LBrace)) return nullptr;
		std::string txt; while (!c.eof() && c.peek().type != TokenType::RBrace) { Token t = c.advance(); if (t.type == TokenType::String) { txt += '"'; txt += '"'; } else txt += t.lexeme; }
		c.match(TokenType::RBrace);
		auto* tn = new TextNode(); tn->text = std::move(txt); return tn;
	}
	return nullptr;
}

static Node* parseNode(Cursor& c, Document* doc) {
	if (c.peek().type == TokenType::LBracket) {
		std::size_t save = c.i;
		StyleTemplateDef* def = parseStyleTemplate(c);
		if (def) { return def; }
		c.i = save;
	}
	if (c.peek().type == TokenType::KeywordText) return parseTextBlock(c);
	if (c.peek().type == TokenType::KeywordStyle) return parseStyleBlock(c, doc);
	if (isIdentifier(c.peek())) return parseElement(c, doc);
	return nullptr;
}

static ElementNode* parseElement(Cursor& c, Document* doc) {
	Token nameTok = c.advance();
	auto* el = new ElementNode(); el->name = tokenLexeme(nameTok);
	if (!c.match(TokenType::LBrace)) return el;
	parseAttributes(c, el);
	while (!c.eof() && c.peek().type != TokenType::RBrace) {
		if (c.peek().type == TokenType::KeywordStyle) {
			Node* n = parseStyleBlock(c, doc);
			if (auto* sb = dynamic_cast<StyleBlockNode*>(n)) el->localStyles.push_back(sb);
			else if (n) el->children.push_back(n);
			continue;
		}
		Node* child = parseNode(c, doc);
		if (child) {
			if (auto* st = dynamic_cast<StyleTemplateDef*>(child)) { /* 不应出现在元素内 */ delete st; }
			else el->children.push_back(child);
		}
		else break;
	}
	c.match(TokenType::RBrace);
	return el;
}

Document* Parser::Parse(const std::string& source) {
	Lexer lx; auto toks = lx.Tokenize(source);
	Cursor c{toks, 0};
	auto* doc = new Document();
	while (!c.eof()) {
		// 模板定义放到顶层
		if (c.peek().type == TokenType::LBracket) {
			std::size_t save = c.i;
			if (StyleTemplateDef* def = parseStyleTemplate(c)) { doc->styleTemplates[def->name] = def; continue; }
			c.i = save;
		}
		Node* n = parseNode(c, doc);
		if (n) doc->children.push_back(n);
		else break;
	}
	return doc;
}