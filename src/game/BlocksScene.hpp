#pragma once

#include <memory>
#include <string>
#include <utility>
#include "engine/Scene.hpp"
#include "engine/SceneLoader.hpp"
#include "game/Ball.hpp"
#include "game/Block.hpp"
#include "util/meta_utils.hpp"

namespace blocks::game {

class BlocksScene;

struct BlocksSceneDefinition {
  std::string nextLevelName_;

  using Fields =
      util::TArray<util::TPair<util::TString<"nextLevelName">, std::string>>;
  using SceneType = BlocksScene;
};

class BlocksScene : public Scene {
 public:
  BlocksScene(const BlocksSceneDefinition& definition) {}
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
