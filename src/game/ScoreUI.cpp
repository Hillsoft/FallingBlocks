#include "game/ScoreUI.hpp"

#include <string>
#include "GlobalSubSystemStack.hpp"
#include "engine/Actor.hpp"
#include "engine/DrawableRegistry.hpp"
#include "engine/Scene.hpp"
#include "game/BlocksScene.hpp"
#include "game/resource/DefaultFont.hpp"
#include "util/debug.hpp"
#include "util/string.hpp"

namespace blocks::game {

ScoreUI::ScoreUI(Scene& scene)
    : Actor(scene), Drawable(scene.getDrawableScene()) {}

void ScoreUI::draw() {
  BlocksScene* scene = dynamic_cast<BlocksScene*>(getScene());
  DEBUG_ASSERT(scene != nullptr);

  std::string scoreString = util::toString(
      GlobalSubSystemStack::get().localisationManager().getLocalisedString(
          "GAME_POINTS"),
      scene->getScore());

  DefaultFontResourceSentinel::get().drawStringUTF8(
      scoreString, {-1.0f, -0.8f}, 0.2f);
}

} // namespace blocks::game
