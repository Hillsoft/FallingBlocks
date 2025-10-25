#include "game/ScoreUI.hpp"

#include <memory>
#include <string>
#include <utility>
#include "GlobalSubSystemStack.hpp"
#include "engine/Actor.hpp"
#include "engine/Scene.hpp"
#include "game/BlocksScene.hpp"
#include "game/UIActor.hpp"
#include "game/resource/DefaultFont.hpp"
#include "render/Font.hpp"
#include "ui/UIObject.hpp"
#include "ui/UIText.hpp"
#include "util/NotNull.hpp"
#include "util/debug.hpp"
#include "util/string.hpp"

namespace blocks::game {

ScoreUI::ScoreUI(Scene& scene) : UIActor(scene), scoreElement_(nullptr) {
  auto uiRoot = std::make_unique<ui::UIObject>();
  uiRoot->childLayoutDirection_ = ui::LayoutDirection::VERTICAL;
  uiRoot->sizeStrategy_ = ui::SizeStrategy::PAD;
  uiRoot->innerPadding_ = 10;

  auto scoreElement = util::makeNotNullUnique<ui::UIText>(
      DefaultFontResourceSentinel::get(), "", render::Font::Size::Line{100.f});
  scoreElement_ = scoreElement.get();
  scoreElement_->crossLayoutPosition_ = ui::Align::LEFT_TOP;
  scoreElement_->maxWidth_ = 0;

  uiRoot->children_.emplace_back(std::move(scoreElement));

  setUIObject(std::move(uiRoot));
}

void ScoreUI::draw() {
  BlocksScene* scene = dynamic_cast<BlocksScene*>(getScene());
  DEBUG_ASSERT(scene != nullptr);

  std::string scoreString = util::toString(
      GlobalSubSystemStack::get().localisationManager().getLocalisedString(
          "GAME_POINTS"),
      scene->getScore());

  scoreElement_->setText(std::move(scoreString));

  UIActor::draw();
}

} // namespace blocks::game
