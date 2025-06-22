#pragma once

#include "input/InputSubSystem.hpp"
#include "render/RenderSubSystem.hpp"

namespace blocks {

class GlobalSubSystemStack {
 public:
  GlobalSubSystemStack();

  GlobalSubSystemStack(const GlobalSubSystemStack& other) = delete;
  GlobalSubSystemStack& operator=(const GlobalSubSystemStack& other) = delete;
  GlobalSubSystemStack(GlobalSubSystemStack&& other) = delete;
  GlobalSubSystemStack& operator=(GlobalSubSystemStack&& other) = delete;

  ~GlobalSubSystemStack();

  static GlobalSubSystemStack& get();

  render::RenderSubSystem& renderSystem();
  render::WindowRef window();
  input::InputSubSystem& inputSystem();

 private:
  render::RenderSubSystem render_;
  render::UniqueWindowHandle window_;
  input::InputSubSystem input_;
};

} // namespace blocks
