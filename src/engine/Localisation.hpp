#pragma once

#include <string>
#include <string_view>
#include <vector>
#include "loader/Locale.hpp"

namespace blocks {

class Localisation {
 public:
  Localisation(std::string locale);

  void setLocale(std::string locale);
  std::vector<std::string> getAvailableLocales() const;

  std::string getLocalisedString(std::string_view key);
  std::string_view getLocaleCode() const;
  std::string_view getLocaleName() const;

 private:
  std::string localeCode_;
  loader::Locale currentLocale_;
};

} // namespace blocks
