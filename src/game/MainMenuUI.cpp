#include "game/MainMenuUI.hpp"

#include <memory>
#include <optional>
#include "GlobalSubSystemStack.hpp"
#include "engine/Scene.hpp"
#include "game/UIActor.hpp"
#include "game/resource/DefaultFont.hpp"
#include "math/vec.hpp"
#include "render/Font.hpp"
#include "render/RenderSubSystem.hpp"
#include "render/renderables/RenderableColor2D.hpp"
#include "ui/UIObject.hpp"
#include "ui/UIRect.hpp"
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

  auto& topChild =
      uiRoot->children_.emplace_back(util::makeNotNullUnique<ui::UIRect>(
          math::Vec4(0.0f, 1.0f, 0.0f, 0.5f), resourceSentinel->getColor()));

  auto& topLeftChild =
      topChild->children_.emplace_back(util::makeNotNullUnique<ui::UIRect>(
          math::Vec4(1.0f, 0.0f, 0.0f, 1.0f), resourceSentinel->getColor()));
  topLeftChild->maxWidth_ = 100;

  auto& topRightChild =
      topChild->children_.emplace_back(util::makeNotNullUnique<ui::UIRect>(
          math::Vec4(1.0f, 0.0f, 0.0f, 1.0f), resourceSentinel->getColor()));
  topRightChild->maxHeight_ = 100;

  uiRoot->children_.push_back(util::makeNotNullUnique<ui::UIRect>(
      math::Vec4(0.0f, 0.0f, 1.0f, 0.5f), resourceSentinel->getColor()));

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
