#pragma once

#include <memory>
#include "engine/Scene.hpp"
#include "engine/SceneLoader.hpp"

namespace blocks::game {

class Level2 : public SceneLoader {
  std::unique_ptr<Scene> loadScene() const final;
};

} // namespace blocks::game
