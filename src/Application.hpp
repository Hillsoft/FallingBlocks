#pragma once

#include "engine/Scene.hpp"
#include "input/InputHandler.hpp"

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

  Scene scene_;
};

} // namespace blocks
