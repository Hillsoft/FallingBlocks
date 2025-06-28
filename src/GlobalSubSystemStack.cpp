#include "GlobalSubSystemStack.hpp"

#include "engine/DrawableRegistry.hpp"
#include "engine/TickRegistry.hpp"
#include "input/InputSubSystem.hpp"
#include "physics/PhysicsScene.hpp"
#include "render/RenderSubSystem.hpp"
#include "util/debug.hpp"

namespace blocks {

namespace {

GlobalSubSystemStack* globalStack = nullptr;

} // namespace

GlobalSubSystemStack::GlobalSubSystemStack()
    : physics_(),
      render_(),
      window_(render_.createWindow()),
      input_(window_->getPresentStack().getWindow()),
      tick_(),
      drawableScene_() {
  DEBUG_ASSERT(globalStack == nullptr);
  globalStack = this;
}

GlobalSubSystemStack::~GlobalSubSystemStack() {
  DEBUG_ASSERT(globalStack == this);
  globalStack = nullptr;
}

GlobalSubSystemStack& GlobalSubSystemStack::get() {
  DEBUG_ASSERT(globalStack != nullptr);
  return *globalStack;
}

physics::PhysicsScene& GlobalSubSystemStack::physicsScene() {
  return physics_;
}

render::RenderSubSystem& GlobalSubSystemStack::renderSystem() {
  return render_;
}

render::WindowRef GlobalSubSystemStack::window() {
  return window_.get();
}

input::InputSubSystem& GlobalSubSystemStack::inputSystem() {
  return input_;
}

TickRegistry& GlobalSubSystemStack::tickSystem() {
  return tick_;
}

DrawableRegistry& GlobalSubSystemStack::drawableScene() {
  return drawableScene_;
}

} // namespace blocks
