#pragma once

#include <string>
#include "math/vec.hpp"
#include "util/meta_utils.hpp"

namespace blocks {

struct Settings {
  std::string localeCode;
  math::Vec<int, 2> resolution;

  using Fields = util::TArray<
      util::TPair<util::TString<"localeCode">, std::string>,
      util::TPair<util::TString<"resolution">, math::Vec<int, 2>>>;

  template <size_t i>
  [[nodiscard]] const Fields::At<i>::Second& get() const;

  template <>
  [[nodiscard]] const std::string& get<0>() const {
    return localeCode;
  }

  template <>
  [[nodiscard]] const math::Vec<int, 2>& get<1>() const {
    return resolution;
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
