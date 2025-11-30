#pragma once

#include "audio/AudioSubSystem.hpp"
#include "engine/Localisation.hpp"
#include "engine/ResourceManager.hpp"
#include "input/InputSubSystem.hpp"
#include "log/Logger.hpp"
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
  audio::AudioSubSystem& audioSystem();
  engine::ResourceManager& resourceManager();
  Localisation& localisationManager();

 private:
  log::LoggerSystem logger_;
  render::RenderSubSystem render_;
  render::UniqueWindowHandle window_;
  input::InputSubSystem input_;
  audio::AudioSubSystem audio_;
  engine::ResourceManager resourceManager_;
  Localisation localisation_;
};

} // namespace blocks
