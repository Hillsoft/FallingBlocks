#pragma once

#include "engine/Actor.hpp"
#include "engine/DrawableRegistry.hpp"
#include "engine/Scene.hpp"

namespace blocks::game {

class BackgroundResourceSentinel {
 public:
  static void load();
  static void unload();
};

class Background : public Actor, public Drawable {
 public:
  Background(Scene& scene);

  void draw() final;
};

} // namespace blocks::game
