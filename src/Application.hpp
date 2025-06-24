#pragma once

#include <memory>
#include <vector>
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
  std::vector<std::unique_ptr<game::Ball>> balls_;
};

} // namespace blocks
