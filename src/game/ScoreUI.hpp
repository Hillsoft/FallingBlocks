#pragma once

#include "engine/Actor.hpp"
#include "engine/DrawableRegistry.hpp"
#include "engine/Scene.hpp"
#include "render/Font.hpp"

namespace blocks::game {

class ScoreUI : public Actor, public Drawable {
 public:
  ScoreUI(Scene& scene);

  void draw() final;

 private:
  render::Font font_;
};

} // namespace blocks::game
