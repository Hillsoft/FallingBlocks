#pragma once

#include <memory>
#include "util/portability.hpp"
#include "util/raii_helpers.hpp"

namespace blocks {

class Scene;

class Actor : public std::enable_shared_from_this<Actor> {
  UNUSEDPRIVATEMEMBER(NO_UNIQUE_ADDRESS util::no_copy_move noCopyMoveTag_);

 public:
  Actor(Scene& scene);
  virtual ~Actor() = default;

  std::weak_ptr<Actor> getRef() { return {weak_from_this()}; }
  std::weak_ptr<const Actor> getRef() const { return weak_from_this(); }

  Scene* getScene() const { return scene_; }

  bool isAlive() const { return alive_; }
  virtual void onDestroy() { alive_ = false; }

 private:
  Scene* scene_;
  bool alive_ = true;
};

} // namespace blocks
