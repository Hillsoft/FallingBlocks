#include "game/BlocksScene.hpp"

namespace blocks::game {

void BlocksScene::onBlockDestroyed() {
  score_++;
}

} // namespace blocks::game
