#include "engine/FontResource.hpp"

#include <string>
#include "GlobalSubSystemStack.hpp"
#include "loader/font/Font.hpp"
#include "util/string.hpp"

namespace blocks {

FontResource::FontResource(const std::string& path)
    : font_(
          GlobalSubSystemStack::get().renderSystem(),
          loader::loadFont(util::toString(RESOURCE_DIR "/", path))) {}

} // namespace blocks
