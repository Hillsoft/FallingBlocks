#include "engine/ColorRenderableResource.hpp"

#include "GlobalSubSystemStack.hpp"
#include "render/renderables/RenderableColor2D.hpp"

namespace blocks {

ColorRenderableResource::ColorRenderableResource()
    : renderable_(GlobalSubSystemStack::get()
                      .renderSystem()
                      .createRenderable<render::RenderableColor2D>()) {}

} // namespace blocks
