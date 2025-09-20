#include "game/BlocksScene.hpp"

#include <memory>
#include <utility>
#include "Application.hpp"
#include "game/Ball.hpp"
#include "game/Block.hpp"
#include "game/scenes/GameOver.hpp"

namespace blocks::game {

void BlocksScene::onBlockDestroyed(Block& block) {
  score_++;

  // check if there are remaining blocks
  const auto& actors = getActors();
  for (const auto& actor : actors) {
    if (dynamic_cast<const Block*>(actor.get()) != nullptr &&
        actor.get() != &block) {
      return;
    }
  }
  Application::getApplication().transitionToScene(std::move(nextScene_));
}

void BlocksScene::onBallDestroyed(Ball& ball) {
  // check if there are remaining balls
  const auto& actors = getActors();
  for (const auto& actor : actors) {
    if (dynamic_cast<const Ball*>(actor.get()) != nullptr &&
        actor.get() != &ball) {
      return;
    }
  }
  Application::getApplication().transitionToScene(std::make_unique<GameOver>());
}

} // namespace blocks::game
