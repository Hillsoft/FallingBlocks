#pragma once

#include <filesystem>
#include <string_view>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace blocks::loader {

class Config {
 public:
  Config(const std::filesystem::path& path);
  Config(std::vector<char> rawData);

  std::string_view readString(std::string_view key);
  std::string_view readStringWithDefault(
      std::string_view key, std::string_view fallback);

  long readInt(std::string_view key);
  long readIntWithDefault(std::string_view key, long fallback);

  std::vector<std::string_view> readStringList(std::string_view key);
  std::vector<std::string_view> readStringListWithDefault(
      std::string_view key, std::vector<std::string_view> fallback);

  std::vector<long> readIntList(std::string_view key);
  std::vector<long> readIntListWithDefault(
      std::string_view key, std::vector<long> fallback);

 private:
  std::vector<char> rawData_;
  std::unordered_map<std::string_view, std::vector<std::string_view>>
      keyValuePairs_;
  std::unordered_set<std::string_view> foundKeys_;
};

} // namespace blocks::loader
