#pragma once

#include <cstdint>
#include <string_view>
#include "loader/font/Font.hpp"
#include "math/vec.hpp"
#include "render/RenderSubSystem.hpp"
#include "render/RenderableQuad.hpp"

namespace blocks::render {

class Font {
 public:
  Font(RenderSubSystem& renderSystem, loader::Font font);

  void drawStringASCII(std::string_view str, math::Vec2 pos);

 private:
  RenderSubSystem* render_;
  loader::Font fontData_;
  UniqueRenderableHandle<UniformData> renderableObject_;
};

} // namespace blocks::render
