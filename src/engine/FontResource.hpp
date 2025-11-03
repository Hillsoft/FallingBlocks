#pragma once

#include <string>
#include "render/Font.hpp"
#include "util/meta_utils.hpp"

namespace blocks {

class FontResource {
 public:
  using Fields = util::TArray<util::TPair<util::TString<"path">, std::string>>;

  FontResource(const std::string& path);

  const render::Font& get() const { return font_; }

 private:
  render::Font font_;
};

} // namespace blocks
