#pragma once

#include <string>
#include "engine/Actor.hpp"
#include "engine/DrawableRegistry.hpp"
#include "engine/Scene.hpp"
#include "math/vec.hpp"

namespace blocks::game {

class StaticText : public Actor, public Drawable {
 public:
  StaticText(Scene& scene, math::Vec2 pos, float lineHeight, std::string text);

  void draw() final;

 private:
  math::Vec2 pos_;
  float lineHeight_;
  std::string text_;
};

} // namespace blocks::game
