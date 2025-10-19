#include "ui/UIRect.hpp"

#include <cstdint>
#include "GlobalSubSystemStack.hpp"
#include "math/vec.hpp"
#include "render/RenderSubSystem.hpp"
#include "render/Simple2DCamera.hpp"
#include "render/renderables/RenderableColor2D.hpp"

namespace blocks::ui {

UIRect::UIRect(
    math::Vec4 color,
    render::RenderableRef<render::RenderableColor2D::InstanceData>
        colorRenderable)
    : color_(color), colorRenderable_(colorRenderable) {}

int UIRect::draw(
    math::Vec<uint16_t, 2> minPos,
    math::Vec<uint16_t, 2> maxPos,
    render::Simple2DCamera& camera,
    int baseZ) {
  GlobalSubSystemStack::get().renderSystem().drawObject(
      GlobalSubSystemStack::get().window(),
      &camera,
      baseZ,
      colorRenderable_,
      render::RenderableColor2D::InstanceData{
          math::modelMatrixFromBounds(
              static_cast<math::Vec2>(minPos), static_cast<math::Vec2>(maxPos)),
          color_});
  return 1;
}

} // namespace blocks::ui
