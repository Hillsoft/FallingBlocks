#pragma once

#include "engine/Actor.hpp"
#include "engine/DrawableRegistry.hpp"
#include "engine/Scene.hpp"
#include "render/Font.hpp"

namespace blocks::game {

class TestText : public Actor, public Drawable {
 public:
  explicit TestText(Scene& scene);

  void draw() final;

 protected:
  render::Font font_;
};

} // namespace blocks::game
