#pragma once

#include <cstdint>
#include <string_view>
#include <utility>
#include <vector>
#include "loader/font/Font.hpp"
#include "math/vec.hpp"
#include "render/RenderSubSystem.hpp"
#include "render/VulkanBuffer.hpp"
#include "render/renderables/RenderableFont.hpp"

namespace blocks::render {

class Font {
 public:
  Font(RenderSubSystem& renderSystem, loader::Font font);

  void drawStringASCII(std::string_view str, math::Vec2 pos);

 private:
  RenderSubSystem* render_;
  loader::Font fontData_;
  std::vector<std::pair<int32_t, int32_t>> glyphRanges_;
  VulkanBuffer fontBuffer_;
  UniqueRenderableHandle<render::RenderableFont::InstanceData>
      renderableObject_;
};

} // namespace blocks::render
