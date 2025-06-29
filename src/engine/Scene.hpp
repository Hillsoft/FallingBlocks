#pragma once

#include <memory>
#include <vector>
#include "engine/Actor.hpp"
#include "engine/DrawableRegistry.hpp"
#include "engine/TickRegistry.hpp"
#include "physics/PhysicsScene.hpp"

namespace blocks {

class Scene {
 public:
  Scene() {}

  template <typename TActor, typename... TArgs>
  Actor::Ref<TActor> createActor(TArgs... args) {
    auto actor = std::make_unique<TActor>(*this, std::forward<TArgs>(args)...);
    auto ref = actor->getRef().to<TActor>();
    actors_.emplace_back(std::move(actor));
    return ref;
  }

  void destroyActor(Actor* actor);
  void cleanupPendingDestruction();

  physics::PhysicsScene& getPhysicsScene() { return physics_; }
  const physics::PhysicsScene& getPhysicsScene() const { return physics_; }
  TickRegistry& getTickRegistry() { return tick_; }
  const TickRegistry& getTickRegistry() const { return tick_; }
  DrawableRegistry& getDrawableScene() { return drawableScene_; }
  const DrawableRegistry& getDrawableScene() const { return drawableScene_; }

 private:
  physics::PhysicsScene physics_;
  TickRegistry tick_;
  DrawableRegistry drawableScene_;

  std::vector<std::unique_ptr<Actor>> actors_;
  std::vector<std::unique_ptr<Actor>> pendingDestruction_;
};

} // namespace blocks
