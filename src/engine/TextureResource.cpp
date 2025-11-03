#include "engine/TextureResource.hpp"

#include <string>
#include "GlobalSubSystemStack.hpp"
#include "render/renderables/RenderableTex2D.hpp"
#include "util/string.hpp"

namespace blocks::engine {

TextureResource::TextureResource(const std::string& path)
    : renderable_(
          GlobalSubSystemStack::get()
              .renderSystem()
              .createRenderable<render::RenderableTex2D>(
                  util::toString(RESOURCE_DIR "/", path))) {}

} // namespace blocks::engine
