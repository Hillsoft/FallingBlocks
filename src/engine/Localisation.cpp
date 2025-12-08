#include "engine/Localisation.hpp"

#include <filesystem>
#include <string>
#include <string_view>
#include <utility>
#include <vector>
#include "engine/Settings.hpp"
#include "loader/Locale.hpp"
#include "util/string.hpp"

namespace blocks {

Localisation::Localisation(std::string locale)
    : localeCode_(std::move(locale)),
      currentLocale_(
          loader::loadLocale(
              std::filesystem::path("data") / "localisation" /
              util::toString(localeCode_, ".txt"))) {}

void Localisation::setLocale(std::string locale) {
  currentLocale_ = loader::loadLocale(
      std::filesystem::path("data") / "localisation" /
      util::toString(locale, ".txt"));
  localeCode_ = std::move(locale);

  Settings::update([&](Settings& settings) {
    settings.localeCode = localeCode_;
  });
}

// NOLINTNEXTLINE(readability-convert-member-functions-to-static)
std::vector<std::string> Localisation::getAvailableLocales() const {
  std::vector<std::string> result;

  for (const auto& dirEntry : std::filesystem::directory_iterator{
           std::filesystem::path("data") / "localisation"}) {
    result.push_back(dirEntry.path().filename().replace_extension().string());
  }

  return result;
}

std::string Localisation::getLocalisedString(std::string_view key) const {
  auto it = currentLocale_.strings.find(std::string{key});
  if (it != currentLocale_.strings.end()) {
    return it->second;
  }
  return std::string{key};
}

std::string_view Localisation::getLocaleCode() const {
  return localeCode_;
}

std::string_view Localisation::getLocaleName() const {
  return currentLocale_.name;
}

} // namespace blocks
