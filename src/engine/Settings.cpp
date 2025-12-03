#include "engine/Settings.hpp"

#include <Windows.h>
#include <shlobj_core.h>
#include <filesystem>
#include <fstream>
#include <stdexcept>
#include <string_view>
#include <vector>
#include "log/Logger.hpp"
#include "serialization/Serialization.hpp"
#include "serialization/yaml/YAMLSerializationProvider.hpp"
#include "util/file.hpp"
#include "util/string.hpp"

namespace blocks {

namespace {

std::filesystem::path getSettingsPath() {
  PWSTR path = nullptr;
  const HRESULT result =
      SHGetKnownFolderPath(FOLDERID_RoamingAppData, 0, nullptr, &path);

  if (result != S_OK) {
    log::LoggerSystem::logToDefault(
        log::LogLevel::WARNING, "Failed to get AppData path for settings");
    throw std::runtime_error{"Failed to get AppData path for settings"};
  }

  const std::filesystem::path settingsPath =
      std::filesystem::path{path} / "Blocks" / "engineConfig.yaml";
  CoTaskMemFree(path);
  return settingsPath;
}

Settings defaultSettings() {
  return {.localeCode = "en_GB"};
}

Settings loadSettings() {
  try {
    const std::filesystem::path settingsPath = getSettingsPath();

    if (!std::filesystem::exists(settingsPath)) {
      log::LoggerSystem::logToDefault(
          log::LogLevel::WARNING,
          util::toString(
              "Failed to read settings file ", settingsPath.generic_string()));
      return defaultSettings();
    }

    std::vector<char> rawContents = util::readFileChars(settingsPath);
    return serialization::
        deserialize<Settings, serialization::yaml::YAMLDeserializationProvider>(
            std::string_view{rawContents.begin(), rawContents.end()});
  } catch (...) {
    log::LoggerSystem::logToDefault(
        log::LogLevel::WARNING,
        "Failed to load settings, falling back to default");
    return defaultSettings();
  }
}

Settings& getSettingsStorage() {
  static Settings settings = loadSettings();
  return settings;
}

} // namespace

void Settings::save() const {
  try {
    const std::filesystem::path settingsPath = getSettingsPath();

    std::ofstream outStream{settingsPath};

    outStream << serialization::serialize<
        Settings,
        serialization::yaml::YAMLSerializationProvider>(*this);
  } catch (...) {
    log::LoggerSystem::logToDefault(
        log::LogLevel::WARNING, "Failed to save settings changes");
  }
}

Settings& Settings::getInternal() {
  return getSettingsStorage();
}

const Settings& getSettings() {
  return getSettingsStorage();
}

} // namespace blocks
