#include "ResultMerger.h"

static inline bool hasNonEmpty(const std::string& s) {
	for (char c : s) {
		if (!isspace(static_cast<unsigned char>(c))) return true;
	}
	return false;
}

std::string ResultMerger::MergeToHtml(const CompilationResult& result) {
	std::string out;
	out.reserve(result.htmlOutput.size() + result.globalStyleOutput.size() + result.globalScriptOutput.size() + 256);

	out += "<!DOCTYPE html>\n<html>\n<head>\n<meta charset=\"utf-8\"/>\n";
	if (hasNonEmpty(result.globalStyleOutput)) {
		out += "<style>\n";
		out += result.globalStyleOutput;
		out += "\n</style>\n";
	}
	out += "</head>\n<body>\n";
	out += result.htmlOutput;
	if (hasNonEmpty(result.globalScriptOutput)) {
		out += "\n<script>\n";
		out += result.globalScriptOutput;
		out += "\n</script>\n";
	}
	out += "</body>\n</html>\n";
	return out;
}