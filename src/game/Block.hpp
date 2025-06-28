#pragma once

#include "engine/DrawableRegistry.hpp"
#include "math/vec.hpp"
#include "physics/RectCollider.hpp"
#include "render/RenderSubSystem.hpp"

namespace blocks::game {

class Block : public physics::RectCollider, Drawable {
 public:
  Block(math::Vec2 p0, math::Vec2 p1);

  void draw() final;

 private:
  render::UniqueRenderableHandle sprite_;
};

} // namespace blocks::game
