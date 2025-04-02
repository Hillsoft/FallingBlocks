#pragma once

#include <cstdint>
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
  void drawFrame();

  RenderSubSystem render_;
  UniqueWindowHandle window_;
  UniqueRenderableHandle renderable_;
  UniqueRenderableHandle renderable2_;

  bool shouldDraw_ = true;
};

} // namespace blocks::render
