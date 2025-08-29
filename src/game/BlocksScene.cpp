#include "game/BlocksScene.hpp"

#include <utility>
#include "Application.hpp"
#include "game/Block.hpp"

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

} // namespace blocks::game
