#include "GlobalSubSystemStack.hpp"

#include "audio/AudioSubSystem.hpp"
#include "engine/Localisation.hpp"
#include "engine/ResourceManager.hpp"
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
      input_(window_->getPresentStack().getWindow()),
      localisation_("en_GB") {
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

audio::AudioSubSystem& GlobalSubSystemStack::audioSystem() {
  return audio_;
}

ResourceSentinelManager& GlobalSubSystemStack::sentinelManager() {
  return sentinelManager_;
}

engine::ResourceManager& GlobalSubSystemStack::resourceManager() {
  return resourceManager_;
}

Localisation& GlobalSubSystemStack::localisationManager() {
  return localisation_;
}

} // namespace blocks
