#include "game/Button.hpp"

#include <functional>
#include <optional>
#include <string>
#include <utility>
#include "GlobalSubSystemStack.hpp"
#include "engine/Actor.hpp"
#include "engine/DrawableRegistry.hpp"
#include "engine/Scene.hpp"
#include "game/resource/DefaultFont.hpp"
#include "input/InputHandler.hpp"
#include "math/vec.hpp"
#include "render/Font.hpp"
#include "render/RenderSubSystem.hpp"
#include "render/renderables/RenderableColor2D.hpp"
#include "util/debug.hpp"

namespace blocks::game {

namespace {

class ButtonResourceSentinelData {
 public:
  ButtonResourceSentinelData()
      : color_(GlobalSubSystemStack::get()
                   .renderSystem()
                   .createRenderable<render::RenderableColor2D>()) {}

  render::RenderableRef<render::RenderableColor2D::InstanceData> getSprite() {
    return color_.get();
  }

 private:
  render::UniqueRenderableHandle<render::RenderableColor2D::InstanceData>
      color_;
};

constinit std::optional<ButtonResourceSentinelData> resourceSentinel;

math::Vec2 screenSpaceToWorldSpace(math::Vec2 screenSpace) {
  auto window = GlobalSubSystemStack::get().window();
  math::Mat3 inverseViewMatrix =
      GlobalSubSystemStack::get()
          .renderSystem()
          .getDefaultCamera()
          .getViewMatrix(window->getCurrentWindowExtent())
          .invert();
  math::Vec3 worldMousePos =
      inverseViewMatrix * math::Vec3{screenSpace.x(), screenSpace.y(), 1.0f};

  return {worldMousePos.x(), worldMousePos.y()};
}

} // namespace

void ButtonResourceSentinel::load() {
  DEBUG_ASSERT(!resourceSentinel.has_value());
  resourceSentinel.emplace();
}

void ButtonResourceSentinel::unload() {
  resourceSentinel.reset();
}

Button::Button(
    Scene& scene,
    math::Vec2 pos0,
    math::Vec2 pos1,
    std::string label,
    std::function<void()> onClick)
    : Actor(scene),
      Drawable(scene.getDrawableScene()),
      input::InputHandler(GlobalSubSystemStack::get().inputSystem()),
      pos0_(pos0),
      pos1_(pos1),
      label_(std::move(label)),
      hovered_(false),
      onClick_(std::move(onClick)) {}

void Button::draw() {
  math::Vec4 color = hovered_
      ? math::Vec4{0.3f, 0.3f, 0.3f, 1.0f}
      : math::Vec4{0.0f, 0.0f, 0.0f, 1.0f};
  auto& render = GlobalSubSystemStack::get().renderSystem();
  auto window = GlobalSubSystemStack::get().window();
  render.drawObject(
      window,
      0,
      resourceSentinel->getSprite(),
      {math::modelMatrixFromBounds(pos0_, pos1_), color});

  float padding = 0.1f;
  DefaultFontResourceSentinel::get().drawStringUTF8(
      label_,
      math::Vec2{
          (pos0_.x() + pos1_.x()) / 2.f,
          pos1_.y() - (pos1_.y() - pos0_.y()) * 0.5f * padding},
      render::Font::Size::Line{(pos1_.y() - pos0_.y()) * (1.0f - padding)},
      render::Font::Align::CENTER,
      render::Font::VAlign::BOTTOM);
}

void Button::onMouseMove(math::Vec2 mousePos) {
  math::Vec2 worldMousePos = screenSpaceToWorldSpace(mousePos);

  hovered_ = pos0_.x() <= worldMousePos.x() && worldMousePos.x() <= pos1_.x() &&
      pos0_.y() <= worldMousePos.y() && worldMousePos.y() <= pos1_.y();
}

void Button::onMouseUp(math::Vec2 mousePos) {
  math::Vec2 worldMousePos = screenSpaceToWorldSpace(mousePos);

  if (onClick_ && pos0_.x() <= worldMousePos.x() &&
      worldMousePos.x() <= pos1_.x() && pos0_.y() <= worldMousePos.y() &&
      worldMousePos.y() <= pos1_.y()) {
    onClick_();
  }
}

} // namespace blocks::game
