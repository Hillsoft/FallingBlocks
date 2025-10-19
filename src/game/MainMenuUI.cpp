#include "game/MainMenuUI.hpp"

#include <algorithm>
#include <memory>
#include <optional>
#include <string>
#include <vector>
#include "Application.hpp"
#include "GlobalSubSystemStack.hpp"
#include "engine/Scene.hpp"
#include "game/UIActor.hpp"
#include "game/resource/DefaultFont.hpp"
#include "game/scenes/Level1.hpp"
#include "game/scenes/MainMenu.hpp"
#include "math/vec.hpp"
#include "render/Font.hpp"
#include "render/RenderSubSystem.hpp"
#include "render/renderables/RenderableColor2D.hpp"
#include "ui/UIButton.hpp"
#include "ui/UIObject.hpp"
#include "ui/UIText.hpp"
#include "util/NotNull.hpp"
#include "util/debug.hpp"

namespace blocks::game {

namespace {

class MainMenuUIResourceSentinelData {
 public:
  MainMenuUIResourceSentinelData()
      : color_(GlobalSubSystemStack::get()
                   .renderSystem()
                   .createRenderable<render::RenderableColor2D>()) {}

  render::RenderableRef<render::RenderableColor2D::InstanceData> getColor() {
    return color_.get();
  }

 private:
  render::UniqueRenderableHandle<render::RenderableColor2D::InstanceData>
      color_;
};

constinit std::optional<MainMenuUIResourceSentinelData> resourceSentinel;

std::unique_ptr<ui::UIObject> makeUI() {
  auto& localisation = GlobalSubSystemStack::get().localisationManager();

  auto uiRoot = std::make_unique<ui::UIObject>();
  uiRoot->childLayoutDirection_ = ui::LayoutDirection::VERTICAL;

  uiRoot->children_.emplace_back(util::makeNotNullUnique<ui::UIText>(
      DefaultFontResourceSentinel::get(),
      localisation.getLocalisedString("BLOCKS_TITLE"),
      render::Font::Size::Line{160.f}));

  auto& buttonsWrapper =
      uiRoot->children_.emplace_back(util::makeNotNullUnique<ui::UIObject>());
  buttonsWrapper->childLayoutDirection_ = ui::LayoutDirection::VERTICAL;
  buttonsWrapper->maxWidth_ = 350;
  buttonsWrapper->crossLayoutPosition_ = ui::Align::MIDDLE;

  auto& buttons = buttonsWrapper->children_.emplace_back(
      util::makeNotNullUnique<ui::UIObject>());
  buttons->childLayoutDirection_ = ui::LayoutDirection::VERTICAL;

  auto& startButton =
      buttons->children_.emplace_back(util::makeNotNullUnique<ui::UIButton>(
          GlobalSubSystemStack::get().inputSystem(),
          math::Vec4{0.f, 0.f, 0.f, 1.0f},
          math::Vec4{0.3f, 0.3f, 0.3f, 1.0f},
          resourceSentinel->getColor(),
          []() {
            Application::getApplication().transitionToScene(
                std::make_unique<Level1>());
          }));
  startButton->children_.emplace_back(util::makeNotNullUnique<ui::UIText>(
      DefaultFontResourceSentinel::get(),
      localisation.getLocalisedString("START_GAME_BUTTON"),
      render::Font::Size::Line{70.0f}));

  auto& languageButton =
      buttons->children_.emplace_back(util::makeNotNullUnique<ui::UIButton>(
          GlobalSubSystemStack::get().inputSystem(),
          math::Vec4{0.f, 0.f, 0.f, 1.0f},
          math::Vec4{0.3f, 0.3f, 0.3f, 1.0f},
          resourceSentinel->getColor(),
          []() {
            auto& localisation =
                GlobalSubSystemStack::get().localisationManager();
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
          }));
  languageButton->children_.emplace_back(util::makeNotNullUnique<ui::UIText>(
      DefaultFontResourceSentinel::get(),
      std::string{localisation.getLocaleName()},
      render::Font::Size::Line{70.0f}));

  auto& quitButton =
      buttons->children_.emplace_back(util::makeNotNullUnique<ui::UIButton>(
          GlobalSubSystemStack::get().inputSystem(),
          math::Vec4{0.f, 0.f, 0.f, 1.0f},
          math::Vec4{0.3f, 0.3f, 0.3f, 1.0f},
          resourceSentinel->getColor(),
          []() { Application::getApplication().close(); }));
  quitButton->children_.emplace_back(util::makeNotNullUnique<ui::UIText>(
      DefaultFontResourceSentinel::get(),
      localisation.getLocalisedString("QUIT_GAME_BUTTON"),
      render::Font::Size::Line{70.0f}));

  buttonsWrapper->children_.emplace_back(
      util::makeNotNullUnique<ui::UIObject>());

  return uiRoot;
}

} // namespace

void MainMenuUIResourceSentinel::load() {
  DEBUG_ASSERT(!resourceSentinel.has_value());
  resourceSentinel.emplace();
}

void MainMenuUIResourceSentinel::unload() {
  resourceSentinel.reset();
}

MainMenuUI::MainMenuUI(Scene& scene) : UIActor(scene, makeUI()) {}

} // namespace blocks::game
