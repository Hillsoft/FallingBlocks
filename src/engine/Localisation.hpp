#pragma once

#include <string>
#include <string_view>
#include <vector>
#include "loader/Locale.hpp"

namespace blocks {

class Localisation {
 public:
  explicit Localisation(std::string locale);

  void setLocale(std::string locale);
  [[nodiscard]] std::vector<std::string> getAvailableLocales() const;

  [[nodiscard]] std::string getLocalisedString(std::string_view key) const;
  [[nodiscard]] std::string_view getLocaleCode() const;
  [[nodiscard]] std::string_view getLocaleName() const;

 private:
  std::string localeCode_;
  loader::Locale currentLocale_;
};

} // namespace blocks
