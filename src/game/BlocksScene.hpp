#pragma once

#include <memory>
#include <utility>
#include "engine/Scene.hpp"
#include "engine/SceneLoader.hpp"
#include "game/Ball.hpp"
#include "game/Block.hpp"

namespace blocks::game {

class BlocksScene : public Scene {
 public:
  BlocksScene() = default;

  void onBlockDestroyed(Block& block);
  void onBallDestroyed(Ball& ball);

  int getScore() const { return score_; }
  void setNextScene(std::unique_ptr<SceneLoader> next) {
    nextScene_ = std::move(next);
  }

 private:
  int score_ = 0;
  std::unique_ptr<SceneLoader> nextScene_ = nullptr;
};

} // namespace blocks::game
