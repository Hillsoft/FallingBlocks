#include "game/GameOverUI.hpp"

#include <memory>
#include <optional>
#include <vector>
#include "GlobalSubSystemStack.hpp"
#include "engine/Scene.hpp"
#include "game/UIActor.hpp"
#include "game/resource/DefaultFont.hpp"
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

std::unique_ptr<ui::UIObject> makeUI() {
  auto& localisation = GlobalSubSystemStack::get().localisationManager();

  auto uiRoot = std::make_unique<ui::UIObject>();
  uiRoot->childLayoutDirection_ = ui::LayoutDirection::VERTICAL;

  uiRoot->children_.emplace_back(util::makeNotNullUnique<ui::UIText>(
      DefaultFontResourceSentinel::get(),
      localisation.getLocalisedString("GAME_OVER"),
      render::Font::Size::Line{160.f}));
  uiRoot->children_.emplace_back(util::makeNotNullUnique<ui::UIText>(
      DefaultFontResourceSentinel::get(),
      localisation.getLocalisedString("PLAY_AGAIN"),
      render::Font::Size::Line{50.f}));

  return uiRoot;
}

} // namespace

void GameOverUIResourceSentinel::load() {
  DEBUG_ASSERT(!resourceSentinel.has_value());
  resourceSentinel.emplace();
}

void GameOverUIResourceSentinel::unload() {
  resourceSentinel.reset();
}

GameOverUI::GameOverUI(Scene& scene) : UIActor(scene, makeUI()) {}

} // namespace blocks::game
