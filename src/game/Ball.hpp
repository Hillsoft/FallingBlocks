#pragma once

#include "math/vec.hpp"
#include "physics/RectCollider.hpp"
#include "render/RenderSubSystem.hpp"

namespace blocks::game {

class Ball : public physics::RectCollider {
 public:
  Ball();

  void update(float deltaTimeSeconds);
  void draw();

  void handleCollision(physics::RectCollider& other) final;

 private:
  math::Vec2 vel_;
  render::UniqueRenderableHandle sprite_;
};

} // namespace blocks::game
