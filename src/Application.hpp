#pragma once

#include "game/Ball.hpp"
#include "game/Paddle.hpp"
#include "input/InputHandler.hpp"

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

  game::Paddle paddle_;
  game::Ball ball_;
};

} // namespace blocks
