#pragma once

#include <GLFW/glfw3.h>

#include "render/VulkanInstance.hpp"

namespace tetris::render {

class VulkanDebugMessenger {
 public:
  VulkanDebugMessenger(VulkanInstance& instance);
  ~VulkanDebugMessenger();

  VulkanDebugMessenger(const VulkanDebugMessenger& other) = delete;
  VulkanDebugMessenger& operator=(const VulkanDebugMessenger& other) = delete;

  VulkanDebugMessenger(VulkanDebugMessenger&& other) noexcept;
  VulkanDebugMessenger& operator=(VulkanDebugMessenger&& other) noexcept;

 private:
  VulkanInstance* instance_;
  VkDebugUtilsMessengerEXT debugMessenger_;
};

} // namespace tetris::render
