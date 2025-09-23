#pragma once

#include <cstddef>
#include <filesystem>
#include <span>
#include <string>
#include <unordered_map>

namespace blocks::loader {

struct Locale {
  std::string name;
  std::unordered_map<std::string, std::string> strings;
};

Locale loadLocale(std::span<const std::byte> data);
Locale loadLocale(const std::filesystem::path& path);

} // namespace blocks::loader
