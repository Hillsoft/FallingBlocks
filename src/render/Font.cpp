#include "render/Font.hpp"

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <optional>
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
#include "util/unicode.hpp"

namespace blocks::render {

namespace {

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

    if (std::holds_alternative<loader::SimpleGlyphData>(
            glyph.contourData.data)) {
      const auto& glyphData =
          std::get<loader::SimpleGlyphData>(glyph.contourData.data);
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
    math::Vec2 pos,
    float lineHeight,
    std::optional<uint16_t>& previousGlyph) {
  auto glyphIndex = fontData.charMap->mapChar(c);
  const auto& glyph = fontData.glyphs[glyphIndex];
  float fontScale = lineHeight / static_cast<float>(fontData.unitsPerEm);

  int16_t kerning = 0;
  if (previousGlyph.has_value()) {
    kerning = fontData.kerning.apply(*previousGlyph, glyphIndex).rawValue;
  }
  previousGlyph = glyphIndex;

  if (std::holds_alternative<loader::SimpleGlyphData>(glyph.contourData.data)) {
    render.drawObject(
        window,
        10,
        renderableObject,
        RenderableFont::InstanceData{
            math::Mat3::translate(
                pos +
                math::Vec2{
                    static_cast<float>(
                        glyph.horizontalMetrics.leftSideBearing.rawValue) *
                            fontScale +
                        static_cast<float>(kerning) * fontScale,
                    -static_cast<float>(glyph.contourData.yMax.rawValue) *
                        fontScale}) *
                math::Mat3::scale(math::Vec2{
                    static_cast<float>(
                        glyph.contourData.xMax.rawValue -
                        glyph.contourData.xMin.rawValue) *
                        fontScale,
                    static_cast<float>(
                        glyph.contourData.yMax.rawValue -
                        glyph.contourData.yMin.rawValue) *
                        fontScale}),
            glyphRanges[glyphIndex].first,
            glyphRanges[glyphIndex].second,
            math::Mat3::translate(math::Vec2{
                static_cast<float>(glyph.contourData.xMin.rawValue),
                static_cast<float>(glyph.contourData.yMin.rawValue)}) *
                math::Mat3::scale(math::Vec2{
                    static_cast<float>(
                        glyph.contourData.xMax.rawValue -
                        glyph.contourData.xMin.rawValue),
                    static_cast<float>(
                        glyph.contourData.yMax.rawValue -
                        glyph.contourData.yMin.rawValue)})});
  } else if (std::holds_alternative<std::vector<loader::CompoundGlyphData>>(
                 glyph.contourData.data)) {
    for (const auto& subGlyphDetails :
         std::get<std::vector<loader::CompoundGlyphData>>(
             glyph.contourData.data)) {
      const auto& subGlyph = fontData.glyphs[subGlyphDetails.glpyhIndex];
      DEBUG_ASSERT(std::holds_alternative<loader::SimpleGlyphData>(
          subGlyph.contourData.data));

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
                static_cast<float>(subGlyphDetails.e) * fontScale,
            (subGlyphDetails.b / n) * pos.x() +
                (subGlyphDetails.d / n) * pos.y() -
                static_cast<float>(subGlyphDetails.f) * fontScale};
      };

      render.drawObject(
          window,
          10,
          renderableObject,
          RenderableFont::InstanceData{
              math::Mat3::translate(
                  pos +
                  transformPos(math::Vec2{
                      static_cast<float>(
                          subGlyph.horizontalMetrics.leftSideBearing.rawValue) *
                              fontScale +
                          static_cast<float>(kerning) * fontScale,
                      -static_cast<float>(subGlyph.contourData.yMax.rawValue) *
                          fontScale})) *
                  math::Mat3::scale(math::Vec2{
                      static_cast<float>(
                          subGlyph.contourData.xMax.rawValue -
                          subGlyph.contourData.xMin.rawValue) *
                          fontScale,
                      static_cast<float>(
                          subGlyph.contourData.yMax.rawValue -
                          subGlyph.contourData.yMin.rawValue) *
                          fontScale}),
              glyphRanges[subGlyphDetails.glpyhIndex].first,
              glyphRanges[subGlyphDetails.glpyhIndex].second,
              math::Mat3::translate(math::Vec2{
                  static_cast<float>(subGlyph.contourData.xMin.rawValue),
                  static_cast<float>(subGlyph.contourData.yMin.rawValue)}) *
                  math::Mat3::scale(math::Vec2{
                      static_cast<float>(
                          subGlyph.contourData.xMax.rawValue -
                          subGlyph.contourData.xMin.rawValue),
                      static_cast<float>(
                          subGlyph.contourData.yMax.rawValue -
                          subGlyph.contourData.yMin.rawValue)})});
    }
  }

  return static_cast<float>(kerning) * fontScale +
      static_cast<float>(glyph.horizontalMetrics.advanceWidth.rawValue) *
      fontScale;
}

} // namespace

Font::Font(RenderSubSystem& renderSystem, loader::Font font)
    : render_(&renderSystem),
      fontData_(std::move(font)),
      glyphRanges_(),
      renderableObject_(
          renderSystem.createRenderable<RenderableFont>(makeFontBuffer(
              renderSystem.getGraphicsDevice(), fontData_, glyphRanges_))) {}

void Font::drawStringASCII(
    std::string_view str, math::Vec2 pos, float lineHeight, Align align) {
  switch (align) {
    case Align::LEFT:
      break;
    case Align::CENTER:
      pos.x() -= stringWidth(Encoding::ASCII, str, lineHeight) / 2;
      break;
    case Align::RIGHT:
      pos.x() -= stringWidth(Encoding::ASCII, str, lineHeight);
      break;
  }

  auto window = GlobalSubSystemStack::get().window();
  std::optional<uint16_t> previousGlyph;
  for (unsigned char c : str) {
    float advance = drawChar(
        *render_,
        fontData_,
        glyphRanges_,
        *renderableObject_,
        window,
        c,
        pos,
        lineHeight,
        previousGlyph);

    pos.x() += advance;
  }
}

void Font::drawStringUTF8(
    std::string_view str, math::Vec2 pos, float lineHeight, Align align) {
  switch (align) {
    case Align::LEFT:
      break;
    case Align::CENTER:
      pos.x() -= stringWidth(Encoding::UTF8, str, lineHeight) / 2;
      break;
    case Align::RIGHT:
      pos.x() -= stringWidth(Encoding::UTF8, str, lineHeight);
      break;
  }

  auto window = GlobalSubSystemStack::get().window();
  std::optional<uint16_t> previousGlyph;
  for (uint32_t c : util::unicodeDecode(str)) {
    float advance = drawChar(
        *render_,
        fontData_,
        glyphRanges_,
        *renderableObject_,
        window,
        c,
        pos,
        lineHeight,
        previousGlyph);

    pos.x() += advance;
  }
}

float Font::stringWidth(
    Encoding encoding, std::string_view str, float lineHeight) const {
  float fontScale = lineHeight / static_cast<float>(fontData_.unitsPerEm);
  float width = 0.0f;

  std::optional<uint16_t> previousGlyph;

  auto processChar = [&](uint32_t c) {
    auto glyphIndex = fontData_.charMap->mapChar(c);

    int16_t kerning = 0;
    if (previousGlyph.has_value()) {
      kerning = fontData_.kerning.apply(*previousGlyph, glyphIndex).rawValue;
    }
    previousGlyph = glyphIndex;

    width += static_cast<float>(kerning) * fontScale;
    width += static_cast<float>(fontData_.glyphs[glyphIndex]
                                    .horizontalMetrics.advanceWidth.rawValue) *
        fontScale;
  };

  if (encoding == ASCII) {
    for (unsigned char c : str) {
      processChar(c);
    }
  } else if (encoding == UTF8) {
    for (uint32_t c : util::unicodeDecode(str)) {
      processChar(c);
    }
  } else {
    DEBUG_ASSERT(false);
  }

  return width;
}

} // namespace blocks::render
