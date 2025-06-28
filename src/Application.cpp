#include "Application.hpp"

#include <chrono>
#include <iostream>
#include <memory>
#include <utility>
#include <vector>
#include <GLFW/glfw3.h>
#include "GlobalSubSystemStack.hpp"
#include "game/Ball.hpp"
#include "input/InputHandler.hpp"

namespace blocks {

Application::Application()
    : input::InputHandler(GlobalSubSystemStack::get().inputSystem()),
      paddle_(),
      balls_() {
  balls_.push_back(std::make_unique<game::Ball>());
}

void Application::run() {
  auto& subsystems = GlobalSubSystemStack::get();
  float prevFrameTimeSecs = 0;
  while (!subsystems.window()->shouldClose()) {
    std::chrono::microseconds maxFrameTime{0};
    std::chrono::microseconds totalFrameTime{0};

    for (int i = 0; i < 1000 && !subsystems.window()->shouldClose(); i++) {
      auto start = std::chrono::high_resolution_clock::now();
      glfwPollEvents();
      update(prevFrameTimeSecs);
      drawFrame();
      auto end = std::chrono::high_resolution_clock::now();

      std::chrono::microseconds curFrameTime =
          std::chrono::duration_cast<std::chrono::microseconds>(end - start);
      totalFrameTime += curFrameTime;
      maxFrameTime = std::max(maxFrameTime, curFrameTime);

      prevFrameTimeSecs =
          std::chrono::duration_cast<std::chrono::microseconds>(curFrameTime)
              .count() /
          1000000.0f;
    }

    std::cout << "FPS: " << 1000000.0f / (totalFrameTime / 1000).count()
              << "\nAverage frame time: " << totalFrameTime / 1000
              << "\nMax frame time: " << maxFrameTime << "\n";
  }

  subsystems.renderSystem().waitIdle();
}

void Application::update(float deltaTimeSeconds) {
  GlobalSubSystemStack::get().tickSystem().update(deltaTimeSeconds);

  GlobalSubSystemStack::get().physicsScene().run();
}

void Application::drawFrame() {
  auto& render = GlobalSubSystemStack::get().renderSystem();
  GlobalSubSystemStack::get().drawableScene().drawAll();
  render.commitFrame();
}

void Application::onKeyPress(int key) {
  if (key == GLFW_KEY_SPACE) {
    for (int i = 0; i < 100; i++)
      balls_.emplace_back(std::make_unique<game::Ball>());
  }
  if (key == GLFW_KEY_ESCAPE) {
    GlobalSubSystemStack::get().window()->close();
  }
}

} // namespace blocks
