#pragma once

#include "engine/ResourceSentinelManager.hpp"
#include "input/InputSubSystem.hpp"
#include "render/RenderSubSystem.hpp"
#include "util/raii_helpers.hpp"

namespace blocks {

class GlobalSubSystemStack : private util::no_copy_move {
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
  ResourceSentinelManager& sentinelManager();

 private:
  render::RenderSubSystem render_;
  render::UniqueWindowHandle window_;
  input::InputSubSystem input_;
  ResourceSentinelManager sentinelManager_;
};

} // namespace blocks
