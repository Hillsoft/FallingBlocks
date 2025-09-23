#pragma once

#include <string>
#include <string_view>
#include "loader/Locale.hpp"

namespace blocks {

class Localisation {
 public:
  Localisation(std::string_view locale);

  std::string getLocalisedString(std::string_view key);

 private:
  loader::Locale currentLocale_;
};

} // namespace blocks
