#pragma once

#include "math/vec.hpp"
#include "physics/PhysicsScene.hpp"
#include "util/raii_helpers.hpp"

namespace blocks::physics {

class RectCollider : private util::no_copy_move {
 public:
  RectCollider(PhysicsScene& scene, math::Vec2 p0, math::Vec2 p1);
  ~RectCollider();

  bool testCollision(const RectCollider& other) const;

  virtual void handleCollision(RectCollider& other) {}

  math::Vec2 getP0() const { return p0_; }
  void setP0(math::Vec2 newP0) { p0_ = newP0; }

  math::Vec2 getP1() const { return p1_; }
  void setP1(math::Vec2 newP1) { p1_ = newP1; }

 private:
  physics::PhysicsScene* scene_;
  math::Vec2 p0_;
  math::Vec2 p1_;
};

} // namespace blocks::physics
