#include "game/scenes/Level2.hpp"

#include <memory>
#include "GlobalSubSystemStack.hpp"
#include "engine/Scene.hpp"
#include "game/Background.hpp"
#include "game/Ball.hpp"
#include "game/BallSpawnBlock.hpp"
#include "game/Block.hpp"
#include "game/BlocksScene.hpp"
#include "game/Paddle.hpp"
#include "game/ScoreUI.hpp"
#include "game/resource/DefaultFont.hpp"
#include "game/scenes/LevelConsts.hpp"
#include "math/vec.hpp"

namespace blocks::game {

std::unique_ptr<Scene> Level2::loadScene() const {
  GlobalSubSystemStack::get()
      .sentinelManager()
      .transitionToSentinelSet<
          game::BackgroundResourceSentinel,
          game::BallResourceSentinel,
          game::BlockResourceSentinel,
          game::BallSpawnBlockResourceSentinel,
          game::PaddleResourceSentinel,
          game::DefaultFontResourceSentinel>();

  std::unique_ptr<BlocksScene> scene = std::make_unique<game::BlocksScene>();

  scene->createActor<game::Background>();

  scene->createActor<game::Paddle>();
  scene->createActor<game::Ball>();

  for (int i = 0; i < kBlockXCount; i++) {
    float x = kBlockWidth * static_cast<float>(i) - 1.f;
    for (int j = 0; j < kBlockYCount; j++) {
      float y = (4 + static_cast<float>(j)) * kBlockHeight - 1.f;
      if ((i + j) % 4 == 0) {
        scene->createActor<game::BallSpawnBlock>(
            math::Vec2{x, y}, math::Vec2{x + kBlockWidth, y + kBlockHeight});
      } else {
        scene->createActor<game::Block>(
            math::Vec2{x, y}, math::Vec2{x + kBlockWidth, y + kBlockHeight});
      }
    }
  }

  scene->createActor<game::ScoreUI>();

  scene->setNextScene(std::make_unique<Level2>());

  return scene;
}

} // namespace blocks::game
