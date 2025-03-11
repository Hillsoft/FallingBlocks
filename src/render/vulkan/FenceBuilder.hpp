#pragma once

#include <GLFW/glfw3.h>
#include "render/vulkan/UniqueHandle.hpp"

namespace blocks::render::vulkan {

class FenceBuilder {
 public:
  explicit FenceBuilder();

  FenceBuilder& setInitiallySignalled(bool initiallySignalled);

  UniqueHandle<VkFence> build(VkDevice device) const;

 private:
  VkFenceCreateInfo createInfo_;
};

} // namespace blocks::render::vulkan
