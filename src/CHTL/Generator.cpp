#include "Generator.h"
#include <sstream>

using namespace CHTLNS;

namespace {
	static void EmitNode(const Document* doc, const Node* n, std::ostringstream& html, std::ostringstream& styles);

	static std::string ExpandLocalStyle(const Document* doc, const std::string& content) {
		// 简化：扫描"/*CHTL_STYLE_REF:NAME*/" 并按模板属性展开为 "prop: value;" 列表
		std::ostringstream out;
		std::size_t i = 0;
		while (i < content.size()) {
			if (i + 17 < content.size() && content.compare(i, 17, "/*CHTL_STYLE_REF:") == 0) {
				std::size_t j = i + 17;
				std::string name;
				while (j + 1 < content.size() && !(content[j] == '*' && content[j+1] == '/')) { name.push_back(content[j]); ++j; }
				if (j + 1 < content.size()) j += 2; // 跳过*/
				auto it = doc->styleTemplates.find(name);
				if (it != doc->styleTemplates.end()) {
					for (const auto& p : it->second->properties) {
						out << p.name << ": " << p.value << ";\n";
					}
				}
				i = j; continue;
			}
			out << content[i++];
		}
		return out.str();
	}

	static void EmitElement(const Document* doc, const ElementNode* el, std::ostringstream& html, std::ostringstream& styles) {
		html << "<" << el->name;
		for (const auto& a : el->attributes) {
			html << " " << a.name << "=\"" << a.value << "\"";
		}
		html << ">";
		for (auto* s : el->localStyles) {
			styles << ExpandLocalStyle(doc, s->content) << "\n";
		}
		for (auto* ch : el->children) EmitNode(doc, ch, html, styles);
		html << "</" << el->name << ">";
	}

	static void EmitNode(const Document* doc, const Node* n, std::ostringstream& html, std::ostringstream& styles) {
		if (auto* t = dynamic_cast<const TextNode*>(n)) { html << t->text; return; }
		if (auto* e = dynamic_cast<const ElementNode*>(n)) { EmitElement(doc, e, html, styles); return; }
	}
}

GenerateResult Generator::Generate(const Document* doc) {
	std::ostringstream html;
	std::ostringstream styles;
	for (auto* n : doc->children) {
		EmitNode(doc, n, html, styles);
	}
	return GenerateResult{ html.str(), styles.str() };
}