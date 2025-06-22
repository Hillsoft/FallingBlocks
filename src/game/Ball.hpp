#pragma once

#include "math/vec.hpp"
#include "render/RenderSubSystem.hpp"

namespace blocks::game {

class Ball {
 public:
  Ball();

  void update(float deltaTimeSeconds);
  void draw();

 private:
  math::Vec2 pos_;
  math::Vec2 vel_;
  render::UniqueRenderableHandle sprite_;
};

} // namespace blocks::game
