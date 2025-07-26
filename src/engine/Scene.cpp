#include "engine/Scene.hpp"

#include <utility>
#include "engine/Actor.hpp"

namespace blocks {

void Scene::destroyActor(Actor* actor) {
  auto it = std::find_if(
      actors_.begin(), actors_.end(), [actor](const auto& uniq_ptr) {
        return actor == uniq_ptr.get();
      });
  if (it != actors_.end()) {
    std::shared_ptr<Actor> deleted{std::move(*it)};
    actors_.erase(it);
    pendingDestruction_.emplace_back(std::move(deleted));
  }
}

void Scene::stepSimulation(float deltaTimeSeconds) {
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

} // namespace blocks
