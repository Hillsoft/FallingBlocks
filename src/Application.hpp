#pragma once

#include "game/Paddle.hpp"
#include "input/InputHandler.hpp"
#include "math/vec.hpp"
#include "render/RenderSubSystem.hpp"

namespace blocks {

class Application : public input::InputHandler {
 public:
  Application();
  ~Application() = default;

  Application(const Application& other) = delete;
  Application(Application&& other) = delete;

  Application& operator=(const Application& other) = delete;
  Application& operator=(Application&& other) = delete;

  void run();

  void onKeyPress(int key) final;

 private:
  void update(float deltaTimeSeconds);
  void drawFrame();

  render::UniqueRenderableHandle renderable_;
  render::UniqueRenderableHandle renderable2_;

  game::Paddle paddle_;

  math::Vec2 pos1_;
  math::Vec2 v1_;
  math::Vec2 pos2_;
  math::Vec2 v2_;
};

} // namespace blocks
