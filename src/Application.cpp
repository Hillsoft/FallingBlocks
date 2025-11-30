#include "Application.hpp"

#include <algorithm>
#include <atomic>
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
#include "log/Logger.hpp"
#include "util/debug.hpp"
#include "util/string.hpp"

namespace blocks {

namespace {

constexpr std::chrono::milliseconds kMinLoadTime{500};

// NOLINTNEXTLINE(cppcoreguidelines-avoid-non-const-global-variables)
Application* currentApplication = nullptr;

} // namespace

Application::Application(
    std::string loadingSceneResourceName, std::string initialSceneResourceName)
    : loadingSceneDefinition_(
          loadSceneDefinitionFromName(std::move(loadingSceneResourceName))),
      initialSceneResourceName_(std::move(initialSceneResourceName)) {
  DEBUG_ASSERT(currentApplication == nullptr);
  currentApplication = this;
}

Application::~Application() {
  currentApplication = nullptr;
}

Application& Application::getApplication() {
  DEBUG_ASSERT(currentApplication != nullptr);
  return *currentApplication;
}

void Application::run() {
  if (currentScene_ == nullptr || currentScene_ != mainScene_.get()) {
    transitionToScene(initialSceneResourceName_);

    // wait for transition scene to be ready, main scene by null
    while (!hasPendingScene_.load(std::memory_order_relaxed)) {
    }
  }

  auto& subsystems = GlobalSubSystemStack::get();
  std::chrono::microseconds prevFrameTime{0};
  while (!subsystems.window()->shouldClose()) {
    std::chrono::microseconds maxFrameTime{0};
    std::chrono::microseconds minFrameTime{9999999};
    std::chrono::microseconds totalFrameTime{0};

    for (int i = 0; i < 1000 && !subsystems.window()->shouldClose(); i++) {
      if (hasPendingScene_.load(std::memory_order_acquire)) {
        currentScene_ = pendingScene_;
        hasPendingScene_.store(false, std::memory_order_relaxed);
        currentScene_->activate();
      }

      auto start = std::chrono::high_resolution_clock::now();
      glfwPollEvents();
      update(prevFrameTime);
      drawFrame();
      auto end = std::chrono::high_resolution_clock::now();

      const auto curFrameTime =
          std::chrono::duration_cast<std::chrono::microseconds>(end - start);
      totalFrameTime += curFrameTime;
      maxFrameTime = std::max(maxFrameTime, curFrameTime);
      minFrameTime = std::min(minFrameTime, curFrameTime);

      prevFrameTime = curFrameTime;
    }

    std::cout
        << "FPS: "
        << 1000000.0f / static_cast<float>((totalFrameTime / 1000).count())
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

    // Create transition scene
    loadingScene_ = loadSceneFromDefinition(loadingSceneDefinition_);

    // Switch to transition scene
    pendingScene_ = loadingScene_.get();
    hasPendingScene_.store(true, std::memory_order_release);

    while (hasPendingScene_.load(std::memory_order_relaxed)) {
    }

    mainScene_.reset();

    auto loadStart = std::chrono::high_resolution_clock::now();

    mainScene_ = loadSceneFromName(sceneName);

    auto loadEnd = std::chrono::high_resolution_clock::now();
    auto loadTime = std::chrono::duration_cast<std::chrono::milliseconds>(
        loadEnd - loadStart);
    log::LoggerSystem::logToDefault(
        log::LogLevel::INFO,
        util::toString(
            "Loaded scene ", sceneName, " in ", loadTime.count(), "ms"));

    if (loadTime < kMinLoadTime) {
      std::this_thread::sleep_for(kMinLoadTime - loadTime);
    }

    pendingScene_ = mainScene_.get();
    hasPendingScene_.store(true, std::memory_order_release);

    while (hasPendingScene_.load(std::memory_order_relaxed)) {
    }

    // Cleanup transition scene
    loadingScene_.reset();
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

// NOLINTNEXTLINE(readability-convert-member-functions-to-static)
void Application::close() {
  GlobalSubSystemStack::get().window()->close();
}

} // namespace blocks
