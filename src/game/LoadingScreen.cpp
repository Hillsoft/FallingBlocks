#include "game/LoadingScreen.hpp"

#include "GlobalSubSystemStack.hpp"
#include "engine/Actor.hpp"
#include "engine/DrawableRegistry.hpp"
#include "engine/Scene.hpp"
#include "engine/TickRegistry.hpp"
#include "math/vec.hpp"

namespace blocks::game {

namespace {

constexpr float kSwitchTime = 1.f;

}

LoadingScreen::LoadingScreen(
    Scene& scene, const LoadingScreenDefinition& definition)
    : Actor(scene),
      Drawable(scene.getDrawableScene()),
      TickHandler(scene.getTickRegistry()),
      prototype_(definition.prototype) {}

void LoadingScreen::update(float deltaTimeSeconds) {
  toTransition -= deltaTimeSeconds;
  if (toTransition < 0.f) {
    firstActive_ = !firstActive_;
    toTransition += kSwitchTime;
  }
}

void LoadingScreen::draw() {
  auto window = GlobalSubSystemStack::get().window();
  if (firstActive_) {
    GlobalSubSystemStack::get().renderSystem().drawObject(
        window,
        0,
        prototype_->texture1->get(),
        {math::modelMatrixFromBounds(
            math::Vec2{-1.f, -1.f}, math::Vec2{1.f, 1.f})});
  } else {
    GlobalSubSystemStack::get().renderSystem().drawObject(
        window,
        0,
        prototype_->texture2->get(),
        {math::modelMatrixFromBounds(
            math::Vec2{-1.f, -1.f}, math::Vec2{1.f, 1.f})});
  }
}

} // namespace blocks::game
