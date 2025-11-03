#include "Application.hpp"

#include <algorithm>
#include <chrono>
#include <iostream>
#include <memory>
#include <string>
#include <thread>
#include <utility>
#include <GLFW/glfw3.h>
#include "GlobalSubSystemStack.hpp"
#include "engine/Scene.hpp"
#include "engine/SceneLoader.hpp"
#include "game/LoadingScreen.hpp"
#include "input/InputHandler.hpp"
#include "util/debug.hpp"

namespace blocks {

namespace {

constexpr std::chrono::milliseconds kMinLoadTime{500};

Application* currentApplication = nullptr;

} // namespace

Application::Application()
    : input::InputHandler(GlobalSubSystemStack::get().inputSystem()),
      currentScene_(&loadingScene_),
      loadThread_() {
  DEBUG_ASSERT(currentApplication == nullptr);
  currentApplication = this;
  loadingScene_.createActor<game::LoadingScreen>();
}

Application::~Application() {
  currentApplication = nullptr;
}

Application& Application::getApplication() {
  DEBUG_ASSERT(currentApplication != nullptr);
  return *currentApplication;
}

void Application::run() {
  if (currentScene_ != mainScene_.get()) {
    transitionToScene("Scene_MainMenu");
  }

  auto& subsystems = GlobalSubSystemStack::get();
  std::chrono::microseconds prevFrameTime{0};
  while (!subsystems.window()->shouldClose()) {
    std::chrono::microseconds maxFrameTime{0};
    std::chrono::microseconds minFrameTime{9999999};
    std::chrono::microseconds totalFrameTime{0};

    for (int i = 0; i < 1000 && !subsystems.window()->shouldClose(); i++) {
      auto start = std::chrono::high_resolution_clock::now();
      glfwPollEvents();
      update(prevFrameTime);
      drawFrame();
      auto end = std::chrono::high_resolution_clock::now();

      std::chrono::microseconds curFrameTime =
          std::chrono::duration_cast<std::chrono::microseconds>(end - start);
      totalFrameTime += curFrameTime;
      maxFrameTime = std::max(maxFrameTime, curFrameTime);
      minFrameTime = std::min(minFrameTime, curFrameTime);

      prevFrameTime = curFrameTime;

      if (hasPendingScene_.load(std::memory_order_acquire)) {
        currentScene_ = pendingScene_;
        hasPendingScene_.store(false, std::memory_order_relaxed);
      }
    }

    std::cout << "FPS: " << 1000000.0f / (totalFrameTime / 1000).count()
              << "\nAverage frame time: " << totalFrameTime / 1000
              << "\nMax frame time: " << maxFrameTime
              << "\nMin frame time: " << minFrameTime << "\n";
  }

  subsystems.renderSystem().waitIdle();
}

void Application::transitionToScene(std::string sceneName) {
  loadThread_ = std::jthread{[this, sceneName = std::move(sceneName)]() {
    // Ensure we are in the main scene
    while (hasPendingScene_.load(std::memory_order_relaxed)) {
    }

    // Switch to transition scene
    pendingScene_ = &loadingScene_;
    hasPendingScene_.store(true, std::memory_order_release);

    while (hasPendingScene_.load(std::memory_order_relaxed)) {
    }

    mainScene_.reset();

    auto loadStart = std::chrono::high_resolution_clock::now();

    mainScene_ = loadSceneFromName(sceneName);

    auto loadEnd = std::chrono::high_resolution_clock::now();
    auto loadTime = std::chrono::duration_cast<std::chrono::milliseconds>(
        loadEnd - loadStart);
    std::cout << "Loaded scene " << sceneName << " in " << loadTime.count()
              << "ms" << std::endl;

    if (loadTime < kMinLoadTime) {
      std::this_thread::sleep_for(kMinLoadTime - loadTime);
    }

    pendingScene_ = mainScene_.get();
    hasPendingScene_.store(true, std::memory_order_release);
  }};
}

void Application::update(std::chrono::microseconds deltaTime) {
  currentScene_->stepSimulation(deltaTime);
}

void Application::drawFrame() {
  auto& render = GlobalSubSystemStack::get().renderSystem();
  currentScene_->drawAll();
  render.commitFrame();
}

void Application::onKeyPress(int key) {
  if (key == GLFW_KEY_ESCAPE) {
    close();
  }
  if (key == GLFW_KEY_F) {
    GlobalSubSystemStack::get().window()->toggleFullScreen();
  }
}

void Application::close() {
  GlobalSubSystemStack::get().window()->close();
}

} // namespace blocks
