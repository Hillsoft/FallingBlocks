#include "game/ScoreUI.hpp"

#include <string>
#include "GlobalSubSystemStack.hpp"
#include "engine/Actor.hpp"
#include "engine/DrawableRegistry.hpp"
#include "engine/Scene.hpp"
#include "game/BlocksScene.hpp"
#include "loader/font/Font.hpp"
#include "util/debug.hpp"
#include "util/string.hpp"

namespace blocks::game {

ScoreUI::ScoreUI(Scene& scene)
    : Actor(scene),
      Drawable(scene.getDrawableScene()),
      font_(
          GlobalSubSystemStack::get().renderSystem(),
          loader::loadFont(RESOURCE_DIR "/times.ttf")) {}

void ScoreUI::draw() {
  BlocksScene* scene = dynamic_cast<BlocksScene*>(getScene());
  DEBUG_ASSERT(scene != nullptr);

  std::string scoreString = util::toString("Score: ", scene->getScore());

  font_.drawStringUTF8(scoreString, {-1.0f, -0.8f});
}

} // namespace blocks::game
