#pragma once

#include <string>
#include "Core/CompilationResult.h"

class ResultMerger {
public:
	static std::string MergeToHtml(const CompilationResult& result);
};