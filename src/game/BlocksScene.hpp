#pragma once

#include "engine/Scene.hpp"

namespace blocks::game {

class BlocksScene : public Scene {
 public:
  BlocksScene() = default;

  void onBlockDestroyed();

  int getScore() const { return score_; }

 private:
  int score_ = 0;
};

} // namespace blocks::game
