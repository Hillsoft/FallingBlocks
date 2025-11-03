#pragma once

#include <atomic>
#include <chrono>
#include <memory>
#include <string>
#include <thread>
#include "engine/ResourceRef.hpp"
#include "engine/Scene.hpp"
#include "engine/SceneLoader.hpp"
#include "input/InputHandler.hpp"

namespace blocks {

class Application : public input::InputHandler {
 public:
  Application(
      std::string loadingSceneResourceName,
      std::string initialSceneResourceName);
  ~Application();

  static Application& getApplication();

  void run();

  void transitionToScene(std::string sceneName);

  void onKeyPress(int key) final;

  void close();

 private:
  void update(std::chrono::microseconds deltaTimeSeconds);
  void drawFrame();

  engine::ResourceRef<SceneDefinition> loadingSceneDefinition_;
  std::unique_ptr<Scene> loadingScene_;
  std::unique_ptr<Scene> mainScene_;
  Scene* currentScene_;
  Scene* pendingScene_ = nullptr;
  std::atomic<bool> hasPendingScene_ = false;
  std::string initialSceneResourceName_;
  std::jthread loadThread_;
};

} // namespace blocks
