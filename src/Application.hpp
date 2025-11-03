#pragma once

#include <atomic>
#include <chrono>
#include <memory>
#include <string>
#include <thread>
#include "engine/Scene.hpp"
#include "input/InputHandler.hpp"

namespace blocks {

class Application : public input::InputHandler {
 public:
  Application();
  ~Application();

  static Application& getApplication();

  void run();

  void transitionToScene(std::string sceneName);

  void onKeyPress(int key) final;

  void close();

 private:
  void update(std::chrono::microseconds deltaTimeSeconds);
  void drawFrame();

  Scene loadingScene_;
  std::unique_ptr<Scene> mainScene_;
  Scene* currentScene_;
  Scene* pendingScene_ = nullptr;
  std::atomic<bool> hasPendingScene_ = false;
  std::jthread loadThread_;
};

} // namespace blocks
