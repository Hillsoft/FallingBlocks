#pragma once

#include "input/InputHandler.hpp"
#include "math/vec.hpp"
#include "render/RenderSubSystem.hpp"

namespace blocks::game {

class Paddle : public input::InputHandler {
 public:
  Paddle();

  void update(float deltaTimeSeconds);
  void draw();

  void onKeyPress(int key) final;
  void onKeyRelease(int key) final;

 private:
  math::Vec2 pos_;
  math::Vec2 vel_;
  render::UniqueRenderableHandle sprite_;
};

} // namespace blocks::game
