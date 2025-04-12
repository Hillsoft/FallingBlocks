#include "loader/Config.hpp"

#include <filesystem>
#include <stdexcept>
#include <string_view>
#include <type_traits>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include "util/file.hpp"
#include "util/string.hpp"

namespace blocks::loader {

namespace {

std::unordered_map<std::string_view, std::vector<std::string_view>> getPairs(
    const std::vector<char>& rawData) {
  std::unordered_map<std::string_view, std::vector<std::string_view>> result;

  std::string_view remaining{rawData.begin(), rawData.end()};

  while (remaining.size() > 0) {
    size_t lineEnd = remaining.find('\n');
    std::string_view line = lineEnd != std::string_view::npos
        ? remaining.substr(0, lineEnd)
        : remaining;
    remaining =
        lineEnd != std::string_view::npos ? remaining.substr(lineEnd + 1) : "";

    if (line.size() == 0) {
      // Empty line
      continue;
    }
    if (line[0] == '#') {
      // Comment
      continue;
    }

    size_t split = line.find('=');
    if (split == std::string_view::npos) {
      throw std::runtime_error{
          util::toString("Improperly formatted line: '", line, "'")};
    }

    std::string_view key = line.substr(0, split);
    if (result.contains(key)) {
      throw std::runtime_error{util::toString("Duplicate key: '", key, "'")};
    }

    std::vector<std::string_view> values;
    std::string_view valuesList = line.substr(split + 1);

    size_t valueSplit;
    while ((valueSplit = valuesList.find(';')) != std::string_view::npos) {
      values.emplace_back(valuesList.substr(0, valueSplit));
      valuesList = valuesList.substr(valueSplit + 1);
    }
    values.emplace_back(valuesList);

    result.emplace(key, std::move(values));
  }

  return result;
}

template <typename T, typename FnFound, typename FnMissing>
T readKey(
    const std::unordered_map<std::string_view, std::vector<std::string_view>>&
        keyValuePairs,
    std::unordered_set<std::string_view>& foundKeys,
    std::string_view key,
    FnFound&& foundHandler,
    FnMissing&& missingHandler) {
  auto value = keyValuePairs.find(key);
  if (value == keyValuePairs.end()) {
    return missingHandler();
  }

  T result = foundHandler(value->second);

  if (!foundKeys.contains(value->first)) {
    foundKeys.insert(value->first);
  }

  return result;
}

template <typename T, typename FnFound>
T readKey(
    const std::unordered_map<std::string_view, std::vector<std::string_view>>&
        keyValuePairs,
    std::unordered_set<std::string_view>& foundKeys,
    std::string_view key,
    FnFound&& foundHandler) {
  return readKey<T>(
      keyValuePairs,
      foundKeys,
      key,
      std::forward<FnFound>(foundHandler),
      [&]() -> T {
        throw std::runtime_error{util::toString("Missing key: '", key, "'")};
      });
}

std::string_view extractString(
    std::string_view key, const std::vector<std::string_view>& valList) {
  if (valList.size() != 1) {
    throw std::runtime_error{util::toString(
        "Type mismatch for key '", key, "'; expected string but got list")};
  }
  return valList[0];
}

long parseInt(std::string_view str) {
  if (str.size() == 0) {
    throw std::runtime_error{"Int parse error"};
  }

  long result = 0;
  long fac = 1;

  if (str[0] == '-') {
    fac = -1;
    str = str.substr(1);
  }

  for (const auto& c : str) {
    if (c < '0' || c > '9') {
      throw std::runtime_error{"Int parse error"};
    }
    result *= 10;
    result += c - '0';
  }

  return result * fac;
}

long extractInt(
    std::string_view key, const std::vector<std::string_view>& valList) {
  if (valList.size() != 1) {
    throw std::runtime_error{util::toString(
        "Type mismatch for key '", key, "'; expected integer but got list")};
  }
  try {
    return parseInt(valList[0]);
  } catch (const std::runtime_error& /* e */) {
    throw std::runtime_error{util::toString(
        "Type mismatch for key '", key, "'; expected integer but got string")};
  }
}

std::vector<long> extractIntList(
    std::string_view key, const std::vector<std::string_view>& valList) {
  std::vector<long> result;
  result.reserve(valList.size());

  try {
    for (const auto& val : valList) {
      result.emplace_back(parseInt(val));
    }
  } catch (const std::runtime_error& /* e */) {
    throw std::runtime_error{util::toString(
        "Type mismatch for key '", key, "'; expected integer but got string")};
  }

  return result;
}

} // namespace

Config::Config(const std::filesystem::path& path)
    : rawData_(util::readFileBytes(path)),
      keyValuePairs_(getPairs(rawData_)),
      foundKeys_() {}

Config::Config(std::vector<char> data)
    : rawData_(std::move(data)),
      keyValuePairs_(getPairs(rawData_)),
      foundKeys_() {}

std::string_view Config::readString(std::string_view key) {
  return readKey<std::string_view>(
      keyValuePairs_,
      foundKeys_,
      key,
      [&](const std::vector<std::string_view>& valList) {
        return extractString(key, valList);
      });
}

std::string_view Config::readStringWithDefault(
    std::string_view key, std::string_view fallback) {
  return readKey<std::string_view>(
      keyValuePairs_,
      foundKeys_,
      key,
      [&](const std::vector<std::string_view>& valList) {
        return extractString(key, valList);
      },
      [&]() { return fallback; });
}

long Config::readInt(std::string_view key) {
  return readKey<long>(
      keyValuePairs_,
      foundKeys_,
      key,
      [&](const std::vector<std::string_view>& valList) {
        return extractInt(key, valList);
      });
}

long Config::readIntWithDefault(std::string_view key, long fallback) {
  return readKey<long>(
      keyValuePairs_,
      foundKeys_,
      key,
      [&](const std::vector<std::string_view>& valList) {
        return extractInt(key, valList);
      },
      [&]() { return fallback; });
}

std::vector<std::string_view> Config::readStringList(std::string_view key) {
  return readKey<std::vector<std::string_view>>(
      keyValuePairs_,
      foundKeys_,
      key,
      [](const std::vector<std::string_view>& valList) { return valList; });
}

std::vector<std::string_view> Config::readStringListWithDefault(
    std::string_view key, std::vector<std::string_view> fallback) {
  return readKey<std::vector<std::string_view>>(
      keyValuePairs_,
      foundKeys_,
      key,
      [](const std::vector<std::string_view>& valList) { return valList; },
      [&]() { return fallback; });
}

std::vector<long> Config::readIntList(std::string_view key) {
  return readKey<std::vector<long>>(
      keyValuePairs_,
      foundKeys_,
      key,
      [&](const std::vector<std::string_view>& valList) {
        return extractIntList(key, valList);
      });
}

std::vector<long> Config::readIntListWithDefault(
    std::string_view key, std::vector<long> fallback) {
  return readKey<std::vector<long>>(
      keyValuePairs_,
      foundKeys_,
      key,
      [&](const std::vector<std::string_view>& valList) {
        return extractIntList(key, valList);
      },
      [&]() { return fallback; });
}

} // namespace blocks::loader
