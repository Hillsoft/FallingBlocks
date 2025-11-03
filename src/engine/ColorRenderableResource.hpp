#pragma once

#include "render/RenderSubSystem.hpp"
#include "render/renderables/RenderableColor2D.hpp"
#include "util/meta_utils.hpp"

namespace blocks {

class ColorRenderableResource {
 public:
  using Fields = util::TArray<>;

  ColorRenderableResource();

  render::RenderableRef<render::RenderableColor2D::InstanceData> get() {
    return renderable_.get();
  }

 private:
  render::UniqueRenderableHandle<render::RenderableColor2D::InstanceData>
      renderable_;
};

} // namespace blocks
