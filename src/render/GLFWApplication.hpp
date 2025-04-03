#pragma once

#include "render/RenderSubSystem.hpp"

namespace blocks::render {

class GLFWApplication {
 public:
  GLFWApplication();
  ~GLFWApplication() = default;

  GLFWApplication(const GLFWApplication& other) = delete;
  GLFWApplication(GLFWApplication&& other) = delete;

  GLFWApplication& operator=(const GLFWApplication& other) = delete;
  GLFWApplication& operator=(GLFWApplication&& other) = delete;

  void run();

 private:
  void update();
  void drawFrame();

  RenderSubSystem render_;
  UniqueWindowHandle window_;
  UniqueWindowHandle window2_;
  UniqueRenderableHandle renderable_;
  UniqueRenderableHandle renderable2_;

  size_t timeMs_ = 0;
  bool shouldDraw_ = true;
};

} // namespace blocks::render
