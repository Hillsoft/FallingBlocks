#pragma once

#include <cstdint>
#include <functional>
#include "input/InputHandler.hpp"
#include "input/InputSubSystem.hpp"
#include "math/vec.hpp"
#include "render/RenderSubSystem.hpp"
#include "render/Simple2DCamera.hpp"
#include "render/renderables/RenderableColor2D.hpp"
#include "ui/UIObject.hpp"

namespace blocks::ui {

class UIButton : public UIObject, input::InputHandler {
 public:
  UIButton(
      input::InputSubSystem& inputSystem,
      math::Vec4 color,
      math::Vec4 hoverColor,
      render::RenderableRef<render::RenderableColor2D::InstanceData>
          colorRenderable,
      std::function<void()> onClick);

  int draw(
      math::Vec<uint16_t, 2> minPos,
      math::Vec<uint16_t, 2> maxPos,
      render::Simple2DCamera& camera,
      int baseZ) final;

  void onMouseMove(math::Vec2 mousePos) final;
  void onMouseUp(math::Vec2 mousePos) final;

 private:
  math::Vec4 color_;
  math::Vec4 hoverColor_;
  render::RenderableRef<render::RenderableColor2D::InstanceData>
      colorRenderable_;
  std::function<void()> onClick_;

  math::Vec2 lastScreenMinPos_{0.f, 0.f};
  math::Vec2 lastScreenMaxPos_{0.f, 0.f};
  bool isHovered_ = false;
};

} // namespace blocks::ui
