#include "AST.h"

using namespace CHTLNS;

ElementNode::~ElementNode() {
	for (auto* n : children) delete n;
	for (auto* s : localStyles) delete s;
}

Document::~Document() {
	for (auto* n : children) delete n;
	for (auto& kv : styleTemplates) delete kv.second;
}