#include "Application.hpp"

#include <chrono>
#include <iostream>
#include <memory>
#include <utility>
#include <GLFW/glfw3.h>
#include "GlobalSubSystemStack.hpp"
#include "engine/Scene.hpp"
#include "game/Ball.hpp"
#include "game/Block.hpp"
#include "game/Paddle.hpp"
#include "input/InputHandler.hpp"
#include "math/vec.hpp"

namespace blocks {

Application::Application()
    : input::InputHandler(GlobalSubSystemStack::get().inputSystem()) {
  scene_.createActor<game::Paddle>();
  scene_.createActor<game::Ball>();

  for (int i = 0; i < 10; i++) {
    float x = (2.f * static_cast<float>(i) / 10.f) - 1.f;
    for (int j = 0; j < 2; j++) {
      float y = static_cast<float>(j) * 0.1f - 1.f;
      scene_.createActor<game::Block>(
          math::Vec2{x, y}, math::Vec2{x + 0.2f, y + 0.1f});
    }
  }
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

  scene_.cleanupPendingDestruction();
}

void Application::drawFrame() {
  auto& render = GlobalSubSystemStack::get().renderSystem();
  GlobalSubSystemStack::get().drawableScene().drawAll();
  render.commitFrame();
}

void Application::onKeyPress(int key) {
  if (key == GLFW_KEY_SPACE) {
    for (int i = 0; i < 100; i++)
      scene_.createActor<game::Ball>();
  }
  if (key == GLFW_KEY_ESCAPE) {
    GlobalSubSystemStack::get().window()->close();
  }
}

} // namespace blocks
