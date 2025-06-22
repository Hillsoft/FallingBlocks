#pragma once

#include "input/InputHandler.hpp"
#include "input/InputSubSystem.hpp"
#include "math/vec.hpp"
#include "render/RenderSubSystem.hpp"

namespace blocks {

class Application {
 public:
  Application();
  ~Application() = default;

  Application(const Application& other) = delete;
  Application(Application&& other) = delete;

  Application& operator=(const Application& other) = delete;
  Application& operator=(Application&& other) = delete;

  void run();

 private:
  class InputHandler : public input::InputHandler {
   public:
    InputHandler(input::InputSubSystem& inputSystem, Application& app)
        : input::InputHandler(inputSystem), app_(&app) {}

    void onKeyPress(int key) final;

   private:
    Application* app_;
  };

  void update(float deltaTimeSeconds);
  void drawFrame();

  render::RenderSubSystem render_;
  render::UniqueWindowHandle window_;
  input::InputSubSystem input_;

  render::UniqueRenderableHandle renderable_;
  render::UniqueRenderableHandle renderable2_;

  math::Vec2 pos1_;
  math::Vec2 v1_;
  math::Vec2 pos2_;
  math::Vec2 v2_;

  InputHandler inputHandler_;
};

} // namespace blocks
