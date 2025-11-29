#pragma once

#include <string>
#include "render/RenderSubSystem.hpp"
#include "render/renderables/RenderableTex2D.hpp"
#include "util/meta_utils.hpp"

namespace blocks::engine {

class TextureResource {
 public:
  using Fields = util::TArray<util::TPair<util::TString<"path">, std::string>>;

  explicit TextureResource(const std::string& path);

  render::RenderableRef<render::RenderableTex2D::InstanceData> get() {
    return renderable_.get();
  }

 private:
  render::UniqueRenderableHandle<render::RenderableTex2D::InstanceData>
      renderable_;
};

} // namespace blocks::engine
