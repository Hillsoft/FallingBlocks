#pragma once

#include <memory>
#include "util/debug.hpp"
#include "util/raii_helpers.hpp"

namespace blocks {

class Scene;

class Actor {
  [[no_unique_address]] util::no_copy_move noCopyMoveTag_;

 private:
  struct ControlBlock {
    bool isAlive = false;
  };

 public:
  template <typename TActor = Actor>
  class Ref {
   public:
    Ref(TActor* actor)
        : actor_(actor),
          control_(actor != nullptr ? actor->control_ : nullptr) {}

    template <typename TOtherActor>
    Ref<TOtherActor> to() const {
      return {dynamic_cast<TOtherActor*>(actor_)};
    }

    bool isAlive() const { return control_ != nullptr && control_->isAlive; }

    TActor& operator*() const {
      DEBUG_ASSERT(isAlive());
      return *actor_;
    }
    TActor* operator->() const {
      DEBUG_ASSERT(isAlive());
      return actor_;
    }

   private:
    TActor* actor_;
    std::shared_ptr<ControlBlock> control_;
  };

  Actor(Scene& scene);
  virtual ~Actor() { control_->isAlive = false; }

  Ref<Actor> getRef() { return {this}; }
  Ref<const Actor> getRef() const { return this; }

  Scene* getScene() const { return scene_; }

 private:
  std::shared_ptr<ControlBlock> control_;
  Scene* scene_;
};

} // namespace blocks
