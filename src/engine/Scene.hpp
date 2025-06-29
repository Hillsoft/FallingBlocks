#pragma once

#include <memory>
#include <vector>
#include "engine/Actor.hpp"

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

 private:
  std::vector<std::unique_ptr<Actor>> actors_;
  std::vector<std::unique_ptr<Actor>> pendingDestruction_;
};

} // namespace blocks
