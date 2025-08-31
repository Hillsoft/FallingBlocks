#include "game/scenes/GameOver.hpp"

#include <memory>
#include "GlobalSubSystemStack.hpp"
#include "engine/Scene.hpp"
#include "game/Background.hpp"
#include "game/StaticText.hpp"
#include "game/resource/DefaultFont.hpp"
#include "math/vec.hpp"

namespace blocks::game {

std::unique_ptr<Scene> GameOver::loadScene() const {
  GlobalSubSystemStack::get()
      .sentinelManager()
      .transitionToSentinelSet<
          game::BackgroundResourceSentinel,
          game::DefaultFontResourceSentinel>();

  std::unique_ptr<Scene> scene = std::make_unique<Scene>();

  scene->createActor<game::Background>();

  scene->createActor<game::StaticText>(math::Vec2{0.0f, 0.0f}, "Game Over");

  return scene;
}

} // namespace blocks::game
