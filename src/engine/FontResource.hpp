#pragma once

#include <string>
#include "render/Font.hpp"
#include "util/meta_utils.hpp"

namespace blocks {

// NOLINTNEXTLINE(bugprone-exception-escape)
class FontResource {
 public:
  using Fields = util::TArray<util::TPair<util::TString<"path">, std::string>>;

  explicit FontResource(const std::string& path);

  [[nodiscard]] const render::Font& get() const { return font_; }

 private:
  render::Font font_;
};

} // namespace blocks
