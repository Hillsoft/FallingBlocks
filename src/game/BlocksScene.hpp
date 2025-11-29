#pragma once

#include <string>
#include "engine/Scene.hpp"
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
  explicit BlocksScene(const BlocksSceneDefinition& definition)
      : nextScene_(definition.nextLevelName_) {}
  BlocksScene() = default;

  void onBlockDestroyed(Block& block);
  void onBallDestroyed(Ball& ball);

  int getScore() const { return score_; }

 private:
  int score_ = 0;
  std::string nextScene_;
};

} // namespace blocks::game
