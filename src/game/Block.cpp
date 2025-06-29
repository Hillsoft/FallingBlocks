#include "game/Block.hpp"

#include "GlobalSubSystemStack.hpp"
#include "engine/Actor.hpp"
#include "engine/DrawableRegistry.hpp"
#include "engine/Scene.hpp"
#include "math/vec.hpp"
#include "physics/RectCollider.hpp"

namespace blocks::game {

Block::Block(Scene& scene, math::Vec2 p0, math::Vec2 p1)
    : Actor(scene),
      physics::RectCollider(
          GlobalSubSystemStack::get().physicsScene(), p0, p1, 0b10, 0),
      Drawable(GlobalSubSystemStack::get().drawableScene()),
      sprite_(GlobalSubSystemStack::get().renderSystem().createRenderable(
          RESOURCE_DIR "/mandelbrot set.png")) {}

void Block::draw() {
  auto& render = GlobalSubSystemStack::get().renderSystem();
  auto window = GlobalSubSystemStack::get().window();

  sprite_->setPosition(getP0(), getP1());
  render.drawObject(window, *sprite_);
}

} // namespace blocks::game
