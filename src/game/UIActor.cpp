#include "game/UIActor.hpp"

#include <cstdint>
#include <memory>
#include <utility>
#include "GlobalSubSystemStack.hpp"
#include "engine/Actor.hpp"
#include "engine/DrawableRegistry.hpp"
#include "engine/Scene.hpp"
#include "math/vec.hpp"
#include "render/Simple2DCamera.hpp"
#include "ui/Layout.hpp"
#include "ui/UIObject.hpp"

namespace blocks::game {

namespace {

constexpr int kUIBaseZ = 1000000;

}

UIActor::UIActor(Scene& scene) : UIActor(scene, nullptr) {}

UIActor::UIActor(Scene& scene, std::unique_ptr<ui::UIObject> ui)
    : Actor(scene),
      Drawable(scene.getDrawableScene()),
      uiCamera_(
          math::Vec2{0.f, 0.f},
          math::Vec2{1.f, 1.f},
          render::Simple2DCamera::AspectRatioHandling::STRETCH),
      ui_(std::move(ui)) {}

void UIActor::setUIObject(std::unique_ptr<ui::UIObject> newUI) {
  ui_ = std::move(newUI);
}

void UIActor::draw() {
  if (ui_ == nullptr) {
    return;
  }

  auto window = GlobalSubSystemStack::get().window();
  std::pair<int, int> windowSizePair = window->getCurrentWindowSize();
  math::Vec<uint16_t, 2> windowSizeVec{
      static_cast<uint16_t>(windowSizePair.first),
      static_cast<uint16_t>(windowSizePair.second)};

  uiCamera_ = render::Simple2DCamera{
      math::Vec2{0.f, 0.f},
      math::Vec2{
          static_cast<float>(windowSizeVec.x()),
          static_cast<float>(windowSizeVec.y())},
      render::Simple2DCamera::AspectRatioHandling::STRETCH};

  ui::layoutAndDraw(*ui_, uiCamera_, windowSizeVec, kUIBaseZ);
}

} // namespace blocks::game
