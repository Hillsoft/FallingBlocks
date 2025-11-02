#include "game/scenes/GameOver.hpp"

#include <chrono>
#include <memory>
#include <GLFW/glfw3.h>
#include "Application.hpp"
#include "GlobalSubSystemStack.hpp"
#include "engine/Actor.hpp"
#include "engine/Scene.hpp"
#include "engine/SceneLoader.hpp"
#include "game/Background.hpp"
#include "game/GameOverUI.hpp"
#include "game/resource/DefaultFont.hpp"
#include "input/InputHandler.hpp"

namespace blocks::game {

namespace {

class GameRestarter : public Actor, public input::InputHandler {
 public:
  GameRestarter(Scene& scene)
      : Actor(scene),
        input::InputHandler(GlobalSubSystemStack::get().inputSystem()) {}

  void onKeyRelease(int keyCode) final {
    if (keyCode == GLFW_KEY_SPACE) {
      Application::getApplication().transitionToScene(
          std::make_unique<SceneLoaderFromResourceFile>("Scene_Level1"));
    }
  }
};

void playGameOverSound(Scene& scene) {
  scene.getTimer().schedule(std::chrono::milliseconds{0}, [&scene]() {
    GlobalSubSystemStack::get().audioSystem().playSineWave(
        {.frequency = 165,
         .volume = 1.f,
         .duration = std::chrono::milliseconds{200}});

    scene.getTimer().schedule(std::chrono::milliseconds{400}, []() {
      GlobalSubSystemStack::get().audioSystem().playSineWave(
          {.frequency = 131,
           .volume = 1.f,
           .duration = std::chrono::milliseconds{200}});
    });

    scene.getTimer().schedule(std::chrono::milliseconds{800}, []() {
      GlobalSubSystemStack::get().audioSystem().playSineWave(
          {.frequency = 110,
           .volume = 1.f,
           .duration = std::chrono::milliseconds{400}});
    });
  });
}

} // namespace

std::unique_ptr<Scene> GameOver::loadScene() const {
  GlobalSubSystemStack::get()
      .sentinelManager()
      .transitionToSentinelSet<
          game::BackgroundResourceSentinel,
          game::GameOverUIResourceSentinel,
          game::DefaultFontResourceSentinel>();

  std::unique_ptr<Scene> scene = std::make_unique<Scene>();

  scene->createActor<game::Background>();

  scene->createActor<game::GameOverUI>();

  scene->createActor<GameRestarter>();

  playGameOverSound(*scene);

  return scene;
}

} // namespace blocks::game
