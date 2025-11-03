#include "game/GameOverUI.hpp"

#include <chrono>
#include <memory>
#include <optional>
#include <vector>
#include <GLFW/glfw3.h>
#include "Application.hpp"
#include "GlobalSubSystemStack.hpp"
#include "engine/Scene.hpp"
#include "engine/SceneLoader.hpp"
#include "game/UIActor.hpp"
#include "input/InputHandler.hpp"
#include "render/Font.hpp"
#include "ui/UIObject.hpp"
#include "ui/UIText.hpp"
#include "util/NotNull.hpp"
#include "util/debug.hpp"

namespace blocks::game {

namespace {

class GameOverUIResourceSentinelData {
 public:
  GameOverUIResourceSentinelData() {}
};

constinit std::optional<GameOverUIResourceSentinelData> resourceSentinel;

std::unique_ptr<ui::UIObject> makeUI(const render::Font& font) {
  auto& localisation = GlobalSubSystemStack::get().localisationManager();

  auto uiRoot = std::make_unique<ui::UIObject>();
  uiRoot->childLayoutDirection_ = ui::LayoutDirection::VERTICAL;
  uiRoot->sizeStrategy_ = ui::SizeStrategy::PAD;
  uiRoot->alongLayoutChildPosition_ = ui::Align::MIDDLE;

  uiRoot->children_.emplace_back(util::makeNotNullUnique<ui::UIText>(
      font,
      localisation.getLocalisedString("GAME_OVER"),
      render::Font::Size::Line{160.f}));
  uiRoot->children_.emplace_back(util::makeNotNullUnique<ui::UIText>(
      font,
      localisation.getLocalisedString("PLAY_AGAIN"),
      render::Font::Size::Line{50.f}));

  return uiRoot;
}

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

void GameOverUIResourceSentinel::load() {
  DEBUG_ASSERT(!resourceSentinel.has_value());
  resourceSentinel.emplace();
}

void GameOverUIResourceSentinel::unload() {
  resourceSentinel.reset();
}

GameOverUI::GameOverUI(Scene& scene, const GameOverUIDefinition& definition)
    : UIActor(scene, makeUI(definition.prototype->fontResource->get())),
      input::InputHandler(GlobalSubSystemStack::get().inputSystem()) {
  playGameOverSound(scene);
}

void GameOverUI::onKeyRelease(int keyCode) {
  if (keyCode == GLFW_KEY_SPACE) {
    Application::getApplication().transitionToScene(
        std::make_unique<SceneLoaderFromResourceFile>("Scene_Level1"));
  }
}

} // namespace blocks::game
