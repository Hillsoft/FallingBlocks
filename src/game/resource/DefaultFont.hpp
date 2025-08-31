#pragma once

#include "render/Font.hpp"

namespace blocks::game {

class DefaultFontResourceSentinel {
 public:
  static void load();
  static void unload();

  static render::Font& get();
};

} // namespace blocks::game
