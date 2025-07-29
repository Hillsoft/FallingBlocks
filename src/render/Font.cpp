#include "render/Font.hpp"

#include <cstdint>
#include <string_view>
#include <utility>
#include "GlobalSubSystemStack.hpp"
#include "loader/font/Font.hpp"
#include "math/vec.hpp"
#include "render/RenderSubSystem.hpp"

namespace blocks::render {

namespace {

constexpr float kFontScale = 1 / 10000.0f;

float drawChar(
    RenderSubSystem& render,
    const loader::Font& fontData,
    RenderableRef renderableObject,
    WindowRef window,
    uint32_t c,
    math::Vec2 pos) {
  auto glyphIndex = fontData.charMap->mapChar(c);
  const auto& glyph = fontData.glyphs[glyphIndex];
  const auto& metrics = fontData.horizontalMetrics.data[glyphIndex];

  render.drawObject(
      window,
      renderableObject,
      pos +
          math::Vec2{
              static_cast<float>(metrics.leftSideBearing) * kFontScale +
                  static_cast<float>(glyph.xMin.rawValue) * kFontScale,
              -static_cast<float>(glyph.yMax.rawValue) * kFontScale},
      pos +
          math::Vec2{
              static_cast<float>(metrics.leftSideBearing) * kFontScale +
                  static_cast<float>(glyph.xMax.rawValue) * kFontScale,
              -static_cast<float>(glyph.yMin.rawValue) * kFontScale});

  return static_cast<float>(metrics.advanceWidth) * kFontScale;
}

} // namespace

Font::Font(RenderSubSystem& renderSystem, loader::Font font)
    : render_(&renderSystem),
      fontData_(std::move(font)),
      renderableObject_(
          renderSystem.createRenderable(RESOURCE_DIR "/white.png")) {}

void Font::drawStringASCII(std::string_view str, math::Vec2 pos) {
  auto window = GlobalSubSystemStack::get().window();
  for (char c : str) {
    float advance =
        drawChar(*render_, fontData_, *renderableObject_, window, c, pos);

    pos.x() += advance;
  }
}

} // namespace blocks::render
