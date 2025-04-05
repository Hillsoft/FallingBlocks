#pragma once

#include "math/vec.hpp"
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
  void update(float deltaTimeSeconds);
  void drawFrame();

  RenderSubSystem render_;
  UniqueWindowHandle window_;
  UniqueRenderableHandle renderable_;
  UniqueRenderableHandle renderable2_;

  math::Vec2 pos1_;
  math::Vec2 v1_;
  math::Vec2 pos2_;
  math::Vec2 v2_;
};

} // namespace blocks::render
