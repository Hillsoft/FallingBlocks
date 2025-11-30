#include "engine/Settings.hpp"

#include <Windows.h>
#include <shlobj_core.h>
#include <filesystem>
#include <string_view>
#include <vector>
#include "log/Logger.hpp"
#include "serialization/Serialization.hpp"
#include "serialization/yaml/YAMLSerializationProvider.hpp"
#include "util/file.hpp"
#include "util/string.hpp"

namespace blocks {

namespace {

Settings defaultSettings() {
  return {.localeCode = "en_GB"};
}

Settings loadSettings() {
  PWSTR path = nullptr;
  const HRESULT result =
      SHGetKnownFolderPath(FOLDERID_RoamingAppData, 0, nullptr, &path);

  if (result != S_OK) {
    log::LoggerSystem::logToDefault(
        log::LogLevel::WARNING, "Failed to get AppData path for settings");
    return defaultSettings();
  }

  const std::filesystem::path settingsPath =
      std::filesystem::path{path} / "Blocks" / "engineConfig.yaml";
  CoTaskMemFree(path);

  if (!std::filesystem::exists(settingsPath)) {
    log::LoggerSystem::logToDefault(
        log::LogLevel::WARNING,
        util::toString(
            "Failed to read settings file ", settingsPath.generic_string()));
    return defaultSettings();
  }

  std::vector<char> rawContents = util::readFileChars(settingsPath);
  return serialization::
      deserialize<Settings, serialization::yaml::YAMLSerializationProvider>(
          std::string_view{rawContents.begin(), rawContents.end()});
}

} // namespace

const Settings& getSettings() {
  const static Settings settings = loadSettings();
  return settings;
}

} // namespace blocks
