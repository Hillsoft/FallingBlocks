#include "game/MainMenuUI.hpp"

#include <algorithm>
#include <memory>
#include <string>
#include <vector>
#include "Application.hpp"
#include "GlobalSubSystemStack.hpp"
#include "engine/ResourceRef.hpp"
#include "engine/Scene.hpp"
#include "game/UIActor.hpp"
#include "input/InputSubSystem.hpp"
#include "math/vec.hpp"
#include "render/Font.hpp"
#include "render/RenderSubSystem.hpp"
#include "render/renderables/RenderableColor2D.hpp"
#include "ui/UIButton.hpp"
#include "ui/UIObject.hpp"
#include "ui/UIText.hpp"
#include "util/NotNull.hpp"

namespace blocks::game {

namespace {

std::unique_ptr<ui::UIObject> makeUI(
    render::RenderableRef<render::RenderableColor2D::InstanceData>
        colorRenderable,
    const render::Font& font,
    input::InputRegistry& inputRegistry) {
  auto& localisation = GlobalSubSystemStack::get().localisationManager();

  auto uiRoot = std::make_unique<ui::UIObject>();
  uiRoot->childLayoutDirection_ = ui::LayoutDirection::VERTICAL;

  auto& title = uiRoot->children_.emplace_back(
      util::makeNotNullUnique<ui::UIText>(
          font,
          localisation.getLocalisedString("BLOCKS_TITLE"),
          render::Font::Size::Line{160.f}));
  title->outerPadding_ = 20;

  auto& buttonsWrapper =
      uiRoot->children_.emplace_back(util::makeNotNullUnique<ui::UIObject>());
  buttonsWrapper->childLayoutDirection_ = ui::LayoutDirection::VERTICAL;
  buttonsWrapper->maxWidth_ = 350;
  buttonsWrapper->crossLayoutPosition_ = ui::Align::MIDDLE;

  auto& buttons = buttonsWrapper->children_.emplace_back(
      util::makeNotNullUnique<ui::UIObject>());
  buttons->childLayoutDirection_ = ui::LayoutDirection::VERTICAL;

  auto& startButton = buttons->children_.emplace_back(
      util::makeNotNullUnique<ui::UIButton>(
          inputRegistry,
          math::Vec4{0.f, 0.f, 0.f, 1.0f},
          math::Vec4{0.3f, 0.3f, 0.3f, 1.0f},
          colorRenderable,
          []() {
            Application::getApplication().transitionToScene("Scene_Level1");
          }));
  startButton->outerPadding_ = 5;
  startButton->innerPadding_ = 5;
  startButton->maxHeight_ = 0;
  startButton->children_.emplace_back(
      util::makeNotNullUnique<ui::UIText>(
          font,
          localisation.getLocalisedString("START_GAME_BUTTON"),
          render::Font::Size::Line{70.0f}));

  auto& languageButton = buttons->children_.emplace_back(
      util::makeNotNullUnique<ui::UIButton>(
          inputRegistry,
          math::Vec4{0.f, 0.f, 0.f, 1.0f},
          math::Vec4{0.3f, 0.3f, 0.3f, 1.0f},
          colorRenderable,
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
            Application::getApplication().transitionToScene("Scene_MainMenu");
          }));
  languageButton->outerPadding_ = 5;
  languageButton->innerPadding_ = 5;
  languageButton->maxHeight_ = 0;
  languageButton->children_.emplace_back(
      util::makeNotNullUnique<ui::UIText>(
          font,
          std::string{localisation.getLocaleName()},
          render::Font::Size::Line{70.0f}));

  auto& quitButton = buttons->children_.emplace_back(
      util::makeNotNullUnique<ui::UIButton>(
          inputRegistry,
          math::Vec4{0.f, 0.f, 0.f, 1.0f},
          math::Vec4{0.3f, 0.3f, 0.3f, 1.0f},
          colorRenderable,
          []() { Application::getApplication().close(); }));
  quitButton->outerPadding_ = 5;
  quitButton->innerPadding_ = 5;
  quitButton->maxHeight_ = 0;
  quitButton->children_.emplace_back(
      util::makeNotNullUnique<ui::UIText>(
          font,
          localisation.getLocalisedString("QUIT_GAME_BUTTON"),
          render::Font::Size::Line{70.0f}));

  buttonsWrapper->children_.emplace_back(
      util::makeNotNullUnique<ui::UIObject>());

  return uiRoot;
}

} // namespace

MainMenuUI::MainMenuUI(Scene& scene, const MainMenuUIDefinition& definition)
    : UIActor(
          scene,
          makeUI(
              definition.prototype->colorResource->get(),
              definition.prototype->fontResource->get(),
              scene.getInputRegistry())) {}

} // namespace blocks::game
