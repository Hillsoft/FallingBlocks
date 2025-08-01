#include "render/Font.hpp"

#include <algorithm>
#include <cstdint>
#include <cstdlib>
#include <string_view>
#include <utility>
#include <variant>
#include <vector>
#include "GlobalSubSystemStack.hpp"
#include "loader/font/Font.hpp"
#include "math/vec.hpp"
#include "render/RenderSubSystem.hpp"
#include "render/renderables/RenderableColor2D.hpp"
#include "util/debug.hpp"

namespace blocks::render {

namespace {

constexpr float kFontScale = 1 / 10000.0f;

float drawChar(
    RenderSubSystem& render,
    const loader::Font& fontData,
    RenderableRef<RenderableColor2D::InstanceData> renderableObject,
    WindowRef window,
    uint32_t c,
    math::Vec2 pos) {
  auto glyphIndex = fontData.charMap->mapChar(c);
  const auto& glyph = fontData.glyphs[glyphIndex];
  const auto& metrics = fontData.horizontalMetrics.data[glyphIndex];

  if (std::holds_alternative<loader::SimpleGlyphData>(glyph.data)) {
    render.drawObject(
        window,
        renderableObject,
        {pos +
             math::Vec2{
                 static_cast<float>(metrics.leftSideBearing) * kFontScale +
                     static_cast<float>(glyph.xMin.rawValue) * kFontScale,
                 -static_cast<float>(glyph.yMax.rawValue) * kFontScale},
         pos +
             math::Vec2{
                 static_cast<float>(metrics.leftSideBearing) * kFontScale +
                     static_cast<float>(glyph.xMax.rawValue) * kFontScale,
                 -static_cast<float>(glyph.yMin.rawValue) * kFontScale},
         math::Vec4{0.6f, 1.0f, 0.6f, 1.0f}});
  } else if (std::holds_alternative<std::vector<loader::CompoundGlyphData>>(
                 glyph.data)) {
    for (const auto& subGlyphDetails :
         std::get<std::vector<loader::CompoundGlyphData>>(glyph.data)) {
      const auto& subGlyph = fontData.glyphs[subGlyphDetails.glpyhIndex];
      DEBUG_ASSERT(
          std::holds_alternative<loader::SimpleGlyphData>(subGlyph.data));

      auto transformPos = [&](math::Vec2 pos) {
        float m0 =
            std::max(std::abs(subGlyphDetails.a), std::abs(subGlyphDetails.b));
        float n0 =
            std::max(std::abs(subGlyphDetails.c), std::abs(subGlyphDetails.d));
        float m =
            std::abs(
                std::abs(subGlyphDetails.a) - std::abs(subGlyphDetails.c)) <=
                (33.0f / 65536.0f)
            ? 2 * m0
            : m0;
        float n =
            std::abs(
                std::abs(subGlyphDetails.c) - std::abs(subGlyphDetails.d)) <=
                (33.0f / 65536.0f)
            ? 2 * n0
            : n0;
        return math::Vec2{
            (subGlyphDetails.a / m) * pos.x() +
                (subGlyphDetails.c / m) * pos.y() +
                static_cast<float>(subGlyphDetails.e) * kFontScale,
            (subGlyphDetails.b / n) * pos.x() +
                (subGlyphDetails.d / n) * pos.y() -
                static_cast<float>(subGlyphDetails.f) * kFontScale};
      };

      render.drawObject(
          window,
          renderableObject,
          {pos +
               transformPos(math::Vec2{
                   static_cast<float>(metrics.leftSideBearing) * kFontScale +
                       static_cast<float>(subGlyph.xMin.rawValue) * kFontScale,
                   -static_cast<float>(subGlyph.yMax.rawValue) * kFontScale}),
           pos +
               transformPos(math::Vec2{
                   static_cast<float>(metrics.leftSideBearing) * kFontScale +
                       static_cast<float>(subGlyph.xMax.rawValue) * kFontScale,
                   -static_cast<float>(subGlyph.yMin.rawValue) * kFontScale}),
           math::Vec4{0.6f, 0.6f, 1.0f, 1.0f}});
    }
  }

  return static_cast<float>(metrics.advanceWidth) * kFontScale;
}

} // namespace

Font::Font(RenderSubSystem& renderSystem, loader::Font font)
    : render_(&renderSystem),
      fontData_(std::move(font)),
      renderableObject_(renderSystem.createRenderable<RenderableColor2D>()) {}

void Font::drawStringASCII(std::string_view str, math::Vec2 pos) {
  auto window = GlobalSubSystemStack::get().window();
  for (unsigned char c : str) {
    float advance =
        drawChar(*render_, fontData_, *renderableObject_, window, c, pos);

    pos.x() += advance;
  }
}

} // namespace blocks::render
