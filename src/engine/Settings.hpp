#pragma once

#include <string>
#include "util/meta_utils.hpp"

namespace blocks {

struct Settings {
  std::string localeCode;

  using Fields =
      util::TArray<util::TPair<util::TString<"localeCode">, std::string>>;
};

const Settings& getSettings();

} // namespace blocks
