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
    std::unique_ptr<Actor> deleted{std::move(*it)};
    actors_.erase(it);
    pendingDestruction_.emplace_back(std::move(deleted));
  }
}

void Scene::cleanupPendingDestruction() {
  pendingDestruction_.clear();
}

} // namespace blocks
