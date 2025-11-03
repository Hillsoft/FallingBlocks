#pragma once

#include <cstdint>
#include <string_view>
#include <utility>
#include <variant>
#include <vector>
#include "loader/font/Font.hpp"
#include "math/vec.hpp"
#include "render/RenderSubSystem.hpp"
#include "render/Simple2DCamera.hpp"
#include "render/renderables/RenderableFont.hpp"

namespace blocks::render {

class Font {
 public:
  struct Size {
   public:
    struct Em {
      explicit Em(float height) : emHeight_(height) {}

      float getEmHeight(const Font& font) const;

      float emHeight_;
    };

    struct Line {
      explicit Line(float height) : lineHeight_(height) {}

      float getEmHeight(const Font& font) const;

      float lineHeight_;
    };

    /* implicit */ Size(Em sizeData) : sizeData_(sizeData) {}
    /* implicit */ Size(Line sizeData) : sizeData_(sizeData) {}

    float getEmHeight(const Font& font) const {
      return std::visit(
          [&](const auto& data) { return data.getEmHeight(font); }, sizeData_);
    }

   private:
    std::variant<Em, Line> sizeData_;
  };

  enum class Encoding { ASCII, UTF8 };
  enum class Align { LEFT, CENTER, RIGHT };
  enum class VAlign { BASELINE, BOTTOM, CENTER, TOP };

  Font(RenderSubSystem& renderSystem, loader::Font font);

  void drawStringASCII(
      std::string_view str,
      math::Vec2 pos,
      Size fontSize,
      Align align = Align::LEFT,
      VAlign valign = VAlign::BASELINE,
      int zDepth = 10,
      Simple2DCamera* camera = nullptr) const;
  void drawStringUTF8(
      std::string_view str,
      math::Vec2 pos,
      Size fontSize,
      Align align = Align::LEFT,
      VAlign valign = VAlign::BASELINE,
      int zDepth = 10,
      Simple2DCamera* camera = nullptr) const;

  float stringWidth(
      Encoding encoding, std::string_view str, Size fontSize) const;
  float stringHeight(Size fontSize) const;

 private:
  float getSizeScale(const Size& size) const;

  RenderSubSystem* render_;
  loader::Font fontData_;
  std::vector<std::pair<int32_t, int32_t>> glyphRanges_;
  UniqueRenderableHandle<render::RenderableFont::InstanceData>
      renderableObject_;
};

} // namespace blocks::render
