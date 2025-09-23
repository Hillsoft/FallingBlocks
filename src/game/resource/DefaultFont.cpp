#include "game/resource/DefaultFont.hpp"

#include <optional>
#include "GlobalSubSystemStack.hpp"
#include "loader/font/Font.hpp"
#include "render/Font.hpp"
#include "util/debug.hpp"

namespace blocks::game {

namespace {

class DefaultFontResourceSentinelData {
 public:
  DefaultFontResourceSentinelData()
      : font_(
            GlobalSubSystemStack::get().renderSystem(),
            loader::loadFont(RESOURCE_DIR "/Deng.ttf")) {}

  render::Font& getFont() { return font_; }

 private:
  render::Font font_;
};

constinit std::optional<DefaultFontResourceSentinelData> resourceSentinel;

} // namespace

void DefaultFontResourceSentinel::load() {
  DEBUG_ASSERT(!resourceSentinel.has_value());
  resourceSentinel.emplace();
}

void DefaultFontResourceSentinel::unload() {
  resourceSentinel.reset();
}

render::Font& DefaultFontResourceSentinel::get() {
  return resourceSentinel->getFont();
}

} // namespace blocks::game
