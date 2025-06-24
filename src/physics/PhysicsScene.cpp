#include "physics/PhysicsScene.hpp"

#include <vector>
#include "physics/RectCollider.hpp"
#include "util/debug.hpp"

namespace blocks::physics {

PhysicsScene::PhysicsScene() {}

PhysicsScene::~PhysicsScene() {
  DEBUG_ASSERT(colliders_.empty());
}

void PhysicsScene::registerCollider(RectCollider& collider) {
  DEBUG_ASSERT(
      std::find(colliders_.begin(), colliders_.end(), &collider) ==
      colliders_.end());
  colliders_.push_back(&collider);
}

void PhysicsScene::unregisterCollider(RectCollider& collider) {
  decltype(colliders_)::iterator it;
  while ((it = std::find(colliders_.begin(), colliders_.end(), &collider)),
         it != colliders_.end()) {
    colliders_.erase(it);
  }
}

void PhysicsScene::run() {
  for (size_t i = 0; i < colliders_.size(); i++) {
    for (size_t j = i + 1; j < colliders_.size(); j++) {
      if (colliders_[i]->testCollision(*colliders_[j])) {
        colliders_[i]->handleCollision(*colliders_[j]);
        colliders_[j]->handleCollision(*colliders_[i]);
      }
    }
  }
}

} // namespace blocks::physics
