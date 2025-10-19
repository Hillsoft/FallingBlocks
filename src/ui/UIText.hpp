#pragma once

#include <cstdint>
#include <string>
#include "math/vec.hpp"
#include "render/Font.hpp"
#include "render/Simple2DCamera.hpp"
#include "ui/UIObject.hpp"

namespace blocks::ui {

class UIText : public UIObject {
 public:
  UIText(render::Font& font, std::string text, render::Font::Size fontSize);

  int draw(
      math::Vec<uint16_t, 2> minPos,
      math::Vec<uint16_t, 2> maxPos,
      render::Simple2DCamera& camera,
      int baseZ) final;

 private:
  render::Font* font_;
  std::string text_;
  render::Font::Size fontSize_;
};

} // namespace blocks::ui
