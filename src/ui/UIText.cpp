#include "ui/UIText.hpp"

#include <cstdint>
#include <string>
#include <utility>
#include "math/vec.hpp"
#include "render/Font.hpp"
#include "render/Simple2DCamera.hpp"

namespace blocks::ui {

UIText::UIText(
    const render::Font& font, std::string text, render::Font::Size fontSize)
    : font_(font), text_(std::move(text)), fontSize_(fontSize) {
  minWidth_ = static_cast<uint16_t>(
      font_->stringWidth(render::Font::Encoding::UTF8, text_, fontSize_));
  minHeight_ = static_cast<uint16_t>(font_->stringHeight(fontSize_));
}

int UIText::draw(
    math::Vec<uint16_t, 2> minPos,
    math::Vec<uint16_t, 2> maxPos,
    render::Simple2DCamera& camera,
    int baseZ) {
  font_->drawStringUTF8(
      text_,
      {static_cast<float>(minPos.x() + maxPos.x()) / 2.f,
       static_cast<float>(maxPos.y())},
      fontSize_,
      render::Font::Align::CENTER,
      render::Font::VAlign::BOTTOM,
      baseZ,
      &camera);
  return 1;
}

void UIText::setText(std::string newText) {
  text_ = std::move(newText);
  minWidth_ = static_cast<uint16_t>(
      font_->stringWidth(render::Font::Encoding::UTF8, text_, fontSize_));
}

} // namespace blocks::ui
