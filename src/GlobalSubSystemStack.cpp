#include "GlobalSubSystemStack.hpp"

#include <memory>
#include <string>
#include <string_view>
#include "audio/AudioSubSystem.hpp"
#include "engine/Localisation.hpp"
#include "engine/ResourceManager.hpp"
#include "engine/Settings.hpp"
#include "input/InputSubSystem.hpp"
#include "log/Logger.hpp"
#include "log/StdoutLoggerBackend.hpp"
#include "render/RenderSubSystem.hpp"
#include "util/debug.hpp"

namespace blocks {

namespace {

// NOLINTNEXTLINE(cppcoreguidelines-avoid-non-const-global-variables)
GlobalSubSystemStack* globalStack = nullptr;

std::unique_ptr<log::LoggerSystem> buildLogger() {
  std::unique_ptr<log::LoggerSystem> logger =
      std::make_unique<log::LoggerSystem>();
  logger->registerBackend(std::make_unique<log::StdoutLoggerBackend>());

  log::LoggerSystem::setDefaultLoggerSystem(logger.get());

  return logger;
}

std::string_view getLocaleCodeFromSettings() {
  return getSettings().localeCode;
}

} // namespace

GlobalSubSystemStack::GlobalSubSystemStack()
    : logger_(buildLogger()),
      window_(render_.createWindow()),
      input_(window_->getPresentStack().getWindow()),
      localisation_(std::string{getLocaleCodeFromSettings()}) {
  DEBUG_ASSERT(globalStack == nullptr);
  globalStack = this;
}

GlobalSubSystemStack::~GlobalSubSystemStack() {
  log::LoggerSystem::setDefaultLoggerSystem(nullptr);

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

engine::ResourceManager& GlobalSubSystemStack::resourceManager() {
  return resourceManager_;
}

Localisation& GlobalSubSystemStack::localisationManager() {
  return localisation_;
}

} // namespace blocks
