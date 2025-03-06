#pragma once

#include <GLFW/glfw3.h>

#include "render/VulkanInstance.hpp"
#include "render/VulkanSurface.hpp"

namespace tetris::render {

class VulkanGraphicsDevice {
 public:
  ~VulkanGraphicsDevice();

  VulkanGraphicsDevice(const VulkanGraphicsDevice& other) = delete;
  VulkanGraphicsDevice& operator=(const VulkanGraphicsDevice& other) = delete;

  VulkanGraphicsDevice(VulkanGraphicsDevice&& other) noexcept;
  VulkanGraphicsDevice& operator=(VulkanGraphicsDevice&& other) noexcept;

  static VulkanGraphicsDevice make(
      VulkanInstance& instance, VulkanSurface& surface);

 private:
  VulkanGraphicsDevice(
      VkDevice device, VkQueue graphicsQueue, VkQueue presentQueue);

  void cleanup();

  VkDevice device_;
  VkQueue graphicsQueue_;
  VkQueue presentQueue_;
};

} // namespace tetris::render
