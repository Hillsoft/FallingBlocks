#pragma once

#include "engine/Scene.hpp"
#include "game/UIActor.hpp"

namespace blocks::game {

class MainMenuUIResourceSentinel {
 public:
  static void load();
  static void unload();
};

class MainMenuUI : public UIActor {
 public:
  MainMenuUI(Scene& scene);
};

} // namespace blocks::game
