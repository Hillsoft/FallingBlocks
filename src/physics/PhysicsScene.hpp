#pragma once

#include "util/Registry.hpp"

namespace blocks::physics {

class RectCollider;

class PhysicsScene : public util::Registry<RectCollider, PhysicsScene> {
 public:
  PhysicsScene();

  void run();
};

} // namespace blocks::physics
