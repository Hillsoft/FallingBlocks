#pragma once

#include <memory>
#include "engine/Scene.hpp"

namespace blocks {

class SceneLoader {
 public:
  virtual ~SceneLoader() = default;

  virtual std::unique_ptr<Scene> loadScene() const = 0;
};

} // namespace blocks
