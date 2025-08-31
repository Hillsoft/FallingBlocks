#pragma once

#include "engine/Actor.hpp"
#include "engine/DrawableRegistry.hpp"
#include "engine/Scene.hpp"

namespace blocks::game {

class ScoreUI : public Actor, public Drawable {
 public:
  ScoreUI(Scene& scene);

  void draw() final;
};

} // namespace blocks::game
