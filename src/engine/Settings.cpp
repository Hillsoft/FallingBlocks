#include "engine/Settings.hpp"

#include <Windows.h>
#include <shlobj_core.h>
#include <filesystem>
#include <iostream>
#include <string_view>
#include <vector>
#include "serialization/Serialization.hpp"
#include "serialization/yaml/YAMLSerializationProvider.hpp"
#include "util/file.hpp"

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
    std::cout << "Failed to get AppData path for settings\n";
    return defaultSettings();
  }

  const std::filesystem::path settingsPath =
      std::filesystem::path{path} / "Blocks" / "engineConfig.yaml";
  CoTaskMemFree(path);

  if (!std::filesystem::exists(settingsPath)) {
    std::cout << "Failed to load settings from " << settingsPath << "\n";
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
