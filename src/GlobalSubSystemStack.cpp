#include "GlobalSubSystemStack.hpp"

#include "engine/ResourceSentinelManager.hpp"
#include "input/InputSubSystem.hpp"
#include "render/RenderSubSystem.hpp"
#include "util/debug.hpp"

namespace blocks {

namespace {

GlobalSubSystemStack* globalStack = nullptr;

} // namespace

GlobalSubSystemStack::GlobalSubSystemStack()
    : render_(),
      window_(render_.createWindow()),
      input_(window_->getPresentStack().getWindow()) {
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

render::RenderSubSystem& GlobalSubSystemStack::renderSystem() {
  return render_;
}

render::WindowRef GlobalSubSystemStack::window() {
  return window_.get();
}

input::InputSubSystem& GlobalSubSystemStack::inputSystem() {
  return input_;
}

ResourceSentinelManager& GlobalSubSystemStack::sentinelManager() {
  return sentinelManager_;
}

} // namespace blocks
