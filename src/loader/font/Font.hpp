#pragma once

#include <cstddef>
#include <filesystem>
#include <span>

namespace blocks::loader {

void loadFont(const std::filesystem::path& path);
void loadFont(std::span<const std::byte> data);

} // namespace blocks::loader
