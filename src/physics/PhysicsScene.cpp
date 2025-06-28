#include "physics/PhysicsScene.hpp"

#include "physics/RectCollider.hpp"

namespace blocks::physics {

PhysicsScene::PhysicsScene() {}

void PhysicsScene::run() {
  auto collidersLock = getRegisteredItemsWrite();
  const auto& colliders = *collidersLock;
  for (size_t i = 0; i < colliders.size(); i++) {
    for (size_t j = i + 1; j < colliders.size(); j++) {
      if (colliders[i]->testCollision(*colliders[j])) {
        colliders[i]->handleCollision(*colliders[j]);
        colliders[j]->handleCollision(*colliders[i]);
      }
    }
  }
}

} // namespace blocks::physics
