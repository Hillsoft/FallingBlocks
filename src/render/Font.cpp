#include "render/Font.hpp"

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <span>
#include <string_view>
#include <utility>
#include <variant>
#include <vector>
#include <GLFW/glfw3.h>
#include "GlobalSubSystemStack.hpp"
#include "loader/font/Font.hpp"
#include "math/vec.hpp"
#include "render/RenderSubSystem.hpp"
#include "render/VulkanBuffer.hpp"
#include "render/VulkanGraphicsDevice.hpp"
#include "render/renderables/RenderableFont.hpp"
#include "util/Generator.hpp"
#include "util/debug.hpp"

namespace blocks::render {

namespace {

constexpr float kFontScale = 1 / 10000.0f;

struct GlyphPoint {
  math::Vec<int32_t, 2> point;
  uint32_t onCurve;
  uint32_t contourEnd;
};

util::Generator<std::pair<size_t, size_t>> contourRanges(
    std::span<const uint16_t> endPoints) {
  co_yield std::pair<size_t, size_t>{0, endPoints[0]};
  for (size_t i = 1; i < endPoints.size(); i++) {
    co_yield std::pair<size_t, size_t>{endPoints[i - 1] + 1, endPoints[i]};
  }
}

VulkanBuffer makeFontBuffer(
    VulkanGraphicsDevice& device,
    const loader::Font& font,
    std::vector<std::pair<int32_t, int32_t>>& glyphRanges) {
  std::vector<GlyphPoint> pointData;
  glyphRanges.clear();

  for (const auto& glyph : font.glyphs) {
    int32_t glyphStart = static_cast<int32_t>(pointData.size());

    if (std::holds_alternative<loader::SimpleGlyphData>(glyph.data)) {
      const auto& glyphData = std::get<loader::SimpleGlyphData>(glyph.data);
      DEBUG_ASSERT(
          glyphData.onCurve.size() == glyphData.xCoords.size() &&
          glyphData.xCoords.size() == glyphData.yCoords.size());

      if (glyphData.endPoints.size() == 0) {
        continue;
      }

      for (auto [contourStart, contourEnd] :
           contourRanges(glyphData.endPoints)) {
        // Ensure the first point is on curve
        if (!glyphData.onCurve[contourStart]) {
          if (!glyphData.onCurve[contourEnd]) {
            // Insert virtual point
            pointData.emplace_back(
                math::Vec<int32_t, 2>{
                    (glyphData.xCoords[contourStart] +
                     glyphData.xCoords[contourEnd]) /
                        2,
                    (glyphData.yCoords[contourStart] +
                     glyphData.yCoords[contourEnd]) /
                        2},
                true,
                false);
          } else {
            // Make the last point the first
            contourEnd--;

            pointData.emplace_back(
                math::Vec<int32_t, 2>{
                    glyphData.xCoords[contourEnd],
                    glyphData.yCoords[contourEnd]},
                true,
                false);
          }
        }

        bool lastOnCurve = true;
        for (size_t pointIndex = contourStart; pointIndex <= contourEnd;
             pointIndex++) {
          if (!lastOnCurve && !glyphData.onCurve[pointIndex]) {
            // Make virtual point explicit
            pointData.emplace_back(
                math::Vec<int32_t, 2>{
                    (glyphData.xCoords[pointIndex - 1] +
                     glyphData.xCoords[pointIndex]) /
                        2,
                    (glyphData.yCoords[pointIndex - 1] +
                     glyphData.yCoords[pointIndex]) /
                        2},
                true,
                false);
          }

          pointData.emplace_back(
              math::Vec<int32_t, 2>{
                  glyphData.xCoords[pointIndex], glyphData.yCoords[pointIndex]},
              glyphData.onCurve[pointIndex],
              pointIndex == contourEnd);

          lastOnCurve = glyphData.onCurve[pointIndex];
        }
      }
    }

    glyphRanges.emplace_back(
        glyphStart, static_cast<int32_t>(pointData.size()));
  }

  return VulkanBuffer{
      device,
      std::span<std::byte>{
          reinterpret_cast<std::byte*>(pointData.data()),
          pointData.size() * sizeof(GlyphPoint)},
      VK_BUFFER_USAGE_STORAGE_BUFFER_BIT};
}

float drawChar(
    RenderSubSystem& render,
    const loader::Font& fontData,
    const std::vector<std::pair<int32_t, int32_t>>& glyphRanges,
    RenderableRef<RenderableFont::InstanceData> renderableObject,
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
        RenderableFont::InstanceData{
            pos +
                math::Vec2{
                    static_cast<float>(metrics.leftSideBearing) * kFontScale +
                        static_cast<float>(glyph.xMin.rawValue) * kFontScale,
                    -static_cast<float>(glyph.yMax.rawValue) * kFontScale},
            pos +
                math::Vec2{
                    static_cast<float>(metrics.leftSideBearing) * kFontScale +
                        static_cast<float>(glyph.xMax.rawValue) * kFontScale,
                    -static_cast<float>(glyph.yMin.rawValue) * kFontScale},
            glyphRanges[glyphIndex].first,
            glyphRanges[glyphIndex].second,
            math::Mat3::translate(math::Vec2{
                static_cast<float>(glyph.xMin.rawValue),
                static_cast<float>(glyph.yMin.rawValue)}) *
                math::Mat3::scale(math::Vec2{
                    static_cast<float>(
                        glyph.xMax.rawValue - glyph.xMin.rawValue),
                    static_cast<float>(
                        glyph.yMax.rawValue - glyph.yMin.rawValue)})});
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
          RenderableFont::InstanceData{
              pos +
                  transformPos(math::Vec2{
                      static_cast<float>(metrics.leftSideBearing) * kFontScale +
                          static_cast<float>(subGlyph.xMin.rawValue) *
                              kFontScale,
                      -static_cast<float>(subGlyph.yMax.rawValue) *
                          kFontScale}),
              pos +
                  transformPos(math::Vec2{
                      static_cast<float>(metrics.leftSideBearing) * kFontScale +
                          static_cast<float>(subGlyph.xMax.rawValue) *
                              kFontScale,
                      -static_cast<float>(subGlyph.yMin.rawValue) *
                          kFontScale}),
              glyphRanges[subGlyphDetails.glpyhIndex].first,
              glyphRanges[subGlyphDetails.glpyhIndex].second,
              math::Mat3::translate(math::Vec2{
                  static_cast<float>(glyph.xMin.rawValue),
                  static_cast<float>(glyph.yMin.rawValue)}) *
                  math::Mat3::scale(math::Vec2{
                      static_cast<float>(
                          glyph.xMax.rawValue - glyph.xMin.rawValue),
                      static_cast<float>(
                          glyph.yMax.rawValue - glyph.yMin.rawValue)})});
    }
  }

  return static_cast<float>(metrics.advanceWidth) * kFontScale;
}

} // namespace

Font::Font(RenderSubSystem& renderSystem, loader::Font font)
    : render_(&renderSystem),
      fontData_(std::move(font)),
      glyphRanges_(),
      fontBuffer_(makeFontBuffer(
          renderSystem.getGraphicsDevice(), fontData_, glyphRanges_)),
      renderableObject_(
          renderSystem.createRenderable<RenderableFont>(fontBuffer_)) {}

void Font::drawStringASCII(std::string_view str, math::Vec2 pos) {
  auto window = GlobalSubSystemStack::get().window();
  for (unsigned char c : str) {
    float advance = drawChar(
        *render_, fontData_, glyphRanges_, *renderableObject_, window, c, pos);

    pos.x() += advance;
  }
}

} // namespace blocks::render
