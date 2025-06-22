#pragma once

#include <cstddef>
#include <filesystem>
#include <span>
#include "loader/Image.hpp"

namespace blocks::loader {

Image loadPng(const std::filesystem::path& path);
Image loadPng(std::span<const std::byte> data);

} // namespace blocks::loader
