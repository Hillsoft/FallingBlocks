#pragma once

#include <memory>
#include "engine/Scene.hpp"
#include "engine/SceneLoader.hpp"

namespace blocks::game {

class MainMenu : public SceneLoader {
  std::unique_ptr<Scene> loadScene() const final;
};

} // namespace blocks::game
