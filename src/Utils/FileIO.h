#pragma once

#include <string>

class FileIO {
public:
	static bool ReadTextFile(const std::string& path, std::string& outContent);
	static bool WriteTextFile(const std::string& path, const std::string& content);
};