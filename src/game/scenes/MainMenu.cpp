#include "game/scenes/MainMenu.hpp"

#include <algorithm>
#include <memory>
#include <string>
#include <vector>
#include "Application.hpp"
#include "GlobalSubSystemStack.hpp"
#include "engine/Scene.hpp"
#include "game/Background.hpp"
#include "game/Button.hpp"
#include "game/MainMenuUI.hpp"
#include "game/resource/DefaultFont.hpp"
#include "game/scenes/Level1.hpp"
#include "math/vec.hpp"

namespace blocks::game {

std::unique_ptr<Scene> MainMenu::loadScene() const {
  auto& localisation = GlobalSubSystemStack::get().localisationManager();
  GlobalSubSystemStack::get()
      .sentinelManager()
      .transitionToSentinelSet<
          game::BackgroundResourceSentinel,
          game::ButtonResourceSentinel,
          game::DefaultFontResourceSentinel,
          game::MainMenuUIResourceSentinel>();

  std::unique_ptr<Scene> scene = std::make_unique<Scene>();

  scene->createActor<game::Background>();

  /*scene->createActor<game::StaticText>(
      math::Vec2{0.0f, -0.4f},
      0.4f,
      localisation.getLocalisedString("BLOCKS_TITLE"));*/
  scene->createActor<game::Button>(
      math::Vec2{-0.3f, -0.2f},
      math::Vec2{0.3f, -0.05f},
      localisation.getLocalisedString("START_GAME_BUTTON"),
      []() {
        Application::getApplication().transitionToScene(
            std::make_unique<Level1>());
      });
  scene->createActor<game::Button>(
      math::Vec2{-0.3f, 0.0f},
      math::Vec2{0.3f, 0.15f},
      std::string{localisation.getLocaleName()},
      []() {
        auto& localisation = GlobalSubSystemStack::get().localisationManager();
        std::vector<std::string> availableLocales =
            localisation.getAvailableLocales();
        auto currentLocaleIt = std::find(
            availableLocales.begin(),
            availableLocales.end(),
            localisation.getLocaleCode());
        if (currentLocaleIt != availableLocales.end()) {
          currentLocaleIt++;
        }
        if (currentLocaleIt == availableLocales.end()) {
          currentLocaleIt = availableLocales.begin();
        }
        localisation.setLocale(*currentLocaleIt);
        Application::getApplication().transitionToScene(
            std::make_unique<MainMenu>());
      });
  scene->createActor<game::Button>(
      math::Vec2{-0.3f, 0.2f},
      math::Vec2{0.3f, 0.35f},
      localisation.getLocalisedString("QUIT_GAME_BUTTON"),
      []() { Application::getApplication().close(); });

  scene->createActor<game::MainMenuUI>();

  return scene;
}

} // namespace blocks::game
