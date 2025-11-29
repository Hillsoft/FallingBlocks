#pragma once

#include <vulkan/vulkan_core.h>
#include "render/vulkan/UniqueHandle.hpp"

namespace blocks::render::vulkan {

class SemaphoreBuilder {
 public:
  explicit SemaphoreBuilder();

  UniqueHandle<VkSemaphore> build(VkDevice device) const;

 private:
  VkSemaphoreCreateInfo createInfo_;
};

} // namespace blocks::render::vulkan
