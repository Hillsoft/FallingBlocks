#include "game/TestText.hpp"

#include "GlobalSubSystemStack.hpp"
#include "engine/Actor.hpp"
#include "engine/DrawableRegistry.hpp"
#include "engine/Scene.hpp"
#include "loader/font/Font.hpp"
#include "math/vec.hpp"
#include "render/Font.hpp"

namespace blocks::game {

TestText::TestText(Scene& scene)
    : Actor(scene),
      Drawable(scene.getDrawableScene()),
      font_(
          GlobalSubSystemStack::get().renderSystem(),
          loader::loadFont(RESOURCE_DIR "/times.ttf")) {}

void TestText::draw() {
  font_.drawStringUTF8(
      "Hello, world! \xc4\xa9",
      math::Vec2{-1.0f, 0.0f},
      render::Font::Size::Em{0.2f});
}

} // namespace blocks::game
