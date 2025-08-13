#include "game/Background.hpp"

#include "GlobalSubSystemStack.hpp"
#include "engine/Actor.hpp"
#include "engine/DrawableRegistry.hpp"
#include "engine/Scene.hpp"
#include "math/vec.hpp"
#include "render/renderables/RenderableTex2D.hpp"

namespace blocks::game {

Background::Background(Scene& scene)
    : Actor(scene),
      Drawable(scene.getDrawableScene()),
      sprite_(GlobalSubSystemStack::get()
                  .renderSystem()
                  .createRenderable<render::RenderableTex2D>(
                      RESOURCE_DIR "/background.png")) {}

void Background::draw() {
  auto& render = GlobalSubSystemStack::get().renderSystem();
  auto window = GlobalSubSystemStack::get().window();

  render.drawObject(
      window,
      *sprite_,
      {math::modelMatrixFromBounds(
          math::Vec2{-1.0f, -1.0f}, math::Vec2{1.0f, 1.0f})});
}

} // namespace blocks::game
