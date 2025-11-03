#pragma once

#include <chrono>
#include <memory>
#include <vector>
#include "engine/Actor.hpp"
#include "engine/DrawableRegistry.hpp"
#include "engine/TickRegistry.hpp"
#include "engine/Timer.hpp"
#include "physics/PhysicsScene.hpp"
#include "util/meta_utils.hpp"

namespace blocks {

class Scene;

struct SceneObjectDefinition {
  using Fields = util::TArray<>;
  using SceneType = Scene;
};

class Scene {
 public:
  Scene(const SceneObjectDefinition& definition) {}
  Scene() {}

  virtual ~Scene() {}

  template <typename TActor, typename... TArgs>
  std::weak_ptr<TActor> createActor(TArgs... args) {
    auto actor = std::make_shared<TActor>(*this, std::forward<TArgs>(args)...);

    if (isActive_) {
      actor->onActivate();
    }

    std::weak_ptr<TActor> ref = actor;
    actors_.emplace_back(std::move(actor));
    return ref;
  }

  void destroyActor(Actor* actor);

  void stepSimulation(std::chrono::microseconds deltaTime);
  void drawAll();

  physics::PhysicsScene& getPhysicsScene() { return physics_; }
  const physics::PhysicsScene& getPhysicsScene() const { return physics_; }
  TickRegistry& getTickRegistry() { return tick_; }
  const TickRegistry& getTickRegistry() const { return tick_; }
  DrawableRegistry& getDrawableScene() { return drawableScene_; }
  const DrawableRegistry& getDrawableScene() const { return drawableScene_; }
  Timer& getTimer() { return timer_; }
  const Timer& getTimer() const { return timer_; }

  const std::vector<std::shared_ptr<Actor>> getActors() const {
    return actors_;
  }

  void activate();

 private:
  void cleanupPendingDestruction();

  physics::PhysicsScene physics_;
  TickRegistry tick_;
  DrawableRegistry drawableScene_;
  Timer timer_;

  std::vector<std::shared_ptr<Actor>> actors_;
  std::vector<std::shared_ptr<Actor>> pendingDestruction_;

  bool isActive_ = false;
};

} // namespace blocks
