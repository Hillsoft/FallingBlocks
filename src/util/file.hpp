#pragma once

#include <filesystem>
#include <vector>

namespace util {

std::vector<char> readFileBytes(const std::filesystem::path& path);

}
