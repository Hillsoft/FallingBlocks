#pragma once

#include "engine/Actor.hpp"
#include "engine/DrawableRegistry.hpp"
#include "engine/Scene.hpp"
#include "engine/TickRegistry.hpp"
#include "input/InputHandler.hpp"
#include "math/vec.hpp"
#include "physics/RectCollider.hpp"
#include "render/RenderSubSystem.hpp"
#include "render/renderables/RenderableTex2D.hpp"

namespace blocks::game {

class Paddle
    : public Actor,
      public input::InputHandler,
      public physics::RectCollider,
      public TickHandler,
      public Drawable {
 public:
  Paddle(Scene& scene);

  void update(float deltaTimeSeconds) final;
  void draw() final;

  void onKeyPress(int key) final;
  void onKeyRelease(int key) final;

 private:
  math::Vec2 vel_;
  render::UniqueRenderableHandle<render::RenderableTex2D::InstanceData> sprite_;
};

} // namespace blocks::game
