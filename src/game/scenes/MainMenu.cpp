#include "game/scenes/MainMenu.hpp"

#include <memory>
#include "GlobalSubSystemStack.hpp"
#include "engine/Scene.hpp"
#include "game/Background.hpp"
#include "game/Button.hpp"
#include "game/MainMenuUI.hpp"
#include "game/resource/DefaultFont.hpp"

namespace blocks::game {

std::unique_ptr<Scene> MainMenu::loadScene() const {
  GlobalSubSystemStack::get()
      .sentinelManager()
      .transitionToSentinelSet<
          game::BackgroundResourceSentinel,
          game::ButtonResourceSentinel,
          game::DefaultFontResourceSentinel,
          game::MainMenuUIResourceSentinel>();

  std::unique_ptr<Scene> scene = std::make_unique<Scene>();

  scene->createActor<game::Background>();

  scene->createActor<game::MainMenuUI>();

  return scene;
}

} // namespace blocks::game
