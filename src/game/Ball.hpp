#pragma once

#include "engine/DrawableRegistry.hpp"
#include "engine/TickRegistry.hpp"
#include "math/vec.hpp"
#include "physics/RectCollider.hpp"
#include "render/RenderSubSystem.hpp"

namespace blocks::game {

class Ball : public physics::RectCollider, public TickHandler, public Drawable {
 public:
  Ball();

  void update(float deltaTimeSeconds) final;
  void draw() final;

  void handleCollision(physics::RectCollider& other) final;

 private:
  math::Vec2 vel_;
  render::UniqueRenderableHandle sprite_;
};

} // namespace blocks::game
