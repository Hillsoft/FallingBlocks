#pragma once

#include <filesystem>
#include <vector>
#include "loader/Image.hpp"

namespace blocks::loader {

Image loadBitmap(const std::filesystem::path& path);
Image loadBitmap(const std::vector<char>& data);

} // namespace blocks::loader
