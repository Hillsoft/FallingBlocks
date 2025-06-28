#pragma once

#include "engine/DrawableRegistry.hpp"
#include "engine/TickRegistry.hpp"
#include "input/InputHandler.hpp"
#include "math/vec.hpp"
#include "physics/RectCollider.hpp"
#include "render/RenderSubSystem.hpp"

namespace blocks::game {

class Paddle
    : public input::InputHandler,
      public physics::RectCollider,
      public TickHandler,
      public Drawable {
 public:
  Paddle();

  void update(float deltaTimeSeconds) final;
  void draw() final;

  void onKeyPress(int key) final;
  void onKeyRelease(int key) final;

 private:
  math::Vec2 vel_;
  render::UniqueRenderableHandle sprite_;
};

} // namespace blocks::game
