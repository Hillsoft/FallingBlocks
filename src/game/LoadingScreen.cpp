#include "game/LoadingScreen.hpp"

#include "GlobalSubSystemStack.hpp"
#include "engine/Actor.hpp"
#include "engine/DrawableRegistry.hpp"
#include "engine/Scene.hpp"
#include "engine/TickRegistry.hpp"
#include "math/vec.hpp"
#include "render/renderables/RenderableTex2D.hpp"

namespace blocks::game {

namespace {

constexpr float kSwitchTime = 1.f;

}

LoadingScreen::LoadingScreen(Scene& scene)
    : Actor(scene),
      Drawable(scene.getDrawableScene()),
      TickHandler(scene.getTickRegistry()),
      image1_(GlobalSubSystemStack::get()
                  .renderSystem()
                  .createRenderable<render::RenderableTex2D>(
                      RESOURCE_DIR "/loading0.png")),
      image2_(GlobalSubSystemStack::get()
                  .renderSystem()
                  .createRenderable<render::RenderableTex2D>(
                      RESOURCE_DIR "/loading1.png")) {}

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
        *image1_,
        {math::modelMatrixFromBounds(
            math::Vec2{-1.f, -1.f}, math::Vec2{1.f, 1.f})});
  } else {
    GlobalSubSystemStack::get().renderSystem().drawObject(
        window,
        *image2_,
        {math::modelMatrixFromBounds(
            math::Vec2{-1.f, -1.f}, math::Vec2{1.f, 1.f})});
  }
}

} // namespace blocks::game
