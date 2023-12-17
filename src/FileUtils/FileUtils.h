#pragma once

#include <filesystem>
#include <string_view>

namespace FileUtils {

std::string Read(const std::filesystem::path & path);
void Write(const std::filesystem::path & path, const std::string_view contents);
void Append(const std::filesystem::path & path, const std::string_view contents);

}