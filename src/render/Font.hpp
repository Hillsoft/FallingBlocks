#pragma once

#include <cstdint>
#include <string_view>
#include <utility>
#include <vector>
#include "loader/font/Font.hpp"
#include "math/vec.hpp"
#include "render/RenderSubSystem.hpp"
#include "render/renderables/RenderableFont.hpp"

namespace blocks::render {

class Font {
 public:
  enum Encoding { ASCII, UTF8 };
  enum Align { LEFT, CENTER, RIGHT };

  Font(RenderSubSystem& renderSystem, loader::Font font);

  void drawStringASCII(
      std::string_view str, math::Vec2 pos, Align align = Align::LEFT);
  void drawStringUTF8(
      std::string_view str, math::Vec2 pos, Align align = Align::LEFT);

  float stringWidth(Encoding encoding, std::string_view str) const;

 private:
  RenderSubSystem* render_;
  loader::Font fontData_;
  std::vector<std::pair<int32_t, int32_t>> glyphRanges_;
  UniqueRenderableHandle<render::RenderableFont::InstanceData>
      renderableObject_;
};

} // namespace blocks::render
