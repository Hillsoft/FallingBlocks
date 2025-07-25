#include "Application.hpp"

#include <chrono>
#include <iostream>
#include <thread>
#include <utility>
#include <GLFW/glfw3.h>
#include "GlobalSubSystemStack.hpp"
#include "engine/Scene.hpp"
#include "game/Ball.hpp"
#include "game/Block.hpp"
#include "game/LoadingScreen.hpp"
#include "game/Paddle.hpp"
#include "input/InputHandler.hpp"
#include "math/vec.hpp"

namespace blocks {

namespace {

void populateMainScene(Scene& scene) {
  auto start = std::chrono::high_resolution_clock::now();

  scene.createActor<game::Paddle>();
  scene.createActor<game::Ball>();

  for (int i = 0; i < 10; i++) {
    float x = (2.f * static_cast<float>(i) / 10.f) - 1.f;
    for (int j = 0; j < 4; j++) {
      float y = static_cast<float>(j) * 0.1f - 1.f;
      scene.createActor<game::Block>(
          math::Vec2{x, y}, math::Vec2{x + 0.2f, y + 0.1f});
    }
  }

  auto end = std::chrono::high_resolution_clock::now();
  std::cout
      << "Loaded scene in "
      << std::chrono::duration_cast<std::chrono::milliseconds>(end - start)
             .count()
      << "ms" << std::endl;
}

} // namespace

Application::Application()
    : input::InputHandler(GlobalSubSystemStack::get().inputSystem()),
      currentScene_(&loadingScene_),
      loadThread_() {
  loadingScene_.createActor<game::LoadingScreen>();
}

void Application::run() {
  if (currentScene_ != &mainScene_) {
    loadThread_ = std::jthread{[&]() {
      populateMainScene(mainScene_);
      loadComplete_.store(true, std::memory_order_release);
    }};
  }

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

      if (currentScene_ != &mainScene_ &&
          loadComplete_.load(std::memory_order_acquire)) {
        currentScene_ = &mainScene_;
      }
    }

    std::cout << "FPS: " << 1000000.0f / (totalFrameTime / 1000).count()
              << "\nAverage frame time: " << totalFrameTime / 1000
              << "\nMax frame time: " << maxFrameTime << "\n";
  }

  subsystems.renderSystem().waitIdle();
}

void Application::update(float deltaTimeSeconds) {
  currentScene_->stepSimulation(deltaTimeSeconds);
}

void Application::drawFrame() {
  auto& render = GlobalSubSystemStack::get().renderSystem();
  currentScene_->drawAll();
  render.commitFrame();
}

void Application::onKeyPress(int key) {
  if (key == GLFW_KEY_SPACE) {
    for (int i = 0; i < 100; i++)
      currentScene_->createActor<game::Ball>();
  }
  if (key == GLFW_KEY_ESCAPE) {
    GlobalSubSystemStack::get().window()->close();
  }
}

} // namespace blocks
