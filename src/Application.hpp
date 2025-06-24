#pragma once

#include <memory>
#include <vector>
#include "game/Ball.hpp"
#include "game/Paddle.hpp"
#include "input/InputHandler.hpp"
#include "util/raii_helpers.hpp"

namespace blocks {

class Application : public input::InputHandler {
 public:
  Application();
  ~Application() = default;

  void run();

  void onKeyPress(int key) final;

 private:
  void update(float deltaTimeSeconds);
  void drawFrame();

  game::Paddle paddle_;
  std::vector<std::unique_ptr<game::Ball>> balls_;
};

} // namespace blocks
