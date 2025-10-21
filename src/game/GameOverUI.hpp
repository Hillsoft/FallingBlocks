#pragma once

#include "engine/Scene.hpp"
#include "game/UIActor.hpp"

namespace blocks::game {

class GameOverUIResourceSentinel {
 public:
  static void load();
  static void unload();
};

class GameOverUI : public UIActor {
 public:
  GameOverUI(Scene& scene);
};

} // namespace blocks::game
