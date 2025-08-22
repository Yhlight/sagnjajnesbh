#pragma once

#include <string>
#include <vector>
#include <unordered_map>

namespace CHTLNS {
	struct Attribute {
		std::string name;
		std::string value;
	};

	struct Node {
		virtual ~Node() = default;
	};

	struct TextNode : public Node {
		std::string text;
	};

	struct StyleBlockNode : public Node {
		std::string content; // 原始样式内容（局部样式块内部文本）
	};

	struct ElementNode : public Node {
		std::string name;
		std::vector<Attribute> attributes;
		std::vector<Node*> children;
		std::vector<StyleBlockNode*> localStyles; // style {}
		~ElementNode();
	};

	struct StyleTemplateDef : public Node {
		std::string name;
		std::vector<Attribute> properties; // 简化：样式属性作为name:value
		std::vector<std::string> includeTemplates; // 组合/继承的父模板名称，按出现顺序
	};

	struct Document {
		std::vector<Node*> children;
		std::unordered_map<std::string, StyleTemplateDef*> styleTemplates; // 名称 -> 样式模板
		~Document();
	};
}