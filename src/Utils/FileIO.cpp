#include "FileIO.h"
#include <fstream>
#include <sstream>

bool FileIO::ReadTextFile(const std::string& path, std::string& outContent) {
	std::ifstream ifs(path, std::ios::in | std::ios::binary);
	if (!ifs) return false;
	std::ostringstream oss;
	oss << ifs.rdbuf();
	outContent = oss.str();
	return true;
}

bool FileIO::WriteTextFile(const std::string& path, const std::string& content) {
	std::ofstream ofs(path, std::ios::out | std::ios::binary);
	if (!ofs) return false;
	ofs << content;
	return true;
}