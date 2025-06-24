#pragma once

#include "input/InputSubSystem.hpp"
#include "physics/PhysicsScene.hpp"
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

  physics::PhysicsScene& physicsScene();
  render::RenderSubSystem& renderSystem();
  render::WindowRef window();
  input::InputSubSystem& inputSystem();

 private:
  physics::PhysicsScene physics_;
  render::RenderSubSystem render_;
  render::UniqueWindowHandle window_;
  input::InputSubSystem input_;
};

} // namespace blocks
