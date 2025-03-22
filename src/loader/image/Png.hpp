#pragma once

#include <filesystem>
#include <span>
#include "loader/Image.hpp"

namespace blocks::loader {

Image loadPng(const std::filesystem::path& path);
Image loadPng(std::span<const unsigned char> data);

} // namespace blocks::loader
