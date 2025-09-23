#include "engine/Localisation.hpp"

#include <filesystem>
#include <string>
#include <string_view>
#include "loader/Locale.hpp"
#include "util/string.hpp"

namespace blocks {

Localisation::Localisation(std::string_view locale)
    : currentLocale_(loader::loadLocale(
          std::filesystem::path{RESOURCE_DIR "/localisation/"} /
          util::toString(locale, ".txt"))) {}

std::string Localisation::getLocalisedString(std::string_view key) {
  auto it = currentLocale_.strings.find(std::string{key});
  if (it != currentLocale_.strings.end()) {
    return it->second;
  }
  return std::string{key};
}

} // namespace blocks
