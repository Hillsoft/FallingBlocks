#pragma once

#include <cstdint>
#include "math/vec.hpp"
#include "render/RenderSubSystem.hpp"
#include "render/Simple2DCamera.hpp"
#include "render/renderables/RenderableColor2D.hpp"
#include "ui/UIObject.hpp"

namespace blocks::ui {

class UIRect : public UIObject {
 public:
  UIRect(
      math::Vec4 color,
      render::RenderableRef<render::RenderableColor2D::InstanceData>
          colorRenderable);

  int draw(
      math::Vec<uint16_t, 2> minPos,
      math::Vec<uint16_t, 2> maxPos,
      render::Simple2DCamera& camera,
      int baseZ) final;

 private:
  math::Vec4 color_;
  render::RenderableRef<render::RenderableColor2D::InstanceData>
      colorRenderable_;
};

} // namespace blocks::ui
