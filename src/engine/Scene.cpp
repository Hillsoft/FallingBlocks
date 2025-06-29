#include "engine/Scene.hpp"

#include "engine/Actor.hpp"

namespace blocks {

void Scene::destroyActor(Actor* actor) {
  auto it = std::find_if(
      actors_.begin(), actors_.end(), [actor](const auto& uniq_ptr) {
        return actor == uniq_ptr.get();
      });
  if (it != actors_.end()) {
    actors_.erase(it);
  }
}

} // namespace blocks
