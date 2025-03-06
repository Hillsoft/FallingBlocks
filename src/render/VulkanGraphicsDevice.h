#pragma once

#include <GLFW/glfw3.h>

#include "render/VulkanInstance.h"

namespace tetris::render {

class VulkanGraphicsDevice {
 public:
  ~VulkanGraphicsDevice();

  VulkanGraphicsDevice(const VulkanGraphicsDevice& other) = delete;
  VulkanGraphicsDevice& operator=(const VulkanGraphicsDevice& other) = delete;

  VulkanGraphicsDevice(VulkanGraphicsDevice&& other) noexcept;
  VulkanGraphicsDevice& operator=(VulkanGraphicsDevice&& other);

  static VulkanGraphicsDevice make(VulkanInstance& instance);

 private:
  VulkanGraphicsDevice(VkDevice device);

  void cleanup();

  VkDevice device_;
};

} // namespace tetris::render
