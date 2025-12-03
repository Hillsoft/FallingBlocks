#pragma once

#include <string>
#include "util/meta_utils.hpp"

namespace blocks {

struct Settings {
  std::string localeCode;

  using Fields =
      util::TArray<util::TPair<util::TString<"localeCode">, std::string>>;

  template <size_t i>
  [[nodiscard]] const Fields::At<i>::Second& get() const;

  template <>
  [[nodiscard]] const std::string& get<0>() const {
    return localeCode;
  }

  template <typename Fn>
  static void update(Fn&& fn) {
    Settings& settings = getInternal();
    std::forward<Fn>(fn)(settings);
    settings.save();
  }

 private:
  void save() const;
  static Settings& getInternal();
};

const Settings& getSettings();

} // namespace blocks
