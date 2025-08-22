#pragma once

#include <string>
#include "AST.h"

namespace CHTLNS {
	struct GenerateResult {
		std::string html;
		std::string localStyle; // 收集局部样式块，供全局合并器提升
	};

	class Generator {
	public:
		GenerateResult Generate(const Document* doc);
	};
}