#include "physics/PhysicsScene.hpp"

#include <optional>
#include "math/vec.hpp"
#include "physics/RectCollider.hpp"
#include "util/debug.hpp"

namespace blocks::physics {

PhysicsScene::PhysicsScene() {}

void PhysicsScene::run() {
  auto collidersLock = getRegisteredItems();
  const auto& colliders = *collidersLock;
  for (size_t i = 0; i < colliders.size(); i++) {
    for (size_t j = i + 1; j < colliders.size(); j++) {
      std::optional<math::Vec2> norm1 =
          colliders[i]->testCollision(*colliders[j]);
      if (norm1.has_value()) {
        if ((colliders[i]->getReceiveEventLayers() &
             colliders[j]->getProduceEventLayers()) > 0) {
          colliders[i]->handleCollision(*colliders[j], *norm1);
        }
        if ((colliders[j]->getReceiveEventLayers() &
             colliders[i]->getProduceEventLayers()) > 0) {
          std::optional<math::Vec2> norm2 =
              colliders[j]->testCollision(*colliders[i]);
          DEBUG_ASSERT(norm2.has_value());
          colliders[j]->handleCollision(*colliders[i], *norm2);
        }
      }
    }
  }
}

} // namespace blocks::physics
