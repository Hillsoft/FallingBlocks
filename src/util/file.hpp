#pragma once

#include <filesystem>
#include <vector>

namespace util {

std::vector<std::byte> readFileBytes(const std::filesystem::path& path);
std::vector<char> readFileChars(const std::filesystem::path& path);

} // namespace util
