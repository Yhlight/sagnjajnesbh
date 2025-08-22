#include "Generator.h"
#include <sstream>
#include <unordered_set>

using namespace CHTLNS;

namespace {
	static void EmitNode(const Document* doc, const Node* n, std::ostringstream& html, std::ostringstream& styles);

	static void ExpandTemplateProps(const Document* doc, const std::string& name, std::vector<Attribute>& outProps, std::unordered_set<std::string>& visiting) {
		auto it = doc->styleTemplates.find(name);
		if (it == doc->styleTemplates.end()) return;
		if (!visiting.insert(name).second) return; // 防循环
		for (const auto& parent : it->second->includeTemplates) {
			ExpandTemplateProps(doc, parent, outProps, visiting);
		}
		// 自身属性，后写覆盖
		for (const auto& p : it->second->properties) {
			// 覆盖同名属性
			bool replaced = false;
			for (auto& exist : outProps) {
				if (exist.name == p.name) { exist.value = p.value; replaced = true; break; }
			}
			if (!replaced) outProps.push_back(p);
		}
	}

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
				std::vector<Attribute> props;
				std::unordered_set<std::string> visiting;
				ExpandTemplateProps(doc, name, props, visiting);
				for (const auto& p : props) { out << p.name << ": " << p.value << ";\n"; }
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