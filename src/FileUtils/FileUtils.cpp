#include "FileUtils.h"

#include <cassert>
#include <fstream>
#include <iostream>

std::string FileUtils::Read(const std::filesystem::path & path)
{
	std::ifstream fileStream(path);
	if (!fileStream.is_open())
	{
		std::cerr << "Failed to open file: " << path << std::endl;
		assert(false && "Failed to open file");
		return "";
	}

	std::stringstream buffer;
	buffer << fileStream.rdbuf();
	return buffer.str();
}

void FileUtils::Write(const std::filesystem::path & path, const std::string_view contents)
{
	std::filesystem::path filePath(path);
	if (std::filesystem::path pathExcludingFileName(filePath.parent_path()); !std::filesystem::exists(pathExcludingFileName))
		std::filesystem::create_directories(pathExcludingFileName);

	std::ofstream fileStream(filePath);
	if (!fileStream.is_open())
	{
		std::cerr << "Failed to open file: " << filePath << std::endl;
		assert(false && "Failed to open file");
	}
	fileStream << contents;
}

void FileUtils::Append(const std::filesystem::path & path, const std::string_view contents)
{
	std::filesystem::path filePath(path);
	if (std::filesystem::path pathExcludingFileName(filePath.parent_path()); !std::filesystem::exists(pathExcludingFileName))
		std::filesystem::create_directories(pathExcludingFileName);

	std::ofstream fileStream(filePath, std::ios_base::app);
	if (!fileStream.is_open())
	{
		std::cerr << "Failed to open file: " << filePath << std::endl;
		assert(false && "Failed to open file");
	}

	fileStream << contents;
}
