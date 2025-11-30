#include "engine/Scene.hpp"

#include <algorithm>
#include <chrono>
#include <memory>
#include <utility>
#include "GlobalSubSystemStack.hpp"
#include "engine/Actor.hpp"
#include "util/debug.hpp"

namespace blocks {

Scene::~Scene() {
  GlobalSubSystemStack::get().inputSystem().unsetActiveRegistry(&input_);
}

void Scene::destroyActor(Actor* actor) {
  if (!actor->isAlive()) {
    return;
  }
  actor->onDestroy();

  auto it = std::find_if(
      actors_.begin(), actors_.end(), [actor](const auto& uniq_ptr) {
        return actor == uniq_ptr.get();
      });
  DEBUG_ASSERT(it != actors_.end());
  if (it != actors_.end()) {
    std::shared_ptr<Actor> deleted{std::move(*it)};
    actors_.erase(it);
    pendingDestruction_.emplace_back(std::move(deleted));
  }
}

void Scene::stepSimulation(std::chrono::microseconds deltaTime) {
  DEBUG_ASSERT(isActive_);

  const float deltaTimeSeconds =
      static_cast<float>(deltaTime.count()) / 1000000.f;

  timer_.tick(deltaTime);

  getTickRegistry().update(deltaTimeSeconds);

  getPhysicsScene().run();

  cleanupPendingDestruction();
}

void Scene::drawAll() {
  getDrawableScene().drawAll();
}

void Scene::cleanupPendingDestruction() {
  for ([[maybe_unused]] const auto& actor : pendingDestruction_) {
    DEBUG_ASSERT(actor.use_count() == 1);
  }
  pendingDestruction_.clear();
}

void Scene::activate() {
  DEBUG_ASSERT(!isActive_);

  isActive_ = true;

  for (auto& actor : actors_) {
    actor->onActivate();
  }

  GlobalSubSystemStack::get().inputSystem().setActiveRegistry(&input_);
}

} // namespace blocks
