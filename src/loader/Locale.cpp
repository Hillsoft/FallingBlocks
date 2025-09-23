#include "loader/Locale.hpp"

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <span>
#include <string>
#include <unordered_map>
#include <utility>
#include "util/file.hpp"

namespace blocks::loader {

Locale loadLocale(const std::filesystem::path& path) {
  return loadLocale(util::readFileBytes(path));
}

Locale loadLocale(std::span<const std::byte> dataBytes) {
  if (dataBytes.size() >= 3 && static_cast<uint8_t>(dataBytes[0]) == 0xEF &&
      static_cast<uint8_t>(dataBytes[1]) == 0xBB &&
      static_cast<uint8_t>(dataBytes[2]) == 0xBF) {
    dataBytes = dataBytes.subspan(3);
  }

  std::span<const char> data{
      reinterpret_cast<const char*>(dataBytes.data()), dataBytes.size()};

  auto firstLineEnd = std::find(data.begin(), data.end(), '\n');
  std::string localeName{data.begin(), firstLineEnd};
  data = data.subspan(localeName.size() + 1);

  std::unordered_map<std::string, std::string> values;
  while (data.size() > 0) {
    auto keyEnd = std::find(data.begin(), data.end(), ':');
    std::string key{data.begin(), keyEnd};
    data = data.subspan(key.size() + 1);

    auto lineEnd = std::find(data.begin(), data.end(), '\n');
    std::string value{data.begin(), lineEnd};
    data = data.subspan(value.size() + 1);

    values.emplace(std::move(key), std::move(value));
  }

  return {std::move(localeName), std::move(values)};
}

} // namespace blocks::loader
