#include "Application.hpp"

#include <algorithm>
#include <chrono>
#include <iostream>
#include <memory>
#include <thread>
#include <utility>
#include <GLFW/glfw3.h>
#include "GlobalSubSystemStack.hpp"
#include "engine/Scene.hpp"
#include "game/Background.hpp"
#include "game/Ball.hpp"
#include "game/BallSpawnBlock.hpp"
#include "game/Block.hpp"
#include "game/BlocksScene.hpp"
#include "game/LoadingScreen.hpp"
#include "game/Paddle.hpp"
#include "game/ScoreUI.hpp"
#include "input/InputHandler.hpp"
#include "math/vec.hpp"
#include "util/debug.hpp"

namespace blocks {

namespace {

constexpr int kBlockXCount = 15;
constexpr float kBlockWidth = 2.f / kBlockXCount;
constexpr int kBlockYCount = 7;
constexpr float kBlockHeight = kBlockWidth / 2.f;

void populateMainScene(std::unique_ptr<Scene>& scene) {
  DEBUG_ASSERT(scene == nullptr);
  auto start = std::chrono::high_resolution_clock::now();

  scene = std::make_unique<game::BlocksScene>();

  scene->createActor<game::Background>();
  scene->createActor<game::Paddle>();
  scene->createActor<game::Ball>();

  for (int i = 0; i < kBlockXCount; i++) {
    float x = kBlockWidth * static_cast<float>(i) - 1.f;
    for (int j = 0; j < kBlockYCount; j++) {
      float y = static_cast<float>(j) * kBlockHeight - 1.f;
      if ((i + j) % 4 == 0) {
        scene->createActor<game::BallSpawnBlock>(
            math::Vec2{x, y}, math::Vec2{x + kBlockWidth, y + kBlockHeight});
      } else {
        scene->createActor<game::Block>(
            math::Vec2{x, y}, math::Vec2{x + kBlockWidth, y + kBlockHeight});
      }
    }
  }

  scene->createActor<game::ScoreUI>();

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
  if (currentScene_ != mainScene_.get()) {
    loadThread_ = std::jthread{[&]() {
      populateMainScene(mainScene_);
      loadComplete_.store(true, std::memory_order_release);
    }};
  }

  auto& subsystems = GlobalSubSystemStack::get();
  float prevFrameTimeSecs = 0;
  while (!subsystems.window()->shouldClose()) {
    std::chrono::microseconds maxFrameTime{0};
    std::chrono::microseconds minFrameTime{9999999};
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
      minFrameTime = std::min(minFrameTime, curFrameTime);

      prevFrameTimeSecs =
          std::chrono::duration_cast<std::chrono::microseconds>(curFrameTime)
              .count() /
          1000000.0f;

      if (currentScene_ != mainScene_.get() &&
          loadComplete_.load(std::memory_order_acquire)) {
        currentScene_ = mainScene_.get();
      }
    }

    std::cout << "FPS: " << 1000000.0f / (totalFrameTime / 1000).count()
              << "\nAverage frame time: " << totalFrameTime / 1000
              << "\nMax frame time: " << maxFrameTime
              << "\nMin frame time: " << minFrameTime << "\n";
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
