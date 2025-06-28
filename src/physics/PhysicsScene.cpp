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
        if ((colliders[i]->getReceiveEventLayers() &
             colliders[j]->getProduceEventLayers()) > 0) {
          colliders[i]->handleCollision(*colliders[j]);
        }
        if ((colliders[j]->getReceiveEventLayers() &
             colliders[i]->getProduceEventLayers()) > 0) {
          colliders[j]->handleCollision(*colliders[i]);
        }
      }
    }
  }
}

} // namespace blocks::physics
