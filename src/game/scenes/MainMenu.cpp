#include "game/scenes/MainMenu.hpp"

#include <memory>
#include "Application.hpp"
#include "GlobalSubSystemStack.hpp"
#include "engine/Scene.hpp"
#include "game/Background.hpp"
#include "game/Button.hpp"
#include "game/StaticText.hpp"
#include "game/resource/DefaultFont.hpp"
#include "game/scenes/Level1.hpp"
#include "math/vec.hpp"

namespace blocks::game {

std::unique_ptr<Scene> MainMenu::loadScene() const {
  GlobalSubSystemStack::get()
      .sentinelManager()
      .transitionToSentinelSet<
          game::BackgroundResourceSentinel,
          game::ButtonResourceSentinel,
          game::DefaultFontResourceSentinel>();

  std::unique_ptr<Scene> scene = std::make_unique<Scene>();

  scene->createActor<game::Background>();

  scene->createActor<game::StaticText>(math::Vec2{0.0f, -0.4f}, 0.4f, "Blocks");
  scene->createActor<game::Button>(
      math::Vec2{-0.3f, -0.2f}, math::Vec2{0.3f, -0.05f}, "Play", []() {
        Application::getApplication().transitionToScene(
            std::make_unique<Level1>());
      });
  scene->createActor<game::Button>(
      math::Vec2{-0.3f, 0.0f}, math::Vec2{0.3f, 0.15f}, "Quit", []() {
        Application::getApplication().close();
      });

  return scene;
}

} // namespace blocks::game
