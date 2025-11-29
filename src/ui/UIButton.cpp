#include "ui/UIButton.hpp"

#include <cstdint>
#include <functional>
#include <utility>
#include "GlobalSubSystemStack.hpp"
#include "input/InputHandler.hpp"
#include "input/InputSubSystem.hpp"
#include "math/vec.hpp"
#include "render/RenderSubSystem.hpp"
#include "render/Simple2DCamera.hpp"
#include "render/renderables/RenderableColor2D.hpp"

namespace blocks::ui {

UIButton::UIButton(
    input::InputSubSystem& inputSystem,
    math::Vec4 color,
    math::Vec4 hoverColor,
    render::RenderableRef<render::RenderableColor2D::InstanceData>
        colorRenderable,
    std::function<void()> onClick)
    : InputHandler(inputSystem),
      color_(color),
      hoverColor_(hoverColor),
      colorRenderable_(colorRenderable),
      onClick_(std::move(onClick)) {}

int UIButton::draw(
    math::Vec<uint16_t, 2> minPos,
    math::Vec<uint16_t, 2> maxPos,
    render::Simple2DCamera& camera,
    int baseZ) {
  auto window = GlobalSubSystemStack::get().window();
  math::Mat3 toScreenSpace =
      camera.getViewMatrix(window->getCurrentWindowExtent());
  auto convertToScreenSpace = [&](math::Vec<uint16_t, 2> pos) {
    math::Vec3 screenSpace = toScreenSpace *
        math::Vec3{
            static_cast<float>(pos.x()), static_cast<float>(pos.y()), 1.f};
    return math::Vec2{screenSpace.x(), screenSpace.y()};
  };
  lastScreenMinPos_ = convertToScreenSpace(minPos);
  lastScreenMaxPos_ = convertToScreenSpace(maxPos);

  GlobalSubSystemStack::get().renderSystem().drawObject(
      GlobalSubSystemStack::get().window(),
      &camera,
      baseZ,
      colorRenderable_,
      render::RenderableColor2D::InstanceData{
          math::modelMatrixFromBounds(
              static_cast<math::Vec2>(minPos), static_cast<math::Vec2>(maxPos)),
          isHovered_ ? hoverColor_ : color_});
  return 1;
}

void UIButton::onMouseMove(math::Vec2 mousePos) {
  isHovered_ = lastScreenMinPos_.x() <= mousePos.x() &&
      mousePos.x() <= lastScreenMaxPos_.x() &&
      lastScreenMinPos_.y() <= mousePos.y() &&
      mousePos.y() <= lastScreenMaxPos_.y();
}

void UIButton::onMouseUp(math::Vec2 /* mousePos */) {
  if (onClick_ && isHovered_) {
    onClick_();
  }
}

} // namespace blocks::ui
