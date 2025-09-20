#include "game/StaticText.hpp"

#include <string>
#include <utility>
#include "engine/Actor.hpp"
#include "engine/DrawableRegistry.hpp"
#include "engine/Scene.hpp"
#include "game/resource/DefaultFont.hpp"
#include "math/vec.hpp"

namespace blocks::game {

StaticText::StaticText(
    Scene& scene, math::Vec2 pos, float lineHeight, std::string text)
    : Actor(scene),
      Drawable(scene.getDrawableScene()),
      pos_(pos),
      lineHeight_(lineHeight),
      text_(std::move(text)) {}

void StaticText::draw() {
  DefaultFontResourceSentinel::get().drawStringUTF8(
      text_, pos_, lineHeight_, render::Font::Align::CENTER);
}

} // namespace blocks::game
