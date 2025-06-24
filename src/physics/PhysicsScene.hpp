#pragma once

#include <vector>
#include "util/raii_helpers.hpp"

namespace blocks::physics {

class RectCollider;

class PhysicsScene : private util::no_copy_move {
 public:
  PhysicsScene();
  ~PhysicsScene();

  void registerCollider(RectCollider& collider);
  void unregisterCollider(RectCollider& collider);

  void run();

 private:
  std::vector<RectCollider*> colliders_;
};

} // namespace blocks::physics
