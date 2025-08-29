#pragma once

#include <atomic>
#include <memory>
#include <thread>
#include "engine/Scene.hpp"
#include "engine/SceneLoader.hpp"
#include "input/InputHandler.hpp"

namespace blocks {

class Application : public input::InputHandler {
 public:
  Application();
  ~Application();

  static Application& getApplication();

  void run();

  void transitionToScene(std::unique_ptr<SceneLoader> sceneLoader);

  void onKeyPress(int key) final;

 private:
  void update(float deltaTimeSeconds);
  void drawFrame();

  Scene loadingScene_;
  std::unique_ptr<Scene> mainScene_;
  Scene* currentScene_;
  Scene* pendingScene_ = nullptr;
  std::atomic<bool> hasPendingScene_ = false;
  std::jthread loadThread_;
};

} // namespace blocks
