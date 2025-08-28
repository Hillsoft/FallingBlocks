#pragma once

#include "engine/Actor.hpp"
#include "engine/DrawableRegistry.hpp"
#include "engine/Scene.hpp"
#include "engine/TickRegistry.hpp"
#include "math/vec.hpp"
#include "physics/RectCollider.hpp"

namespace blocks::game {

class BallResourceSentinel {
 public:
  static void load();
  static void unload();
};

class Ball
    : public Actor,
      public physics::RectCollider,
      public TickHandler,
      public Drawable {
 public:
  Ball(Scene& scene, math::Vec2 pos, math::Vec2 vel);
  Ball(Scene& scene);

  void update(float deltaTimeSeconds) final;
  void draw() final;

  void handleCollision(physics::RectCollider& other, math::Vec2 normal) final;

 private:
  math::Vec2 vel_;
};

} // namespace blocks::game
